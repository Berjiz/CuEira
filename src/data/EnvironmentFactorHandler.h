#ifndef ENVIRONMENTFACTORHANDLER_H_
#define ENVIRONMENTFACTORHANDLER_H_

#include <vector>

#include <Id.h>
#include <EnvironmentFactor.h>
#include <HostMatrix.h>
#include <HostVector.h>
#include <EnvironmentFactorHandlerException.h>

namespace CuEira {

/**
 * This is ...
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class EnvironmentFactorHandler {
public:
  EnvironmentFactorHandler(Container::HostMatrix* dataMatrix, std::vector<EnvironmentFactor*>* environmentFactors);
  virtual ~EnvironmentFactorHandler();

  virtual const std::vector<const EnvironmentFactor*>& getHeaders() const;

  //This vector needs to be deleted, but it wont free the memory of the matrix since its a subpart of the whole matrix
  virtual const Container::HostVector* getData(const EnvironmentFactor& environmentFactor) const;
  virtual int getNumberOfIndividualsToInclude() const;

  EnvironmentFactorHandler(const EnvironmentFactorHandler&) = delete;
  EnvironmentFactorHandler(EnvironmentFactorHandler&&) = delete;
  EnvironmentFactorHandler& operator=(const EnvironmentFactorHandler&) = delete;
  EnvironmentFactorHandler& operator=(EnvironmentFactorHandler&&) = delete;

private:
  Container::HostMatrix* dataMatrix;
  std::vector<EnvironmentFactor*>* environmentFactors;
  std::vector<const EnvironmentFactor*> constEnvironmentFactors;
  const int numberOfColumns;
  const int numberOfIndividualsToInclude;
};

} /* namespace CuEira */

#endif /* ENVIRONMENTFACTORHANDLER_H_ */
