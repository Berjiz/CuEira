#ifndef GPUWORKERTHREAD_H_
#define GPUWORKERTHREAD_H_

#include <thread>

#include <Configuration.h>
#include <Device.h>
#include <DataHandlerFactory.h>
#include <DeviceVector.h>
#include <DeviceMatrix.h>
#include <HostVector.h>
#include <HostMatrix.h>
#include <PinnedHostVector.h>
#include <PinnedHostMatrix.h>
#include <DataHandler.h>
#include <Stream.h>
#include <CudaLogisticRegressionConfiguration.h>
#include <CudaLogisticRegression.h>
#include <LogisticRegressionModelHandler.h>
#include <SNP.h>
#include <DataHandlerState.h>
#include <HostToDevice.h>
#include <DeviceToHost.h>
#include <InteractionStatistics.h>
#include <StreamFactory.h>
#include <KernelWrapper.h>
#include <MKLWrapper.h>
#include <ResultWriter.h>
#include <ModelInformation.h>
#include <CombinedResults.h>
#include <CombinedResultsFactory.h>
#ifdef PROFILE
#include <boost/chrono/chrono_io.hpp>
#endif

namespace CuEira {
namespace CUDA {

/**
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */

/**
 * Thread to do work on GPU
 */
void GPUWorkerThread(const Configuration* configuration, const Device* device,
    const DataHandlerFactory* dataHandlerFactory, FileIO::ResultWriter* resultWriter,
    const Container::PinnedHostMatrix* covariates);

} /* namespace CUDA */
} /* namespace CuEira */

#endif /* GPUWORKERTHREAD_H_ */
