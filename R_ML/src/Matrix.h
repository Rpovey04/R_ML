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
*/



template<class T>
class Matrix {
private:
	std::vector<unsigned int> dim;
	unsigned int arrSize;
	T* arr;

	// indexing functionality
	unsigned int indexCoefficientForPosition(unsigned int idx) {
		unsigned int res = 1;
		for (int i = 0; i < idx; i++) {
			res *= dim[i];
		}
		return res;
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
			index += idx[i] * indexCoefficientForPosition(i);
		}
		return index;
	}

public:
	// Constructors / Destructors
	Matrix(std::vector<unsigned int> dim) : dim(dim) {
		arrSize = 1;
		for (int i = 0; i < dim.size(); i++) { arrSize *= dim[i]; }
		arr = new T[arrSize];
	}
	~Matrix() {
		delete arr;
	}

	// Getters
	T operator[](std::vector<unsigned int> idx) {
		unsigned int index = findCorrespondingIndex(idx);
		return (index != -1) ? arr[index] : T();
	}

	// Setters
	void set(std::vector<unsigned int> idx, T v) {
		unsigned int index = findCorrespondingIndex(idx);
		if (index == -1) { return; }
		
		// std::cout << "found index: " << index << " when given {" << idx[0] << ", " << idx[1] << ", " << idx[2] << "}" << std::endl;

		arr[index] = v;
	}

	void setAll(T v) {
		for (int i = 0; i < arrSize; i++) { arr[i] = v; }
	}

	T* dump() {
		return arr;
	}
};

// END FOR NAMESPACE
}