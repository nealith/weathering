#ifndef AVERAGESUMERROR_H
#define AVERAGESUMERROR_H

#include "CORE.h"

namespace quilting {

  class AverageSumError : public ErrorImg1Img2{
  public:
    AverageSumError(ErrorAtIJ * errorAtIJ);
    virtual double operator()(const Mat & img1, const Mat & img2);
  };

}


#endif // AVERAGESUMERROR_H
