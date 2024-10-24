#pragma once
#include "imageFormatting.h"

class DatasetHandle {
private:
	std::string trainingPath;
	std::string testingPath;

	std::unordered_map<std::string, std::vector<std::string>> trainingMap;
	std::unordered_map<std::string, std::vector<std::string>> testingMap;

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
			if (potentialPaths[i][potentialPaths[i].length() - 1] == 'g' && potentialPaths[i][potentialPaths[i].length() - 2] == 'p' && potentialPaths[i][potentialPaths[i].length() - 3] == 'j') {
				// printf("found path: " << potentialPaths[i]);
				res.push_back(potentialPaths[i]);
			}
		}

		return res;
	}

	static void insertPathsWithLabel(std::string label, const std::string path, std::unordered_map<std::string, std::vector<std::string>>* map) {
		std::vector<std::string> newPaths = pathsFrom(path);
		if (newPaths.size() == 0) {
			printf("Unable to load any data from given path");
			return;
		}
		else {
			// printf("Found " << newPaths.size() << " files for label " << label);
		}

		auto vec = map->find(label);
		if (vec == map->end()) {
			map->insert(std::pair<std::string, std::vector<std::string>>(label, newPaths));
		}
		else {
			for (int i = 0; i < newPaths.size(); i++) { vec->second.push_back(newPaths[i]); }
		}
	}

public:
	// constructors (do nothing)
	DatasetHandle() : trainingPath(""), testingPath("") {}
	~DatasetHandle() {}

	// setters
	void setTrainingPath(std::string path) { trainingPath = path; }
	void setTestingPath(std::string path) { testingPath = path; }

	// finds all paths from subDir and saves them in trainingPaths[label] / testPaths[label]
	void loadImagesWithLabel(std::string label, const std::string subDir) {
		insertPathsWithLabel(label, trainingPath + "/" + subDir + "/*", &trainingMap);
		insertPathsWithLabel(label, testingPath + "/" + subDir + "/*", &testingMap);
	}

	// load a new image (only loads 3 channels, alpha channel is ignored)
	template <class T>
	RML::Matrix<T> loadTrainingImage(std::string label, int i) {
		auto elem = trainingMap.find(label);
		if (elem == trainingMap.end()) {
			printf("No data loaded with given label");
		}
		if (i >= elem->second.size()) {
			// printf(i << " out of range for samples with label " << label);
			printf("i is out of range for given label");
		}

		std::string path = trainingPath + "/" + label + "/" + elem->second[i];
		stbi_set_flip_vertically_on_load(1);
		int w, h;
		unsigned char* tempImgBuffer = stbi_load(&path[0], &w, &h, nullptr, 4);

		// printf(elem->second[i] << " loaded. W: " << w << " H : " << h);
		RML::Matrix<T> res = RML::imageFormatting::averageBasedResize<T>(tempImgBuffer, w, h);
		// printf("Image compressed");

		delete[] tempImgBuffer;
		return res;
	}
	RML::Matrix<unsigned char> loadTestingImage(std::string label, int i) {
		
	}
};