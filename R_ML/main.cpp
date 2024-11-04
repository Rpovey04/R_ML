#include <iostream>
#include <time.h>

#include "limsimple/Window.h"
#include "src/NeuralNetwork.h"
#include "datasetHandling/DatasetHandle.h"
#include "datasetHandling/ubyteReader.h"

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
	DatasetHandle dh(128, 128, 1);
	
	dh.setTestingPath("D:/Datasets/Final_year/Tumours/Testing");
	dh.setTrainingPath("D:/Datasets/Final_year/Tumours/Training");
	
	dh.loadImagesWithLabel("glioma", "glioma");
	dh.loadImagesWithLabel("meningioma", "meningioma");
	dh.loadImagesWithLabel("notumor", "notumor");
	dh.loadImagesWithLabel("pituitary", "pituitary");

	Window myWindow("LimSimple", 256, 256);

	RML::Matrix<double> img;
	unsigned char* imgData;
	int index = 0;
	while (!myWindow.shouldClose()) {
		img = dh.loadTrainingImage<double>("glioma", index);
		imgData = RML::Matrix<double>::toImageFromGreyscale(img);

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

void matrixMultiplicationTest() {
	// Write more test cases for this


	RML::Matrix<int> m1({ 2, 3 });
	m1.set({ 0, 0 }, 1); m1.set({ 0, 1 }, 2); m1.set({ 0, 2 }, 3);
	m1.set({ 1, 0 }, 4); m1.set({ 1, 1 }, 5); m1.set({ 1, 2 }, 6);

	RML::Matrix<int> v1 = RML::Matrix<int>::vector(3);
	v1.set({ 0 }, 10); v1.set({ 1 }, 10); v1.set({ 2 }, 10);

	// RML::Matrix<int> id = RML::Matrix<int>::identity2D(m1.size()[1], 2);
	// RML::Matrix<int> id = m1.clone();
	// id.transpose2D();

	RML::Matrix<int> res = RML::Matrix<int>::matmul2D(m1, v1);
	v1.transpose2D();

	m1.display2D();
	//m2.display2D();
	v1.display2D();
	res.display2D();

	m1.clear();
	v1.clear();
	res.clear();
}

void frontPropTest() {
	// vector of 60000 images
	/*
	std::vector<unsigned char*> Imgdata = ubyteReader::ToChar(60000, 784, ubyteReader::ExtractData(60000, 784, "D:/Datasets/MNIST/train-images-idx3-ubyte/train-images-idx3-ubyte"), 28, 28);
	std::vector<RML::Matrix<unsigned char>> inputs;
	for (int i = 0; i < Imgdata.size(); i++) {
		inputs.push_back(ubyteReader::toMatrix(Imgdata[i], 28, 28));
	}
	std::cout << "Inputs loaded and formatted" << std::endl;

	// labels 0-9
	std::vector<int> labels = ubyteReader::ExtractLables(60000, "D:/Datasets/MNIST/train-labels-idx1-ubyte/train-labels-idx1-ubyte");

	/*
	Window myWindow("LimSimple", 280, 280);
	unsigned char* currentImg;
	std::string pause;
	for (int i = 0; i < 10; i++) {
		currentImg = RML::Matrix<unsigned char>::toImageFromGreyscale(inputs[i]);

		myWindow.displayImage(currentImg, 28, 28, 4);
		myWindow.pollOnce();

		std::cout << labels[i] << std::endl;
		std::cin >> pause;

		delete[] currentImg;
	}
	*/

	// RML::Matrix<double> in = inputs[0].flatten<double>();
	RML::Matrix<double> in = RML::Matrix<double>({ 784, 1 });
	in.randomise();
	// in.display();

	RML::DenseLayer<double> *l1, *l2;
	l1 = new RML::DenseLayer<double>(in.elements(), 20, sigmoid<double>);
	l2 = new RML::DenseLayer<double>(20, 2, sigmoid<double>);

	RML::NeuralNetwork<double> network = RML::NeuralNetwork<double>({ l1, l2 });
	RML::Matrix<double> res1 = network.forward(in);
	
	std::cout << "\nResult: " << std::endl;
	res1.display();
}

int main() {
	srand(time(NULL));
	// limSimpleTest();
	// uniqueTest({ 1,1,1,1 });
	// uniqueTest({ 1,2,3,4 });
	// uniqueTest({ 4,3,2,1 });
	//transposeTest({10, 5});

	// imageTestWithLimsimple();
	// addTest();
	// matrixMultiplicationTest();
	frontPropTest();
}