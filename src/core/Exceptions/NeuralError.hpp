#pragma once

#include "NLMException.hpp"

namespace nlm {

class NeuralError : public NLMException {
public:
    explicit NeuralError(const std::string& message, const std::string& code = "NEURAL_ERROR")
        : NLMException(message, code) {}
    
    NeuralError(const std::string& message, const std::string& code,
               const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm