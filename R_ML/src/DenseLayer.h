#include "Layer.h"


namespace RML {

template<class T>
class DenseLayer : public RML::Layer<T> {
private:
	RML::Matrix<double> w;
	RML::Matrix<double> bias;

public:
	DenseLayer()
	{}
	DenseLayer(unsigned int inputDim, unsigned int outputDim, void(*a)(T*), void(*aGrad)(T*)) {
		this->activation = a;
		this->activationGrad = aGrad;

		w = RML::Matrix<double>({ inputDim, outputDim });
		w.randomise();
		bias = RML::Matrix<double>({outputDim, 1});
		bias.randomise();
	}

	~DenseLayer() {
		w.clear();
		bias.clear();
	}

	RML::Matrix<T> forward(RML::Matrix<T>& input) override {
		if (input.size().size() != 2 || input.size()[1] != 1 || input.size()[0] != w.size()[0]) {		// crude
			printf("Input vector was either not 1d or was the wrong size");
			return RML::Matrix<T>({ 0 });
		}

		w.transpose2D();

		RML::Matrix<T> res = RML::Matrix<T>::matmul2D(w, input);
		res += bias;

		w.transpose2D();
		return res;
	}

	RML::Matrix<T> backward(RML::Matrix<T>& input) override {		// ended up being done in neural net
		return RML::Matrix<T>({ 0 });
	}

	void applyGradients(RML::Matrix<T>& biasGrad, RML::Matrix<T>& weightGrad) override {
		w -= weightGrad;
		bias -= biasGrad;
	}

	// getters
	std::vector<unsigned int> getDimensions() override {
		return w.size();
	}
	RML::Matrix <double> getWeights() override {
		return w;
	}
	RML::Matrix<double> getBias() override {
		return bias;
	}
};


};