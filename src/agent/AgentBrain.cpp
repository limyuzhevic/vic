#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
{
    // Initialize neuromodulation state
    neuromodulationState_.dopamine = 0.0f;
    neuromodulationState_.curiosity = 0.0f;
    neuromodulationState_.novelty = 0.0f;
    neuromodulationState_.predictionError = 0.0f;
    neuromodulationState_.expectedReward = 0.0f;
    neuromodulationState_.developmentalAge = 0.0;
    neuromodulationState_.plasticityModifier = 1.0f;
    
    // Initialize configuration flags
    rewardModulationEnabled_ = true;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    curiosityEnabled_ = true;
    
    // Distribute neurons after brain is set
    if (brain_) {
        distributeMotorNeurons();
        distributeSensoryNeurons();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    neuromodulationState_.developmentalAge = 0.0;
    neuromodulationState_.plasticityModifier = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryGroups_.vision.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryGroups_.touch.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryGroups_.internal.size()) + " internal sensory neurons");
}

void AgentBrain::distributeMotorNeurons() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons to different action groups
                    size_t idx = motorGroups_.forward.size() + motorGroups_.backward.size() + 
                                motorGroups_.turnLeft.size() + motorGroups_.turnRight.size() +
                                motorGroups_.interact.size() + motorGroups_.wait.size();
                    
                    switch (idx % 6) {
                        case 0: motorGroups_.forward.push_back(n); break;
                        case 1: motorGroups_.backward.push_back(n); break;
                        case 2: motorGroups_.turnLeft.push_back(n); break;
                        case 3: motorGroups_.turnRight.push_back(n); break;
                        case 4: motorGroups_.interact.push_back(n); break;
                        case 5: motorGroups_.wait.push_back(n); break;
                    }
                }
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute sensory neurons
                    size_t idx = sensoryGroups_.vision.size() + sensoryGroups_.touch.size() +
                                sensoryGroups_.internal.size() + sensoryGroups_.proprioception.size();
                    
                    switch (idx % 4) {
                        case 0: sensoryGroups_.vision.push_back(n); break;
                        case 1: sensoryGroups_.touch.push_back(n); break;
                        case 2: sensoryGroups_.internal.push_back(n); break;
                        case 3: sensoryGroups_.proprioception.push_back(n); break;
                    }
                }
            }
        }
    }
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
    
    // Vision input (256 values -> vision neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryGroups_.vision.size() && i < vision.size(); ++i) {
        if (sensoryGroups_.vision[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryGroups_.vision[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> touch neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryGroups_.touch.size() && i < touch.size(); ++i) {
        if (sensoryGroups_.touch[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryGroups_.touch[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> internal neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryGroups_.internal.size() && i < intern.size(); ++i) {
        if (sensoryGroups_.internal[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryGroups_.internal[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> proprioception neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryGroups_.proprioception.size() && i < proprio.size(); ++i) {
        if (sensoryGroups_.proprioception[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryGroups_.proprioception[i]->injectCurrent(current);
        }
    }
    
    // Update novelty detection
    updateNovelty(vision);
}

void AgentBrain::updateNovelty(const std::vector<float>& currentVision) {
    if (!curiosityEnabled_) return;
    
    // Compute novelty (difference from previous vision)
    if (!currentVision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < currentVision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(currentVision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        neuromodulationState_.novelty = totalDiff / std::max<size_t>(currentVision.size(), 1);
        
        // Decay and update
        neuromodulationState_.novelty *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = currentVision;
    }
    
    // Update curiosity based on novelty
    neuromodulationState_.curiosity = neuromodulationState_.novelty * 2.0f + 
                                    std::abs(neuromodulationState_.predictionError) * 0.5f;
    neuromodulationState_.curiosity = std::clamp(neuromodulationState_.curiosity, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && neuromodulationState_.curiosity > 0.3f) {
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
    
    float forwardAct = calcActivity(motorGroups_.forward);
    float backwardAct = calcActivity(motorGroups_.backward);
    float leftAct = calcActivity(motorGroups_.turnLeft);
    float rightAct = calcActivity(motorGroups_.turnRight);
    float interactAct = calcActivity(motorGroups_.interact);
    float waitAct = calcActivity(motorGroups_.wait);
    
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
    if (neuromodulationState_.curiosity > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = neuromodulationState_.curiosity * 0.3f;  // Up to 30% random
        
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
    neuromodulationState_.predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    neuromodulationState_.expectedReward = 0.95f * neuromodulationState_.expectedReward + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    neuromodulationState_.dopamine = neuromodulationState_.predictionError;
    
    // Clamp to reasonable range
    neuromodulationState_.dopamine = std::clamp(neuromodulationState_.dopamine, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * neuromodulationState_.dopamine * neuromodulationState_.plasticityModifier;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * neuromodulationState_.dopamine;
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
    
    neuromodulationState_.developmentalAge += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (neuromodulationState_.developmentalAge < 60.0) {  // ~1 minute
        neuromodulationState_.plasticityModifier = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (neuromodulationState_.developmentalAge < 300.0) {  // ~5 minutes
        neuromodulationState_.plasticityModifier = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (neuromodulationState_.developmentalAge < 900.0) {  // ~15 minutes
        neuromodulationState_.plasticityModifier = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        neuromodulationState_.plasticityModifier = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * neuromodulationState_.plasticityModifier;
            float pruneRate = 0.00001f * (2.0f - neuromodulationState_.plasticityModifier);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

void AgentBrain::reset() {
    neuromodulationState_.dopamine = 0.0f;
    neuromodulationState_.curiosity = 0.0f;
    neuromodulationState_.novelty = 0.0f;
    neuromodulationState_.predictionError = 0.0f;
    neuromodulationState_.expectedReward = 0.0f;
    neuromodulationState_.developmentalAge = 0.0;
    neuromodulationState_.plasticityModifier = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm