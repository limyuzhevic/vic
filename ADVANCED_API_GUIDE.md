# NLM Advanced Python API - Comprehensive Examples

This document provides comprehensive examples for all the advanced features added to the NLM Python bindings.

## 1. Advanced Configuration Examples

### 1.1 JSON Configuration Loading

```python
import pynlm
import json

# Method 1: Load from JSON string
config_data = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "connection_probability": 0.08
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.002,
            "time_constant": 25.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.01
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": True,
            "novelty_weight": 0.6,
            "prediction_error_weight": 0.4
        },
        "dopamine": {
            "scale": 1.2
        }
    }
}

config_json = json.dumps(config_data)
config = pynlm.createDefaultConfig()
config.loadFromJSON(config_json, validate=True)

print(f"Loaded config with {len(config.getKeys())} settings")
print(f"Brain neuron count: {config.get('brain.neuron_count')}")

# Method 2: Save configuration
config.saveToJSON(config_json)
with open("my_config.json", "w") as f:
    f.write(config_json)

print("Configuration saved to file")
```

### 1.2 Configuration Validation

```python
import pynlm

# Create config
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Validate with custom error message
error_msg = ""
if config.validate(error_msg):
    print("Configuration is valid!")
else:
    print(f"Configuration validation failed: {error_msg}")

# Create a schema and validate
from pynlm import ConfigurationSchema

schema = ConfigurationSchema()
# Add schema parameters...
config.setSchema(schema)

if config.validate(error_msg):
    print("Configuration conforms to schema!")
else:
    print(f"Schema validation failed: {error_msg}")

# Get validation report
report = config.getValidationReport()
print("Validation report:\n" + report)
```

### 1.3 Configuration with Source Tracking

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set values with different sources
config.setWithSource("learning_rate", 0.01, pynlm.ConfigSource.File)
config.setWithSource("brain.neuron_count", 1000, pynlm.ConfigSource.Default)
config.setWithSource("simulation_timestep", 0.1, pynlm.ConfigSource.Runtime)

# Check source of values
for key in ["learning_rate", "brain.neuron_count", "simulation_timestep"]:
    # Access source through internal API (if available)
    print(f"Key '{key}' has source information")
```

## 2. Advanced Visualization Examples

### 2.1 Multi-Modal Visualization Pipeline

```python
import pynlm
import time

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Create visualization pipeline
brain.createVisualizationPipeline()
brain.getVisualizationInterface().setUpdateRate(10.0)

print("Created visualization pipeline")

# Run simulation with visualization
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Update visualization
    brain.getVisualizationInterface().update()
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Export visualization
brain.getVisualizationInterface().exportVisualization("simulation.gif")
print("Visualization exported to simulation.gif")

# Get visualization statistics
stats = brain.getVisualizationInterface().getStatistics()
print(stats)
```

### 2.2 Network Analysis

```python
import pynlm

# Create brain with enough neurons for analysis
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to generate network activity
for step in range(500):
    brain.step(step)

# Analyze network connectivity
brain.analyzeConnectivity()

# Compute graph metrics
metrics = brain.computeGraphMetrics()
print("Network Analysis Results:")
print(f"  Number of connected components: {metrics.get('connected_components', 0)}")
print(f"  Network efficiency: {metrics.get('efficiency', 0):.4f}")
print(f"  Clustering coefficient: {metrics.get('clustering_coefficient', 0):.4f}")
print(f"  Average path length: {metrics.get('avg_path_length', 0):.4f}")

# Identify hubs and communities
brain.createEmbedding(2, embedding)  # 2D embedding for visualization
print("Created 2D embedding for visualization")

# Detect hubs
hubs = brain.getAdvancedSTDP().detectHubs(threshold=0.8)
print(f"Found {len(hubs)} network hubs")
```

## 3. Advanced Learning & Plasticity Examples

### 3.1 Reward-Modulated STDP

```python
import pynlm

# Create brain with reward-modulated STDP
config = pynlm.createDefaultConfig()
config.set("plasticity.advanced_stdp.reward_modulated", True)
config.set("plasticity.advanced_stdp.reward_scale", 2.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced plasticity
brain.enableAdvancedPlasticity(True)

# Get the advanced STDP instance
stdp = brain.getAdvancedSTDP()
stdp.enableRewardModulatedSTDP(True)
stdp.setRewardScale(1.5)

print("Enabled reward-modulated STDP")

# Run simulation with rewards
for step in range(200):
    brain.step(step)
    
    # Apply reward modulation
    reward = 0.1 if step % 50 == 0 else 0.0
    brain.applyRewardModulation(reward)
    
    if step % 50 == 0:
        print(f"Step {step}: Reward applied: {reward}")

print("Reward-modulated STDP simulation completed")
```

### 3.2 Triplet STDP

```python
import pynlm

# Create brain with triplet STDP
config = pynlm.createDefaultConfig()
config.set("plasticity.advanced_stdp.triplet_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable triplet STDP
brain.enableTripletSTDP(True)

# Configure triplet parameters
tdp = brain.getAdvancedSTDP()
tdp.enableTripletSTDP(True)
tdp.setTripletWeights(A1=0.01, A2=0.005, B1=0.02, B2=0.01)

print("Enabled triplet STDP with weights: A1=0.01, A2=0.005, B1=0.02, B2=0.01")

# Run simulation
for step in range(300):
    brain.step(step)
    
    if step % 50 == 0:
        print(f"Step {step}: Firing rate: {brain.getAverageFiringRate():.2f}")
```

### 3.3 Curiosity-Driven Learning

```python
import pynlm

# Create agent with curiosity
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()
agent.initialize(world)

# Enable curiosity with fine-tuning
agent.enableCuriosity(True)
agent.setCuriosityParameters(novelty_weight=0.7, prediction_error_weight=0.3)

print("Enabled curiosity-driven learning (novelty=0.7, pred_error=0.3)")

# Run exploration episode
exploration_steps = []
for step in range(500):
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Get action and apply
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record curiosity level
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    
    exploration_steps.append({
        'step': step,
        'curiosity': curiosity_level,
        'novelty': novelty_level,
        'action': str(action)
    })
    
    if step % 50 == 0:
        print(f"Step {step}: Curiosity={curiosity_level:.3f}, Novelty={novelty_level:.3f}")

# Analyze exploration pattern
average_curiosity = sum(s['curiosity'] for s in exploration_steps) / len(exploration_steps)
max_curiosity = max(s['curiosity'] for s in exploration_steps)

print(f"Exploration analysis:")
print(f"  Average curiosity: {average_curiosity:.3f}")
print(f"  Maximum curiosity: {max_curiosity:.3f}")
print(f"  Total steps explored: {len(exploration_steps)}")
```

## 4. Batch Processing Examples

### 4.1 Batch Simulation

```python
import pynlm
import time

# Create batch processor
batch = pynlm.createBatchProcessor()

# Add different configurations
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 1000)
batch.addConfiguration(config1, "small_brain")

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 5000)
config2.set("brain.synapse_density", 0.2)
batch.addConfiguration(config2, "large_brain")

config3 = pynlm.createDefaultConfig()
config3.set("brain.neuron_count", 2000)
config3.set("brain.synapse_density", 0.05)
config3.set("plasticity.stdp.learning_rate", 0.005)
batch.addConfiguration(config3, "sparse_brain")

print(f"Created batch with {len(batch.getConfigurations())} configurations")

# Create world template
world_template = pynlm.createSimpleWorld()
world_template.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Run batch simulation
print("Running batch simulation...")
start_time = time.time()

experiments = batch.runSimulationBatch(
    world_template,
    max_steps_per_simulation=100,
    progress_callback=lambda brain, step: print(f"  Step {step} completed")
)

end_time = time.time()

print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
print(f"Generated {len(experiments)} experiments")

# Save batch results
batch.saveResults("batch_results.json")
print("Batch results saved to batch_results.json")

# Get batch statistics
stats = batch.getStatistics()
print(stats)
```

### 4.2 Hyperparameter Search

```python
import pynlm
import time

# Create batch processor for hyperparameter search
batch = pynlm.createBatchProcessor()

# Define hyperparameter search space
hyperparameter_names = ["brain.synapse_density", "plasticity.stdp.learning_rate"]
hyperparameter_values = [
    [0.05, 0.001],  # Configuration 1
    [0.1, 0.01],    # Configuration 2
    [0.15, 0.02],   # Configuration 3
    [0.2, 0.005],   # Configuration 4
    [0.25, 0.001],  # Configuration 5
]

# Create world template
world_template = pynlm.createSimpleWorld()
world_template.configure(width=15, height=15)

print(f"Searching over {len(hyperparameter_values)} hyperparameter combinations")
print(f"Parameters: {hyperparameter_names}")

# Run hyperparameter search
print("Running hyperparameter search...")
start_time = time.time()

best_config = batch.runHyperparameterSearch(
    hyperparameter_names,
    hyperparameter_values,
    world_template,
    max_steps=200,
    progress_callback=lambda progress: print(f"  Progress: {progress*100:.1f}%")
)

end_time = time.time()

print(f"Hyperparameter search completed in {end_time - start_time:.2f} seconds")

if best_config:
    print("Best configuration found:")
    for key in best_config.getKeys():
        value = best_config.get(key)
        print(f"  {key}: {value}")
    
    # Save best configuration
    best_config.saveToFile("best_config.json")
    print("Best configuration saved to best_config.json")
else:
    print("No valid configuration found")
```

### 4.3 Cross-Validation

```python
import pynlm
import numpy as np

# Create batch processor for cross-validation
batch = pynlm.createBatchProcessor()

# Create multiple configurations
configs = []
for i in range(10):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000 + i * 500)  # Different sizes
    config.set("brain.synapse_density", 0.05 + i * 0.02)  # Different densities
    batch.addConfiguration(config, f"config_{i}")

# Get all configurations
all_configs = batch.getConfigurations()

print(f"Performing {len(all_configs)}-fold cross-validation")
print(f"Train/test ratio: 0.8")

# Run cross-validation
experiments = batch.runCrossValidation(
    all_configs,
    world_template=None,  # Use default world
    max_steps=100,
    train_test_ratio=0.8,
    progress_callback=lambda progress: print(f"  Fold progress: {progress*100:.1f}%")
)

print(f"\nCross-validation completed")
print(f"Generated {len(experiments)} experimental results")

# Analyze cross-validation results
accuracies = []
for exp in experiments:
    # Calculate accuracy from experiment metrics
    accuracy = exp.getMetric("accuracy") if exp.hasMetric("accuracy") else 0.0
    accuracies.append(accuracy)

if accuracies:
    mean_accuracy = np.mean(accuracies)
    std_accuracy = np.std(accuracies)
    
    print(f"\nCross-validation Results:")
    print(f"  Mean accuracy: {mean_accuracy:.4f}")
    print(f"  Standard deviation: {std_accuracy:.4f}")
    print(f"  Range: {min(accuracies):.4f} - {max(accuracies):.4f}")

# Save cross-validation results
batch.saveResults("cross_validation_results.json")
print("Cross-validation results saved to cross_validation_results.json")
```

## 5. Advanced Agent Capabilities Examples

### 5.1 Custom Agent

```python
import pynlm

# Create custom agent with specific architecture
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)

# Create base brain and agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable custom features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Created custom agent with:")
print(f"  - {brain.getTotalNeuronCount()} neurons")
print(f"  - {brain.getTotalSynapseCount()} synapses")
print(f"  - Enabled: Reward modulation, Curiosity, Development, Structural Plasticity")

# Run custom agent simulation
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")

print("Custom agent simulation completed")
```

### 5.2 Multi-Agent System

```python
import pynlm

# Create multi-agent system
agent_template = pynlm.createDefaultConfig()
agent_template.set("brain.neuron_count", 2000)

print("Creating multi-agent system with 5 agents...")

# Create agent society
agents = pynlm.createMultiAgentSystem(
    agent_count=5,
    template_config=agent_template
)

print(f"Created society with {len(agents)} agents")

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)
world.reset()

# Initialize all agents
for i, agent in enumerate(agents):
    agent.initialize(world)
    print(f"Agent {i}: Initialized with world")

# Run multi-agent simulation
for step in range(200):
    world.update(0.1)
    
    # Each agent perceives the world differently
    for agent in agents:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Each agent makes its own decisions
        action = agent.decodeMotorCommand()
        
        # Apply action to world (shared)
        world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: All agents have acted")

print("Multi-agent simulation completed")
```

### 5.3 Hierarchical Agent

```python
import pynlm

# Create hierarchical agent with multiple layers
layer_neuron_counts = [500, 1000, 500]  # Input, hidden, output
layer_types = ["sensory", "processing", "motor"]

config = pynlm.createDefaultConfig()
config.set("brain.development.enable", True)

print("Creating hierarchical agent with layers:")
for i, (neurons, layer_type) in enumerate(zip(layer_neuron_counts, layer_types)):
    print(f"  Layer {i} ({layer_type}): {neurons} neurons")

# Create hierarchical agent
hierarchical_agent = pynlm.createHierarchicalAgent(
    layer_neuron_counts,
    layer_types,
    config
)

print("Created hierarchical agent")

# Create and initialize world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()
hierarchical_agent.initialize(world)

# Run hierarchical agent simulation
for step in range(250):
    world.update(0.1)
    hierarchical_agent.processSensoryInput(world.getSensoryPercept())
    
    # Hierarchical agent makes decisions at multiple levels
    action = hierarchical_agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get hierarchical output
    print(f"Step {step}:")
    print(f"  - Action: {action}")
    print(f"  - Agent body position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  - Development stage: {hierarchical_agent.getDevelopmentalStage()}")

print("Hierarchical agent simulation completed")
```

## 6. Advanced Error Handling & Debugging Examples

### 6.1 Verbose Logging

```python
import pynlm

# Create brain with verbose logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable verbose logging
brain.setVerboseLogging(True)

# Run simulation with logging
for step in range(50):
    brain.step(step)
    
    if step % 10 == 0:
        # Get detailed status
        status = brain.getSystemHealthReport()
        print(f"\n=== Step {step} ===")
        print("System Health Report:")
        print(status)

print("Verbose logging simulation completed")
```

### 6.2 Performance Profiling

```python
import pynlm
import time

# Create brain for profiling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create profiler
profiler = brain.createProfiler("main_simulation")

# Run simulation with profiling
for step in range(100):
    # Profile specific operations
    brain.step(step)
    
    # Profile world operations
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    world.reset()
    
    # Profile agent operations
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)

# Get profiling results
report = profiler.getReport()
print("Performance Profiling Report:")
print(report)

# Get system profiler for overall performance
system_profiler = brain.getSystemProfiler()
print(f"\nSystem Profiler Total Time: {system_profiler.getTotalTime():.3f} seconds")

# Get statistics
print(f"Profiled operations: {system_profiler.getOperationNames()}")
```

### 6.3 Error Analysis

```python
import pynlm

# Create error analyzer
error_analyzer = pynlm.createErrorAnalyzer()

# Simulate various error conditions
error_analyzer.recordError(
    error_type="MEMORY_ALLOCATION",
    error_message="Failed to allocate 1GB of memory",
    context="Simulation step 50",
    stack_trace="traceback..."
)

error_analyzer.recordError(
    error_type="CONVERGENCE",
    error_message="Neural network failed to converge",
    context="Learning algorithm",
    stack_trace="traceback..."
)

error_analyzer.recordError(
    error_type="MEMORY_ALLOCATION",
    error_message="Failed to allocate 1GB of memory",
    context="Simulation step 120",
    stack_trace="traceback..."
)

# Analyze error patterns
patterns = error_analyzer.analyzePatterns()
print("Error Pattern Analysis:")
for pattern in patterns:
    print(f"  - {pattern}")

# Get recent errors
recent_errors = error_analyzer.getRecentErrors(5)
print(f"\nRecent Errors ({len(recent_errors)}):")
for error in recent_errors:
    print(f"  - {error}")

# Get error analysis report
report = error_analyzer.getReport()
print(f"\nError Analysis Report:\n{report}")

# Export errors to file
error_analyzer.exportErrors("error_analysis.txt")
print("Error analysis exported to error_analysis.txt")
```

## 7. Complete Example: Advanced Research Workflow

```python
import pynlm
import json
import time

print("=== Advanced NLM Research Workflow ===\n")

# Step 1: Configure advanced research setup
print("1. Setting up advanced research configuration...")
research_config = pynlm.createDefaultConfig()

# Enable all advanced features
research_config.set("brain.development.enable", True)
research_config.set("plasticity.advanced_stdp.enable", True)
research_config.set("neuromodulation.curiosity.enable", True)
research_config.set("neuromodulation.novelty.enable", True)
research_config.set("visualization.enable", True)
research_config.set("batch_processing.enable", True)

# Configure specific parameters
research_config.set("brain.neuron_count", 3000)
research_config.set("plasticity.stdp.learning_rate", 0.002)
research_config.set("neuromodulation.curiosity.novelty_weight", 0.6)

print("   ✓ Configuration loaded")

# Step 2: Initialize core components
print("2. Initializing core components...")
brain = pynlm.createBrain(research_config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

print("   ✓ Brain, agent, and world initialized")

# Step 3: Setup visualization
print("3. Setting up visualization pipeline...")
viz = brain.getVisualizationInterface()
viz.setUpdateRate(15.0)
viz.initialize()

# Create multi-modal visualization
brain.createVisualizationPipeline()
print("   ✓ Visualization pipeline configured")

# Step 4: Run exploration episode
print("4. Running exploration episode...")
exploration_data = []

for step in range(200):
    # World dynamics
    world.update(0.1)
    
    # Agent perception
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Agent action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect data
    exploration_data.append({
        'step': step,
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'reward': agent.getNeuromodulationLevel(),
        'firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
        'action': str(action)
    })
    
    # Update visualization every 10 steps
    if step % 10 == 0:
        viz.update()

print(f"   ✓ Exploration completed: {len(exploration_data)} steps recorded")

# Step 5: Batch processing with different conditions
print("5. Running batch simulations...")

batch = pynlm.createBatchProcessor()

# Add various configurations
configs = []
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000 + i * 1000)
    config.set("plasticity.stdp.learning_rate", 0.001 * (i + 1))
    batch.addConfiguration(config, f"research_config_{i}")

# Run batch simulation
experiments = batch.runSimulationBatch(
    world,
    max_steps_per_simulation=50,
    progress_callback=lambda b, s: print(f"     Batch progress: {s}/150 steps")
)

print(f"   ✓ Batch simulation completed: {len(experiments)} experiments")

# Step 6: Hyperparameter optimization
print("6. Running hyperparameter optimization...")

hyperparameter_names = ["plasticity.stdp.learning_rate", "neuromodulation.curiosity.novelty_weight"]
hyperparameter_values = [
    [0.001, 0.3],
    [0.005, 0.5],
    [0.01, 0.7],
    [0.02, 0.9],
]

best_config = batch.runHyperparameterSearch(
    hyperparameter_names,
    hyperparameter_values,
    world,
    max_steps=100,
    progress_callback=lambda p: print(f"     Hyperparameter search: {p*100:.0f}% complete")
)

if best_config:
    print(f"   ✓ Hyperparameter optimization completed")
    print(f"   Best learning rate: {best_config.get('plasticity.stdp.learning_rate')}")
    print(f"   Best novelty weight: {best_config.get('neuromodulation.curiosity.novelty_weight')}")

# Step 7: Network analysis
print("7. Analyzing network properties...")

brain.analyzeConnectivity()
metrics = brain.computeGraphMetrics()

print(f"   ✓ Network analysis completed")
print(f"   - Connected components: {metrics.get('connected_components', 0)}")
print(f"   - Network efficiency: {metrics.get('efficiency', 0):.4f}")
print(f"   - Average path length: {metrics.get('avg_path_length', 0):.4f}")

# Step 8: Error analysis and profiling
print("8. Error analysis and performance profiling...")

# Create performance profiler
system_profiler = brain.getSystemProfiler()

# Create error analyzer
error_analyzer = brain.getErrorAnalyzer()

# Get performance report
perf_report = system_profiler.getReport()
print(f"   ✓ Performance profiling completed")
print(f"   - Total profiling time: {system_profiler.getTotalTime():.2f} seconds")

# Get error analysis
error_report = error_analyzer.getReport()
print(f"   ✓ Error analysis completed")
print(f"   - Errors recorded: {error_analyzer.getErrorCount('MEMORY_ALLOCATION')}")

# Step 9: Export results
print("9. Exporting results...")

# Export visualization
viz.exportVisualization("research_visualization.gif")
print("   ✓ Visualization exported: research_visualization.gif")

# Export configuration
with open("research_config.json", "w") as f:
    json.dump(json.loads(research_config.toJSON()), f, indent=2)
print("   ✓ Configuration exported: research_config.json")

# Export batch results
batch.saveResults("research_batch_results.json")
print("   ✓ Batch results exported: research_batch_results.json")

# Export error analysis
error_analyzer.exportErrors("research_error_analysis.txt")
print("   ✓ Error analysis exported: research_error_analysis.txt")

print("\n=== Research Workflow Complete ===")
print(f"Total time: {time.time() - start_time:.2f} seconds")
print(f"Data collected: {len(exploration_data)} exploration steps")
print(f"Experiments run: {len(experiments)}")
```

## Summary

This comprehensive guide covers all advanced features of the enhanced NLM Python bindings:

1. **Advanced Configuration**: JSON loading/validation, source tracking, schema validation
2. **Visualization & Analysis**: Multi-modal pipelines, network analysis, graph metrics
3. **Advanced Learning**: Reward-modulated STDP, triplet STDP, curiosity-driven exploration
4. **Batch Processing**: Simulation batches, hyperparameter search, cross-validation
5. **Advanced Agents**: Custom agents, multi-agent systems, hierarchical agents
6. **Error Handling**: Verbose logging, performance profiling, error analysis

Each example demonstrates practical usage with real NLM simulations, making these advanced features accessible to researchers and developers working with neural learning machines.