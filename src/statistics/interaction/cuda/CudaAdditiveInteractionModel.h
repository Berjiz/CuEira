#ifndef CUDAADDITIVEINTERACTIONMODEL_H_
#define CUDAADDITIVEINTERACTIONMODEL_H_

#include <AdditiveInteractionModel.h>
#include <EnvironmentVector.h>
#include <InteractionVector.h>
#include <SNPVector.h>
#include <DeviceVector.h>
#include <KernelWrapper.h>
#include <Stream.h>

namespace CuEira {
namespace CUDA {

using namespace CuEira::Container;

/*
 * This class ...
 *
 *  @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class CudaAdditiveInteractionModel: public AdditiveInteractionModel<DeviceVector> {
public:
  explicit CudaAdditiveInteractionModel(const Stream& stream);
  virtual ~CudaAdditiveInteractionModel();

  virtual void applyModel(SNPVector<DeviceVector>& snpVector, EnvironmentVector<DeviceVector>& environmentVector,
      InteractionVector<DeviceVector>& interactionVector);

protected:
  const Stream& stream;
};

} /* namespace CUDA */
} /* namespace CuEira */

#endif /* CUDAADDITIVEINTERACTIONMODEL_H_ */
