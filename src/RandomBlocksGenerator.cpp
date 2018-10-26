#include "RandomBlocksGenerator.h"

/***********************************************
 *  RandomBlocksGenerator
 ***********************************************/

quilting::RandomBlocksGenerator::RandomBlocksGenerator(Mat & sample, int blocksWidth,  int blocksHeight,  int librarySize)
  : BlocksGenerator(blocksWidth,blocksHeight),
    m_librarySize(librarySize)
{
	m_samples.push_back(sample);
}

void quilting::RandomBlocksGenerator::operator++ (int x)
{
  m_blocks.clear();
  for ( int imgNumber = 0; imgNumber < m_librarySize; imgNumber++) {
    m_blocks.push_back(getRandomBlock());
  }
}

int quilting::RandomBlocksGenerator::getLibrarySize()
{
  return m_librarySize;
}

void quilting::RandomBlocksGenerator::setLibrarySize(int size)
{
  assert(size > 0);
  if (size > 0) {
      m_librarySize = size;
    }
}
