#ifndef IMAGEWEATHERING_H
#define IMAGEWEATHERING_H

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
    void operator()(const Mat & input,std::vector<std::pair<unsigned int,Mat*>> & output, std::vector<std::pair<unsigned int,Mat*>> * weathered_map = nullptr, Mat * shadowMap = nullptr, bool grabcut = true);

  protected:

    Rect2d * userInput(const Mat & input);
    Mat * grabCut(const Mat & input);
    Mat computeWeatheringDegreeMap(const Mat & user_input_grabcut, const Mat & mask_input_grabcut, Rect2d * user_input);
    Mat computeShadowMap(const Mat & user_input_grabcut, const Mat & mask_input_grabcut, const Mat & degree_map, const Mat & segmentation);
    Mat segment(const Mat & degree_map, const Mat & mask_input_grabcut);
    Mat computeWeatheringExemplar(const Mat & user_input_grabcut,const Mat & degree_map, Rect2d & coord);
    Mat updateWeatheringDegreeMap(const Mat & degree_map, const Mat & segmentation, unsigned int degree);
    Mat computeWeatheringImage(const Mat & input, const Mat & generated_texture, const Mat & updated_degree_map, const Mat & exemplar, const Mat & segmentation, const Mat & shadow_map, double threshold);
    Mat computeTexture(const Mat & input_bgr,const Mat & exemplar_bgr, const Mat & segmentation);

  public:

    static double * featureVector(const Mat & img, int r, int c, double siga = 0.2, double sigs = 50);
    static double computeWd(const Mat & img,int i, int j,const vector<wdi> & omega);
    static double rbf(const double * i,const double * j);
    //Mat initDi(const Mat & img, const Mat & mask);

  };

}

#endif // TEXTUREGENERATOR_H
