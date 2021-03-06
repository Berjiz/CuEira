#ifndef DATAFILESREADERFACTORY_H_
#define DATAFILESREADERFACTORY_H_

#include <stdexcept>
#include <map>
#include <memory>

#include <DataFilesReader.h>
#include <Configuration.h>
#include <BedReader.h>
#include <BimReader.h>
#include <FamReader.h>
#include <DimensionMismatch.h>
#include <CSVReader.h>
#include <PersonHandler.h>
#include <PersonHandlerLocked.h>
#include <PersonHandlerFactory.h>
#include <SNPVectorFactory.h>
#include <CpuSNPVectorFactory.h>
#include <RegularHostVector.h>

#ifndef CPU
#include <CudaSNPVectorFactory.h>
#include <Stream.h>
#include <DeviceVector.h>
#endif

namespace CuEira {
namespace FileIO {

/**
 * This is ...
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class DataFilesReaderFactory {
public:
  explicit DataFilesReaderFactory(const Configuration& configuration);
  virtual ~DataFilesReaderFactory();

  DataFilesReader<Container::RegularHostVector>* constructCpuDataFilesReader();

#ifndef CPU
  DataFilesReader<Container::DeviceVector>* constructCudaDataFilesReader(const CUDA::Stream& stream);
#endif

private:
  const Configuration& configuration;
  std::shared_ptr<const PersonHandlerLocked> personHandlerLocked;
  std::shared_ptr<const BimReader> bimReader;
  std::shared_ptr<const CSVReader> csvReader;
};

} /* namespace FileIO */
} /* namespace CuEira */

#endif /* DATAFILESREADERFACTORY_H_ */
