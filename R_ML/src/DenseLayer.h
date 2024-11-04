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
	DenseLayer(unsigned int inputDim, unsigned int outputDim, void(*a)(T*)) {
		this->activation = a;
		
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
		res.apply(this->activation);
		
		w.transpose2D();
		return res;
	}

	RML::Matrix<T> backward(RML::Matrix<T>& input) {
		return RML::Matrix<T>({ 0 });
	}
};


};