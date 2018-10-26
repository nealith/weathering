#ifndef TEXTUREGENERATOR_H
#define TEXTUREGENERATOR_H

#include "CORE.h"

namespace quilting {

  /////////////////////////////////////////////////
  /// \brief Generate the output texture
  /////////////////////////////////////////////////

  class TextureGenerator{
  public:
    TextureGenerator(PositionChooser * positionChooser, BlockSelector * blockSelector, MaskGenerator * maskGenerator, int overlapSize = 0, MaskPostTreatement * maskPostTreatement  = nullptr, BorderPostTreatement * borderPostTreatement = nullptr);
    ~TextureGenerator();
    void operator()(Mat & output);
    int getOverlapSize() const;
    void setOverlapSize(int value);
    void setBorderPostTreatement(BorderPostTreatement * bpt);
    void setMaskPostTreatement(MaskPostTreatement * mpt);

  protected:
    PositionChooser * m_positionChooser;
    BlockSelector * m_blockSelector;
    MaskGenerator * m_maskGenerator;
    int m_overlapSize;
    MaskPostTreatement * m_maskPostTreatement;
    BorderPostTreatement * m_borderPostTreatement;
  };

}

#endif // TEXTUREGENERATOR_H
