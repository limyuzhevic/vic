#pragma once

#include "NLMPlugin.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class SensoryPlugin
 * @brief Plugin interface for custom sensory modalities
 */
class SensoryPlugin : public virtual NLMPlugin {
public:
    virtual ~SensoryPlugin() = default;
    
    // Type check
    bool isSensoryPlugin() const override { return true; }
    
    // Sensory factory methods
    virtual void* getSensoryFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for sensory
    virtual SensoryInput* createSensoryModality() = 0;
    virtual bool canCreateSensoryModality(const std::string& modalityType) const = 0;
    virtual std::vector<std::string> getSupportedSensoryModalityTypes() const = 0;
    
    // Sensory configuration
    virtual bool configureSensoryModality(SensoryInput& modality, const std::string& config) = 0;
    virtual std::string getSensoryModalityConfig(const SensoryInput& modality) const = 0;
};

} // namespace nlm