#ifndef TEXTUREGENERATOR_H
#define TEXTUREGENERATOR_H

#include "CORE.h"
#include <opencv2/tracking.hpp>

namespace weathering {

  struct wdi {
    unsigned int i;
    unsigned int j;
    double di;
    double a;
  };

  struct objective_function_degree_map_data {
    vector<wdi> * omega;
    const Mat * input;
  };

  double objective_function_degree_map(const std::vector<double> &x, std::vector<double> &grad, void* f_data);

  struct objective_function_segmentation_data {
    vector<wdi> * omega;
    const Mat * input;
  };

  double objective_function_segmentation(const std::vector<double> &x, std::vector<double> &grad, void* f_data);

  Mat patch_match(const Mat & to_fill, const Mat & to_fill_mask , const Mat & exemplar);

  /////////////////////////////////////////////////
  /// \brief Generate weathered image
  /////////////////////////////////////////////////

  class ImageWeathering{
  public:
    ImageWeathering();
    ~ImageWeathering();
    void operator()(Mat & input,std::list<std::pair<float,Mat *>> & output, std::list<std::pair<float,Mat *>> * weatheringMaps = nullptr, Mat * shadowMap = nullptr );

  protected:

    Rect2d * userInput(const Mat & input);
    Mat * grabCut(const Mat & input);
    Mat computeWeatheringDegreeMap(const Mat & user_input_grabcut, const Mat & mask_input_grabcut, Rect2d * user_input);
    Mat computeShadowMap(const Mat & user_input_grabcut, const Mat & mask_input_grabcut, const Mat & degree_map, const Mat & segmentation);
    Mat segment(const Mat & degree_map, const Mat & mask_input_grabcut);
    Mat computeWeatheringExemplar(Mat & user_input_grabcut, Mat & degree_map);
    Mat updateWeatheringDegreeMap(const Mat & degree_map, const Mat & segmentation, unsigned int degree);
    Mat computeWeatheringImage(Mat & weatheringDegreeMapUpdated, Mat & shadowMap, Mat & output);

  public:

    static double * featureVector(const Mat & img, int r, int c, double siga = 0.2, double sigs = 50);
    static double computeWd(const Mat & img,int i, int j,const vector<wdi> & omega);
    static double rbf(const double * i,const double * j);
    //Mat initDi(const Mat & img, const Mat & mask);

  };

}

#endif // TEXTUREGENERATOR_H
