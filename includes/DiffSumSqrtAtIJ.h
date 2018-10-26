#ifndef DIFFSUMSQRTATIJ_H
#define DIFFSUMSQRTATIJ_H

#include "CORE.h"

namespace quilting {

  class DiffSumSqrtAtIJ : public ErrorAtIJ{
  public:
    DiffSumSqrtAtIJ();
    virtual double operator()(const Vec3b & img1, const Vec3b & img2);
  };

}

#endif // DIFFSUMSQRTATIJ_H
