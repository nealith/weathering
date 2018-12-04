#include "SeamCarvingMaskGenerator.h"

quilting::SeamCarvingMaskGenerator::SeamCarvingMaskGenerator(ErrorMapImg1Img2 * errorMapImg1Img2)
  : MaskGenerator(), m_errorMapImg1Img2(errorMapImg1Img2)
{}

/**
 * @brief SeamCarvingMaskGenerator::operator ()
 * @param block : 8UC3
 * @param subOutput : 8UC4 subOutput.cols = block.cols && subOutput.rows = block.rows
 * @param mask : CV_64FC1, initialized in the function.
 * @param overlapSize : < block.rows && < block.cols
 * @return
 */
std::list<int * > quilting::SeamCarvingMaskGenerator::operator()(const Mat &block,const Mat & output, int i, int j, Mat &mask, int overlapSize)
{
  assert(overlapSize < block.cols || overlapSize < block.rows && "SeamCarvingMaskGenerator::operator() :: overlapSize > block.rows or cols length");


  Mat * subOutput = extractSubOutput(output,Range(i,i+block.cols),Range(j,j+block.rows));

  std::list<int * > list;
  Mat borderMask(block.rows,block.cols,CV_64FC1,Scalar(0));
  computeOverlapMaskVerticalPassOneBorder(block,(*subOutput),borderMask,overlapSize);
  cv::rotate(block, block, ROTATE_90_CLOCKWISE);
  cv::rotate((*subOutput), (*subOutput), ROTATE_90_CLOCKWISE);
  cv::rotate(borderMask, borderMask, ROTATE_90_CLOCKWISE);
  computeOverlapMaskVerticalPassOneBorder(block,(*subOutput),borderMask,overlapSize);
  cv::rotate(block, block, ROTATE_90_CLOCKWISE);
  cv::rotate((*subOutput), (*subOutput), ROTATE_90_CLOCKWISE);
  cv::rotate(borderMask, borderMask, ROTATE_90_CLOCKWISE);
  computeOverlapMaskVerticalPassOneBorder(block,(*subOutput),borderMask,overlapSize);
  cv::rotate(block, block, ROTATE_90_CLOCKWISE);
  cv::rotate((*subOutput), (*subOutput), ROTATE_90_CLOCKWISE);
  cv::rotate(borderMask, borderMask, ROTATE_90_CLOCKWISE);
  computeOverlapMaskVerticalPassOneBorder(block,(*subOutput),borderMask,overlapSize);
  cv::rotate(block, block, ROTATE_90_CLOCKWISE);
  cv::rotate((*subOutput), (*subOutput), ROTATE_90_CLOCKWISE);
  cv::rotate(borderMask, borderMask, ROTATE_90_CLOCKWISE);

  delete subOutput;

  // DEBUG
  //imshow( "The border mask", borderMask );
  //waitKey(0);

  Mat masktmp = borderMask.clone();
  Mat masktmpf;
  masktmp.convertTo(masktmpf,CV_32FC1);
  floodFill(masktmpf,cv::Point(block.cols/2,block.rows/2), Scalar(1.0));
  masktmpf.convertTo(masktmp,CV_64FC1);

  Mat kernel = getStructuringElement( MORPH_RECT, Size( 3, 3 ) );

  morphologyEx(masktmp,mask, MORPH_OPEN, kernel);

  //imshow( "The mask", mask );
  //waitKey(0);

  int * tuple;


  for(int ibm = 0; ibm < borderMask.rows; ibm++) {
    double * pborderMask = borderMask.ptr<double>(ibm);
    double * pmask = mask.ptr<double>(ibm);
    for(int jbm = 0; jbm < borderMask.cols; jbm++) {
        if (pborderMask[jbm] == pmask[jbm]) {
          bool v = false;
          bool h = false;
          bool push = true;

          tuple = new int[3];
          tuple[0] = ibm;
          tuple[1] = jbm;

          if (i+ibm > 0 && i+ibm < output.rows-1 && j+jbm > 0 && j+jbm < output.cols-1) {
            if ((ibm > 0 && ibm < borderMask.rows -1) && (jbm > 0 && jbm < borderMask.cols -1)) {

              if (pmask[jbm+1] == 1.0 || pmask[jbm-1] == 1.0) {
                h = true;
              }
              if (mask.ptr<double>(ibm-1)[jbm] == 1.0 || mask.ptr<double>(ibm+1)[jbm] == 1.0) {
                v = true;
              }

            } else if ((ibm > 0 && ibm < borderMask.rows -1) && (jbm == 0 || jbm == borderMask.cols - 1)){
              h = true;
              if (mask.ptr<double>(ibm-1)[jbm] == 1.0 || mask.ptr<double>(ibm+1)[jbm] == 1.0) {
                v = true;
              }
              if ((j-1 >= 0 && output.ptr<Vec4b>(i+ibm)[j-1].val[3] != 255) || (j+mask.cols+1 < output.cols && output.ptr<Vec4b>(i+ibm)[j+mask.cols+1].val[3] != 255)) {
                push = false;
              }
            } else if ((jbm > 0 && jbm < borderMask.cols -1) && (ibm == 0 || ibm == borderMask.rows - 1)){
              v = true;
              if (pmask[jbm-1] == 1.0 || pmask[jbm+1] == 1.0) {
                h = true;
              }
              if ((i-1 >= 0 && output.ptr<Vec4b>(i-1)[j+jbm].val[3] != 255) || (i+mask.rows+1 < output.rows && output.ptr<Vec4b>(i+mask.rows+1)[j+jbm].val[3] != 255)) {
                push = false;
              }
            }

            if (v && h) {
              tuple[2] = 2;
            } else if(v){
              tuple[2] = 1;
            } else if(h) {
              tuple[2] = 0;
            }

            if (push) {
              list.push_front(tuple);
            }
          }
        }

      }
    }
  return list;
}

/**
 * @brief SeamCarvingMaskGenerator::computeOverlapCouture
 * @param errorMap : CV_64F
 * @return
 */
std::list<std::pair<int, int> > quilting::SeamCarvingMaskGenerator::computeOverlapCouture(const Mat &errorMap)
{
  std::list<std::pair<int, int> > couture;

  Mat cumul = errorMap.clone(); //CV_64F

  // Attention aux types !!! cumul c'est du CV_64F, 1 double.
  for(int i = 1; i < cumul.rows; i++) {
    double * pcumul = cumul.ptr<double>(i);
    double * pcumulpre = cumul.ptr<double>(i-1);
      for(int j = 0; j < cumul.cols; j++) {
          if(j>0 && j<cumul.cols-1){
              pcumul[j] += fmin(pcumulpre[j-1],fmin(pcumulpre[j],pcumulpre[j+1]));
            } else if(j==cumul.cols-1){
              pcumul[j] += fmin(pcumulpre[j-1],pcumulpre[j]);
            } else if(j==0){
              pcumul[j] += fmin(pcumulpre[j],pcumulpre[j+1]);
            }

        }
    }

  int minX(0);
  for(int j = 0; j < cumul.cols; j++){
      if(cumul.ptr<double>(cumul.rows-1)[j] < cumul.ptr<double>(cumul.rows-1)[minX]){
          minX = j;
        }
    }
  int x(minX);
  couture.push_front(std::pair<int, int>(x,cumul.rows-1));
  for(int i = cumul.rows-1; i > 0; i--){
      int nextX = x;
      if(x > 0 && cumul.ptr<double>(i-1)[x-1] < cumul.ptr<double>(i-1)[nextX]){
          nextX = x-1;
        }
      if(x < cumul.cols-1 && cumul.ptr<double>(i-1)[x+1] < cumul.ptr<double>(i-1)[nextX]){
          nextX = x+1;
        }
      couture.push_front(std::pair<int, int>(nextX,i-1));
      x = nextX;
    }
  return couture;
}

/**
 * @brief SeamCarvingMaskGenerator::computeOverlapMaskVerticalPassOneBorder
 * @param block : 8UC3
 * @param subOutput : 8UC4
 * @param mask : 8UC3
 * @param overlapSize
 */
void quilting::SeamCarvingMaskGenerator::computeOverlapMaskVerticalPassOneBorder(const Mat &block, const Mat &subOutput, Mat &mask, int overlapSize)
{
  int status(2); // 1 : looking for overlap area
  // 2 : looking for non overlap area

  if(subOutput.ptr<Vec4b>(0)[0].val[3] == 0 || (subOutput.ptr<Vec4b>(0)[0].val[3] != 0 && subOutput.ptr<Vec4b>(0)[overlapSize].val[3] != 0 && subOutput.ptr<Vec4b>(overlapSize)[0].val[3] == 0)) {
      status = 1;
  }

  int yBeginOverlapArea(0);

  for(int i = 0; i < block.rows; i++) {
      Vec4b p1 = subOutput.ptr<Vec4b>(i)[0];

      if(status == 1 && p1.val[3] != 0 && (subOutput.ptr<Vec4b>(i)[overlapSize].val[3] == 0 || (subOutput.ptr<Vec4b>(0)[overlapSize].val[3] != 0 && (i>=(subOutput.rows-overlapSize) || subOutput.ptr<Vec4b>(overlapSize)[0].val[3] != 0)))){ // find a overlap area
          status = 2;
          yBeginOverlapArea = i;
          for(int j=0; j < overlapSize; j++){ // draw a horizontal border before overlap area
              mask.ptr<double>(i-1)[j] = 1.0;
          }
      } else if(status == 1){ // draw a vertical broder
        mask.ptr<double>(i)[0] = 1.0;
      }

      if(status == 2 && (p1.val[3] == 0 || i == mask.rows-1)){ // find a non overlap area || end of rows
          int ip(i);
          if (i == mask.rows-1) {
            ip++;
          }

          status = 1;
          Mat errorMap;
          errorMap = (*m_errorMapImg1Img2)(block(Range(yBeginOverlapArea,ip),Range(0,overlapSize)),subOutput(Range(yBeginOverlapArea,ip),Range(0,overlapSize)));
          std::list<std::pair<int, int> > couture = computeOverlapCouture(errorMap);
          for(auto it = couture.begin(); it != couture.end(); it++ ){
            mask.ptr<double>(it->second+yBeginOverlapArea)[it->first+0] = 1.0;
          }

          if (p1.val[3] == 0) {
            for(int j=0; j < 0 + overlapSize; j++){ // draw a horizontal border after the overlap area
                mask.ptr<double>(i)[j] = 1.0;
            }
          }
        }
    }
}
