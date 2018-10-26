#ifndef RANDOMBLOCKSGENERATOR_H
#define RANDOMBLOCKSGENERATOR_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief Create a list of block from a sample
  ///        of texture by random selection
  /////////////////////////////////////////////////

  class RandomBlocksGenerator : public BlocksGenerator{
  public:
    RandomBlocksGenerator(Mat & sample, int blocksWidth = 32,  int blocksHeight = 32,  int librarySize = 64);
    virtual void operator++ (int x);
    int getLibrarySize();
    void setLibrarySize(int size);
  protected:
     int m_librarySize;
  };

}

#endif // RANDOMBLOCKSGENERATOR_H
