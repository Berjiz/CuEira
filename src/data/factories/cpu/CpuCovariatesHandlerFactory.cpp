#include "CpuCovariatesHandlerFactory.h"

namespace CuEira {
namespace CPU {

CpuCovariatesHandlerFactory::CpuCovariatesHandlerFactory(const Configuration& configuration,
    const MKLWrapper& mklWrapper) :
    environmentColumnName(configuration.getEnvironmentColumnName()), mklWrapper(mklWrapper) {

}

CpuCovariatesHandlerFactory::~CpuCovariatesHandlerFactory() {

}

CovariatesHandler<HostMatrix>* CpuCovariatesHandlerFactory::constructCovariatesHandler(
    const Container::HostMatrix& matrix, const std::vector<std::string>& columnNames) const {
  const int numberOfColumns = matrix.getNumberOfColumns();
  Container::RegularHostMatrix* covariates = new Container::RegularHostMatrix(matrix.getNumberOfRows(),
      numberOfColumns - 1);

  int col = 0;
  for(int i = 0; i < numberOfColumns; ++i){
    if(environmentColumnName != columnNames[i]){
      Container::HostVector* covVectorFrom = matrix(i);
      Container::HostVector* covVectorTo = covariates(i);

      mklWrapper.copyVector(covVectorFrom, covVectorTo);

      delete covVectorFrom;
      delete covVectorFrom;
      ++col;
    }
  }

  return new CovariatesHandler<HostMatrix>(covariates);
}

} /* namespace CPU */
} /* namespace CuEira */
