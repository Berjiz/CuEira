#include "HostMatrix.h"

namespace CuEira {
namespace Container {

HostMatrix::HostMatrix(unsigned int numberOfRows, unsigned int numberOfColums, PRECISION* hostMatrix) :
    numberOfRows(numberOfRows), numberOfColumns(numberOfColums), hostMatrix(hostMatrix) {

}

HostMatrix::~HostMatrix() {

}

int HostMatrix::getNumberOfRows() const {
  return numberOfRows;
}

int HostMatrix::getNumberOfColumns() const {
  return numberOfColumns;
}

PRECISION* HostMatrix::getMemoryPointer() {
  return hostMatrix;
}

const PRECISION* HostMatrix::getMemoryPointer() const {
  return hostMatrix;
}

} /* namespace Container */
} /* namespace CuEira */
