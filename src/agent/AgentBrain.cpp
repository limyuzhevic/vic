#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../visualization/IntegrationVisualizer.hpp"
#include "../visualization/PerformanceVisualizer.hpp"
#include "../visualization/DebugVisualizer.hpp"
#include "../visualization/ConfigVisualizer.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

// Integration control helpers
bool isIntegrationSystemEnabled(const std::string& name, std::shared_ptr<Config> config = nullptr) {
    static std::unordered_set<std::string> enabledSystems;
    
    if (config) {
        // Check if specific system is enabled in config
        std::string key = "integration_system_" + name;
        if (config->has(key)) {
            std::string value = config->getOr<std::string>(key, "false");
            return value == "true";
        }
    }
    
    return enabledSystems.find(name) != enabledSystems.end();
}

void enableIntegrationSystem(const std::string& name, std::shared_ptr<Config> config = nullptr) {
    static std::unordered_set<std::string> enabledSystems;
    
    enabledSystems.insert(name);
    
    if (config) {
        std::string key = "integration_system_" + name;
        config->set(key, "true", ConfigSource::Runtime);
    }
    
    NLM_LOG_INFO("Integration system enabled: " + name);
}

void disableIntegrationSystem(const std::string& name, std::shared_ptr<Config> config = nullptr) {
    static std::unordered_set<std::string> enabledSystems;
    
    enabledSystems.erase(name);
    
    if (config) {
        std::string key = "integration_system_" + name;
        config->set(key, "false", ConfigSource::Runtime);
    }
    
    NLM_LOG_INFO("Integration system disabled: " + name);
}

void AgentBrain::updateVisualizationWithCLIConfig() {
    if (!brain_ || !config_) return;
    
    // Integration visualization updates
    auto integrationConfig = std::make_shared<Config>("integration");
    if (config_->has("monitor_integration")) {
        integrationConfig->set("monitor_integration", 
                             config_->getOr<bool>("monitor_integration", false));
    }
    if (config_->has("integration_features")) {
        integrationConfig->set("integration_features", 
                             config_->getOr<std::string>("integration_features", ""));
    }
    if (config_->has("integration_threshold_prediction_error")) {
        integrationConfig->set("integration_threshold_prediction_error",
                             config_->getOr<float>("integration_threshold_prediction_error", 0.5f));
    }
    
    // Performance visualization updates
    auto performanceConfig = std::make_shared<Config>("performance");
    if (config_->has("metrics_interval")) {
        performanceConfig->set("metrics_interval",
                             config_->getOr<int>("metrics_interval", 100));
    }
    if (config_->has("performance_profiling")) {
        performanceConfig->set("performance_profiling",
                             config_->getOr<bool>("performance_profiling", false));
    }
    
    // Debug visualization updates
    auto debugConfig = std::make_shared<Config>("debug");
    if (config_->has("debug_level")) {
        debugConfig->set("debug_level",
                       config_->getOr<int>("debug_level", 3));
    }
    if (config_->has("debug_build")) {
        debugConfig->set("debug_build",
                       config_->getOr<bool>("debug_build", false));
    }
    
    // Configuration visualization updates
    auto configConfig = std::make_shared<Config>("config");
    if (config_->has("generate_reports")) {
        configConfig->set("generate_reports",
                        config_->getOr<bool>("generate_reports", false));
    }
    if (config_->has("analyze_performance")) {
        configConfig->set("analyze_performance",
                        config_->getOr<bool>("analyze_performance", false));
    }
    
    NLM_LOG_INFO("Visualization configuration applied");
}

// Debug and monitoring helpers
void updateDebugSettings(std::shared_ptr<Config> config) {
    if (!config) return;
    
    int debugLevel = config->getOr<int>("debug_level", 3);
    NLM_LOG_INFO("Set debug level: " + std::to_string(debugLevel));
    
    // Note: Debug levels may need to be implemented in Logger class
    // For now, just log the setting
    
    // Update debug logging for specific systems
    bool debugAll = config->getOr<bool>("debug_log_all", false);
    bool debugBrain = config->getOr<bool>("debug_log_brain", false);
    bool debugIntegration = config->getOr<bool>("debug_log_integration", false);
    bool debugMemory = config->getOr<bool>("debug_log_memory", false);
    bool debugPrediction = config->getOr<bool>("debug_log_prediction", false);
    bool debugNeuromodulation = config->getOr<bool>("debug_log_neuromodulation", false);
    bool debugCuriosity = config->getOr<bool>("debug_log_curiosity", false);
    bool debugNovelty = config->getOr<bool>("debug_log_novelty", false);
    
    if (debugAll) {
        NLM_LOG_INFO("Debug logging enabled for ALL systems");
    }
    if (debugBrain) NLM_LOG_INFO("Debug logging enabled for Brain");
    if (debugIntegration) NLM_LOG_INFO("Debug logging enabled for Integration");
    if (debugMemory) NLM_LOG_INFO("Debug logging enabled for Memory");
    if (debugPrediction) NLM_LOG_INFO("Debug logging enabled for Prediction");
    if (debugNeuromodulation) NLM_LOG_INFO("Debug logging enabled for Neuromodulation");
    if (debugCuriosity) NLM_LOG_INFO("Debug logging enabled for Curiosity");
    if (debugNovelty) NLM_LOG_INFO("Debug logging enabled for Novelty");
}

void updateMonitoringSettings(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;
    
    bool monitorStatus = config->getOr<bool>("monitor_status", false);
    NLM_LOG_INFO("System status monitoring: " + std::string(monitorStatus ? "Enabled" : "Disabled"));
    
    int metricsInterval = config->getOr<int>("metrics_interval", 100);
    NLM_LOG_INFO("Metrics recording interval: " + std::to_string(metricsInterval) + " steps");
    
    // If metrics interval is set, we could set up periodic metrics collection
    // For now, just log the setting
    if (monitorStatus) {
        brain->logStatus();
    }
}

void updateAdvancedFeatures(std::shared_ptr<Config> config) {
    if (!config) return;
    
    bool experimentalFeatures = config->getOr<bool>("experimental_features", false);
    NLM_LOG_INFO("Experimental features: " + std::string(experimentalFeatures ? "Enabled" : "Disabled"));
    
    bool debugBuild = config->getOr<bool>("debug_build", false);
    NLM_LOG_INFO("Debug build: " + std::string(debugBuild ? "Enabled" : "Disabled"));
    
    bool performanceProfiling = config->getOr<bool>("performance_profiling", false);
    NLM_LOG_INFO("Performance profiling: " + std::string(performanceProfiling ? "Enabled" : "Disabled"));
    
    bool developmentTools = config->getOr<bool>("development_tools", false);
    NLM_LOG_INFO("Development tools: " + std::string(developmentTools ? "Enabled" : "Disabled"));
    
    bool enableReplay = config->getOr<bool>("enable_replay", false);
    NLM_LOG_INFO("Replay system: " + std::string(enableReplay ? "Enabled" : "Disabled"));
    
    bool enableSleep = config->getOr<bool>("enable_sleep", false);
    NLM_LOG_INFO("Sleep system: " + std::string(enableSleep ? "Enabled" : "Disabled"));
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Visualization integration
    integrationVisualizer_ = std::make_shared<IntegrationVisualizer>();
    performanceVisualizer_ = std::make_shared<PerformanceVisualizer>();
    debugVisualizer_ = std::make_shared<DebugVisualizer>();
    configVisualizer_ = std::make_shared<ConfigVisualizer>();
    
    // Set references to brain and config
    integrationVisualizer_->setBrain(brain);
    integrationVisualizer_->setConfig(config);
    
    performanceVisualizer_->setBrain(brain);
    performanceVisualizer_->setPerformanceProfiler(brain->getPerformanceProfiler());
    
    debugVisualizer_->setBrain(brain);
    debugVisualizer_->setConfig(config);
    
    configVisualizer_->setConfig(config);
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                     motorTurnLeft_.size() + motorTurnRight_.size() +
                                     motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                     sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Update integration control parameters
    if (config_) {
        updateIntegrationControlParameters(brain_, config_);
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::updateWithCLIConfig() {
    if (!brain_ || !config_) return;
    
    // Apply integration control parameters
    updateIntegrationControlParameters(brain_, config_);
    
    // Update monitoring settings
    updateMonitoringSettings(brain_, config_);
    
    // Update advanced features
    updateAdvancedFeatures(config_);
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
