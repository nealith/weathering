#include "TopTenBlockSelector.h"

/***********************************************
 *  TopTenBlockSelector
 ***********************************************/

quilting::TopTenBlockSelector::TopTenBlockSelector(BlocksGenerator * blocksGenerator, ErrorImg1Img2 * errorImg1Img2, bool redraw)
  : BlockSelector(blocksGenerator), m_errorImg1Img2(errorImg1Img2), m_redraw(redraw), m_firstDraw(true)
{
}

quilting::TopTenBlockSelector::~TopTenBlockSelector()
{
  delete m_errorImg1Img2;
}

/**
 * @brief TopTenBlockSelector::operator ()
 * @param output 8UC4
 * @param x : row index of Pold top-left corner in Is
 * @param y : col index of Pold top-left corner in Is
 * @return matching block candidate Pin in Io
 */
quilting::Block quilting::TopTenBlockSelector::operator()(const Mat & output, int i, int j)
{
  if(!m_firstDraw){
    (*m_blocksGenerator)++;
    m_firstDraw = false;
  }
  if (m_redraw) { // Regenerate library each draw
    (*m_blocksGenerator)++;
  }
  Mat * subOutput = extractSubOutput(output,Range(i,i+m_blocksGenerator->h()),Range(j,j+m_blocksGenerator->w()));
  vector<quilting::Block > & candidates = (*m_blocksGenerator)(); // 8UC3

  for (auto it = candidates.begin(); it != candidates.end(); ++it) {
    it->err = (*m_errorImg1Img2)(*(it->ptr),*subOutput);
  }

  sort(candidates.begin(),candidates.end(),quilting::Block::compare);
  // Selects without destroying list
//  namedWindow("DEBUG", WINDOW_AUTOSIZE);
//  imshow("DEBUG", **candidatesIt);
//  waitKey();
  delete subOutput;
  return candidates[m_rng.uniform(0, (int)fmin(9,candidates.size()-1))];
}
