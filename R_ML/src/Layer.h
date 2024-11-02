#pragma once
# include "Activations.h"

namespace RML {

template<class T>
class Layer {
public:
	virtual RML::Matrix<T> forward(RML::Matrix<T>& a)
	{ return RML::Matrix<T>({ 0 }); }
	virtual RML::Matrix<T> backward(RML::Matrix<T>& a)
	{ return RML::Matrix<T>({ 0 }); }
};

};