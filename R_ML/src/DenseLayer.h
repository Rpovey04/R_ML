#include "Layer.h"


template<class T>
class DenseLayer : public Layer<T> {
private:
	Matrix<double> w;
	Matrix<double> bias;

public:
	DenseLayer()
	{}
	DenseLayer(unsigned int inputDim, unsigned int outputDim, void(*a)(T*), void(*aGrad)(T*)) {
		this->activation = a;
		this->activationGrad = aGrad;

		w = Matrix<double>({ inputDim, outputDim });
		w.randomise();
		bias = Matrix<double>({outputDim, 1});
		bias.randomise();
	}

	~DenseLayer() {
		w.clear();
		bias.clear();
	}

	Matrix<T> forward(Matrix<T>& input) override {
		if (input.size().size() != 2 || input.size()[1] != 1 || input.size()[0] != w.size()[0]) {		// crude
			printf("Input vector was either not 1d or was the wrong size");
			return Matrix<T>({ 0 });
		}

		w.transpose2D();

		Matrix<T> res = Matrix<T>::matmul2D(w, input);
		res += bias;

		w.transpose2D();
		return res;
	}
	void applyGradients(Matrix<T>& biasGrad, Matrix<T>& weightGrad) override {
		w -= weightGrad;
		bias -= biasGrad;
	}

	// getters
	std::vector<unsigned int> getDimensions() override {
		return w.size();
	}
	Matrix <double> getWeights() override {
		return w;
	}
	Matrix<double> getBias() override {
		return bias;
	}
};