// ExperimentRegistry.cpp implementation
#include "ExperimentRegistry.hpp"

namespace nlm {

// Static members definition
std::map<std::string, ExperimentRegistry::ExperimentInfo> ExperimentRegistry::registry;
std::map<std::type_index, ExperimentRegistry::ExperimentFactory> ExperimentRegistry::factories;

std::unique_ptr<IExperiment> ExperimentRegistry::createExperiment(const std::string& name) {
    std::string normalizedName = normalizeName(name);
    
    auto it = registry.find(normalizedName);
    if (it == registry.end()) {
        return nullptr; // Not found
    }
    
    // In a real implementation, we would instantiate the experiment
    // For now, return a generic experiment
    return std::make_unique<Experiment>(name);
}

ExperimentRegistry::ExperimentInfo* ExperimentRegistry::getExperimentInfo(const std::string& name) {
    std::string normalizedName = normalizeName(name);
    
    auto it = registry.find(normalizedName);
    if (it != registry.end()) {
        return &it->second;
    }
    
    return nullptr;
}

bool ExperimentRegistry::isExperimentRegistered(const std::string& name) {
    std::string normalizedName = normalizeName(name);
    return registry.find(normalizedName) != registry.end();
}

} // namespace nlm
