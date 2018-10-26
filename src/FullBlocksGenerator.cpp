#include "FullBlocksGenerator.h"

/***********************************************
 *  FullBlocksGenerator
 ***********************************************/
quilting::FullBlocksGenerator::FullBlocksGenerator(Mat &sample,  int blocksWidth,  int blocksHeight)
: BlocksGenerator(blocksWidth, blocksHeight)
{
  m_samples.push_back(sample);
}

void quilting::FullBlocksGenerator::operator ++(int x)
{
  // No one wants to regenerate zillions of blocks.
  assert(m_blocks.empty());
  // Reserve capacity for m_corners (Much more efficient than relying on vector growth)
  size_t vecsize = (m_samples.front().rows - h()) *
      (m_samples.front().cols - w());
  m_blocks.reserve(vecsize);
  std::cout << "vecsize :" << vecsize << std::endl;
    // Pre-generate all top-left corner positions in m_corners
  Mat Io = m_samples.front();
  for (int i(0); i < Io.rows - h(); ++i) {
    for (int j(0); j < Io.cols - w(); ++j) {
        m_blocks.push_back(quilting::Block(new Mat(Io(Range(i,i+h()),Range(j,j+w()))),&(m_samples.front()),i,j,0.0));
    }
  }
}
