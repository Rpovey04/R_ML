#pragma once
# include "Activations.h"

template<class T>
class Layer {
protected:
	void (*activation)(T*);
	void (*activationGrad)(T*);

public:

	// templates
	virtual Matrix<T> forward(Matrix<T>& a)		// returns the sum of the previous weights and input but does not apply activation
	{
		return Matrix<T>({ 0 });
	}
	virtual void applyGradients(Matrix<T>& biasGrad, Matrix<T>& weightGrad) 
	{}

	// getters
	virtual std::vector<unsigned int> getDimensions()
	{
		return { 0 };
	}
	virtual Matrix<double> getWeights()
	{
		return Matrix<double>({ 0 });
	}
	virtual Matrix<double> getBias()
	{
		return Matrix<double>({ 0 });
	}

	void (*getActivation())(T*) {
		return activation;
	}
	void (*getActivationGrad())(T*) {
		return activationGrad;
	}

};
