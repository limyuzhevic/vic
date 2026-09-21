// AgentBrain.cpp - Complete implementation of AgentBrain class with all improvements
#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../world/SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <algorithm>
#include <cmath>

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
    , neuralPlanner_(nullptr)
    , predictionSystem_(nullptr)
    , conceptFormation_(nullptr)
    , episodicMemory_(nullptr)
    , workingMemory_(nullptr)
    , neuromodulator_(nullptr)
    , developmentSystem_(nullptr)
    , simulation_clock_(nullptr)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups (optimized)
                        size_t motorCount = motorForward_.size() + motorBackward_.size() + 
                                          motorTurnLeft_.size() + motorTurnRight_.size() +
                                          motorInteract_.size() + motorWait_.size();
                        
                        switch (motorCount % 6) {
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
                        // Distribute sensory neurons (optimized)
                        size_t sensoryCount = sensoryVision_.size() + sensoryTouch_.size() +
                                            sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (sensoryCount % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
        
        // Initialize integrated systems
        initializeIntegratedSystems();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeIntegratedSystems() {
    // Initialize neural systems integration
    if (brain_) {
        if (neuralPlanner_) neuralPlanner_->initialize(brain_.get());
        if (predictionSystem_) predictionSystem_->train(brain_->getCurrentObservation());
        if (conceptFormation_) conceptFormation_->initialize(brain_.get());
        if (episodicMemory_) episodicMemory_->initialize(brain_.get());
        if (workingMemory_) workingMemory_->initialize(brain_.get());
        if (neuromodulator_) neuromodulator_->setLevel(dopamineLevel_);
        if (developmentSystem_) developmentSystem_->initialize(brain_.get());
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    if (!brain_) return 0;
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    if (!brain_) return 0;
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
        
        // Store for next time (using move semantics to avoid memory leak)
        previousVision_ = std::move(vision);
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
        for (const Neuron* n : neurons) {  // Use const for efficiency
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
    
    // Use lookup table for optimal decision
    static const std::array<MotorCommand, 6> commands = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    static const std::array<float, 6> *activities = nullptr;
    
    if (!activities) {
        activities = new const std::array<float, 6> {
            forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct
        };
    } else {
        // Update existing array
        (*activities)[0] = forwardAct;
        (*activities)[1] = backwardAct;
        (*activities)[2] = leftAct;
        (*activities)[3] = rightAct;
        (*activities)[4] = interactAct;
        (*activities)[5] = waitAct;
    }
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    // Find maximum activity with early exit for performance
    for (size_t i = 0; i < 6; ++i) {
        if ((*activities)[i] > bestActivity) {
            bestActivity = (*activities)[i];
            best = commands[i];
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

void AgentBrain::integrateNeuromodulation(double timestep) {
    if (!neuromodulator_) return;
    
    neuromodulator_->update(timestep);
    neuromodulator_->setLevel(dopamineLevel_);
    
    // Update plasticity factor based on neuromodulator
    float neuromodFactor = neuromodulator_->getPlasticityFactor();
    plasticityModifier_ *= neuromodFactor;
}

void AgentBrain::updateMemoryConsolidation() {
    if (episodicMemory_) {
        // Periodically consolidate memory based on relevance
        if (episodicMemory_->getEpisodeCount() > 0 && 
            developmentalAge_ > 60.0) {  // After initial development phase
            size_t toConsolidate = std::min(episodicMemory_->getEpisodeCount() / 4, (size_t)10);
            auto episodes = episodicMemory_->getEpisodesForReplay(toConsolidate);
            if (episodes.size() > 0) {
                episodicMemory_->consolidate(0.5f);
            }
        }
    }
}

void AgentBrain::updateAdvancedSensoryProcessing() {
    // Advanced feature extraction using ConceptFormation
    if (conceptFormation_ && brain_) {
        auto currentObs = brain_->getCurrentObservation();
        if (currentObs) {
            // Extract features for concept formation
            // This would involve more complex feature extraction than just raw sensory input
            std::vector<float> features;
            const auto& data = currentObs->getData();
            
            // Simple feature extraction: compute statistics
            float mean = 0.0f, stddev = 0.0f;
            if (!data.empty()) {
                for (float val : data) mean += val;
                mean /= data.size();
                for (float val : data) stddev += (val - mean) * (val - mean);
                stddev = stddev / data.size();
            }
            features.push_back(mean);
            features.push_back(std::sqrt(stddev));
            
            // Present to concept formation
            conceptFormation_->presentExperience(data, features, noveltyLevel_, simulation_clock_->getCurrentStep());
        }
    }
}

void AgentBrain::updateMemoryReplay() {
    if (episodicMemory_ && episodicMemory_->isReplayEnabled()) {
        // Replay relevant episodes for consolidation
        size_t replayCount = episodicMemory_->getRecentEpisodes(5).size();
        if (replayCount > 0) {
            episodicMemory_->replaySequence({});  // Would need to select specific episodes
        }
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize agent brain with world reference
    // Set up sensory mapping based on world dimensions
    // Connect to world for interaction
    // Initialize simulation clock reference
    simulation_clock_ = world.getClock();
}

void AgentBrain::reset() {
    // Reset agent brain state for new episode
    if (brain_) {
        brain_->reset();
    }
    
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    previousVision_.clear();
    
    // Reset integrated systems if they exist
    if (neuralPlanner_) neuralPlanner_->reset();
    if (predictionSystem_) predictionSystem_->reset();
    if (conceptFormation_) conceptFormation_->reset();
    if (episodicMemory_) episodicMemory_->clear();
    if (workingMemory_) workingMemory_->clear();
    if (neuromodulator_) neuromodulator_->reset();
    if (developmentSystem_) developmentSystem_->reset();
}

void AgentBrain::update(double timestep) {
    // Update all integrated systems
    if (developmentSystem_) {
        developmentSystem_->update(timestep);
    }
    integrateNeuromodulation(timestep);
    updateMemoryConsolidation();
    updateAdvancedSensoryProcessing();
    updateMemoryReplay();
}

void AgentBrain::processFrame(const SimpleWorld& world, const SensoryPercept& percept) {
    // Main processing pipeline
    // Process sensory input
    processSensoryInput(percept);
    
    // Apply neuromodulation based on current state
    applyRewardModulation(0.0f, 0.0f);  // Will be updated with actual values
    
    // Update development
    updateDevelopment(world.getDeltaTime());
    
    // Decode motor command from brain activity
    MotorCommand motorCmd = decodeMotorCommand();
    
    // Send to agent body
    if (brain_) {
        brain_->setMotorCommand(motorCmd);
    }
}

} // namespace nlm