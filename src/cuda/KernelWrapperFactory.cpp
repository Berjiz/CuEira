#include "KernelWrapperFactory.h"

namespace CuEira {
namespace CUDA {

KernelWrapperFactory::KernelWrapperFactory() {

}

KernelWrapperFactory::~KernelWrapperFactory() {

}

KernelWrapper* KernelWrapperFactory::constructKernelWrapper(const Stream& stream) const {
  return new KernelWrapper(stream.getCudaStream(), stream.getCublasHandle());
}

} /* namespace CUDA */
} /* namespace CuEira */
