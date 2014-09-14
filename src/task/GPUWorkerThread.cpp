#include "GPUWorkerThread.h"

namespace CuEira {
namespace CUDA {

void GPUWorkerThread(const Configuration* configuration, const Device* device,
    const DataHandlerFactory* dataHandlerFactory, FileIO::ResultWriter* resultWriter) {

  DataHandler* dataHandler = dataHandlerFactory->constructDataHandler();
  device->setActiveDevice();
  CUDA::handleCudaStatus(cudaGetLastError(), "Error before initialisation in GPUWorkerThread "); //<< std::this_thread::get_id() << " : "); //FIXME

  KernelWrapperFactory kernelWrapperFactory;
  StreamFactory streamFactory;
  StatisticsFactory statisticsFactory;
  const Container::DeviceVector& deviceOutcomes = device->getOutcomes();

  Stream* stream = streamFactory.constructStream(*device);
  KernelWrapper* kernelWrapper = kernelWrapperFactory.constructKernelWrapper(*stream);
  HostToDevice hostToDevice(stream->getCudaStream()); //FIXME
  DeviceToHost deviceToHost(stream->getCudaStream()); //FIXME

  Model::LogisticRegression::LogisticRegressionConfiguration* logisticRegressionConfiguration = nullptr;

  if(configuration->covariateFileSpecified()){
    //logisticRegressionConfiguration = new Model::LogisticRegression::LogisticRegressionConfiguration(*configuration,
    //hostToDevice, deviceOutcomes, *kernelWrapper, *covariates);
  }else{
    logisticRegressionConfiguration = new Model::LogisticRegression::LogisticRegressionConfiguration(*configuration,
        hostToDevice, deviceOutcomes, *kernelWrapper);
  }

  Model::LogisticRegression::LogisticRegression* logisticRegression = new Model::LogisticRegression::LogisticRegression(
      logisticRegressionConfiguration, hostToDevice, deviceToHost);
  Model::ModelHandler* modelHandler = new Model::GpuModelHandler(statisticsFactory, dataHandler,
      *logisticRegressionConfiguration, logisticRegression);
  CUDA::handleCudaStatus(cudaGetLastError(), "Error with initialisation in GPUWorkerThread "); // << std::this_thread::get_id() << " : "); //FIXME

  DataHandlerState dataHandlerState = modelHandler->next();
  while(dataHandlerState != DONE){
    const SNP& snp = modelHandler->getCurrentSNP();
    const EnvironmentFactor& envFactor = modelHandler->getCurrentEnvironmentFactor();

    if(dataHandlerState == SKIP){
      resultWriter->writePartialResult(snp, envFactor);
    }else{
      Statistics* statistics = modelHandler->calculateModel(snp, envFactor, *statistics, modelHandler->getSNPVector());

      CUDA::handleCudaStatus(cudaGetLastError(), "Error with ModelHandler in GPUWorkerThread "); // << std::this_thread::get_id() << " : "); //FIXME

      resultWriter->writeFullResult();

      delete statistics;
    } //else

    dataHandlerState = modelHandler->next();
  }

  delete stream;
  delete kernelWrapper;
  delete modelHandler;

}

} /* namespace CUDA */
} /* namespace CuEira */
