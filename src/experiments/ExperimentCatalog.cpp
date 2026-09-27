#include "ExperimentEnhanced.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct ExperimentCatalog::Impl {
    std::vector<std::unique_ptr<ExperimentFactory>> factories;
    std::unordered_map<std::string, std::unique_ptr<ExperimentFactory>> factoryMap;
    std::unordered_map<std::string, std::map<std::string, ExperimentParameter>> defaultConfigs;
};

ExperimentCatalog::ExperimentCatalog() : pImpl(new Impl) {}

ExperimentCatalog::~ExperimentCatalog() = default;

void ExperimentCatalog::registerFactory(std::unique_ptr<ExperimentFactory> factory) {
    if (!factory) {
        throw std::invalid_argument("Factory cannot be null");
    }
    
    std::string type = factory->getType();
    if (factoryMap.find(type) != factoryMap.end()) {
        throw std::runtime_error("Experiment type already registered: " + type);
    }
    
    pImpl->factories.push_back(std::move(factory));
    pImpl->factoryMap[type] = std::move(pImpl->factories.back());
}

bool ExperimentCatalog::hasType(const std::string& type) const {
    return factoryMap.find(type) != factoryMap.end();
}

std::unique_ptr<Experiment> ExperimentCatalog::createExperiment(const std::string& type, const std::string& name) {
    auto it = factoryMap.find(type);
    if (it == factoryMap.end()) {
        throw std::runtime_error("Unknown experiment type: " + type);
    }
    
    return it->second->create(name);
}

std::vector<ExperimentType> ExperimentCatalog::getExperimentTypes() const {
    std::vector<ExperimentType> types;
    types.reserve(factoryMap.size());
    for (const auto& pair : factoryMap) {
        types.push_back(pair.first);
    }
    return types;
}

const std::string& ExperimentCatalog::getExperimentDescription(const std::string& type) const {
    auto it = factoryMap.find(type);
    if (it == factoryMap.end()) {
        static const std::string empty = "";
        return empty;
    }
    return it->second->getDescription();
}

bool ExperimentCatalog::validateConfig(const std::string& type, const std::map<std::string, ExperimentParameter>& config) const {
    auto it = factoryMap.find(type);
    if (it == factoryMap.end()) {
        return false;
    }
    return it->second->validateConfig(config);
}

std::map<std::string, ExperimentParameter> ExperimentCatalog::getDefaultConfig(const std::string& type) const {
    // Default configuration would be stored in the factory or catalog
    // For now, return an empty map
    return std::map<std::string, ExperimentParameter>();
}

} // namespace nlm
