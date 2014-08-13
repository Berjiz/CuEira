#ifndef DATAQUEUE_H_
#define DATAQUEUE_H_

#include <utility>
#include <vector>
#include <sstream>

#include <Id.h>
#include <SNP.h>
#include <InvalidState.h>

namespace CuEira {
namespace Task {

/**
 * This is
 *
 * @author Daniel Berglund daniel.k.berglund@gmail.com
 */
class DataQueue {
public:
  DataQueue(std::vector<SNP*> snpQueue);
  virtual ~DataQueue();

  bool hasNext();
  SNP* next();

private:
  std::vector<SNP*> snpQueue;
};

} /* namespace Task */
} /* namespace CuEira */

#endif /* DATAQUEUE_H_ */
