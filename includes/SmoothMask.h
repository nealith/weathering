#ifndef SMOOTHMASK_H
#define SMOOTHMASK_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief Gaussian smooth on the overlap mask
  /////////////////////////////////////////////////
  class SmoothMask : public MaskPostTreatement {
  public:
    SmoothMask(float sigma, int overlapSize);
    void operator()(Mat & mask) override;
  private:
    float m_sigma;
    int m_os; //overlap size
  };

}

#endif // SMOOTHMASK_H
