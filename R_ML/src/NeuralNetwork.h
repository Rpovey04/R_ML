#include "DenseLayer.h"

namespace RML {

template<class T>
class NeuralNetwork {
private:
	unsigned int nLayers;
	std::vector<RML::Layer<T>*> layers;

	bool shouldCache;
	std::vector<RML::Matrix<T>> cachedSums;
	std::vector<RML::Matrix<T>> cachedActivations;

	double learningRate;
public:
	NeuralNetwork() {}
	NeuralNetwork(std::vector<RML::Layer<T>*> l, bool cache = 1, double lr=0.1) : layers(l), shouldCache(cache), learningRate(lr) {
		nLayers = layers.size();
		for (int i = 0; i < nLayers + 1; i++) {
			cachedSums.push_back(RML::Matrix<T>());		// cachedSums[0] will always be empty
			cachedActivations.push_back(RML::Matrix<T>());
		}
	}
	~NeuralNetwork() {
		for (int i = 0; i < nLayers + 1; i++) {
			cachedSums[i].clear();
			cachedActivations[i].clear();
		}
	}

	RML::Matrix<T> forward(RML::Matrix<T> input) {
		RML::Matrix<T> newInput = input.clone();		// allows for manipulation of this layer, including caching
		if (shouldCache) {
			cachedActivations[0].clear();
			cachedActivations[0] = newInput.clone();
		}
		
		RML::Matrix<T> res;
		for (int i = 0; i < nLayers; i++) {
			// get sum from layer
			res = layers[i]->forward(newInput);
			if (shouldCache) {
				cachedSums[i + 1].clear();
				cachedSums[i + 1] = res.clone();
			}

			// apply activation
			res.apply(layers[i]->getActivation());
			if (shouldCache) {
				cachedActivations[i + 1].clear();
				cachedActivations[i + 1] = res.clone();		// clone used in both cases so that cache logic doesn't effect main logic
			}
			
			newInput.clear();
			newInput = res;
		}
		return res;
	}

	std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>> backward(RML::Matrix<T> cost) {	// returns vector of {bias gradients, weight gradients}
		if (!shouldCache) {
			printf("Caching is disabled, backprop not possible");
			return std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>>();
		}
		if (!cachedActivations[nLayers].sameSize(cost)) {
			printf("Dimensions of cost do not match dimensions of final layer output");
			return std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>>();
		}

		
		std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>> gradients = std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>>(nLayers);
		
		RML::Matrix<T> sumGradient, activationGradient, costGradient;
		RML::Matrix<T> weightGrad, biasGrad;

		costGradient = cost.clone();
		for (int l = nLayers - 1; l >= 0; l--) {	
			sumGradient = cachedActivations[l];		// weird indexing for this but l is the previous layer (think l = 0, cachedActivations[l] = input layer)
			
			activationGradient = cachedSums[l+1];				// same again for this
			activationGradient.apply(layers[l]->getActivationGrad());

			biasGrad = activationGradient * costGradient;
			
			biasGrad.transpose2D();
			weightGrad = sumGradient.matmul2D(biasGrad);			
			biasGrad.transpose2D();

			gradients[l] = std::pair<RML::Matrix<double>, RML::Matrix<double>>(biasGrad, weightGrad);

			costGradient.clear();
			costGradient = layers[l]->getWeights().matmul2D(biasGrad);

			/*
			std::cout << "l: " << l << std::endl;
			std::cout << "cost: " << std::endl;
			costGradient.displayDimensions();
			std::cout << "sum: " << std::endl;
			sumGradient.displayDimensions();
			std::cout << "activation: " << std::endl;
			activationGradient.displayDimensions();
			std::cout << "weights: " << std::endl;
			layers[l]->getWeights().displayDimensions();
			std::cout << "weight gradients: " << std::endl;
			weightGrad.displayDimensions();
			std::cout << "bias: " << std::endl;
			layers[l]->getBias().displayDimensions();
			std::cout << "bias gradients: " << std::endl;
			biasGrad.displayDimensions();
			std::cout << "\n\n" << std::endl;
			*/
		}
		costGradient.clear();
		return gradients;
	}

	void applyGradients(std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>> gradients) {
		double lr = learningRate;
		for (int l = 0; l < nLayers; l++) {
			gradients[l].first.apply([lr](T* v) {(*v) *= lr; });
			gradients[l].second.apply([lr](T* v) {(*v) *= lr; });
			layers[l]->applyGradients(gradients[l].first, gradients[l].second);
		}
	}
};

};