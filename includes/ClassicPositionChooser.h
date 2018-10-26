#ifndef CLASSICPOSITIONCHOOSER_H
#define CLASSICPOSITIONCHOOSER_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief  Choose the next position from left to
  ///         right and top to bottom. Assume that
  ///         block are of the same size
  /////////////////////////////////////////////////

  class ClassicPositionChooser : public PositionChooser{
  public:
    ClassicPositionChooser();
    virtual pair<int, int> operator()(int blocksWidth, int blocksHeight, int overlapSize);
    virtual operator bool () const;
  protected:
    int m_nextI;
    int m_nextJ;
    bool m_continue;
  };

}
#endif // CLASSICPOSITIONCHOOSER_H
