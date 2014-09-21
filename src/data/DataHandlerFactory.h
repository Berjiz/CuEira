#ifndef DATAHANDLERFACTORY_H_
#define DATAHANDLERFACTORY_H_

#include <Configuration.h>
#include <DataHandler.h>
#include <BedReader.h>
#include <DataQueue.h>
#include <Configuration.h>
#include <ContingencyTableFactory.h>
#include <EnvironmentFactor.h>
#include <EnvironmentFactorHandler.h>
#include <EnvironmentVector.h>
#include <InteractionVector.h>
#include <ModelInformationFactory.h>

namespace CuEira {

/**
 * This is
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class DataHandlerFactory {
public:
  DataHandlerFactory(const Configuration& configuration, FileIO::BedReader& bedReader,
      const ContingencyTableFactory& contingencyTableFactory,
      const Model::ModelInformationFactory& modelInformationFactory,
      const EnvironmentFactorHandler& environmentFactorHandler, Task::DataQueue& dataQueue);
  virtual ~DataHandlerFactory();

  virtual DataHandler* constructDataHandler() const;

  DataHandlerFactory(const DataHandlerFactory&) = delete;
  DataHandlerFactory(DataHandlerFactory&&) = delete;
  DataHandlerFactory& operator=(const DataHandlerFactory&) = delete;
  DataHandlerFactory& operator=(DataHandlerFactory&&) = delete;

private:
  const Configuration& configuration;
  FileIO::BedReader& bedReader;
  const ContingencyTableFactory& contingencyTableFactory;
  const Model::ModelInformationFactory& modelInformationFactory;
  const EnvironmentFactorHandler& environmentFactorHandler;
  Task::DataQueue& dataQueue;
};

} /* namespace CuEira */

#endif /* DATAHANDLERFACTORY_H_ */
