# NLM - API Reference

**Date:** 2026-10-13
**Audience:** All users
**Level:** API documentation

---

## Table of Contents

1. [API Overview](#api-overview)
2. [Core NLM API](#core-nlm-api)
3. [Advanced API](#advanced-api)
4. [Python Bindings](#python-bindings)
5. [C++ API](#c++-api)
6. [Factory Functions](#factory-functions)
7. [Type Definitions](#type-definitions)
8. [Error Handling](#error-handling)
9. [Memory Management](#memory-management)
10. [Thread Safety](#thread-safety)

---

## API Overview

The NLM API provides **comprehensive access** to neural network simulation capabilities through multiple interfaces:

### API Architecture

```
NLM API Structure:
├── Python Bindings (pynlm)
│   ├── Core NLM API
│   │   ├── Brain, AgentBrain, SimpleWorld
│   │   ├── Configuration management
│   │   └── Legacy compatibility
│   └── Advanced API (pynlm.advanced_api)
│       ├── High-level convenience functions
│       ├── Class abstractions
│       ├── Analysis tools
│       └── Debugging utilities
└── C++ API (via pybind11)
    ├── Low-level neural dynamics
    ├── Performance infrastructure
    └── Extension interfaces
```

### API Access Patterns

#### Pattern 1: Python Interface (Primary)
```python
import pynlm

# Access core API
brain = pynlm.createBrain(config)

# Access Advanced API
from pynlm.advanced_api import AdvancedAPI, SimpleAgent
```

#### Pattern 2: Advanced API Usage
```python
from pynlm.advanced_api import (
    AdvancedAPI,
    createBrainProfiler,
    createNetworkConnectivityAnalyzer,
    SimulationRunner,
    SimpleAgent
)
```

#### Pattern 3: Direct C++ Access (via Python)
```python
import pynlm

# Access C++ classes through Python bindings
brain = pynlm.Brain(config)
brain.initialize()
```

## Core NLM API

### Brain Class

#### Constructor
```python
brain = pynlm.createBrain(config)
brain = pynlm.Brain(config)  # Direct access to C++ class
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `createBrain(config)` | `Brain(config)` | Create brain instance | `std::invalid_argument` |
| `initialize()` | `void initialize()` | Initialize brain | `std::runtime_error` |
| `step(step)` | `void step(step)` | Run simulation step | `std::runtime_error` |
| `reset()` | `void reset()` | Reset brain state | `std::runtime_error` |
| `getTotalNeuronCount()` | `size_t getTotalNeuronCount() const` | Get neuron count | `none` |
| `getTotalSynapseCount()` | `size_t getTotalSynapseCount() const` | Get synapse count | `none` |
| `getFiringNeuronCount()` | `size_t getFiringNeuronCount() const` | Get firing neurons | `none` |
| `getAverageFiringRate()` | `double getAverageFiringRate() const` | Get average firing rate | `none` |
| `getTotalSpikeCount()` | `size_t getTotalSpikeCount() const` | Get total spikes | `none` |
| `getExcitationInhibitionRatio()` | `double getExcitationInhibitionRatio() const` | Get E/I balance | `none` |
| `getNeuronPotential(neuron_id)` | `double getNeuronPotential(size_t neuron_id) const` | Get neuron potential | `std::out_of_range` |
| `getSynapticWeight(pre_id, post_id)` | `double getSynapticWeight(size_t pre_id, size_t post_id) const` | Get synaptic weight | `std::out_of_range` |
| `save(filepath)` | `bool save(const std::string& filepath) const` | Save brain state | `std::runtime_error` |
| `load(filepath)` | `bool load(const std::string& filepath)` | Load brain state | `std::runtime_error` |
| `receiveSensoryInput(percept)` | `void receiveSensoryInput(const SensoryPercept& percept)` | Receive sensory input | `std::invalid_argument` |
| `produceAction()` | `MotorCommand produceAction()` | Produce motor command | `std::runtime_error` |

#### Inline Methods

```python
# These methods can be called more efficiently
brain.initialize()  # Initialize brain
brain.reset()       # Reset to initial state
brain.step(0)       # Run step with step number
brain.step(0, 0.1)  # Run step with timestamp
```

### AgentBrain Class

#### Constructor
```python
agent = pynlm.createAgentBrain(brain)
agent = pynlm.AgentBrain(brain)  # Direct access
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `createAgentBrain(brain)` | `AgentBrain(Brain& brain)` | Create agent interface | `std::invalid_argument` |
| `initialize(world)` | `void initialize(IWorldInterface& world)` | Initialize with world | `std::invalid_argument` |
| `processSensoryInput(percept)` | `void processSensoryInput(const SensoryPercept& percept)` | Process sensory input | `std::invalid_argument` |
| `decodeMotorCommand()` | `MotorCommand decodeMotorCommand()` | Decode motor command | `std::runtime_error` |
| `applyRewardModulation(reward, predicted)` | `void applyRewardModulation(double reward, double predicted)` | Apply reward modulation | `std::invalid_argument` |
| `updateDevelopment(timestep)` | `void updateDevelopment(double timestep)` | Update development | `std::invalid_argument` |
| `enableRewardModulation(enable)` | `void enableRewardModulation(bool enable)` | Enable/disable reward learning | `none` |
| `enableCuriosity(enable)` | `void enableCuriosity(bool enable)` | Enable/disable curiosity | `none` |
| `enableStructuralPlasticity(enable)` | `void enableStructuralPlasticity(bool enable)` | Enable/disable structural plasticity | `none` |
| `enableDevelopment(enable)` | `void enableDevelopment(bool enable)` | Enable/disable development | `none` |
| `getCuriosityLevel()` | `double getCuriosityLevel() const` | Get curiosity level | `none` |
| `getNoveltyLevel()` | `double getNoveltyLevel() const` | Get novelty level | `none` |
| `getPredictionError()` | `double getPredictionError() const` | Get prediction error | `none` |
| `getNeuromodulationLevel()` | `double getNeuromodulationLevel() const` | Get neuromodulation level | `none` |
| `getDevelopmentalStage()` | `DevelopmentalStage getDevelopmentalStage() const` | Get developmental stage | `none` |

### SimpleWorld Class

#### Constructor
```python
world = pynlm.createSimpleWorld()
world = pynlm.SimpleWorld()  # Direct access
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `createSimpleWorld()` | `SimpleWorld()` | Create simple world | `none` |
| `configure(width, height, visionWidth, visionHeight)` | `void configure(int w, int h, int vw, int vh)` | Configure world dimensions | `std::invalid_argument` |
| `reset()` | `void reset()` | Reset world to initial state | `std::runtime_error` |
| `update(timestep)` | `void update(double timestep)` | Update world by timestep | `std::runtime_error` |
| `getSensoryPercept()` | `SensoryPercept getSensoryPercept()` | Get sensory percept | `std::runtime_error` |
| `applyMotorCommand(action, time)` | `void applyMotorCommand(const MotorCommand& action, double time)` | Apply motor command | `std::runtime_error` |
| `getSimulationTime()` | `double getSimulationTime() const` | Get simulation time | `none` |
| `getWidth()` | `int getWidth() const` | Get world width | `none` |
| `getHeight()` | `int getHeight() const` | Get world height | `none` |
| `getVisionWidth()` | `int getVisionWidth() const` | Get vision width | `none` |
| `getVisionHeight()` | `int getVisionHeight() const` | Get vision height | `none` |
| `isValidPosition(x, y)` | `bool isValidPosition(double x, double y)` | Check if position is valid | `none` |

### Configuration Class

#### Constructor
```python
config = pynlm.createDefaultConfig()
config = pynlm.Config()  # Direct access
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `createDefaultConfig()` | `Config()` | Create default configuration | `none` |
| `set(key, value)` | `void set(const std::string& key, const std::string& value)` | Set configuration value | `std::invalid_argument` |
| `set(key, int)` | `void set(const std::string& key, int value)` | Set integer value | `std::invalid_argument` |
| `set(key, double)` | `void set(const std::string& key, double value)` | Set floating-point value | `std::invalid_argument` |
| `set(key, bool)` | `void set(const std::string& key, bool value)` | Set boolean value | `std::invalid_argument` |
| `get(key, default)` | `std::string get(const std::string& key, const std::string& default) const` | Get string value | `std::out_of_range` |
| `get(key, default)` | `int get(const std::string& key, int default) const` | Get integer value | `std::out_of_range` |
| `get(key, default)` | `double get(const std::string& key, double default) const` | Get floating-point value | `std::out_of_range` |
| `get(key, default)` | `bool get(const std::string& key, bool default) const` | Get boolean value | `std::out_of_range` |
| `has(key)` | `bool has(const std::string& key) const` | Check if key exists | `none` |
| `getKeys()` | `std::vector<std::string> getKeys() const` | Get all configuration keys | `none` |
| `loadFromFile(filepath)` | `bool loadFromFile(const std::string& filepath)` | Load configuration from file | `std::runtime_error` |
| `saveToFile(filepath)` | `bool saveToFile(const std::string& filepath) const` | Save configuration to file | `std::runtime_error` |

## Advanced API

### AdvancedAPI Class

#### Constructor
```python
advanced_api = AdvancedAPI()
advanced_api = AdvancedAPI(brain)  # Create with specific brain
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `AdvancedAPI()` | `AdvancedAPI()` | Default constructor | `none` |
| `AdvancedAPI(brain)` | `AdvancedAPI(Brain* brain)` | Constructor with brain | `none` |
| `runSimulation(config)` | `bool runSimulation(const SimulationConfig& config)` | Run complete simulation | `std::invalid_argument` |
| `runEpisode(agent, world, steps)` | `void runEpisode(SimpleAgent& agent, IWorldInterface& world, size_t steps)` | Run agent episode | `std::invalid_argument` |
| `runExperiment(manager)` | `void runExperiment(ExperimentManager& manager)` | Run experiment | `std::invalid_argument` |
| `createBrainProfiler(brain)` | `std::unique_ptr<BrainProfiler> createBrainProfiler(Brain* brain)` | Create brain profiler | `std::invalid_argument` |
| `createSimpleAgent(config)` | `std::unique_ptr<SimpleAgent> createSimpleAgent(const BrainConfig& config)` | Create simple agent | `std::invalid_argument` |
| `createSimulationRunner(numSteps, dt)` | `std::unique_ptr<SimulationRunner> createSimulationRunner(size_t numSteps, double dt)` | Create simulation runner | `std::invalid_argument` |
| `createVisualizationHelper()` | `std::unique_ptr<VisualizationHelper> createVisualizationHelper()` | Create visualization helper | `none` |
| `createExperimentManager()` | `std::unique_ptr<ExperimentManager> createExperimentManager()` | Create experiment manager | `none` |
| `analyzeBrainActivity(brain)` | `BrainAnalysis analyzeBrainActivity(Brain* brain)` | Analyze brain activity | `std::invalid_argument` |
| `computeStatistics(data)` | `StatisticalResults computeStatistics(const std::vector<double>& data)` | Compute statistics | `std::invalid_argument` |
| `visualizeBrainState(brain)` | `void visualizeBrainState(Brain* brain)` | Visualize brain state | `std::runtime_error` |
| `saveExperimentResults(manager, filepath)` | `void saveExperimentResults(ExperimentManager& manager, const std::string& filepath)` | Save experiment results | `std::runtime_error` |
| `loadExperimentResults(filepath)` | `ExperimentResults loadExperimentResults(const std::string& filepath)` | Load experiment results | `std::runtime_error` |

#### Factory Methods

```python
# Factory methods for creating analysis tools
brain_profiler = advanced_api.createBrainProfiler(brain)
network_analyzer = advanced_api.createNetworkConnectivityAnalyzer(brain)
memory_profiler = advanced_api.createMemorySystemProfiler(brain)
neuromod_analyzer = advanced_api.createNeuromodulationAnalyzer(brain)
development_analyzer = advanced_api.createDevelopmentStageAnalyzer(brain)

# Factory methods for creating agents and simulations
simple_agent = advanced_api.createSimpleAgent(config)
simulation_runner = advanced_api.createSimulationRunner(1000, 0.1)

# Convenience factory functions
from pynlm.advanced_api import (
    createBrainProfiler,
    createSimpleAgent,
    createSimulationRunner,
    createVisualizationHelper,
    createExperimentManager
)
```

### SimpleAgent Class

#### Constructor
```python
agent = SimpleAgent(brain, world, "agent_name")
agent = SimpleAgent(brain, world)  # Without name
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `SimpleAgent(brain, world, name)` | `SimpleAgent(Brain* brain, IWorldInterface* world, const std::string& name)` | Constructor with parameters | `std::invalid_argument` |
| `SimpleAgent(brain, world)` | `SimpleAgent(Brain* brain, IWorldInterface* world)` | Constructor without name | `std::invalid_argument` |
| `processSensoryInput(percept)` | `void processSensoryInput(const SensoryPercept& percept)` | Process sensory input | `std::invalid_argument` |
| `decodeMotorCommand()` | `MotorCommand decodeMotorCommand()` | Decode motor command | `std::runtime_error` |
| `applyRewardModulation(reward, predicted)` | `void applyRewardModulation(double reward, double predicted)` | Apply reward modulation | `std::invalid_argument` |
| `updateDevelopment(timestep)` | `void updateDevelopment(double timestep)` | Update development | `std::invalid_argument` |
| `enableRewardModulation(enable)` | `void enableRewardModulation(bool enable)` | Enable/disable reward learning | `none` |
| `enableCuriosity(enable)` | `void enableCuriosity(bool enable)` | Enable/disable curiosity | `none` |
| `enableStructuralPlasticity(enable)` | `void enableStructuralPlasticity(bool enable)` | Enable/disable structural plasticity | `none` |
| `enableDevelopment(enable)` | `void enableDevelopment(bool enable)` | Enable/disable development | `none` |
| `getCuriosityLevel()` | `double getCuriosityLevel() const` | Get curiosity level | `none` |
| `getNoveltyLevel()` | `double getNoveltyLevel() const` | Get novelty level | `none` |
| `getPredictionError()` | `double getPredictionError() const` | Get prediction error | `none` |
| `getNeuromodulationLevel()` | `double getNeuromodulationLevel() const` | Get neuromodulation level | `none` |
| `getDevelopmentalStage()` | `DevelopmentalStage getDevelopmentalStage() const` | Get developmental stage | `none` |

### SimulationRunner Class

#### Constructor
```python
sim_runner = SimulationRunner(num_steps=1000, dt=0.1)
sim_runner = SimulationRunner()  # Default constructor
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `SimulationRunner(size_t numSteps, double dt)` | `SimulationRunner(size_t numSteps, double dt)` | Constructor with parameters | `std::invalid_argument` |
| `SimulationRunner()` | `SimulationRunner()` | Default constructor | `none` |
| `addAgent(agent)` | `void addAgent(SimpleAgent& agent)` | Add agent to simulation | `std::invalid_argument` |
| `removeAgent(agent)` | `void removeAgent(SimpleAgent& agent)` | Remove agent from simulation | `std::invalid_argument` |
| `setWorld(world)` | `void setWorld(IWorldInterface& world)` | Set simulation world | `std::invalid_argument` |
| `getWorld()` | `IWorldInterface* getWorld()` | Get simulation world | `none` |
| `start()` | `void start()` | Start simulation | `std::runtime_error` |
| `stop()` | `void stop()` | Stop simulation | `std::runtime_error` |
| `step()` | `void step()` | Run one simulation step | `std::runtime_error` |
| `step(step)` | `void step(size_t step)` | Run step with step number | `std::runtime_error` |
| `run()` | `void run()` | Run complete simulation | `std::runtime_error` |
| `run(steps)` | `void run(size_t steps)` | Run simulation for specified steps | `std::runtime_error` |
| `getStatistics()` | `SimulationStatistics getStatistics() const` | Get simulation statistics | `none` |
| `getLearningCurve()` | `std::vector<std::pair<size_t, double>> getLearningCurve() const` | Get learning curve | `none` |
| `addProfiler(profiler)` | `void addProfiler(BrainProfiler* profiler)` | Add profiler to simulation | `std::invalid_argument` |
| `addMonitor(monitor)` | `void addMonitor(MonitorBase* monitor)` | Add monitor to simulation | `std::invalid_argument` |
| `addAnalyzer(analyzer)` | `void addAnalyzer(AnalysisTool* analyzer)` | Add analyzer to simulation | `std::invalid_argument` |
| `saveResults(filepath)` | `void saveResults(const std::string& filepath)` | Save simulation results | `std::runtime_error` |
| `getPerformanceMetrics()` | `PerformanceMetrics getPerformanceMetrics() const` | Get performance metrics | `none` |

### BrainProfiler Class

#### Constructor
```python
profiler = BrainProfiler(brain)
profiler = BrainProfiler(brain, profiling_config)  # With configuration
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `BrainProfiler(Brain* brain)` | `BrainProfiler(Brain* brain)` | Default constructor | `std::invalid_argument` |
| `BrainProfiler(Brain* brain, const ProfilingConfig& config)` | `BrainProfiler(Brain* brain, const ProfilingConfig& config)` | Constructor with configuration | `std::invalid_argument` |
| `start()` | `void start()` | Start brain profiling | `std::runtime_error` |
| `stop()` | `void stop()` | Stop brain profiling | `std::runtime_error` |
| `getProfile(duration)` | `BrainProfile getProfile(double duration)` | Get brain profile | `std::runtime_error` |
| `getProfileSummary()` | `std::string getProfileSummary() const` | Get profile summary | `none` |
| `getNeuralDynamics()` | `NeuralDynamicsAnalysis getNeuralDynamics() const` | Get neural dynamics analysis | `none` |
| `getNetworkAnalysis()` | `NetworkAnalysis getNetworkAnalysis() const` | Get network analysis | `none` |
| `getMemoryAnalysis()` | `MemoryAnalysis getMemoryAnalysis() const` | Get memory analysis | `none` |
| `getNeuromodulationAnalysis()` | `NeuromodulationAnalysis getNeuromodulationAnalysis() const` | Get neuromodulation analysis | `none` |
| `getDevelopmentAnalysis()` | `DevelopmentAnalysis getDevelopmentAnalysis() const` | Get development analysis | `none` |
| `getActivityStats(step)` | `ActivityStatistics getActivityStats(size_t step) const` | Get activity statistics | `none` |

### VisualizationHelper Class

#### Constructor
```python
viz_helper = VisualizationHelper()
viz_helper = VisualizationHelper(viz_config)  # With configuration
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `VisualizationHelper()` | `VisualizationHelper()` | Default constructor | `none` |
| `VisualizationHelper(const VisualizationConfig& config)` | `VisualizationHelper(const VisualizationConfig& config)` | Constructor with configuration | `none` |
| `plotHeatmap(data, title, filename)` | `void plotHeatmap(const std::vector<std::vector<double>>& data, const std::string& title, const std::string& filename)` | Plot heatmap | `std::runtime_error` |
| `plotLearningCurve(steps, values, filename)` | `void plotLearningCurve(const std::vector<size_t>& steps, const std::vector<double>& values, const std::string& filename)` | Plot learning curve | `std::runtime_error` |
| `plotNetworkTopology(data, filename)` | `void plotNetworkTopology(const NetworkData& data, const std::string& filename)` | Plot network topology | `std::runtime_error` |
| `plotTimeSeries(data, filename)` | `void plotTimeSeries(const TimeSeriesData& data, const std::string& filename)` | Plot time series | `std::runtime_error` |
| `exportToCSV(data, filename)` | `void exportToCSV(const std::vector<std::vector<double>>& data, const std::string& filename)` | Export to CSV | `std::runtime_error` |
| `exportToJSON(data, filename)` | `void exportToJSON(const std::vector<std::vector<double>>& data, const std::string& filename)` | Export to JSON | `std::runtime_error` |
| `exportToMATLAB(data, filename)` | `void exportToMATLAB(const std::vector<std::vector<double>>& data, const std::string& filename)` | Export to MATLAB | `std::runtime_error` |
| `getColorMap(name)` | `std::vector<std::array<double, 3>> getColorMap(const std::string& name)` | Get color map | `std::invalid_argument` |
| `generateColorGradient(start_color, end_color, steps)` | `std::vector<std::array<double, 3>> generateColorGradient(const std::array<double, 3>& start_color, const std::array<double, 3>& end_color, size_t steps)` | Generate color gradient | `none` |

### ExperimentManager Class

#### Constructor
```python
experiment_manager = ExperimentManager()
experiment_manager = ExperimentManager(exp_config)  # With configuration
```

#### Core Methods

| Method | Signature | Description | Exception |
|--------|-----------|-------------|-----------|
| `ExperimentManager()` | `ExperimentManager()` | Default constructor | `none` |
| `ExperimentManager(const ExperimentConfig& config)` | `ExperimentManager(const ExperimentConfig& config)` | Constructor with configuration | `none` |
| `configure(config)` | `void configure(const ExperimentConfig& config)` | Configure experiment | `std::invalid_argument` |
| `getConfig()` | `const ExperimentConfig& getConfig() const` | Get experiment configuration | `none` |
| `runExperiment(experiment_id)` | `void runExperiment(const std::string& experiment_id)` | Run experiment | `std::runtime_error` |
| `runBatch(experiment_ids)` | `void runBatch(const std::vector<std::string>& experiment_ids)` | Run batch of experiments | `std::runtime_error` |
| `addResult(experiment_id, results)` | `void addResult(const std::string& experiment_id, const ExperimentResults& results)` | Add experiment results | `std::invalid_argument` |
| `getResult(experiment_id)` | `const ExperimentResults& getResult(const std::string& experiment_id) const` | Get experiment results | `std::out_of_range` |
| `compare(experiment_id1, experiment_id2)` | `ExperimentComparison compare(const std::string& experiment_id1, const std::string& experiment_id2) const` | Compare experiments | `std::out_of_range` |
| `analyzeAcross(experiment_ids)` | `AnalysisResults analyzeAcross(const std::vector<std::string>& experiment_ids) const` | Analyze across experiments | `std::invalid_argument` |
| `exportAllResults(filepath)` | `void exportAllResults(const std::string& filepath)` | Export all results | `std::runtime_error` |
| `generateReport()` | `std::string generateReport() const` | Generate experiment report | `none` |

## Factory Functions

### High-Level Convenience Functions

| Function | Signature | Description | Return Type |
|----------|-----------|-------------|-------------|
| `run_simulation()` | `bool run_simulation(const SimulationConfig& config)` | Run complete simulation | `bool` |
| `run_episode()` | `void run_episode(SimpleAgent& agent, IWorldInterface& world, size_t steps)` | Run single agent episode | `void` |
| `run_experiment()` | `void run_experiment(ExperimentManager& manager)` | Run scientific experiment | `void` |
| `create_brain_from_config()` | `Brain* create_brain_from_config(const BrainConfig& config)` | Create brain from config | `Brain*` |
| `create_brain_from_json()` | `Brain* create_brain_from_json(const std::string& json)` | Create brain from JSON | `Brain*` |
| `save_brain_state()` | `bool save_brain_state(Brain* brain, const std::string& filepath)` | Save brain state | `bool` |
| `load_brain_state()` | `bool load_brain_state(Brain* brain, const std::string& filepath)` | Load brain state | `bool` |
| `analyze_brain_activity()` | `BrainAnalysis analyze_brain_activity(Brain* brain)` | Analyze brain activity | `BrainAnalysis` |
| `compute_statistics()` | `StatisticalResults compute_statistics(const std::vector<double>& data)` | Compute statistics | `StatisticalResults` |
| `visualize_brain_state()` | `void visualize_brain_state(Brain* brain)` | Visualize brain state | `void` |

### Analysis Tool Factory Functions

| Function | Signature | Description | Return Type |
|----------|-----------|-------------|-------------|
| `createBrainProfiler()` | `std::unique_ptr<BrainProfiler> createBrainProfiler(Brain* brain)` | Create brain profiler | `std::unique_ptr<BrainProfiler>` |
| `createNeuronActivityMonitor()` | `std::unique_ptr<NeuronActivityMonitor> createNeuronActivityMonitor(Brain* brain)` | Create neuron activity monitor | `std::unique_ptr<NeuronActivityMonitor>` |
| `createNetworkConnectivityAnalyzer()` | `std::unique_ptr<NetworkConnectivityAnalyzer> createNetworkConnectivityAnalyzer(Brain* brain)` | Create network connectivity analyzer | `std::unique_ptr<NetworkConnectivityAnalyzer>` |
| `createMemorySystemProfiler()` | `std::unique_ptr<MemorySystemProfiler> createMemorySystemProfiler(Brain* brain)` | Create memory system profiler | `std::unique_ptr<MemorySystemProfiler>` |
| `createNeuromodulationAnalyzer()` | `std::unique_ptr<NeuromodulationAnalyzer> createNeuromodulationAnalyzer(Brain* brain)` | Create neuromodulation analyzer | `std::unique_ptr<NeuromodulationAnalyzer>` |
| `createDevelopmentStageAnalyzer()` | `std::unique_ptr<DevelopmentStageAnalyzer> createDevelopmentStageAnalyzer(Brain* brain)` | Create development stage analyzer | `std::unique_ptr<DevelopmentStageAnalyzer>` |
| `createVisualizationHelper()` | `std::unique_ptr<VisualizationHelper> createVisualizationHelper()` | Create visualization helper | `std::unique_ptr<VisualizationHelper>` |
| `createExperimentManager()` | `std::unique_ptr<ExperimentManager> createExperimentManager()` | Create experiment manager | `std::unique_ptr<ExperimentManager>` |

## Type Definitions

### Core Type Aliases

```cpp
typedef std::vector<double> TimeSeries;
typedef std::vector<std::vector<double>> Matrix;
typedef std::pair<size_t, double> TimeValuePair;
typedef std::map<std::string, double> MetricsMap;
typedef std::vector<std::string> StringList;
typedef std::vector<size_t> IndexList;
typedef std::vector<std::vector<size_t>> AdjacencyMatrix;
typedef std::unordered_map<size_t, std::vector<size_t>> AdjacencyList;
```

### Enumerations

#### Brain State
```cpp
enum class BrainState {
    Initializing,
    Running,
    Paused,
    Stopped,
    Error
};
```

#### Development Stage
```cpp
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

#### Action Types
```cpp
enum class ActionType {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact,
    Eat,
    Sleep,
    Explore
};
```

#### Neuron Types
```cpp
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Sensory,
    Motor,
    Modulatory,
    Internal
};
```

#### World Object Types
```cpp
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker,
    Food,
    Water,
    Shelter
};
```

### Struct Definitions

#### Brain Configuration
```cpp
struct BrainConfig {
    size_t neuron_count;
    double synapse_density;
    double connection_probability;
    double initial_weight_mean;
    double initial_weight_std;
    double v_thresh;
    double v_rest;
    double v_reset;
    double tau_mem;
    double tau_ref;
    
    // Default constructor
    BrainConfig() : neuron_count(1000), synapse_density(0.1),
                   connection_probability(0.05), initial_weight_mean(0.5),
                   initial_weight_std(0.1), v_thresh(-50.0), v_rest(-70.0),
                   v_reset(-75.0), tau_mem(20.0), tau_ref(2.0) {}
};
```

#### Simulation Configuration
```cpp
struct SimulationConfig {
    size_t steps;
    double dt;
    double max_time;
    std::string world_config;
    std::string brain_config;
    std::string plasticity_config;
    std::string neuromodulation_config;
    std::string development_config;
    
    // Default constructor
    SimulationConfig() : steps(1000), dt(0.1), max_time(100.0) {}
};
```

#### Brain Analysis
```cpp
struct BrainAnalysis {
    double average_firing_rate;
    size_t total_spikes;
    size_t active_neurons;
    double excitation_inhibition_ratio;
    std::vector<double> firing_rate_distribution;
    NetworkAnalysis network;
    MemoryAnalysis memory;
    NeuromodulationAnalysis neuromodulation;
    DevelopmentAnalysis development;
    TimeSeries firing_rate_time_series;
    
    // Calculate summary statistics
    double getStandardDeviation() const {
        return calculateStdDev(firing_rate_distribution);
    }
    
    double getPercentile(double percentile) const {
        return calculatePercentile(firing_rate_distribution, percentile);
    }
};
```

#### Neural Dynamics
```cpp
struct NeuralDynamicsAnalysis {
    double mean_firing_rate;
    double firing_rate_variance;
    double firing_rate_skewness;
    double firing_rate_kurtosis;
    std::vector<double> burst_intervals;
    std::vector<double> inter_spike_intervals;
    double coefficient_of_variation;
    bool has_bursts;
    
    // Burst detection
    bool detectBursts() {
        return !burst_intervals.empty() && burst_intervals.front() > 10.0;
    }
};
```

#### Network Analysis
```cpp
struct NetworkAnalysis {
    double clustering_coefficient;
    double characteristic_path_length;
    double small_worldness;
    double connection_density;
    std::vector<size_t> hub_neurons;
    std::map<size_t, std::vector<size_t>> community_structure;
    std::vector<std::vector<double>> adjacency_matrix;
    
    // Network metrics
    double calculateModularity() const {
        // Implementation of modularity calculation
    }
    
    double calculateBetweennessCentrality(size_t node_id) const {
        // Implementation of betweenness centrality
    }
};
```

## Error Handling

### Error Classes

#### Base NLM Error
```cpp
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
    
    virtual ErrorCode getErrorCode() const = 0;
};
```

#### Configuration Error
```cpp
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& key = "") 
        : NLMError(message), config_key(key) {}
    
    std::string getConfigKey() const { return config_key; }
    
private:
    std::string config_key;
};
```

#### Brain Error
```cpp
class BrainError : public NLMError {
public:
    enum class ErrorType {
        InitializationFailed,
        InvalidState,
        SimulationError,
        MemoryError
    };
    
    BrainError(const std::string& message, ErrorType type)
        : NLMError(message), error_type(type) {}
    
    ErrorType getErrorType() const { return error_type; }
    
private:
    ErrorType error_type;
};
```

### Error Handling Functions

```cpp
// Exception safety
void safe_operation() {
    try {
        // Critical operation
        brain.step(0);
    } catch (const ConfigurationError& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        // Recovery logic
    } catch (const BrainError& e) {
        std::cerr << "Brain error: " << e.what() << std::endl;
        // Recovery logic
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        // Error handling
    }
}

// Error recovery
bool recover_from_error(const std::exception& e) {
    if (dynamic_cast<const ConfigurationError*>(&e)) {
        // Recover from configuration error
        return restore_default_configuration();
    } else if (dynamic_cast<const BrainError*>(&e)) {
        // Recover from brain error
        return reset_brain();
    }
    return false;
}
```

## Memory Management

### Memory Allocation Strategies

#### Smart Pointers
```cpp
// Use smart pointers for automatic memory management
class AdvancedAPI {
private:
    std::unique_ptr<BrainProfiler> brain_profiler_;
    std::shared_ptr<SimulationRunner> sim_runner_;
    std::unique_ptr<SimpleAgent> agent_;
    
public:
    // Constructor
    AdvancedAPI(Brain* brain) {
        brain_profiler_ = std::make_unique<BrainProfiler>(brain);
        sim_runner_ = std::make_shared<SimulationRunner>(1000, 0.1);
    }
    
    // Destructor (automatically called)
    ~AdvancedAPI() {
        // Smart pointers automatically delete objects
    }
};
```

#### Memory Pools
```cpp
// Memory pool for efficient allocation
class MemoryPool {
public:
    template<typename T>
    T* allocate() {
        if (!pool_.empty()) {
            T* ptr = pool_.back();
            pool_.pop_back();
            return ptr;
        }
        return new T();
    }
    
    template<typename T>
    void deallocate(T* ptr) {
        pool_.push_back(ptr);
    }
    
    void clear() {
        pool_.clear();
    }
    
private:
    std::vector<void*> pool_;
};
```

### Memory Efficiency

```cpp
// Memory-efficient data structures
struct EfficientNeuralData {
    std::vector<float> membrane_potentials;  // Cache-friendly layout
    std::vector<uint8_t> firing_states;     // Use minimal memory
    std::vector<uint32_t> refractory_counters; // 32-bit counters
    
    // Constructor with efficient allocation
    EfficientNeuralData(size_t neuron_count) {
        membrane_potentials.resize(neuron_count);
        firing_states.resize(neuron_count);
        refractory_counters.resize(neuron_count);
    }
    
    // Calculate memory usage
    size_t getMemoryUsage() const {
        return sizeof(float) * membrane_potentials.size() +
               sizeof(uint8_t) * firing_states.size() +
               sizeof(uint32_t) * refractory_counters.size();
    }
};
```

## Thread Safety

### Thread-Safe Access

```cpp
class ThreadSafeBrain {
private:
    std::mutex mutex_;
    Brain brain_;
    
public:
    // Thread-safe method access
    void step(size_t step) {
        std::lock_guard<std::mutex> lock(mutex_);
        brain_.step(step);
    }
    
    double getFiringRate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return brain_.getAverageFiringRate();
    }
    
    // Read-only access (can be called concurrently)
    double getFiringRateReadOnly() const {
        return brain_.getAverageFiringRate();
    }
};
```

### Concurrent Simulation

```cpp
// Concurrent simulation with thread pool
class ConcurrentSimulation {
private:
    std::vector<std::thread> threads_;
    std::vector<Brain*> brains_;
    std::mutex mutex_;
    
public:
    void runConcurrentSimulation(size_t num_threads, size_t steps) {
        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back(&ConcurrentSimulation::simulation_worker, this, i, steps);
        }
        
        for (auto& thread : threads_) {
            thread.join();
        }
    }
    
private:
    void simulation_worker(size_t thread_id, size_t steps) {
        // Each thread gets its own brain instance
        BrainConfig config;
        config.neuron_count = 1000 / num_threads;
        
        Brain* brain = new Brain(config);
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Synchronize progress
            {
                std::lock_guard<std::mutex> lock(mutex_);
                // Update shared progress counter
            }
        }
        
        delete brain;
    }
};
```

## Summary

This API reference provides **complete documentation** of the NLM API with:

### ✅ Core API Coverage
1. **Brain Class** - Main neural network interface
2. **AgentBrain Class** - Agent-world interface
3. **SimpleWorld Class** - Environment interface
4. **Configuration Class** - Configuration management

### ✅ Advanced API Coverage
1. **AdvancedAPI Class** - High-level convenience functions
2. **SimpleAgent Class** - Advanced agent interface
3. **SimulationRunner Class** - Simulation management
4. **BrainProfiler Class** - Brain analysis tools
5. **VisualizationHelper Class** - Data visualization
6. **ExperimentManager Class** - Experiment management

### ✅ Factory Functions
1. **High-level convenience functions** for common tasks
2. **Analysis tool factory functions** for creating analysis tools
3. **Configuration factory functions** for creating configurations

### ✅ Type Definitions
1. **Core type aliases** for common data structures
2. **Enumerations** for categorical data
3. **Struct definitions** for complex data

### ✅ Error Handling
1. **Exception hierarchy** for error management
2. **Error recovery** mechanisms
3. **Exception safety** guarantees

### ✅ Memory Management
1. **Smart pointer usage** for automatic memory management
2. **Memory pool strategies** for efficiency
3. **Memory-efficient data structures** for performance

### ✅ Thread Safety
1. **Thread-safe access** methods
2. **Concurrent simulation** patterns
3. **Synchronization primitives** for shared resources

This API reference is designed for **comprehensive documentation** of NLM's API, providing developers with **complete understanding** of available interfaces, **best practices** for API usage, and **guidelines** for extending the API.

---

**Last Updated:** 2026-10-13
**Status:** Production Ready
**License:** Apache 2.0

For more detailed information, see:
- `NLM_IMPROVEMENTS_COMPLETE.md` - Implementation details
- `USAGE_GUIDE.md` - Usage examples and tutorials
- `ADVANCED_API_DOCUMENTATION.md` - Advanced API documentation
- `examples_comprehensive.py` - Complete example implementations