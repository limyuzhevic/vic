#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../core/Logger/Logger.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "MotorCommandSelector.hpp"
#include "NoveltyCalculator.hpp"
#include "MotorDecoder.hpp"

namespace nlm {

class Brain;
class Neuron;
class MotorCommandSelector;
class NoveltyCalculator;
class MotorDecoder;

class AgentBrainConfig {
public:
    struct MotorSelectorConfig {
        float curiosityThreshold = 0.3f;      // Threshold for curiosity-based exploration
        float randomActionThreshold = 0.5f;   // Threshold for high curiosity exploration
        float maxExploreChance = 0.3f;       // Maximum probability of random action
        bool enableCuriosity = true;          // Enable curiosity system
        std::vector<MotorCommand> actionSpace; // Available actions for random selection
    };

    struct NoveltyCalculatorConfig {
        float sensoryNoveltyDecay = 0.99f;   // Decay rate for novelty levels
        float visionScaleFactor = 2.0f;     // Scale factor for vision novelty calculation
        float touchScaleFactor = 1.0f;      // Scale factor for touch signals
        float internalScaleFactor = 1.0f;   // Scale factor for internal signals
        float proprioceptionScaleFactor = 1.0f; // Scale factor for proprioception
        bool enableCuriosity = true;         // Enable curiosity system
    };

    struct MotorDecoderConfig {
        float activationThreshold = 0.5f;       // Minimum activity to consider action
        bool normalizeActivities = true;        // Normalize neuron activities
        float visionScaleFactor = 5.0f;         // Scale factor for vision input
        float touchScaleFactor = 8.0f;          // Scale factor for touch input
        float internalScaleFactor = 5.0f;       // Scale factor for internal signals
        float proprioceptionScaleFactor = 3.0f; // Scale factor for proprioception
    };

    // Configuration flags
    bool enableDevelopment = true;
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    bool enableCuriosity = true;

    // Sensory processing configuration
    float visionScaleFactor = 5.0f;
    float touchScaleFactor = 8.0f;
    float internalScaleFactor = 5.0f;
    float proprioceptionScaleFactor = 3.0f;

    // Novelty configuration
    float sensoryNoveltyDecay = 0.99f;

    MotorSelectorConfig motorSelectorConfig;
    NoveltyCalculatorConfig noveltyCalculatorConfig;
    MotorDecoderConfig motorDecoderConfig;

    // Command line interface options
    struct CommandLineOptions {
        bool verbose = false;
        bool debug = false;
        bool benchmark = false;
        std::string logLevel = "Info";
        std::string outputFormat = "text";
        std::map<std::string, std::string> customOptions;
    };

    CommandLineOptions cliOptions;

    std::string getConfigSummary() const;
    void printConfig() const;
    bool validateConfig() const;
    void updateFromFile(const std::string& filename);
    void updateFromCommandLine(int argc, char** argv);
    std::map<std::string, std::string> getKeyValuePairs() const;
};

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
    ~AgentBrain();

    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;

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

    // Get developmental age
    double getDevelopmentalAge() const;

    // Get plasticity modifier
    float getPlasticityModifier() const;

    // Reset agent for new episode
    void reset();

    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }

    // Configuration methods
    void enableRewardModulation(bool enable) { config_.enableRewardModulation = enable; }
    void enableStructuralPlasticity(bool enable) { config_.enableStructuralPlasticity = enable; }
    void enableDevelopment(bool enable) { config_.enableDevelopment = enable; }
    void enableCuriosity(bool enable) { 
        config_.enableCuriosity = enable; 
        if (motorSelector_) motorSelector_->enableCuriosity(enable);
        if (noveltyCalculator_) noveltyCalculator_->enableCuriosity(enable);
    }

    bool isRewardModulationEnabled() const { return config_.enableRewardModulation; }
    bool isStructuralPlasticityEnabled() const { return config_.enableStructuralPlasticity; }
    bool isDevelopmentEnabled() const { return config_.enableDevelopment; }
    bool isCuriosityEnabled() const { return config_.enableCuriosity; }

    // Advanced features for power users
    void setActionSpace(const std::vector<MotorCommand>& actions);
    void setSensoryScaleFactors(float vision, float touch, float internal, float proprioception);
    void setCuriosityParameters(float threshold, float maxChance);
    void setCommandLineOptions(const AgentBrainConfig::CommandLineOptions& options) { config_.cliOptions = options; }

    // Configuration access
    AgentBrainConfig& getConfig() { return config_; }
    const AgentBrainConfig& getConfig() const { return config_; }

    // Utility methods
    std::string getConfigSummary() const;
    void printStatus() const;

    // Command line interface
    static bool processCommandLine(int argc, char** argv, AgentBrainConfig& config);
    static void printHelp();

private:
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;

    // Neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;

    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;

    // State variables
    float dopamineLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = 1.0f;
    std::vector<float> previousVision_;

    // Subsystem components
    std::shared_ptr<MotorCommandSelector> motorSelector_;
    std::shared_ptr<NoveltyCalculator> noveltyCalculator_;
    std::shared_ptr<MotorDecoder> motorDecoder_;

    // Private helper methods
    void initializeNeuronGroups();
    void applyNeuromodulationToBrain();
    void updateDevelopmentStages();
};

// Global functions for command line interface
namespace AgentBrainCLI {
    bool runSimulation(AgentBrainConfig& config);
    bool runTest(AgentBrainConfig& config, const std::string& testName);
    bool exportStats(AgentBrainConfig& config, const std::string& outputFile);
    bool importConfig(AgentBrainConfig& config, const std::string& configFile);
    void benchmarkAgent(AgentBrainConfig& config, int iterations);
}

} // namespace nlm
