#pragma once
# include "Activations.h"

namespace RML {

template<class T>
class Layer {
protected:
	void (*activation)(T*);
	void (*activationGrad)(T*);

public:

	// templates
	virtual RML::Matrix<T> forward(RML::Matrix<T>& a)		// returns the sum of the previous weights and input but does not apply activation
	{
		return RML::Matrix<T>({ 0 });
	}
	virtual void applyGradients(RML::Matrix<T>& biasGrad, RML::Matrix<T>& weightGrad) 
	{}

	// getters
	virtual std::vector<unsigned int> getDimensions()
	{
		return { 0 };
	}
	virtual RML::Matrix<double> getWeights()
	{
		return RML::Matrix<double>({ 0 });
	}
	virtual RML::Matrix<double> getBias()
	{
		return RML::Matrix<double>({ 0 });
	}

	void (*getActivation())(T*) {
		return activation;
	}
	void (*getActivationGrad())(T*) {
		return activationGrad;
	}

};
};
