#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <functional>
#include <string>

// Include expert brain headers if available
#ifdef HAVE_EXPERTBRAIN_HPP
#include "ExpertBrain.hpp"
#include "ExpertBrainController.hpp"
#endif

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
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
    , expertModeEnabled_(false)
#ifdef HAVE_EXPERTBRAIN_HPP
    , expertBrain_(nullptr)
    , expertController_(nullptr)
#endif
{
    void AgentBrain::initializeExpertBrain() {
#ifdef HAVE_EXPERTBRAIN_HPP
    if (expertModeEnabled_ && brain_) {
        try {
            // Create expert brain configuration
            ExpertBrain::ExpertConfig expertConfig;
            expertConfig.enable_direct_neuron_access = true;
            expertConfig.enable_advanced_synaptic_config = true;
            expertConfig.enable_region_optimization = true;
            expertConfig.enable_population_control = true;
            expertConfig.enable_performance_monitoring = true;
            expertConfig.enable_safety_checks = true;
            
            // Create expert brain instance
            expertBrain_ = std::make_unique<ExpertBrain>(brain_, expertConfig);
            
            // Create expert controller
            ExpertBrainController::ControllerConfig controllerConfig;
            controllerConfig.enable_expert_mode = true;
            controllerConfig.enable_profiling = true;
            controllerConfig.enable_safety_monitoring = true;
            
            expertController_ = std::make_unique<ExpertBrainController>(brain_, controllerConfig);
            
            NLM_LOG_INFO("Expert brain successfully initialized");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize expert brain: " + std::string(e.what()));
            expertModeEnabled_ = false;
        }
    }
#endif
}
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
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

#ifdef HAVE_EXPERTBRAIN_HPP

// Expert brain integration methods
bool AgentBrain::enableExpertMode(bool enable) {
    if (enable && !expertModeEnabled_) {
        expertModeEnabled_ = true;
        initializeExpertBrain();
        NLM_LOG_INFO("Expert mode enabled");
    } else if (!enable && expertModeEnabled_) {
        expertModeEnabled_ = false;
        // Cleanup expert brain resources
        expertBrain_.reset();
        expertController_.reset();
        NLM_LOG_INFO("Expert mode disabled");
    }
    return expertModeEnabled_;
}

bool AgentBrain::isExpertModeEnabled() const {
    return expertModeEnabled_;
}

ExpertBrainController& AgentBrain::getExpertController() {
    if (!expertController_) {
        NLM_LOG_ERROR("Expert controller not initialized. Enable expert mode first.");
        static ExpertBrainController dummy(nullptr);
        return dummy;
    }
    return *expertController_;
}

const ExpertBrainController& AgentBrain::getExpertController() const {
    if (!expertController_) {
        NLM_LOG_ERROR("Expert controller not initialized. Enable expert mode first.");
        static ExpertBrainController dummy(nullptr);
        return dummy;
    }
    return *expertController_;
}

bool AgentBrain::executeExpertOperation(const std::string& operationName,
                                       const std::function<bool()>& operation,
                                       const std::string& category) {
    if (!expertModeEnabled_ || !expertController_) {
        NLM_LOG_ERROR("Expert mode not enabled or expert controller not available");
        return false;
    }
    
    return expertController_->executeOperation(operationName, operation, category);
}

bool AgentBrain::createExpertCheckpoint(const std::string& name,
                                       const std::unordered_map<std::string, std::string>& metadata) {
    if (!expertModeEnabled_ || !expertController_) {
        NLM_LOG_ERROR("Expert mode not enabled or expert controller not available");
        return false;
    }
    
    return expertController_->createCheckpoint(name, metadata);
}

bool AgentBrain::restoreExpertCheckpoint(const std::string& name) {
    if (!expertModeEnabled_ || !expertController_) {
        NLM_LOG_ERROR("Expert mode not enabled or expert controller not available");
        return false;
    }
    
    return expertController_->restoreCheckpoint(name);
}

std::string AgentBrain::getExpertBrainStatus() const {
    if (!expertModeEnabled_ || !expertBrain_) {
        return "Expert brain not available";
    }
    
    return expertBrain_->generateBrainReport();
}

bool AgentBrain::applyExpertOptimization(const std::string& optimizationName,
                                        const std::unordered_map<std::string, float>& params) {
    if (!expertModeEnabled_ || !expertBrain_) {
        NLM_LOG_ERROR("Expert mode not enabled or expert brain not available");
        return false;
    }
    
    return expertBrain_->configureOptimization(optimizationName, params);
}

bool AgentBrain::exportExpertData(const std::string& filepath, ExportFormat format) {
    if (!expertModeEnabled_ || !expertBrain_) {
        NLM_LOG_ERROR("Expert mode not enabled or expert brain not available");
        return false;
    }
    
    return expertBrain_->exportBrainState(filepath, format);
}

bool AgentBrain::importExpertData(const std::string& filepath, ImportFormat format) {
    if (!expertModeEnabled_ || !expertBrain_) {
        NLM_LOG_ERROR("Expert mode not enabled or expert brain not available");
        return false;
    }
    
    return expertBrain_->importBrainState(filepath, format);
}

#endif // HAVE_EXPERTBRAIN_HPP

} // namespace nlm
