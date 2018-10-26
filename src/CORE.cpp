#include "CORE.h"

Vec3b quilting::extractVec3b(const Mat & img, int i, int j)
{
  assert(i < img.rows && j < img.cols);
  if (img.channels() == 4) {
    Vec4b v = img.ptr<Vec4b>(i)[j];
    return Vec3b(v.val[0],v.val[1],v.val[2]);
  } else if (img.channels() == 3){
    return img.ptr<Vec3b>(i)[j];
  } else {
    throw std::invalid_argument( "Invalid number of channels" );
  }
}

Mat * quilting::extractSubOutput(const Mat & output, Range rows, Range cols)
{
  Mat * subOutput = new Mat(rows.size(),cols.size(),CV_8UC4);
  for (int i = 0; i < subOutput->rows; i++) {
    Vec4b * s = subOutput->ptr<Vec4b>(i);
    for (int j = 0; j < subOutput->cols; j++) {
      if (i+rows.start >= 0 && i+rows.start < output.rows && j+cols.start >= 0 && j+cols.start < output.cols) {
        s[j].val[0] = (unsigned char) output.ptr<Vec4b>(rows.start+i)[cols.start+j].val[0];
        s[j].val[1] = (unsigned char) output.ptr<Vec4b>(rows.start+i)[cols.start+j].val[1];
        s[j].val[2] = (unsigned char) output.ptr<Vec4b>(rows.start+i)[cols.start+j].val[2];
        s[j].val[3] = (unsigned char) output.ptr<Vec4b>(rows.start+i)[cols.start+j].val[3];
      } else {
        s[j].val[0] = (unsigned char) 0;
        s[j].val[1] = (unsigned char) 0;
        s[j].val[2] = (unsigned char) 0;
        s[j].val[3] = (unsigned char) 0;
      }
    }
  }
  return subOutput;
}
/***********************************************
 *  BlocksGenerator
 ***********************************************/

quilting::BlocksGenerator::BlocksGenerator(int blocksWidth, int blocksHeight):
m_blocksWidth(blocksWidth),
m_blocksHeight(blocksHeight)
{}

quilting::BlocksGenerator::~BlocksGenerator()
{}

vector<quilting::Block> & quilting::BlocksGenerator::operator()(){
  return m_blocks;
}

int quilting::BlocksGenerator::w() const {
  return m_blocksWidth;
}

int quilting::BlocksGenerator::h() const {
  return m_blocksHeight;
}

quilting::Block quilting::BlocksGenerator::getRandomBlock(){
  int rand_sample(0);
  if (m_samples.size() > 1) {
    rand_sample = m_rng.uniform(0, (int)m_samples.size()-1);
  }
  int maxRow = m_samples[rand_sample].rows - h();
  int maxCol = m_samples[rand_sample].cols - w();
  int rand_row = m_rng.uniform(0, maxRow);
  int rand_col = m_rng.uniform(0, maxCol);
  assert(rand_row + h() <= m_samples[rand_sample].rows);
  assert(rand_col + w() <= m_samples[rand_sample].cols);
  Mat * ptr = new Mat(m_samples[rand_sample](Rect(rand_col, rand_row, w(), h())));
  return Block(ptr,&(m_samples[rand_sample]),rand_row,rand_col,0.0);

}

vector<Mat> & quilting::BlocksGenerator::getSamples(){
  return m_samples;
}

/***********************************************
 *  BlockSelector
 ***********************************************/

quilting::BlockSelector::BlockSelector(BlocksGenerator * blocksGenerator):
m_blocksGenerator(blocksGenerator)
{}

quilting::BlockSelector::~BlockSelector(){
  delete m_blocksGenerator;
}

quilting::Block quilting::BlockSelector::getInitRandomBlock() const {
  return m_blocksGenerator->getRandomBlock();
}

quilting::BlocksGenerator * quilting::BlockSelector::getGenerator() const {
  return m_blocksGenerator;
}

/***********************************************
 *  PositionChooser
 ***********************************************/

quilting::PositionChooser::PositionChooser():
m_height(0),
m_width(0)
{}

void quilting::PositionChooser::operator[](const Mat & output){
  m_width = output.cols;
  m_height = output.rows;
}
/***********************************************
 *  ErrorImg1Img2
 ***********************************************/

quilting::ErrorImg1Img2::ErrorImg1Img2(ErrorAtIJ * errorAtIJ):
m_errorAtIJ(errorAtIJ){}

quilting::ErrorImg1Img2::~ErrorImg1Img2(){
 delete m_errorAtIJ;
}

/***********************************************
 *  ErrorMapImg1Img2
 ***********************************************/

quilting::ErrorMapImg1Img2::ErrorMapImg1Img2(ErrorAtIJ * errorAtIJ)
  : m_errorAtIJ(errorAtIJ)
{}

quilting::ErrorMapImg1Img2::~ErrorMapImg1Img2()
{
 delete m_errorAtIJ;
}

Mat quilting::ErrorMapImg1Img2::operator()(const Mat & img1, const Mat & img2)
{
  Mat errorMap(img1.rows,img2.cols,CV_64FC1);
  for(int i = 0; i < img1.rows; i++) {
    double * p = errorMap.ptr<double>(i);
      for(int j = 0; j < img1.cols; j++) {
          p[j] = (*m_errorAtIJ)(extractVec3b(img1,i,j),extractVec3b(img2,i,j));
      }
  }
  return errorMap;
}
