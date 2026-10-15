#include "MemoryManagementPlugin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MemoryManagementPlugin::MemoryManagementPlugin()
    : brain_(nullptr)
    , config_(nullptr)
    , capacityMultiplier_(1.0f)
    , consolidationEnabled_(true)
    , consolidationThreshold_(0.8f)
    , originalCapacity_(0)
    , originalMaxEpisodes_(0)
{
}

MemoryManagementPlugin::~MemoryManagementPlugin() = default;

std::string MemoryManagementPlugin::getName() const {
    return "MemoryManagementPlugin";
}

std::string MemoryManagementPlugin::getVersion() const {
    return "1.0.0";
}

std::string MemoryManagementPlugin::getDescription() const {
    return "Advanced memory management plugin for NLM brain";
}

std::string MemoryManagementPlugin::getType() const {
    return "memory_management";
}

bool MemoryManagementPlugin::initialize(Brain* brain, std::shared_ptr<Config> config) {
    if (!brain || !config) {
        return false;
    }
    
    brain_ = brain;
    config_ = config;
    
    // Store original capacities
    originalCapacity_ = config_->getOr<size_t>("working_memory.capacity", 0);
    originalMaxEpisodes_ = config_->getOr<size_t>("episodic_memory.max_episodes", 0);
    
    // Apply capacity multipliers from config
    float memoryMultiplier = config_->getOr<float>("plugin.memory_management.capacity_multiplier", 1.0f);
    if (memoryMultiplier > 0.0f) {
        capacityMultiplier_ = memoryMultiplier;
    }
    
    // Check if consolidation is enabled
    consolidationEnabled_ = config_->getOr<bool>("plugin.memory_management.consolidation_enabled", true);
    consolidationThreshold_ = config_->getOr<float>("plugin.memory_management.consolidation_threshold", 0.8f);
    
    NLM_LOG_INFO("MemoryManagementPlugin initialized successfully");
    NLM_LOG_INFO("  Original working memory capacity: " + std::to_string(originalCapacity_));
    NLM_LOG_INFO("  Original max episodes: " + std::to_string(originalMaxEpisodes_));
    NLM_LOG_INFO("  Capacity multiplier: " + std::to_string(capacityMultiplier_));
    NLM_LOG_INFO("  Consolidation enabled: " + std::to_string(consolidationEnabled_));
    
    return true;
}

void MemoryManagementPlugin::update(double timestep) {
    if (!brain_ || !config_) {
        return;
    }
    
    // Adaptive capacity adjustment based on memory usage
    if (auto* workingMemory = brain_->getWorkingMemory()) {
        size_t currentTraces = workingMemory->getActiveTraces();
        float utilization = currentTraces > 0 ? 
            static_cast<float>(currentTraces) / originalCapacity_ : 0.0f;
        
        if (utilization > consolidationThreshold_ && consolidationEnabled_) {
            // Enable consolidation when utilization is high
            NLM_LOG_INFO("MemoryUtilization high (" + std::to_string(utilization * 100) + "%)");
        }
    }
    
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        size_t currentEpisodes = episodicMemory->getEpisodeCount();
        float utilization = currentEpisodes > 0 ? 
            static_cast<float>(currentEpisodes) / originalMaxEpisodes_ : 0.0f;
        
        if (utilization > consolidationThreshold_ && consolidationEnabled_) {
            // Periodically consolidate when episodic memory is heavily used
            if (static_cast<size_t>(timestep) % 1000 == 0) {
                NLM_LOG_INFO("Episodic memory utilization high (" + std::to_string(utilization * 100) + "%)");
                episodicMemory->consolidate(consolidationThreshold_);
            }
        }
    }
}

void MemoryManagementPlugin::shutdown() {
    NLM_LOG_INFO("MemoryManagementPlugin shutting down");
    brain_ = nullptr;
    config_ = nullptr;
}

void MemoryManagementPlugin::setCapacityMultiplier(float multiplier) {
    if (multiplier > 0.0f) {
        capacityMultiplier_ = multiplier;
        NLM_LOG_INFO("MemoryManagementPlugin capacity multiplier set to: " + std::to_string(multiplier));
    }
}

void MemoryManagementPlugin::enableConsolidation(bool enable) {
    consolidationEnabled_ = enable;
    NLM_LOG_INFO("MemoryManagementPlugin consolidation " + std::string(enable ? "enabled" : "disabled"));
}

void MemoryManagementPlugin::setConsolidationThreshold(float threshold) {
    if (threshold >= 0.0f && threshold <= 1.0f) {
        consolidationThreshold_ = threshold;
        NLM_LOG_INFO("MemoryManagementPlugin consolidation threshold set to: " + std::to_string(threshold));
    }
}

}} // namespace nlm