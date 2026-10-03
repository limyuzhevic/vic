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
    Observation(nullptr);
    ~Observation(nullptr) = default;
    
    // Get data
    const std::vector<float>& getData(nullptr) const;
    size_t getDimensions(nullptr) const;
    
    // Set from sensory percept
    void setFromPercept(const class SensoryPercept& percept);
    
private:
    std::vector<float> data_;
};

} // namespace nlm
