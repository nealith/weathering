#include "./includes/AverageSumError.h"
#include "./includes/FullBlocksGenerator.h"
#include "./includes/BorderKernelBlur.h"
#include "./includes/RandomBlocksGenerator.h"
#include "./includes/ClassicPositionChooser.h"
#include "./includes/SeamCarvingMaskGenerator.h"
#include "./includes/DiffSumSqrtAtIJ.h"
#include "./includes/TopTenBlockSelector.h"
#include "./includes/EpsilonBlockSelector.h"
#include "./includes/SmoothMask.h"
#include "./includes/TextureTransfert.h"
#include "./includes/NormalizedInvertedErrorMask.h"

int main(int argc, const char ** argv) {

  String texturePath( "grass.jpg" ); // by default

  Mat texture = imread( texturePath, IMREAD_COLOR );

  if( texture.empty() )                      // Check for invalid input
  {
      cout <<  "Could not open or find the image : "<< texturePath << std::endl ;
      return -1;
  }

  String outputPath( "ferrari.png" ); // by default

  Mat output = imread( outputPath, IMREAD_UNCHANGED );

  if( output.empty() )                      // Check for invalid input
  {
      cout <<  "Could not open or find the image : "<< outputPath << std::endl ;
      return -1;
  }

  if (output.channels() != 4) {
    cv::cvtColor(output, output,cv::COLOR_BGR2BGRA,4);
  }

  namedWindow("Display window", WINDOW_AUTOSIZE);
  imshow("Texture", texture);
  imshow("Output", output);
  waitKey(0);

  // Parms par défaut : bw:32, bh:32, libsize:64, overlapsize:8; epsilon:0.2
  bool redraw = true;

  quilting::DiffSumSqrtAtIJ * diff = new quilting::DiffSumSqrtAtIJ();
  quilting::AverageSumError * ase = new quilting::AverageSumError(diff);

  quilting::RandomBlocksGenerator * g = new quilting::RandomBlocksGenerator(texture,32,32,128);
  quilting::TopTenBlockSelector * s = new quilting::TopTenBlockSelector(g,ase, redraw);

  //quilting::FullBlocksGenerator * g = new quilting::FullBlocksGenerator(texture, 64, 64);
  //quilting::EpsilonBlockSelector * s = new quilting::EpsilonBlockSelector(g, ase, 0.2);

  quilting::ClassicPositionChooser * p = new quilting::ClassicPositionChooser();

  quilting::ErrorMapImg1Img2 * e = new quilting::ErrorMapImg1Img2(diff);
  quilting::NormalizedInvertedErrorMask * m = new quilting::NormalizedInvertedErrorMask(e,Range(5000,5000));

  quilting::TextureTransfert tg = quilting::TextureTransfert(p,s,m,0);
  //quilting::BorderPostTreatement * bpt = new quilting::BorderKernelBlur(5);
  //quilting::SmoothMask * sm = new quilting::SmoothMask(0.1,16);


  //tg.setBorderPostTreatement(bpt);
  //tg.setMaskPostTreatement(sm);

  TickMeter tm;
  tm.start();
  tg(output,
    [texture,output](const Mat & in, Mat & out){

      if (in.data == texture.data) {
        out = Mat(in.rows,in.cols,CV_8UC3);
        for (unsigned int i = 0; i < in.rows; i++) {
          for (unsigned int j = 0; j < in.cols; j++) {
              out.ptr<Vec3b>(i)[j].val[0] = fmin(in.ptr<Vec3b>(i)[j].val[1]+50,255);
              out.ptr<Vec3b>(i)[j].val[1] = fmin(in.ptr<Vec3b>(i)[j].val[1]+50,255);
              out.ptr<Vec3b>(i)[j].val[2] = fmin(in.ptr<Vec3b>(i)[j].val[1]+50,255);
          }
        }
        imwrite("./textureMM.jpg", out);
      } else if(in.data == output.data){
        out = Mat(in.rows,in.cols,CV_8UC4);
        for (unsigned int i = 0; i < in.rows; i++) {
          for (unsigned int j = 0; j < in.cols; j++) {

              if (in.ptr<Vec4b>(i)[j].val[2] > 60 && in.ptr<Vec4b>(i)[j].val[0] < 40 && in.ptr<Vec4b>(i)[j].val[1] < 40) {
                out.ptr<Vec4b>(i)[j].val[0] = in.ptr<Vec4b>(i)[j].val[2];
                out.ptr<Vec4b>(i)[j].val[1] = in.ptr<Vec4b>(i)[j].val[2];
                out.ptr<Vec4b>(i)[j].val[2] = in.ptr<Vec4b>(i)[j].val[2];
                out.ptr<Vec4b>(i)[j].val[3] = 255;
              } else {
                out.ptr<Vec4b>(i)[j].val[0] = 0;
                out.ptr<Vec4b>(i)[j].val[1] = 0;
                out.ptr<Vec4b>(i)[j].val[2] = 0;
                out.ptr<Vec4b>(i)[j].val[3] = 255;
              }

          }
        }
        imwrite("./imageMM.jpg", out);
      }

      imshow("Display matching", out);
      waitKey(0);


    }
  );
  tm.stop();
  cout << "Time : " << tm.getTimeSec() << " sec" << endl;

  imshow("Display window", output);
  waitKey(0);

  imwrite("./transfert.jpg", output);

  return 0;
}
