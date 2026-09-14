#include "SensorimotorInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SensorimotorInterface::Impl {
    // Spike event handlers for real-time monitoring
    std::function<void(const DetailedSpikeEvent&)> spikeHandler;
    std::function<void(const DelayedSpikeEvent&)> delayedSpikeHandler;
    
    // State update callbacks
    std::function<void(const std::vector<float>&, const std::vector<NeuronId>&, float, float)> stateUpdateCallback;
    
    Impl() : spikeHandler(nullptr), delayedSpikeHandler(nullptr), stateUpdateCallback(nullptr) {}
};

SensorimotorInterface::SensorimotorInterface()
    : pImpl(new Impl)
    , brain_(nullptr)
    , agentBrain_(nullptr)
    , motorFeedbackEnabled_(true)
    , neuromodulationSharingEnabled_(true)
    , developmentCoordinationEnabled_(true)
    , sensorySampleRate_(10.0f)  // 10 Hz update rate
    , motorFeedbackLatency_(0.05f)  // 50ms latency
    , neuromodulationGain_(1.0f)
    , currentState_()
    , previousState_()
{
    NLM_LOG_INFO("SensorimotorInterface created");
}

SensorimotorInterface::~SensorimotorInterface() = default;

void SensorimotorInterface::initialize(Brain* brain, AgentBrain* agentBrain) {
    if (!brain || !agentBrain) {
        NLM_LOG_ERROR("SensorimotorInterface: Cannot initialize with null pointers");
        return;
    }
    
    brain_ = brain;
    agentBrain_ = agentBrain;
    
    // Register spike event handlers for real-time monitoring
    auto spikeSystem = brain_->getSpikeSystem();
    if (spikeSystem) {
        pImpl->spikeHandler = [this](const DetailedSpikeEvent& event) {
            // Update current state with spike information
            updateStateFromSpike(event);
        };
        
        pImpl->delayedSpikeHandler = [this](const DelayedSpikeEvent& event) {
            // Update state when delayed spikes are delivered
            updateStateFromDelayedSpike(event);
        };
        
        spikeSystem->registerHandler(pImpl->spikeHandler);
        spikeSystem->registerDelayedHandler(pImpl->delayedSpikeHandler);
        
        NLM_LOG_INFO("SensorimotorInterface: Registered spike event handlers");
    }
    
    // Register state update callback
    pImpl->stateUpdateCallback = [this](const std::vector<float>& activity,
                                        const std::vector<NeuronId>& firingNeurons,
                                        float avgFiringRate,
                                        float eInertiaRatio) {
        currentState_.neuralActivityPattern = activity;
        currentState_.activeNeurons = firingNeurons;
        currentState_.averageFiringRate = avgFiringRate;
        currentState_.eInertiaRatio = eInertiaRatio;
    };
    
    // Reset state
    reset();
    
    NLM_LOG_INFO("SensorimotorInterface initialized successfully");
}

void SensorimotorInterface::sendBrainState(const std::vector<float>& neuralActivity,
                                         const std::vector<NeuronId>& firingNeurons,
                                         float averageFiringRate,
                                         float eInertiaRatio) {
    if (!brain_ || !agentBrain_) {
        return;
    }
    
    // Update current state
    currentState_.neuralActivityPattern = neuralActivity;
    currentState_.activeNeurons = firingNeurons;
    currentState_.averageFiringRate = averageFiringRate;
    currentState_.eInertiaRatio = eInertiaRatio;
    
    // Get neuromodulation levels
    auto dopamine = brain_->getDopamine();
    auto novelty = brain_->getNovelty();
    auto curiosity = brain_->getCuriosity();
    auto predictionError = brain_->getPredictionErrorSignal();
    auto development = brain_->getDevelopmentSystem();
    
    if (dopamine) currentState_.dopamineLevel = dopamine->getLevel();
    if (novelty) currentState_.noveltyLevel = novelty->getLevel();
    if (curiosity) currentState_.curiosityLevel = curiosity->getLevel();
    if (predictionError) currentState_.predictionError = predictionError->getLevel();
    
    if (development) {
        currentState_.developmentalStage = development->getCurrentStage();
        currentState_.developmentalAge = development->getDevelopmentalAge();
        currentState_.plasticityModifier = development->getPlasticityModifier();
    }
    
    // Detect significant state changes
    detectAndHandleStateChanges();
}

void SensorimotorInterface::sendMotorFeedback(ActionType expectedAction,
                                            ActionType actualAction,
                                            float reward,
                                            const std::vector<float>& actionOutcome) {
    if (!agentBrain_ || !motorFeedbackEnabled_) {
        return;
    }
    
    // Apply latency to simulate real-world delay
    if (motorFeedbackLatency_ > 0.0f) {
        // Store for delayed application
        actionOutcomes_.push_back({reward, actualAction});
        if (actionOutcomes_.size() > 10) {
            actionOutcomes_.erase(actionOutcomes_.begin());
        }
    }
    
    // Process motor outcome for learning
    if (expectedAction != actualAction) {
        // Mismatch - record for learning
        NLM_LOG_INFO("SensorimotorInterface: Motor command mismatch - " +
                     std::to_string(static_cast<int>(expectedAction)) + " vs " +
                     std::to_string(static_cast<int>(actualAction)));
    }
    
    // Apply reward modulation through agent brain
    agentBrain_->applyRewardModulation(reward, 0.0f);  // Use agent brain's reward processing
    
    // Update action quality in neural planner if available
    auto planner = brain_->getPlanner();
    if (planner) {
        std::vector<ActionType> plannedActions = {expectedAction};
        std::vector<ActionType> actualActions = {actualAction};
        planner->updatePlanQuality(plannedActions, actualActions, reward);
    }
}

void SensorimotorInterface::sendNeuromodulationUpdate(float dopamineLevel,
                                                     float noveltyLevel,
                                                     float curiosityLevel,
                                                     float predictionError) {
    if (!agentBrain_ || !neuromodulationSharingEnabled_) {
        return;
    }
    
    // Update agent's internal neuromodulation state
    // This allows the agent to adapt its behavior based on brain neuromodulation
    float curiosityFromBrain = curiosityLevel * neuromodulationGain_;
    float noveltyFromBrain = noveltyLevel * neuromodulationGain_;
    
    // Apply to agent brain (which has its own neuromodulation state)
    if (curiosityFromBrain > agentBrain_->getCuriosityLevel()) {
        // Increase curiosity if brain shows more novelty
        // This encourages exploration when the environment is novel
    }
    
    // Store for learning and adaptation
    recentActions_.push_back({ActionType::Wait, std::vector<float>{dopamineLevel, noveltyLevel, curiosityLevel, predictionError}});
    if (recentActions_.size() > 100) {
        recentActions_.erase(recentActions_.begin());
    }
}

void SensorimotorInterface::sendDevelopmentStage(DevelopmentalStage stage,
                                                float developmentalAge,
                                                float plasticityModifier) {
    if (!agentBrain_ || !developmentCoordinationEnabled_) {
        return;
    }
    
    // Coordinate development stages between brain and agent
    // This ensures the agent adapts its behavior as the brain develops
    
    // Update agent's developmental stage
    // Note: AgentBrain doesn't have developmental stage directly, but we can infer from plasticity
    if (plasticityModifier > 0.5f) {
        // High plasticity = early developmental stage
        // Agent can be more exploratory and learn faster
    }
}

void SensorimotorInterface::processSensoryInput(const SensoryPercept& percept) {
    if (!agentBrain_) {
        return;
    }
    
    // Process sensory input through agent brain
    agentBrain_->processSensoryInput(percept);
}

ActionType SensorimotorInterface::decodeMotorCommand() {
    if (!agentBrain_) {
        return ActionType::Wait;
    }
    
    // Decode motor command from agent's brain
    return agentBrain_->decodeMotorCommand();
}

void SensorimotorInterface::applyRewardModulation(float reward, float predictedReward) {
    if (!agentBrain_) {
        return;
    }
    
    // Apply reward modulation through agent brain
    agentBrain_->applyRewardModulation(reward, predictedReward);
}

void SensorimotorInterface::updateDevelopment(double timestep) {
    if (!agentBrain_) {
        return;
    }
    
    // Update development through agent brain
    agentBrain_->updateDevelopment(timestep);
}

SensorimotorInterface::CurrentBrainState SensorimotorInterface::getCurrentBrainState() const {
    return currentState_;
}

void SensorimotorInterface::reset() {
    currentState_ = CurrentBrainState();
    previousState_ = CurrentBrainState();
    recentActions_.clear();
    actionOutcomes_.clear();
    
    NLM_LOG_INFO("SensorimotorInterface reset");
}

void SensorimotorInterface::updateStateFromSpike(const DetailedSpikeEvent& event) {
    // Extract state information from spike event
    // This provides real-time brain state updates
    
    // For now, we'll just log spikes
    // In a full implementation, we would update neural activity patterns
    // based on which neurons are firing
    
    if (currentState_.averageFiringRate > 100.0f) {
        NLM_LOG_DEBUG("SensorimotorInterface: High firing rate spike detected");
    }
}

void SensorimotorInterface::updateStateFromDelayedSpike(const DelayedSpikeEvent& event) {
    // Update state when delayed spikes are delivered to neurons
    // This provides feedback about neural connectivity and signal propagation
    
    // Check if this affects motor neurons
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == event.destination_neuron) {
                        // This is an important connection - update state
                        if (currentState_.neuralActivityPattern.size() > 100) {
                            // Add some activity pattern update
                            // For now, just increment average firing rate
                            currentState_.averageFiringRate = std::min(100.0f, 
                                                                     currentState_.averageFiringRate + 5.0f);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void SensorimotorInterface::detectAndHandleStateChanges() {
    // Compare current state with previous state to detect significant changes
    bool stateChanged = false;
    
    // Check for significant changes in key metrics
    if (std::abs(currentState_.averageFiringRate - previousState_.averageFiringRate) > 10.0f) {
        stateChanged = true;
        NLM_LOG_INFO("SensorimotorInterface: Significant firing rate change detected: " +
                     std::to_string(currentState_.averageFiringRate) + " Hz");
    }
    
    if (std::abs(currentState_.dopamineLevel - previousState_.dopamineLevel) > 0.1f) {
        stateChanged = true;
        NLM_LOG_INFO("SensorimotorInterface: Significant dopamine level change detected");
    }
    
    // Handle state changes
    if (stateChanged) {
        // Trigger any registered callbacks
        if (pImpl->stateUpdateCallback) {
            pImpl->stateUpdateCallback(currentState_.neuralActivityPattern,
                                      currentState_.activeNeurons,
                                      currentState_.averageFiringRate,
                                      currentState_.eInertiaRatio);
        }
    }
    
    // Update previous state
    previousState_ = currentState_;
}

} // namespace nlm