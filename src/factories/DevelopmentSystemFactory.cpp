#include "factories/DevelopmentSystemFactory.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystemFactory::Impl {
    size_t creationCount;
    std::string lastCreatedType;
};

DevelopmentSystemFactory::DevelopmentSystemFactory() : creationCount_(0) {
    pImpl_ = std::make_unique<Impl>();
    pImpl_->creationCount = 0;
    pImpl_->lastCreatedType = "None";
}

DevelopmentSystemFactory::~DevelopmentSystemFactory() = default;

std::unique_ptr<AbstractDevelopmentSystem> DevelopmentSystemFactory::createDevelopmentSystem(
    DevelopmentSystemType type,
    std::shared_ptr<Config> config
) {
    auto developmentSystem = createFromConfig(type, config);
    ++pImpl_->creationCount;
    ++creationCount_;
    return developmentSystem;
}

std::unique_ptr<AbstractDevelopmentSystem> DevelopmentSystemFactory::createDevelopmentSystem(
    const std::string& developmentSystemName,
    std::shared_ptr<Config> config
) {
    DevelopmentSystemType type;
    
    // Map string names to types
    if (developmentSystemName == "Synaptogenesis") type = DevelopmentSystemType::Synaptogenesis;
    else if (developmentSystemName == "Pruning") type = DevelopmentSystemType::Pruning;
    else if (developmentSystemName == "Maturation") type = DevelopmentSystemType::Maturation;
    else if (developmentSystemName == "DevelopmentSystem") type = DevelopmentSystemType::FullDevelopmentSystem;
    else type = DevelopmentSystemType::FullDevelopmentSystem; // Default to full system
    
    return createDevelopmentSystem(type, config);
}

std::unique_ptr<AbstractDevelopmentSystem> DevelopmentSystemFactory::createFromConfig(
    DevelopmentSystemType type,
    std::shared_ptr<Config> config
) {
    switch (type) {
        case DevelopmentSystemType::Synaptogenesis:
            return createSynaptogenesis(config);
        case DevelopmentSystemType::Pruning:
            return createPruning(config);
        case DevelopmentSystemType::Maturation:
            return createMaturation(config);
        case DevelopmentSystemType::FullDevelopmentSystem:
            return createDevelopmentSystem(config);
        default:
            return nullptr;
    }
}

std::unique_ptr<AbstractDevelopmentSystem> DevelopmentSystemFactory::createFromConfig(
    const Config& config
) {
    // Get development system type from config
    std::string developmentSystemType = config.get_or<std::string>("development_system_type", "DevelopmentSystem");
    
    // Map type name to enum
    DevelopmentSystemType type;
    if (developmentSystemType == "Synaptogenesis") type = DevelopmentSystemType::Synaptogenesis;
    else if (developmentSystemType == "Pruning") type = DevelopmentSystemType::Pruning;
    else if (developmentSystemType == "Maturation") type = DevelopmentSystemType::Maturation;
    else if (developmentSystemType == "DevelopmentSystem") type = DevelopmentSystemType::FullDevelopmentSystem;
    else type = DevelopmentSystemType::FullDevelopmentSystem; // Default to full system
    
    return createFromConfig(type, std::make_shared<Config>(config));
}

std::unique_ptr<AbstractDevelopmentSystem> DevelopmentSystemFactory::createFromPlugin(
    const std::string& pluginName,
    std::shared_ptr<Config> config
) {
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        auto developmentSystem = it->second(config);
        ++pImpl_->creationCount;
        ++creationCount_;
        return developmentSystem;
    }
    
    NLM_LOG_ERROR("Plugin not found: " + pluginName);
    return nullptr;
}

void DevelopmentSystemFactory::configureFromConfig(AbstractDevelopmentSystem* developmentSystem, const Config& config) {
    if (!developmentSystem) return;
    
    NLM_LOG_INFO("Configuring development system from config");
}

void DevelopmentSystemFactory::configureFromConfig(Synaptogenesis* synaptogenesis, const Config& config) {
    if (!synaptogenesis) return;
    
    float rate = config.get_or<float>("synaptogenesis_rate", 0.0001f);
    float maxConnections = config.get_or<float>("synaptogenesis_max_connections", 1.0f);
    
    synaptogenesis->setRate(rate);
    synaptogenesis->setMaxConnections(maxConnections);
}

void DevelopmentSystemFactory::configureFromConfig(Pruning* pruning, const Config& config) {
    if (!pruning) return;
    
    float rate = config.get_or<float>("pruning_rate", 0.00001f);
    float survivalThreshold = config.get_or<float>("pruning_survival_threshold", 0.1f);
    
    pruning->setRate(rate);
    pruning->setSurvivalThreshold(survivalThreshold);
}

void DevelopmentSystemFactory::configureFromConfig(Maturation* maturation, const Config& config) {
    if (!maturation) return;
    
    float maturationRate = config.get_or<float>("maturation_rate", 0.001f);
    float threshold = config.get_or<float>("maturation_threshold", 0.5f);
    
    maturation->setMaturationRate(maturationRate);
    maturation->setThreshold(threshold);
}

bool DevelopmentSystemFactory::registerPlugin(
    const std::string& name,
    std::function<std::unique_ptr<AbstractDevelopmentSystem>(std::shared_ptr<Config>)> factory
) {
    plugins_[name] = factory;
    NLM_LOG_INFO("Registered development system plugin: " + name);
    return true;
}

std::unique_ptr<AbstractDevelopmentSystem> DevelopmentSystemFactory::createFromRegisteredPlugin(
    const std::string& name,
    std::shared_ptr<Config> config
) {
    return createFromPlugin(name, config);
}

std::vector<std::string> DevelopmentSystemFactory::getRegisteredPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

} // namespace nlm
