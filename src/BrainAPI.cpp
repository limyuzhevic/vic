// BrainAPI.cpp - Implementation of simplified user interface for NLM

/**
 * Brain API implementation - Easy-to-use interface for NLM users
 * 
 * This API provides a simplified interface to the complex NLM brain system.
 * It handles configuration, initialization, and basic operations while
 * maintaining access to advanced features for power users.
 */

#include "BrainAPI.hpp"
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <any>

namespace nlm {

// Forward declarations for helper functions
namespace {
    // Helper function to parse configuration string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return str;
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    // Helper function to convert string to lowercase
    std::string toLowercase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Helper function to split string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(trim(token));
            }
        }
        return tokens;
    }
}

// Configuration profile definitions
namespace {
    const std::unordered_map<std::string, std::vector<std::pair<std::string, std::any>>> DEFAULT_PROFILES = {
        {"research", {
            {"brain.neuron_count", 500},
            {"brain.synapse_density", 0.1},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"neuromod.dopamine.scale", 1.0},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"simulation.timestep", 0.001},
            {"checkpoint.enable", true},
            {"development.enable", true}
        }},
        {"performance", {
            {"brain.neuron_count", 2000},
            {"brain.synapse_density", 0.05},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", false},
            {"neuromod.dopamine.scale", 1.5},
            {"neuromod.curiosity.enable", false},
            {"neuromod.novelty.enable", false},
            {"simulation.timestep", 0.0005},
            {"checkpoint.enable", false},
            {"development.enable", false}
        }},
        {"learning", {
            {"brain.neuron_count", 1000},
            {"brain.synapse_density", 0.15},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"neuromod.dopamine.scale", 2.0},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"simulation.timestep", 0.001},
            {"checkpoint.enable", true},
            {"development.enable", true},
            {"memory.episodic.max_episodes", 2000}
        }},
        {"minimal", {
            {"brain.neuron_count", 100},
            {"brain.synapse_density", 0.05},
            {"plasticity.stdp.enable", false},
            {"plasticity.hebbian.enable", false},
            {"plasticity.structural.enable", false},
            {"neuromod.dopamine.scale", 0.5},
            {"neuromod.curiosity.enable", false},
            {"neuromod.novelty.enable", false},
            {"simulation.timestep", 0.001},
            {"checkpoint.enable", false},
            {"development.enable", false},
            {"memory.enable", false}
        }}
    };
}

// BrainAPI implementation
BrainAPI::BrainAPI() : pImpl_(std::make_unique<Impl>()), initialized_(false) {
    // Initialize callback system
    stepCallback_ = nullptr;
    spikeCallback_ = nullptr;
}

BrainAPI::~BrainAPI() = default;

std::unique_ptr<BrainAPI> BrainAPI::createDefault() {
    auto api = std::make_unique<BrainAPI>();
    api->getConfig().set("brain.neuron_count", 1000);
    api->getConfig().set("brain.synapse_density", 0.1);
    api->getConfig().set("simulation.timestep", 0.001);
    return api;
}

std::unique_ptr<BrainAPI> BrainAPI::createFromFile(const std::string& configFile) {
    auto api = std::make_unique<BrainAPI>();
    
    // Load configuration from file
    if (api->getConfig().loadFromFile(configFile)) {
        NLM_LOG_INFO("Configuration loaded from: " + configFile);
    } else {
        NLM_LOG_WARN("Failed to load configuration from: " + configFile);
        NLM_LOG_WARN("Using default configuration");
    }
    
    return api;
}

std::unique_ptr<BrainAPI> BrainAPI::createCustom(const Config& config) {
    auto api = std::make_unique<BrainAPI>();
    
    // Apply custom configuration
    for (const auto& key : config.getKeys()) {
        auto value = config.getAny(key);
        if (value) {
            api->getConfig().setAny(key, *value);
        }
    }
    
    return api;
}

bool BrainAPI::initialize() {
    try {
        // Validate configuration before initialization
        if (!validateConfiguration()) {
            NLM_LOG_ERROR("Configuration validation failed");
            return false;
        }
        
        // Log initialization start
        NLM_LOG_INFO("Initializing NLM Brain API...");
        
        // Create underlying brain implementation
        pImpl_->brain_ = std::make_unique<Brain>(pImpl_->config_);
        
        // Initialize the brain
        if (!pImpl_->brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize underlying brain");
            return false;
        }
        
        // Set up event handlers
        setupEventHandlers();
        
        // Update initialization status
        initialized_ = true;
        
        // Log successful initialization
        NLM_LOG_INFO("NLM Brain API initialized successfully");
        logStatus();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        return false;
    }
}

void BrainAPI::step(int steps) {
    if (!initialized_) {
        NLM_LOG_ERROR("Brain not initialized. Call initialize() first.");
        return;
    }
    
    for (int i = 0; i < steps; ++i) {
        // Call step callback if registered
        if (stepCallback_) {
            stepCallback_(i);
        }
        
        // Step the brain
        pImpl_->brain_->step(i);
    }
}

void BrainAPI::step(double timeDelta) {
    if (!initialized_) {
        NLM_LOG_ERROR("Brain not initialized. Call initialize() first.");
        return;
    }
    
    static int stepCounter = 0;
    
    // Call step callback if registered
    if (stepCallback_) {
        stepCallback_(stepCounter);
    }
    
    // Step the brain with time delta
    pImpl_->brain_->step(stepCounter, timeDelta);
    stepCounter++;
}

BrainAPI::Stats BrainAPI::getStats() const {
    Stats stats;
    
    if (!initialized_) {
        NLM_LOG_WARN("Brain not initialized. Returning default stats.");
        return stats;
    }
    
    // Get brain statistics
    stats.totalNeurons = pImpl_->brain_->getTotalNeuronCount();
    stats.totalSynapses = pImpl_->brain_->getTotalSynapseCount();
    stats.avgFiringRate = pImpl_->brain_->getAverageFiringRate();
    stats.eI_Ratio = pImpl_->brain_->getExcitationInhibitionRatio();
    stats.totalSpikes = pImpl_->brain_->getTotalSpikeCount();
    
    // Get memory system stats
    auto* workingMemory = pImpl_->brain_->getWorkingMemory();
    if (workingMemory) {
        stats.workingMemoryTraced = workingMemory->getActiveTraces();
    }
    
    auto* episodicMemory = pImpl_->brain_->getEpisodicMemory();
    if (episodicMemory) {
        stats.memoryEpisodes = episodicMemory->getEpisodeCount();
    }
    
    // Get neuromodulation stats
    auto* dopamine = pImpl_->brain_->getDopamine();
    if (dopamine) {
        stats.dopamineLevel = dopamine->getLevel();
    }
    
    auto* curiosity = pImpl_->brain_->getCuriosity();
    if (curiosity) {
        stats.curiosityLevel = curiosity->getLevel();
    }
    
    auto* novelty = pImpl_->brain_->getNovelty();
    if (novelty) {
        stats.noveltyLevel = novelty->getLevel();
    }
    
    return stats;
}

void BrainAPI::reset() {
    if (!initialized_) {
        NLM_LOG_WARN("Brain not initialized. Nothing to reset.");
        return;
    }
    
    NLM_LOG_INFO("Resetting NLM Brain API...");
    pImpl_->brain_->reset();
    NLM_LOG_INFO("NLM Brain API reset complete");
}

bool BrainAPI::save(const std::string& path) {
    if (!initialized_) {
        NLM_LOG_ERROR("Brain not initialized. Cannot save state.");
        return false;
    }
    
    NLM_LOG_INFO("Saving brain state to: " + path);
    return pImpl_->brain_->save(path);
}

bool BrainAPI::load(const std::string& path) {
    NLM_LOG_INFO("Loading brain state from: " + path);
    
    // Create a new brain implementation
    pImpl_->brain_ = std::make_unique<Brain>(pImpl_->config_);
    
    // Load the brain state
    if (!pImpl_->brain_->load(path)) {
        NLM_LOG_ERROR("Failed to load brain state");
        return false;
    }
    
    // Update initialization status
    initialized_ = true;
    
    // Log successful load
    NLM_LOG_INFO("Brain state loaded successfully");
    logStatus();
    
    return true;
}

void BrainAPI::enableFeature(const std::string& feature, bool enable) {
    std::string normalizedFeature = toLowercase(feature);
    
    if (normalizedFeature == "reward_modulation" || 
        normalizedFeature == "dopamine" ||
        normalizedFeature == "reward_learning") {
        // Enable/disable dopamine system
        pImpl_->brain_->enableRewardModulation(enable);
    } else if (normalizedFeature == "structural_plasticity" || 
               normalizedFeature == "synaptogenesis" ||
               normalizedFeature == "pruning") {
        // Enable/disable structural plasticity
        pImpl_->brain_->enableStructuralPlasticity(enable);
    } else if (normalizedFeature == "development" || 
               normalizedFeature == "maturation" ||
               normalizedFeature == "plasticity_modulation") {
        // Enable/disable development system
        pImpl_->brain_->enableDevelopment(enable);
    } else if (normalizedFeature == "curiosity" || 
               normalizedFeature == "exploration" ||
               normalizedFeature == "novelty_driven") {
        // Enable/disable curiosity system
        pImpl_->brain_->enableCuriosity(enable);
    } else if (normalizedFeature == "novelty" || 
               normalizedFeature == "surprise" ||
               normalizedFeature == "unexpectedness") {
        // Enable/disable novelty detection
        pImpl_->brain_->enableNovelty(enable);
    } else {
        NLM_LOG_WARN("Unknown feature: " + feature);
    }
}

bool BrainAPI::isFeatureEnabled(const std::string& feature) const {
    std::string normalizedFeature = toLowercase(feature);
    
    if (normalizedFeature == "reward_modulation" || 
        normalizedFeature == "dopamine" ||
        normalizedFeature == "reward_learning") {
        // Check dopamine system
        auto* dopamine = pImpl_->brain_->getDopamine();
        return dopamine && dopamine->getLevel() > 0.0f;
    } else if (normalizedFeature == "structural_plasticity" || 
               normalizedFeature == "synaptogenesis" ||
               normalizedFeature == "pruning") {
        // Check structural plasticity system
        return pImpl_->brain_->isStructuralPlasticityEnabled();
    } else if (normalizedFeature == "development" || 
               normalizedFeature == "maturation" ||
               normalizedFeature == "plasticity_modulation") {
        // Check development system
        return pImpl_->brain_->isDevelopmentEnabled();
    } else if (normalizedFeature == "curiosity" || 
               normalizedFeature == "exploration" ||
               normalizedFeature == "novelty_driven") {
        // Check curiosity system
        auto* curiosity = pImpl_->brain_->getCuriosity();
        return curiosity && curiosity->getLevel() > 0.0f;
    } else if (normalizedFeature == "novelty" || 
               normalizedFeature == "surprise" ||
               normalizedFeature == "unexpectedness") {
        // Check novelty system
        auto* novelty = pImpl_->brain_->getNovelty();
        return novelty && novelty->getLevel() > 0.0f;
    } else {
        NLM_LOG_WARN("Unknown feature: " + feature);
        return false;
    }
}

void BrainAPI::setDevelopmentStage(DevelopmentalStage stage) {
    if (initialized_) {
        pImpl_->brain_->setDevelopmentalStage(stage);
    }
}

DevelopmentalStage BrainAPI::getDevelopmentStage() const {
    if (initialized_) {
        return pImpl_->brain_->getDevelopmentalStage();
    }
    return DevelopmentalStage::Initial;
}

void BrainAPI::setStepCallback(std::function<void(int)> callback) {
    stepCallback_ = std::move(callback);
}

void BrainAPI::setSpikeCallback(std::function<void(const DetailedSpikeEvent&)> callback) {
    spikeCallback_ = std::move(callback);
    
    // Register the callback with the spike system if available
    if (initialized_) {
        auto* spikeSystem = pImpl_->brain_->getSpikeSystem();
        if (spikeSystem) {
            spikeSystem->registerHandler(spikeCallback_);
        }
    }
}

Config& BrainAPI::getConfig() {
    return pImpl_->config_;
}

const Config& BrainAPI::getConfig() const {
    return pImpl_->config_;
}

bool BrainAPI::loadProfile(const std::string& profileName) {
    std::string normalizedProfile = toLowercase(profileName);
    
    auto it = DEFAULT_PROFILES.find(normalizedProfile);
    if (it == DEFAULT_PROFILES.end()) {
        NLM_LOG_ERROR("Unknown configuration profile: " + profileName);
        NLM_LOG_ERROR("Available profiles: research, performance, learning, minimal");
        return false;
    }
    
    // Apply profile configuration
    for (const auto& setting : it->second) {
        const auto& key = setting.first;
        const auto& value = setting.second;
        
        // Try to set the value with appropriate type
        if (std::holds_alternative<int>(value)) {
            getConfig().set<int>(key, std::get<int>(value));
        } else if (std::holds_alternative<float>(value)) {
            getConfig().set<float>(key, std::get<float>(value));
        } else if (std::holds_alternative<double>(value)) {
            getConfig().set<double>(key, std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            getConfig().set<bool>(key, std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            getConfig().set<std::string>(key, std::get<std::string>(value));
        }
    }
    
    NLM_LOG_INFO("Loaded configuration profile: " + profileName);
    return true;
}

std::vector<std::string> BrainAPI::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    for (const auto& pair : DEFAULT_PROFILES) {
        profiles.push_back(pair.first);
    }
    return profiles;
}

bool BrainAPI::validateConfiguration() const {
    bool valid = true;
    
    // Check required parameters
    if (!pImpl_->config_.has("brain.neuron_count")) {
        NLM_LOG_ERROR("Missing required parameter: brain.neuron_count");
        valid = false;
    }
    
    if (!pImpl_->config_.has("brain.synapse_density")) {
        NLM_LOG_ERROR("Missing required parameter: brain.synapse_density");
        valid = false;
    }
    
    // Validate parameter values
    if (pImpl_->config_.has("brain.neuron_count")) {
        int64_t neuronCount = pImpl_->config_.getOr<int64_t>("brain.neuron_count", 0);
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("Invalid parameter value: brain.neuron_count must be positive");
            valid = false;
        }
    }
    
    if (pImpl_->config_.has("brain.synapse_density")) {
        float density = pImpl_->config_.getOr<float>("brain.synapse_density", 0.0f);
        if (density < 0.0f || density > 1.0f) {
            NLM_LOG_ERROR("Invalid parameter value: brain.synapse_density must be between 0 and 1");
            valid = false;
        }
    }
    
    return valid;
}

void BrainAPI::setupEventHandlers() {
    // Set up spike event handler
    auto* spikeSystem = pImpl_->brain_->getSpikeSystem();
    if (spikeSystem && spikeCallback_) {
        spikeSystem->registerHandler(spikeCallback_);
    }
}

void BrainAPI::logStatus() const {
    if (!initialized_) {
        NLM_LOG_INFO("=== NLM Brain API Status (Not Initialized) ===");
        return;
    }
    
    NLM_LOG_INFO("=== NLM Brain API Status ===");
    
    // Get basic statistics
    auto stats = getStats();
    NLM_LOG_INFO("Total neurons: " + std::to_string(stats.totalNeurons));
    NLM_LOG_INFO("Total synapses: " + std::to_string(stats.totalSynapses));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(stats.avgFiringRate));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(stats.eI_Ratio));
    NLM_LOG_INFO("Total spikes: " + std::to_string(stats.totalSpikes));
    
    // Log memory system status
    if (stats.workingMemoryTraced > 0) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(stats.workingMemoryTraced));
    }
    if (stats.memoryEpisodes > 0) {
        NLM_LOG_INFO("Memory episodes: " + std::to_string(stats.memoryEpisodes));
    }
    
    // Log neuromodulation status
    if (stats.dopamineLevel > 0.0f) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(stats.dopamineLevel));
    }
    if (stats.curiosityLevel > 0.0f) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(stats.curiosityLevel));
    }
    if (stats.noveltyLevel > 0.0f) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(stats.noveltyLevel));
    }
    
    // Log configuration status
    NLM_LOG_INFO("Configuration file: " + pImpl_->config_.getSourcePath());
    
    // Log development status
    DevelopmentalStage stage = getDevelopmentStage();
    NLM_LOG_INFO("Development stage: " + std::to_string(static_cast<int>(stage)));
}

// Implementation details for BrainAPI::Impl
struct BrainAPI::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    Impl() : config(std::make_shared<Config>()) {}
};

// Initialize global brain API
void initializeGlobalBrainAPI() {
    if (!globalBrainAPI) {
        globalBrainAPI = BrainAPI::createDefault();
        globalBrainAPI->initialize();
    }
}

// Global brain API instance for backward compatibility
std::unique_ptr<BrainAPI> globalBrainAPI;

// Backward compatibility functions
namespace {
    // Wrapper functions for backward compatibility
    void backwardCompatibleBrainStep(int steps) {
        if (globalBrainAPI) {
            globalBrainAPI->step(steps);
        }
    }
    
    void backwardCompatibleBrainStepWithTime(double time) {
        if (globalBrainAPI) {
            globalBrainAPI->step(time);
        }
    }
    
    void backwardCompatibleBrainReset() {
        if (globalBrainAPI) {
            globalBrainAPI->reset();
        }
    }
    
    bool backwardCompatibleBrainSave(const std::string& path) {
        if (globalBrainAPI) {
            return globalBrainAPI->save(path);
        }
        return false;
    }
    
    bool backwardCompatibleBrainLoad(const std::string& path) {
        if (globalBrainAPI) {
            return globalBrainAPI->load(path);
        }
        return false;
    }
    
    size_t backwardCompatibleGetTotalNeurons() {
        if (globalBrainAPI) {
            auto stats = globalBrainAPI->getStats();
            return stats.totalNeurons;
        }
        return 0;
    }
    
    size_t backwardCompatibleGetTotalSynapses() {
        if (globalBrainAPI) {
            auto stats = globalBrainAPI->getStats();
            return stats.totalSynapses;
        }
        return 0;
    }
    
    float backwardCompatibleGetAverageFiringRate() {
        if (globalBrainAPI) {
            auto stats = globalBrainAPI->getStats();
            return stats.avgFiringRate;
        }
        return 0.0f;
    }
    
    float backwardCompatibleGetEI_Ratio() {
        if (globalBrainAPI) {
            auto stats = globalBrainAPI->getStats();
            return stats.eI_Ratio;
        }
        return 0.0f;
    }
}

// Backward compatibility API functions
void brainStep(int steps) {
    backwardCompatibleBrainStep(steps);
}

void brainStepWithTime(double time) {
    backwardCompatibleBrainStepWithTime(time);
}

void brainReset() {
    backwardCompatibleBrainReset();
}

bool brainSave(const std::string& path) {
    return backwardCompatibleBrainSave(path);
}

bool brainLoad(const std::string& path) {
    return backwardCompatibleBrainLoad(path);
}

size_t getTotalNeurons() {
    return backwardCompatibleGetTotalNeurons();
}

size_t getTotalSynapses() {
    return backwardCompatibleGetTotalSynapses();
}

float getAverageFiringRate() {
    return backwardCompatibleGetAverageFiringRate();
}

float getExcitationInhibitionRatio() {
    return backwardCompatibleGetEI_Ratio();
}

} // namespace nlm
