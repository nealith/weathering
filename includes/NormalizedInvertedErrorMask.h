#ifndef NORMALIZEDINVERTEDERRORMASK_H
#define NORMALIZEDINVERTEDERRORMASK_H

#include "CORE.h"

namespace quilting {

  class NormalizedInvertedErrorMask : public MaskGenerator {
  public:
    NormalizedInvertedErrorMask(ErrorMapImg1Img2 * errorMapImg1Img2, Range threshold = Range(0,std::numeric_limits<int>::max()));
    std::list<int * > operator()(const Mat &block,const Mat & output, int i, int j, Mat &mask, int overlapSize) override;
  private:
    ErrorMapImg1Img2 * m_errorMapImg1Img2;
    Range m_threshold;
  };

}

#endif // NORMALIZEDINVERTEDERRORMASK_H
