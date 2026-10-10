#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "core/ErrorHandling/ErrorHandling.hpp"

namespace nlm {

// Forward declarations for forward references
class Neuron;
class RandomGenerator;

// Topology types for power user neural architectures
enum class TopologyType {
    FeedForward,
    Recurrent,
    SkipConnections,
    Modular
};

// Custom Neuron Type
class CustomNeuronType {
public:
    CustomNeuronType(NeuronId id);
    ~CustomNeuronType();
    
    // Disable copying
    CustomNeuronType(const CustomNeuronType&) = delete;
    CustomNeuronType& operator=(const CustomNeuronType&) = delete;
    
    // Enable moving
    CustomNeuronType(CustomNeuronType&&) noexcept;
    CustomNeuronType& operator=(CustomNeuronType&&) noexcept;
    
    void setCustomParameters(float threshold, float capacitance, float timeConstant);
    float getCustomParameter(const std::string& param) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced Neural Architecture
class AdvancedNeuralArchitecture {
public:
    AdvancedNeuralArchitecture();
    ~AdvancedNeuralArchitecture();
    
    // Disable copying
    AdvancedNeuralArchitecture(const AdvancedNeuralArchitecture&) = delete;
    AdvancedNeuralArchitecture& operator=(const AdvancedNeuralArchitecture&) = delete;
    
    // Enable moving
    AdvancedNeuralArchitecture(AdvancedNeuralArchitecture&&) noexcept;
    AdvancedNeuralArchitecture& operator=(AdvancedNeuralArchitecture&&) noexcept;
    
    std::vector<NeuronId> createCustomTopology(int numNeurons, TopologyType type);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Custom Neuromodulation Strategy
class CustomNeuromodulationStrategy {
public:
    CustomNeuromodulationStrategy();
    ~CustomNeuromodulationStrategy();
    
    // Disable copying
    CustomNeuromodulationStrategy(const CustomNeuromodulationStrategy&) = delete;
    CustomNeuromodulationStrategy& operator=(const CustomNeuromodulationStrategy&) = delete;
    
    // Enable moving
    CustomNeuromodulationStrategy(CustomNeuromodulationStrategy&&) noexcept;
    CustomNeuromodulationStrategy& operator=(CustomNeuromodulationStrategy&&) noexcept;
    
    struct NeuromodulationParameters {
        float baselineLevel;
        float burstSensitivity;
        float decayRate;
        float noiseLevel;
        float adaptationRate;
    };
    
    NeuromodulationParameters getDopamineParameters() const;
    NeuromodulationParameters getAcetylcholineParameters() const;
    NeuromodulationParameters getNorepinephrineParameters() const;
    NeuromodulationParameters getSerotoninParameters() const;
    
    float calculateDynamicModulation(float baseline, float predictionError, 
                                    float timeFactor, float noise);
    float calculateAttentionModulation(float acetylcholineLevel, float noveltyLevel);
    float calculateArousalModulation(float norepinephrineLevel, float stressLevel);
    float calculateMoodModulation(float serotoninLevel, float rewardHistory);
    
    void updateDynamicParameters(float deltaTime);
    void setExternalInput(float reward, float predictionError, float novelty);
    float getModulationStrength() const;
    float getAdaptiveModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Power User Configuration Manager
class PowerUserConfigurationManager {
public:
    PowerUserConfigurationManager();
    ~PowerUserConfigurationManager();
    
    // Disable copying
    PowerUserConfigurationManager(const PowerUserConfigurationManager&) = delete;
    PowerUserConfigurationManager& operator=(const PowerUserConfigurationManager&) = delete;
    
    // Enable moving
    PowerUserConfigurationManager(PowerUserConfigurationManager&&) noexcept;
    PowerUserConfigurationManager& operator=(PowerUserConfigurationManager&&) noexcept;
    
    bool loadPowerUserConfig(const std::string& filepath);
    bool savePowerUserConfig(const std::string& filepath) const;
    bool createPowerUserProfile(const std::string& profileName, 
                               const std::unordered_map<std::string, std::string>& settings);
    bool loadPowerUserProfile(const std::string& profileName);
    std::vector<std::string> getAvailableProfiles() const;
    bool exportProfile(const std::string& profileName, const std::string& exportPath);
    bool importProfile(const std::string& importPath, const std::string& profileName);
    
    std::unordered_map<std::string, std::string> getPowerUserSettings() const;
    void setPowerUserSetting(const std::string& key, const std::string& value);
    std::string getPowerUserSetting(const std::string& key) const;
    
    bool resetToDefaults();
    std::string generatePowerUserProfileReport() const;
    
    bool applyPowerUserSetting(const std::string& key, const std::string& value);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Power User Command Line Interface
class PowerUserCommandLine {
public:
    PowerUserCommandLine();
    ~PowerUserCommandLine();
    
    // Disable copying
    PowerUserCommandLine(const PowerUserCommandLine&) = delete;
    PowerUserCommandLine& operator=(const PowerUserCommandLine&) = delete;
    
    // Enable moving
    PowerUserCommandLine(PowerUserCommandLine&&) noexcept;
    PowerUserCommandLine& operator=(PowerUserCommandLine&&) noexcept;
    
    bool parseArguments(int argc, char** argv);
    bool processCommands();
    
    void loadProfile(const std::string& profileName);
    void createProfile(const std::string& profileName);
    void listProfiles();
    void exportProfile(const std::string& filepath);
    void importProfile(const std::string& filepath);
    void resetToDefaults();
    void setTopology(const std::string& topology);
    void setNeuromodulationStrategy(const std::string& strategy);
    
    void showHelp();
    void showVersion();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
