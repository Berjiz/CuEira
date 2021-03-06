#ifndef ID_H_
#define ID_H_

#include <string>
#include <sstream>
#include <iostream>
#include <ostream>

namespace CuEira {

/**
 * This class represents ids
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class Id {
  friend std::ostream& operator<<(std::ostream& os, const Id& id);
public:
  explicit Id(std::string id);
  virtual ~Id();

  const std::string getString() const;

  bool operator<(const Id& otherId) const;
  bool operator==(const Id& otherId) const;

private:
  const std::string id;
};

} /* namespace CuEira */

#endif /* ID_H_ */
