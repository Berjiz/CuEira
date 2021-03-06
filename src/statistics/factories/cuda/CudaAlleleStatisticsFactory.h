#ifndef CUDAALLELESTATISTICSFACTORY_H_
#define CUDAALLELESTATISTICSFACTORY_H_

#include <AlleleStatisticsFactory.h>
#include <DeviceVector.h>
#include <KernelWrapper.h>
#include <SNPVector.h>
#include <PhenotypeVector.h>
#include <DeviceToHost.h>
#include <Stream.h>

namespace CuEira {
namespace CUDA {

/*
 * This class ...
 *
 *  @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class CudaAlleleStatisticsFactory: public CuEira::AlleleStatisticsFactory<Container::DeviceVector> {
public:
  explicit CudaAlleleStatisticsFactory(const Stream& stream);
  virtual ~CudaAlleleStatisticsFactory();

protected:
  virtual std::vector<int>* getNumberOfAllelesPerGenotype(
      const Container::SNPVector<Container::DeviceVector>& snpVector,
      const Container::PhenotypeVector<Container::DeviceVector>& phenotypeVector) const;

  const Stream& stream;
};

} /* namespace CUDA */
} /* namespace CuEira */

#endif /* CUDAALLELESTATISTICSFACTORY_H_ */
