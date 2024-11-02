#include "Layer.h"


namespace RML {

template<class T>
class DenseLayer : public RML::Layer<T> {
private:
	void (*activation)(T*);
	RML::Matrix<double> w;

public:
	DenseLayer()
	{}
	DenseLayer(unsigned int inputDim, unsigned int outputDim, void(*a)(T*)) {
		activation = a;
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
		res.apply(activation);
		
		w.transpose2D();
		return res;
	}

	RML::Matrix<T> backward(RML::Matrix<T>& input) {
		return RML::Matrix<T>({ 0 });
	}
};


};