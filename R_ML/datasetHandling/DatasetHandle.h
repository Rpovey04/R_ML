#pragma once
#include "imageFormatting.h"

class DatasetHandle {
private:
	std::string trainingPath;
	std::string testingPath;

	std::unordered_map<std::string, std::vector<std::string>> trainingMap;
	std::unordered_map<std::string, std::vector<std::string>> testingMap;

	static unsigned int STANDARD_WIDTH, STANDARD_HEIGHT;
	static bool STANDARD_GREYSCALE;

	unsigned int editWidth, editHeight;
	bool greyscale;

	static std::vector<std::string> getAvailableDirectories(const std::string fPath) {
		std::wstring stemp = std::wstring(fPath.begin(), fPath.end());
		LPCWSTR folderPath = stemp.c_str();

		WIN32_FIND_DATA findFileData;
		std::wstring tempCast;

		std::vector<std::string> names;

		HANDLE find = FindFirstFile(folderPath, &findFileData);
		if (find) {
			do {
				tempCast = findFileData.cFileName;
				if (tempCast[0] != '.') { names.push_back(std::string(tempCast.begin(), tempCast.end())); }
			} while (FindNextFile(find, &findFileData));
			FindClose(find);
		}
		return names;
	}

	static std::vector<std::string> pathsFrom(const std::string path) {
		std::vector<std::string> potentialPaths = getAvailableDirectories(path);

		std::vector<std::string> res;
		for (int i = 0; i < potentialPaths.size(); i++) {
			// if jpg (only supported type for now)
			if (potentialPaths[i][potentialPaths[i].length() - 1] == 'g' && potentialPaths[i][potentialPaths[i].length() - 2] == 'p' && potentialPaths[i][potentialPaths[i].length() - 3] == 'j') {
				res.push_back(potentialPaths[i]);
			}
		}

		return res;
	}

	static int insertPathsWithLabel(std::string label, const std::string path, std::unordered_map<std::string, std::vector<std::string>>* map) {
		std::vector<std::string> newPaths = pathsFrom(path);
		if (newPaths.size() == 0) {
			printf("Unable to load any data from given path");
			return 0;
		}
		
		auto vec = map->find(label);
		if (vec == map->end()) {
			map->insert(std::pair<std::string, std::vector<std::string>>(label, newPaths));
		}
		else {
			for (int i = 0; i < newPaths.size(); i++) { vec->second.push_back(newPaths[i]); }
		}

		return newPaths.size();
	}

	template <class T>
	static Matrix<T> loadImage(std::string label, int i, std::unordered_map<std::string, std::vector<std::string>> map, std::string basePath, unsigned int width, unsigned int height, bool greyScale) {
		auto elem = map.find(label);
		if (elem == map.end()) {
			printf("No data loaded with given label");
			return Matrix<T>();
		}
		if (i >= elem->second.size()) {
			printf("i is out of range for given label");
			return Matrix<T>();
		}

		std::string path = basePath + "/" + label + "/" + elem->second[i];
		stbi_set_flip_vertically_on_load(1);
		int w, h;
		unsigned char* tempImgBuffer = stbi_load(&path[0], &w, &h, nullptr, 4);

		
		
		
		
		
		Matrix<T> res = imageFormatting<T>::averageBasedResize(tempImgBuffer, w, h, width, height, greyScale);

		delete[] tempImgBuffer;
		return res;
	}

public:
	// constructors (do nothing)
	DatasetHandle(unsigned int w = STANDARD_WIDTH, unsigned int h = STANDARD_HEIGHT, bool g = STANDARD_GREYSCALE) : trainingPath(""), testingPath(""), editWidth(w), editHeight(h), greyscale(g)
	{}
	~DatasetHandle() 
	{}

	// setters
	void setTrainingPath(std::string path) { trainingPath = path; }
	void setTestingPath(std::string path) { testingPath = path; }

	// finds all paths from subDir and saves them in trainingPaths[label] / testPaths[label]. Returns a pair: {training images found, testing images found}
	std::pair<int, int> loadImagesWithLabel(std::string label, const std::string subDir) {
		return {
			insertPathsWithLabel(label, trainingPath + "/" + subDir + "/*", &trainingMap),
			insertPathsWithLabel(label, testingPath + "/" + subDir + "/*", &testingMap)
		};
	}

	// load a new image (only loads 3 channels, alpha channel is ignored)
	template <class T>
	Matrix<T> loadTrainingImage(std::string label, int i) {
		return loadImage<T>(label, i, trainingMap, trainingPath, editWidth, editHeight, greyscale);
	}
	template <class T>
	Matrix<T> loadTestingImage(std::string label, int i) {
		return loadImage<T>(label, i, testingMap, testingPath, editWidth, editHeight, greyscale);
	}

	void setEditWidth(unsigned int w) { editWidth = w; }
	void setEditHeight(unsigned int h) { editHeight = h; }
	void setGreyscale(bool g) { greyscale = g; }
	static void setStandardWidth(unsigned int w) { STANDARD_WIDTH = w; }
	static void setStandardHeight(unsigned int h) { STANDARD_HEIGHT = h; }
	static void setStandardGreyscale(bool g) { STANDARD_GREYSCALE = g; }
};

// completely arbitrary
unsigned int DatasetHandle::STANDARD_WIDTH = 20;
unsigned int DatasetHandle::STANDARD_HEIGHT = 20;
bool DatasetHandle::STANDARD_GREYSCALE = 1;