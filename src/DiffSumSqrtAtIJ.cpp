#include "DiffSumSqrtAtIJ.h"

/***********************************************
 *  DiffSumSqrtAtIJ
 ***********************************************/
quilting::DiffSumSqrtAtIJ::DiffSumSqrtAtIJ()
  : ErrorAtIJ()
{}

double quilting::DiffSumSqrtAtIJ::operator()(const Vec3b & img1, const Vec3b & img2)
{
  double error(0.0);
  error += (img1.val[0]-img2.val[0])*(img1.val[0]-img2.val[0]);
  error += (img1.val[1]-img2.val[1])*(img1.val[1]-img2.val[1]);
  error += (img1.val[2]-img2.val[2])*(img1.val[2]-img2.val[2]);
  sqrt(error);
  error/=3.0;
  return error;
}
