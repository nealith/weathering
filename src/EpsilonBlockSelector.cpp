#include "EpsilonBlockSelector.h"

/***********************************************
 *  epsilonBlockSelector
 ***********************************************/
quilting::EpsilonBlockSelector::EpsilonBlockSelector(BlocksGenerator *blocksGenerator, ErrorImg1Img2 *errorImg1Img2, double epsilon, bool redraw)
  : BlockSelector(blocksGenerator), m_errorImg1Img2(errorImg1Img2), m_epsilon(epsilon),  m_redraw(redraw), m_firstDraw(true)
{

}

quilting::EpsilonBlockSelector::~EpsilonBlockSelector()
{
  delete m_errorImg1Img2;
}

quilting::Block quilting::EpsilonBlockSelector::operator()(const Mat &output, int i, int j)
{

  if(m_firstDraw){
    (*m_blocksGenerator)++;
    // reserve capacity for m_candidates (same size as m_corners for security)
    size_t vecsize = (*m_blocksGenerator)().size();
    std::cout << "m_candidates size:" << vecsize << std::endl;
    m_candidates.reserve(vecsize);
    assert(m_epsilon > 0.0 && m_epsilon < 1.0);
    m_firstDraw = false;
  }
  if(m_redraw){
    (*m_blocksGenerator)++;
  }
  // Get Pold (block to be filled) in Is (result image) at pos (x,y) with a block size (h,w)
  Mat * Pold = extractSubOutput(output,Range(i,i+m_blocksGenerator->w()),Range(j,j+m_blocksGenerator->h()));
  // Calculer l'erreur entre Pold et chaque bloc Pin dans Io, conserver dmin;
  double dmin = std::numeric_limits<double>::max();
  int err0 (0);
  vector<quilting::Block> & pins = (*m_blocksGenerator)();

  for (auto it = pins.begin(); it != pins.end(); ++it) {
    double err = (*m_errorImg1Img2)(*(it->ptr), *Pold);
    if (err == 0.) {
        err0++;
      }
    it->err = err; // Just override error, as the pos (i,j) are already set
    if (err < dmin && err >= 0.01) {
        dmin = err;
    }
  }
  // Get all blocks Pin that respects err < (1+e)*dmin
  for (auto it = pins.begin(); it != pins.end(); ++it) {
      if (it->err < ((1+m_epsilon)*dmin)) {
          m_candidates.push_back(*it);
      }
  }
//  std::cout << "candidates size:" << m_candidates.size() << std::endl;
  assert(m_candidates.size() > 0);
  assert(m_candidates.size() < pins.size());
  // Select a candidate randomly WITH A GOOD MOTHERFUCKING RAND GENERATOR
  int rand_cand = m_rng.uniform(0, (int)m_candidates.size()-1);
  assert(rand_cand < m_candidates.size());
  // DEBUG
//  namedWindow("BLOCK DEBUG", WINDOW_AUTOSIZE);
//  imshow("BLOCK DEBUG", m_chosenBlock);
//  waitKey();
  // Retourner ce block.
  delete Pold;
  quilting::Block b = m_candidates[rand_cand];
  m_candidates.clear();
  return b;
}
