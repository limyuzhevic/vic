# NLM (熙然) - Neural Learning Machine Python API Documentation

**Comprehensive documentation for the NLM Python API**

## Overview

The NLM (熙然) project provides a comprehensive Python API for neural simulation and experimentation. This document covers all available functions, classes, and features for working with NLM.

## Installation

```bash
# Build NLM from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
brain.step(100)

# Get statistics
stats = pynlm.get_network_stats(brain)
print(f"Neurons: {stats['total_neurons']}, Spikes: {stats['total_spikes']}")
```

### Advanced Usage

```python
import pynlm

# Create a complete demo system
system = pynlm.createDemoSystem()
brain = system["brain"]
world = system["world"]
agent = system["agent"]

# Configure brain
pynlm.configure_brain(brain, neuronCount=500, regionCount=2)

# Enable all features
pynlm.enable_all_features(agent)

# Run simulation
pynlm.run_simulation(brain, world, agent, 1000)

# Analysis and visualization
stats = pynlm.get_network_stats(brain)
analysis = pynlm.analyze_brain(brain)
visualization = pynlm.get_visualization_data(brain)
```

## Core Classes

### Config Class

The Config class manages NLM configuration parameters.

#### Methods

- `config.loadFromFile(filepath)` - Load configuration from JSON file
- `config.loadFromArgs(argc, argv)` - Load configuration from command line
- `config.saveToFile(filepath)` - Save configuration to JSON file
- `config.has(key)` - Check if configuration key exists
- `config.getKeys()` - Get all configuration keys
- `config.clear()` - Clear all configuration entries
- `config.summary()` - Get configuration summary string

#### Configuration Keys

- `neuron_count` - Number of neurons
- `region_count` - Number of regions
- `connection_probability` - Connection probability between neurons
- `stdp_ltp_weight` - STDP long-term potentiation weight
- `stdp_ltd_weight` - STDP long-term depression weight
- `stdp_tau` - STDP time constant
- `synaptogenesis_rate` - Synaptogenesis rate
- `pruning_rate` - Pruning rate
- `simulation_timestep` - Simulation timestep duration
- `max_steps` - Maximum simulation steps
- `checkpoint_interval` - Checkpoint save interval
- `enable_replay` - Enable memory replay
- `enable_development` - Enable development system
- `reward_modulation` - Enable reward modulation
- `structural_plasticity` - Enable structural plasticity
- `development` - Enable development
- `curiosity` - Enable curiosity
- `novelty_sensitivity` - Novelty detection sensitivity
- `curiosity_threshold` - Curiosity activation threshold
- `world_width` - World width
- `world_height` - World height
- `vision_width` - Vision width
- `vision_height` - Vision height
- `max_energy` - Maximum world energy
- `energy_decay_rate` - World energy decay rate

### Brain Class

The Brain class represents the neural simulation brain.

#### Methods

- `brain.initialize()` - Initialize the brain
- `brain.step(currentStep)` - Perform simulation step
- `brain.step(currentStep, currentTime)` - Perform simulation step with timestamp
- `brain.receiveSensoryInput(input)` - Inject sensory input
- `brain.injectCurrent(neuron, current)` - Inject current into specific neuron
- `brain.injectCurrentToNeurons(type, current)` - Inject current into neurons of specific type
- `brain.produceAction()` - Produce motor action
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` - Save brain state to file
- `brain.load(filepath)` - Load brain state from file
- `brain.addRegion(name)` - Add neural region
- `brain.getRegion(id)` - Get region by ID
- `brain.getRegionCount()` - Get number of regions
- `brain.getRegionIds()` - Get all region IDs
- `brain.getRegions()` - Get all regions
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count
- `brain.getActiveNeuronCount()` - Get count of active neurons
- `brain.getFiringNeuronCount()` - Get count of firing neurons
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getExcitationInhibitionRatio()` - Get E/I balance ratio
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getDevelopmentalStage()` - Get developmental stage
- `brain.setDevelopmentalStage(stage)` - Set developmental stage
- `brain.getConfig()` - Get configuration
- `brain.logStatus()` - Log brain status

#### Statistics

- `getTotalNeuronCount()` - Total number of neurons
- `getTotalSynapseCount()` - Total number of synapses
- `getActiveNeuronCount()` - Count of active neurons
- `getFiringNeuronCount()` - Count of currently firing neurons
- `getAverageFiringRate()` - Average firing rate across neurons
- `getExcitationInhibitionRatio()` - Excitation/inhibition balance ratio
- `getTotalSpikeCount()` - Total spike count in simulation
- `getDevelopmentalStage()` - Current developmental stage (0-4)

### AgentBrain Class

The AgentBrain class connects NLM brain to the world.

#### Methods

- `agent.initialize(world)` - Initialize with world
- `agent.getSensoryInputSize()` - Get expected sensory input size
- `agent.getMotorOutputSize()` - Get expected motor output size
- `agent.processSensoryInput(percept)` - Process sensory input and inject into brain
- `agent.decodeMotorCommand()` - Decode brain activity into motor command
- `agent.applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)` - Update development system
- `agent.getDevelopmentalStage()` - Get current developmental stage
- `agent.getNeuromodulationLevel()` - Get current neuromodulation level
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error
- `agent.reset()` - Reset agent for new episode
- `agent.getBrain()` - Get underlying brain
- `agent.enableRewardModulation(enable)` - Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `agent.enableDevelopment(enable)` - Enable/disable development
- `agent.enableCuriosity(enable)` - Enable/disable curiosity
- `agent.isRewardModulationEnabled()` - Check if reward modulation enabled
- `agent.isStructuralPlasticityEnabled()` - Check if structural plasticity enabled
- `agent.isDevelopmentEnabled()` - Check if development enabled
- `agent.isCuriosityEnabled()` - Check if curiosity enabled

### SimpleWorld Class

The SimpleWorld class represents the simulation world.

#### Methods

- `world.configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `world.reset()` - Reset world state
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.update(timestep)` - Update world by timestep
- `world.applyMotorCommand(cmd, currentTime)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept from world
- `world.getAgentBody()` - Get agent body state
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove object at position
- `world.isValidPosition(x, y)` - Check if position is valid
- `world.getWidth()` - Get world width
- `world.getHeight()` - Get world height
- `world.getMaxEnergy()` - Get max energy
- `world.setMaxEnergy(e)` - Set max energy
- `world.getEnergyDecayRate()` - Get energy decay rate
- `world.setEnergyDecayRate(r)` - Set energy decay rate
- `world.getSimulationTime()` - Get current simulation time
- `world.setRandomSeed(seed)` - Set random seed
- `world.getRandomSeed()` - Get random seed

## Core Functions

### Basic Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with configuration
- `createSimpleBrain()` - Create simple brain with defaults
- `create_brain_with_config(file)` - Create brain with config file
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain interface

### Configuration

- `configure_brain(brain, neurons, regions, conn_prob)` - Configure brain parameters
- `enable_features(agent, reward_mod, structural, development, curiosity)` - Enable features
- `enable_all_features(agent)` - Enable all agent features

### Simulation

- `run_simulation(brain, world, agent, steps)` - Run simulation
- `batch_step(brain, steps)` - Run multiple simulation steps efficiently

### Data Access

- `get_network_stats(brain)` - Get network statistics
- `analyze_brain(brain)` - Analyze brain performance
- `get_brain_stats(brain)` - Alias for get_network_stats
- `get_system_stats(system)` - Get demo system statistics
- `get_neural_diagnostics(brain)` - Get neural system diagnostics

### I/O Operations

- `checkpoint_brain(brain, file)` - Save brain state
- `restore_brain(brain, file)` - Load brain state
- `test_brain_functionality(brain)` - Test brain functionality

### Demo System

- `createDemoSystem()` - Create complete demo system
- `run_demo_simulation(system, steps)` - Run demo simulation

### Advanced Visualization

- `get_visualization_data(brain)` - Get complete visualization data
- `visualize_complete_state(brain)` - Get complete state for coordinated visualization
- `visualize_network(brain)` - Visualize network connectivity
- `visualize_activity(brain)` - Visualize neural activity patterns
- `visualize_development(brain)` - Visualize developmental progression
- `visualize_memory(brain)` - Visualize memory system state
- `visualize_neuromodulation(brain)` - Visualize neuromodulation system
- `visualize_prediction(brain)` - Visualize prediction system state

### Specific Data Access

- `get_activity_data(brain)` - Get neural activity data
- `get_network_data(brain)` - Get network connectivity data
- `get_memory_data(brain)` - Get memory system data
- `get_neuromodulation_data(brain)` - Get neuromodulation system data
- `get_prediction_data(brain)` - Get prediction system data
- `get_development_data(brain)` - Get developmental system data

### System Information

- `get_system_version()` - Get system version
- `get_system_capabilities()` - Get available capabilities
- `get_feature_list()` - Get list of all features
- `get_summary()` - Get system summary

### Diagnostics

- `test_integration(brain, world, agent, steps)` - Test system integration
- `check_system_health(brain, world, agent)` - Check system health
- `get_sensory_diagnostics(agent)` - Get sensory system diagnostics
- `get_motor_diagnostics(agent)` - Get motor system diagnostics
- `get_world_diagnostics(world)` - Get world environment diagnostics

### Help System

- `help()` - Show comprehensive help documentation
- `helpBrain()` - Get Brain class help documentation
- `helpAgent()` - Get AgentBrain class help documentation
- `helpWorld()` - Get SimpleWorld class help documentation
- `helpConfig()` - Get Config class help documentation
- `helpViz()` - Get visualization help documentation
- `helpExperimental()` - Get experimental features help documentation
- `helpCLI()` - Get CLI help documentation
- `helpAdvanced()` - Get advanced features help documentation
- `getAllHelp()` - Get complete API help documentation
- `showHelp(topic)` - Show help documentation for specific topic
- `generateDocumentation()` - Generate complete API documentation

## Advanced Features

### Experiment Management

The `advanced_nlm` module provides advanced functionality for power users:

#### ExperimentManager

- `ExperimentManager(name)` - Create experiment manager
- `run_experiment(brain, world, agent, steps, description, tags)` - Run experiment with tracking
- `save_results(filepath)` - Save experiment results to JSON
- `compare_experiments(names)` - Compare results across experiments

#### PerformanceProfiler

- `PerformanceProfiler()` - Create performance profiler
- `profile_simulation(brain, world, agent, steps)` - Profile simulation run
- `get_performance_report()` - Generate comprehensive performance report

#### ScriptRecorder

- `ScriptRecorder()` - Create script recorder
- `start_recording(name, description)` - Start recording a script
- `record_step(brain, world, agent, event_type)` - Record a simulation step
- `stop_recording()` - Stop recording and save script
- `save_script(filepath)` - Save current script to file
- `load_script(filepath)` - Load script from file
- `replay_script(script_data, world, agent)` - Replay recorded script

## Visualization

NLM provides comprehensive visualization tools:

### Complete Visualization

- `visualize_complete_state(brain)` - Get complete brain state for coordinated visualization

### System Components

- `visualize_network(brain)` - Get network connectivity visualization data
- `visualize_activity(brain)` - Get neural activity visualization data
- `visualize_development(brain)` - Get developmental progression visualization
- `visualize_memory(brain)` - Get memory system visualization data
- `visualize_neuromodulation(brain)` - Get neuromodulation system visualization
- `visualize_prediction(brain)` - Get prediction system visualization data

### Data Access

- `get_visualization_data(brain)` - Get complete visualization data for all systems
- `get_activity_data(brain)` - Get neural activity patterns
- `get_network_data(brain)` - Get network connectivity information
- `get_memory_data(brain)` - Get memory system state
- `get_neuromodulation_data(brain)` - Get neuromodulation system data
- `get_prediction_data(brain)` - Get prediction system data
- `get_development_data(brain)` - Get developmental system data

## Error Handling

NLM provides comprehensive error handling:

### Exception Handling

All NLM Python functions include proper error handling with descriptive error messages.

### Common Errors and Solutions

- `"Brain not initialized"` - Call `brain.initialize()` before using the brain
- `"World not configured"` - Call `world.configure()` with appropriate dimensions
- `"Agent not initialized"` - Call `agent.initialize(world)` after creating agent
- `"Invalid configuration"` - Check configuration file syntax and required keys
- `"File not found"` - Ensure the file exists and has proper permissions
- `"Memory usage too high"` - Reduce brain size or enable memory optimization features
- `"Performance too slow"` - Reduce simulation steps or optimize brain configuration
- `"Low stability"` - Adjust plasticity parameters or increase brain size
- `"Low adaptability"` - Enable curiosity system or adjust neuromodulation
- `"Low activity"` - Increase input strength or adjust sensory parameters

## Performance Optimization

### Tips for Optimal Performance

1. **Use appropriate brain sizes**: Start with smaller brains for testing, then scale up
2. **Configure for your use case**: Use configurations appropriate for your specific application
3. **Monitor resource usage**: Use performance profiling to identify bottlenecks
4. **Save intermediate results**: Avoid recomputation by saving intermediate results
5. **Use batch processing**: Use `batch_step()` for large simulations
6. **Enable optimizations**: Use all learning features for better performance

### Performance Profiling

Use the `PerformanceProfiler` class to analyze and optimize simulation performance:

```python
from advanced_nlm import PerformanceProfiler

profiler = PerformanceProfiler()
metrics = profiler.profile_advanced_simulation(brain, world, agent, 2000)
report = profiler.get_performance_report()
```

## Experiment Management

### Advanced Experiment Features

NLM provides comprehensive experiment management tools:

#### Automated Experimentation

```python
from advanced_nlm import create_advanced_simulation, run_advanced_experiment, profile_advanced_simulation

# Create advanced simulation
simulation = create_advanced_simulation()
brain = simulation["brain"]
world = simulation["world"]
agent = simulation["agent"]

# Run experiment with tracking
result = run_advanced_experiment(
    "advanced_experiment",
    brain, world, agent, 5000,
    description="Testing neural plasticity",
    tags=["advanced", "plasticity", "development"]
)

# Profile performance
profiler = PerformanceProfiler()
metrics = profiler.profile_advanced_simulation(brain, world, agent, 5000)
```

#### Experiment Comparison

```python
# Compare multiple experiments
comparison = experiment.compare_experiments(["exp1", "exp2", "exp3"])

# Get performance rankings
performance_ranking = comparison["metrics"]["performance_ranking"]
health_ranking = comparison["metrics"]["health_ranking"]
```

## Configuration Management

### JSON Configuration Files

NLM supports sophisticated JSON configuration management:

#### Example Configuration

```json
{
    "brain": {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "checkpoint_interval": 1000,
        "enable_replay": true,
        "enable_development": true
    },
    "agent": {
        "reward_modulation": true,
        "structural_plasticity": true,
        "development": true,
        "curiosity": true,
        "novelty_sensitivity": 0.5,
        "curiosity_threshold": 0.3
    },
    "world": {
        "width": 16,
        "height": 16,
        "vision_width": 8,
        "vision_height": 8,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    }
}
```

### Configuration Loading

```python
import json

# Load configuration from JSON file
with open("advanced_config.json", 'r') as f:
    config = json.load(f)

# Apply configuration
pynlm.configure_brain(brain,
                     neuronCount=config["brain"]["neuron_count"],
                     regionCount=config["brain"]["region_count"],
                     connectionProb=config["brain"]["connection_probability"])

# Apply agent configuration
agent.enableRewardModulation(config["agent"]["reward_modulation"])
agent.enableStructuralPlasticity(config["agent"]["structural_plasticity"])
agent.enableDevelopment(config["agent"]["development"])
agent.enableCuriosity(config["agent"]["curiosity"])
```

## Best Practices

### 1. Always Profile Your Simulations

```python
from advanced_nlm import PerformanceProfiler

profiler = PerformanceProfiler()
metrics = profiler.profile_advanced_simulation(brain, world, agent, 2000)
report = profiler.get_performance_report()

# Check for performance issues
issues = profiler._identify_issues()
recommendations = profiler._generate_recommendations(issues)
```

### 2. Use Appropriate Configurations

```python
# Start with small brain for testing
pynlm.configure_brain(brain, neuronCount=100, regionCount=1)

# Scale up for production
pynlm.configure_brain(brain, neuronCount=1000, regionCount=3)
```

### 3. Monitor Resource Usage

```python
# Get comprehensive network statistics
stats = pynlm.get_network_stats(brain)
print(f"Memory usage: {stats.get('memory_usage_mb', 0):.1f} MB")
print(f"CPU time: {stats.get('cpu_time_ms', 0):.1f} ms")
```

### 4. Save Intermediate Results

```python
# Save progress regularly
if step % 1000 == 0:
    pynlm.checkpoint_brain(brain, f"progress_step_{step}.bin")
```

### 5. Use Batch Processing for Large Simulations

```python
# Run many steps efficiently
pynlm.batch_step(brain, 10000)  # Run 10,000 steps
```

### 6. Document Your Experiments

```python
from advanced_nlm import ExperimentManager

experiment = ExperimentManager("my_experiment")
result = experiment.run_experiment(
    brain, world, agent, 2000,
    description="My neural experiment",
    tags=["neural", "plasticity", "learning"]
)
experiment.save_results("results/my_experiment.json")
```

### 7. Compare Configurations

```python
# Test different configurations
configs = [
    {"neuron_count": 100, "region_count": 1},
    {"neuron_count": 500, "region_count": 2},
    {"neuron_count": 1000, "region_count": 3}
]

for i, config in enumerate(configs):
    result = experiment.run_experiment(
        brain, world, agent, 1000,
        description=f"Configuration {i+1}: {config['neuron_count']} neurons",
        tags=[f"config_{i+1}"]
    )
    experiment.save_results(f"results/config_{i+1}.json")
```

### 8. Use Version Control

```bash
# Use git to track experiment results
git init experiment_results
git add results/
git commit -m "Add experiment results from neural plasticity study"
```

### 9. Automate Repetitive Tasks

```python
def run_repeated_experiments(base_config, iterations):
    from advanced_nlm import ExperimentManager
    
    experiment = ExperimentManager("repeated_experiments")
    
    for i in range(iterations):
        # Modify configuration for each iteration
        config = base_config.copy()
        config["brain"]["neuron_count"] = config["brain"]["neuron_count"] + i * 100
        
        result = experiment.run_experiment(
            brain, world, agent, 2000,
            description=f"Iteration {i+1}",
            tags=[f"iteration_{i+1}"]
        )
        
        experiment.save_results(f"results/iteration_{i+1}.json")
    
    return experiment
```

### 10. Share Your Results

```python
# Share experiment results with the community
experiment.save_results("public_results/my_experiment.json")
print("Results shared! Visit the NLM portal to view and compare with others.")
```

## Getting Help

### Online Resources

- **Documentation**: https://docs.nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Examples**: https://github.com/nlm-project/nlm-examples

### Support

- **Bug Reports**: Please report bugs through GitHub issues
- **Feature Requests**: Suggest new features through GitHub discussions
- **Questions**: Ask questions on GitHub discussions or Stack Overflow
- **Help**: Get help from the NLM community

### Community

- **Discord**: Join the NLM Discord server
- **Slack**: Join the NLM Slack community
- **Forums**: Participate in the NLM forums
- **Social Media**: Follow NLM on social media

## Acknowledgements

NLM is made possible by:

- The open-source community
- Contributors and maintainers
- Users and testers
- Funding agencies
- Research institutions
- Educational partners

## License

MIT License

## Copyright

Copyright (c) 2024 NLM Project Authors

---

*This documentation is continuously updated. Please check for the latest version.*
