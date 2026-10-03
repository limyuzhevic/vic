#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Memory system configuration
    void setWorkingMemoryCapacity(size_t capacity);
    void setWorkingMemoryDecayRate(float rate);
    void enableEpisodicMemory(bool enable);
    void setMaxEpisodicEpisodes(size_t maxEpisodes);
    void enableAssociativeMemory(bool enable);
    void enablePredictionSystem(bool enable);
    void setPredictionInterval(size_t interval);
    
    // Cognition system configuration
    void setPlanningDepth(size_t depth);
    void enableConceptFormation(bool enable);
    void setAttentionInhibitionStrength(float strength);
    void setAttentionExcitationStrength(float strength);
    void enableSelfModel(bool enable);
    void enableSocialLearning(bool enable);
    
    // Development system configuration
    void setDevelopmentStage(DevelopmentalStage stage);
    void enableMaturation(bool enable);
    void setCriticalPeriodDuration(float duration);
    void setMaturationDuration(float duration);
    
    // Neuromodulation system configuration
    void enableDopamine(bool enable);
    void setDopamineEffect(float effect);
    void enableCuriosity(bool enable);
    void setCuriosityGain(float gain);
    void enableNovelty(bool enable);
    void setNoveltyThreshold(float threshold);
    void enablePredictionError(bool enable);
    
    // Plasticity system configuration
    void enableSTDP(bool enable);
    void setSTDPParameters(float ltpWeight, float ltdWeight, float tau);
    void enableHebbian(bool enable);
    void enableStructuralPlasticity(bool enable);
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
    // Sleep/rest cycle configuration
    void enableSleepCycle(bool enable);
    void setRestInterval(size_t interval);
    void setConsolidationInterval(size_t interval);
    void setReplayInterval(size_t interval);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    void setPerformanceSampleInterval(size_t interval);
    
    // Checkpoint system
    void setCheckpointDir(const std::string& dir);
    void setCheckpointInterval(size_t interval);
    void enableCheckpointCompression(bool enable);
    
    // Statistics and diagnostics
    size_t getWorkingMemoryCapacity() const;
    float getWorkingMemoryDecayRate() const;
    size_t getEpisodicMemoryEpisodeCount() const;
    size_t getEpisodicMemoryMaxEpisodes() const;
    size_t getAssociativeMemoryPatternCount() const;
    size_t getPredictionSystemFeatureCount() const;
    float getAttentionInhibitionStrength() const;
    float getAttentionExcitationStrength() const;
    size_t getPlannerDepth() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    DevelopmentalStage getCurrentDevelopmentalStage() const;
    size_t getRestStepsRemaining() const;
    size_t getConsolidationStepsRemaining() const;
    
    // Advanced commands for expert users
    void saveBrainState(const std::string& filepath) const;
    bool loadBrainState(const std::string& filepath);
    void resetMemorySystems();
    void clearEpisodicMemory();
    void consolidateMemory();
    void replayEpisodicMemory(size_t episodeCount);
    void enableReplay(bool enable);
    
    // Configuration import/export
    void importConfiguration(const std::string& configFile);
    void exportConfiguration(const std::string& configFile) const;
    void resetConfiguration();
    
    // Debug and monitoring
    void enableDebugLogging(bool enable);
    void setLogLevel(const std::string& level);
    void logSystemStatus() const;
    
private:
    // Internal configuration methods
    void configureMemorySystems();
    void configurePredictionSystem();
    void configureCognitionSystems();
    void configureDevelopmentSystem();
    void configureNeuromodulationSystems();
    void configurePlasticitySystems();
    void configureSleepRestCycle();
    void configurePerformanceMonitoring();
    void configureCheckpointSystem();

} // namespace nlm
