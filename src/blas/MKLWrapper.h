#ifndef MKLWRAPPER_H_
#define MKLWRAPPER_H_

#ifdef MKL_BLAS
#include <mkl_vml.h>
#include <mkl.h>
#else
#include <cblas.h>
#endif

#include <iostream>

#include <BlasException.h>
#include <HostMatrix.h>
#include <HostVector.h>
#include <DimensionMismatch.h>

namespace CuEira {

using namespace CuEira::Container;

/**
 * This is ...
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class MKLWrapper {
public:
  MKLWrapper();
  virtual ~MKLWrapper();

  void copyVector(const HostVector& vectorFrom, HostVector& vectorTo) const;

  bool svd(HostMatrix& matrix, HostMatrix& uSVD, HostVector& sigma, HostMatrix& vtSVD) const;

  void matrixVectorMultiply(const HostMatrix& matrix, const HostVector& vector, HostVector& resultVector,
      PRECISION alpha = 1, PRECISION beta = 0) const;

  void matrixTransVectorMultiply(const HostMatrix& matrix, const HostVector& vector, HostVector& resultVector,
      PRECISION alpha = 1, PRECISION beta = 0) const;

  void matrixMatrixMultiply(const HostMatrix& matrix1, const HostMatrix& matrix2, HostMatrix& resultMatrix,
      PRECISION alpha = 1, PRECISION beta = 0) const;

  void matrixTransMatrixMultiply(const HostMatrix& matrix1, const HostMatrix& matrix2, HostMatrix& resultMatrix,
      PRECISION alpha = 1, PRECISION beta = 0) const;

  /**
   * vector2(i) = vector2(i)-vector1(i)
   */
  void differenceElememtWise(const HostVector& vector1, HostVector& vector2) const;

  void multiplicationElementWise(const HostVector& vector1, const HostVector& vector2, const HostVector& result) const;

  void absoluteSum(const HostVector& vector, PRECISION& result) const;

};

} /* namespace CuEira */

#endif /* MKLWRAPPER_H_ */
