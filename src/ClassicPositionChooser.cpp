#include "ClassicPositionChooser.h"

/***********************************************
 *  ClassicPositionChooser
 ***********************************************/

quilting::ClassicPositionChooser::ClassicPositionChooser()
  : PositionChooser(),
  m_nextI(0),
  m_nextJ(0),
  m_continue(true)
{}

pair<int, int> quilting::ClassicPositionChooser::operator()(int blocksWidth, int blocksHeight, int overlapSize)
{
  if ((int)m_nextI + (int)blocksHeight - (int)overlapSize < 0) {
      m_nextI = 0;
    } else {
      m_nextI += blocksHeight - overlapSize;
    }

  if (m_nextI + overlapSize >= m_height) {
      m_nextI = 0;
      m_nextJ += blocksWidth - overlapSize;
    }

  if (m_nextI + blocksHeight >= m_height - overlapSize && m_nextJ + blocksWidth - overlapSize >= m_width) {
      m_continue = false;
    }

  return pair<int, int>(m_nextI,m_nextJ);
}

quilting::ClassicPositionChooser::operator bool () const
{
  return m_continue;
}
