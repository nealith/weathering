#include "./includes/AverageSumError.h"
#include "./includes/FullBlocksGenerator.h"
#include "./includes/BorderKernelBlur.h"
#include "./includes/RandomBlocksGenerator.h"
#include "./includes/ClassicPositionChooser.h"
#include "./includes/SeamCarvingMaskGenerator.h"
#include "./includes/TextureGenerator.h"
#include "./includes/DiffSumSqrtAtIJ.h"
#include "./includes/TopTenBlockSelector.h"
#include "./includes/EpsilonBlockSelector.h"
#include "./includes/SmoothMask.h"

int main(int argc, const char ** argv) {

  String inputPath( "../textures/ground9.jpg" ); // by default

  if( argc > 1)
  {
      inputPath = argv[1];
  }
  Mat input = imread( inputPath, IMREAD_COLOR );

  if( input.empty() )                      // Check for invalid input
  {
      cout <<  "Could not open or find the image : "<< inputPath << std::endl ;
      return -1;
  }

  namedWindow("Display window", WINDOW_AUTOSIZE);
  imshow("Display window", input);
  waitKey(0);

  // Attention, le retour de TextureGenerator() c'est du 8UC4, pas du 8UC3 !


  Mat output(1024, 1024, CV_8UC4);
  // Parms par défaut : bw:32, bh:32, libsize:64, overlapsize:8; epsilon:0.2
  bool redraw = true;

  quilting::DiffSumSqrtAtIJ * diff = new quilting::DiffSumSqrtAtIJ();
  quilting::AverageSumError * ase = new quilting::AverageSumError(diff);


  quilting::RandomBlocksGenerator * g = new quilting::RandomBlocksGenerator(input,64,64,128);
  quilting::TopTenBlockSelector * s = new quilting::TopTenBlockSelector(g,ase, redraw);

  //quilting::FullBlocksGenerator * g = new quilting::FullBlocksGenerator(input, 64, 64);
  //quilting::EpsilonBlockSelector * s = new quilting::EpsilonBlockSelector(g, ase, 0.2);

  quilting::ClassicPositionChooser * p = new quilting::ClassicPositionChooser();

  quilting::ErrorMapImg1Img2 * e = new quilting::ErrorMapImg1Img2(diff);
  quilting::SeamCarvingMaskGenerator * m = new quilting::SeamCarvingMaskGenerator(e);

  quilting::TextureGenerator tg = quilting::TextureGenerator(p,s,m,16);
  quilting::BorderPostTreatement * bpt = new quilting::BorderKernelBlur(5);
  quilting::SmoothMask * sm = new quilting::SmoothMask(0.1,16);


  //tg.setBorderPostTreatement(bpt);
  tg.setMaskPostTreatement(sm);

  TickMeter tm;
  tm.start();
  tg(output);
  tm.stop();
  cout << "Time : " << tm.getTimeSec() << " sec" << endl;

  imshow("Display window", output);
  waitKey(0);

  imwrite("./output.jpg", output);

  return 0;
}
