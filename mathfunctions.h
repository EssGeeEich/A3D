#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H
#include <vector>

std::vector<float> generateGaussianKernel(int size, float sigma);
std::vector<float> generateGaussianEdgeKernel(int size, float power);

#endif
