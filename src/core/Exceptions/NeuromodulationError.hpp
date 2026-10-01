#pragma once

#include "NLMException.hpp"

namespace nlm {

class NeuromodulationError : public NLMException {
public:
    explicit NeuromodulationError(const std::string& message, const std::string& code = "NEUROMODULATION_ERROR")
        : NLMException(message, code) {}
    
    NeuromodulationError(const std::string& message, const std::string& code,
                        const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm