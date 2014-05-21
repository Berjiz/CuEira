#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <math.h>

#include <CudaAdapter.cu>
#include <CudaException.h>
#include <CublasException.h>
#include <KernelWrapper.h>
#include <DeviceVector.h>
#include <DeviceMatrix.h>
#include <HostVector.h>
#include <HostMatrix.h>
#include <PinnedHostMatrix.h>
#include <PinnedHostVector.h>
#include <DeviceToHost.h>
#include <HostToDevice.h>

namespace CuEira {
namespace CUDA {

/**
 * Test for testing ....
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class LogitTransformTest: public ::testing::Test {
protected:
  LogitTransformTest();
  virtual ~LogitTransformTest();
  virtual void SetUp();
  virtual void TearDown();

  cublasStatus_t cublasStatus;
  cudaStream_t stream1;
  cublasHandle_t cublasHandle;
  HostToDevice hostToDeviceStream1;
  DeviceToHost deviceToHostStream1;
  KernelWrapper kernelWrapper;
};

LogitTransformTest::LogitTransformTest() :
    cublasStatus(cublasCreate(&cublasHandle)), hostToDeviceStream1(HostToDevice(stream1)), deviceToHostStream1(
        DeviceToHost(stream1)), kernelWrapper(stream1) {

  handleCublasStatus(cublasStatus, "Failed to create cublas handle:");
  handleCudaStatus(cudaStreamCreate(&stream1), "Failed to create cuda stream 1:");
  handleCublasStatus(cublasSetStream(cublasHandle, stream1), "Failed to set cuda stream:");

}

LogitTransformTest::~LogitTransformTest() {
  handleCublasStatus(cublasDestroy(cublasHandle), "Failed to destroy cublas handle:");
  handleCudaStatus(cudaStreamDestroy(stream1), "Failed to destroy cuda stream 1:");
}

void LogitTransformTest::SetUp() {

}

void LogitTransformTest::TearDown() {

}

TEST_F(LogitTransformTest, KernelSmallVector) {
  const int numberOfRows = 5;

  Container::PinnedHostVector* hostVectorFrom = new Container::PinnedHostVector(numberOfRows);
  for(int i = 0; i < numberOfRows; ++i){
    (*hostVectorFrom)(i) = i / 10;
  }

  Container::DeviceVector* logitDeviceVector = hostToDeviceStream1.transferVector(hostVectorFrom);
  Container::DeviceVector* probDeviceVector = new Container::DeviceVector(numberOfRows);

  kernelWrapper.logisticTransform(*logitDeviceVector, *probDeviceVector);

  Container::HostVector* resultHostVector = deviceToHostStream1.transferVector(probDeviceVector);
  cudaStreamSynchronize(stream1);

  ASSERT_EQ(numberOfRows, resultHostVector->getNumberOfRows());

  for(int i = 0; i < numberOfRows; ++i){
    PRECISION x = i / 10;
    EXPECT_EQ(exp(x) / (1 + exp(x)), (*resultHostVector)(i));
  }

  delete hostVectorFrom;
  delete logitDeviceVector;
  delete probDeviceVector;
  delete resultHostVector;
}

}
/* namespace CUDA */
} /* namespace CuEira */

