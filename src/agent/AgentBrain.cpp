#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

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
{
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
        return;
    }

    try {
        // Initialize motor and sensory neuron groups with better error handling
        initializeNeuronGroups();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize AgentBrain neuron groups: " + std::string(e.what()));
        // Continue with empty neuron groups
    }
}

AgentBrain::~AgentBrain() {
    // Clear neuron pointers to prevent double deletion
    clearNeuronPointers();
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain_ is null");
        return;
    }

    try {
        // Use world parameters if available, otherwise use defaults
        size_t visionWidth = world.getVisionWidth();
        size_t visionHeight = world.getVisionHeight();
        
        previousVision_.resize(visionWidth * visionHeight, 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized successfully:");
        NLM_LOG_INFO("  Vision size: " + std::to_string(visionWidth) + "x" + std::to_string(visionHeight));
        NLM_LOG_INFO("  Vision sensory neurons: " + std::to_string(sensoryVision_.size()));
        NLM_LOG_INFO("  Touch sensory neurons: " + std::to_string(sensoryTouch_.size()));
        NLM_LOG_INFO("  Internal sensory neurons: " + std::to_string(sensoryInternal_.size()));
        NLM_LOG_INFO("  Proprioception neurons: " + std::to_string(sensoryProprioception_.size()));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error initializing AgentBrain: " + std::string(e.what()));
        // Re-throw for calling code to handle
        throw;
    }
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) {
        throw std::runtime_error("Cannot initialize neuron groups: brain_ is null");
    }
    
    // Clear existing groups before re-initialization
    clearNeuronPointers();
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                addMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                addSensoryNeurons(pop->getNeurons());
            }
        }
    }
    
    // Log summary
    size_t totalMotor = motorForward_.size() + motorBackward_.size() + 
                       motorTurnLeft_.size() + motorTurnRight_.size() +
                       motorInteract_.size() + motorWait_.size();
    size_t totalSensory = sensoryVision_.size() + sensoryTouch_.size() +
                        sensoryInternal_.size() + sensoryProprioception_.size();
    
    NLM_LOG_INFO("Neuron groups initialized: " + std::to_string(totalMotor) + 
                " motor, " + std::to_string(totalSensory) + " sensory neurons");
}

void AgentBrain::clearNeuronPointers() {
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

void AgentBrain::addMotorNeurons(const std::vector<Neuron*>& neurons) {
    if (!neurons.empty()) {
        // Distribute motor neurons to different action groups
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                    motorTurnLeft_.size() + motorTurnRight_.size() +
                    motorInteract_.size() + motorWait_.size();
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            Neuron* n = neurons[i];
            if (!n) {
                continue;  // Skip null pointers
            }
            
            switch ((idx + i) % 6) {
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

void AgentBrain::addSensoryNeurons(const std::vector<Neuron*>& neurons) {
    if (!neurons.empty()) {
        // Distribute sensory neurons
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                    sensoryInternal_.size() + sensoryProprioception_.size();
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            Neuron* n = neurons[i];
            if (!n) {
                continue;  // Skip null pointers
            }
            
            switch ((idx + i) % 4) {
                case 0: sensoryVision_.push_back(n); break;
                case 1: sensoryTouch_.push_back(n); break;
                case 2: sensoryInternal_.push_back(n); break;
                case 3: sensoryProprioception_.push_back(n); break;
            }
        }
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Calculate based on actual neuron groups rather than hardcoded values
    size_t totalInputSize = 0;
    
    // Vision: each vision neuron expects one input value
    // This is derived from the brain's vision percept structure
    for (const auto* neuron : sensoryVision_) {
        if (neuron) {
            totalInputSize++;
        }
    }
    
    // Touch: each touch neuron expects one input value
    for (const auto* neuron : sensoryTouch_) {
        if (neuron) {
            totalInputSize++;
        }
    }
    
    // Internal: each internal neuron expects one input value
    for (const auto* neuron : sensoryInternal_) {
        if (neuron) {
            totalInputSize++;
        }
    }
    
    // Proprioception: each proprioception neuron expects one input value
    for (const auto* neuron : sensoryProprioception_) {
        if (neuron) {
            totalInputSize++;
        }
    }
    
    // Safety check: if no neurons, return a reasonable default
    if (totalInputSize == 0) {
        NLM_LOG_WARNING("No sensory neurons found, returning default input size");
        totalInputSize = 256 + 8 + 4 + 6;  // Original hardcoded values
    }
    
    return totalInputSize;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Each motor action group represents one output dimension
    return 6;  // One for each action type (Forward, Backward, Left, Right, Interact, Wait)
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: brain_ is null");
        return;
    }
    
    try {
        // Process each sensory modality with error checking
        processVisionInput(percept);
        processTouchInput(percept);
        processInternalInput(percept);
        processProprioceptionInput(percept);
        
        // Compute novelty with enhanced error handling
        computeNovelty(percept);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error processing sensory input: " + std::string(e.what()));
        // Continue with other processing
    }
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron && neuron->getMembranePotential() >= 0.0f) {  // Check if neuron exists and is valid
            // Inject current proportional to vision intensity with bounds checking
            float current = vision[i] * 5.0f;
            // Clamp current to reasonable range
            current = std::max(0.0f, std::min(current, 100.0f));
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * 8.0f;
            current = std::max(0.0f, std::min(current, 100.0f));
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            // Center around zero and clamp
            current = std::clamp(current, -50.0f, 50.0f);
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            current = std::clamp(current, -30.0f, 30.0f);
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t validSamples = 0;
        
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
            validSamples++;
        }
        
        if (validSamples > 0) {
            // Normalize by number of samples
            noveltyLevel_ = totalDiff / static_cast<float>(validSamples);
            
            // Apply decay
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Store for next time
            previousVision_ = vision;
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain_ is null");
        return MotorCommand::Wait;
    }
    
    try {
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration with bounds checking
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
        
        return decoded;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error decoding motor command: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group with error checking
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) continue;  // Skip null pointers
            
            try {
                // Use membrane potential deviation from rest as activity measure
                float potential = n->getMembranePotential();
                float restingPotential = n->getState().restingPotential;
                sum += std::abs(potential - restingPotential);
                validNeurons++;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error accessing neuron state: " + std::string(e.what()));
                continue;
            }
        }
        
        if (validNeurons == 0) return 0.0f;
        return sum / validNeurons;
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity with enhanced decision logic
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
    
    // Enhanced decision logic with confidence threshold
    if (bestActivity < 0.5f) {
        // Low confidence - choose wait or random exploration
        if (curiosityLevel_ > 0.7f) {
            // High curiosity - random action
            return selectWithCuriosity(MotorCommand::Wait);
        } else {
            return MotorCommand::Wait;
        }
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
    
    try {
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply to all synapses with eligibility traces
        applyRewardToSynapses(dopamineLevel_);
        
        // Modulate plasticity based on dopamine
        modulatePlasticity(dopamineLevel_);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in reward modulation: " + std::string(e.what()));
    }
}

void AgentBrain::applyRewardToSynapses(float dopamine) {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;  // Safety check
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamine * plasticityModifier_;
                // Clamp delta to prevent large weight changes
                delta = std::clamp(delta, -0.1f, 0.1f);
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::modulatePlasticity(float dopamine) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamine;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        float ltpWeight = 0.01f * plasticityFactor;
        float ltdWeight = 0.012f * plasticityFactor;
        
        // Clamp to reasonable ranges
        ltpWeight = std::clamp(ltpWeight, 0.001f, 0.05f);
        ltdWeight = std::clamp(ltdWeight, 0.001f, 0.06f);
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    try {
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
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
                
                // Clamp to reasonable ranges
                synRate = std::clamp(synRate, 0.00001f, 0.001f);
                pruneRate = std::clamp(pruneRate, 0.000001f, 0.0001f);
                
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in development update: " + std::string(e.what()));
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
    try {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Clear previous vision
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        
        NLM_LOG_INFO("AgentBrain reset successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error resetting AgentBrain: " + std::string(e.what()));
    }
}

} // namespace nlm