#include "ImageWeathering.h"

#include "nlopt.hpp"

/***********************************************
 *  ImageWeathering
 ***********************************************/

weathering::ImageWeathering::ImageWeathering()
{}

weathering::ImageWeathering::~ImageWeathering()
{}

void weathering::ImageWeathering::operator()(Mat & input,std::list<std::pair<float,Mat *>> & output, std::list<std::pair<float,Mat *>> * weatheringMaps, Mat * shadowMap)
{

  Mat * user_input_grabcut = grabCut(input);

  //imshow("user_input", user_input_grabcut[0]);
  //imshow("user_input_background", user_input_grabcut[1]);
  //imshow("user_input_foreground", user_input_grabcut[2]);
  imshow("user_input_foreground", user_input_grabcut[3]);

  waitKey(0);

    Rect2d * user_input = userInput(user_input_grabcut[3]);

    Mat user_input_grabcut_lab;
    cvtColor(user_input_grabcut[3], user_input_grabcut_lab, CV_BGR2Lab);


  Mat degree_map = computeWeatheringDegreeMap(user_input_grabcut_lab,user_input_grabcut[0],user_input);

  Mat degree_map_uchar(degree_map.rows, degree_map.cols, CV_8UC1);

  for (unsigned int i = 0; i < degree_map.rows; i++) {
    double * ptr_dm = degree_map.ptr<double>(i);
    unsigned char * ptr_dmu = degree_map_uchar.ptr<unsigned char>(i);
    for (unsigned int j = 0; j < degree_map.cols; j++) {
      ptr_dmu[j] = floor(255.0 * ptr_dm[j]);
    }
  }

  imshow("degree_map", degree_map_uchar);



  //Mat di = initDi(input,user_input[0])
  waitKey(0);


  delete[] user_input;

}

Rect2d * weathering::ImageWeathering::userInput(const Mat & input)
{
  Rect2d * user_input = new Rect2d[2];

  std::cout << "Select most weathered area (only most weathered pixel)" << '\n';
  user_input[0] = selectROI(input);


  std::cout << "Select least weathered area (only least weathered pixel)" << '\n';
  user_input[1] = selectROI(input);

  return user_input;
}

Mat * weathering::ImageWeathering::grabCut(const Mat & input)
{
  Mat * user_input = new Mat[4];


  Rect2d r = selectROI(input);

  cv::grabCut(input,user_input[0],r, user_input[1], user_input[2], 1, cv::GC_INIT_WITH_RECT);
  cv::compare(user_input[0],cv::GC_PR_FGD,user_input[0],cv::CMP_EQ);

  user_input[3] = Mat(input.size(),CV_8UC3,cv::Scalar(255,255,255));
  input.copyTo(user_input[3],user_input[0]);

  return user_input;
}

double weathering::ImageWeathering::computeWd(const Mat & img,int i, int j,const vector<wdi> & omega)
{

  double * fvecij = featureVector(img,i,j);
  double d = 0;
  for (auto it = omega.begin(); it != omega.end(); it++) {
    double * fvecomegaij = featureVector(img,it->i,it->j);
    d += it->a * rbf(fvecij,fvecomegaij);

    delete[] fvecomegaij;
  }

  delete[] fvecij;
  return d;
}


double weathering::objective_function(const std::vector<double> &x, std::vector<double> &grad, void* f_data)
{


  objective_function_data * data = (objective_function_data*) f_data;

  double E = 0;
  double gradi = 0;

  for (unsigned int i = 0; i < x.size(); i++) {
    (*(data->omega))[i].a = x[i];
  }

  double tmp(0);

  for (unsigned int i = 0; i < (*(data->omega)).size(); i++) {

    tmp = (*(data->omega))[i].di - ImageWeathering::computeWd((*(data->input)),(*(data->omega))[i].i,(*(data->omega))[i].j,(*(data->omega)));

    E += tmp*tmp;

    if (!grad.empty()) {
      double tmp_a((*(data->omega))[i].a);

      (*(data->omega))[i].a = 1.0;

      for (unsigned int j = 0; j < (*(data->omega)).size(); j++) {
        tmp = (*(data->omega))[j].di - ImageWeathering::computeWd((*(data->input)),(*(data->omega))[j].i,(*(data->omega))[j].j,(*(data->omega)));
        gradi += tmp*tmp;
      }

      (*(data->omega))[i].a = tmp_a;
      grad[i] = gradi;
      gradi = 0;
    }


  }

  return E;

}

Mat weathering::ImageWeathering::computeWeatheringDegreeMap(const Mat & user_input_grabcut, const Mat & mask_input_grabcut, Rect2d * user_input)
{

  Mat dm(user_input_grabcut.rows, user_input_grabcut.cols, CV_64FC1);

  std::cout << "Compute Weathering Degree Map ..." << '\n';

  std::cout << "-> prepare omega set ..." << '\n';

  vector<wdi> omega;

  for (unsigned int i = user_input[0].y; i < user_input[0].y + user_input[0].height; i++) {
    for (unsigned int j = user_input[0].x; j < user_input[0].x + user_input[0].width; j++) {
      wdi el;
      el.i = i;
      el.j = j;
      el.di = 1.0;
      el.a = 1.0;
      omega.push_back(el);
    }
  }

  for (unsigned int i = user_input[1].y; i < user_input[1].y + user_input[1].height; i++) {
    for (unsigned int j = user_input[1].x; j < user_input[1].x + user_input[1].width; j++) {
      wdi el;
      el.i = i;
      el.j = j;
      el.di = 0.01;
      el.a = 1.0;
      omega.push_back(el);
    }
  }



  std::cout << "-> size of omega = " << omega.size() << '\n';

  std::cout << "-> create solver COLYBA ..." << '\n';

  nlopt::opt solver((nlopt::algorithm)NLOPT_LN_COBYLA,(unsigned int) omega.size());

  std::cout << "-> set solver objective_function and stopping criteria ..." << '\n';

  vector<double> omega_alpha_min(omega.size(),0.0);
  vector<double> omega_alpha_max(omega.size(),1.0);

  objective_function_data data;
  data.omega = &omega;
  data.input = &user_input_grabcut;

  solver.set_min_objective( (nlopt::vfunc) objective_function , &data);
  solver.set_ftol_abs(0.0001);
  solver.set_lower_bounds(omega_alpha_min);
  solver.set_upper_bounds(omega_alpha_max);

  std::cout << "-> launch optimization ..." << '\n';

  vector<double> omega_alpha(omega.size(),1.0);

  for (size_t i = 0; i < omega.size(); i++) {
    omega_alpha[i] = omega[i].di;
  }

  double E(0);

  solver.optimize(omega_alpha,E);

  std::cout << "-> recover coefficients ..." << '\n';

  for (unsigned int i = 0; i < omega.size(); i++) {
    omega[i].a = omega_alpha[i];
  }

  std::cout << "-> compute degree of each pixel ..." << '\n';

  double max(0.0);

  for (unsigned int i = 0; i < user_input_grabcut.rows; i++) {
    double * ptr_dm = dm.ptr<double>(i);
    for (unsigned int j = 0; j < user_input_grabcut.cols; j++) {
      if (255 == mask_input_grabcut.ptr<unsigned char>(i)[j]) {
        ptr_dm[j] = computeWd(user_input_grabcut,i,j,omega);
        if (ptr_dm[j] > max) {
          max = ptr_dm[j];
        }
      } else {
        ptr_dm[j] = 0;
      }
    }
  }



  for (unsigned int i = 0; i < dm.rows; i++) {
    double * ptr_dm = dm.ptr<double>(i);
    for (unsigned int j = 0; j < dm.cols; j++) {
      ptr_dm[j] /= max;
    }
  }


  return dm;




}

double * weathering::ImageWeathering::featureVector(const Mat & img, int r, int c, double siga, double sigs)
{
  double * fVec = new double[5];

  Vec3b lab = img.ptr<Vec3b>(r)[c];

  double rn = (r * 2.0 / (double)img.rows) -1.0;
  double cn = (c * 2.0 / (double)img.cols) -1.0;

  fVec[0] = lab.val[0]/(255.0*siga);
  fVec[1] = lab.val[1]/(255.0*siga);
  fVec[2] = lab.val[2]/(255.0*siga);
  fVec[3] = cn/sigs;
  fVec[4] = rn/sigs;

  return fVec;

}

double weathering::ImageWeathering::rbf(const double * i,const double * j)
{

  double fVecij[5];

  fVecij[0] = i[0] - j[0] ;
  fVecij[1] = i[1] - j[1] ;
  fVecij[2] = i[2] - j[2] ;
  fVecij[3] = i[3] - j[3] ;
  fVecij[4] = i[4] - j[4] ;

  double norm = sqrt(fVecij[0]*fVecij[0]+fVecij[1]*fVecij[1]+fVecij[2]*fVecij[2]+fVecij[3]*fVecij[3]+fVecij[4]*fVecij[4]);
  return exp(-(norm*norm));


}


Mat weathering::ImageWeathering::computeShadowMap(Mat & userInput)
{

}

Mat weathering::ImageWeathering::computeWeatheringExemplar(Mat & userInput, Mat & weatheringDegreeMap)
{

}

Mat weathering::ImageWeathering::updateWeatheringDegreeMap(Mat & weatheringDegreeMap, float degree)
{

}

Mat weathering::ImageWeathering::computeWeatheringImage(Mat & weatheringDegreeMapUpdated, Mat & shadowMap, Mat & output)
{

}
