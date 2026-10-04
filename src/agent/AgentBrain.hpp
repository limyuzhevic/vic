#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations for better encapsulation
class Neuron;
class Synapse;
class DevelopmentalSystem;
class NeuromodulationSystem;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with enhanced modularity
class AgentBrain {
public:
    // Configuration parameters for agent behavior
    struct Config {
        // Sensory processing parameters
        float visionScaleFactor = 5.0f;
        float touchScaleFactor = 8.0f;
        float internalScaleFactor = 5.0f;
        float proprioceptionScaleFactor = 3.0f;
        
        // Motor processing parameters
        float motorActivityThreshold = 0.5f;
        float curiosityThreshold = 0.3f;
        float maxCuriosityLevel = 1.0f;
        
        // Novelty detection parameters
        float noveltyDecayRate = 0.99f;
        float noveltySensitivity = 1.0f;
        float temporalNoveltyWeight = 0.3f;
        
        // Curiosity and exploration parameters
        float baseExplorationRate = 0.05f;
        float curiosityExplorationScaling = 0.8f;
        float noveltyDrivenExploration = 0.6f;
        
    // Development parameters
    float initialPlasticity = 1.0f;
    float criticalPeriodPlasticity = 0.8f;
    float maturationPlasticity = 0.5f;
    float adultPlasticity = 0.2f;
    
    // Structural plasticity parameters
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Neuromodulation parameters
    float dopamineMin = -1.0f;
    float dopamineMax = 1.0f;
    float plasticityRangeMin = 0.1f;
    float plasticityRangeMax = 2.0f;
        
        // Motor neuron group distribution
        std::array<int, 6> motorDistribution = {1, 1, 1, 1, 1, 1}; // forward, backward, left, right, interact, wait
        std::array<int, 4> sensoryDistribution = {4, 4, 2, 2};    // vision, touch, internal, proprioception
    };

    AgentBrain(std::shared_ptr<Brain> brain, const Config& config = Config{});
    ~AgentBrain();
    
    // Initialize with world and configure sensory input
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain with optimized batching
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command using advanced algorithms
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward with eligibility traces
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system with configurable stages
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level with temporal component
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get expected reward (for feedback to world)
    float getExpectedReward() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Reset agent for new episode with full state cleanup
    void reset();
    
    // Get brain pointer (const version for safe access)
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration methods for runtime adjustment
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }
    
    // Get motor neuron group statistics
    struct MotorStats {
        int forward, backward, left, right, interact, wait;
    };
    
    MotorStats getMotorStats() const;
    
    // Get sensory neuron group statistics  
    struct SensoryStats {
        int vision, touch, internal, proprioception;
    };
    
    SensoryStats getSensoryStats() const;
    
    // Enable/disable individual features
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

private:
    // Core systems
    std::shared_ptr<Brain> brain_;
    Config config_;
    
    // Motor neuron groups with better organization
    struct MotorGroups {
        std::vector<Neuron*> forward;
        std::vector<Neuron*> backward;
        std::vector<Neuron*> left;
        std::vector<Neuron*> right;
        std::vector<Neuron*> interact;
        std::vector<Neuron*> wait;
        std::vector<Neuron*> lookLeft;  // Added from original code
        std::vector<Neuron*> lookRight; // Added from original code
        
        void clear() {
            forward.clear(); backward.clear(); left.clear(); right.clear();
            interact.clear(); wait.clear(); lookLeft.clear(); lookRight.clear();
        }
    };
    
    // Sensory neuron groups with better organization
    struct SensoryGroups {
        std::vector<Neuron*> vision;
        std::vector<Neuron*> touch;
        std::vector<Neuron*> internal;
        std::vector<Neuron*> proprioception;
        
        void clear() {
            vision.clear(); touch.clear(); internal.clear(); proprioception.clear();
        }
    };
    
    MotorGroups motorGroups_;
    SensoryGroups sensoryGroups_;
    
    // Neuromodulation state with temporal dynamics
    struct NeuromodulationState {
        float dopamineLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float predictionError = 0.0f;
        float expectedReward = 0.0f;
        
        // Temporal components
        float noveltyHistory = 0.0f;
        float curiosityHistory = 0.0f;
        
        void reset() {
            dopamineLevel = noveltyLevel = curiosityLevel = predictionError = expectedReward = 0.0f;
            noveltyHistory = curiosityHistory = 0.0f;
        }
    };
    
    NeuromodulationState neuromodState_;
    
    // Development state with configurable stages
    struct DevelopmentState {
        double age = 0.0;
        float plasticityModifier = 1.0f;
        DevelopmentalStage stage = DevelopmentalStage::Initial;
        
        // Stage timing thresholds (in simulation steps)
        static constexpr double initialThreshold = 60.0;    // ~1 minute
        static constexpr double criticalThreshold = 300.0;  // ~5 minutes
        static constexpr double maturationThreshold = 900.0; // ~15 minutes
        
        void reset() {
            age = 0.0;
            plasticityModifier = 1.0f;
            stage = DevelopmentalStage::Initial;
        }
        
        void updateStage() {
            if (age < initialThreshold) {
                plasticityModifier = config.initialPlasticity;
                stage = DevelopmentalStage::Initial;
            } else if (age < criticalThreshold) {
                plasticityModifier = config.criticalPeriodPlasticity;
                stage = DevelopmentalStage::CriticalPeriod;
            } else if (age < maturationThreshold) {
                plasticityModifier = config.maturationPlasticity;
                stage = DevelopmentalStage::Maturation;
            } else {
                plasticityModifier = config.adultPlasticity;
                stage = DevelopmentalStage::Adult;
            }
        }
    };
    
    DevelopmentState devState_;
    
    // Configuration flags
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
    
    // Previous sensory state for novelty detection with circular buffer
    struct VisionBuffer {
        std::vector<float> buffer;
        size_t writeIndex = 0;
        bool initialized = false;
        
        void initialize(size_t size) {
            buffer.resize(size, 0.0f);
            writeIndex = 0;
            initialized = true;
        }
        
        void update(const std::vector<float>& newVision) {
            if (!initialized) return;
            
            const size_t size = buffer.size();
            for (size_t i = 0; i < std::min(newVision.size(), size); ++i) {
                buffer[writeIndex] = newVision[i];
                writeIndex = (writeIndex + 1) % size;
            }
        }
        
        float computeNovelty(const std::vector<float>& newVision, float sensitivity = 1.0f) {
            if (!initialized || newVision.empty()) return 0.0f;
            
            const size_t size = buffer.size();
            if (size == 0) return 0.0f;
            
            float totalDiff = 0.0f;
            size_t compareCount = 0;
            
            // Compare with recent history
            for (size_t i = 0; i < std::min(newVision.size(), size); ++i) {
                size_t idx = (writeIndex + size - 1 - i) % size;
                float diff = std::abs(newVision[i] - buffer[idx]);
                totalDiff += diff;
                compareCount++;
            }
            
            return (compareCount > 0) ? (totalDiff / compareCount) * sensitivity : 0.0f;
        }
        
        void reset() {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            writeIndex = 0;
            initialized = false;
        }
    } visionBuffer_;
    
    // Advanced exploration states for curiosity-driven learning
    enum class ExplorationState {
        EXPLOITATION,    // Exploit known behaviors
        EXPLORATION,     // Try new behaviors
        INTENSITY_EXPLORATION, // High intensity exploration
        STRATEGIC_EXPLORATION    // Goal-directed exploration
    };
    
    ExplorationState explorationState_ = ExplorationState::EXPLOITATION;
    float explorationTimer_ = 0.0f;
    float stateTransitionThreshold_ = 0.7f;
    
    // Performance optimization caches
    mutable bool motorStatsCacheValid_ = false;
    mutable MotorStats motorStatsCache_;
    mutable bool sensoryStatsCacheValid_ = false;
    mutable SensoryStats sensoryStatsCache_;
    
    // Private methods for better encapsulation
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    
    // Enhanced motor decoding algorithms
    MotorCommand decodeFromMotorNeuronsAdvanced();
    MotorCommand decodeFromMotorNeuronsTraditional();
    
    // Advanced curiosity and exploration mechanisms
    MotorCommand selectWithCuriosityAdvanced(MotorCommand defaultCmd);
    MotorCommand selectWithCuriosityStrategic(MotorCommand defaultCmd);
    
    // Neuromodulation with eligibility traces
    void applyRewardModulationAdvanced(float reward, float predictedReward);
    void updatePlasticityBasedOnDopamine();
    
    // Development system integration
    void updateDevelopmentAdvanced(double timestep);
    void updateStructuralPlasticity(double timestep);
    
    // Novelty detection with temporal components
    void computeAdvancedNovelty(const std::vector<float>& vision);
    float computeTemporalNovelty(const std::vector<float>& currentVision);
    
    // Curiosity and exploration state management
    void updateExplorationState();
    float getExplorationRate() const;
    MotorCommand generateStrategicExplorationAction() const;
    
    // Performance optimization helpers
    void invalidateCaches() { motorStatsCacheValid_ = false; sensoryStatsCacheValid_ = false; }
    void updateMotorStatsCache() const;
    void updateSensoryStatsCache() const;
    
    // Input validation and safety checks
    void validateInputArrays(const std::vector<float>& vision, const std::vector<float>& touch,
                           const std::vector<float>& internal, const std::vector<float>& proprioception) const;
    
    // Statistics and logging helpers
    void logSystemState() const;
    
}; // namespace nlm