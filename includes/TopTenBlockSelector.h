#ifndef TOPTENBLOCKSELECTOR_H
#define TOPTENBLOCKSELECTOR_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief  Select a block randomly in the first
  ///         ten blocks sorted according to the
  ///         lower error with the output
  /////////////////////////////////////////////////

  class TopTenBlockSelector : public BlockSelector {
  public:
    TopTenBlockSelector(BlocksGenerator * blocksGenerator,ErrorImg1Img2 * errorImg1Img2, bool redraw = false);
    ~TopTenBlockSelector();
    virtual Block operator()(const Mat & output, int i, int j);

  protected:
    ErrorImg1Img2 * m_errorImg1Img2;
    bool m_redraw;
    bool m_firstDraw;
    randutils::mt19937_rng m_rng;
  };

}

#endif // TOPTENBLOCKSELECTOR_H
