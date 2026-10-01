#pragma once

#include "NLMException.hpp"

namespace nlm {

class MemorySystemError : public NLMException {
public:
    explicit MemorySystemError(const std::string& message, const std::string& code = "MEMORY_SYSTEM_ERROR")
        : NLMException(message, code) {}
    
    MemorySystemError(const std::string& message, const std::string& code,
                     const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm