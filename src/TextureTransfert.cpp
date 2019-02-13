#include "TextureTransfert.h"
#include <map>

/***********************************************
 *  TextureTransfert
 ***********************************************/

quilting::TextureTransfert::TextureTransfert(
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

quilting::TextureTransfert::~TextureTransfert()
{
  delete m_positionChooser;
  delete m_blockSelector;
  delete m_maskPostTreatement;
  delete m_borderPostTreatement;
}

void quilting::TextureTransfert::setBorderPostTreatement(BorderPostTreatement * bpt) {
	m_borderPostTreatement = bpt;
}

void quilting::TextureTransfert::setMaskPostTreatement(MaskPostTreatement * mpt) {
	m_maskPostTreatement = mpt;
}

int quilting::TextureTransfert::getOverlapSize() const
{
    return m_overlapSize;
}

void quilting::TextureTransfert::setOverlapSize(int overlapSize)
{
    m_overlapSize = overlapSize;
}

void quilting::TextureTransfert::operator()(Mat & output, std::function<void (const Mat & input, Mat & output)> matchingMapBuilder)
{

  std::map<Mat*,Mat> mapSamplesMatchingMapToSamples;

  for (auto it = (*m_blockSelector).getGenerator()->getSamples().begin(); it != (*m_blockSelector).getGenerator()->getSamples().end(); ++it) {
    Mat sampleMatchingMap;

    mapSamplesMatchingMapToSamples.insert(pair<Mat *, Mat>(&(*it),(*it)));
    matchingMapBuilder(*it,sampleMatchingMap);
    (*it) = sampleMatchingMap;
  }

  Mat outputMatchingMap;
  matchingMapBuilder(output,outputMatchingMap);

  Mat workOutput = output.clone();

  (*m_positionChooser)[output]; //grabs output's width and height

  // Dans le papier ils initialisent l'output avec un premier bloc random venant
  // de l'input sur lequel est ensuite lancé l'algo. Ca évite de se retrouver avec un calcul
  // d'erreur non nécessaire et en plus sur une zone vide lorsqu'on pose le 1er bloc. (Sans initialisation
  // c'est ce qu'il se passe, on appelle le blockSelector qui appelle AverageSumError qui regarde
  // si y'a des cases avec alpha à 255 (overlap) pour calculer, mais y'en a pas pour le 1er bloc
  // et il sort des NaN à fond, je sais même pas comment std::sort crash pas avec des NaN...)

  //DEBUG
  //namedWindow("DEBUG", WINDOW_AUTOSIZE);
  //imshow("DEBUG", outputMatchingMap);
  //waitKey(0);

  int old_w(0);
  int old_h(0);

  while((bool) (*m_positionChooser)){

    std::pair<int, int> p = (*m_positionChooser)(old_w,old_h,m_overlapSize);

    //DEBUG
    //std::cout << "it : P : " << p.first << ":" << p.second << '\n';

    int i = p.first;
    int j = p.second;

    quilting::Block block = (*m_blockSelector)(outputMatchingMap,i,j);
    assert(!block.ptr->empty());

    old_w = block.ptr->cols;
    old_h = block.ptr->rows;

    Mat mask;
    std::list<int *> border = (*m_maskGenerator)(*(block.ptr),outputMatchingMap,i,j,mask,m_overlapSize);

    if (m_maskPostTreatement != nullptr) {
      (*m_maskPostTreatement)(mask);
    }

    for (int ib = 0; (ib < block.ptr->rows && i+ib < output.rows); ib++) {
      double * m = mask.ptr<double>(ib);
      Vec3b * b = block.ptr->ptr<Vec3b>(ib);
      Vec4b * o = outputMatchingMap.ptr<Vec4b>(i+ib);
      for (int jb = 0; (jb < block.ptr->cols && j+jb < output.cols); jb++) {
        double mval = fmax(m[jb],0.0);
        o[j+jb].val[0] = (unsigned char) (((double)(b[jb].val[0]))*mval + ((double)(o[j+jb].val[0]))*(1.0-mval));
        o[j+jb].val[1] = (unsigned char) (((double)(b[jb].val[1]))*mval + ((double)(o[j+jb].val[1]))*(1.0-mval));
        o[j+jb].val[2] = (unsigned char) (((double)(b[jb].val[2]))*mval + ((double)(o[j+jb].val[2]))*(1.0-mval));
        o[j+jb].val[3] = (unsigned char) 255;
      }
    }

    Mat blockInSample = mapSamplesMatchingMapToSamples[block.originalSample](Range(block.i,block.i+block.ptr->rows),Range(block.j,block.j+block.ptr->cols));

    for (int ib = 0; (ib < block.ptr->rows && i+ib < output.rows); ib++) {
      double * m = mask.ptr<double>(ib);
      Vec3b * b = blockInSample.ptr<Vec3b>(ib);
      Vec4b * o = workOutput.ptr<Vec4b>(i+ib);
      for (int jb = 0; (jb < block.ptr->cols && j+jb < output.cols); jb++) {
        double mval = fmax(m[jb],0.0);
        o[j+jb].val[0] = (unsigned char) (((double)(b[jb].val[0]))*mval + ((double)(o[j+jb].val[0]))*(1.0-mval));
        o[j+jb].val[1] = (unsigned char) (((double)(b[jb].val[1]))*mval + ((double)(o[j+jb].val[1]))*(1.0-mval));
        o[j+jb].val[2] = (unsigned char) (((double)(b[jb].val[2]))*mval + ((double)(o[j+jb].val[2]))*(1.0-mval));
      }
    }

    output = workOutput.clone();

    if (m_borderPostTreatement != nullptr) {
      for (auto it = border.begin(); it != border.end(); ++it) {
        if ((*it)[0]+i>= 0 && (*it)[0]+i < workOutput.rows && (*it)[1]+j>= 0 && (*it)[1]+j < workOutput.cols) {
          (*m_borderPostTreatement)(workOutput,output,(*it)[0]+i,(*it)[1]+j,(*it)[2]);
          delete (*it);
        }
      }
    }

    //DEBUG
//    namedWindow("DEBUG", WINDOW_AUTOSIZE);
//    imshow("DEBUG",outputMatchingMap);
//    waitKey(0);
  }

}
