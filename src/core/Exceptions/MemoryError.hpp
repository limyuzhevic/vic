#pragma once

#include "NLMException.hpp"

namespace nlm {

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message, const std::string& code = "MEMORY_ERROR")
        : NLMException(message, code) {}
    
    MemoryError(const std::string& message, const std::string& code,
               const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm