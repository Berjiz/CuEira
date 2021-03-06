#ifndef LOGISTICTRANSFORM_H_
#define LOGISTICTRANSFORM_H_

#include <cuda_runtime.h>
#include <cublas_v2.h>

#include <CudaAdapter.cu>

namespace CuEira {
namespace CUDA {
namespace Kernel {

/**
 * This is ...
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
__global__ void LogisticTransform(const PRECISION* logitProb, PRECISION* probabilites, const int length) {
  int threadId = blockDim.x * blockIdx.x + threadIdx.x;

  if(threadId < length){
#ifdef DOUBLEPRECISION
    PRECISION expLogitProb = exp(logitProb[threadId]);
#else
    PRECISION expLogitProb = expf(logitProb[threadId]);
#endif

    probabilites[threadId] = expLogitProb / (1 + expLogitProb);
  } /* if threadId < numberOfRows */
}

} /* namespace Kernel */
} /* namespace CUDA */
} /* namespace CuEira */

#endif /* LOGISTICTRANSFORM_H_ */
