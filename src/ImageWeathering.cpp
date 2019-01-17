#include "ImageWeathering.h"

#include "nlopt.hpp"

#include <cmath>

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

    Rect2d * user_input = userInput(user_input_grabcut[3]);

    Mat user_input_grabcut_lab;
    cvtColor(user_input_grabcut[3], user_input_grabcut_lab, CV_BGR2Lab);


  Mat degree_map = computeWeatheringDegreeMap(user_input_grabcut_lab,user_input_grabcut[0],user_input);
  Mat segmentation = segment(degree_map,user_input_grabcut[0]);
  Mat shadow_map = computeShadowMap(user_input_grabcut_lab,user_input_grabcut[0],degree_map,segmentation);

  Mat degree_map_uchar(degree_map.rows, degree_map.cols, CV_8UC1);
  Mat shadow_map_uchar(shadow_map.rows, shadow_map.cols, CV_8UC1);

  for (unsigned int i = 0; i < user_input_grabcut_lab.rows; i++) {
    const double * ptr_dm = degree_map.ptr<double>(i);
    unsigned char * ptr_dmu = degree_map_uchar.ptr<unsigned char>(i);
    const double * ptr_sm = shadow_map.ptr<double>(i);
    unsigned char * ptr_smu = shadow_map_uchar.ptr<unsigned char>(i);
    for (unsigned int j = 0; j < user_input_grabcut_lab.cols; j++) {
      ptr_dmu[j] = floor(ptr_dm[j]*255.0);
      ptr_smu[j] = floor(ptr_sm[j]*255.0);
    }
  }

  imshow("degree_map", degree_map_uchar);
  imshow("shadow_map", shadow_map_uchar);
  imshow("segmentation", segmentation);


  std::cout << "compute exemplar ..." << '\n';
  Mat exemplar = computeWeatheringExemplar(input,degree_map);

  imshow("exemplar",exemplar);

  std::cout << "update degree map..." << '\n';
  Mat updated_degree_map = updateWeatheringDegreeMap(degree_map,segmentation,20);

  Mat updated_degree_map_uchar(degree_map.rows, degree_map.cols, CV_8UC1);

  for (unsigned int i = 0; i < user_input_grabcut_lab.rows; i++) {
    const double * ptr_udm = updated_degree_map.ptr<double>(i);
    unsigned char * ptr_udmu = updated_degree_map_uchar.ptr<unsigned char>(i);
    for (unsigned int j = 0; j < user_input_grabcut_lab.cols; j++) {
      ptr_udmu[j] = floor(ptr_udm[j]*255.0);
    }
  }

  std::cout << "finish" << '\n';

  imshow("updated_degree_map",updated_degree_map_uchar);

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


double weathering::objective_function_degree_map(const std::vector<double> &x, std::vector<double> &grad, void* f_data)
{


  objective_function_degree_map_data * data = (objective_function_degree_map_data*) f_data;

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

  std::cout << "-> create solver SLSQP ..." << '\n';

  nlopt::opt solver((nlopt::algorithm)NLOPT_LD_SLSQP,(unsigned int) omega.size()); // NLOPT_LN_COBYLA

  std::cout << "-> set solver objective_function_degree_map and stopping criteria ..." << '\n';

  vector<double> omega_alpha_min(omega.size(),0.0);
  vector<double> omega_alpha_max(omega.size(),1.0);

  objective_function_degree_map_data data;
  data.omega = &omega;
  data.input = &user_input_grabcut;

  solver.set_min_objective( (nlopt::vfunc) objective_function_degree_map , &data);
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


  if (max > 1.0) {
    for (unsigned int i = 0; i < dm.rows; i++) {
      double * ptr_dm = dm.ptr<double>(i);
      for (unsigned int j = 0; j < dm.cols; j++) {
        ptr_dm[j] /= max;
      }
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

double weathering::objective_function_segmentation(const std::vector<double> &x, std::vector<double> &grad, void* f_data)
{
  objective_function_segmentation_data * data = (objective_function_segmentation_data*) f_data;



  double E = 0;
  return E;
}

Mat weathering::ImageWeathering::segment(const Mat & degree_map, const Mat & mask_input_grabcut)
{

  Mat segmentation(degree_map.rows, degree_map.cols,CV_8UC1);


  for (unsigned int i = 0; i < degree_map.rows; i++) {
    const double * ptr_degree_map = degree_map.ptr<double>(i);
    const unsigned char * ptr_mask_input_grabcut = mask_input_grabcut.ptr<unsigned char>(i);
    unsigned char * ptr_segmentation = segmentation.ptr<unsigned char>(i);
    for (unsigned int j = 0; j < degree_map.cols; j++) {
      if (ptr_mask_input_grabcut[j] == 255) {
        if (ptr_degree_map[j] > 0.2) {
          ptr_segmentation[j] = 255;
        } else {
          ptr_segmentation[j] = 0;
        }
      } else {
        ptr_segmentation[j] = 128;
      }
    }
  }



  return segmentation;


}


Mat weathering::ImageWeathering::computeShadowMap(const Mat & user_input_grabcut, const Mat & mask_input_grabcut, const Mat & degree_map, const Mat & segmentation)
{

  list<wdi> vals;

  Mat shadow_map(user_input_grabcut.rows, user_input_grabcut.cols, CV_64FC1);

  for (unsigned int i = 0; i < user_input_grabcut.rows; i++) {
    const Vec3b * ptr_user_input_grabcut = user_input_grabcut.ptr<Vec3b>(i);
    const unsigned char * ptr_segmentation = segmentation.ptr<unsigned char>(i);
    const double * ptr_degree_map = degree_map.ptr<double>(i);
    for (unsigned int j = 0; j < user_input_grabcut.cols; j++) {

      if (ptr_segmentation[j] == 0) {
        wdi val;
        val.i = i;
        val.j = j;
        val.di = ptr_degree_map[j];
        val.a = ((double)ptr_user_input_grabcut[j].val[0])/255.0;
        vals.push_back(val);
      }

    }
  }

  std::vector<double> av(20,0.0);
  std::vector<std::pair<double,double>> intervals(20,pair<double,double>(0.0,0.0));

  vals.sort([](const wdi & a,const wdi & b) -> double{
    return a.di < b.di;
  });

  int nb(ceil(vals.size()/20.0));
  int l(0);
  int m(0);
  int n(0);
  int k(0);

  for (auto kt = vals.begin(); kt != vals.end(); kt++) {
    if(l == 0){
      intervals[m].first = kt->di;
      n = vals.size() - k;
    }

    av[m] += kt->a;

    if (l == nb - 1 || k == vals.size() - 1) {
      l = -1;
      intervals[m].second = kt->di;
      av[m] /= (double)min(nb,n);
      m++;
    }

    l++;
    k++;

  }

  for (size_t i = 0; i < av.size(); i++) {
    std::cout << "average : " << av[i] << "; interval  " << intervals[i].first << " : " << intervals[i].second << '\n';
  }



  for (unsigned int i = 0; i < user_input_grabcut.rows; i++) {
    const Vec3b * ptr_user_input_grabcut = user_input_grabcut.ptr<Vec3b>(i);
    const unsigned char * ptr_mask_input_grabcut = mask_input_grabcut.ptr<unsigned char>(i);
    const unsigned char * ptr_segmentation = segmentation.ptr<unsigned char>(i);
    const double * ptr_degree_map = degree_map.ptr<double>(i);
    double * ptr_shadow_map = shadow_map.ptr<double>(i);
    for (unsigned int j = 0; j < user_input_grabcut.cols; j++) {
      if (ptr_mask_input_grabcut[j] == 255) {
        double luminance = ((double)ptr_user_input_grabcut[j].val[0])/255.0;
        if (ptr_segmentation[j] == 0) {
          for (size_t k = 0; k < av.size(); k++) {
            if (ptr_degree_map[j] >= intervals[k].first && ptr_degree_map[j] <= intervals[k].second) {
              ptr_shadow_map[j] = luminance/av[k];
              break;
            }
          }

        } else {
          ptr_shadow_map[j] = luminance;
        }
      } else {
        ptr_shadow_map[j] = 0.0;
      }

    }
  }

  return shadow_map;

}

Mat patch_match(const Mat & to_fill, const Mat & to_fill_mask, const Mat & exemplar)
{

}

Mat weathering::ImageWeathering::computeWeatheringExemplar(Mat & user_input_grabcut, Mat & degree_map)
{

  int s(300);

  Mat tmp_exemplar;
  Mat exemplar;
  double max_sum(0.0);
  int ei, ej;

  for (unsigned int i = 0; i < user_input_grabcut.rows-s; i++) {
    for (unsigned int j = 0; j < user_input_grabcut.cols-s; j++) {
      double sum(0.0);
      for (unsigned int m = i; m < i+s; m++) {
        const double * ptr_degree_map = degree_map.ptr<double>(m);
        for (unsigned int n = j; n < j+s; n++) {
          sum+=ptr_degree_map[n];
        }
      }
      if (sum > max_sum) {
        ei = i;
        ej = j;
        max_sum = sum;
      }
    }
  }

  std::cout << "max_sum" << '\n';



  tmp_exemplar = user_input_grabcut(Range(ei,ei+s),Range(ej,ej+s));
  std::cout << "select a area with only wheathered pixel" << '\n';
  exemplar = tmp_exemplar(selectROI(tmp_exemplar));

  return exemplar;



}

Mat weathering::ImageWeathering::updateWeatheringDegreeMap(const Mat & degree_map, const Mat & segmentation, unsigned int degree)
{

  double ks = 0.25;
  double kw = 1.0;

  Mat propagation_map = degree_map.clone();
  Mat updated_degree_map = degree_map.clone();
  int count;

  while (degree != 0) {
    for (unsigned int i = 0; i < degree_map.rows; i++) {
      double * ptr_propagation_map = propagation_map.ptr<double>(i);
      for (unsigned int j = 0; j < degree_map.cols; j++) {
        count = 0;


        if (segmentation.ptr<unsigned char>(i)[j] == 0) {
          ptr_propagation_map[j] = 0.0;
          for (unsigned int in = fmax(0,i-1); in < fmin(degree_map.rows, i+1); in++) {
            for (unsigned int jn = fmax(0,j-1); jn < fmin(degree_map.rows, j+1); jn++) {
              ptr_propagation_map[j]+=degree_map.ptr<double>(in)[jn];
              count++;
            }
          }

          ptr_propagation_map[j] /= count;
        }


      }
    }

    for (unsigned int i = 0; i < degree_map.rows; i++) {
      const double * ptr_propagation_map = propagation_map.ptr<double>(i);
      double * ptr_updated_degree_map = updated_degree_map.ptr<double>(i);
      for (unsigned int j = 0; j < degree_map.cols; j++) {
        if (segmentation.ptr<unsigned char>(i)[j] == 0) {
          ptr_updated_degree_map[j] += (ptr_propagation_map[j] * ks*kw);
        }
      }
    }

    degree--;
  }

  return updated_degree_map;


}

Mat weathering::ImageWeathering::computeWeatheringImage(Mat & weatheringDegreeMapUpdated, Mat & shadowMap, Mat & output)
{

}
