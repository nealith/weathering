#include "SmoothMask.h"

/////////////////////////////////////////////////
/// \brief Gaussian smooth on the overlap mask
/////////////////////////////////////////////////
quilting::SmoothMask::SmoothMask(float sigma, int overlapSize)
  : m_sigma(sigma), m_os(overlapSize)
{
  assert(sigma > 0.0);
}

void quilting::SmoothMask::operator()(Mat &mask)
{
  // TOP
  GaussianBlur(mask(Rect(0, 0, mask.cols, m_os)), mask(Rect(0, 0, mask.cols, m_os)),
              Size(3,3), m_sigma, m_sigma);
              
  // LEFT
  GaussianBlur(mask(Rect(0, m_os, m_os, mask.rows-2*m_os)), mask(Rect(0, m_os, m_os, mask.rows-2*m_os)),
              Size(3,3), m_sigma, m_sigma);

  // BOTTOM
  GaussianBlur(mask(Rect(0, mask.rows-m_os, mask.cols, m_os)), mask(Rect(0, mask.rows-m_os, mask.cols, m_os)),
              Size(3,3), m_sigma, m_sigma);

  // RIGHT
  GaussianBlur(mask(Rect(mask.cols-m_os, m_os, m_os, mask.rows-2*m_os)), mask(Rect(mask.cols-m_os, m_os, m_os, mask.rows-2*m_os)),
              Size(3,3), m_sigma, m_sigma);
}
