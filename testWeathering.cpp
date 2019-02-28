#include "ImageWeathering.h"

int main(int argc, const char ** argv) {

  Mat im;

  if( argc < 2 || !(im=imread(argv[1], IMREAD_UNCHANGED)).data)
      return -1;

  bool grabcut = true;
  if( argc > 2){
    grabcut = std::stoi(std::string(argv[2]).c_str());
  }

  weathering::ImageWeathering iw;
  std::vector<std::pair<unsigned int,Mat*>> m;
  std::vector<std::pair<unsigned int,Mat*>> uw;

  for (size_t i = 50; i < 201; i+=50) {
    m.push_back(std::pair<unsigned int,Mat *>(i,nullptr));
    uw.push_back(std::pair<unsigned int,Mat *>(i,nullptr));
  }

  iw(im,m,&uw,nullptr,grabcut);



  return 0;
}
