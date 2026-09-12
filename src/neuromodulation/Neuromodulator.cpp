#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Neuromodulator implementation - central coordinator of all neuromodulatory signals

Neuromodulator::Neuromodulator()
    : brain_(nullptr)
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
    , sensoryNoveltyDecay_(0.95f)
{
    // Initialize all motor and sensory neuron groups
    // These would be populated from the brain during initialization
    NLM_LOG_INFO("Neuromodulator system created");
}

Neuromodulator::~Neuromodulator() {
    NLM_LOG_INFO("Neuromodulator system destroyed");
}

float Neuromodulator::getNeuromodulationLevel() const {
    // Overall neuromodulation level affects global brain state
    float totalModulation = dopamineLevel_ + curiosityLevel_ + noveltyLevel_;
    // Clamp to reasonable range
    return std::min(1.0f, totalModulation);
}

void Neuromodulator::update(TimestepDuration dt) {
    // Update all neuromodulatory systems for this timestep
    
    // Dopamine dynamics (reward prediction error, plasticity modulation)
    if (rewardModulationEnabled_) {
        // Decay dopamine towards baseline (energy efficiency)
        float dopamineDecay = 0.05f * static_cast<float>(dt);
        dopamineLevel_ = std::max(0.0f, dopamineLevel_ - dopamineDecay);
        
        // Add noise for biological realism
        float dopamineNoise = 0.01f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        dopamineLevel_ += dopamineNoise;
    }

    // Novelty detection with decay
    if (noveltyLevel_ > 0.0f) {
        float noveltyDecay = 0.1f * static_cast<float>(dt);
        noveltyLevel_ = std::max(0.0f, noveltyLevel_ - noveltyDecay);
    }

    // Curiosity dynamics (exploration drive, modulated by novelty and prediction error)
    if (curiosityEnabled_) {
        float curiosityTarget = noveltyLevel_ * 0.5f + std::abs(predictionError_) * 0.3f;
        // Smooth update towards target with time constant
        float curiosityAdjustment = (curiosityTarget - curiosityLevel_) * 0.1f;
        curiosityLevel_ += curiosityAdjustment * static_cast<float>(dt);
        
        // Clamp to reasonable range
        curiosityLevel_ = std::max(0.0f, std::min(1.0f, curiosityLevel_));
    }

    // Update development-related plasticity modifiers
    if (developmentEnabled_) {
        developmentalAge_ += dt;
        
        // Age-dependent plasticity modulation
        // Younger brains have higher plasticity, older brains are more stable
        if (developmentalAge_ < 10.0f) {
            // Critical period - high plasticity
            plasticityModifier_ = 2.0f;
            curiosityLevel_ *= 1.5f;  // Increased exploration
        } else if (developmentalAge_ < 50.0f) {
            // Rapid learning phase
            plasticityModifier_ = 1.5f;
        } else if (developmentalAge_ < 100.0f) {
            // Maturation phase - moderate plasticity
            plasticityModifier_ = 1.0f;
        } else {
            // Adult phase - stable plasticity
            plasticityModifier_ = 0.8f;
        }
        
        // Development also affects neuromodulation sensitivity
        if (developmentalAge_ > 20.0f) {
            // Reduced sensitivity to novel inputs
            noveltyLevel_ *= 0.9f;
        }
        
        // Development affects attention stability
        auto* attention = brain_->getAttention();
        if (attention) {
            if (developmentalAge_ > 50.0f) {
                // Adult brain - stable attention
                attention->setInhibitionStrength(0.5f);
                attention->setExcitationStrength(1.0f);
            } else {
                // Developing brain - flexible attention
                attention->setInhibitionStrength(0.3f);
                attention->setExcitationStrength(1.5f);
            }
        }
    }
}

void Neuromodulator::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply neuromodulatory effects to brain systems
    
    // 1. Dopamine effects on plasticity
    if (rewardModulationEnabled_ && dopamineLevel_ > 0.0f) {
        // Modulate STDP and Hebbian plasticity based on dopamine level
        float plasticityMod = 1.0f + (dopamineLevel_ - 0.5f) * 0.5f;  // 0.75 to 1.25x
        brain->applyNeuromodulation(*this);
        
        // Dopamine also affects structural plasticity rates
        auto* sp = brain->getStructuralPlasticity();
        if (sp) {
            float currentRate = sp->getSynaptogenesisRate();
            sp->setSynaptogenesisRate(currentRate * plasticityMod);
        }
    }

    // 2. Neuromodulation effects on working memory
    if (curiosityLevel_ > 0.5f) {
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            // Higher curiosity -> more working memory activity
            wm->updateConsolidationRate(curiosityLevel_);
            wm->strengthenMemory(curiosityLevel_);
        }
    }

    // 3. Novelty effects on attention and concept formation
    if (noveltyLevel_ > 0.3f) {
        auto* attention = brain->getAttention();
        if (attention) {
            // Novelty increases attentional scope
            attention->setInhibitionStrength(0.3f);
            attention->setExcitationStrength(2.0f);
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            // Novelty drives concept exploration
            conceptFormation->setFormationThreshold(0.5f);
        }
    }

    // 4. Prediction error effects on learning
    if (std::abs(predictionError_) > 0.1f) {
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            // High prediction error = more learning
            predictionSystem->train(brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
        }
    }

    // 5. Developmental effects on cognitive functions
    if (developmentEnabled_) {
        if (developmentalAge_ > 50.0f && developmentalAge_ < 100.0f) {
            // Critical period - enhanced learning
            auto* development = brain->getDevelopmentSystem();
            if (development) {
                development->update(brain, *brain->getRandomGenerator(), 1.0f);
            }
        }
    }

    // 6. Integration effects - neuromodulators affect each other
    if (dopamineLevel_ > 0.5f && curiosityLevel_ > 0.5f) {
        // High reward and curiosity = enhanced learning and exploration
        auto* planner = brain->getPlanner();
        if (planner) {
            planner->setPlanningDepth(std::min(5, planner->getPlanningDepth() + 1));
        }
    }
}

void Neuromodulator::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize Neuromodulator with null brain");
        return;
    }
    
    brain_ = brain;
    
    // Extract neuron groups from brain for processing
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            for (Neuron* n : pop->getNeurons()) {
                // Categorize neurons for efficient processing
                if (type == NeuronType::Motor) {
                    // Organize motor neurons by action type for faster decoding
                    MotorCommand command = n->getMotorCommand();
                    switch (command) {
                        case MotorCommand::MoveForward:
                            motorForward_.push_back(n);
                            break;
                        case MotorCommand::MoveBackward:
                            motorBackward_.push_back(n);
                            break;
                        case MotorCommand::TurnLeft:
                            motorTurnLeft_.push_back(n);
                            break;
                        case MotorCommand::TurnRight:
                            motorTurnRight_.push_back(n);
                            break;
                        case MotorCommand::Interact:
                            motorInteract_.push_back(n);
                            break;
                        case MotorCommand::Wait:
                            motorWait_.push_back(n);
                            break;
                    }
                } else if (type == NeuronType::Sensory) {
                    // Organize sensory neurons by modality
                    auto modality = n->getSensoryModality();
                    switch (modality) {
                        case SensoryModality::Vision:
                            sensoryVision_.push_back(n);
                            break;
                        case SensoryModality::Touch:
                            sensoryTouch_.push_back(n);
                            break;
                        case SensoryModality::Internal:
                            sensoryInternal_.push_back(n);
                            break;
                        case SensoryModality::Proprioception:
                            sensoryProprioception_.push_back(n);
                            break;
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Neuromodulator initialized with " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
                               motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) +
                 " motor neurons and " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() + sensoryInternal_.size() +
                               sensoryProprioception_.size()) + " sensory neurons");
}

void Neuromodulator::reset() {
    // Reset all neuromodulatory state for new episode
    
    // Reset neuromodulation levels
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development age
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset neuron group references (will be re-populated)
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
    
    previousVision_.clear();
    
    NLM_LOG_INFO("Neuromodulator reset for new episode");
}

void Neuromodulator::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process sensory input for learning and adaptation
    
    // 1. Compute novelty (difference from previous experience)
    const auto& vision = percept.getVision();
    if (!previousVision_.empty() && !vision.empty()) {
        float totalDiff = 0.0f;
        size_t compareLen = std::min(previousVision_.size(), vision.size());
        
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and scale to novelty level
        float avgDiff = totalDiff / compareLen;
        noveltyLevel_ = std::min(1.0f, avgDiff / 0.5f);
    }
    
    // Store current vision for next comparison
    previousVision_ = vision;
    
    // 2. Update prediction error based on sensory input
    auto* predictionSystem = brain_->getPredictionSystem();
    if (predictionSystem && percept.getInternal().size() > 0) {
        // Compute prediction error from internal representation mismatch
        float currentPrediction = predictionSystem->getPredictionError();
        float actualInput = percept.getInternal()[0];
        
        predictionError_ = std::abs(actualInput - currentPrediction);
        
        // High prediction error -> stronger curiosity
        if (predictionError_ > 0.5f) {
            curiosityLevel_ = std::min(1.0f, curiosityLevel_ + 0.1f);
        }
    }
    
    // 3. Inject sensory input into sensory neurons for processing
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Convert visual intensity to neural current
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < percept.getTouch().size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = percept.getTouch()[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // 4. Reward prediction based on sensory state
    if (rewardModulationEnabled_) {
        expectedReward_ = 0.0f;
        // Simple reward prediction based on input patterns
        for (float val : vision) {
            expectedReward_ += val * 0.1f;
        }
    }
}

void Neuromodulator::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_) return;
    
    // Compute reward prediction error
    float error = reward - predictedReward;
    
    // Store expected reward for future predictions
    expectedReward_ = reward;
    
    // Update prediction error signal
    predictionError_ = error;
    
    // Dopamine response:
    // - Positive error (better than expected) -> increase dopamine
    // - Negative error (worse than expected) -> decrease dopamine
    if (error > 0.0f) {
        // Reward prediction error -> dopamine burst
        dopamineLevel_ = std::min(1.0f, dopamineLevel_ + error * 0.5f);
        
        // Strong reward -> increase exploration (curiosity)
        curiosityLevel_ = std::min(1.0f, curiosityLevel_ + error * 0.2f);
    } else if (error < -0.1f) {
        // Prediction error -> decrease dopamine
        dopamineLevel_ = std::max(0.0f, dopamineLevel_ + error * 0.3f);
        
        // Negative prediction error -> reduce exploration
        curiosityLevel_ = std::max(0.0f, curiosityLevel_ + error * 0.1f);
    }
    
    // Limit magnitude of changes
    dopamineLevel_ = std::max(0.0f, std::min(1.0f, dopamineLevel_));
    curiosityLevel_ = std::max(0.0f, std::min(1.0f, curiosityLevel_));
    
    NLM_LOG_INFO("Reward modulation: reward=" + std::to_string(reward) +
                 " predicted=" + std::to_string(predictedReward) +
                 " error=" + std::to_string(error) +
                 " dopamine=" + std::to_string(dopamineLevel_));
}

void Neuromodulator::updateDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    // Update developmental stage and maturation effects
    
    developmentalAge_ += timestep;
    
    // Age-dependent plasticity modulation
    // Younger brains have higher plasticity, older brains are more stable
    if (developmentalAge_ < 10.0f) {
        // Critical period - high plasticity
        plasticityModifier_ = 2.0f;
        curiosityLevel_ *= 1.5f;  // Increased exploration
    } else if (developmentalAge_ < 50.0f) {
        // Rapid learning phase
        plasticityModifier_ = 1.5f;
    } else if (developmentalAge_ < 100.0f) {
        // Maturation phase - moderate plasticity
        plasticityModifier_ = 1.0f;
    } else {
        // Adult phase - stable plasticity
        plasticityModifier_ = 0.8f;
    }
    
    // Development also affects neuromodulation sensitivity
    if (developmentalAge_ > 20.0f) {
        // Reduced sensitivity to novel inputs
        noveltyLevel_ *= 0.9f;
    }
    
    // Development affects attention stability
    auto* attention = brain_->getAttention();
    if (attention) {
        if (developmentalAge_ > 50.0f) {
            // Adult brain - stable attention
            attention->setInhibitionStrength(0.5f);
            attention->setExcitationStrength(1.0f);
        } else {
            // Developing brain - flexible attention
            attention->setInhibitionStrength(0.3f);
            attention->setExcitationStrength(1.5f);
        }
    }
}

MotorCommand Neuromodulator::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Simple motor command decoding based on neuromodulation state
    
    // Choose action based on neuromodulation levels
    MotorCommand selectedCommand = MotorCommand::Wait;
    float highestActivity = 0.0f;
    
    // Check for goal-directed actions (when prediction error is high)
    if (predictionError_ > 0.5f) {
        // High prediction error -> exploration
        if (curiosityLevel_ > 0.7f) {
            // Strong curiosity -> random exploration
            int choice = rand() % 6;
            switch (choice) {
                case 0: selectedCommand = MotorCommand::MoveForward; break;
                case 1: selectedCommand = MotorCommand::MoveBackward; break;
                case 2: selectedCommand = MotorCommand::TurnLeft; break;
                case 3: selectedCommand = MotorCommand::TurnRight; break;
                case 4: selectedCommand = MotorCommand::Interact; break;
                default: selectedCommand = MotorCommand::Wait; break;
            }
        } else {
            // Moderate curiosity -> directed exploration
            selectedCommand = MotorCommand::MoveForward;
        }
    } else {
        // Low prediction error -> exploitation
        if (dopamineLevel_ > 0.7f) {
            // High dopamine -> approach behavior
            selectedCommand = MotorCommand::MoveForward;
        } else if (dopamineLevel_ < 0.3f) {
            // Low dopamine -> withdrawal behavior
            selectedCommand = MotorCommand::Wait;
        } else {
            // Moderate dopamine -> status quo
            selectedCommand = MotorCommand::Wait;
        }
    }
    
    return selectedCommand;
}

DevelopmentalStage Neuromodulator::getDevelopmentalStage() const {
    if (developmentalAge_ < 10.0f) {
        return DevelopmentalStage::Initial;
    } else if (developmentalAge_ < 50.0f) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ < 100.0f) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

} // namespace nlm

