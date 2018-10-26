#include "NormalizedInvertedErrorMask.h"

quilting::NormalizedInvertedErrorMask::NormalizedInvertedErrorMask(ErrorMapImg1Img2 * errorMapImg1Img2, Range threshold)
  : MaskGenerator(), m_errorMapImg1Img2(errorMapImg1Img2), m_threshold(threshold)
{}

/**
 * @brief NormalizedInvertedErrorMask::operator ()
 * @param block : 8UC3
 * @param subOutput : 8UC4 subOutput.cols = block.cols && subOutput.rows = block.rows
 * @param mask : CV_64FC1, initialized in the function.
 * @param overlapSize : < block.rows && < block.cols
 * @return
 */
std::list<int * > quilting::NormalizedInvertedErrorMask::operator()(const Mat &block,const Mat & output, int i, int j, Mat &mask, int overlapSize)
{
  assert(overlapSize < block.cols || overlapSize < block.rows && "NormalizedInvertedErrorMask::operator() :: overlapSize > block.rows or cols length");

  mask = Mat(block.rows,block.cols,CV_64FC1,Scalar(0));
  Mat * subOutput = extractSubOutput(output,Range(i,i+block.cols),Range(j,j+block.rows));

  std::list<int * > list;

  Mat errorMap = (*m_errorMapImg1Img2)(block,*subOutput);
  Mat errorMapN;

  normalize(errorMap, errorMapN, 0, 1, NORM_MINMAX, -1, Mat() );

  for (unsigned int ib = 0; ib < errorMapN.rows; ib++) {
    for (unsigned int jb = 0; jb < errorMapN.cols; jb++) {
      if (errorMap.ptr<double>(ib)[jb] < m_threshold.start) {
        mask.ptr<double>(ib)[jb] = 1.0;
      } else if (errorMap.ptr<double>(ib)[jb] < m_threshold.end) {
        mask.ptr<double>(ib)[jb] = 1.0 - errorMapN.ptr<double>(ib)[jb];
      }
    }
  }


  delete subOutput;

  return list;
}
