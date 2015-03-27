#include "CudaCovariatesHandlerFactory.h"

namespace CuEira {
namespace CUDA {

CudaCovariatesHandlerFactory::CudaCovariatesHandlerFactory(const Configuration& configuration) :
    environmentColumnName(configuration.getEnvironmentColumnName()) {

}

CudaCovariatesHandlerFactory::~CudaCovariatesHandlerFactory() {

}

CovariatesHandler<DeviceMatrix>* CudaCovariatesHandlerFactory::constructCovariatesHandler(
    const Container::PinnedHostMatrix& covariates, const std::vector<std::string>& columnNames,
    const HostToDevice& hostToDevice) const {
  const int numberOfColumns = covariates.getNumberOfColumns();
  Container::DeviceMatrix* covariatesDevice = new Container::DeviceMatrix(covariates.getNumberOfRows(),
      numberOfColumns - 1);

  int col = 0;
  for(int i = 0; i < numberOfColumns; ++i){
    if(environmentColumnName != columnNames[i]){
      Container::PinnedHostVector covVector = covariates(i);
      hostToDevice.transferVector(*covVector, covariatesDevice(0, col));

      delete covVector;
      ++col;
    }
  }

  return new CovariatesHandler<DeviceMatrix>(covariatesDevice);
}

}
/* namespace CUDA */
} /* namespace CuEira */
