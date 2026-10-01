#pragma once

#include "NLMException.hpp"

namespace nlm {

class CognitiveError : public NLMException {
public:
    explicit CognitiveError(const std::string& message, const std::string& code = "COGNITIVE_ERROR")
        : NLMException(message, code) {}
    
    CognitiveError(const std::string& message, const std::string& code,
                  const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm