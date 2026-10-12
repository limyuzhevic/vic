// Example Custom Sensory Plugin
// Demonstrates creating a new sensory modality

#include "../include/nlm/plugin/sensory/SensoryPlugin.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include <string>
#include <vector>

class CustomSensoryPlugin : public nlm::SensoryPlugin {
public:
    NLM_PLUGIN_IMPLEMENTATION
    
    // Custom sensory factory
    nlm::SensoryInput* createSensoryModality() override {
        return new nlm::SensoryInput();
    }
    
    bool canCreateSensoryModality(const std::string& modalityType) const override {
        return modalityType == "vision" || modalityType == "audio" || modalityType == "olfactory" ||
               modalityType == "gustatory" || modalityType == "proprioceptive" ||
               modalityType == "interoceptive" || modalityType == "magnetic" ||
               modalityType == "electromagnetic" || modalityType == "quantum";
    }
    
    std::vector<std::string> getSupportedSensoryModalityTypes() const override {
        return {"vision", "audio", "olfactory", "gustatory", "proprioceptive", "interoceptive",
                "magnetic", "electromagnetic", "quantum", "thermal", "pressure", "pain"};
    }
    
    bool configureSensoryModality(nlm::SensoryInput& modality, const std::string& config) override {
        // Parse configuration and apply to modality
        if (config.find("sensitivity=") != std::string::npos) {
            // Extract sensitivity and configure modality
        }
        return true;
    }
    
    std::string getSensoryModalityConfig(const nlm::SensoryInput& modality) const override {
        return "custom_sensory_modality:sensitivity=1.0;range=[0,1]";
    }
};

NLM_PLUGIN_FACTORY(CustomSensoryPlugin)