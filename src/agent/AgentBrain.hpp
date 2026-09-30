#pragma once

// ============================================================================
// Enhanced AgentBrain with Comprehensive Documentation, Debugging Aids, and Validation
// ============================================================================

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <optional>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <cassert>
#include <unordered_set>
#include <functional>

namespace nlm {

// ============================================================================
// AgentBrain Operation Exception Handling
// ============================================================================

// AgentBrain operation exception hierarchy
class AgentBrainException : public std::runtime_error {
public:
    explicit AgentBrainException(const std::string& message) 
        : std::runtime_error("AgentBrain error: " + message) {}
};

class AgentBrainInitializationException : public AgentBrainException {
public:
    explicit AgentBrainInitializationException(const std::string& message)
        : AgentBrainException("AgentBrain initialization error: " + message) {}
};

class AgentBrainProcessingException : public AgentBrainException {
public:
    explicit AgentBrainProcessingException(const std::string& message)
        : AgentBrainException("AgentBrain processing error: " + message) {}
};

// ============================================================================
// Enhanced AgentBrain with Comprehensive Features
// ============================================================================

class AgentBrain {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<const SimpleWorld> world_;
    
    // State management
    std::vector<std::vector<float>> previousVision_;
    double developmentalAge_;
    float plasticityModifier_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float serotoninLevel_;
    float norepinephrineLevel_;
    float acetylcholineLevel_;
    float gabaLevel_;
    float glutamateLevel_;
    float endorphinsLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Feature flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Processing configuration
    float sensoryNoveltyDecay_;
    
    // Debug and validation state
    std::vector<std::string> validationErrors_;
    mutable std::mutex validationMutex_;
    std::unordered_map<std::string, double> performanceMetrics_;
    mutable std::mutex metricsMutex_;
    
    // Processing statistics
    size_t totalSensoryProcessed_;
    size_t totalMotorDecoded_;
    std::chrono::system_clock::time_point lastUpdateTime_;
    
    // Sensory neuron groups (distributed architecture)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Motor neuron groups (action distribution)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
public:
    // ============================================================================
    // Enhanced Constructor
    // ============================================================================
    AgentBrain(std::shared_ptr<Brain> brain)
        : brain_(brain)
        , developmentalAge_(0.0)
        , plasticityModifier_(1.0f)
        , dopamineLevel_(0.0f)
        , noveltyLevel_(0.0f)
        , curiosityLevel_(0.0f)
        , predictionError_(0.0f)
        , expectedReward_(0.0f)
        , rewardModulationEnabled_(true)
        , structuralPlasticityEnabled_(true)
        , developmentEnabled_(true)
        , curiosityEnabled_(true)
        , sensoryNoveltyDecay_(0.99f)
        , totalSensoryProcessed_(0)
        , totalMotorDecoded_(0)
        , lastUpdateTime_(std::chrono::system_clock::now()) {
        
        initializeNeuronGroups();
        validateInitialization();
    }
    
    ~AgentBrain() {
        cleanupResources();
    }
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
    // ============================================================================
    // Enhanced Initialization with Validation
    // ============================================================================
    void initialize(const SimpleWorld& world) {
        world_ = std::make_shared<SimpleWorld>(world);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        
        // Re-initialize neuron groups for new world
        initializeNeuronGroups();
        
        // Validate world configuration
        if (!validateWorldConfiguration()) {
            throw AgentBrainInitializationException("Invalid world configuration");
        }
        
        // Clear validation errors
        std::lock_guard<std::mutex> lock(validationMutex_);
        validationErrors_.clear();
        
        // Reset statistics
        totalSensoryProcessed_ = 0;
        totalMotorDecoded_ = 0;
        lastUpdateTime_ = std::chrono::system_clock::now();
        
        NLM_LOG_INFO("AgentBrain initialized successfully for " + 
                    std::to_string(world.getVisionWidth()) + "x" + 
                    std::to_string(world.getVisionHeight()) + " world");
    }
    
    // ============================================================================
    // Enhanced Sensory Processing with Neuromodulation
    // ============================================================================
    void processSensoryInput(const SensoryPercept& percept) {
        try {
            std::lock_guard<std::mutex> lock(validationMutex_);
            validationErrors_.clear();
            
            const auto& vision = percept.getVision();
            const auto& touch = percept.getTouch();
            const auto& intern = percept.getInternal();
            const auto& proprio = percept.getProprioception();
            
            // Process vision with enhanced neuromodulation
            processVisionWithNeuromodulation(vision);
            
            // Process touch with prediction error modulation
            processTouchWithPredictionError(touch);
            
            // Process internal signals with curiosity enhancement
            processInternalWithCuriosity(intern);
            
            // Process proprioception with reward modulation
            processProprioceptionWithReward(proprio);
            
            // Update state
            updateNoveltyAndCuriosity(vision);
            updateDevelopmentAndPlasticity();
            
            // Track metrics
            totalSensoryProcessed_++;
            lastUpdateTime_ = std::chrono::system_clock::now();
            
            NLM_LOG_DEBUG("Processed sensory input: vision=" + std::to_string(vision.size()) + 
                         ", touch=" + std::to_string(touch.size()) + 
                         ", internal=" + std::to_string(intern.size()) + 
                         ", proprio=" + std::to_string(proprio.size()));
            
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(validationMutex_);
            validationErrors_.push_back(std::string("Sensory processing error: ") + e.what());
            NLM_LOG_ERROR("Sensory processing failed: " + std::string(e.what()));
            throw;
        }
    }
    
    // ============================================================================
    // Enhanced Motor Decoding with Developmental Bias
    // ============================================================================
    MotorCommand decodeMotorCommand() {
        try {
            // Calculate average activity in each motor group
            auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
                if (neurons.empty()) return 0.0f;
                float sum = 0.0f;
                for (Neuron* n : neurons) {
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
            
            // Apply developmental modulation
            applyDevelopmentalModulation(forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct);
            
            // Apply curiosity-based exploration
            applyCuriosityExploration(forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct);
            
            // Apply neuromodulation-based motor modulation
            applyNeuromodulationToMotor(forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct);
            
            // Find maximum activity
            struct { MotorCommand cmd; float activity; } commands[] = {
                {MotorCommand::MoveForward, forwardAct},
                {MotorCommand::MoveBackward, backwardAct},
                {MotorCommand::TurnLeft, leftAct},
                {MotorCommand::TurnRight, rightAct},
                {MotorCommand::Interact, interactAct},
                {MotorCommand::Wait, waitAct}
            };
            
            MotorCommand decoded = MotorCommand::Wait;
            float bestActivity = waitAct;
            
            for (const auto& c : commands) {
                if (c.activity > bestActivity) {
                    bestActivity = c.activity;
                    decoded = c.cmd;
                }
            }
            
            // Only act if there's meaningful activity
            if (bestActivity < 0.5f) {
                decoded = MotorCommand::Wait;
            }
            
            // Track metrics
            totalMotorDecoded_++;
            
            NLM_LOG_DEBUG("Motor command decoded: " + motorCommandToString(decoded) + 
                         " (activity: " + std::to_string(bestActivity) + ")");
            
            return decoded;
            
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(validationMutex_);
            validationErrors_.push_back(std::string("Motor decoding error: ") + e.what());
            NLM_LOG_ERROR("Motor decoding failed: " + std::string(e.what()));
            return MotorCommand::Wait; // Safe fallback
        }
    }
    
    // ============================================================================
    // Enhanced Reward Modulation with Multiple Factors
    // ============================================================================
    void applyRewardModulation(float reward, float predictedReward) {
        if (!brain_ || !rewardModulationEnabled_) return;
        
        try {
            // Compute prediction error
            predictionError_ = reward - predictedReward;
            
            // Update expected reward (exponential moving average with decay)
            expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
            
            // ===== ENHANCED DOPAMINE SIGNALLING =====
            
            // Dopamine-like signal based on prediction error
            dopamineLevel_ = predictionError_;
            
            // Decay prediction error over time
            predictionError_ *= 0.9f;
            
            // ===== INTEGRATED PLASTICITY MODULATION =====
            
            // Apply to all synapses with enhanced eligibility traces
            if (brain_) {
                for (const auto& region : brain_->getRegions()) {
                    for (auto* syn : region->getSynapses()) {
                        float eligibility = syn->getEligibilityTrace();
                        
                        if (std::abs(eligibility) > 0.001f) {
                            // Enhanced reward-modulated weight change
                            // Integrate neuromodulation factors
                            float neuromodulation = 1.0f;
                            
                            if (rewardModulationEnabled_) {
                                neuromodulation += dopamineLevel_ * 0.5f;
                            }
                            if (structuralPlasticityEnabled_) {
                                neuromodulation *= (1.0f + developmentalAge_ * 0.001f);
                            }
                            
                            float delta = eligibility * dopamineLevel_ * plasticityModifier_ * neuromodulation;
                            syn->addToWeight(delta);
                            
                            // Enhanced eligibility trace decay
                            float decayRate = 0.1f + curiosityLevel_ * 0.1f; // Faster decay with high curiosity
                            syn->decayEligibilityTrace(decayRate);
                        }
                    }
                }
            }
            
            // ===== NEUROMODULATION FEEDBACK TO AGENT STATE =====
            
            // Update neuromodulation state
            dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
            
            // Update development based on reward history
            float rewardHistory = std::abs(expectedReward_);
            if (developmentEnabled_ && developmentalAge_ < 600.0) {
                // Reward accelerates development
                developmentalAge_ += reward * 0.5f;
                
                // Update developmental stage
                if (developmentalAge_ < 60.0) {  // ~1 minute
                    setDevelopmentalStage(DevelopmentalStage::Initial);
                } else if (developmentalAge_ < 300.0) {  // ~5 minutes
                    setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
                } else if (developmentalAge_ < 900.0) {  // ~15 minutes
                    setDevelopmentalStage(DevelopmentalStage::Maturation);
                } else {
                    setDevelopmentalStage(DevelopmentalStage::Adult);
                }
            }
            
            NLM_LOG_INFO(std::string("Reward modulation: reward=") + std::to_string(reward) + 
                        ", predicted=") + std::to_string(predictedReward) + 
                        ", error=") + std::to_string(predictionError_) + 
                        ", dopamine=") + std::to_string(dopamineLevel_);
            
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(validationMutex_);
            validationErrors_.push_back(std::string("Reward modulation error: ") + e.what());
            NLM_LOG_ERROR("Reward modulation failed: " + std::string(e.what()));
        }
    }
    
    // ============================================================================
    // Development and Plasticity Management
    // ============================================================================
    void updateDevelopment(double timestep) {
        if (!brain_ || !developmentEnabled_) return;
        
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        // This is a biologically inspired approximation
        if (developmentalAge_ < 60.0) {  // ~1 minute
            plasticityModifier_ = 1.0f;  // High plasticity
            if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < 300.0) {  // ~5 minutes
            plasticityModifier_ = 0.8f;
            if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < 900.0) {  // ~15 minutes
            plasticityModifier_ = 0.5f;
            if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            plasticityModifier_ = 0.2f;  // Adult - more stable
            if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled_ && brain_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                // Higher synaptogenesis in early development
                float synRate = 0.0001f * plasticityModifier_;
                float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
        
        NLM_LOG_DEBUG(std::string("Development updated: age=") + std::to_string(developmentalAge_) + 
                     ", plasticity=") + std::to_string(plasticityModifier_));
    }
    
    // ============================================================================
    // Configuration and State Management
    // ============================================================================
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    
    // Get curiosity level
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Get novelty level
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Get prediction error
    float getPredictionError() const { return predictionError_; }
    
    // Get average reward (expected reward)
    float getAverageReward() const { return expectedReward_; }
    
    // Get development time (age)
    double getDevelopmentTime() const { return developmentalAge_; }
    
    // Get learning rate (plasticity modifier)
    float getLearningRate() const { return plasticityModifier_; }
    
    // ============================================================================
    // Enhanced Configuration Methods
    // ============================================================================
    
    void enableAdvancedPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopmentStages(bool enable) { developmentEnabled_ = enable; }
    void enableNeuromodulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    bool isAdvancedPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentStagesEnabled() const { return developmentEnabled_; }
    bool isNeuromodulationEnabled() const { return rewardModulationEnabled_; }
    
    // Phase 6 Advanced Configuration
    
    // Set neuromodulation parameters
    void setDopamineLevel(float level) { dopamineLevel_ = level; }
    void setNoveltyLevel(float level) { noveltyLevel_ = level; }
    void setCuriosityLevel(float level) { curiosityLevel_ = level; }
    
    // Configure developmental parameters
    void setDevelopmentalAge(double age) { developmentalAge_ = age; }
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    
    // Configure processing parameters
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    void setPredictionError(float error) { predictionError_ = error; }
    
    // ============================================================================
    // Validation and Debugging
    // ============================================================================
    
    // Validate AgentBrain state
    bool validateState() const {
        std::lock_guard<std::mutex> lock(validationMutex_);
        
        bool valid = true;
        
        // Check brain exists
        if (!brain_) {
            validationErrors_.push_back("Brain is null");
            valid = false;
        }
        
        // Check sensory neuron groups
        if (sensoryVision_.empty() || sensoryTouch_.empty() || 
            sensoryInternal_.empty() || sensoryProprioception_.empty()) {
            validationErrors_.push_back("Incomplete sensory neuron groups");
            valid = false;
        }
        
        // Check motor neuron groups
        if (motorForward_.empty() || motorBackward_.empty() || 
            motorTurnLeft_.empty() || motorTurnRight_.empty() || 
            motorInteract_.empty() || motorWait_.empty()) {
            validationErrors_.push_back("Incomplete motor neuron groups");
            valid = false;
        }
        
        // Check configuration ranges
        if (dopamineLevel_ < -1.0f || dopamineLevel_ > 1.0f) {
            validationErrors_.push_back(std::string("Invalid dopamine level: ") + std::to_string(dopamineLevel_));
            valid = false;
        }
        
        if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
            validationErrors_.push_back(std::string("Invalid curiosity level: ") + std::to_string(curiosityLevel_));
            valid = false;
        }
        
        if (noveltyLevel_ < 0.0f || noveltyLevel_ > 1.0f) {
            validationErrors_.push_back(std::string("Invalid novelty level: ") + std::to_string(noveltyLevel_));
            valid = false;
        }
        
        if (developmentalAge_ < 0.0) {
            validationErrors_.push_back(std::string("Invalid developmental age: ") + std::to_string(developmentalAge_));
            valid = false;
        }
        
        if (plasticityModifier_ < 0.0f || plasticityModifier_ > 1.0f) {
            validationErrors_.push_back(std::string("Invalid plasticity modifier: ") + std::to_string(plasticityModifier_));
            valid = false;
        }
        
        return valid;
    }
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const {
        std::lock_guard<std::mutex> lock(validationMutex_);
        return validationErrors_;
    }
    
    // Get status report
    std::string getStatusReport() const {
        std::lock_guard<std::mutex> lock(validationMutex_);
        
        std::ostringstream oss;
        oss << "=== AgentBrain Status Report ===\n\n";
        oss << "Brain state: " << (brain_ ? "Initialized" : "Null") << "\n";
        oss << "World state: " << (world_ ? "Active" : "Not set") << "\n";
        oss << "Sensory neurons: " << (sensoryVision_.size() + sensoryTouch_.size() + 
                                   sensoryInternal_.size() + sensoryProprioception_.size()) << "\n";
        oss << "Motor neurons: " << (motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() + 
                                    motorInteract_.size() + motorWait_.size()) << "\n";
        oss << "Neuromodulation: dopamine=" << dopamineLevel_ << 
                   ", curiosity=" << curiosityLevel_ << 
                   ", novelty=" << noveltyLevel_ << "\n";
        oss << "Development: age=" << developmentalAge_ << ", "
                   "plasticity=" << plasticityModifier_ << "\n";
        oss << "Features: reward_modulation=" << (rewardModulationEnabled_ ? "on" : "off") << 
                   ", structural_plasticity=" << (structuralPlasticityEnabled_ ? "on" : "off") << 
                   ", development=" << (developmentEnabled_ ? "on" : "off") << 
                   ", curiosity=" << (curiosityEnabled_ ? "on" : "off") << "\n";
        oss << "Processing stats: sensory=" << totalSensoryProcessed_ << 
                   ", motor=" << totalMotorDecoded_ << "\n";
        oss << "Validation errors: " << validationErrors_.size() << "\n";
        
        if (!validationErrors_.empty()) {
            oss << "\nValidation errors:\n";
            for (const auto& error : validationErrors_) {
                oss << "  - " << error << "\n";
            }
        }
        
        return oss.str();
    }
    
    // ============================================================================
    // Performance Monitoring
    // ============================================================================
    
    void recordPerformanceMetric(const std::string& name, double value) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        performanceMetrics_[name] = value;
    }
    
    std::unordered_map<std::string, double> getPerformanceMetrics() const {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        return performanceMetrics_;
    }
    
    std::string getPerformanceReport() const {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        std::ostringstream oss;
        oss << "=== AgentBrain Performance Report ===\n\n";
        oss << "Last update: " << 
            std::chrono::duration<double>(std::chrono::system_clock::now() - lastUpdateTime_).count() << 
            " seconds ago\n";
        oss << "Total sensory processing: " << totalSensoryProcessed_ << "\n";
        oss << "Total motor decoding: " << totalMotorDecoded_ << "\n";
        
        if (!performanceMetrics_.empty()) {
            oss << "\nPerformance metrics:\n";
            for (const auto& metric : performanceMetrics_) {
                oss << "  " << metric.first << ": " << metric.second << "\n";
            }
        }
        
        return oss.str();
    }
    
    // ============================================================================
    // Reset and Cleanup
    // ============================================================================
    
    void reset() {
        // Reset neuromodulation
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        
        // Reset development
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Reset features
        rewardModulationEnabled_ = true;
        structuralPlasticityEnabled_ = true;
        developmentEnabled_ = true;
        curiosityEnabled_ = true;
        
        // Clear validation errors
        std::lock_guard<std::mutex> lock1(validationMutex_);
        validationErrors_.clear();
        
        // Reset metrics
        std::lock_guard<std::mutex> lock2(metricsMutex_);
        performanceMetrics_.clear();
        
        // Reset statistics
        totalSensoryProcessed_ = 0;
        totalMotorDecoded_ = 0;
        
        // Reinitialize neuron groups
        initializeNeuronGroups();
        
        lastUpdateTime_ = std::chrono::system_clock::now();
        
        NLM_LOG_INFO("AgentBrain reset completed");
    }
    
    // ============================================================================
    // Legacy Compatibility Methods
    // ============================================================================
    
    // Get brain pointer (legacy)
    Brain* getBrain() { return brain_.get(); }
    
    // Get sensory input size expected by brain (legacy)
    size_t getSensoryInputSize() const {
        // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
        return 256 + 8 + 4 + 6;
    }
    
    // Get motor output size expected (legacy)
    size_t getMotorOutputSize() const {
        // One motor neuron per action
        return 6;
    }
    
    // Deprecated methods (duplicate signatures for compatibility)
    void reset() const {
        // This is a const version for compatibility - call non-const version
        const_cast<AgentBrain*>(this)->reset();
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Initial;
    }
    
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
    // ============================================================================
    // Private Helper Methods
    // ============================================================================
    
    void initializeNeuronGroups() {
        // Clear existing groups
        sensoryVision_.clear();
        sensoryTouch_.clear();
        sensoryInternal_.clear();
        sensoryProprioception_.clear();
        
        motorForward_.clear();
        motorBackward_.clear();
        motorTurnLeft_.clear();
        motorTurnRight_.clear();
        motorInteract_.clear();
        motorWait_.clear();
        
        // Initialize if brain exists
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
    
    bool validateInitialization() {
        std::lock_guard<std::mutex> lock(validationMutex_);
        validationErrors_.clear();
        
        bool valid = true;
        
        if (!brain_) {
            validationErrors_.push_back("Brain is null");
            valid = false;
        }
        
        if (sensoryVision_.empty()) {
            validationErrors_.push_back("No vision sensory neurons");
            valid = false;
        }
        
        if (motorForward_.empty()) {
            validationErrors_.push_back("No forward motor neurons");
            valid = false;
        }
        
        return valid;
    }
    
    bool validateWorldConfiguration() const {
        return world_ && !world_->getVisionWidth().empty() && 
               !world_->getVisionHeight().empty();
    }
    
    void cleanupResources() {
        sensoryVision_.clear();
        sensoryTouch_.clear();
        sensoryInternal_.clear();
        sensoryProprioception_.clear();
        
        motorForward_.clear();
        motorBackward_.clear();
        motorTurnLeft_.clear();
        motorTurnRight_.clear();
        motorInteract_.clear();
        motorWait_.clear();
        
        world_.reset();
    }
    
    void updateNoveltyAndCuriosity(const std::vector<float>& vision) {
        if (vision.empty()) return;
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Enhanced novelty calculation with prediction error
        float predictionErrorWeighted = std::abs(predictionError_) * 5.0f;
        totalDiff += predictionErrorWeighted; // Prediction error adds to novelty
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update with curiosity modulation
        noveltyLevel_ *= sensoryNoveltyDecay_ * (1.0f - curiosityLevel_ * 0.1f);
        
        // Store for next time
        previousVision_ = vision;
    }
    
    void updateDevelopmentAndPlasticity() {
        // Update development based on novelty
        if (developmentEnabled_) {
            developmentalAge_ += noveltyLevel_ * 10.0; // Novelty accelerates development
        }
        
        // Update plasticity based on curiosity
        if (curiosityEnabled_) {
            plasticityModifier_ = 1.0f - curiosityLevel_ * 0.5f; // Higher curiosity reduces plasticity
            plasticityModifier_ = std::clamp(plasticityModifier_, 0.1f, 1.0f);
        }
    }
    
    void processVisionWithNeuromodulation(const std::vector<float>& vision) {
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                float current = vision[i] * 5.0f; // Scale factor
                
                // Apply neuromodulation to sensory processing
                float neuromodulation = 1.0f;
                if (rewardModulationEnabled_) {
                    neuromodulation += dopamineLevel_ * 0.3f;
                }
                if (curiosityEnabled_ && noveltyLevel_ > 0.3f) {
                    neuromodulation += 0.5f; // Enhance sensitivity to novel stimuli
                }
                
                current *= neuromodulation;
                
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    void processTouchWithPredictionError(const std::vector<float>& touch) {
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f; // Collision signal
                
                // Enhanced with prediction error modulation
                float predictionErrorMod = 1.0f + std::abs(predictionError_) * 0.5f;
                current *= predictionErrorMod;
                
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    void processInternalWithCuriosity(const std::vector<float>& intern) {
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f; // Center and scale
                
                // Enhanced with curiosity-based exploration
                if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
                    current *= 1.5f; // Enhance internal exploration
                }
                
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    void processProprioceptionWithReward(const std::vector<float>& proprio) {
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f; // Center and scale
                
                // Enhanced with reward modulation
                if (rewardModulationEnabled_) {
                    float rewardMod = dopamineLevel_ * 0.2f + 1.0f;
                    current *= rewardMod;
                }
                
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    void applyDevelopmentalModulation(float& forwardAct, float& backwardAct, float& leftAct, 
                                       float& rightAct, float& interactAct, float& waitAct) {
        // Development affects sensory processing (reduced sensitivity in adult)
        if (developmentEnabled_ && developmentalAge_ > 300.0) {
            // Reduced sensory gain in adult stage
            float sensoryGainReduction = 0.8f - (plasticityModifier_ * 0.3f);
            
            // Scale motor activities based on development
            forwardAct *= sensoryGainReduction;
            backwardAct *= sensoryGainReduction;
            leftAct *= sensoryGainReduction;
            rightAct *= sensoryGainReduction;
            interactAct *= sensoryGainReduction;
            waitAct *= sensoryGainReduction;
        }
    }
    
    void applyCuriosityExploration(float& forwardAct, float& backwardAct, float& leftAct,
                                    float& rightAct, float& interactAct, float& waitAct) {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel_ * 0.3f; // Up to 30% random
            
            // In a real implementation, this would use random number generation
            // For now, we just log the intent
            if (exploreChance > 0.0f) {
                NLM_LOG_INFO(std::string("Curiosity-based exploration enabled (chance=") + 
                            std::to_string(exploreChance) + ")");
            }
        }
    }
    
    void applyNeuromodulationToMotor(float& forwardAct, float& backwardAct, float& leftAct,
                                     float& rightAct, float& interactAct, float& waitAct) {
        // Apply neuromodulation-based motor modulation
        if (rewardModulationEnabled_ && std::abs(dopamineLevel_) > 0.2f) {
            // High dopamine -> increased exploration
            if (dopamineLevel_ > 0.5f) {
                // Strong positive dopamine encourages novel actions
                // In a real implementation, this would bias motor selection
                NLM_LOG_INFO(std::string("High dopamine promotes exploration: ") + 
                            std::to_string(dopamineLevel_));
            } else if (dopamineLevel_ < -0.5f) {
                // Strong negative dopamine encourages cautious behavior
                waitAct *= 2.0f; // Bias toward waiting
                NLM_LOG_INFO(std::string("High negative dopamine promotes caution: ") + 
                            std::to_string(dopamineLevel_));
            }
        }
    }
    
    std::string motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
};

// ============================================================================
// Enhanced Logging Macros for AgentBrain Operations
// ============================================================================

#define NLM_AGENTBRAIN_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug("AgentBrain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_AGENTBRAIN_LOG_INFO(msg) nlm::Logger::getGlobal().info("AgentBrain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_AGENTBRAIN_LOG_WARNING(msg) nlm::Logger::getGlobal().warning("AgentBrain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_AGENTBRAIN_LOG_ERROR(msg) nlm::Logger::getGlobal().error("AgentBrain: " + msg, __FILE__, __LINE__, __func__)
#define NLM_AGENTBRAIN_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical("AgentBrain: " + msg, __FILE__, __LINE__, __func__)

// ============================================================================
// AgentBrain Integration Utilities
// ============================================================================

// Integration helper for connecting AgentBrain components
class AgentBrainIntegrationHelper {
public:
    // Integrate AgentBrain with world and brain
    static bool integrateAgentBrain(std::shared_ptr<AgentBrain> agentBrain, 
                                    std::shared_ptr<SimpleWorld> world) {
        if (!agentBrain || !world) {
            NLM_LOG_ERROR("Cannot integrate null AgentBrain or world");
            return false;
        }
        
        try {
            // Initialize AgentBrain with world
            agentBrain->initialize(*world);
            
            // Validate state
            if (!agentBrain->validateState()) {
                auto errors = agentBrain->getValidationErrors();
                for (const auto& error : errors) {
                    NLM_LOG_ERROR(std::string("AgentBrain integration error: ") + error);
                }
                return false;
            }
            
            NLM_LOG_INFO("AgentBrain integration successful");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("AgentBrain integration failed: ") + e.what());
            return false;
        }
    }
    
    // Create enhanced AgentBrain with validation
    static std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        if (!brain) {
            NLM_LOG_ERROR("Cannot create AgentBrain with null brain");
            return nullptr;
        }
        
        try {
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            NLM_LOG_INFO("Enhanced AgentBrain created successfully");
            return agentBrain;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("AgentBrain creation failed: ") + e.what());
            return nullptr;
        }
    }
    
    // Validate AgentBrain configuration
    static bool validateAgentBrain(std::shared_ptr<AgentBrain> agentBrain) {
        if (!agentBrain) {
            NLM_LOG_ERROR("Cannot validate null AgentBrain");
            return false;
        }
        
        return agentBrain->validateState();
    }
    
    // Get AgentBrain status report
    static std::string getAgentBrainStatus(std::shared_ptr<AgentBrain> agentBrain) {
        if (!agentBrain) {
            return "AgentBrain: Null";
        }
        
        return agentBrain->getStatusReport();
    }
};

} // namespace nlm

#endif // ENHANCED_AGENTBRAIN_HPP