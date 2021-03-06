#include "CudaLogisticRegressionConfiguration.h"

namespace CuEira {
namespace Model {
namespace LogisticRegression {
namespace CUDA {

CudaLogisticRegressionConfiguration::CudaLogisticRegressionConfiguration(const Configuration& configuration,
    const Stream& stream, const DeviceVector& deviceOutcomes) :
    LogisticRegressionConfiguration(configuration, false, deviceOutcomes.getNumberOfRows(), 4), stream(stream), devicePredictors(
        new DeviceMatrix(numberOfRows, numberOfPredictors)), deviceOutcomes(&deviceOutcomes), devicePredictorsMemoryPointer(
        devicePredictors->getMemoryPointer()), betaCoefficentsDevice(new DeviceVector(numberOfPredictors)), probabilitesDevice(
        new DeviceVector(numberOfRows)), scoresDevice(new DeviceVector(numberOfPredictors)), informationMatrixDevice(
        new DeviceMatrix(numberOfPredictors, numberOfPredictors)), workMatrixNxMDevice(
        new DeviceMatrix(numberOfRows, numberOfPredictors)), workVectorNx1Device(new DeviceVector(numberOfRows)), betaCoefficentsDefaultHost(
        new PinnedHostVector(numberOfPredictors)), scoresHost(new PinnedHostVector(numberOfPredictors))
#ifdef PROFILE
,beforeCov(nullptr),afterCov(nullptr),beforeIntercept(nullptr),afterIntercept(nullptr),beforeSNP(nullptr),afterSNP(nullptr),
beforeEnv(nullptr),afterEnv(nullptr),beforeInter(nullptr),afterInter(nullptr)
#endif
{

  transferIntercept();

  setDefaultBeta(*betaCoefficentsDefaultHost);
}

CudaLogisticRegressionConfiguration::CudaLogisticRegressionConfiguration(const Configuration& configuration,
    const Stream& stream, const DeviceVector& deviceOutcomes, const PinnedHostMatrix& covariates) :
    LogisticRegressionConfiguration(configuration, true, deviceOutcomes.getNumberOfRows(),
        4 + covariates.getNumberOfColumns()), stream(stream), devicePredictors(
        new DeviceMatrix(numberOfRows, numberOfPredictors)), deviceOutcomes(&deviceOutcomes), devicePredictorsMemoryPointer(
        devicePredictors->getMemoryPointer()), betaCoefficentsDevice(new DeviceVector(numberOfPredictors)), probabilitesDevice(
        new DeviceVector(numberOfRows)), scoresDevice(new DeviceVector(numberOfPredictors)), informationMatrixDevice(
        new DeviceMatrix(numberOfPredictors, numberOfPredictors)), workMatrixNxMDevice(
        new DeviceMatrix(numberOfRows, numberOfPredictors)), workVectorNx1Device(new DeviceVector(numberOfRows)), betaCoefficentsDefaultHost(
        new PinnedHostVector(numberOfPredictors)), scoresHost(new PinnedHostVector(numberOfPredictors))
#ifdef PROFILE
,beforeCov(nullptr),afterCov(nullptr),beforeIntercept(nullptr),afterIntercept(nullptr),beforeSNP(nullptr),afterSNP(nullptr),
beforeEnv(nullptr),afterEnv(nullptr),beforeInter(nullptr),afterInter(nullptr)
#endif
{

  transferIntercept();

  //Transfer covariates
  PRECISION* pos = devicePredictorsMemoryPointer + numberOfRows * 4; //Putting the covariates in the columns after the intercept, snp, environment and interaction columns.

#ifdef PROFILE
      beforeCov = new Event(stream);
#endif
  transferMatrix(stream, covariates, pos);
#ifdef PROFILE
  afterCov = new Event(stream);
#endif

#ifdef FERMI
  stream.syncStream();
#endif

  setDefaultBeta(*betaCoefficentsDefaultHost);
}

CudaLogisticRegressionConfiguration::CudaLogisticRegressionConfiguration(const Configuration& configuration,
    const Stream& stream) :
    LogisticRegressionConfiguration(configuration, false, 0, 0), deviceOutcomes(nullptr), devicePredictors(nullptr), devicePredictorsMemoryPointer(
        nullptr), betaCoefficentsDevice(nullptr), probabilitesDevice(nullptr), scoresDevice(nullptr), informationMatrixDevice(
        nullptr), workMatrixNxMDevice(nullptr), workVectorNx1Device(nullptr), betaCoefficentsDefaultHost(nullptr), scoresHost(
        nullptr), stream(stream)
#ifdef PROFILE
,beforeCov(nullptr),afterCov(nullptr),beforeIntercept(nullptr),afterIntercept(nullptr),beforeSNP(nullptr),afterSNP(nullptr),
beforeEnv(nullptr),afterEnv(nullptr),beforeInter(nullptr),afterInter(nullptr)
#endif
{

}

CudaLogisticRegressionConfiguration::~CudaLogisticRegressionConfiguration(){
  delete devicePredictors;
  delete betaCoefficentsDevice;
  delete probabilitesDevice;
  delete scoresDevice;
  delete informationMatrixDevice;
  delete workMatrixNxMDevice;
  delete workVectorNx1Device;
  delete betaCoefficentsDefaultHost;
  delete scoresHost;

#ifdef PROFILE
  delete beforeCov;
  delete afterCov;
  delete beforeIntercept;
  delete afterIntercept;
  delete beforeSNP;
  delete afterSNP;
  delete beforeEnv;
  delete afterEnv;
  delete beforeInter;
  delete afterInter;
#endif
}

void CudaLogisticRegressionConfiguration::transferIntercept(){
  PinnedHostVector interceptHostVector(numberOfRows);
  for(int i = 0; i < numberOfRows; ++i){
    interceptHostVector(i) = 1;
  }

#ifdef PROFILE
  beforeIntercept = new Event(stream);
#endif

  transferVector(stream, interceptHostVector, devicePredictorsMemoryPointer); //Putting the intercept as first column

#ifdef PROFILE
  afterIntercept = new Event(stream);
#endif

#ifdef FERMI
  stream.syncStream();
#endif
}

void CudaLogisticRegressionConfiguration::setEnvironmentFactor(const HostVector& environmentData){
  PRECISION* pos = devicePredictorsMemoryPointer + numberOfRows * 2; //Putting the environment as the third column

#ifdef PROFILE
      beforeEnv = new Event(stream);
#endif

  transferVector(stream, (const PinnedHostVector&) environmentData, pos);

#ifdef PROFILE
  afterEnv = new Event(stream);
#endif

#ifdef FERMI
  stream.syncStream();
#endif
}

void CudaLogisticRegressionConfiguration::setSNP(const HostVector& snpData){
  PRECISION* pos = devicePredictorsMemoryPointer + numberOfRows * 1; //Putting the snp column as the second column

#ifdef PROFILE
      beforeSNP = new Event(stream);
#endif

  transferVector(stream, (const PinnedHostVector&) snpData, pos);

#ifdef PROFILE
  afterSNP = new Event(stream);
#endif

#ifdef FERMI
  stream.syncStream();
#endif
}

void CudaLogisticRegressionConfiguration::setInteraction(const HostVector& interactionVector){
  PRECISION* pos = devicePredictorsMemoryPointer + numberOfRows * 3; //Putting the interaction column as the fourth column

#ifdef PROFILE
      beforeInter = new Event(stream);
#endif

  transferVector(stream, (const PinnedHostVector&) interactionVector, pos);

#ifdef PROFILE
  afterInter = new Event(stream);
#endif

#ifdef FERMI
  stream.syncStream();
#endif
}

const Stream& CudaLogisticRegressionConfiguration::getStream() const{
  return stream;
}

const DeviceMatrix& CudaLogisticRegressionConfiguration::getPredictors() const{
  return *devicePredictors;
}

const DeviceVector& CudaLogisticRegressionConfiguration::getOutcomes() const{
  return *deviceOutcomes;
}

DeviceVector& CudaLogisticRegressionConfiguration::getProbabilites(){
  return *probabilitesDevice;
}

DeviceVector& CudaLogisticRegressionConfiguration::getScores(){
  return *scoresDevice;
}

HostVector& CudaLogisticRegressionConfiguration::getScoresHost(){
  return *scoresHost;
}

DeviceMatrix& CudaLogisticRegressionConfiguration::getInformationMatrix(){
  return *informationMatrixDevice;
}

DeviceVector& CudaLogisticRegressionConfiguration::getBetaCoefficents(){
  return *betaCoefficentsDevice;
}

DeviceMatrix& CudaLogisticRegressionConfiguration::getWorkMatrixNxM(){
  return *workMatrixNxMDevice;
}

DeviceVector& CudaLogisticRegressionConfiguration::getWorkVectorNx1(){
  return *workVectorNx1Device;
}

const PinnedHostVector& CudaLogisticRegressionConfiguration::getDefaultBetaCoefficents() const{
  return *betaCoefficentsDefaultHost;
}

} /* namespace CUDA */
} /* namespace LogisticRegression */
} /* namespace Model */
} /* namespace CuEira */
