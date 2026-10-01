#pragma once

#include "NLMException.hpp"

namespace nlm {

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message, const std::string& code = "WORLD_ERROR")
        : NLMException(message, code) {}
    
    WorldError(const std::string& message, const std::string& code,
              const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm