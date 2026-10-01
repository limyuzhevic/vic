#pragma once

#include "NLMException.hpp"

namespace nlm {

class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message, const std::string& code = "INTEGRATION_ERROR")
        : NLMException(message, code) {}
    
    IntegrationError(const std::string& message, const std::string& code,
                    const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm