#ifndef TEXTUREGENERATOR_H
#define TEXTUREGENERATOR_H

#include "CORE.h"
#include <functional>

namespace quilting {


  /////////////////////////////////////////////////
  /// \brief Transfert texture in a image
  /////////////////////////////////////////////////

  class TextureTransfert{
  public:
    TextureTransfert(PositionChooser * positionChooser, BlockSelector * blockSelector, MaskGenerator * maskGenerator, int overlapSize = 0, MaskPostTreatement * maskPostTreatement  = nullptr, BorderPostTreatement * borderPostTreatement = nullptr);
    ~TextureTransfert();
    void operator()(Mat & output, std::function<void (const Mat & input, Mat & output)> matchingMapBuilder);
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
