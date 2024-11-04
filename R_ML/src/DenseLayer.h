#include "Layer.h"


namespace RML {

template<class T>
class DenseLayer : public RML::Layer<T> {
private:
	RML::Matrix<double> w;
	double bias;

public:
	DenseLayer()
	{}
	DenseLayer(unsigned int inputDim, unsigned int outputDim, T(*a)(T)) {
		RML::Layer<T>::activation = a;
		RML::Layer<T>::bias = (((double)rand() / (double)RAND_MAX) * 2) - 1;
		
		w = RML::Matrix<double>({ inputDim, outputDim });
		w.randomise();
	}

	~DenseLayer() {
		w.clear();
	}

	RML::Matrix<T> forward(RML::Matrix<T>& input) override {
		if (input.size().size() != 2 || input.size()[1] != 1 || input.size()[0] != w.size()[0]) {		// crude
			printf("Input vector was either not 1d or was the wrong size");
			return RML::Matrix<T>({ 0 });
		}

		w.transpose2D();

		RML::Matrix<T> res = RML::Matrix<T>::matmul2D(w, input);
		res.apply(RML::Layer<T>::activate);
		
		w.transpose2D();
		return res;
	}

	RML::Matrix<T> backward(RML::Matrix<T>& input) {
		return RML::Matrix<T>({ 0 });
	}
};


};