
#include "Matrix.h"

namespace Maths
{

Matrix::Matrix(int n, int m) {
	nb = n*m;
	allocateMemory(n, m);
}

Matrix::Matrix(const Matrix& src) {
	copy(src);
}

Matrix::~Matrix() {
	freeMemory();
}

void Matrix::allocateMemory(int n, int m) {
	data = new number[nb];
}

void Matrix::freeMemory() {
	delete[] data;
}

void Matrix::copy(const Matrix& src) {
	this->n = src.n;
	this->m = src.m;
	this->nb = src.nb;
	allocateMemory(n, m);
	for (int i = 0 ; i < nb ; i++) this->data[i] = src.data[i];
}

void Matrix::set(int i, int j, number val) {
	data[i*m + j] = val;
}

number Matrix::get(int i, int j) const {
	return data[i*m + j];
}

Matrix& Matrix::operator=(const Matrix& src) {

	if (this != &src) {
		freeMemory();
		copy(src);
	}

	return *this;

}

}