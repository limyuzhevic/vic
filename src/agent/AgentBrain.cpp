#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>

namespace nlm {

// Helper function to calculate average activity in a neuron group
static float calculateNeuronActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (n == nullptr) continue;
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

// Helper function to distribute neurons into groups
static void distributeNeurons(const std::vector<Neuron*>& neurons,
                            std::vector<Neuron*>& group0,
                            std::vector<Neuron*>& group1,
                            std::vector<Neuron*>& group2,
                            std::vector<Neuron*>& group3,
                            std::vector<Neuron*>& group4,
                            std::vector<Neuron*>& group5,
                            size_t mod) {
    if (neurons.empty() || mod == 0) return;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (neurons[i] == nullptr) continue;
        switch (i % mod) {
            case 0: group0.push_back(neurons[i]); break;
            case 1: group1.push_back(neurons[i]); break;
            case 2: group2.push_back(neurons[i]); break;
            case 3: group3.push_back(neurons[i]); break;
            case 4: group4.push_back(neurons[i]); break;
            case 5: group5.push_back(neurons[i]); break;
        }
    }
}

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
    // Validate input
    assert(brain_ != nullptr);
    
    // Collect neurons by type
    std::vector<Neuron*> motorNeurons;
    std::vector<Neuron*> sensoryNeurons;
    
    for (const auto& region : brain_->getRegions()) {
        assert(region != nullptr);
        for (auto& pop : region->getPopulations()) {
            assert(pop != nullptr);
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                const auto& neurons = pop->getNeurons();
                motorNeurons.insert(motorNeurons.end(), neurons.begin(), neurons.end());
            } else if (type == NeuronType::Sensory) {
                const auto& neurons = pop->getNeurons();
                sensoryNeurons.insert(sensoryNeurons.end(), neurons.begin(), neurons.end());
            }
        }
    }
    
    // Distribute neurons into groups
    if (!motorNeurons.empty()) {
        distributeNeurons(motorNeurons,
                         motorForward_, motorBackward_, motorTurnLeft_,
                         motorTurnRight_, motorInteract_, motorWait_, 6);
    }
    
    if (!sensoryNeurons.empty()) {
        distributeNeurons(sensoryNeurons,
                         sensoryVision_, sensoryTouch_, sensoryInternal_,
                         sensoryProprioception_, {}, {}, 4);
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain is null");
        return;
    }
    
    // Validate world parameters
    if (world.getVisionWidth() <= 0 || world.getVisionHeight() <= 0) {
        NLM_LOG_ERROR("Invalid world dimensions: " + std::to_string(world.getVisionWidth()) + "x" + std::to_string(world.getVisionHeight()));
        return;
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " +
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    assert(brain_ != nullptr);
    assert(percept.getVision().size() == 256); // Vision is 16x16
    
    // Vision input processing
    const auto& vision = percept.getVision();
    const size_t visionMax = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < visionMax; ++i) {
        assert(sensoryVision_[i] != nullptr);
        float current = vision[i] * 5.0f;
        sensoryVision_[i]->injectCurrent(current);
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    const size_t touchMax = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < touchMax; ++i) {
        assert(sensoryTouch_[i] != nullptr);
        float current = touch[i] * 8.0f;
        sensoryTouch_[i]->injectCurrent(current);
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    const size_t internMax = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < internMax; ++i) {
        assert(sensoryInternal_[i] != nullptr);
        float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
        sensoryInternal_[i]->injectCurrent(current);
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    const size_t proprioMax = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < proprioMax; ++i) {
        assert(sensoryProprioception_[i] != nullptr);
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        sensoryProprioception_[i]->injectCurrent(current);
    }
    
    // Process novelty if we have vision data
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        const size_t maxIterations = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < maxIterations; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and decay
        noveltyLevel_ = totalDiff / std::max<size_t>(maxIterations, 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    assert(brain_ != nullptr);
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    
    // Find maximum activity using command array
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
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
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
    assert(brain_ != nullptr);
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
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
    assert(brain_ != nullptr);
    if (!rewardModulationEnabled_) return;
    
    // Validate inputs
    assert(std::isfinite(reward) && std::isfinite(predictedReward));
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated weight changes
    for (const auto& region : brain_->getRegions()) {
        assert(region != nullptr);
        for (auto* syn : region->getSynapses()) {
            assert(syn != nullptr);
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
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
    assert(brain_ != nullptr);
    if (!developmentEnabled_) return;
    
    assert(timestep >= 0.0);
    developmentalAge_ += timestep;
    
    // Set developmental stage based on age
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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

// Convenience methods

void AgentBrain::setBrain(std::shared_ptr<Brain> brain) {
    assert(brain != nullptr);
    brain_ = brain;
}

bool AgentBrain::validateState() const {
    if (!brain_) return false;
    
    // Check that all neuron vectors are valid (may be empty but shouldn't be nullptr)
    if (sensoryVision_.empty() && sensoryTouch_.empty() && 
        sensoryInternal_.empty() && sensoryProprioception_.empty()) {
        return false; // At least one sensory group should have neurons
    }
    
    return true;
}

float AgentBrain::getSensoryActivitySum() const {
    float totalActivity = 0.0f;
    totalActivity += calculateNeuronActivity(sensoryVision_);
    totalActivity += calculateNeuronActivity(sensoryTouch_);
    totalActivity += calculateNeuronActivity(sensoryInternal_);
    totalActivity += calculateNeuronActivity(sensoryProprioception_);
    return totalActivity;
}

float AgentBrain::getMotorActivitySum() const {
    float totalActivity = 0.0f;
    totalActivity += calculateNeuronActivity(motorForward_);
    totalActivity += calculateNeuronActivity(motorBackward_);
    totalActivity += calculateNeuronActivity(motorTurnLeft_);
    totalActivity += calculateNeuronActivity(motorTurnRight_);
    totalActivity += calculateNeuronActivity(motorInteract_);
    totalActivity += calculateNeuronActivity(motorWait_);
    return totalActivity;
}

} // namespace nlm
