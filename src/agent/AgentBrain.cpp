#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config)
    : brain_(brain)
    , config_(config)
    , motorDecoder_(config)
    , sensoryProcessor_(config)
    , neuromodulationManager_(config)
    , developmentManager_(config)
    , curiosityManager_(config)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
{
    if (config_.enableDebugLogging) {
        NLM_LOG_INFO("AgentBrain initialized with configurable agent architecture");
    }
    
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize components with world configuration
    sensoryProcessor_.initialize(world.getVisionWidth(), world.getVisionHeight());
    
    // Reset all state
    reset();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision + touch + internal + proprioception
    return config_.visionSensoryScale + config_.touchSensoryScale + 
           config_.internalSensoryScale + config_.proprioceptionSensoryScale;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action group
    return motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
           motorTurnRight_.size() + motorInteract_.size() + motorWait_.size();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Update curiosity based on novelty detection
    if (curiosityEnabled_) {
        const auto& vision = percept.getVision();
        float novelty = sensoryProcessor_.computeNovelty(vision);
        noveltyLevel_ = novelty;
        curiosityLevel_ = curiosityManager_.updateCuriosity(novelty, predictionError_);
    }
    
    // Process sensory input through sensory processor
    sensoryProcessor_.processSensoryInput(
        percept,
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_
    );
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Decode motor neurons to get primary action
    MotorCommand decoded = motorDecoder_.decodeFromMotorNeurons(
        motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_,
        motorInteract_, motorWait_
    );
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.0f) {
        decoded = motorDecoder_.selectWithCuriosity(decoded, curiosityLevel_, 
                                                  brain_->getRandomGenerator());
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Update neuromodulation using neuromodulation manager
    neuromodulationManager_.update(reward, predictedReward, 
                                  dopamineLevel_, predictionError_, expectedReward_);
    
    // Apply plasticity changes (placeholder - would need synapse access)
    neuromodulationManager_.applyPlasticityChanges(
        {}, // Would need neuron/synapse access
        dopamineLevel_, plasticityModifier_, 
        brain_->getSTDP()
    );
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    // Update development using development manager
    DevelopmentalStage currentStage = getDevelopmentalStage();
    developmentManager_.updateDevelopment(timestep, developmentalAge_, 
                                        plasticityModifier_, currentStage);
    
    // Update structural plasticity
    if (structuralPlasticityEnabled_) {
        float synaptogenesisRate, pruningRate;
        developmentManager_.updateStructuralPlasticity(
            plasticityModifier_, synaptogenesisRate, pruningRate);
        
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            sp->setSynaptogenesisRate(synaptogenesisRate);
            sp->setPruningRate(pruningRate);
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
    
    // Reset sensory processor
    sensoryProcessor_.reset();
}

void AgentBrain::initializeNeuronGroups() {
    organizeNeuronsByType(brain_.get());
}

void AgentBrain::organizeNeuronsByType(const Brain* brain) {
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                organizeMotorNeurons(brain);
            } else if (type == NeuronType::Sensory) {
                organizeSensoryNeurons(brain);
            }
        }
    }
}

void AgentBrain::organizeMotorNeurons(const Brain* brain) {
    for (Neuron* n : brain->getRegion(0)->getPopulations()[0]->getNeurons()) {
        if (n) {
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
    }
}

void AgentBrain::organizeSensoryNeurons(const Brain* brain) {
    for (Neuron* n : brain->getRegion(1)->getPopulations()[0]->getNeurons()) {
        if (n) {
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

} // namespace nlm
