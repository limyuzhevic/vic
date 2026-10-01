#pragma once

#include "NLMException.hpp"

namespace nlm {

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& message, const std::string& code = "AGENT_ERROR")
        : NLMException(message, code) {}
    
    AgentError(const std::string& message, const std::string& code,
              const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm