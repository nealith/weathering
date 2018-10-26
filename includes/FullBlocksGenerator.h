#ifndef FULLBLOCKSGENERATOR_H
#define FULLBLOCKSGENERATOR_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief Generate all blocks from input texture
  ///        sample
  /////////////////////////////////////////////////

  class FullBlocksGenerator : public BlocksGenerator {
  public:
    FullBlocksGenerator(Mat & sample,  int blocksWidth = 32,  int blocksHeight = 32);
    void operator++(int x) override;
  };

}

#endif // FULLBLOCKSGENERATOR_H
