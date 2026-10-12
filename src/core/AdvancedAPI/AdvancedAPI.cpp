#include "AdvancedAPI.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/Memory.hpp"
#include "../environment/Environment.hpp"
#include <algorithm>

namespace nlm {

// AgentConfiguration implementation
Config AgentConfiguration::createLearningAgentConfig() {
    Config config;
    config.set("agent.type", std::string("learning"));
    config.set("brain.plasticity.enabled", true);
    config.set("brain.development.enabled", true);
    config.set("brain.reward.modulation", true);
    config.set("brain.curiosity.enabled", true);
    config.set("memory.episodic.enabled", true);
    config.set("memory.working.capacity", size_t(1000));
    config.set("agent.exploration.rate", 0.1f);
    config.set("agent.learning.rate", 0.01f);
    config.set("agent.noise.level", 0.05f);
    
    config.addTag("learning");
    config.addTag("adaptive");
    
    return config;
}

Config AgentConfiguration::createReactiveAgentConfig() {
    Config config;
    config.set("agent.type", std::string("reactive"));
    config.set("brain.plasticity.enabled", false);
    config.set("brain.development.enabled", false);
    config.set("brain.reward.modulation", true);
    config.set("brain.curiosity.enabled", false);
    config.set("memory.episodic.enabled", false);
    config.set("agent.response.speed", 1.0f);
    config.set("agent.memory.capacity", size_t(100));
    
    config.addTag("reactive");
    config.addTag("fast");
    
    return config;
}

Config AgentConfiguration::createMemoryAgentConfig() {
    Config config;
    config.set("agent.type", std::string("memory"));
    config.set("brain.plasticity.enabled", true);
    config.set("brain.development.enabled", true);
    config.set("brain.reward.modulation", true);
    config.set("brain.curiosity.enabled", true);
    config.set("memory.episodic.enabled", true);
    config.set("memory.working.capacity", size_t(5000));
    config.set("memory.episodic.capacity", size_t(10000));
    config.set("agent.memory.priority", 1.0f);
    config.set("agent.retention.time", 3600.0f);
    
    config.addTag("memory");
    config.addTag("persistent");
    
    return config;
}

Config AgentConfiguration::createExplorationAgentConfig() {
    Config config;
    config.set("agent.type", std::string("exploration"));
    config.set("brain.plasticity.enabled", true);
    config.set("brain.development.enabled", true);
    config.set("brain.reward.modulation", true);
    config.set("brain.curiosity.enabled", true);
    config.set("memory.episodic.enabled", true);
    config.set("agent.exploration.rate", 0.5f);
    config.set("agent.noise.level", 0.2f);
    config.set("agent.uncertainty.threshold", 0.3f);
    
    config.addTag("exploration");
    config.addTag("curious");
    
    return config;
}

Config AgentConfiguration::createRLAgentConfig() {
    Config config;
    config.set("agent.type", std::string("reinforcement_learning"));
    config.set("brain.plasticity.enabled", true);
    config.set("brain.development.enabled", false);
    config.set("brain.reward.modulation", true);
    config.set("brain.curiosity.enabled", false);
    config.set("memory.episodic.enabled", true);
    config.set("agent.learning.algorithm", std::string("q_learning"));
    config.set("agent.learning.rate", 0.001f);
    config.set("agent.discount.factor", 0.99f);
    config.set("agent.epsilon.greedy", 0.1f);
    config.set("agent.batch.size", size_t(32));
    
    config.addTag("rl");
    config.addTag("learning");
    
    return config;
}

bool AgentConfiguration::applyToBrain(Config& config, Brain* brain) {
    if (!brain) {
        return false;
    }
    
    try {
        // Apply various configurations to brain
        if (config.has("brain.plasticity.enabled")) {
            bool plasticityEnabled = config.getOr<bool>("brain.plasticity.enabled", false);
            // brain->enablePlasticity(plasticityEnabled);
        }
        
        if (config.has("brain.development.enabled")) {
            bool developmentEnabled = config.getOr<bool>("brain.development.enabled", false);
            // brain->enableDevelopment(developmentEnabled);
        }
        
        if (config.has("brain.reward.modulation")) {
            bool rewardModulation = config.getOr<bool>("brain.reward.modulation", false);
            // brain->enableRewardModulation(rewardModulation);
        }
        
        return true;
    } catch (const std::exception& e) {
        nlm::GlobalErrorHandler().logError(
            nlm::ErrorCode::ConfigInvalidParameter,
            std::string("Failed to apply configuration to brain: ") + e.what()
        );
        return false;
    }
}

Config AgentConfiguration::getTaskConfig(const std::string& taskType) {
    if (taskType == "navigation") {
        Config config = createLearningAgentConfig();
        config.set("agent.task.type", std::string("navigation"));
        config.set("agent.exploration.rate", 0.2f);
        return config;
    } else if (taskType == "object_manipulation") {
        Config config = createRLAgentConfig();
        config.set("agent.task.type", std::string("object_manipulation"));
        config.set("agent.action.precision", 0.95f);
        return config;
    } else if (taskType == "social_interaction") {
        Config config = createExplorationAgentConfig();
        config.set("agent.task.type", std::string("social_interaction"));
        config.set("agent.social.awareness", true);
        return config;
    }
    
    return createDefaultConfig();
}

// SimpleBrain implementation
std::unique_ptr<Brain> SimpleBrain::createFromConfig(const Config& config) {
    // In a real implementation, this would create a brain from config
    // For now, return nullptr as placeholder
    return nullptr;
}

std::unique_ptr<Brain> SimpleBrain::createDefault() {
    Config config = AgentConfiguration::createLearningAgentConfig();
    return createFromConfig(config);
}

std::unique_ptr<Brain> SimpleBrain::createForEnvironment(const Environment& environment) {
    Config config = AgentConfiguration::createReactiveAgentConfig();
    
    // Configure based on environment
    if (environment.getComplexity() > 0.7f) {
        config = AgentConfiguration::createMemoryAgentConfig();
    } else if (environment.getComplexity() < 0.3f) {
        config = AgentConfiguration::createExplorationAgentConfig();
    }
    
    return createFromConfig(config);
}

bool SimpleBrain::train(Brain* brain, const SensoryInput& input, float reward) {
    if (!brain) {
        return false;
    }
    
    try {
        // Process input and apply reward
        brain->receiveSensoryInput(input);
        
        // Create neuromodulation signal for reward
        Neuromodulator neuromodulator;
        neuromodulator.type = NeuromodulatorType::Dopamine;
        neuromodulator.strength = reward;
        neuromodulator.source = NeuromodulatorSource::Reward;
        
        brain->applyNeuromodulation(neuromodulator);
        
        return true;
    } catch (const std::exception& e) {
        nlm::GlobalErrorHandler().logError(
            nlm::ErrorCode::BrainInitializationFailed,
            std::string("Brain training failed: ") + e.what()
        );
        return false;
    }
}

std::map<std::string, double> SimpleBrain::getPerformanceMetrics(Brain* brain) {
    std::map<std::string, double> metrics;
    
    if (!brain) {
        metrics["success"] = 0.0;
        return metrics;
    }
    
    // Get basic metrics
    metrics["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
    metrics["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
    metrics["firing_rate"] = static_cast<double>(brain->getAverageFiringRate());
    metrics["excitatory_inhibitory_ratio"] = static_cast<double>(brain->getExcitationInhibitionRatio());
    
    // Calculate derived metrics
    if (metrics["neuron_count"] > 0) {
        metrics["spike_per_neuron"] = metrics["spike_count"] / metrics["neuron_count"];
    }
    
    return metrics;
}

// AgentContext implementation
AgentContext::AgentContext(AgentBrain* agent, Brain* brain, const SimpleWorld& world)
    : agent_(agent), brain_(brain), world_(std::addressof(world)), 
      metricsDirty_(true) {
}

MotorCommand AgentContext::process(const SensoryPercept& percept) {
    if (!agent_ || !brain_) {
        MotorCommand cmd;
        cmd.action = ActionType::Wait;
        cmd.value = 0.0f;
        return cmd;
    }
    
    metricsDirty_ = true;
    return agent_->processSensoryToMotor(percept);
}

std::string AgentContext::getStatus() const {
    if (!agent_ || !brain_) {
        return "Error: Agent or brain not initialized";
    }
    
    std::ostringstream oss;
    oss << "Agent Status:" << std::endl;
    oss << "  Ready: " << (isReady() ? "Yes" : "No") << std::endl;
    oss << "  Step: " << (agent_->getNeuromodulationLevel() > 0.0f ? "Active" : "Inactive") << std::endl;
    oss << "  Learning: " << (agent_->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
    
    return oss.str();
}

bool AgentContext::isReady() const {
    return agent_ && brain_ && brain_->isInitialized();
}

std::string AgentContext::getStateSummary() const {
    if (!agent_) {
        return "Agent not initialized";
    }
    
    return agent_->getStateSummary();
}

void AgentContext::resetForEpisode(const SimpleWorld& world) {
    if (agent_) {
        agent_->resetForEpisode(world);
        metricsDirty_ = true;
    }
}

std::map<std::string, double> AgentContext::getMetrics() const {
    if (!metricsDirty_ && !cachedMetrics_.empty()) {
        return cachedMetrics_;
    }
    
    cachedMetrics_.clear();
    
    if (agent_ && brain_) {
        cachedMetrics_["neuromodulation_level"] = agent_->getNeuromodulationLevel();
        cachedMetrics_["curiosity_level"] = agent_->getCuriosityLevel();
        cachedMetrics_["novelty_level"] = agent_->getNoveltyLevel();
        cachedMetrics_["prediction_error"] = agent_->getPredictionError();
        
        cachedMetrics_["spike_count"] = static_cast<double>(brain_->getTotalSpikeCount());
        cachedMetrics_["neuron_count"] = static_cast<double>(brain_->getTotalNeuronCount());
        cachedMetrics_["firing_rate"] = static_cast<double>(brain_->getAverageFiringRate());
    }
    
    metricsDirty_ = false;
    return cachedMetrics_;
}

// MemoryManager implementation
MemoryManager::MemoryManager(Brain* brain) : brain_(brain) {
}

bool MemoryManager::storeExperience(const SensoryInput& input, float reward, const MotorCommand& action) {
    if (!brain_) {
        return false;
    }
    
    try {
        // Store input in working memory
        brain_->getWorkingMemory()->store(input.getPattern(), reward);
        
        // Store as episodic memory
        EpisodicMemoryItem item;
        item.timestamp = SimulationStep(0); // Would get from clock
        item.neurons = std::vector<NeuronId>(); // Would extract from input
        item.values = std::vector<float>(); // Would extract from input
        item.metadata = "Training experience";
        
        brain_->getEpisodicMemory()->storeEpisode(item);
        
        return true;
    } catch (const std::exception& e) {
        nlm::GlobalErrorHandler().logError(
            nlm::ErrorCode::MemorySystemNotAvailable,
            std::string("Failed to store experience: ") + e.what()
        );
        return false;
    }
}

std::vector<EpisodicMemoryItem> MemoryManager::retrieveSimilarExperiences(
    const SensoryInput& input, size_t count) const {
    
    if (!brain_) {
        return std::vector<EpisodicMemoryItem>();
    }
    
    return brain_->getEpisodicMemory()->getRecentEpisodes(count);
}

std::map<std::string, double> MemoryManager::getMemoryStats() const {
    std::map<std::string, double> stats;
    
    if (!brain_) {
        stats["success"] = 0.0;
        return stats;
    }
    
    if (brain_->getWorkingMemory()) {
        stats["working_memory_active_traces"] = static_cast<double>(brain_->getWorkingMemory()->getActiveTraces());
        stats["working_memory_capacity"] = static_cast<double>(brain_->getWorkingMemory()->getCapacity());
        stats["working_memory_utilization"] = static_cast<double>(brain_->getWorkingMemory()->getCurrentSize()) / 
                                             brain_->getWorkingMemory()->getCapacity();
    }
    
    if (brain_->getEpisodicMemory()) {
        stats["episodic_memory_episodes"] = static_cast<double>(brain_->getEpisodicMemory()->getEpisodeCount());
    }
    
    return stats;
}

void MemoryManager::cleanupOldMemories(SimulationStep maxAge) {
    if (brain_ && brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->consolidate(0.0f); // Would use appropriate threshold
    }
}

bool MemoryManager::isMemoryHealthy() const {
    if (!brain_) {
        return false;
    }
    
    auto stats = getMemoryStats();
    
    if (stats.find("working_memory_capacity") != stats.end() && 
        stats["working_memory_capacity"] > 0) {
        return stats["working_memory_utilization"] < 1.0;
    }
    
    return false;
}

std::string MemoryManager::getUsageStatistics() const {
    std::ostringstream oss;
    auto stats = getMemoryStats();
    
    oss << "Memory Usage Statistics:" << std::endl;
    oss << "  Working Memory: " << (stats["working_memory_active_traces"], "/" 
        << stats["working_memory_capacity"]) << std::endl;
    oss << "  Episodic Memory: " << stats["episodic_memory_episodes"] << " episodes" << std::endl;
    oss << "  Healthy: " << (isMemoryHealthy() ? "Yes" : "No") << std::endl;
    
    return oss.str();
}

// BrainTrainer implementation
std::unique_ptr<BrainTrainer> BrainTrainer::create(const Config& config) {
    auto trainer = std::make_unique<BrainTrainer>();
    trainer->config_ = config;
    return trainer;
}

bool BrainTrainer::train(Brain* brain, const SensoryInput& input, float reward, double timestep) {
    if (!brain) {
        return false;
    }
    
    try {
        // Process sensory input
        brain->receiveSensoryInput(input);
        
        // Apply reward modulation
        Neuromodulator neuromodulator;
        neuromodulator.type = NeuromodulatorType::Dopamine;
        neuromodulator.strength = reward;
        neuromodulator.source = NeuromodulatorSource::Reward;
        
        brain->applyNeuromodulation(neuromodulator);
        
        // Update step
        brain->step(SimulationStep(static_cast<int>(timestep)), Timestamp(timestep));
        
        trainingSteps_++;
        
        return true;
    } catch (const std::exception& e) {
        nlm::GlobalErrorHandler().logError(
            nlm::ErrorCode::BrainInitializationFailed,
            std::string("Brain training failed: ") + e.what()
        );
        return false;
    }
}

std::map<std::string, double> BrainTrainer::getTrainingProgress(Brain* brain) const {
    std::map<std::string, double> progress;
    
    progress["training_steps"] = static_cast<double>(trainingSteps_);
    
    if (brain) {
        progress["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        progress["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        
        if (trainingSteps_ > 0) {
            progress["spikes_per_step"] = progress["total_spikes"] / trainingSteps_;
        }
    }
    
    return progress;
}

bool BrainTrainer::isTrainingComplete(Brain* brain) const {
    if (!brain) {
        return true;
    }
    
    auto progress = getTrainingProgress(brain);
    
    if (config_.has("training.max_steps")) {
        size_t maxSteps = config_.getOr<size_t>("training.max_steps", 0);
        if (progress["training_steps"] >= maxSteps) {
            return true;
        }
    }
    
    if (config_.has("training.max_reward")) {
        double maxReward = config_.getOr<double>("training.max_reward", 0.0);
        if (progress["total_spikes"] >= maxReward) {
            return true;
        }
    }
    
    return false;
}

void BrainTrainer::resetProgress(Brain* brain) {
    trainingSteps_ = 0;
}

Config BrainTrainer::getConfig() const {
    return config_;
}

void BrainTrainer::applyConfig(const Config& config) {
    config_ = config;
}

// EnvironmentHelper implementation
SensoryInput EnvironmentHelper::createObservation(const SensoryPercept& percept, const SimpleWorld& world) {
    SensoryInput input;
    
    // Convert percept to input pattern
    input.pattern = percept.getVisionData(); // Would extract from percept
    input.timestamp = SimulationStep(0);
    input.worldId = world.getId();
    input.agentId = world.getAgentId();
    
    return input;
}

} // namespace nlm
