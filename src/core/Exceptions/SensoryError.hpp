#pragma once

#include "NLMException.hpp"

namespace nlm {

class SensoryError : public NLMException {
public:
    explicit SensoryError(const std::string& message, const std::string& code = "SENSORY_ERROR")
        : NLMException(message, code) {}
    
    SensoryError(const std::string& message, const std::string& code,
                const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm