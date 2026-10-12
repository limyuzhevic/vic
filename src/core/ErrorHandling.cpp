#include "ErrorHandling.hpp"

namespace nlm {

std::function<void(const NLMException&)> ExceptionManager::errorHandler = nullptr;

} // namespace nlm
