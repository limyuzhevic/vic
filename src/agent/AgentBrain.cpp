// AgentBrain implementation with full modularity and DI
#include "../agent/ModularityLayer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<IBrainInterface> brainInterface,
                       std::unique_ptr<MotorDecodingStrategy> motorStrategy,
                       std::unique_ptr<CuriosityHandler> curiosityHandler,
                       std::shared_ptr<Config> config)
    : brainInterface_(std::move(brainInterface))
    , motorStrategy_(std::move(motorStrategy))
    , curiosityHandler_(std::move(curiosityHandler))
    , config_(std::move(config))
    , noveltyLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , sensoryInputSize_(256 + 8 + 4 + 6)  // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    , motorOutputSize_(6)  // One motor neuron per action
{
    // Validate inputs
    if (!brainInterface_) {
        throw std::invalid_argument("brainInterface cannot be null");
    }
    if (!motorStrategy_) {
        throw std::invalid_argument("motorStrategy cannot be null");
    }
    if (!curiosityHandler_) {
        throw std::invalid_argument("curiosityHandler cannot be null");
    }
    
    // Initialize neuron groups
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::configureFromConfig(const Config& config) {
    // Configure from configuration
    sensoryInputSize_ = config.getOr<size_t>("sensory_input_size", 274);
    motorOutputSize_ = config.getOr<size_t>("motor_output_size", 6);
    
    rewardModulationEnabled_ = config.getOr<bool>("reward_modulation_enabled", true);
    structuralPlasticityEnabled_ = config.getOr<bool>("structural_plasticity_enabled", true);
    developmentEnabled_ = config.getOr<bool>("development_enabled", true);
    curiosityEnabled_ = config.getOr<bool>("curiosity_enabled", true);
    
    sensoryNoveltyDecay_ = config.getOr<float>("sensory_novelty_decay", 0.99f);
    
    // Update curiosity handler with config parameters if needed
    // ...
}

void AgentBrain::initializeNeuronGroups() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Note: In a real implementation, we would get neurons from the brain interface
    // For now, this is a placeholder
    NLM_LOG_INFO("AgentBrain initialized with modular design");
}

template<typename T>
bool AgentBrain::safeAccess(const std::vector<T*>& container, size_t index, T*& out) {
    out = nullptr;
    if (index < container.size()) {
        out = container[index];
    }
    return out != nullptr;
}

template<typename T>
bool AgentBrain::safeAccess(const std::vector<T*>& container, size_t index, T*& out) {
    out = nullptr;
    if (index < container.size()) {
        out = container[index];
    }
    return out != nullptr;
}

void AgentBrain::initializeNeuronGroups() {
    // Modern implementation
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
}

void AgentBrain::validatePercept(const SensoryPercept& percept) const {
    if (sensoryInputSize_ > 0 && percept.getVision().size() > sensoryInputSize_) {
        NLM_LOG_WARNING("Percept vision size (" + std::to_string(percept.getVision().size()) +
                       ") exceeds configured sensory input size (" + 
                       std::to_string(sensoryInputSize_) + ")");
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!isValid()) {
        NLM_LOG_ERROR("AgentBrain is not in valid state");
        return;
    }
    
    validatePercept(percept);
    
    const auto& vision = percept.getVision();
    
    // Process vision input with null safety
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = nullptr;
        if (safeAccess(sensoryVision_, i, neuron) && neuron) {
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        } else {
            NLM_LOG_WARNING("Sensory neuron at index " + std::to_string(i) + " is null or invalid");
        }
    }
    
    // Similar processing for other sensory modalities...
    // Touch, internal, proprioception would be processed here
    
    // Compute novelty
    if (!vision.empty() && !previousVision_.empty()) {
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
    
    // Update curiosity
    if (curiosityEnabled_ && curiosityHandler_) {
        curiosityHandler_->update(noveltyLevel_, predictionError_, 1.0);  // timestep = 1.0
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!isValid()) {
        return MotorCommand::Wait;
    }
    
    if (!brainInterface_) {
        return MotorCommand::Wait;
    }
    
    // Collect all motor neurons
    std::vector<Neuron*> allMotorNeurons;
    allMotorNeurons.insert(allMotorNeurons.end(), motorForward_.begin(), motorForward_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorBackward_.begin(), motorBackward_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorTurnLeft_.begin(), motorTurnLeft_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorTurnRight_.begin(), motorTurnRight_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorInteract_.begin(), motorInteract_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorWait_.begin(), motorWait_.end());
    
    // Use strategy pattern for decoding
    MotorCommand decoded = MotorCommand::Wait;
    if (motorStrategy_) {
        decoded = motorStrategy_->decode(allMotorNeurons);
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityHandler_) {
        float curiosityLevel = curiosityHandler_->getCuriosityLevel();
        if (curiosityLevel > 0.3f) {
            // For curiosity selection, we would need another strategy
            // For now, use basic random exploration
            float exploreChance = curiosityLevel * 0.3f;
            float r = brainInterface_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                int choice = brainInterface_->getRandomGenerator()->uniformInt(0, 7);
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
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!isValid() || !brainInterface_ || !rewardModulationEnabled_) {
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Update curiosity handler with prediction error
    if (curiosityHandler_) {
        curiosityHandler_->update(noveltyLevel_, predictionError_, 1.0);
    }
    
    // Apply neuromodulation to brain through interface
    // ... (implementation would apply to neurons/synapses)
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!isValid() || !brainInterface_ || !developmentEnabled_) {
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;
        brainInterface_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brainInterface_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brainInterface_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brainInterface_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brainInterface_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::reset() {
    noveltyLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    previousVision_.clear();
    
    // Reset curiosity handler if available
    if (curiosityHandler_) {
        curiosityHandler_->update(0.0f, 0.0f, 0.0);
    }
}

bool AgentBrain::isValid() const {
    return brainInterface_ && brainInterface_->getActiveNeuronCount() > 0;
}
