#include <iostream>
#include <stdexcept>
#include <vector>
#include <thread>
#include <boost/chrono/chrono_io.hpp>

#include <Configuration.h>
#include <DataFilesReaderFactory.h>
#include <DataFilesReader.h>
#include <SNPVector.h>
#include <HostMatrix.h>
#include <HostVector.h>
#include <PersonHandler.h>
#include <EnvironmentFactor.h>
#include <Recode.h>
#include <DataHandler.h>
#include <ModelHandler.h>
#include <EnvironmentVector.h>
#include <InteractionVector.h>
#include <ContingencyTableFactory.h>
#include <AlleleStatisticsFactory.h>
#include <DataHandlerFactory.h>
#include <ResultWriter.h>
#include <LogisticRegressionModelHandler.h>

#ifdef CPU

#else
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <HostToDevice.h>
#include <DeviceToHost.h>
#include <CudaAdapter.cu>
#include <LogisticRegressionConfiguration.h>
#include <CudaException.h>
#include <GPUWorkerThread.h>
#include <Device.h>
#include <CudaException.h>
#include <Stream.h>
#include <StreamFactory.h>
#endif

#ifdef PROFILE
#include <CudaLogisticRegressionConfiguration.h>
#include <LogisticRegressionConfiguration.h>
#endif

/**
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
int main(int argc, char* argv[]) {
  using namespace CuEira;
  std::cerr << "Starting" << std::endl;
#ifdef DEBUG
  std::cerr << "WARNING CuEira was compiled in debug mode, this can affect performance." << std::endl;
#endif
  boost::chrono::system_clock::time_point startPoint = boost::chrono::system_clock::now();

  Configuration configuration(argc, argv);
  FileIO::DataFilesReaderFactory dataFilesReaderFactory;
  Container::SNPVectorFactory snpVectorFactory(configuration);
  AlleleStatisticsFactory alleleStatisticsFactory;
  Model::ModelInformationFactory modelInformationFactory;

  FileIO::DataFilesReader* dataFilesReader = dataFilesReaderFactory.constructDataFilesReader(configuration);
  FileIO::ResultWriter* resultWriter = new FileIO::ResultWriter(configuration);
#ifdef PROFILE
  boost::chrono::system_clock::time_point beforePersonHandlerPoint = boost::chrono::system_clock::now();
#endif
  PersonHandler* personHandler = dataFilesReader->readPersonInformation();
#ifdef PROFILE
  boost::chrono::system_clock::time_point afterPersonHandlerPoint = boost::chrono::system_clock::now();
  boost::chrono::duration<double> diffPersonHandlerSec = afterPersonHandlerPoint - beforePersonHandlerPoint;

  std::cerr << "Time for reading personal data: " << diffPersonHandlerSec << std::endl;
#endif

  const int numberOfIndividualsToInclude = personHandler->getNumberOfIndividualsToInclude();
  const Container::HostVector& outcomes = personHandler->getOutcomes();

#ifndef CPU
  int numberOfDevices = -1;
  if(configuration.isNumberOfGPUsSet()){
    numberOfDevices = configuration.getNumberOfGPUs();
  }else{
    cudaGetDeviceCount(&numberOfDevices);
    if(numberOfDevices == 0){
      throw new CudaException("No CUDA devices found.");
    }
  }

  const int numberOfStreams = configuration.getNumberOfStreams();
  const int numberOfThreads = numberOfDevices * numberOfStreams;
  std::cerr << "Calculating using " << numberOfDevices << " with " << numberOfStreams << " each." << std::endl;

  CUDA::StreamFactory* streamFactory = new CUDA::StreamFactory();
  std::vector<CUDA::Device*> devices(numberOfDevices);
  std::vector<std::thread*> workers(numberOfThreads);
  std::vector<CUDA::Stream*>* outcomeTransferStreams = new std::vector<CUDA::Stream*>(numberOfDevices);

  for(int deviceNumber = 0; deviceNumber < numberOfDevices; ++deviceNumber){
    CUDA::Device* device = new CUDA::Device(deviceNumber);
    devices[deviceNumber] = device;

    device->setActiveDevice();
    CUDA::Stream* stream = streamFactory->constructStream(*device);
    (*outcomeTransferStreams)[deviceNumber] = stream;

    CUDA::HostToDevice hostToDevice(*stream);

    device->setOutcomes(hostToDevice.transferVector((const PinnedHostVector&) outcomes));
  }
#endif

#ifdef PROFILE
  boost::chrono::system_clock::time_point beforeEnvFactorPoint = boost::chrono::system_clock::now();
#endif
  EnvironmentFactorHandler* environmentFactorHandler = dataFilesReader->readEnvironmentFactorInformation(
      *personHandler);
#ifdef PROFILE
  boost::chrono::system_clock::time_point afterEnvFactorPoint = boost::chrono::system_clock::now();
  boost::chrono::duration<double> diffEnvSec = afterEnvFactorPoint - beforeEnvFactorPoint;

  std::cerr << "Time for reading environment information: " << diffEnvSec << std::endl;
#endif
  std::vector<SNP*>* snpInformation = dataFilesReader->readSNPInformation();
#ifdef PROFILE
  boost::chrono::system_clock::time_point afterSNPPoint = boost::chrono::system_clock::now();
  boost::chrono::duration<double> diffSnpSec = afterSNPPoint - afterEnvFactorPoint;

  std::cerr << "Time for reading snp information: " << diffSnpSec << std::endl;
#endif

  const int numberOfSNPs = snpInformation->size();

  FileIO::BedReader* bedReader = new FileIO::BedReader(configuration, snpVectorFactory, alleleStatisticsFactory,
      *personHandler, numberOfSNPs);
  Task::DataQueue* dataQueue = new Task::DataQueue(snpInformation);

  ContingencyTableFactory contingencyTableFactory(outcomes);
  DataHandlerFactory* dataHandlerFactory = new DataHandlerFactory(configuration, *bedReader, contingencyTableFactory,
      modelInformationFactory, *environmentFactorHandler, *dataQueue);

  Container::HostMatrix* covariates = nullptr;
  std::vector<std::string>* covariatesNames = nullptr;
  if(configuration.covariateFileSpecified()){
#ifdef PROFILE
    boost::chrono::system_clock::time_point beforeCovPoint = boost::chrono::system_clock::now();
#endif
    std::pair<Container::HostMatrix*, std::vector<std::string>*>* covPair = dataFilesReader->readCovariates(
        *personHandler);
#ifdef PROFILE
    boost::chrono::system_clock::time_point afterCovPoint = boost::chrono::system_clock::now();
    boost::chrono::duration<double> diffCovSec = afterCovPoint - beforeCovPoint;

    std::cerr << "Time for reading covariates: " << diffCovSec << std::endl;
#endif
    covariates = covPair->first;
    covariatesNames = covPair->second;
    delete covPair;

  }

#ifdef PROFILE
  boost::chrono::system_clock::time_point afterInitPoint = boost::chrono::system_clock::now();
  boost::chrono::duration<double> diffInitSec = afterInitPoint - startPoint;

  std::cerr << "Time for initialisation: " << diffInitSec << std::endl;
#endif

#ifdef CPU
  //TODO
#else
  //GPU
  for(int deviceNumber = 0; deviceNumber < numberOfDevices; ++deviceNumber){
    CUDA::Device* device = devices[deviceNumber];
    device->setActiveDevice();
    CUDA::Stream* outcomeTransfeStream = (*outcomeTransferStreams)[deviceNumber];
    outcomeTransfeStream->syncStream();
    delete outcomeTransfeStream;

    //Start threads
    for(int streamNumber = 0; streamNumber < numberOfStreams; ++streamNumber){
      std::thread* thread = new std::thread(CuEira::CUDA::GPUWorkerThread, &configuration, device, dataHandlerFactory,
          resultWriter, (Container::PinnedHostMatrix*) covariates);
      workers[deviceNumber * numberOfStreams + streamNumber] = thread;
    }
  }
  delete outcomeTransferStreams;

  for(int threadNumber = 0; threadNumber < numberOfThreads; ++threadNumber){
    workers[threadNumber]->join();
  }

  for(int threadNumber = 0; threadNumber < numberOfThreads; ++threadNumber){
    delete workers[threadNumber];
  }

  for(int deviceNumber = 0; deviceNumber < numberOfDevices; ++deviceNumber){
    delete devices[deviceNumber];
  }

#endif

#ifdef PROFILE
  //Printing all the static timers
  std::cerr << "DataHandler" << std::endl;
  std::cerr << "Time spent recode: " << boost::chrono::duration_cast<boost::chrono::seconds>(DataHandler::timeSpentRecode) << std::endl;
  std::cerr << "Time spent next: " << boost::chrono::duration_cast<boost::chrono::seconds>(DataHandler::timeSpentNext) << std::endl;
  std::cerr << "Time spent read snp: " << boost::chrono::duration_cast<boost::chrono::seconds>(DataHandler::timeSpentSNPRead) << std::endl;
  std::cerr << "Time spent statistic model: " << boost::chrono::duration_cast<boost::chrono::seconds>(DataHandler::timeSpentStatModel) << std::endl;

  std::cerr << "CudaLogisticRegression" << std::endl;
  std::cerr << "Time spent CudaLR: " << boost::chrono::duration_cast<boost::chrono::seconds>(Model::LogisticRegression::CUDA::CudaLogisticRegression::timeSpentTotal) << std::endl;
  std::cerr << "Time spent CPU: " << boost::chrono::duration_cast<boost::chrono::seconds>(Model::LogisticRegression::CUDA::CudaLogisticRegression::timeSpentCPU) << std::endl;
  std::cerr << "Time spent GPU: " << Model::LogisticRegression::CUDA::CudaLogisticRegression::timeSpentGPU / 1000 << " seconds" << std::endl;
  std::cerr << "Time spent LR transferFromDevice: " << Model::LogisticRegression::CUDA::CudaLogisticRegression::timeSpentTransferFromDevice / 1000 << " seconds" << std::endl;
  std::cerr << "Time spent LR transferToDevice: " << Model::LogisticRegression::CUDA::CudaLogisticRegression::timeSpentTransferToDevice / 1000 << " seconds" << std::endl;

  boost::chrono::system_clock::time_point afterCalcPoint = boost::chrono::system_clock::now();
  boost::chrono::duration<double> diffCalcSec = afterCalcPoint - afterInitPoint;

  std::cerr << "Time for calculations: " << diffCalcSec << std::endl;
#endif

#ifndef CPU
  delete streamFactory;
#endif

  delete resultWriter;
  delete bedReader;
  delete dataFilesReader;
  delete personHandler;
  delete environmentFactorHandler;
  delete dataQueue;
  delete covariates;
  delete covariatesNames;
  delete dataHandlerFactory;

  boost::chrono::system_clock::time_point endPoint = boost::chrono::system_clock::now();

#ifdef PROFILE
  boost::chrono::duration<double> diffCleanupSec = endPoint - afterCalcPoint;
  std::cerr << "Time for cleanup: " << diffCleanupSec << std::endl;
#endif

  boost::chrono::duration<double> diffSec = endPoint - startPoint;
  std::cerr << "Complete, time: " << diffSec << std::endl;

}
