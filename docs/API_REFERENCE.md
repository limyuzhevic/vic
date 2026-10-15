# NLM API Reference

## Overview
NLM's API provides comprehensive access to neural network functionality, configuration, and experiment management.

## Core API

### Config Class
The `Config` class manages configuration parameters for the NLM brain.

#### Constructor
```cpp
Config()
```
Creates a new configuration object with default values.

#### Methods

**loadFromFile**
```cpp
bool loadFromFile(const std::string& filename)
```
Loads configuration from a file.

**set**
```cpp
void set(const std::string& key, const std::string& value)
```
Sets a configuration parameter.

**set**
```cpp
void set(const std::string& key, int value)
```
Sets a configuration parameter.

**set**
```cpp
void set(const std::string& key, float value)
```
Sets a configuration parameter.

**get**
```cpp
std::string get(const std::string& key) const
```
Gets a configuration parameter as a string.

**get**
```cpp
int get(const std::string& key) const
```
Gets a configuration parameter as an integer.

**get**
```cpp
float get(const std::string& key) const
```
Gets a configuration parameter as a float.

**validate**
```cpp
bool validate() const
```
Validates the configuration.

**summary**
```cpp
std::string summary() const
```
Returns a summary of the configuration.

#### Static Methods

**loadDefaults**
```cpp
static std::shared_ptr<Config> loadDefaults()
```
Loads default configuration.

**loadFromString**
```cpp
static std::shared_ptr<Config> loadFromString(const std::string& configString)
```
Loads configuration from a string.

#### Constants

**DEFAULT_NEURON_COUNT**
```cpp
static constexpr int DEFAULT_NEURON_COUNT = 1000;
```
Default number of neurons.

**DEFAULT_REGION_COUNT**
```cpp
static constexpr int DEFAULT_REGION_COUNT = 2;
```
Default number of regions.

**DEFAULT_CONNECTION_PROBABILITY**
```cpp
static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1;
```
Default connection probability.

### Brain Class
The `Brain` class manages neural computation and brain dynamics.

#### Constructor
```cpp
Brain(std::shared_ptr<Config> config)
```
Creates a new brain with the given configuration.

#### Methods

**initialize**
```cpp
bool initialize()
```
Initializes the brain.

**step**
```cpp
void step(float dt)
```
Performs one simulation step.

**getAverageFiringRate**
```cpp
float getAverageFiringRate() const
```
Gets the average firing rate.

**getTotalSpikeCount**
```cpp
int getTotalSpikeCount() const
```
Gets the total spike count.

**getNeuronActivity**
```cpp
std::vector<float> getNeuronActivity() const
```
Gets the neural activity.

**getMemoryContent**
```cpp
std::vector<float> getMemoryContent() const
```
Gets the memory content.

**getSimulationTime**
```cpp
float getSimulationTime() const
```
Gets the current simulation time.

**updateConfig**
```cpp
void updateConfig(const std::map<std::string, std::string>& parameters)
```
Updates the brain configuration.

### ExtensibleBrain Class
The `ExtensibleBrain` class provides access to integrated memory systems.

#### Constructor
```cpp
ExtensibleBrain(std::shared_ptr<Config> config)
```
Creates a new extensible brain with the given configuration.

#### Methods

**getWorkingMemory**
```cpp
NeuralWorkingMemory* getWorkingMemory() const
```
Gets the working memory system.

**getEpisodicMemory**
```cpp
NeuralEpisodicMemory* getEpisodicMemory() const
```
Gets the episodic memory system.

**getAssociativeMemory**
```cpp
NeuralAssociativeMemory* getAssociativeMemory() const
```
Gets the associative memory system.

**getPredictionSystem**
```cpp
PredictionSystem* getPredictionSystem() const
```
Gets the prediction system.

**getNeuralPlanner**
```cpp
NeuralPlanner* getNeuralPlanner() const
```
Gets the neural planner.

**getConceptFormation**
```cpp
ConceptFormation* getConceptFormation() const
```
Gets the concept formation system.

**getAttentionalSelection**
```cpp
AttentionalSelection* getAttentionalSelection() const
```
Gets the attentional selection system.

**getDevelopmentSystem**
```cpp
DevelopmentSystem* getDevelopmentSystem() const
```
Gets the development system.

**getDopamine**
```cpp
Dopamine* getDopamine() const
```
Gets the dopamine system.

**getCuriosity**
```cpp
Curiosity* getCuriosity() const
```
Gets the curiosity system.

**getPredictionError**
```cpp
PredictionError* getPredictionError() const
```
Gets the prediction error system.

### AgentBrain Class
The `AgentBrain` class connects the NLM brain to the world.

#### Constructor
```cpp
AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment)
```
Creates a new agent brain with the given brain and environment.

#### Methods

**processSensoryInput**
```cpp
void processSensoryInput(const std::vector<float>& sensoryInput)
```
Processes sensory input.

**decodeMotorCommand**
```cpp
std::vector<float> decodeMotorCommand() const
```
Decodes the motor command.

**act**
```cpp
void act()
```
Takes an action.

**getAction**
```cpp
std::vector<float> getAction() const
```
Gets the action.

**getReward**
```cpp
float getReward() const
```
Gets the reward.

### Experiment Class
The `Experiment` class defines the interface for all experiments.

#### Constructor
```cpp
Experiment()
```
Creates a new experiment.

#### Methods

**initialize**
```cpp
virtual bool initialize(const Config& config) = 0
```
Initializes the experiment.

**runStep**
```cpp
virtual void runStep(float dt) = 0
```
Runs one simulation step.

**collectData**
```cpp
virtual ExperimentData collectData() const = 0
```
Collects experiment data.

**analyze**
```cpp
virtual AnalysisResults analyze(const std::vector<ExperimentData>& data) = 0
```
Analyzes experiment data.

**generateReport**
```cpp
virtual std::string generateReport(const AnalysisResults& results) = 0
```
Generates a report.

**configure**
```cpp
virtual void configure(const std::map<std::string, std::string>& parameters) = 0
```
Configures the experiment.

### ExperimentRunner Class
The `ExperimentRunner` class manages experiment execution.

#### Constructor
```cpp
ExperimentRunner()
```
Creates a new experiment runner.

#### Methods

**run**
```cpp
virtual bool run(Experiment& experiment, const Config& config) = 0
```
Runs an experiment.

**runWithCollection**
```cpp
virtual bool runWithCollection(Experiment& experiment, const Config& config, DataLogger& logger) = 0
```
Runs an experiment with data collection.

**getResults**
```cpp
virtual const std::vector<ExperimentData>& getResults() const = 0
```
Gets the experiment results.

**getMetrics**
```cpp
virtual const std::vector<AnalysisResults>& getMetrics() const = 0
```
Gets the experiment metrics.

### MetricsCalculator Class
The `MetricsCalculator` class calculates performance metrics.

#### Constructor
```cpp
MetricsCalculator()
```
Creates a new metrics calculator.

#### Methods

**calculate**
```cpp
virtual AnalysisResults calculate(const std::vector<ExperimentData>& data) = 0
```
Calculates metrics.

**validate**
```cpp
virtual bool validate(const AnalysisResults& results) = 0
```
Validates metrics.

**getMetricNames**
```cpp
virtual std::vector<std::string> getMetricNames() const = 0
```
Gets the metric names.

### DataLogger Class
The `DataLogger` class logs experiment data.

#### Constructor
```cpp
DataLogger()
```
Creates a new data logger.

#### Methods

**logData**
```cpp
virtual bool logData(const ExperimentData& data) = 0
```
Logs data.

**logMetadata**
```cpp
virtual bool logMetadata(const std::map<std::string, std::string>& metadata) = 0
```
Logs metadata.

**getLogPath**
```cpp
virtual std::string getLogPath() const = 0
```
Gets the log path.

**close**
```cpp
virtual void close() = 0
```
Closes the data logger.

## Plugin API

### Plugin Interface
The `PluginInterface` class defines the interface for all plugins.

#### Methods

**getName**
```cpp
virtual std::string getName() const = 0
```
Gets the plugin name.

**initialize**
```cpp
virtual bool initialize(const std::map<std::string, std::string>& parameters) = 0
```
Initializes the plugin.

**update**
```cpp
virtual void update(float dt) = 0
```
Updates the plugin.

**isEnabled**
```cpp
virtual bool isEnabled() const = 0
```
Checks if the plugin is enabled.

**enable**
```cpp
virtual void enable() = 0
```
Enables the plugin.

**disable**
```cpp
virtual void disable() = 0
```
Disables the plugin.

### Neuron Model Plugin
The `INeuronModelPlugin` class defines the interface for neuron model plugins.

#### Methods

**getName**
```cpp
virtual std::string getName() const = 0
```
Gets the plugin name.

**updateDynamics**
```cpp
virtual float updateDynamics(const NeuronState& state, float dt) = 0
```
Updates the neuron dynamics.

**hasSpike**
```cpp
virtual bool hasSpike() const = 0
```
Checks if the neuron has spiked.

**resetSpike**
```cpp
virtual void resetSpike() = 0
```
Resets the spike state.

### Plasticity Rule Plugin
The `IPlasticityRulePlugin` class defines the interface for plasticity rule plugins.

#### Methods

**getName**
```cpp
virtual std::string getName() const = 0
```
Gets the plugin name.

**computeWeightChange**
```cpp
virtual float computeWeightChange(const SynapseState& pre, const SynapseState& post, float dt) = 0
```
Computes the weight change.

### PluginManager Class
The `PluginManager` class manages plugins.

#### Methods

**loadPlugin**
```cpp
bool loadPlugin(const std::string& pluginName, const std::string& pluginPath)
```
Loads a plugin.

**unloadPlugin**
```cpp
bool unloadPlugin(const std::string& pluginName)
```
Unloads a plugin.

**getPlugin**
```cpp
PluginInterface* getPlugin(const std::string& pluginName) const
```
Gets a plugin.

**getPluginNames**
```cpp
std::vector<std::string> getPluginNames() const
```
Gets the plugin names.

## CLI API

### Command Line Interface
NLM provides a comprehensive command-line interface for running simulations and experiments.

#### Basic Commands

**--help**
```bash
nlm --help
```
Shows help information.

**--version**
```bash
nlm --version
```
Shows version information.

**--mode**
```bash
nlm --mode basic
```
Runs a basic simulation.

**--config**
```bash
nlm --config config.cfg
```
Loads configuration from a file.

**--neurons**
```bash
nlm --neurons 1000
```
Sets the number of neurons.

**--steps**
```bash
nlm --steps 1000
```
Sets the number of simulation steps.

**--checkpoint**
```bash
nlm --checkpoint checkpoint.bin
```
Sets the checkpoint file.

**--log-level**
```bash
nlm --log-level Debug
```
Sets the log level.

#### Advanced Commands

**--profile**
```bash
nlm --profile detailed
```
Enables profiling.

**--validate**
```bash
nlm --validate
```
Validates the configuration.

**--experimental**
```bash
nlm --experimental
```
Runs experimental mode.

**--benchmark**
```bash
nlm --benchmark
```
Runs benchmark mode.

## Configuration API

### Configuration Parameters

**Config.hpp**
```cpp
// Core configuration parameters
struct Config {
    int neuron_count;
    int region_count;
    float connection_probability;
    float simulation_timestep;
    float plasticity_learning_rate;
    float stdp_ltp_weight;
    float stdp_ltd_weight;
    float stdp_time_constant;
    float development_synaptogenesis_rate;
    float development_pruning_rate;
    float dopamine_baseline;
    float curiosity_rate;
    float reward_discount_factor;
    std::string environment_name;
    int environment_width;
    int environment_height;
    int max_simulation_steps;
    float simulation_time_limit;
    bool visualization_enabled;
    float visualization_update_rate;
    std::string log_level;
    bool log_to_file;
    std::string log_filename;
};
```

## Error Handling

### Error Classes

**NLMError**
```cpp
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message);
};
```
Base error class for NLM.

**ConfigurationError**
```cpp
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message);
};
```
Configuration error.

**PluginError**
```cpp
class PluginError : public NLMError {
public:
    PluginError(const std::string& message);
};
```
Plugin error.

**ExperimentError**
```cpp
class ExperimentError : public NLMError {
public:
    ExperimentError(const std::string& message);
};
```
Experiment error.

**MemoryError**
```cpp
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message);
};
```
Memory error.

## Integration Examples

### C++ Integration
```cpp
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <memory>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set basic parameters
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully!" << std::endl;
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        
        // Get statistics
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }
    
    return 0;
}
```

### Python Integration
```python
import nlm

# Create configuration
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)

# Create brain
brain = nlm.Brain(config)

# Initialize
if brain.initialize():
    print("Brain initialized successfully!")
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Get statistics
    print("Average firing rate:", brain.getAverageFiringRate())
    print("Total spikes:", brain.getTotalSpikeCount())
```

## API Versioning

### Version Management
- **Major Version**: Breaking changes
- **Minor Version**: New features
- **Patch Version**: Bug fixes

### Version Compatibility
- **Backward Compatibility**: Older versions can read new configurations
- **Forward Compatibility**: Newer versions can read old configurations
- **API Compatibility**: Plugin interfaces remain stable

### Version Information
```cpp
class Version {
public:
    int major;
    int minor;
    int patch;
    
    std::string toString() const;
    bool isCompatible(const Version& other) const;
};
```