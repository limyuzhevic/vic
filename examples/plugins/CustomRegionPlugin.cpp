// Example Custom Region Plugin
// Demonstrates creating a custom brain region type

#include "../include/nlm/plugin/region/RegionPlugin.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include <string>
#include <vector>

class CustomRegionPlugin : public nlm::RegionPlugin {
public:
    NLM_PLUGIN_IMPLEMENTATION
    
    // Custom region factory
    nlm::NeuralRegion* createRegion(const std::string& name = "") override {
        return new nlm::NeuralRegion(nlm::RegionId(0), "Custom " + name);
    }
    
    nlm::NeuralRegion* createRegion(nlm::RegionId id, const std::string& name = "") override {
        return new nlm::NeuralRegion(id, "Custom " + name);
    }
    
    bool canCreateRegion(const std::string& regionType) const override {
        return regionType == "custom" || regionType == "cortical" || regionType == "subcortical";
    }
    
    std::vector<std::string> getSupportedRegionTypes() const override {
        return {"custom", "cortical", "subcortical", "associative", "working"};
    }
    
    bool configureRegion(nlm::NeuralRegion& region, const std::string& config) override {
        // Parse configuration and apply to region
        // This is a simple example - real implementation would parse JSON or key=value format
        if (config.find("density=") != std::string::npos) {
            // Extract density value and configure region connectivity
        }
        return true;
    }
    
    std::string getRegionConfig(const nlm::NeuralRegion& region) const override {
        return "custom_region_config:" + region.getName();
    }
};

NLM_PLUGIN_FACTORY(CustomRegionPlugin)