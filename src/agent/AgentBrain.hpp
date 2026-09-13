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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced agent configuration options
    void setSensoryWeight(float weight) { sensoryWeight_ = weight; }
    void setMotorThreshold(float threshold) { motorThreshold_ = threshold; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setNoveltyDecay(float decay) { noveltyDecay_ = decay; }
    void setDevelopmentRate(double rate) { developmentRate_ = rate; }
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    
    float getSensoryWeight() const { return sensoryWeight_; }
    float getMotorThreshold() const { return motorThreshold_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getNoveltyDecay() const { return noveltyDecay_; }
    double getDevelopmentRate() const { return developmentRate_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Performance and monitoring options
    void setMaxSteps(int maxSteps) { maxSteps_ = maxSteps; }
    void setEnableMetrics(bool enable) { enableMetrics_ = enable; }
    void setEnableProfiling(bool enable) { enableProfiling_ = enable; }
    
    int getMaxSteps() const { return maxSteps_; }
    bool isMetricsEnabled() const { return enableMetrics_; }
    bool isProfilingEnabled() const { return enableProfiling_; }
    
    // Advanced behavior options
    void setExplorationMode(bool exploration) { explorationMode_ = exploration; }
    void setExploitationMode(bool exploitation) { exploitationMode_ = exploitation; }
    void setAdaptiveLearning(bool adaptive) { adaptiveLearning_ = adaptive; }
    
    bool isExplorationMode() const { return explorationMode_; }
    bool isExploitationMode() const { return exploitationMode_; }
    bool isAdaptiveLearning() const { return adaptiveLearning_; }
    
    // Output formatting options
    void setVerboseOutput(bool verbose) { verboseOutput_ = verbose; }
    void setJsonOutput(bool json) { jsonOutput_ = json; }
    
    bool isVerboseOutput() const { return verboseOutput_; }
    bool isJsonOutput() const { return jsonOutput_; }
    
    // Configuration serialization
    std::map<std::string, std::string> getConfig() const;
    void setConfig(const std::map<std::string, std::string>& config);
    
    // Batch processing and scripting capabilities
    void enableBatchMode(bool enable) { batchMode_ = enable; }
    bool isBatchMode() const { return batchMode_; }
    
    // Advanced feature toggles
    void setMemoryReplayEnabled(bool enable) { memoryReplayEnabled_ = enable; }
    void setCheckpointEnabled(bool enable) { checkpointEnabled_ = enable; }
    void setVisualizationEnabled(bool enable) { visualizationEnabled_ = enable; }
    
    bool isMemoryReplayEnabled() const { return memoryReplayEnabled_; }
    bool isCheckpointEnabled() const { return checkpointEnabled_; }
    bool isVisualizationEnabled() const { return visualizationEnabled_; }
    
    // Get comprehensive agent status
    std::map<std::string, py::object> getStatus() const;
    
    // Command processing and scripting
    void executeCommand(const std::string& command);
    bool commandExists(const std::string& command) const;
    std::vector<std::string> getAvailableCommands() const;
    
    // Advanced commands for research and development
    void runResearchSuite(const std::vector<std::string>& config_names, const SimpleWorld& world, int steps);
    void run_batch(int steps);
    void exportResults(const std::string& filename);
    void importConfiguration(const std::string& filename);
    void profilePerformance();
    void visualizeActivity(const std::string& output_format = "plot");
    void analyzeLearningCurves();
    void generateReport(const std::string& report_type = "comprehensive");
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Advanced configuration options
    float sensoryWeight_;
    float motorThreshold_;
    float curiosityThreshold_;
    float noveltyDecay_;
    double developmentRate_;
    
    // Performance monitoring
    int maxSteps_;
    bool enableMetrics_;
    bool enableProfiling_;
    
    // Advanced behavior modes
    bool explorationMode_;
    bool exploitationMode_;
    bool adaptiveLearning_;
    
    // Output formatting
    bool verboseOutput_;
    bool jsonOutput_;
    
    // Batch processing
    bool batchMode_;
    
    // Feature toggles
    bool memoryReplayEnabled_;
    bool checkpointEnabled_;
    bool visualizationEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
