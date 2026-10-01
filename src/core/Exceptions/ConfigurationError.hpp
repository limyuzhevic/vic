#pragma once

#include "NLMException.hpp"

namespace nlm {

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message, const std::string& code = "CONFIG_ERROR")
        : NLMException(message, code) {}
    
    ConfigurationError(const std::string& message, const std::string& code,
                      const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm