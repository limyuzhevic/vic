// Enhanced Python API for NLM
// Header file for enhanced Python bindings with better error handling and type hints

#pragma once

#include <pybind11/pybind11.h>
#include <memory>
#include <functional>
#include <string>
#include <vector>

namespace py = pybind11;
namespace nlm {

// Forward declarations
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;

// Exception classes for enhanced error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& msg) : NLMError("Agent error: " + msg) {}
};

// Forward declaration for enhanced classes
class EnhancedConfig;
class EnhancedBrain;
class EnhancedAgentBrain;
class EnhancedSimpleWorld;

// Enhanced Config class with Pythonic interface
class EnhancedConfig {
public:
    EnhancedConfig();
    
    // Set configuration values with type safety
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, float value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // Get configuration values with type safety
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Configuration checks
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    
    // Configuration profiles
    void configureBrain(size_t neuronCount = 1000, float connectionDensity = 0.1f);
    void configureMemory(size_t capacity = 10000);
    void configureNeuromodulation(float dopamineScale = 1.0f, bool enableCuriosity = true);
    
    // File I/O
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    std::string summary(bool formatted = true) const;
    
    // Factory methods
    static EnhancedConfig fromJSON(const std::string& json);
    
    // Get underlying config
    std::shared_ptr<Config> getConfig() const;
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Brain class with better Python interface
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config);
    
    // Core simulation methods
    bool initialize();
    void step(size_t stepNumber);
    void step(size_t stepNumber, double time);
    void receiveSensoryInput(const std::vector<float>& input);
    
    // Statistics and monitoring
    py::dict getStatistics() const;
    void runSimulation(size_t steps, std::function<void(size_t, py::dict)> progressCallback = nullptr);
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region information
    py::dict getRegionInfo(size_t regionIndex = 0) const;
    std::vector<py::dict> getAllRegions() const;
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const;
    
    // Context manager support
    class BrainContext;
    std::unique_ptr<BrainContext> createContext();
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced AgentBrain class with Pythonic interface
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain);
    
    // Core agent methods
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const py::dict& perceptData);
    
    // Agent state and statistics
    py::dict getAgentState() const;
    py::dict getStatistics() const;
    
    // Neuromodulation and plasticity
    void enableRewardModulation(bool enable);
    void enableCuriosity(bool enable);
    void enableDevelopment(bool enable);
    void enableStructuralPlasticity(bool enable);
    
    // Subsystem status
    bool isRewardModulationEnabled() const;
    bool isCuriosityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    
    // Learning and development
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // State management
    void reset();
    
    // Get underlying agent brain
    std::shared_ptr<AgentBrain> getAgentBrain() const;
    
    // Factory method
    static std::shared_ptr<EnhancedAgentBrain> createEnhanced(std::shared_ptr<Brain> brain);
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Enhanced SimpleWorld class with Pythonic interface
class EnhancedSimpleWorld {
public:
    EnhancedSimpleWorld();
    
    // World configuration
    void configure(size_t width, size_t height, size_t visionWidth = 8, size_t visionHeight = 8);
    void setAgentStart(double x, double y);
    
    // Simulation control
    void update(double timestep);
    void applyMotorCommand(const ActionType& actionType, double currentTime);
    
    // State access
    py::dict getSensoryPercept() const;
    py::dict getAgentBody() const;
    
    // World management
    void reset();
    void addObject(double x, double y, WorldObjectType type, float value = 1.0f, float radius = 0.5f);
    void removeObject(double x, double y);
    bool isValidPosition(double x, double y) const;
    
    // World properties
    py::dict getDimensions() const;
    double getSimulationTime() const;
    void setRandomSeed(unsigned int seed);
    unsigned int getRandomSeed() const;
    
    // Get underlying world
    std::shared_ptr<SimpleWorld> getWorld() const;
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Factory functions
std::shared_ptr<EnhancedConfig> createEnhancedConfig();
std::shared_ptr<EnhancedBrain> createEnhancedBrain(std::shared_ptr<Config> config);
std::shared_ptr<EnhancedAgentBrain> createEnhancedAgentBrain(std::shared_ptr<Brain> brain);
std::shared_ptr<EnhancedSimpleWorld> createEnhancedSimpleWorld();

// Convenience functions
py::dict runCompleteAgentSimulation(size_t numSteps = 1000);
py::dict getDefaultConfiguration();
std::string generateConfigurationCode(const py::dict& config);

} // namespace nlm
