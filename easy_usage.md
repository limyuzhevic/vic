# NLM - Enhanced Usage Guide for All Users

## Welcome to NLM (熙然)

NLM (Neural Learning Machine) is an experimental computational brain project that simulates biological neural systems. The system has evolved through multiple phases, with Phase 6 representing the final integration of all advanced features.

## User Levels and Access

### Beginner Level (Phase 1-3)
- **Easy Usage**: Simple brain simulation with basic neural computation
- **Standard Usage**: Real spiking neurons with plasticity and structural changes
- **World Interaction**: Agent-based world navigation

### Intermediate Level (Phase 4)
- **Standard Usage**: Advanced cognition with prediction, memory, attention
- **Memory Integration**: Working memory, episodic memory, concept formation
- **Planning Systems**: Multi-step action planning

### Expert Level (Phase 6)
- **Advanced Usage**: Meta-learning, self-repair, autonomous exploration
- **Performance Optimization**: Real-time monitoring, profiling, optimization
- **Custom Scenarios**: Ablation studies, custom experiments, benchmarking
- **Research Tools**: Comprehensive analysis, visualization, export capabilities

## Quick Start Examples

### Example 1: Beginner - Simple Brain (Phase 1-3)
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run basic neural computation
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Spike count: {brain.getTotalSpikeCount()}")
```

### Example 2: Standard Usage - Agent in World (Phase 3-4)
```python
import pynlm

# Setup brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Agent completed {brain.getTotalSpikeCount()} steps")
```

### Example 3: Expert - Advanced Experiment (Phase 6)
```python
import pynlm
from pynlm import EnhancedPhase6IntegratedExperiment, AdvancedPhase6Config

# Create enhanced experiment
experiment = EnhancedPhase6IntegratedExperiment()

# Configure advanced experiment
config = AdvancedPhase6Config()
config.baseConfig.maxSteps = 2000
config.baseConfig.neuronCount = 1000
config.baseConfig.enableCheckpointing = true
config.baseConfig.enableReplay = true

# Enable advanced features
config.enableRealTimeMonitoring = true
config.enablePerformanceProfiling = true
config.enableMetaLearning = true
config.enableSelfTuning = true
config.enableAdvancedAnalysis = true

# Run advanced experiment
result = experiment.runAdvanced(config)

# Analyze results
print(f"Learning progress: {result.learningProgressScore}/10")
print(f"Neural efficiency: {result.neuralEfficiencyScore}/10")
print(f"Behavioral adaptation: {result.behavioralAdaptationScore}/10")
print(f"Survival fitness: {result.survivalFitnessScore}/10")
```

## Advanced Features for Expert Users

### 1. Meta-Learning and Self-Optimization
```python
# Enable meta-learning
agent.enableMetaLearning(True)
agent.enableSelfTuning(True)

# Set learning strategies
agent.setLearningStrategy("adaptive")
agent.setExplorationRate(0.2)

# Monitor learning progress
progress = agent.getLearningProgress()
adaptation = agent.getAdaptationCurve()
```

### 2. Enhanced Configuration System
```python
# Create advanced configuration
from pynlm import Config

config = Config()
config.loadFromJSON("advanced_config.json")

# Set advanced parameters
config.set("learning_strategy", "meta_adaptive")
config.set("enable_self_repair", True)
config.set("enable_autonomous_exploration", True)
config.set("enable_meta_cognition", True)

# Apply presets
config.applyPreset("research")
config.applyPreset("benchmark")
```

### 3. Advanced Analysis and Monitoring
```python
# Enable real-time monitoring
experiment.enableRealTimeMonitoring(True)
experiment.enablePerformanceProfiling(True)

# Configure analysis metrics
experiment.addAnalysisMetric("spike_rate")
experiment.addAnalysisMetric("firing_patterns")
experiment.addAnalysisMetric("novelty")
experiment.addAnalysisMetric("curiosity")

# Get monitoring data
data = experiment.getRealTimeData()
metrics = experiment.getCurrentPerformanceMetrics()
```

### 4. Custom Scenarios and Ablation Studies
```python
# Run custom scenario
experiment = EnhancedPhase6IntegratedExperiment()
result = experiment.runCustomScenario("memory_study", config)

# Run ablation study
result = experiment.runAblationStudy("neuromodulation", config)

# Export results
experiment.exportResults("experiment_results.json", result)
experiment.exportAnalysis("analysis_data.json", result.detailedAnalysis)
```

### 5. Advanced Neural Control
```python
# Force specific actions
agent.forceAction(pynlm.MotorCommand.MoveForward)

# Set motor decoding modes
agent.setMotorDecodingMode(1)  # Weighted decoding
agent.setMotorDecodingMode(2)  # Fuzzy logic

# Configure sensory scaling
agent.setSensoryScaling(2.0, 3.0, 2.5, 1.5)  # Enhanced sensitivity

# Set curiosity thresholds
agent.setCuriosityThreshold(0.2)  # Lower threshold for exploration
agent.setExplorationRate(0.3)    # Higher exploration rate
```

## Performance Optimization

### Monitoring and Profiling
```python
# Enable comprehensive monitoring
experiment.enableMemoryProfiling(True)
experiment.enableNeuralProfiling(True)
experiment.enableSynapticAnalysis(True)
experiment.enableCircuitAnalysis(True)

# Set sample sizes
experiment.setMemorySampleSize(5000)

# Analyze performance
performance = experiment.benchmarkPerformance(config)
memory_usage = experiment.getMemoryUsageHistory()
memory_leaks = experiment.analyzeMemoryLeaks()
```

### Advanced Learning Configuration
```python
# Configure learning parameters
config.learningStrategy = "meta_adaptive"
config.learningRate = 0.02
config.explorationRate = 0.15
config.enableTransferLearning = True
config.enableMetaLearning = True
config.enableSelfTuning = True

# Configure development
config.enableDevelopment(True)
config.developmentalStages = [60.0, 300.0, 900.0]  # Custom stage timing
```

## Export and Analysis

### Export Results
```python
# Export experiment data
experiment.exportResults("experiment_results.json", result)
experiment.exportAnalysis("analysis_data.json", result.detailedAnalysis)
experiment.exportPerformance("performance_metrics.json", result.performanceMetrics)

# Export in different formats
experiment.exportResultsCSV("results.csv")
experiment.exportResultsJSON("results.json")
experiment.exportResultsPDF("report.pdf")
experiment.exportResultsHTML("report.html")
```

### Analysis Functions
```python
# Analyze spike trains
spike_analysis = experiment.analyzeSpikeTrain(spike_times)

# Analyze behavior
behavior_analysis = experiment.analyzeBehavior(actions)

# Analyze learning
learning_analysis = experiment.analyzeLearning(rewards)

# Analyze memory
memory_analysis = experiment.analyzeMemory(memory_scores)
```

## Configuration Presets

### Available Presets
```python
from pynlm import AdvancedExperiment

# Quick test configuration
quick_test = AdvancedExperiment.getQuickTestConfig()

# Production configuration
production = AdvancedExperiment.getProductionConfig()

# Research configuration
research = AdvancedExperiment.getResearchConfig()

# Benchmark configuration
benchmark = AdvancedExperiment.getBenchmarkConfig()

# Apply preset
config = AdvancedExperiment.createAdvancedConfig("research")
```

### Custom Presets
```python
# Create custom preset
custom_preset = AdvancedExperiment.ConfigPreset()
custom_preset.name = "my_research"
custom_preset.description = "My custom research configuration"
custom_preset.isSystem = False

custom_preset.settings = {
    "neuron_count": 5000,
    "learning_strategy": "meta_adaptive",
    "enable_self_repair": True,
    "enable_autonomous_exploration": True,
    "enable_meta_cognition": True,
    "enable_advanced_analysis": True,
    "enable_real_time_monitoring": True
}

# Add and apply preset
config.addPreset(custom_preset)
config.applyPreset("my_research")
```

## Quick Reference

### Core Classes
- **Brain**: Central neural simulation brain
- **AgentBrain**: Advanced agent brain interface
- **SimpleWorld**: Advanced world simulation
- **SensoryPercept**: Enhanced sensory data
- **Action**: Advanced action representation
- **Config**: Advanced configuration system

### Key Methods (AgentBrain)
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted)`: Apply reward
- `updateDevelopment(timestep)`: Update development
- `forceAction(command)`: Force specific action
- `addSensoryPattern(name, pattern)`: Add sensory pattern

### Key Methods (Config)
- `loadFromJSON(jsonString)`: Load from JSON
- `loadFromFile(filepath)`: Load from file
- `saveToJSON(filepath)`: Save to JSON
- `set(key, value)`: Set configuration value
- `getOr(key, default)`: Get value with default
- `applyPreset(name)`: Apply configuration preset
- `validate()`: Validate configuration

### Key Methods (EnhancedPhase6IntegratedExperiment)
- `runAdvanced(config)`: Run advanced experiment
- `runCustomScenario(name, config)`: Run custom scenario
- `runAblationStudy(target, config)`: Run ablation study
- `benchmarkPerformance(config)`: Benchmark performance
- `exportAnalysis(filepath, analysis)`: Export analysis
- `exportResults(filepath, result)`: Export results
- `getRealTimeData()`: Get real-time data

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"
- **Solution**: Ensure you call `brain.initialize()` before simulation
- **Solution**: Check that world.update() is being called
- **Solution**: Enable logging for debugging

#### "Agent isn't moving"
- **Solution**: Ensure world.applyMotorCommand() is being called
- **Solution**: Check that agent.decodeMotorCommand() returns actions
- **Solution**: Enable agent debugging output

#### "Everything is 0"
- **Solution**: Brains need time to "warm up" - try more steps
- **Solution**: Ensure sensory input is connected
- **Solution**: Check neural parameters are reasonable

#### "Performance issues"
- **Solution**: Reduce simulation parameters
- **Solution**: Enable performance monitoring
- **Solution**: Use configuration presets

#### "Configuration errors"
- **Solution**: Check JSON/YAML syntax
- **Solution**: Use preset configurations
- **Solution**: Validate configuration before use

### Debugging Tips
```python
# Enable detailed logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Enable experiment debugging
experiment.enableDetailedLogging(True)
experiment.enableRealTimeMonitoring(True)

# Check memory usage
experiment.enableMemoryProfiling(True)
memory_usage = experiment.getMemoryUsageHistory()

# Validate configuration
config = AdvancedExperiment.createAdvancedConfig("research")
if not config.validate():
    print("Configuration validation failed")
    errors = config.getValidationErrors()
    for error in errors:
        print(f"Error: {error}")
```

## Next Steps

### For Beginners
1. Start with the simple examples in `easy_usage.md`
2. Try the standard agent example
3. Experiment with different world configurations
4. Study the neural dynamics output

### For Intermediate Users
1. Explore the Phase 4 cognition systems
2. Implement custom sensory processing
3. Study memory formation and consolidation
4. Develop custom action strategies

### For Expert Users
1. Implement meta-learning algorithms
2. Create custom ablation studies
3. Develop performance optimization routines
4. Publish analysis tools and visualizations

## Resources

### Documentation
- `docs/ARCHITECTURE.md`: Detailed architecture documentation
- `docs/ROADMAP.md`: Development roadmap
- `docs/SCIENCE.md`: Scientific background and theories
- `docs/EXPERIMENTS.md`: Experiment descriptions and results

### Code Examples
- `examples/simple_brain.py`: Simple brain simulation
- `examples/agent_world.py`: Agent in world interaction
- `examples/advanced_experiment.py`: Advanced experiment setup

### API Reference
- `python/bindings.cpp`: Complete Python bindings API
- `src/agent/AgentBrain.hpp`: AgentBrain API reference
- `src/core/Config/Config.hpp`: Config API reference

## Support and Community

For technical support, visit the NLM GitHub repository or join the NLM community forum. Share your experiments, ask questions, and contribute to the growing NLM ecosystem.

---

**Version**: Phase 6 Enhanced Edition  
**License**: MIT  
**Documentation**: https://nlm.readthedocs.io  
**Repository**: https://github.com/nlm-project/nlm  
**Community**: https://discord.gg/nlm  

This enhanced documentation provides comprehensive guidance for users at all levels, from beginners to advanced researchers, to effectively use and extend the NLM Phase 6 system.
