#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    // Reserve memory for neuron groups to improve performance
    motorForward_.reserve(50);  // Typical motor neurons per region
    motorBackward_.reserve(50);
    motorTurnLeft_.reserve(50);
    motorTurnRight_.reserve(50);
    motorInteract_.reserve(50);
    motorWait_.reserve(50);
    
    sensoryVision_.reserve(50);
    sensoryTouch_.reserve(50);
    sensoryInternal_.reserve(50);
    sensoryProprioception_.reserve(50);
    
    // Process brain regions to distribute neurons
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            if (!region) continue;
            
            for (const auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    distributeMotorNeurons(pop.get());
                } else if (type == NeuronType::Sensory) {
                    distributeSensoryNeurons(pop.get());
                }
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(Population* population) {
    if (!population) return;
    
    const auto& neurons = population->getNeurons();
    size_t currentTotal = motorForward_.size() + motorBackward_.size() + 
                         motorTurnLeft_.size() + motorTurnRight_.size() +
                         motorInteract_.size() + motorWait_.size();
    
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("Null neuron found in motor population");
            continue;
        }
        
        // Distribute motor neurons to different action groups
        switch (currentTotal % 6) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
        currentTotal++;
    }
}

void AgentBrain::distributeSensoryNeurons(Population* population) {
    if (!population) return;
    
    const auto& neurons = population->getNeurons();
    size_t currentTotal = sensoryVision_.size() + sensoryTouch_.size() +
                         sensoryInternal_.size() + sensoryProprioception_.size();
    
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("Null neuron found in sensory population");
            continue;
        }
        
        switch (currentTotal % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
        currentTotal++;
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain with null brain");
        return;
    }
    
    // Resize previous vision buffer based on world dimensions
    previousVision_.clear();
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
    // This matches the default SensoryPercept configuration
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (including wait)
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input with null brain");
        return;
    }
    
    // Validate percept before processing
    if (!percept.validate()) {
        NLM_LOG_WARNING("Invalid sensory percept detected");
        // Continue processing but log warning
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    processVisionInput(percept);
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    processTouchInput(percept);
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    processInternalInput(percept);
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    processProprioceptionInput(percept);
    
    // Compute novelty (difference from previous vision)
    computeNovelty(percept);
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    size_t minSize = std::min(sensoryVision_.size(), vision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (!sensoryVision_[i]) {
            NLM_LOG_WARNING("Null vision neuron at index " + std::to_string(i));
            continue;
        }
        
        // Inject current proportional to vision intensity
        float current = vision[i] * 5.0f;  // Scale factor
        sensoryVision_[i]->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    size_t minSize = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (!sensoryTouch_[i]) {
            NLM_LOG_WARNING("Null touch neuron at index " + std::to_string(i));
            continue;
        }
        
        float current = touch[i] * 8.0f;  // Collision signal
        sensoryTouch_[i]->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    size_t minSize = std::min(sensoryInternal_.size(), intern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (!sensoryInternal_[i]) {
            NLM_LOG_WARNING("Null internal neuron at index " + std::to_string(i));
            continue;
        }
        
        float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
        sensoryInternal_[i]->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (!sensoryProprioception_[i]) {
            NLM_LOG_WARNING("Null proprioception neuron at index " + std::to_string(i));
            continue;
        }
        
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
        sensoryProprioception_[i]->injectCurrent(current);
    }
}

void AgentBrain::computeNovelty(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    if (vision.empty() || previousVision_.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    size_t minSize = std::min(vision.size(), previousVision_.size());
    float totalDiff = 0.0f;
    
    // Compute absolute differences between current and previous vision
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize by number of compared pixels
    noveltyLevel_ = totalDiff / static_cast<float>(std::max(minSize, size_t(1)));
    
    // Decay novelty over time
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Update previous vision state
    previousVision_ = vision;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command with null brain");
        return MotorCommand::Wait;
    }
    
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
        for (const Neuron* n : neurons) {
            if (!n) continue;
            
            const auto& state = n->getState();
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(state.membranePotential - state.restingPotential);
        }
        
        return neurons.empty() ? 0.0f : sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity using a structured approach
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    // Store activities in a vector for easier iteration
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find command with highest activity
    for (const auto& ca : commands) {
        if (ca.activity > bestActivity) {
            bestActivity = ca.activity;
            best = ca.cmd;
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
    if (curiosityLevel_ > 0.5f && brain_) {
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
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot apply reward modulation with null brain");
        }
        return;
    }
    
    // Validate input parameters
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_ERROR("Invalid reward modulation parameters: reward=" + 
                     std::to_string(reward) + ", predicted=" + std::to_string(predictedReward));
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward modulation to synapses
    applyRewardToSynapses(dopamineLevel_);
    
    // Update developmental plasticity
    updatePlasticityFromDopamine(dopamineLevel_);
}

void AgentBrain::applyRewardToSynapses(float dopamine) {
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_WARNING("Null synapse found in region");
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamine * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::updatePlasticityFromDopamine(float dopamine) {
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamine;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot update development with null brain");
        }
        return;
    }
    
    if (timestep < 0.0) {
        NLM_LOG_WARNING("Negative timestep in development update: " + std::to_string(timestep));
        return;
    }
    
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
    
    // Clear previous vision state
    previousVision_.clear();
}

} // namespace nlm
