#include <iostream>
#include <fstream>
#include <vector>
#include "../src/Matrix.h"

/*
    OLD CODE FROM A PREVIOUS PROJECT
*/

using namespace std;

class ubyteReader {
public:
    static int ReverseInt(int i)
    {
        unsigned char ch1, ch2, ch3, ch4;
        ch1 = i & 255;
        ch2 = (i >> 8) & 255;
        ch3 = (i >> 16) & 255;
        ch4 = (i >> 24) & 255;
        return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
    }
    static void ReadMNIST(int NumberOfImages, int DataOfAnImage, vector<vector<double>>& arr, std::string path)
    {
        arr.resize(NumberOfImages, vector<double>(DataOfAnImage));
        ifstream file(path, ios::binary);
        if (file.is_open())
        {
            int magic_number = 0;
            int number_of_images = 0;
            int n_rows = 0;
            int n_cols = 0;
            file.read((char*)&magic_number, sizeof(magic_number));
            magic_number = ReverseInt(magic_number);
            file.read((char*)&number_of_images, sizeof(number_of_images));
            number_of_images = ReverseInt(number_of_images);
            file.read((char*)&n_rows, sizeof(n_rows));
            n_rows = ReverseInt(n_rows);
            file.read((char*)&n_cols, sizeof(n_cols));
            n_cols = ReverseInt(n_cols);
            for (int i = 0; i < number_of_images; ++i)
            {
                for (int r = 0; r < n_rows; ++r)
                {
                    for (int c = 0; c < n_cols; ++c)
                    {
                        unsigned char temp = 0;
                        file.read((char*)&temp, sizeof(temp));
                        arr[i][(n_rows * r) + c] = (double)temp;
                    }
                }
            }
        }
    }


    static std::vector<std::vector<double>> ExtractData(int numItems, int pixels, std::string path) {
        std::vector<std::vector<double>> temp;
        std::cout << "Loading from [" << path << "]..." << std::endl;
        ReadMNIST(numItems, pixels, temp, path);
        std::cout << "Done!" << std::endl;
        return temp;
    }

    static std::vector<unsigned char*> ToChar(int numItems, int pixels, std::vector<std::vector<double>> D, int width, int height) {
        std::vector<unsigned char*>newVec;
        int index, p;
        for (int i = 0; i < numItems; i++) {
            newVec.push_back(new unsigned char[pixels * 4]);
            for (int w = 0; w < width; w++) {
                for (int h = 0; h < height; h++) {
                    p = (w * height) + h;
                    index = ((pixels - 1) - ((w * height) + (height - h))) * 4;

                    newVec[i][index] = (D[i][p]);
                    newVec[i][index + 1] = (D[i][p]);
                    newVec[i][index + 2] = (D[i][p]);
                    newVec[i][index + 3] = (255);
                }
            }
        }
        return newVec;
    }

    static std::vector<int> ExtractLables(int numItems, std::string path) {
        std::cout << "Loading from [" << path << "]..." << std::endl;
        std::ifstream file(path, ios::binary);
        std::vector<int> newVec;

        int magic_number = 0;
        int number_of_lables = 0;

        if (file.is_open()) {
            file.read((char*)&magic_number, sizeof(magic_number));
            magic_number = ReverseInt(magic_number);
            file.read((char*)&number_of_lables, sizeof(number_of_lables));
            number_of_lables = ReverseInt(number_of_lables);
            for (int i = 0; i < number_of_lables; i++) {
                unsigned char temp = 0;
                file.read((char*)&temp, sizeof(temp));
                newVec.push_back(int(temp));
                // std::cout << int(temp) << std::endl;
            }
        }
        std::cout << "Done!" << std::endl;
        return newVec;
    }

    static RML::Matrix<unsigned char> toMatrix(unsigned char* imgData, int w, int h) {
        RML::Matrix<unsigned char> res = RML::Matrix<unsigned char>({ (unsigned int)w, (unsigned int)h });
        int index = 0;
        for (unsigned int i = 0; i < w; i++) {
            for (unsigned int j = 0; j < h; j++) {
                res.set({ i, j }, imgData[4*index++]);
            }
        }
        return res;
    }
};