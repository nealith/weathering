#include "AverageSumError.h"

/***********************************************
 *  AverageSumError
 ***********************************************/

quilting::AverageSumError::AverageSumError(ErrorAtIJ * errorAtIJ)
  : ErrorImg1Img2(errorAtIJ)
{}

double quilting::AverageSumError::operator()(const Mat & img1, const Mat & img2)
{
  assert(img1.cols == img2.cols && img1.rows == img2.rows);
  double error(0.0);
  int c(0);
  bool i1(false);
  bool i2(false);
  for(int i = 0; i < img1.rows; i++) {
      for(int j = 0; j < img1.cols; j++){
          i1 = ((img1.channels() == 4 && img1.ptr<Vec4b>(i)[j].val[3] == (unsigned char)255) || img1.channels() == 3);
          i2 = ((img2.channels() == 4 && img2.ptr<Vec4b>(i)[j].val[3] == (unsigned char)255) || img2.channels() == 3);

          if (i1 && i2) {
            error+= (*m_errorAtIJ)(extractVec3b(img1,i,j),extractVec3b(img2,i,j));
            c++;
          }
        }
    }
  assert(c != 0);
  return error/(c);
}
