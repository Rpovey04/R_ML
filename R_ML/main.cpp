#include <iostream>
#include "limsimple/Window.h"
#include "src/Matrix.h"
#include "datasetHandling/DatasetHandle.h"

static void limSimpleTest() {
	std::string p1 = "../../../ExampleTexture/00000001_005.jpg";
	std::string p2 = "../../../ExampleTexture/00000001_000.jpg";

	int ticks = 0;
	Window myWindow("LimSimple", 500, 375);
	myWindow.displayImage(p1);
	while (!myWindow.shouldClose()) {
		if (ticks++ == 100000) {
			std::cout << "switching" << std::endl;
			myWindow.displayImage(p2);
		}
		myWindow.pollOnce();
	}
}

template<class T>
void sigmoid(void* p) {
	T* val = (T*)p;
	T res = (1 / (1 + exp(-*(val)/300)));
	*val = res;
}

template<class T>
void setToOne(void* p) {
	T* val = (T*)p;
	*val = T(1);
}


void uniqueTest(std::vector<unsigned int> dim) {
	RML::Matrix<double> m = RML::Matrix<double>(dim);
	// testing for unqiue holding
	int v = 0;
	for (int i = 0; i < dim[0]; i++) {
		for (int j = 0; j < dim[1]; j++) {
			for (int k = 0; k < dim[2]; k++) {
				for (int l = 0; l < dim[3]; l++) {
					m.set({ (unsigned int)i, (unsigned int)j, (unsigned int)k, (unsigned int)l }, v++);
				}
			}
		}
	}

	v = 0;
	for (int i = 0; i < dim[0]; i++) {
		for (int j = 0; j < dim[1]; j++) {
			for (int k = 0; k < dim[2]; k++) {
				for (int l = 0; l < dim[3]; l++) {
					if (m[{(unsigned int)i, (unsigned int)j, (unsigned int)k, (unsigned int)l}] != v) {
						std::cout << "error at index: (" << i << ", " << j << ", " << k << "). Expected: " << v << "\tGot: " << m[{(unsigned int)i, (unsigned int)j, (unsigned int)k}] << std::endl;
					}
					v++;
				}
			}
		}
	}

	/*
	m.set({ 2,2,2,2 }, 5);
	m.set({ 0,0,0,0 }, 1);
	m.set({ 1,2,3,1 }, 6);
	m.set({ 5, 4, 3, 2 }, 10);
	*/

	double* arr = m.dump();
	std::vector<double> f = {};
	for (int i = 0; i < dim[0] * dim[1] * dim[2] * dim[3]; i++) {
		for (int j = 0; j < f.size(); j++) {
			if (arr[i] == f[j]) { std::cout << "found duplicate at: " << i << "\twith value: " << arr[i] << std::endl; }
		}
		f.push_back(arr[i]);
	}

	// m.apply(&sigmoid<double>);
	// m.apply(&setToOne<double>);

	std::cout << "[";
	for (int i = 0; i < m.elements(); i++) { std::cout << arr[i] << ", "; }
	std::cout << "]" << std::endl;

	std::cout << dim[0] << " " << dim[1] << " " << dim[2] << " " << dim[3] << std::endl;
}

void imageTestWithLimsimple() {
	/*
	std::string path = "../../ExampleTexture/00000001_005.jpg";
	int width, height, BPP;
	unsigned char* tempImgBuffer;
	stbi_set_flip_vertically_on_load(1);
	tempImgBuffer = stbi_load(&path[0], &width, &height, &BPP, 4);

	RML::Matrix<unsigned char> imgMatrix = RML::Matrix<unsigned char>::fromImage(tempImgBuffer, width, height, 4);

	unsigned char* img = imgMatrix.dump();

	Window myWindow("LimSimple", 500, 375);
	myWindow.displayImage(img, width, height, BPP);
	while (!myWindow.shouldClose()) { myWindow.pollOnce(); }

	imgMatrix.clear();
	*/

	DatasetHandle dh;
	dh.setTestingPath("D:/Datasets/Final_year/Tumours/Testing");
	dh.setTrainingPath("D:/Datasets/Final_year/Tumours/Training");

	dh.loadImagesWithLabel("glioma", "glioma");
	dh.loadImagesWithLabel("meningioma", "meningioma");
	dh.loadImagesWithLabel("notumor", "notumor");
	dh.loadImagesWithLabel("pituitary", "pituitary");

	RML::imageFormatting::setDesiredWidth(64);
	RML::imageFormatting::setDesiredHeight(64);
	RML::imageFormatting::setGreyscale(1);

	Window myWindow("LimSimple", 256, 256);

	RML::Matrix<double> img({0});
	unsigned char* imgData;
	int index = 0;
	while (!myWindow.shouldClose()) {
		img = dh.loadTrainingImage<double>("glioma", index);
		imgData = RML::Matrix<double>::toImageGreyscale(img);
		myWindow.displayImage(imgData, img.size()[0], img.size()[1], 4);

		myWindow.pollOnce();

		if (index < 1320) { index += 1; }
		// printf(index);

		if (index % 100 == 0) { std::cout << index << std::endl; }

		img.clear();
		delete[] imgData;
	}
	
}

// needs proper testing
void transposeTest(std::vector<unsigned int> dim) {
	RML::Matrix<int> m = RML::Matrix<int>(dim);
	int v = 0;
	for (int i = 0; i < dim[0]; i++) {
		for (int j = 0; j < dim[1]; j++) {
			m.set({ (unsigned int)i, (unsigned int)j }, v++);
		}
	}

	m.display2D();
	m.transpose2D();
	m.display2D();
	m.transpose2D();
	m.display2D();

	m.clear();
}

void expansionTest(std::vector<unsigned int> dim) {
	
}

void addTest() {
	RML::Matrix<int> m1 = RML::Matrix<int>({4, 4});
	RML::Matrix<int> m2 = RML::Matrix<int>({ 4, 4 });
	int v = 0;
	std::vector<unsigned int> dim = m1.size();
	for (int i = 0; i < dim[0]; i++) {
		for (int j = 0; j < dim[1]; j++) {
			m1.set({ (unsigned int)i, (unsigned int)j }, v);
			m2.set({ (unsigned int)i, (unsigned int)j }, 16+ (v++));
		}
	}

	m1.display2D();
	m2.display2D();

	m2.apply(&setToOne<int>);

	RML::Matrix<int> res = m1 + m2;
	res.display2D();

	res.clear();
	res = m1 / 2;
	res.display2D();

	int sum = 0;
	m1.apply([&sum](int* a) {sum += *a; });
	std::cout << sum << std::endl;
	std::cout << m1.dot(m2) << std::endl;


	m1.clear();
	m2.clear();
	res.clear();
}

void matrixMultiplicationTest() {
	// Write more test cases for this


	RML::Matrix<int> m1({ 2, 3 });
	m1.set({ 0, 0 }, 1); m1.set({ 0, 1 }, 2); m1.set({ 0, 2 }, 3);
	m1.set({ 1, 0 }, 4); m1.set({ 1, 1 }, 5); m1.set({ 1, 2 }, 6);

	RML::Matrix<int> m2({ 3, 2 });
	m2.set({ 0, 0 }, 10); m2.set({ 0, 1 }, 11);
	m2.set({ 1, 0 }, 20); m2.set({ 1, 1 }, 21);
	m2.set({ 2, 0 }, 30); m2.set({ 2, 1 }, 31);

	// RML::Matrix<int> id = RML::Matrix<int>::identity2D(m1.size()[1], 2);
	RML::Matrix<int> id = m1.clone();
	id.transpose2D();

	RML::Matrix<int> res = RML::Matrix<int>::matmul2D(m1, id);

	m1.display2D();
	//m2.display2D();
	id.display2D();
	res.display2D();

	m1.clear();
	m2.clear();
	res.clear();
}

int main() {
	// limSimpleTest();
	// uniqueTest({ 1,1,1,1 });
	// uniqueTest({ 1,2,3,4 });
	// uniqueTest({ 4,3,2,1 });
	//transposeTest({10, 5});

	imageTestWithLimsimple();
	// addTest();
	// matrixMultiplicationTest();

}