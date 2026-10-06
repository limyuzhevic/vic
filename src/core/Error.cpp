#include "Error.h"
#include <stdexcept>
#include <mutex>

namespace nlm {

ErrorManager* ErrorManager::instance = nullptr;

} // namespace nlm
