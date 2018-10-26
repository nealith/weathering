#ifndef SEAMCARVINGMASKGENERATOR_H
#define SEAMCARVINGMASKGENERATOR_H

#include "CORE.h"

namespace quilting {

  class SeamCarvingMaskGenerator : public MaskGenerator {
  public:
    SeamCarvingMaskGenerator(ErrorMapImg1Img2 * errorMapImg1Img2);
    std::list<int * > operator()(const Mat &block,const Mat & output, int i, int j, Mat &mask, int overlapSize) override;
  private:
    ErrorMapImg1Img2 * m_errorMapImg1Img2;
    std::list<std::pair<int, int> > computeOverlapCouture(const Mat &errorMap);
    void computeOverlapMaskVerticalPassOneBorder(const Mat &block, const Mat &underTheBlock, Mat &mask, int overlapSize);
  };

}

#endif // SEAMCARVINGMASKGENERATOR_H
