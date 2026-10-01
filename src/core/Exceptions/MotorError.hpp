#pragma once

#include "NLMException.hpp"

namespace nlm {

class MotorError : public NLMException {
public:
    explicit MotorError(const std::string& message, const std::string& code = "MOTOR_ERROR")
        : NLMException(message, code) {}
    
    MotorError(const std::string& message, const std::string& code,
              const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm