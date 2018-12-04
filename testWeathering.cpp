#include "ImageWeathering.h"

int main(int argc, const char ** argv) {

  Mat im;

  if( argc != 2 || !(im=imread(argv[1], IMREAD_UNCHANGED)).data)
      return -1;

  weathering::ImageWeathering iw;
  std::list<std::pair<float,Mat *>> m;
  iw(im,m);



  return 0;
}
