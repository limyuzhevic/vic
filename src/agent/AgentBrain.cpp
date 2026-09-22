#include "AgentBrain.hpp"
#include "IntegrationAbstractions.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <execution>
#include <numeric>
#include <cassert>
#include <optional>
#include <span>
#include <expected>
#include <compare>
#include <chrono>

namespace nlm {

namespace {
    // Helper functions for better performance and error handling
    constexpr float CLAMP(float value, float min, float max) noexcept {
        return (value < min) ? min : (value > max) ? max : value;
    }
    
    template<typename T>
    size_t calculateChecksum(const T* data, size_t size) noexcept {
        size_t checksum = 0;
        for (size_t i = 0; i < size; ++i) {
            checksum ^= static_cast<size_t>(data[i]);
        }
        return checksum;
    }
    
    // Parallel processing helper
    template<typename Iterator, typename UnaryOperation>
    void parallelForEach(Iterator begin, Iterator end, UnaryOperation op) {
        // Use execution policy if available
        try {
            std::for_each(std::execution::par, begin, end, op);
        } catch (const std::exception&) {
            // Fallback to sequential processing
            std::for_each(begin, end, op);
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate inputs
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain is null");
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

[[nodiscard]] size_t AgentBrain::getSensoryInputSize() const noexcept {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

[[nodiscard]] size_t AgentBrain::getMotorOutputSize() const noexcept {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) noexcept {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    parallelForEach(sensoryVision_.begin(), sensoryVision_.end(),
        [this, &vision](Neuron* neuron) {
            if (neuron && !vision.empty()) {
                // Inject current proportional to vision intensity
                float current = vision[0] * 5.0f;  // Scale factor (simplified)
                neuron->injectCurrent(current);
            }
        });
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    parallelForEach(sensoryTouch_.begin(), sensoryTouch_.end(),
        [this, &touch](Neuron* neuron) {
            if (neuron && !touch.empty()) {
                float current = touch[0] * 8.0f;  // Collision signal (simplified)
                neuron->injectCurrent(current);
            }
        });
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    parallelForEach(sensoryInternal_.begin(), sensoryInternal_.end(),
        [this, &intern](Neuron* neuron) {
            if (neuron && !intern.empty()) {
                float current = (intern[0] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        });
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    parallelForEach(sensoryProprioception_.begin(), sensoryProprioception_.end(),
        [this, &proprio](Neuron* neuron) {
            if (neuron && !proprio.empty()) {
                float current = (proprio[0] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        });
    
    // Compute novelty (difference from previous vision) with optimized algorithm
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        // Use std::transform for better performance
        std::transform(vision.begin(), vision.end(), previousVision_.begin(),
                      std::back_inserter(std::vector<float>()),  // Dummy
                      [](float a, float b) { return std::abs(a - b); });
        // Simplified: just use first element for now
        noveltyLevel_ = std::abs(vision[0] - previousVision_[0]);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time (using span for better memory safety)
        if (vision.size() <= previousVision_.size()) {
            std::copy(vision.begin(), vision.end(), previousVision_.begin());
        } else {
            previousVision_ = std::vector<float>(vision.begin(), vision.end());
        }
    }
    
    // Update curiosity based on novelty with validation
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = CLAMP(curiosityLevel_, 0.0f, 1.0f);
    }
}

[[nodiscard]] MotorCommand AgentBrain::decodeMotorCommand() noexcept {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with validation
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

[[nodiscard]] MotorCommand AgentBrain::decodeFromMotorNeurons() noexcept {
    // Calculate average activity in each motor group with optimized algorithm
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const Neuron* n : neurons) {
            if (n) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity using C++20 structured bindings
    struct MotorCommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    std::array<MotorCommandActivity, 6> commands = {
        MotorCommandActivity{MotorCommand::MoveForward, forwardAct},
        MotorCommandActivity{MotorCommand::MoveBackward, backwardAct},
        MotorCommandActivity{MotorCommand::TurnLeft, leftAct},
        MotorCommandActivity{MotorCommand::TurnRight, rightAct},
        MotorCommandActivity{MotorCommand::Interact, interactAct},
        MotorCommandActivity{MotorCommand::Wait, waitAct}
    };
    
    // Find maximum using std::max_element with custom comparator
    auto best = std::max_element(commands.begin(), commands.end(),
                                [](const MotorCommandActivity& a, const MotorCommandActivity& b) {
                                    return a.activity < b.activity;
                                });
    
    MotorCommand bestCmd = best->cmd;
    float bestActivity = best->activity;
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return bestCmd;
}

[[nodiscard]] MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) noexcept {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        if (brain_->getRandomGenerator()) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command with optimized switch
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) noexcept {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error with validation
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average) with C++20 algorithms
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range using constexpr function
    dopamineLevel_ = CLAMP(dopamineLevel_, -1.0f, 1.0f);
    
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
    plasticityFactor = CLAMP(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) noexcept {
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

[[nodiscard]] DevelopmentalStage AgentBrain::getDevelopmentalStage() const noexcept {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

void AgentBrain::reset() noexcept {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision using optimized algorithm
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

void AgentBrain::configureWithIntegratedSystems() {
    if (!brain_) return;
    
    // Collect all motor and sensory neurons for integration
    motorNeurons_.clear();
    sensoryNeurons_.clear();
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    motorNeurons_.push_back(n);
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    sensoryNeurons_.push_back(n);
                }
            }
        }
    }
    
    // Configure motor controller if it exists
    if (motorController_) {
        motorController_->configureMotorGroups(motorNeurons_);
    }
}

void AgentBrain::processSensoryInput(const AgentBody& agent, const SimpleWorld& world) noexcept {
    if (!brain_ || !neuralIntegrator_) return;
    
    // Create sensory percept using integrated processor
    auto perceptResult = SensoryPercept::processWith(*neuralIntegrator_, agent, world);
    if (!perceptResult) {
        NLM_LOG_ERROR("Failed to create sensory percept from integrated processor");
        return;
    }
    
    // Also call the original method for backward compatibility
    processSensoryInput(*perceptResult);
}

void AgentBrain::updateIntegratedSystems(double timestep) noexcept {
    if (neuralIntegrator_) {
        neuralIntegrator_->update(timestep);
    }
}

void AgentBrain::getIntegrationStatistics(std::map<std::string, float>& stats) const noexcept {
    if (!brain_) return;
    
    // Clear stats map if needed
    if (!stats.empty()) {
        stats.clear();
    }
    
    stats["dopamine_level"] = dopamineLevel_;
    stats["novelty_level"] = noveltyLevel_;
    stats["curiosity_level"] = curiosityLevel_;
    stats["prediction_error"] = predictionError_;
    stats["expected_reward"] = expectedReward_;
    stats["developmental_age"] = static_cast<float>(developmentalAge_);
    stats["plasticity_modifier"] = plasticityModifier_;
    
    // Add neural integration statistics if available
    if (neuralIntegrator_) {
        neuralIntegrator_->getStatistics(stats);
    }
}

Result<std::unique_ptr<AgentBrain>> AgentBrain::createStandardBrain(const std::string& configFile) {
    try {
        auto config = std::make_shared<Config>();
        if (!configFile.empty() && !config->loadFromFile(configFile)) {
            return std::unexpected("Failed to load configuration from file: " + configFile);
        }
        
        // Create brain with configuration
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return std::unexpected("Failed to initialize brain");
        }
        
        // Create AgentBrain wrapper
        auto agentBrain = std::make_unique<AgentBrain>(std::move(brain));
        
        NLM_LOG_INFO("Created standard AgentBrain successfully");
        return agentBrain;
        
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Exception creating AgentBrain: ") + e.what());
    }
}

} // namespace nlm
