#ifndef CPUMISSINGDATAHANDLER_H_
#define CPUMISSINGDATAHANDLER_H_

#include <MissingDataHandler.h>
#include <HostVector.h>
#include <RegularHostVector.h>

namespace CuEira {
namespace CPU {

/*
 * This class ...
 *
 *  @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class CpuMissingDataHandler: public MissingDataHandler<Container::RegularHostVector> {
public:
  explicit CpuMissingDataHandler(const int numberOfIndividualsTotal);
  virtual ~CpuMissingDataHandler();

  virtual void copyNonMissing(const Container::RegularHostVector& fromVector,
      Container::RegularHostVector& toVector) const;

protected:

};

} /* namespace CPU */
} /* namespace CuEira */

#endif /* CPUMISSINGDATAHANDLER_H_ */
