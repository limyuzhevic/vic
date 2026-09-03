#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class AgentBody;
class SensoryPercept;

// Sensory observation wrapper (simplified for compatibility)
class Observation {
public:
    Observation();
    ~Observation() = default;
    
    // Get data
    const std::vector<float>& getData() const;
    size_t getDimensions() const;
    
    // Set from sensory percept
    void setFromPercept(const class SensoryPercept& percept);
    
private:
    std::vector<float> data_;
};

} // namespace nlm
