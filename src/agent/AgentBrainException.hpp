#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

namespace nlm {

// Exception class for AgentBrain errors
class AgentBrainException : public std::runtime_error {
public:
    explicit AgentBrainException(const std::string& message)
        : std::runtime_error("AgentBrain: " + message) {}
    
    static AgentBrainException nullPointerError(const std::string& component) {
        std::stringstream ss;
        ss << "Null pointer error: " << component;
        return AgentBrainException(ss.str());
    }
    
    static AgentBrainException invalidValueError(const std::string& operation, float value, float expectedMin, float expectedMax) {
        std::stringstream ss;
        ss << "Invalid value for " << operation << ": " << value 
           << " (expected range: [" << expectedMin << ", " << expectedMax << "])";
        return AgentBrainException(ss.str());
    }
};

} // namespace nlm
