#include "BorderKernelBlur.h"

void quilting::applyKernel(const Mat & input, Mat & output, const Mat & kernel, int i, int j){
	Vec4b r;
	for (unsigned int ik = 0; ik < kernel.rows; ik++) {
		for (unsigned int jk = 0; jk < kernel.cols; jk++) {
			Vec4b v(0,0,0,0);
			unsigned int m = ik + i - kernel.rows/2;
			unsigned int n = jk + j - kernel.cols/2;
			if (m < input.rows && n < input.cols) {
				v = input.ptr<Vec4b>(m)[n];
			} else if(m < input.rows){
				v = input.ptr<Vec4b>(m)[j + (kernel.cols-jk) - kernel.cols/2];
			} else if(n < input.cols){
				v = input.ptr<Vec4b>(i + (kernel.rows-ik) - kernel.rows/2)[n];
			} else {
				v = input.ptr<Vec4b>(i + (kernel.rows-ik) - kernel.rows/2)[j + (kernel.cols-jk) - kernel.cols/2];
			}
			r+=v*kernel.ptr<double>(ik)[jk];
		}
	}

	output.ptr<Vec4b>(i)[j] = r;

}

quilting::BorderKernelBlur::BorderKernelBlur(unsigned int size, double sigma){
	m_kernel = getGaussianKernel(size,sigma);
}

quilting::BorderKernelBlur::BorderKernelBlur(const Mat &kernel):m_kernel(kernel){}

void quilting::BorderKernelBlur::operator()(const Mat & input, Mat & output, int i, int j, int orientation) {
	applyKernel(input,output,m_kernel,i,j);
}
