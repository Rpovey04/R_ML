#include "Matrix.h"

template<class T>
void ReLU(T* v) {
	T val = *v;
	if (val < 0) { *v = 0; }
}

template<class T>
void leakyReLU(T* v) {
	T val = *v;
	if (val < 0) { (*v) =  val / 10; }
}