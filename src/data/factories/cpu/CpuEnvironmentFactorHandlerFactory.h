#ifndef CPUENVIRONMENTFACTORHANDLERFACTORY_H_
#define CPUENVIRONMENTFACTORHANDLERFACTORY_H_

#include <memory>
#include <vector>
#include <string>

#include <EnvironmentFactorHandlerFactory.h>
#include <EnvironmentFactorHandler.h>
#include <HostVector.h>
#include <RegularHostVector.h>
#include <MKLWrapper.h>

namespace CuEira {
namespace CPU {

/*
 * This class ...
 *
 *  @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class CpuEnvironmentFactorHandlerFactory: public EnvironmentFactorHandlerFactory<Container::HostMatrix,
    Container::HostVector> {
public:
  explicit CpuEnvironmentFactorHandlerFactory(const Configuration& configuration,
      const std::vector<std::string>& columnNames, const Container::HostMatrix& matrix);
  virtual ~CpuEnvironmentFactorHandlerFactory();

  virtual EnvironmentFactorHandler<Container::HostVector>* constructEnvironmentFactorHandler() const;

  CpuEnvironmentFactorHandlerFactory(const CpuEnvironmentFactorHandlerFactory&) = delete;
  CpuEnvironmentFactorHandlerFactory(CpuEnvironmentFactorHandlerFactory&&) = delete;
  CpuEnvironmentFactorHandlerFactory& operator=(const CpuEnvironmentFactorHandlerFactory&) = delete;
  CpuEnvironmentFactorHandlerFactory& operator=(CpuEnvironmentFactorHandlerFactory&&) = delete;

private:

};

} /* namespace CPU */
} /* namespace CuEira */

#endif /* CPUENVIRONMENTFACTORHANDLERFACTORY_H_ */
