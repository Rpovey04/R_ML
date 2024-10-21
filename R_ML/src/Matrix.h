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
		std::vector<unsigned int> values;
		unsigned int res = 1;
		for (int i = 0; i < dim.size(); i++) {
			values.push_back(res);
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
		for (int i = 0; i < dim.size(); i++) {
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
		// read image data backwards
		for (int c = 0; c < channels; c++) {
			for (int h = 0; h < height; h++) {
				for (int w = 0; w < width; w++) {
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

	// interprets data as if it were input as an image using the dimensions {width, height, channel}

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
		// std::cout << "found index: " << index << " when given {" << idx[0] << ", " << idx[1] << ", " << idx[2] << "}" << std::endl;
	}

	void setAll(T v) {
		for (int i = 0; i < arrSize; i++) { arr[i] = v; }
	}

	void apply(void (*f)(void*)) {
		for (int i = 0; i < arrSize; i++) { f(&arr[i]); }
	}
};

// END FOR NAMESPACE
}