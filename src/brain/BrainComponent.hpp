// BrainComponent.hpp - Abstract base class for brain components
#pragma once

#include <string>

namespace nlm {

// Abstract base class for all brain components
// Components can be neural dynamics, plasticity, memory, cognition, etc.
class BrainComponent {
public:
    virtual ~BrainComponent() = default;
    
    // Initialize the component
    virtual bool initialize() = 0;
    
    // Update component state
    virtual void update() = 0;
    
    // Get component name for identification and configuration
    virtual std::string getName() const = 0;
    
    // Get component type for factory registration
    virtual std::string getType() const = 0;
    
    // Check if component is active
    virtual bool isActive() const = 0;
    
    // Activate/deactivate component
    virtual void setActive(bool active) = 0;
    
    // Save component state (for checkpointing)
    virtual bool save(const std::string& filepath) const = 0;
    
    // Load component state
    virtual bool load(const std::string& filepath) = 0;
    
    // Get component configuration (virtual)
    virtual std::string getConfiguration() const { return ""; }
};

} // namespace nlm
