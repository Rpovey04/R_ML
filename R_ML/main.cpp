#include <iostream>
#include "limsimple/Window.h"

int main() {
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