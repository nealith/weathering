#ifndef EPSILONBLOCKSELECTOR_H
#define EPSILONBLOCKSELECTOR_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief  Uniformly select a block from a
  ///         selection of blocks that respects the
  ///         condition D(k,l) < (1+e)dmin, dmin
  ///         beeing the minimum error computed
  ///         over all the blocks from Io.
  /////////////////////////////////////////////////
  class EpsilonBlockSelector : public BlockSelector {
  public:
    EpsilonBlockSelector(BlocksGenerator * blocksGenerator,ErrorImg1Img2 * errorImg1Img2, double epsilon, bool redraw = false);
    ~EpsilonBlockSelector();
    Block operator()(const Mat & output, int x, int y) override;

  private:
    ErrorImg1Img2 * m_errorImg1Img2;
    double m_epsilon;
    vector<Block> m_candidates;
    randutils::mt19937_rng m_rng;
    bool m_redraw;
    bool m_firstDraw;
  };

}
#endif // EPSILONBLOCKSELECTOR_H
