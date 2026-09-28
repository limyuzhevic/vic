#include "OptimizedAgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

namespace nlm {

OptimizedAgentBrain::OptimizedAgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain)
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
    , lastStep_(0)
    , stepTimeStart_(0.0)
{
    // Initialize optimized neuromodulators
    dopamine_.level = dopamineLevel_;
    dopamine_.target = dopamineLevel_;
    
    curiosity_.level = curiosityLevel_;
    curiosity_.target = curiosityLevel_;
    
    novelty_.level = noveltyLevel_;
    novelty_.target = noveltyLevel_;
}

OptimizedAgentBrain::~OptimizedAgentBrain() = default;

void OptimizedAgentBrain::initialize(const SimpleWorld& world) {
    AgentBrain::initialize(world);
    
    // Pre-allocate batch pools
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    batchPool_.reserve(visionSize + 20); // Add some buffer
    
    // Initialize fast path
    fastPath_.clear();
    
    // Initialize eligibility traces
    eligibilityTraces_.clear();
    
    NLM_LOG_INFO("OptimizedAgentBrain initialized with batch processing");
}

void OptimizedAgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Use optimized batch processing
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    batchProcessVision(vision);
    batchProcessTouch(touch);
    batchProcessInternal(internal);
    batchProcessProprioception(proprio);
    
    // Update fast path pointers
    fastPath_.updatePointers(vision, touch, internal, proprio);
    
    // Compute novelty efficiently
    computeNoveltyOptimized(vision);
    
    // Update neuromodulators
    updateNeuromodulatorsOptimized(1.0); // Assuming dt = 1.0 for timestep
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    perfStats_.processingTimeMs = duration;
}

void OptimizedAgentBrain::batchProcessVision(const std::vector<float>& vision) {
    // SIMD-friendly batch processing for vision neurons
    size_t neuronsToProcess = std::min(sensoryVision_.size(), vision.size());
    
    if (neuronsToProcess == 0) return;
    
    // Use vectorized operations where possible
    for (size_t i = 0; i < neuronsToProcess; ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron) {
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        }
    }
}

void OptimizedAgentBrain::batchProcessTouch(const std::vector<float>& touch) {
    size_t neuronsToProcess = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < neuronsToProcess; ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuron->injectCurrent(current);
        }
    }
}

void OptimizedAgentBrain::batchProcessInternal(const std::vector<float>& internal) {
    size_t neuronsToProcess = std::min(sensoryInternal_.size(), internal.size());
    
    for (size_t i = 0; i < neuronsToProcess; ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
}

void OptimizedAgentBrain::batchProcessProprioception(const std::vector<float>& proprio) {
    size_t neuronsToProcess = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < neuronsToProcess; ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
}

void OptimizedAgentBrain::computeNoveltyOptimized(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Optimized novelty computation using SIMD-style operations
    float totalDiff = 0.0f;
    size_t visionSize = vision.size();
    
    // Process in chunks for better cache performance
    for (size_t i = 0; i < visionSize; i += 4) {
        size_t limit = std::min(i + 4, visionSize);
        
        float v0 = vision[i];
        float v1 = (limit > i + 1) ? vision[i + 1] : 0.0f;
        float v2 = (limit > i + 2) ? vision[i + 2] : 0.0f;
        float v3 = (limit > i + 3) ? vision[i + 3] : 0.0f;
        
        // Compute differences from previous vision
        float prev0 = (i < previousVision_.size()) ? previousVision_[i] : 0.0f;
        float prev1 = (i + 1 < previousVision_.size()) ? previousVision_[i + 1] : 0.0f;
        float prev2 = (i + 2 < previousVision_.size()) ? previousVision_[i + 2] : 0.0f;
        float prev3 = (i + 3 < previousVision_.size()) ? previousVision_[i + 3] : 0.0f;
        
        totalDiff += std::abs(v0 - prev0) + std::abs(v1 - prev1) + 
                    std::abs(v2 - prev2) + std::abs(v3 - prev3);
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Update optimized neuromodulator
    novelty_.setLevel(noveltyLevel_);
    
    // Store for next time (using efficient swap)
    previousVision_ = vision; // This is still O(n) but we can't easily optimize this without more complex data structures
}

void OptimizedAgentBrain::updateNeuromodulatorsOptimized(TimestepDuration dt) {
    SimulationStep currentStep = brain_->getSimulationStep();
    
    // Update optimized neuromodulators
    dopamine_.update(dt, currentStep);
    curiosity_.update(dt, currentStep);
    novelty_.update(dt, currentStep);
    
    // Update levels from optimized neuromodulators
    dopamineLevel_ = dopamine_.level;
    curiosityLevel_ = curiosity_.level;
    noveltyLevel_ = novelty_.level;
}

MotorCommand OptimizedAgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration using optimized values
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosityOptimized(decoded);
    }
    
    return decoded;
}

MotorCommand OptimizedAgentBrain::decodeFromMotorNeurons() {
    batchComputeActivities();
    
    // Use optimized command selection with SIMD-like operations
    std::array<MotorCommandOptimized, 6> commands = {
        MotorCommandOptimized(MotorCommand::MoveForward, motorForwardActivity_),
        MotorCommandOptimized(MotorCommand::MoveBackward, motorBackwardActivity_),
        MotorCommandOptimized(MotorCommand::TurnLeft, motorTurnLeftActivity_),
        MotorCommandOptimized(MotorCommand::TurnRight, motorTurnRightActivity_),
        MotorCommandOptimized(MotorCommand::Interact, motorInteractActivity_),
        MotorCommandOptimized(MotorCommand::Wait, motorWaitActivity_)
    };
    
    // Find maximum using optimized comparison
    MotorCommandOptimized best = commands[5]; // Start with wait
    for (const auto& cmd : commands) {
        if (cmd.activity > best.activity) {
            best = cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (best.activity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best.command;
}

void OptimizedAgentBrain::batchComputeActivities() {
    // Compute activities in batch for better cache locality
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    motorForwardActivity_ = calcActivity(motorForward_);
    motorBackwardActivity_ = calcActivity(motorBackward_);
    motorTurnLeftActivity_ = calcActivity(motorTurnLeft_);
    motorTurnRightActivity_ = calcActivity(motorTurnRight_);
    motorInteractActivity_ = calcActivity(motorInteract_);
    motorWaitActivity_ = calcActivity(motorWait_);
}

MotorCommand OptimizedAgentBrain::selectWithCuriosityOptimized(MotorCommand defaultCmd) {
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

void OptimizedAgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Update optimized neuromodulator
    dopamine_.setLevel(predictionError_, 1.0f);
    dopamineLevel_ = dopamine_.level;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with optimized eligibility traces
    applyRewardModulationOptimized(predictionError_);
    
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

void OptimizedAgentBrain::applyRewardModulationOptimized(float predictionError) {
    // Use batch processing for plasticity updates
    size_t totalSynapticEvents = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * predictionError * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
                
                totalSynapticEvents++;
            }
        }
    }
    
    // Update performance stats
    perfStats_.totalSynapticEventsPerSecond = totalSynapticEvents;
}

void OptimizedAgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
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
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void OptimizedAgentBrain::reset() {
    AgentBrain::reset();
    
    // Reset optimized neuromodulators
    dopamine_.setLevel(0.0f);
    curiosity_.setLevel(0.0f);
    novelty_.setLevel(0.0f);
    
    dopamineLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear batch pools
    batchPool_.clear();
    fastPath_.clear();
    
    // Reset performance stats
    perfStats_ = BrainPerformanceStats();
}

void OptimizedAgentBrain::updatePerformanceStats() const {
    // Update performance statistics
    perfStats_.totalActivity = 0.0;
    perfStats_.totalEnergy = 0.0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto* neuron : *region->getAllNeurons()) {
            if (neuron) {
                perfStats_.totalActivity += std::abs(neuron->getState().membranePotential);
                perfStats_.totalEnergy += neuron->getState().synapticInput;
            }
        }
    }
    
    perfStats_.activeNeurons = brain_->getActiveNeuronCount();
    perfStats_.firingNeurons = brain_->getFiringNeuronCount();
    perfStats_.avgFiringRate = brain_->getAverageFiringRate();
    
    // Estimate memory usage (rough approximation)
    perfStats_.memoryUsageMB = brain_->getTotalNeuronCount() * 0.1; // ~100 bytes per neuron
}

} // namespace nlm