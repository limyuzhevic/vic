#include "factories/MemorySystemFactory.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

struct MemorySystemFactory::Impl {
    size_t creationCount;
    std::string lastCreatedType;
};

MemorySystemFactory::MemorySystemFactory() : creationCount_(0) {
    pImpl_ = std::make_unique<Impl>();
    pImpl_->creationCount = 0;
    pImpl_->lastCreatedType = "None";
}

MemorySystemFactory::~MemorySystemFactory() = default;

std::unique_ptr<AbstractMemorySystem> MemorySystemFactory::createMemorySystem(
    MemorySystemType type,
    std::shared_ptr<Config> config
) {
    auto memorySystem = createFromConfig(type, config);
    ++pImpl_->creationCount;
    ++creationCount_;
    return memorySystem;
}

std::unique_ptr<AbstractMemorySystem> MemorySystemFactory::createMemorySystem(
    const std::string& memorySystemName,
    std::shared_ptr<Config> config
) {
    MemorySystemType type;
    
    // Map string names to types
    if (memorySystemName == "WorkingMemory") type = MemorySystemType::WorkingMemory;
    else if (memorySystemName == "EpisodicMemory") type = MemorySystemType::EpisodicMemory;
    else if (memorySystemName == "AssociativeMemory") type = MemorySystemType::AssociativeMemory;
    else if (memorySystemName == "NeuralWorkingMemory") type = MemorySystemType::NeuralWorkingMemory;
    else if (memorySystemName == "NeuralEpisodicMemory") type = MemorySystemType::NeuralEpisodicMemory;
    else type = MemorySystemType::WorkingMemory; // Default to WorkingMemory
    
    return createMemorySystem(type, config);
}

std::unique_ptr<AbstractMemorySystem> MemorySystemFactory::createFromConfig(
    MemorySystemType type,
    std::shared_ptr<Config> config
) {
    switch (type) {
        case MemorySystemType::WorkingMemory:
            return createWorkingMemory(config);
        case MemorySystemType::EpisodicMemory:
            return createEpisodicMemory(config);
        case MemorySystemType::AssociativeMemory:
            return createAssociativeMemory(config);
        case MemorySystemType::NeuralWorkingMemory:
        case MemorySystemType::NeuralEpisodicMemory:
            // Neural memory systems would need additional Brain reference
            return nullptr;
        default:
            return nullptr;
    }
}

std::unique_ptr<AbstractMemorySystem> MemorySystemFactory::createFromConfig(
    const Config& config
) {
    // Get memory system type from config
    std::string memorySystemType = config.get_or<std::string>("memory_system_type", "WorkingMemory");
    
    // Map type name to enum
    MemorySystemType type;
    if (memorySystemType == "WorkingMemory") type = MemorySystemType::WorkingMemory;
    else if (memorySystemType == "EpisodicMemory") type = MemorySystemType::EpisodicMemory;
    else if (memorySystemType == "AssociativeMemory") type = MemorySystemType::AssociativeMemory;
    else if (memorySystemType == "NeuralWorkingMemory") type = MemorySystemType::NeuralWorkingMemory;
    else if (memorySystemType == "NeuralEpisodicMemory") type = MemorySystemType::NeuralEpisodicMemory;
    else type = MemorySystemType::WorkingMemory; // Default to WorkingMemory
    
    return createFromConfig(type, std::make_shared<Config>(config));
}

std::unique_ptr<AbstractMemorySystem> MemorySystemFactory::createFromPlugin(
    const std::string& pluginName,
    std::shared_ptr<Config> config
) {
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        auto memorySystem = it->second(config);
        ++pImpl_->creationCount;
        ++creationCount_;
        return memorySystem;
    }
    
    NLM_LOG_ERROR("Plugin not found: " + pluginName);
    return nullptr;
}

void MemorySystemFactory::configureFromConfig(AbstractMemorySystem* memorySystem, const Config& config) {
    if (!memorySystem) return;
    
    NLM_LOG_INFO("Configuring memory system from config");
}

void MemorySystemFactory::configureFromConfig(WorkingMemory* workingMemory, const Config& config) {
    if (!workingMemory) return;
    
    size_t capacity = config.get_or<size_t>("working_memory_capacity", 100);
    float decayRate = config.get_or<float>("working_memory_decay_rate", 0.01f);
    
    workingMemory->setCapacity(capacity);
    workingMemory->setDecayRate(decayRate);
}

void MemorySystemFactory::configureFromConfig(EpisodicMemory* episodicMemory, const Config& config) {
    if (!episodicMemory) return;
    
    size_t maxEpisodes = config.get_or<size_t>("episodic_memory_max_episodes", 1000);
    float consolidationThreshold = config.get_or<float>("episodic_memory_consolidation_threshold", 0.3f);
    
    episodicMemory->setMaxEpisodes(maxEpisodes);
    episodicMemory->setConsolidationThreshold(consolidationThreshold);
}

void MemorySystemFactory::configureFromConfig(AssociativeMemory* associativeMemory, const Config& config) {
    if (!associativeMemory) return;
    
    size_t maxAssociations = config.get_or<size_t>("associative_memory_max_associations", 1000);
    float similarityThreshold = config.get_or<float>("associative_memory_similarity_threshold", 0.5f);
    
    associativeMemory->setMaxAssociations(maxAssociations);
    associativeMemory->setSimilarityThreshold(similarityThreshold);
}

bool MemorySystemFactory::registerPlugin(
    const std::string& name,
    std::function<std::unique_ptr<AbstractMemorySystem>(std::shared_ptr<Config>)> factory
) {
    plugins_[name] = factory;
    NLM_LOG_INFO("Registered memory system plugin: " + name);
    return true;
}

std::unique_ptr<AbstractMemorySystem> MemorySystemFactory::createFromRegisteredPlugin(
    const std::string& name,
    std::shared_ptr<Config> config
) {
    return createFromPlugin(name, config);
}

std::vector<std::string> MemorySystemFactory::getRegisteredPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

} // namespace nlm
