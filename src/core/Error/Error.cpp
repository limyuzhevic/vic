#include "Error.hpp"
#include <algorithm>

namespace nlm {

std::vector<NLMException> ErrorHandler::lastErrors_;
std::function<void(const NLMException&)> ErrorHandler::errorHandler_;

} // namespace nlm
