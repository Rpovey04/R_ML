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