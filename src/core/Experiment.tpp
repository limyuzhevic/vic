#include "Experiment.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Global experiment configuration instance
static std::unique_ptr<ExperimentConfig> gExperimentConfig = nullptr;

// ExperimentConfig implementation
ExperimentConfig::~ExperimentConfig() = default;

// Global experiment configuration access functions
ExperimentConfig& getExperimentConfig() {
    if (!gExperimentConfig) {
        gExperimentConfig = ExperimentConfigFactory::createConfig("default");
    }
    return *gExperimentConfig;
}

void setExperimentConfig(std::unique_ptr<ExperimentConfig> config) {
    gExperimentConfig = std::move(config);
}

std::string getCurrentExperimentType() {
    return gExperimentConfig ? gExperimentConfig->getType() : "none";
}

} // namespace nlm
