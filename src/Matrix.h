#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;
namespace fre {
	typedef vector<double> Vector;
	typedef vector<Vector> Matrix;
	// overload operators as independent functions
	Vector operator*(const Matrix& C, const Vector& V);
	Vector operator*(const double& a, const Vector& V);
	Vector operator*(const Vector& V, const Vector& W);
	Vector operator+(const double& a, const Vector& V);
	Vector operator+(const Vector& V, const Vector& W);  // referece to const for V and W, V + W -> U, V and W do not change
	Vector operator-(const Vector& V, const Vector& W);
	Vector operator/(const Vector& V, const Vector& W);
	Vector exp(const Vector& V);
	double operator^(const Vector& V, const Vector& W); // scalar operator
	ostream& operator<<(ostream& out, Vector& V); // Overload cout for Vector
	ostream& operator<<(ostream& out, vector<string>& V);
	ostream& operator<<(ostream& out, Matrix& W);	// Overload cout for Matrix
}


