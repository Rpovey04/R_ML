#include "Matrix.h"
#include <cmath>

template<class T>
void ReLU(T* v) {
	if ((*v) < 0) { (*v) = 0; }
}

template<class T>
void leakyReLU(T* v) {
	if ((*v) < 0) { (*v) = (*v) / 10; }
}

template<class T>
void sigmoid(T* v) {
	(*v) = 1 / (1 + std::exp(-(*v)));
}

template<class T>
void sigmoidGrad(T* v) {
	T v1 = (*v);
	sigmoid(&v1);
	(*v) = v1 * (1 - v1);
}