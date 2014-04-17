#ifndef ID_H_
#define ID_H_

#include <string>
#include <sstream>
#include <iostream>

namespace CuEira {

/**
 * This class represents ids
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class Id {
public:
  explicit Id(std::string id);
  virtual ~Id();

private:
  std::string id;
};

} /* namespace CuEira */

#endif /* ID_H_ */