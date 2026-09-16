#include "ErrorHandling/Error.hpp"
#include "Logger/Logger.hpp"
#include <stdexcept>
#include <iostream>

namespace nlm {

void logException(const std::exception& e) {
    const NLMError* nlmError = dynamic_cast<const NLMError*>(&e);
    if (nlmError) {
        Logger::getGlobal().error("Exception caught: " + std::string(e.what()) + 
                                 " (Code: " + std::to_string(static_cast<int>(nlmError->getCode())) + ")");
    } else {
        Logger::getGlobal().error("Exception caught: " + std::string(e.what()));
    }
}

} // namespace nlm
