#pragma once
#include <vector>

#ifdef _DEBUG
#include <iostream>
#define printf(x) \
std::cout<< x << std::endl
#endif

#ifdef _RELEASE
#define printf(x) \
NULL
#endif

namespace RML {
/*
	--DESIRED INTERFACE--
	Creation:
		RML::Matrix<T> m = RML::Matrix<T>(std::vector<unsigned int> d)		d = {dim1, dim2, dim3, ...}		 initialises a matrix of all zeros with the given size. Matrix should not have a public initializer, instead relying on zeros as a factory
	
	Getters:
		T k = m[std::vector<unsigned int> coord]	returns the element at that position. If coord does not lead directly to an element, return the corresponding sub-matrix. T should determine this
		m.size()	display the dimensions of the matrix

	Setters:
		m[std::vector<unsigned int> coord] = T k	set the element at this coordinate. Coordinate must lead to an element of type T
			Array order should be kept sequential if the last element is incremented (I.E {1, 1, 0} should be followed by {1, 1, 1} in arr)
		m.apply(void* rfunct)		apply the function to 


	Operations:
		RML::Matrix::Dot(m, m) should return a new matrix containing the dot product. Should be returned as an a new object
		RML::Matrix::Mul(m, m) should do the same but for matrix multiplication
		m*constant should apply the operation *constant to every element.	Same goes for +, -, /.	Can easily use .apply when implemented

*/



template<class T>
class Matrix {
private:
	std::vector<unsigned int> dim;
	unsigned int arrSize;
	T* arr;

	// indexing functionality
	std::vector<unsigned int> indexCoefficientValues;
	std::vector<unsigned int> calculateIndexCoefficients() {
		std::vector<unsigned int> values(dim.size());
		unsigned int res = 1;
		for (int i = dim.size()-1; i >= 0; i--) {
			values[i] = res;
			res *= dim[i];
		}
		return values;
	}

	unsigned int findCorrespondingIndex(std::vector<unsigned int> idx) {
		if (idx.size() != dim.size()) {		// dimensions do not match
			printf("Dimensions of given index does not match the dimensions of the matrix");
			return -1;
		}
		int index = 0;
		for (int i = dim.size()-1; i >= 0; i--) {
			if (idx[i] >= dim[i]) {
				printf("Given index would attempt to index outside of the array");
				return -1;
			}
			index += idx[i] * indexCoefficientValues[i];
		}
		return index;
	}

public:
	// Constructors / Destructors
	Matrix(std::vector<unsigned int> dim) : dim(dim) {
		arrSize = 1;
		for (int i = 0; i < dim.size(); i++) { arrSize *= dim[i]; }
		arr = new T[arrSize];

		indexCoefficientValues = calculateIndexCoefficients();	// achieves constant look-up and set operations
	}
	~Matrix() {
		delete arr;
	}

	static Matrix<T>* fromImage(T* buffer, unsigned int width, unsigned int height, unsigned int channels) {
		RML::Matrix<T>* m = new RML::Matrix<T>({ width, height, channels });
		int k = 0;
		for (int w = 0; w < width; w++) {
			for (int h = 0; h < height; h++) {
				for (int c = 0; c < channels; c++) {
					m->set({ (unsigned int)w, (unsigned int)h, (unsigned int)c }, buffer[k++]);
				}
			}
		}
		return m;
	}

	// Getters
	T* dump() {
		return arr;
	}

	unsigned int elements() {
		return arrSize;
	}

	std::vector<unsigned int> size() {
		return dim;
	}

	T operator[](std::vector<unsigned int> idx) {
		unsigned int index = findCorrespondingIndex(idx);
		return (index != -1) ? arr[index] : T();
	}

	// Setters
	void set(std::vector<unsigned int> idx, T v) {
		unsigned int index = findCorrespondingIndex(idx);
		if (index != -1) { arr[index] = v; }
		// std::cout << "found index: " << index << " when given {" << idx[0] << ", " << idx[1] << ", " << idx[2] << ", " << idx[3] << "}" << std::endl;
	}

	// NOTE: Order is not preserved after transpose. {a, b} will not be followed by {a, b+1} in the array. However we can achieve constant time by doing this
	void transpose2D() {
		if (dim.size() != 2) {
			printf("Invalid dimensions, only 2d matrices can be transposed");
			return;
		}

		std::vector<unsigned int> oldDim = dim;
		std::vector<unsigned int> oldCoefficients = indexCoefficientValues;
		for (int i = 0; i < dim.size(); i++) {		// will only ever be 2 but looks neater
			dim[i] = oldDim[dim.size() - 1 - i];
			indexCoefficientValues[i] = oldCoefficients[dim.size() - 1 - i];
		}
	}

	void setAll(T v) {
		for (int i = 0; i < arrSize; i++) { arr[i] = v; }
	}

	void apply(void (*f)(void*)) {
		for (int i = 0; i < arrSize; i++) { f(&arr[i]); }
	}

	// DISPLAY, may take out after testing
	void display2D() {
		std::cout << "Dimensions: (" << dim[0] << ", " << dim[1] << ")" << std::endl;
		for (int y = 0; y < dim[1]; y++) {
			std::cout << "[";
			for (int x = 0; x < dim[0]; x++) {
				std::cout << (*this)[{(unsigned int)x, (unsigned int)y}] << ",\t ";
			}
			std::cout << "]" << std::endl;
		}
	}
};

// END FOR NAMESPACE
}