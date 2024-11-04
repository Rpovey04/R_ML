#pragma once
# include "Activations.h"

namespace RML {

template<class T>
class Layer {
protected:
	T (*activation)(T);
	double bias = 0;

public: 
	activate(T* val) {
		T v = (*val);
		(*val) = activation(v + bias);
	}

// templates
	virtual RML::Matrix<T> forward(RML::Matrix<T>& a)
	{ return RML::Matrix<T>({ 0 }); }
	virtual RML::Matrix<T> backward(RML::Matrix<T>& a)
	{ return RML::Matrix<T>({ 0 }); }
};

};