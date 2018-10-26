#ifndef BORDERKERNELBLUR_H
#define BORDERKERNELBLUR_H

#include "CORE.h"

namespace quilting {

	/////////////////////////////////////////////////
	/// \brief Return a error map between two images
	///
	/// \param input CV_8UC4
	/// \param output CV_8UC4
	/////////////////////////////////////////////////
	void applyKernel(const Mat & input, Mat & output, const Mat & kernel, int i, int j);

	class BorderKernelBlur : public BorderPostTreatement{

	private:
		Mat m_kernel;
	public:
	  BorderKernelBlur(unsigned int size = 3, double sigma = -1.0);
	  BorderKernelBlur(const Mat &kernel);
	  void operator()(const Mat & input, Mat & output, int i, int j, int orientation);

	};

}


#endif // BORDERKERNELBLUR_H
