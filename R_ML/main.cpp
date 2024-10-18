#include <iostream>
#include "limsimple/Window.h"
#include "src/Matrix.h"

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

int main() {
	// limSimpleTest();
	RML::Matrix<double> m = RML::Matrix<double>({6,5,4,3});

	// testing for unqiue holding
	int v = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 3; l++) {
					m.set({ (unsigned int)i, (unsigned int)j, (unsigned int)k, (unsigned int) l}, v++);
				}
			}
		}
	}

	v = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 3; l++) {
					if (m[{(unsigned int)i, (unsigned int)j, (unsigned int)k, (unsigned int)l}] != v) {
						std::cout << "error at index: (" << i << ", " << j << ", " << k << "). Expected: " << v << "\tGot: " << m[{(unsigned int)i, (unsigned int)j, (unsigned int)k}] << std::endl;
					}
					v++;
				}
			}
		}
	}

	//m.set({ 2,2,2,2 }, 5);

	double* arr = m.dump();
	std::vector<double> f = {};
	for (int i = 0; i < 6 * 5 * 4 * 3; i++) {
		for (int j = 0; j < f.size(); j++) {
			if (arr[i] == f[j]) { std::cout << "found duplicate at: " << i << "\twith value: " << arr[i] << std::endl; }
		}
		f.push_back(arr[i]);
	}
}