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
	MAY CHANGE HOW VECTORS WORK
*/

template<class T>
class Matrix {
private:
	std::vector<unsigned int> dim;
	unsigned int arrSize;
	T* arr;

	bool isVector;

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
		if (idx.size() != dim.size()) {
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

	// Helper function for +, -, *, / operations
	template <typename T1>
	Matrix<T> applyOnCorrespondingMat(Matrix<T>& m, T1 fn) {
		if (!sameSize(m)) {return Matrix<T>({ 1 });}

		Matrix<T> res(dim);
		T* resArr = res.dump();
		T* mArr = m.dump();
		for (int i = 0; i < arrSize; i++) {
			resArr[i] = fn(arr[i], mArr[i]);
		}
		return res;
	}

public:
	// Constructors / Destructors
	Matrix() : dim({0}), arrSize(0)
	{}
	Matrix(std::vector<unsigned int> dim, bool v = 0) : dim(dim), isVector(v) {
		arrSize = 1;
		for (int i = 0; i < dim.size(); i++) { arrSize *= dim[i]; }
		arr = new T[arrSize];

		indexCoefficientValues = calculateIndexCoefficients();	// achieves constant look-up and set operations
	}
	~Matrix() {
		// chose not to delete the underlying array when object is deleted. This is usually done when the object is passed back, making allocation messy
		// delete[] arr;
	}

	// Creation functions: Create a certain mathmatical matrix or load from a datatype
	static Matrix<T> fromImage(T* buffer, unsigned int width, unsigned int height, unsigned int channels) {
		RML::Matrix<T> m({ width, height, channels });
		int k = 0;
		for (int w = 0; w < width; w++) {
			for (int h = 0; h < height; h++) {
				for (int c = 0; c < channels; c++) {
					m.set({ (unsigned int)w, (unsigned int)h, (unsigned int)c }, buffer[k++]);
				}
			}
		}
		return m;
	}

	// returns a four channeled image based on a greyscale matrix
	static unsigned char* toImageFromGreyscale(RML::Matrix<T> m) {
		T* imgData = m.dump();
		unsigned char* img4Channel = new unsigned char[m.elements() * 4];
		for (int i = 0; i < m.elements(); i++) {
			for (int c = 0; c < 4; c++) {
				img4Channel[i * 4 + c] = (unsigned char)imgData[i];
			}
		}
		return img4Channel;
	}

	static Matrix<T> identity2D(unsigned int n, T v = T(1)) {	// inclussion of v to allow for faster setup of multiplication
		Matrix<T> m({ n, n });
		m.apply([](T* a) {(*a) = T(0); });
		for (unsigned int i = 0; i < n; i++) { m.set({ i, i }, v); }
		return m;
	}

	static Matrix<T> vector(unsigned int n) {
		return RML::Matrix<T>({ n, 1 }, 1);
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

	bool sameSize(Matrix<T>& m) {
		if (m.size().size() == dim.size()) {
			for (int i = 0; i < dim.size(); i++) {
				if (m.size()[i] != dim[i]) {
					return 0;
				}
			}
		}
		return 1;
	}

	bool isVec() {
		return isVector;
	}

	T operator[](std::vector<unsigned int> idx) {
		if (isVector && idx.size() == 1) { idx.push_back(0); }
		unsigned int index = findCorrespondingIndex(idx);
		return (index != -1) ? arr[index] : T();
	}

	Matrix<T> clone() {
		Matrix<T> res(dim, isVector);
		T* resArr = res.dump();
		for (int i = 0; i < arrSize; i++) {
			resArr[i] = arr[i];
		}
		return res;
	}

	// Mathmatical matrix operations
	Matrix<T> operator+(Matrix<T>& m) {
		return applyOnCorrespondingMat(m, [](T a, T b) {return a + b; });
	}
	Matrix<T> operator-(Matrix<T>& m) {
		return applyOnCorrespondingMat(m, [](T a, T b) {return a - b; });
	}
	Matrix<T> operator/(Matrix<T>& m) {
		return applyOnCorrespondingMat(m, [](T a, T b) {return a / b; });
	}
	Matrix<T> operator*(Matrix<T>& m) {		// in place multiplication. Only works on same sized matrixes
		return applyOnCorrespondingMat(m, [](T a, T b) {return a * b; });
	}
	Matrix<T> operator+(T v) {
		Matrix<T> res = clone();
		res.apply([v](T* a) {(*a) += v; });
		return res;
	}
	Matrix<T> operator-(T v) {
		Matrix<T> res = clone();
		res.apply([v](T* a) {(*a) -=v; });
		return res;
	}
	Matrix<T> operator/(T v) {
		Matrix<T> res = clone();
		res.apply([v](T* a) {(*a) /= v; });
		return res;
	}
	Matrix<T> operator*(T v) {
		Matrix<T> res = clone();
		res.apply([v](T* a) {(*a) *= v; });
		return res;
	}
	T dot(Matrix<T>& m) {
		if (!sameSize(m)) { return T(-1); }

		T sum = T(0);
		T* mArr = m.dump();
		for (int i = 0; i < arrSize; i++) { sum += arr[i] * mArr[i]; }
		return sum;
	}
	static T dot(Matrix<T>& m1, Matrix<T>& m2) {
		return m1.dot(m2);
	}

	Matrix<T> matmul2D(Matrix<T>& m) {
		std::vector<unsigned int> mDim = m.size();
		if (dim.size() != 2 || mDim.size() != 2) { 
			printf("Multiplication must be between two 2D matricies"); 
			return Matrix<T>({ 0 });
		}
		if (dim[1] != mDim[0]) {
			printf("Second dimension of the first matrix should match the first dimension of the second matrix");
			return Matrix<T>({ 0 });
		}

		std::vector<unsigned int> resDim = { dim[0], mDim[1] };
		Matrix<T> res(resDim);
		T sum;
		for (unsigned int i = 0; i < resDim[0]; i++) {
			for (unsigned int j = 0; j < resDim[1]; j++) {
				sum = T(0);
				for (unsigned int k = 0; k < dim[1]; k++) {
					sum += (*this)[{i, k}] * m[{k, j}];
				}
				res.set({ i, j }, sum);
			}
		}
		return res;
	}
	static Matrix<T> matmul2D(Matrix<T>& m1, Matrix<T>& m2) {
		return m1.matmul2D(m2);
	}

	// Setters
	void set(std::vector<unsigned int> idx, T v) {
		if (isVector && idx.size() == 1) { idx.push_back(0); }
		unsigned int index = findCorrespondingIndex(idx);
		if (index != -1) { arr[index] = v; }
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

	template <typename T1>
	void apply(T1 f) {
		for (int i = 0; i < arrSize; i++) { f(&arr[i]); }
	}

	// DISPLAY, may take out after testing
	void display2D() {
		std::cout << "Dimensions: (" << dim[0] << ", " << dim[1] << ")" << std::endl;
		for (int x = 0; x < dim[0]; x++) {
			std::cout << "[";
			for (int y = 0; y < dim[1]; y++) {
				std::cout << (*this)[{(unsigned int)x, (unsigned int)y}] << ",\t ";
			}
			std::cout << "]" << std::endl;
		}
	}

	// should only be called before the entire array is deleted
	void clear() {
		delete[] arr;
	}
};

// END FOR NAMESPACE
}