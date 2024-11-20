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
	int gradientCount;
	std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>> runningGradient;		// formatted as <biasGradient, weightGradient> pairs
	
public:
	NeuralNetwork() {}
	NeuralNetwork(std::vector<RML::Layer<T>*> l, bool cache = 1, double lr = 0.1) : layers(l), shouldCache(cache), learningRate(lr) {
		nLayers = layers.size();
		for (int i = 0; i < nLayers + 1; i++) {
			cachedSums.push_back(RML::Matrix<T>());		// cachedSums[0] will always be empty
			cachedActivations.push_back(RML::Matrix<T>());
		}

		gradientCount = 0;
		RML::Matrix<double> currentWeightGrad, currentBiasGrad;
		for (int i = 0; i < nLayers; i++) {
			currentWeightGrad = RML::Matrix<double>(l[i]->getDimensions());
			currentWeightGrad.apply([](double* v) {(*v) = 0; });
			currentBiasGrad = RML::Matrix<double>({ l[i]->getDimensions()[1], 1 });
			currentBiasGrad.apply([](double* v) {(*v) = 0; });
			runningGradient.push_back(std::pair<RML::Matrix<double>, RML::Matrix<double>>(currentBiasGrad, currentWeightGrad));
		}
	}
	~NeuralNetwork() {
		for (int i = 0; i < nLayers + 1; i++) {
			cachedSums[i].clear();
			cachedActivations[i].clear();

			if (i < nLayers) {
				runningGradient[i].first.clear();
				runningGradient[i].second.clear();
			}
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
			
			// Still need to check if this is the same calculation I used in my last project but I'm fairly sure it is
			biasGrad.transpose2D();
			weightGrad = sumGradient.matmul2D(biasGrad);			
			biasGrad.transpose2D();

			gradients[l] = std::pair<RML::Matrix<double>, RML::Matrix<double>>(biasGrad, weightGrad);

			costGradient.clear();
			costGradient = layers[l]->getWeights().matmul2D(biasGrad);
		}
		costGradient.clear();
		return gradients;
	}

	void addRunningGradients(std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>> gradient, bool clear = 1) {
		for (int l = 0; l < nLayers; l++) {
			runningGradient[l].first += gradient[l].first;
			runningGradient[l].second += gradient[l].second;
			if (clear) {
				gradient[l].first.clear();
				gradient[l].second.clear();
			}
		}

		gradientCount += 1;
	}
	
	void applyRunningGradients() {
		if (gradientCount == 0) {
			printf("No running gradient to apply");
			return;
		}

		double coefficient = learningRate / gradientCount;
		for (int l = 0; l < nLayers; l++) {
			runningGradient[l].first *= coefficient;
			runningGradient[l].second *= coefficient;
			layers[l]->applyGradients(runningGradient[l].first, runningGradient[l].second);

			// set back to zero
			runningGradient[l].first.apply([](double* v) {(*v) = 0; });
			runningGradient[l].second.apply([](double* v) {(*v) = 0; });
		}
		gradientCount = 0;
	}

	template<typename T1>
	int runGradientDescent(std::vector<RML::Matrix<T>> inputs, std::vector<T1> desiredOutputs, RML::Matrix<T>(*costFunctionGrad)(RML::Matrix<T>, T1), int(*checkFunction)(RML::Matrix<T>, T1) = nullptr) {
		RML::Matrix<T> pred, loss;
		std::vector<std::pair<RML::Matrix<double>, RML::Matrix<double>>> grad;
		int correct = 0;
		
		for (int i = 0; i < inputs.size(); i++) {
			pred = this->forward(inputs[i]);
			loss = costFunctionGrad(pred, desiredOutputs[i]);
			grad = this->backward(loss);
			this->addRunningGradients(grad, 1);

			if (checkFunction != nullptr) {
				correct += checkFunction(pred, desiredOutputs[i]);
			}

			pred.clear();
			loss.clear();
		}
		this->applyRunningGradients();
		return correct;
	}
};

};