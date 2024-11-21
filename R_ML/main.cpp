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
	Matrix<double> m = Matrix<double>(dim);
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

	Matrix<double> img;
	unsigned char* imgData;
	int index = 0;
	while (!myWindow.shouldClose()) {
		img = dh.loadTrainingImage<double>("glioma", index);
		imgData = Matrix<double>::toImageFromGreyscale(img);

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
	Matrix<int> m = Matrix<int>(dim);
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


	Matrix<int> m1({ 2, 3 });
	m1.set({ 0, 0 }, 1); m1.set({ 0, 1 }, 2); m1.set({ 0, 2 }, 3);
	m1.set({ 1, 0 }, 4); m1.set({ 1, 1 }, 5); m1.set({ 1, 2 }, 6);

	Matrix<int> v1 = Matrix<int>::vector(3);
	v1.set({ 0 }, 10); v1.set({ 1 }, 10); v1.set({ 2 }, 10);

	// Matrix<int> id = Matrix<int>::identity2D(m1.size()[1], 2);
	// Matrix<int> id = m1.clone();
	// id.transpose2D();

	Matrix<int> res = Matrix<int>::matmul2D(m1, v1);
	v1.transpose2D();

	m1.display2D();
	//m2.display2D();
	v1.display2D();
	res.display2D();

	m1.clear();
	v1.clear();
	res.clear();
}

Matrix<double> calculateLoss(Matrix<double> pred, int y) {
	Matrix<double> loss = Matrix<double>({ 10, 1 });
	double v = 0;
	for (unsigned int i = 0; i < 10; i++) {
		if (i == (unsigned int)y) { v = pred[{i, 0}] - 1; }
		else { v = pred[{i, 0}]; }		
		loss.set({ i, 0 }, v*2);
	}
	return loss;
}

int isCorrect(Matrix<double> pred, int y) {
	double max = 0;
	int chosen = -1;
	for (unsigned int i = 0; i < 10; i++) {
		if (pred[{i, 0}] >= max) {
			chosen = i;
			max = pred[{i, 0}];
		}
	}
	return chosen == y;
}

int outputToPred(Matrix<double> output) {
	double max = 0;
	int chosen = -1;
	for (unsigned int i = 0; i < 10; i++) {
		if (output[{i, 0}] >= max) {
			chosen = i;
			max = output[{i, 0}];
		}
	}
	return chosen;
}

void frontPropTest() {
	/*
		THIS MODEL ACHIEVES AVERAGE ACCURACY OF 71.7%
	*/

	// vector of 60000 images
	std::vector<unsigned char*> Imgdata = ubyteReader::ToChar(60000, 784, ubyteReader::ExtractData(60000, 784, "D:/Datasets/MNIST/train-images-idx3-ubyte/train-images-idx3-ubyte"), 28, 28);
	std::vector<Matrix<unsigned char>> inputs;
	for (int i = 0; i < Imgdata.size(); i++) {
		inputs.push_back(ubyteReader::toMatrix(Imgdata[i], 28, 28));
	}
	std::cout << "Inputs loaded and formatted" << std::endl;

	// labels 0-9
	std::vector<int> labels = ubyteReader::ExtractLables(60000, "D:/Datasets/MNIST/train-labels-idx1-ubyte/train-labels-idx1-ubyte");
	
	// Model definition
	Matrix<double> in = inputs[0].flatten<double>();
	DenseLayer<double>* l1, * l2, * l3, * l4;
	l1 = new DenseLayer<double>(in.elements(), 60, sigmoid<double>, sigmoidGrad<double>);
	l3 = new DenseLayer<double>(60, 28, sigmoid<double>, sigmoidGrad<double>);
	l4 = new DenseLayer<double>(28, 10, sigmoid<double>, sigmoidGrad<double>);
	NeuralNetwork<double> network = NeuralNetwork<double>({ l1, l3, l4 }, 1, 0.1);

	// training
	int egs = 55000;
	int epochs = 1;
	int total = 0;
	int numCorrect = 0;
	int interval = 10;

	std::vector<Matrix<double>> currentInputs;
	std::vector<int> currentLabels;

	for (int e = 0; e < epochs; e++) {
		std::cout << "Epoch: " << e << std::endl;
		for (int i = 0; i < egs; i++) {
			if (i % 500 == 0) {
				std::cout << ((double)i / (double)egs) * 100.0f << "%" << std::endl;
				std::cout << "interval accuracy: " << (double)numCorrect / (double)total << std::endl;
				numCorrect = 0; total = 0;
			}

			currentInputs.push_back(inputs[i].flatten<double>());
			currentLabels.push_back(labels[i]);
			total += 1;

			if (i % interval == 0 && i > 0) {
				numCorrect += network.runGradientDescent<int>(currentInputs, currentLabels, calculateLoss, isCorrect);
				while (!currentInputs.empty()) {		// empty input vector
					currentInputs[currentInputs.size() - 1].clear();
					currentInputs.pop_back();
				}
				currentLabels.clear();
			}
		}
	}

	// testing
	unsigned char* currentImg;
	Window myWindow("LimSimple", 280, 280);
	std::string pause;
	total = 0;
	numCorrect = 0;

	Matrix<double> pred;
	for (int i = 55000; i < 60000; i++) {
		currentImg = Matrix<unsigned char>::toImageFromGreyscale(inputs[i]);
		myWindow.displayImage(currentImg, 28, 28, 4);
		myWindow.pollOnce();
		in = inputs[i].flatten<double>();
		pred = network.forward(in);
		

		total += 1;
		if (outputToPred(pred) == labels[i]) { numCorrect += 1; }
		if (i % 100 == 0) {
			std::cout << "running accuracy: " << (double)numCorrect / (double)total << std::endl;
		}

		/*
		std::cout << "Pred: " << outputToPred(pred) << "\tReal: " << labels[i] << std::endl;	
		/*
		std::cin >> pause;
		*/
		

		in.clear();
		pred.clear();
		delete[] currentImg;
	}
}

int main() {
	srand(time(NULL));
	frontPropTest();
	imageTestWithLimsimple();
}