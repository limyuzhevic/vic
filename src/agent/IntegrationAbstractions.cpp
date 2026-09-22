#include "IntegrationAbstractions.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <execution>
#include <cassert>
#include <optional>
#include <span>

namespace nlm {

// Helper functions for better error handling and performance
namespace {
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
}

// ISensoryProcessor implementations
IntegratedSensoryProcessor::IntegratedSensoryProcessor(std::shared_ptr<RandomGenerator> rng)
    : rng_(std::move(rng))
    , visionEnabled_(true)
    , touchEnabled_(true)
    , internalEnabled_(true)
    , proprioceptionEnabled_(true)
    , audioEnabled_(false)
    , initialized_(false)
    , enabled_(true) {
}

void IntegratedSensoryProcessor::initialize() {
    if (initialized_) return;
    
    // Use std::expected for better error handling
    auto regionWeightsResult = configureRegionWeights();
    if (!regionWeightsResult) {
        NLM_LOG_ERROR("Failed to configure region weights: " + regionWeightsResult.error());
        return;
    }
    
    regionWeights_ = std::move(*regionWeightsResult);
    initialized_ = true;
    NLM_LOG_INFO("IntegratedSensoryProcessor initialized");
}

Result<std::map<RegionId, float>> IntegratedSensoryProcessor::configureRegionWeights() {
    std::map<RegionId, float> weights;
    
    // Validate configuration
    if (maxRegionId_ > 10) {
        return std::unexpected("Invalid max region ID");
    }
    
    // Weight distribution: more weight to anterior regions for visual processing
    for (size_t i = 1; i <= maxRegionId_; ++i) {
        RegionId id(i);
        float weight = 1.0f - static_cast<float>(i) / static_cast<float>(maxRegionId_);
        weights[id] = weight;
    }
    
    // Validate weights sum to approximately 1.0
    float totalWeight = 0.0f;
    for (const auto& pair : weights) {
        totalWeight += pair.second;
    }
    
    if (std::abs(totalWeight - 1.0f) > 0.01f) {
        return std::unexpected("Region weights don't sum to 1.0");
    }
    
    return weights;
}

SensoryPercept IntegratedSensoryProcessor::processInput(const AgentBody& agent, const SimpleWorld& world) {
    if (!enabled_) {
        return SensoryPercept();
    }
    
    // Use C++20 std::expected for better error handling
    auto perceptResult = createSensoryPercept(agent, world);
    if (!perceptResult) {
        NLM_LOG_ERROR("Failed to create sensory percept: " + perceptResult.error());
        return SensoryPercept();
    }
    
    return std::move(*perceptResult);
}

Result<SensoryPercept> IntegratedSensoryProcessor::createSensoryPercept(const AgentBody& agent, 
                                                                        const SimpleWorld& world) {
    SensoryPercept percept;
    
    // Vision processing with std::span for zero-copy views where possible
    if (visionEnabled_) {
        auto visionResult = processVision(agent, world);
        if (!visionResult) {
            return std::unexpected("Vision processing failed");
        }
        
        const auto& visionData = *visionResult;
        percept.setVision(std::span<const float>(visionData));
        percept.setVisionWidth(VISION_WIDTH);
        percept.setVisionHeight(VISION_HEIGHT);
    }
    
    // Touch processing
    if (touchEnabled_) {
        auto touchResult = processTouch(agent, world);
        if (!touchResult) {
            return std::unexpected("Touch processing failed");
        }
        
        percept.setTouch(std::span<const float>(*touchResult));
    }
    
    // Internal state processing
    if (internalEnabled_) {
        auto internalResult = processInternalState(agent);
        if (!internalResult) {
            return std::unexpected("Internal state processing failed");
        }
        
        percept.setInternal(std::span<const float>(*internalResult));
    }
    
    // Proprioception processing
    if (proprioceptionEnabled_) {
        auto proprioResult = processProprioception(agent);
        if (!proprioResult) {
            return std::unexpected("Proprioception processing failed");
        }
        
        percept.setProprioception(std::span<const float>(*proprioResult));
    }
    
    // Audio processing (placeholder)
    if (audioEnabled_) {
        // Would integrate with actual audio input here
        percept.setAudio(std::span<const float>());
    }
    
    // Set timestamp with chrono for better time handling
    auto timestamp = std::chrono::system_clock::now();
    percept.setTimestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()));
    
    return percept;
}

Result<std::vector<float>> IntegratedSensoryProcessor::processVision(const AgentBody& agent, 
                                                                      const SimpleWorld& world) {
    std::vector<float> visionData;
    visionData.reserve(VISION_WIDTH * VISION_HEIGHT);
    
    size_t visionWidth = std::min(VISION_WIDTH, world.getVisionWidth());
    size_t visionHeight = std::min(VISION_HEIGHT, world.getVisionHeight());
    
    // Sample world vision with parallel processing where beneficial
    for (size_t y = 0; y < visionHeight; ++y) {
        for (size_t x = 0; x < visionWidth; ++x) {
            // Sample from agent's position with noise
            float noise = (rng_) ? rng_->uniformReal(-0.1f, 0.1f) : 0.0f;
            float sample = agent.x * 0.01f + agent.y * 0.01f + noise;
            sample = CLAMP(sample, 0.0f, 1.0f);
            visionData.push_back(sample);
        }
    }
    
    // Pad to expected size if necessary using std::span operations
    if (visionData.size() < VISION_WIDTH * VISION_HEIGHT) {
        visionData.resize(VISION_WIDTH * VISION_HEIGHT, 0.0f);
    }
    
    return visionData;
}

Result<std::vector<float>> IntegratedSensoryProcessor::processTouch(const AgentBody& agent, 
                                                                     const SimpleWorld& world) {
    std::vector<float> touchData(TOUCH_SIZE, 0.0f);
    
    // Check for nearby objects (simplified collision detection)
    for (size_t i = 0; i < TOUCH_SIZE && i < world.getObjectCount(); ++i) {
        const auto& obj = world.getObject(i);
        float distance = std::sqrt(std::pow(agent.x - obj.x, 2) + 
                                  std::pow(agent.y - obj.y, 2));
        
        if (distance < 1.0f) {  // Touch threshold
            touchData[i] = std::max(0.0f, 1.0f - distance);
        }
    }
    
    return touchData;
}

Result<std::vector<float>> IntegratedSensoryProcessor::processInternalState(const AgentBody& agent) {
    std::vector<float> internalData(INTERNAL_SIZE, 0.0f);
    
    // Include agent energy, health, and activity level
    internalData[0] = agent.energy;  // Energy level
    internalData[1] = agent.health;  // Health level
    internalData[2] = agent.isMoving ? 1.0f : 0.0f;  // Movement state
    internalData[3] = agent.age > 100.0f ? 1.0f : 0.0f;  // Aging indicator
    
    return internalData;
}

Result<std::vector<float>> IntegratedSensoryProcessor::processProprioception(const AgentBody& agent) {
    std::vector<float> proprioData(PROPRIOCEPTION_SIZE, 0.0f);
    
    // Position and velocity derivatives
    proprioData[0] = agent.x;  // X position
    proprioData[1] = agent.y;  // Y position
    proprioData[2] = agent.orientation;  // Orientation
    proprioData[3] = agent.velocityX;  // X velocity
    proprioData[4] = agent.velocityY;  // Y velocity
    proprioData[5] = agent.angularVelocity;  // Angular velocity
    
    return proprioData;
}

size_t IntegratedSensoryProcessor::getInputSize() const {
    size_t total = 0;
    if (visionEnabled_) total += VISION_WIDTH * VISION_HEIGHT;
    if (touchEnabled_) total += TOUCH_SIZE;
    if (internalEnabled_) total += INTERNAL_SIZE;
    if (proprioceptionEnabled_) total += PROPRIOCEPTION_SIZE;
    if (audioEnabled_) total += AUDIO_SIZE;
    return total;
}

bool IntegratedSensoryProcessor::isEnabled() const {
    return enabled_ && (visionEnabled_ || touchEnabled_ || 
                       internalEnabled_ || proprioceptionEnabled_ || audioEnabled_);
}

void IntegratedSensoryProcessor::enable(bool enabled) {
    enabled_ = enabled;
}

const std::map<RegionId, float>& IntegratedSensoryProcessor::getRegionWeights() const {
    return regionWeights_;
}

// IntegratedMotorController implementations
IntegratedMotorController::IntegratedMotorController(std::shared_ptr<RandomGenerator> rng)
    : rng_(std::move(rng))
    , dopamineLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , enabled_(true)
    , curiosityEnabled_(true)
    , rewardModulationEnabled_(true)
    , sensoryNoveltyDecay_(0.99f) {
}

void IntegratedMotorController::initialize(const Brain& brain) {
    NLM_LOG_INFO("IntegratedMotorController initialized");
}

MotorCommand IntegratedMotorController::decodeCommand(const std::vector<float>& neuralActivity) {
    if (!enabled_) {
        return MotorCommand::Wait;
    }
    
    // Apply neuromodulation with modern C++20 features
    applyNeuromodulation(neuralActivity);
    
    // Simple motor decoding based on neural activity patterns
    // In a real implementation, this would integrate with actual motor neuron groups
    
    // Default to wait if no activity
    MotorCommand command = MotorCommand::Wait;
    float maxActivity = 0.0f;
    
    // This would be replaced with actual motor neuron group activity analysis
    // For now, return a simple default based on activity levels
    if (!neuralActivity.empty()) {
        float avgActivity = std::accumulate(neuralActivity.begin(), 
                                          neuralActivity.end(), 0.0f) / neuralActivity.size();
        if (avgActivity > 0.5f) {
            command = MotorCommand::MoveForward;
        }
    }
    
    return command;
}

size_t IntegratedMotorController::getOutputSize() const {
    return 7;  // Number of motor commands
}

void IntegratedMotorController::applyNeuromodulation(MotorCommand& command, float neuromodulationLevel) {
    if (!rewardModulationEnabled_) return;
    
    // Apply dopamine-based modulation
    if (std::abs(neuromodulationLevel) > 0.3f) {
        // High dopamine = more exploration, lower inhibition
        if (curiosityEnabled_ && (rng_) && (rng_->uniformReal(0.0f, 1.0f) < 0.3f)) {
            // Occasionally choose a different command for exploration
            int choice = (rng_) ? rng_->uniformInt(0, 6) : 0;
            switch (choice) {
                case 0: command = MotorCommand::MoveForward; break;
                case 1: command = MotorCommand::MoveBackward; break;
                case 2: command = MotorCommand::TurnLeft; break;
                case 3: command = MotorCommand::TurnRight; break;
                case 4: command = MotorCommand::LookLeft; break;
                case 5: command = MotorCommand::LookRight; break;
                case 6: command = MotorCommand::Interact; break;
            }
        }
    }
}

bool IntegratedMotorController::isEnabled() const {
    return enabled_;
}

void IntegratedMotorController::enable(bool enabled) {
    enabled_ = enabled;
}

void IntegratedMotorController::configureMotorGroups(const std::vector<Neuron*>& motorNeurons) {
    // Reset groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Distribute neurons across groups
    size_t idx = 0;
    for (Neuron* neuron : motorNeurons) {
        switch (idx % 6) {
            case 0: motorForward_.push_back(neuron); break;
            case 1: motorBackward_.push_back(neuron); break;
            case 2: motorTurnLeft_.push_back(neuron); break;
            case 3: motorTurnRight_.push_back(neuron); break;
            case 4: motorInteract_.push_back(neuron); break;
            case 5: motorWait_.push_back(neuron); break;
        }
        idx++;
    }
}

IntegratedMotorController::MotorStats IntegratedMotorController::getMotorStats() const {
    MotorStats stats;
    
    // This would be populated with actual motor group activity analysis
    // For now, return empty stats
    return stats;
}

// IntegratedNeuralIntegrator implementations
IntegratedNeuralIntegrator::IntegratedNeuralIntegrator(std::shared_ptr<RandomGenerator> rng)
    : rng_(std::move(rng))
    , developmentalAge_(0.0)
    , currentStage_(DevelopmentalStage::Initial)
    , plasticityModifier_(1.0f)
    , workingMemory_(nullptr)
    , episodicMemory_(nullptr)
    , associativeMemory_(nullptr)
    , dopamine_(nullptr)
    , curiosity_(nullptr)
    , novelty_(nullptr)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , lastUpdateTime_(0.0)
    , stepCount_(0) {
}

void IntegratedNeuralIntegrator::initialize(const Brain& brain) {
    // Initialize from brain systems with validation
    workingMemory_ = brain.getWorkingMemory();
    episodicMemory_ = brain.getEpisodicMemory();
    associativeMemory_ = brain.getAssociativeMemory();
    dopamine_ = brain.getDopamine();
    curiosity_ = brain.getCuriosity();
    novelty_ = brain.getNovelty();
    
    // Validate that required systems are available
    if (!workingMemory_ || !episodicMemory_ || !dopamine_) {
        NLM_LOG_WARNING("IntegratedNeuralIntegrator initialized with missing systems");
    }
    
    NLM_LOG_INFO("IntegratedNeuralIntegrator initialized");
}

void IntegratedNeuralIntegrator::update(double timestep) {
    if (timestep <= 0.0) return;
    
    developmentalAge_ += timestep;
    stepCount_++;
    
    // Update developmental stage
    advanceDevelopmentalStage();
    
    // Update learning variables with validation
    if (novelty_) {
        novelty_->update(timestep);
    }
    
    if (curiosity_) {
        curiosity_->update(timestep);
    }
    
    if (dopamine_) {
        dopamine_->update(timestep);
        predictionError_ = dopamine_->getLevel();
    }
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * predictionError_;
    
    lastUpdateTime_ += timestep;
}

void IntegratedNeuralIntegrator::getStatistics(std::map<std::string, float>& stats) const {
    // Validate stats map
    if (!stats.empty()) {
        stats.clear();
    }
    
    stats["developmental_age"] = static_cast<float>(developmentalAge_);
    stats["plasticity_modifier"] = plasticityModifier_;
    stats["prediction_error"] = predictionError_;
    stats["expected_reward"] = expectedReward_;
    stats["step_count"] = static_cast<float>(stepCount_);
    
    // Add optional statistics with validation
    if (novelty_) {
        stats["novelty_level"] = novelty_->getLevel();
    }
    
    if (curiosity_) {
        stats["curiosity_level"] = curiosity_->getLevel();
    }
    
    if (dopamine_) {
        stats["dopamine_level"] = dopamine_->getLevel();
    }
    
    if (workingMemory_) {
        stats["working_memory_active"] = workingMemory_->getActiveTraces();
    }
    
    if (episodicMemory_) {
        stats["episodic_memory_episodes"] = episodicMemory_->getEpisodeCount();
    }
}

bool IntegratedNeuralIntegrator::hasReachedDevelopmentalStage(DevelopmentalStage stage) const {
    return currentStage_ >= stage;
}

DevelopmentalStage IntegratedNeuralIntegrator::getCurrentStage() const {
    return currentStage_;
}

void IntegratedNeuralIntegrator::advanceDevelopmentalStage() {
    DevelopmentalStage newStage = currentStage_;
    
    // Validate developmental age
    if (developmentalAge_ < 0.0) {
        developmentalAge_ = 0.0;
    }
    
    if (developmentalAge_ < 60.0) {  // ~1 minute
        newStage = DevelopmentalStage::Initial;
        plasticityModifier_ = 1.0f;
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        newStage = DevelopmentalStage::CriticalPeriod;
        plasticityModifier_ = 0.8f;
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        newStage = DevelopmentalStage::Maturation;
        plasticityModifier_ = 0.5f;
    } else {
        newStage = DevelopmentalStage::Adult;
        plasticityModifier_ = 0.2f;
    }
    
    currentStage_ = newStage;
}

} // namespace nlm
