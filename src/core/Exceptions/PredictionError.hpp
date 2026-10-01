#pragma once

#include "NLMException.hpp"

namespace nlm {

class PredictionError : public NLMException {
public:
    explicit PredictionError(const std::string& message, const std::string& code = "PREDICTION_ERROR")
        : NLMException(message, code) {}
    
    PredictionError(const std::string& message, const std::string& code,
                   const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm