#include "vector.hpp"
using namespace MTB;
Vector::Vector(): X(0.0), Y(0.0){}
Vector::Vector(double x, double y): X(x), Y(y){}
Vector::~Vector(){}
double Vector::dot(Vector vector){
  return this->X * vector.X + this->Y * vector.Y;
}
Vector Vector::operator+(Vector vector){
  return Vector(this->X + vector.X, this->Y + vector.Y);
}
Vector Vector::operator-(Vector vector){
  return Vector(this->X - vector.X, this->Y - vector.Y);
}
Vector Vector::operator*(double scale){
  return Vector(this->X * scale, this->Y * scale);
}
