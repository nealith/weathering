#ifndef CORE_H
#define CORE_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <cmath>
#include <string>
#include <memory>

#include <list>

#include "randutils.h"

using namespace cv;
using namespace std;

namespace quilting {

  Vec3b extractVec3b(const Mat & img, int i, int j);

  Mat * extractSubOutput(const Mat & output, Range rows, Range cols);

  // struct representing a block Pin from its top-left corner and its distance
  // error with Pold
  struct Block {
    shared_ptr<Mat> ptr; // ptr to the block
    Mat * originalSample; // ptr to the sample from where the block is extracted /!\ this ptr should be only use to read the source, not modify it !
    double err; // Distance error with Pold (aka the last subOutput where the block has been put)
    int i; // row coord of top left corner
    int j; // col coord of top left corner
    Block(Mat * _ptr, Mat * _originalSample,int _i, int _j,  double _err)
      : ptr(_ptr),originalSample(_originalSample), err(_err), i(_i), j(_j)
      {
        assert(ptr != nullptr && originalSample != nullptr);
      }
    ~Block(){
    } // deleting originalSample is not part of the job au Block
    static bool compare(Block & a, Block & b){return a.err < b.err;}

  };


  /////////////////////////////////////////////////
  /// \brief Create a list of block from a sample
  ///        of texture
  /////////////////////////////////////////////////

  class BlocksGenerator{
  public:
    BlocksGenerator(int blocksWidth = 32, int blocksHeight = 32);
    virtual ~BlocksGenerator();
    vector<Block> & operator()();
    virtual void operator++ (int x) = 0;
    Block getRandomBlock();
    vector<Mat> & getSamples();


    int w() const;
    int h() const;

  protected:
    vector<Block> m_blocks;
    vector<Mat> m_samples;
    int m_blocksWidth;
    int m_blocksHeight;
    randutils::mt19937_rng m_rng;
  };

  /////////////////////////////////////////////////
  /// \brief  Select a block in a list of blocks
  ///         that will be put in output texture
  /////////////////////////////////////////////////

  class BlockSelector{
  public:
    BlockSelector(BlocksGenerator * blocksGenerator);
    virtual ~BlockSelector();
    virtual Block operator()(const Mat & output, int i, int j) = 0;
    Block getInitRandomBlock() const;
    BlocksGenerator * getGenerator() const;
  protected:
    BlocksGenerator * m_blocksGenerator;
  };

  /////////////////////////////////////////////////
  /// \brief  Choose the next position in output
  ///         texture where to put a block
  /////////////////////////////////////////////////

  class PositionChooser{
  public:
    PositionChooser();
    virtual ~PositionChooser() {}
    virtual pair<int, int> operator()(int blocksWidth, int blocksHeight, int overlapSize) = 0;

    // return true if a block can be put, false if not
    // usage : if( (bool) myPositionChooser ){...}
    virtual operator bool () const = 0;

    // Used to configure the PositionChooser according to the output texture
    virtual void operator[](const Mat & output);
  protected:
    int m_height;
    int m_width;
  };

  /////////////////////////////////////////////////
  /// \brief Return the error between two pixels
  ///        from two differents images
  /////////////////////////////////////////////////

  class ErrorAtIJ{
  public:
    virtual double operator()(const Vec3b & img1, const Vec3b & img2) = 0;
    virtual ~ErrorAtIJ() {}
  };

  /////////////////////////////////////////////////
  /// \brief Return the error between two images
  /////////////////////////////////////////////////

  class ErrorImg1Img2{
  public:
    ErrorImg1Img2(ErrorAtIJ * errorAtIJ);
    virtual ~ErrorImg1Img2();
    virtual double operator()(const Mat & img1, const Mat & img2) = 0;
  protected:
    ErrorAtIJ * m_errorAtIJ;
  };

  /////////////////////////////////////////////////
  /// \brief Return a error map between two images
  ///
  /// \return Mat<double> (CV_64FC1)
  /////////////////////////////////////////////////

  class ErrorMapImg1Img2 {
  public:
    ErrorMapImg1Img2(ErrorAtIJ * errorAtIJ);
    virtual ~ErrorMapImg1Img2();
    virtual Mat operator()(const Mat & img1, const Mat & img2);
  protected:
    ErrorAtIJ * m_errorAtIJ;
  };

  /////////////////////////////////////////////////
  /// \brief Make a post treatment on the mask
  ///        before it's use to put the block
  /////////////////////////////////////////////////

  class MaskPostTreatement{
  public:
    virtual void operator()(Mat & mask) = 0;
    virtual ~MaskPostTreatement() {}
  };

  /////////////////////////////////////////////////
  /// \brief Do a post treament along the border of
  ///        the block after it had been put
  /////////////////////////////////////////////////

  class BorderPostTreatement{
  public:
    virtual void operator()(const Mat & input, Mat & output, int i, int j, int orientation) = 0;
    virtual ~BorderPostTreatement() {}
  };

  /////////////////////////////////////////////////
  /// \brief Generate a mask for a given block and subOutput
  /////////////////////////////////////////////////

  class MaskGenerator{
  public:
    virtual std::list<int * > operator()(const Mat &block,const Mat & output, int i, int j, Mat &mask, int overlapSize) = 0;
  };

}

#endif // CORE_H
