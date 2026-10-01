#pragma once

#include "NLMException.hpp"

namespace nlm {

class DevelopmentError : public NLMException {
public:
    explicit DevelopmentError(const std::string& message, const std::string& code = "DEVELOPMENT_ERROR")
        : NLMException(message, code) {}
    
    DevelopmentError(const std::string& message, const std::string& code,
                    const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm