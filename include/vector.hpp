#ifndef MTB_VECTOR_H
#define MTB_VECTOR_H

namespace MTB{
  class Vector{
  public:
    double X, Y, Z;
    Vector(); // Initializes a vector to all 0's
    Vector(double, double, double); // Initializes a vector with values
    ~Vector();
    double dot(Vector); // Returns the dot product of 2 vectors
    Vector operator+(Vector); // Adds 2 vectors together
    Vector operator-(Vector); // subtracts one vector from another
    Vector operator*(double); // Scales a vector by a scalar
  };
};

#endif /* MTB_VECTOR_H */
