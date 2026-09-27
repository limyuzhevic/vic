#include "AgentBrain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , curiosityThreshold_(0.3f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
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
    
    // ========== PREDICTION TRAINING ==========
    
    // Convert current sensory input to format compatible with brain's prediction system
    std::vector<float> currentSensoryState;
    if (!vision.empty()) {
        // Combine all sensory modalities for prediction
        currentSensoryState.reserve(vision.size() + touch.size() + intern.size() + proprio.size());
        
        // Add vision (primary prediction target)
        for (float v : vision) {
            currentSensoryState.push_back(v);
        }
        
        // Add touch
        for (float t : touch) {
            currentSensoryState.push_back(t);
        }
        
        // Add internal
        for (float i : intern) {
            currentSensoryState.push_back(i);
        }
        
        // Add proprioception
        for (float p : proprio) {
            currentSensoryState.push_back(p);
        }
    }
    
    if (!currentSensoryState.empty() && brain_->getPredictionSystem()) {
        // ========== PREDICTION GENERATION ==========
        
        // Get current step from brain's simulation clock
        SimulationStep currentStep = 0;  // Default, would need access to simulation clock
        
        // Check if we have a prediction system and can generate predictions
        NeuralPrediction* neuralPredictor = dynamic_cast<NeuralPrediction*>(brain_->getPredictionSystem());
        if (neuralPredictor) {
            // Record current sensory state for learning
            neuralPredictor->recordSensoryState(currentSensoryState, currentStep);
            
            // Generate prediction for next state
            std::vector<float> predictedState = neuralPredictor->generatePrediction(currentStep);
            
            if (!predictedState.empty()) {
                // Store prediction for comparison with actual input
                predictedSensoryState_ = predictedState;
                
                // Update prediction confidence based on prediction system
                predictionConfidence_ = neuralPredictor->getPredictionConfidence();
                
                // ========== PREDICTION ERROR COMPUTATION ==========
                
                // Compute prediction error between predicted and actual sensory input
                float predictionError = computePredictionError(predictedState, currentSensoryState);
                
                // Store prediction error for learning
                predictionErrorHistory_.push_back(predictionError);
                if (predictionErrorHistory_.size() > 100) {
                    predictionErrorHistory_.pop_front();
                }
                
                // Update AgentBrain's prediction error for neuromodulation
                predictionError_ = predictionError;
                
                // ========== PREDICTION ERROR LEARNING ==========
                
                // Use prediction error for synaptic plasticity and learning
                applyPredictionErrorLearning(predictionError, currentSensoryState);
                
                // Update prediction system with actual observation
                float learningResult = neuralPredictor->updateWithObservation(currentSensoryState, currentStep);
                
                // Propagate prediction error to neuromodulation system
                if (brain_->getPredictionErrorSignal()) {
                    brain_->getPredictionErrorSignal()->setPredictionError(predictionError);
                    brain_->getPredictionErrorSignal()->update(0.001);  // timestep
                }
            }
        } else {
            // Fallback to basic prediction system training
            // Create a simple sensory input from current state
            class SimpleSensoryInput : public SensoryInput {
            public:
                SimpleSensoryInput(const std::vector<float>& data) : data_(data) {}
                const char* getType() const override { return "Simple"; }
                const std::vector<float>& getData() const override { return data_; }
                size_t getDimensions() const override { return data_.size(); }
                std::unique_ptr<SensoryInput> clone() const override {
                    return std::make_unique<SimpleSensoryInput>(data_);
                }
            private:
                std::vector<float> data_;
            };
            
            SimpleSensoryInput sensoryObservation(currentSensoryState);
            
            // Train the prediction model with this observation
            brain_->getPredictionSystem()->train(sensoryObservation);
            
            // Make prediction for next state
            auto prediction = brain_->getPredictionSystem()->predictNextState(sensoryObservation);
            
            if (prediction) {
                // Compute prediction error
                float predictionError = computePredictionError(prediction->getData(), currentSensoryState);
                
                // Update predictions with actual observation
                brain_->getPredictionSystem()->updatePredictions(prediction->getData(), currentSensoryState);
                
                // Store prediction error
                predictionError_ = predictionError;
                predictionErrorHistory_.push_back(predictionError);
                if (predictionErrorHistory_.size() > 100) {
                    predictionErrorHistory_.pop_front();
                }
                
                // Apply prediction error learning
                applyPredictionErrorLearning(predictionError, currentSensoryState);
            }
        }
    }
    
    // ========== PREDICTION-INFORMED ACTION SELECTION ==========
    
    // Adjust action selection based on prediction confidence and error
    if (curiosityEnabled_ && (predictionConfidence_ < 0.5f || std::abs(predictionError_) > 0.1f)) {
        // High prediction uncertainty increases exploration
        // Lower curiosity threshold when prediction confidence is low
        curiosityThreshold_ = 0.2f;
    } else {
        curiosityThreshold_ = 0.3f;
    }
    
    // Update curiosity based on prediction error and novelty
    if (curiosityEnabled_) {
        float errorComponent = std::abs(predictionError_) * 2.0f;
        curiosityLevel_ = noveltyLevel_ * 1.5f + errorComponent * 0.3f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
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
}

// Helper method to compute prediction error
float AgentBrain::computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.empty() || actual.empty()) return 1.0f;  // Maximum error if no predictions
    
    // Ensure both vectors are same size
    size_t minSize = std::min(predicted.size(), actual.size());
    if (minSize == 0) return 1.0f;
    
    float totalError = 0.0f;
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        totalError += diff;
    }
    
    // Normalize error to [0, 1] range
    float normalizedError = totalError / static_cast<float>(minSize);
    return std::clamp(normalizedError, 0.0f, 1.0f);
}

// Helper method to apply prediction error learning
void AgentBrain::applyPredictionErrorLearning(float error, const std::vector<float>& currentState) {
    if (!brain_ || error < 0.05f) return;  // Skip if error is small
    
    // Use prediction error to modulate synaptic plasticity
    // Positive error strengthens synapses that predicted incorrectly
    // Negative error weakens synapses that predicted too strongly
    
    float learningRate = error * 0.01f;  // Scale learning rate by error magnitude
    
    // Apply to motor neurons based on prediction error
    // This helps the agent learn better action sequences
    
    // Modulate motor neuron excitability based on prediction error
    float errorSignal = error > 0.0f ? 1.0f : -1.0f;
    
    // Apply error-driven plasticity to motor neurons
    for (auto* neuron : motorForward_) {
        if (neuron) {
            float modulation = learningRate * errorSignal * 0.1f;
            neuron->injectCurrent(modulation);
        }
    }
    
    for (auto* neuron : motorBackward_) {
        if (neuron) {
            float modulation = learningRate * errorSignal * 0.1f;
            neuron->injectCurrent(modulation);
        }
    }
    
    for (auto* neuron : motorTurnLeft_) {
        if (neuron) {
            float modulation = learningRate * errorSignal * 0.1f;
            neuron->injectCurrent(modulation);
        }
    }
    
    for (auto* neuron : motorTurnRight_) {
        if (neuron) {
            float modulation = learningRate * errorSignal * 0.1f;
            neuron->injectCurrent(modulation);
        }
    }
    
    for (auto* neuron : motorInteract_) {
        if (neuron) {
            float modulation = learningRate * errorSignal * 0.1f;
            neuron->injectCurrent(modulation);
        }
    }
    
    // Apply prediction error to curiosity modulation
    curiosityLevel_ = std::clamp(curiosityLevel_ + error * 0.05f, 0.0f, 1.0f);
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
    
    // ========= ENHANCED PREDICTION ERROR CALCULATION =========
    
    // Compute prediction error with temporal difference learning
    // Combine immediate reward prediction error with expected reward prediction error
    float rewardPredictionError = reward - predictedReward;
    
    // Enhance prediction error with temporal context
    // Use exponential moving average of past prediction errors
    static const float PREDICTION_ERROR_DECAY = 0.9f;
    float enhancedPredictionError = PREDICTION_ERROR_DECAY * predictionError_ + 
                                  (1.0f - PREDICTION_ERROR_DECAY) * rewardPredictionError;
    
    // Scale error by uncertainty (inverse of prediction confidence)
    float confidenceFactor = 1.0f / (1.0f + std::abs(predictionError_) * 2.0f);
    enhancedPredictionError *= confidenceFactor;
    
    // Store for historical context
    predictionErrorHistory_.push_back(enhancedPredictionError);
    if (predictionErrorHistory_.size() > 100) {
        predictionErrorHistory_.pop_front();
    }
    
    // Use enhanced prediction error for neuromodulation
    predictionError_ = enhancedPredictionError;
    
    // ========= NEUROMODULATION SYSTEM INTEGRATION =========
    
    // Update Dopamine: encodes reward prediction error (classical RL)
    float dopamine = rewardPredictionError;
    
    // Update Acetylcholine: encodes uncertainty and prediction error magnitude
    float acetylcholine = std::abs(predictionError_) * confidenceFactor;
    
    // Update Norepinephrine: encodes salience and learning signal
    float norepinephrine = std::abs(rewardPredictionError) * std::min(1.0f, 1.0f / confidenceFactor);
    
    // Store neuromodulator levels
    dopamineLevel_ = std::clamp(dopamine, -1.0f, 1.0f);
    
    // Integrate with prediction error system if available
    if (brain_->getPredictionErrorSignal()) {
        // Connect enhanced prediction error to neuromodulation
        brain_->getPredictionErrorSignal()->computeError(predictedReward, reward);
        brain_->getPredictionErrorSignal()->update(0.001f);
    }
    
    // ========= EXPLORATION AND CURiosity INTEGRATION =========
    
    // Connect prediction error to curiosity system
    float curiosityUpdate = std::abs(predictionError_) * 0.1f + rewardPredictionError * 0.05f;
    curiosityLevel_ = std::clamp(curiosityLevel_ + curiosityUpdate * 0.01f, 0.0f, 1.0f);
    
    // Modulate exploration based on prediction error and neuromodulators
    if (curiosityEnabled_) {
        // Higher prediction error = more exploration
        float explorationBias = 0.5f + 0.5f * std::abs(predictionError_);
        
        // Higher uncertainty (acetylcholine) = more exploration
        float uncertaintyBias = 0.3f + 0.7f * acetylcholine;
        
        // Combine biases for final exploration tendency
        float explorationTendency = explorationBias * uncertaintyBias;
        
        // Adjust action selection thresholds based on exploration tendency
        curiosityThreshold_ = 0.2f + 0.3f * explorationTendency;
    }
    
    // ========= ADAPTIVE PLASTICITY =========
    
    // Compute adaptive learning rate based on prediction error history
    float historicalErrorVariance = 0.0f;
    if (predictionErrorHistory_.size() > 10) {
        float meanError = std::accumulate(predictionErrorHistory_.begin(), 
                                         predictionErrorHistory_.end(), 0.0f) / 
                          predictionErrorHistory_.size();
        
        float varianceSum = 0.0f;
        for (float error : predictionErrorHistory_) {
            varianceSum += (error - meanError) * (error - meanError);
        }
        historicalErrorVariance = varianceSum / predictionErrorHistory_.size();
    }
    
    // Adaptive learning rate: faster for high variance, slower for stable predictions
    float adaptiveLearningRate = 0.01f * std::sqrt(historicalErrorVariance + 0.01f);
    adaptiveLearningRate = std::clamp(adaptiveLearningRate, 0.001f, 0.1f);
    
    // Enhanced neuromodulation level for synaptic plasticity
    float plasticityModulator = 0.5f + 0.5f * dopamineLevel_;
    
    // ========= REWARD-MODULATED SYNAPTIC PLASTICITY =========
    
    // Apply prediction error to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* synapse : region->getSynapses()) {
            float eligibility = synapse->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced reward-modulated weight change
                // Combine prediction error with neuromodulator signals
                float predictionErrorInfluence = enhancedPredictionError * 0.1f;
                float uncertaintyInfluence = acetylcholine * 0.05f;
                float salienceInfluence = norepinephrine * 0.03f;
                
                float combinedModulator = dopamineLevel_ + 
                                         predictionErrorInfluence + 
                                         uncertaintyInfluence + 
                                         salienceInfluence;
                
                combinedModulator *= plasticityModulator;
                
                float delta = eligibility * combinedModulator * adaptiveLearningRate;
                synapse->addToWeight(delta);
                
                // Adaptive eligibility trace decay
                float decayRate = 0.1f + 0.05f * std::abs(predictionError_);
                synapse->decayEligibilityTrace(decayRate);
            }
        }
    }
    
    // ========= DEVELOPMENTAL ADAPTATION =========
    
    // Use prediction error to modulate developmental processes
    if (developmentEnabled_) {
        // High prediction error accelerates development
        float developmentFactor = std::pow(1.0f - std::abs(predictionError_), 2.0f);
        
        // Apply to structural plasticity if enabled
        if (structuralPlasticityEnabled_) {
            auto* structuralPlasticity = brain_->getStructuralPlasticity();
            if (structuralPlasticity) {
                // Modulate synaptogenesis and pruning based on prediction error
                float synaptogenesisRate = 0.0001f * developmentFactor * plasticityModulator;
                float pruningRate = 0.00001f * (2.0f - developmentFactor) * plasticityModulator;
                
                structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
                structuralPlasticity->setPruningRate(pruningRate);
            }
        }
    }
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Modulate STDP based on combined neuromodulators
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Enhanced STDP parameters based on prediction error
        float ltpWeight = 0.01f * plasticityModulator * (1.0f + enhancedPredictionError);
        float ltdWeight = 0.012f * plasticityModulator * (1.0f - enhancedPredictionError * 0.5f);
        
        ltpWeight = std::clamp(ltpWeight, 0.005f, 0.05f);
        ltdWeight = std::clamp(ltdWeight, 0.006f, 0.06f);
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
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

float AgentBrain::getCuriosityThreshold() const {
    return curiosityThreshold_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    curiosityThreshold_ = 0.3f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
