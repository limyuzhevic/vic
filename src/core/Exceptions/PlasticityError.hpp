#pragma once

#include "NLMException.hpp"

namespace nlm {

class PlasticityError : public NLMException {
public:
    explicit PlasticityError(const std::string& message, const std::string& code = "PLASTICITY_ERROR")
        : NLMException(message, code) {}
    
    PlasticityError(const std::string& message, const std::string& code,
                   const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm