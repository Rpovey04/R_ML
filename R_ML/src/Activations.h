#include "Matrix.h"

template<class T>
T ReLU(T v) {
	if (v < 0) { return 0; }
	else { return v; }
}

template<class T>
T leakyReLU(T v) {
	if (v < 0) { return v / 10; }
	else { return v; }
}