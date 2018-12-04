#include "TextureGenerator.h"

/***********************************************
 *  TextureGenerator
 ***********************************************/

quilting::TextureGenerator::TextureGenerator(
  PositionChooser * positionChooser,
  BlockSelector * blockSelector,
  MaskGenerator * maskGenerator,
  int overlapSize,
  MaskPostTreatement * maskPostTreatement,
  BorderPostTreatement * borderPostTreatement):
m_positionChooser(positionChooser),
m_blockSelector(blockSelector),
m_maskGenerator(maskGenerator),
m_overlapSize(overlapSize),
m_maskPostTreatement(maskPostTreatement),
m_borderPostTreatement(borderPostTreatement)
{}

quilting::TextureGenerator::~TextureGenerator()
{
  delete m_positionChooser;
  delete m_blockSelector;
  delete m_maskPostTreatement;
  delete m_borderPostTreatement;
}

void quilting::TextureGenerator::setBorderPostTreatement(BorderPostTreatement * bpt) {
	m_borderPostTreatement = bpt;
}

void quilting::TextureGenerator::setMaskPostTreatement(MaskPostTreatement * mpt) {
	m_maskPostTreatement = mpt;
}

int quilting::TextureGenerator::getOverlapSize() const
{
    return m_overlapSize;
}

void quilting::TextureGenerator::setOverlapSize(int overlapSize)
{
    m_overlapSize = overlapSize;
}

void quilting::TextureGenerator::operator()(Mat & output)
{
  Mat outputAlpha(output.rows,output.cols,CV_8UC4, Scalar(0));
  (*m_positionChooser)[output]; //grabs output's width and height

  // Initialisation de Is avec un bloc Pin choisi uniformément dans Io.
  quilting::Block block = (*m_blockSelector).getInitRandomBlock();
  for (int ib(0); ib < block.ptr->rows; ++ib) {
      Vec3b * pfb = block.ptr->ptr<Vec3b>(ib);
      Vec4b * oa = outputAlpha.ptr<Vec4b>(ib);
      for (int jb(0); jb < block.ptr->cols; ++jb) {
          oa[jb] = Vec4b(pfb[jb].val[0], pfb[jb].val[1], pfb[jb].val[2], 255);
        }
    }
  //DEBUG
  //namedWindow("DEBUG", WINDOW_AUTOSIZE);
  //imshow("DEBUG", outputAlpha);
  //waitKey(0);

  int old_w(block.ptr->cols);
  int old_h(block.ptr->rows);

  while((bool) (*m_positionChooser)){

    std::pair<int, int> p = (*m_positionChooser)(old_w,old_h,m_overlapSize);

    //DEBUG
    //std::cout << "it : P : " << p.first << ":" << p.second << '\n';

    int i = p.first;
    int j = p.second;

    block = (*m_blockSelector)(outputAlpha,i,j);
    assert(!block.ptr->empty());

    old_w = block.ptr->cols;
    old_h = block.ptr->rows;

    Mat mask;
    std::list<int *> border = (*m_maskGenerator)(*(block.ptr),outputAlpha,i,j,mask,m_overlapSize);



    if (m_maskPostTreatement != nullptr) {
      (*m_maskPostTreatement)(mask);
    }

    for (int ib = 0; (ib < block.ptr->rows && i+ib < output.rows); ib++) {
      double * m = mask.ptr<double>(ib);
      Vec3b * b = block.ptr->ptr<Vec3b>(ib);
      Vec4b * o = outputAlpha.ptr<Vec4b>(i+ib);
      for (int jb = 0; (jb < block.ptr->cols && j+jb < output.cols); jb++) {
        double mval = fmax(m[jb],0.0);
        o[j+jb].val[0] = (unsigned char) (((double)(b[jb].val[0]))*mval + ((double)(o[j+jb].val[0]))*(1.0-mval));
        o[j+jb].val[1] = (unsigned char) (((double)(b[jb].val[1]))*mval + ((double)(o[j+jb].val[1]))*(1.0-mval));
        o[j+jb].val[2] = (unsigned char) (((double)(b[jb].val[2]))*mval + ((double)(o[j+jb].val[2]))*(1.0-mval));
        o[j+jb].val[3] = (unsigned char) 255;
      }
    }
    output = outputAlpha.clone();

    if (m_borderPostTreatement != nullptr) {
      for (auto it = border.begin(); it != border.end(); ++it) {
        if ((*it)[0]+i>= 0 && (*it)[0]+i < outputAlpha.rows && (*it)[1]+j>= 0 && (*it)[1]+j < outputAlpha.cols) {
          (*m_borderPostTreatement)(outputAlpha,output,(*it)[0]+i,(*it)[1]+j,(*it)[2]);
          delete (*it);
        }
      }
    }

    //DEBUG
//    namedWindow("DEBUG", WINDOW_AUTOSIZE);
//    imshow("DEBUG",outputAlpha);
//    waitKey(0);
  }

}
