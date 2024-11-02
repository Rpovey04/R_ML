#include "DenseLayer.h"

namespace RML {

template<class T>
class NeuralNetwork {
private:
	std::vector<RML::Layer<T>*> layers;

public:
	NeuralNetwork() {}
	NeuralNetwork(std::vector<RML::Layer<T>*> l) : layers(l) {
		
	}
	~NeuralNetwork()
	{}

	RML::Matrix<T> forward(RML::Matrix<T> input) {
		RML::Matrix<T> newInput = input;
		RML::Matrix<T> res;
		for (int i = 0; i < layers.size(); i++) {
			res = layers[i]->forward(newInput);		// will move activation to here, works better for activations that consider the whole set

			newInput.clear();
			newInput = res;
		}
		return res;
	}
};

};