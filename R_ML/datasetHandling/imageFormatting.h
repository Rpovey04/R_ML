#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <windows.h>

#include "../src/Matrix.h"

namespace RML {

class imageFormatting {
private:
	static unsigned int newWidth;
	static unsigned int newHeight;
	static bool greyScale;

public:
	template <class T>
	static RML::Matrix<T> averageBasedResize(unsigned char* imgSrc, int originalWidth, int originalHeight) {
		std::vector<unsigned char>* imgSums;
		if (greyScale) { imgSums = new std::vector<unsigned char>[newWidth * newHeight]; }
		else { imgSums = new std::vector<unsigned char>[newWidth * newHeight * 3]; }
		
		int oldIndex, newIndex, newX, newY;
		int hComponent;
		double currentYRatio;
		for (int h = 0; h < originalHeight; h++) {
			newY = ((double)h / (double)originalHeight) * newHeight;
			for (int w = 0; w < originalWidth; w++) {
				oldIndex = h * originalWidth + w;
				newX = newWidth * ((double)w / (double)originalWidth);
				newIndex = newY * newWidth + newX;
				if (greyScale) {
					imgSums[newIndex].push_back(imgSrc[oldIndex * 4]);		// rgb average
				}
				else {
					imgSums[newIndex * 3].push_back(imgSrc[oldIndex * 4]);
					imgSums[newIndex * 3 + 1].push_back(imgSrc[oldIndex * 4 + 1]);
					imgSums[newIndex * 3 + 2].push_back(imgSrc[oldIndex * 4 + 2]);
				}
			}
		}
		// printf("End of sums reached");

		std::vector<unsigned int> newDim = { newWidth, newHeight };
		if (!greyScale) { newDim.push_back(3); }		// add colour channels
		RML::Matrix<T> res(newDim);
		int count = 0;
		int currentIndex = 0;
		for (unsigned int w = 0; w < newWidth; w++) {
			for (unsigned int h = 0; h < newHeight; h++) {
				count = 0;
				if (!greyScale) {
					for (unsigned int c = 0; c < 3; c++) {
						if (imgSums[currentIndex].size() > 0) {
							for (int i = 0; i < imgSums[currentIndex].size(); i++) { count += imgSums[currentIndex][i]; }
						}
						res.set({ w, h, c }, (T)((double)count / (double)imgSums[currentIndex].size()));
						currentIndex += 1;
					}
				}
				else {
					if (imgSums[currentIndex].size() > 0) {
						for (int i = 0; i < imgSums[currentIndex].size(); i++) { count += imgSums[currentIndex][i]; }
					}
					res.set({ w, h }, (T)((double)count / (double)imgSums[currentIndex].size()));
					currentIndex += 1;
				}
			}
		}
		delete[] imgSums;
		return res;
	}

	static void setDesiredWidth(int w) { newWidth = w; }
	static void setDesiredHeight(int w) { newHeight = w; }
	static void setGreyscale(bool v = 1) { greyScale = v; }
};

unsigned int imageFormatting::newWidth = 0;
unsigned int imageFormatting::newHeight = 0;
bool imageFormatting::greyScale = 1;


// namespace end
}