#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <windows.h>

#include "../src/Matrix.h"

template <class T>
class imageFormatting {
private:
	static int vectorAverage(std::vector<unsigned char> v) {
		int sum = 0;
		for (int i = 0; i < v.size(); i++) { sum += v[i]; }
		return sum;
	}

	static std::vector<unsigned char>* calculatePixelSums(unsigned char* imgSrc, int originalWidth, int originalHeight, unsigned int newWidth, unsigned int newHeight, bool greyscale) {
		std::vector<unsigned char>* imgSums;
		if (greyscale) { imgSums = new std::vector<unsigned char>[newWidth * newHeight]; }
		else { imgSums = new std::vector<unsigned char>[newWidth * newHeight * 3]; }	// still ignores alpha channels

		int oldIndex, newIndex, newX, newY, rgbAverage;
		int hComponent;
		double currentYRatio;
		for (int h = 0; h < originalHeight; h++) {
			newY = ((double)h / (double)originalHeight) * newHeight;
			for (int w = 0; w < originalWidth; w++) {
				oldIndex = h * originalWidth + w;
				newX = newWidth * ((double)w / (double)originalWidth);
				newIndex = newY * newWidth + newX;
				if (greyscale) {
					rgbAverage = (imgSrc[oldIndex * 4] + imgSrc[oldIndex * 4 + 1] + imgSrc[oldIndex * 4 + 2]) / 3;
					imgSums[newIndex].push_back((unsigned char)rgbAverage);
				}
				else {
					imgSums[newIndex * 3].push_back(imgSrc[oldIndex * 4]);
					imgSums[newIndex * 3 + 1].push_back(imgSrc[oldIndex * 4 + 1]);
					imgSums[newIndex * 3 + 2].push_back(imgSrc[oldIndex * 4 + 2]);
				}
			}
		}

		return imgSums;
	}

	static RML::Matrix<T> takeAverages(std::vector<unsigned char>* pixelSums, unsigned int width, unsigned int height, bool greyscale, std::vector<unsigned int> dim) {
		RML::Matrix<T> res = RML::Matrix<T>(dim);

		int currentIndex = 0;
		for (unsigned int w = 0; w < width; w++) {
			for (unsigned int h = 0; h < height; h++) {
				if (greyscale) {
					res.set({ w, h }, vectorAverage(pixelSums[currentIndex]));
				}
				else {
					res.set({ w, h, 0 }, (T)vectorAverage(pixelSums[currentIndex * 3]));
					res.set({ w, h, 1 }, (T)vectorAverage(pixelSums[currentIndex * 3 + 1]));
					res.set({ w, h, 2 }, (T)vectorAverage(pixelSums[currentIndex * 3 + 2]));
				}
				currentIndex += 1;
			}
		}

		return res;
	}

public:
	static RML::Matrix<T> averageBasedResize(unsigned char* imgSrc, int originalWidth, int originalHeight, unsigned int newWidth, unsigned int newHeight, bool greyscale) {
		std::vector<unsigned char>* pixelSums = calculatePixelSums(imgSrc, originalWidth, originalHeight, newWidth, newHeight, greyscale);

		std::vector<unsigned int> newDim = { newWidth, newHeight };
		if (!greyscale) { newDim.push_back(3); }		// add colour channels
		RML::Matrix<T> res = takeAverages(pixelSums, newWidth, newHeight, greyscale, newDim);

		delete[] pixelSums;
		return res;
	}
};