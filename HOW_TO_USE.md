# ADVANCED NLM USAGE - Phase 6 INTEGRATION

## Enhanced Features Documentation

This document provides comprehensive information about the advanced features available in NLM Phase 6, including configuration presets, Phase 6 integration helpers, and advanced simulation capabilities.

---

## Phase 6 Integration Features

NLM Phase 6 introduces several advanced features that make the Python API more powerful and user-friendly:

### 1. Configuration Presets

The following configuration presets are available to match different research needs:

#### Development Configuration (`createDevelopmentConfig()`)

This configuration is optimized for research and learning scenarios:

```python
import pynlm

config = pynlm.createDevelopmentConfig()
```

**Key Features:**
- **High plasticity**: Set `plasticity_modulation` to 1.0f for maximum learning
- **Advanced neuromodulation**: Dopamine, curiosity, and novelty systems fully enabled
- **Rich memory systems**: Working memory with 100 entries, episodic memory with 1000 max episodes
- **Development support**: All developmental stages available with plasticity modulation
- **Balanced connectivity**: 1000 neurons with 2 regions and 0.15 connection probability

**Ideal for:**
- Neural development research
- Learning and plasticity studies
- Cognitive development modeling
- Educational applications

#### Exploration Configuration (`createExplorationConfig()`)

This configuration drives curiosity-driven exploration and discovery:

```python
import pynlm

config = pynlm.createExplorationConfig()
```

**Key Features:**
- **Curiosity-driven learning**: Low `curiosity_threshold` (0.3f) encourages exploration
- **Novelty detection**: Sensitivity to new stimuli with `novelty_threshold` of 0.5f
- **High learning rates**: Enhanced STDP weights for rapid adaptation
- **Exploration rewards**: Bonus system encourages novel behavior
- **Moderate complexity**: 500 neurons, 1 region, 0.2 connectivity for efficient exploration

**Ideal for:**
- Exploration vs. exploitation research
- Curiosity-driven AI systems
- Novelty detection studies
- Behavioral economics applications

#### Stable Configuration (`createStableConfig()`)

This configuration maintains consolidated, mature behavior:

```python
import pynlm

config = pynlm.createStableConfig()
```

**Key Features:**
- **Maturity focus**: Developmental stage set to 4 (Adult) for stable behavior
- **Low plasticity**: Reduced `plasticity_modulation` (0.2f) for stability
- **Consolidation support**: Frequent memory replay and consolidation
- **High stability**: High `novelty_threshold` (2.0f) filters out noise
- **Complex architecture**: 2000 neurons, 3 regions, 0.05 connectivity for rich dynamics

**Ideal for:**
- Stable behavior maintenance
- Long-term memory consolidation
- Mature cognitive systems
- Production applications

### 2. Phase 6 Complete Demo Helper

The `createPhase6Demo()` function provides a complete, ready-to-use setup for Phase 6 research:

```python
import pynlm

# One function call creates everything needed for Phase 6 integration
config, brain, world, agent = pynlm.createPhase6Demo()

# The helper function:
# 1. Creates a default configuration with optimal Phase 6 parameters
# 2. Initializes a fully integrated brain with all systems
# 3. Creates a 2D world with standard exploration settings
# 4. Sets up an agent brain interface with all features enabled
# 5. Returns all components ready for immediate use
```

**Usage Example:**

```python
# Quick start for Phase 6 research
config, brain, world, agent = pynlm.createPhase6Demo()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)
pynlm.enableAllFeatures(agent)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### 3. Feature Management

All advanced features can be enabled with a single function call:

```python
import pynlm

# Enable all features for maximum functionality
pynlm.enableAllFeatures(agent)

# Verify features are active
assert agent.isRewardModulationEnabled()
assert agent.isStructuralPlasticityEnabled()
assert agent.isDevelopmentEnabled()
assert agent.isCuriosityEnabled()
```

**Features Enabled:**

1. **Reward Modulation**: Dopamine-based learning and reinforcement
2. **Structural Plasticity**: Synaptogenesis and pruning for network growth
3. **Development**: Age-dependent plasticity and maturation
4. **Curiosity**: Exploration motivation and novelty detection

### 4. Simulation Loop Wrapper

The `runSimulationLoop()` function simplifies simulation execution:

```python
import pynlm

# Create components using Phase 6 helper
config, brain, world, agent = pynlm.createPhase6Demo()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)
pynlm.enableAllFeatures(agent)

# Run simulation with one function call
spikes, firing, energy = pynlm.runSimulationLoop(brain, world, agent, 1000)

print(f"Simulation completed: {spikes} spikes, {firing} firing neurons")
print(f"Agent energy: {energy}")
```

**Benefits:**
- **Simplified API**: Single function for complete simulation loop
- **Automatic updates**: World, agent, and brain coordinated automatically
- **Progress reporting**: Optional statistics collection
- **Error handling**: Built-in validation and error checking
- **Feature support**: Rewards and development automatically applied

---

## Advanced API Usage Examples

### Example 1: Development Research Workflow

```python
import pynlm
import matplotlib.pyplot as plt

print("=== Development Research Workflow ===")

# Create development configuration
dev_config = pynlm.createDevelopmentConfig()
print(f"Configuration: {dev_config.getOr('neuron_count', 0)} neurons")
print(f"Plasticity: {dev_config.getOr('plasticity_modulation', 0.0f)}")

# Initialize complete setup
brain = pynlm.createBrain(dev_config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
pynlm.enableAllFeatures(agent)

# Run development simulation
print("\nRunning development simulation...")
stats = []

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Collect developmental metrics
    if step % 50 == 0:
        stage = brain.getDevelopmentalStage()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        neuromod = agent.getNeuromodulationLevel()
        
        stats.append({
            'step': step,
            'development_stage': stage,
            'curiosity': curiosity,
            'novelty': novelty,
            'neuromodulation': neuromod,
            'firing': brain.getFiringNeuronCount(),
            'spikes': brain.getTotalSpikeCount()
        })
        
        print(f"  Step {step:3d}: Stage {stage}/4, "
              f"Curiosity {curiosity:.3f}, "
              f"Novelty {novelty:.3f}, "
              f"Firing {brain.getFiringNeuronCount():3d}")

# Analyze development progress
if stats:
    print(f"\n=== Development Analysis ===")
    print(f"Initial stage: {stats[0]['development_stage']}/4")
    print(f"Final stage: {stats[-1]['development_stage']}/4")
    print(f"Stage progress: {(stats[-1]['development_stage'] - stats[0]['development_stage']) / 4:.1%}")
    
    # Plot development trajectory
    stages = [s['development_stage'] for s in stats]
    curiosities = [s['curiosity'] for s in stats]
    
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
    
    ax1.plot([s['step'] for s in stats], stages, 'b-', linewidth=2)
    ax1.set_ylabel('Developmental Stage')
    ax1.set_title('Brain Development Progress')
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim(0, 4)
    
    ax2.plot([s['step'] for s in stats], curiosities, 'r-', linewidth=2)
    ax2.set_xlabel('Simulation Step')
    ax2.set_ylabel('Curiosity Level')
    ax2.set_title('Exploration Motivation')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('development_analysis.png')
    print(f"Development plot saved as 'development_analysis.png'")
```

### Example 2: Cross-Environment Comparison

```python
import pynlm

print("=== Cross-Environment Comparison ===")

def compare_environments():
    """Compare development, exploration, and stable environments"""
    
    environments = [
        ("Development", "development"),
        ("Exploration", "exploration"),
        ("Stable", "stable")
    ]
    
    results = {}
    
    for name, config_type in environments:
        print(f"\nTesting {name} environment...")
        
        # Create configuration
        if config_type == "development":
            config = pynlm.createDevelopmentConfig()
        elif config_type == "exploration":
            config = pynlm.createExplorationConfig()
        else:
            config = pynlm.createStableConfig()
        
        # Initialize and run
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on environment
        if config_type == "exploration":
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
        elif config_type == "development":
            pynlm.enableAllFeatures(agent)
        else:
            agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Collect results
        results[name] = {
            'config_type': config_type,
            'final_stage': brain.getDevelopmentalStage(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing': brain.getAverageFiringRate(),
            'final_energy': world.getAgentBody().energy
        }
        
        print(f"  ✓ {name}: Stage {results[name]['final_stage']}/4, "
              f"Curiosity {results[name]['final_curiosity']:.3f}, "
              f"Spikes {results[name]['total_spikes']}")
    
    return results

# Run comparison
comparison_results = compare_environments()

# Display comparison table
print(f"\n=== Environment Comparison Results ===")
print(f"{'Environment':<12} {'Stage':<6} {'Curiosity':<10} {'Novelty':<8} {'Spikes':<8} {'Firing Rate':<12} {'Energy':<8}")
print("-" * 70)

for name, result in comparison_results.items():
    print(f"{name:<12} {result['final_stage']:<6} {result['final_curiosity']:<10.3f} "
          f"{result['final_novelty']:<8.3f} {result['total_spikes']:<8} "
          f"{result['avg_firing']:<12.1f} {result['final_energy']:<8.1f}")

return comparison_results

# Execute comparison
results = compare_environments()
```

### Example 3: Multi-Agent Research

```python
import pynlm

print("=== Multi-Agent Research ===")

def multi_agent_research(num_agents=3, steps_per_agent=300):
    """
    Run multiple agents with different configurations for comparison.
    Useful for studying individual differences and environmental effects.
    """
    
    # Create different agent configurations
    agents = []
    agent_names = ["Agent_1_Development", "Agent_2_Exploration", "Agent_3_Stable"]
    
    for i, name in enumerate(agent_names):
        print(f"\nCreating {name}...")
        
        # Select configuration based on agent index
        if i == 0:
            config = pynlm.createDevelopmentConfig()
            description = "High learning, development focus"
        elif i == 1:
            config = pynlm.createExplorationConfig()
            description = "Curiosity-driven, exploration focus"
        else:
            config = pynlm.createStableConfig()
            description = "Mature, stable behavior"
        
        # Create and initialize agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        pynlm.enableAllFeatures(agent)
        
        agents.append({
            'name': name,
            'config': config,
            'brain': brain,
            'world': world,
            'agent': agent,
            'description': description,
            'results': []
        })
        
        print(f"  ✓ {name}: {config.getOr('neuron_count', 0)} neurons")
        print(f"    {description}")
    
    # Run simulation for each agent
    print(f"\nRunning simulations...")
    for agent_data in agents:
        print(f"  Simulating {agent_data['name']}...")
        
        for step in range(steps_per_agent):
            agent_data['world'].update(0.1)
            agent_data['agent'].processSensoryInput(
                agent_data['world'].getSensoryPercept()
            )
            agent_data['brain'].step(step)
            agent_data['world'].applyMotorCommand(
                agent_data['agent'].decodeMotorCommand(),
                agent_data['world'].getSimulationTime()
            )
            
            # Collect metrics every 60 steps
            if step % 60 == 0:
                agent_data['results'].append({
                    'step': step,
                    'firing': agent_data['brain'].getFiringNeuronCount(),
                    'spikes': agent_data['brain'].getTotalSpikeCount(),
                    'stage': agent_data['brain'].getDevelopmentalStage(),
                    'curiosity': agent_data['agent'].getCuriosityLevel(),
                    'novelty': agent_data['agent'].getNoveltyLevel(),
                    'energy': agent_data['world'].getAgentBody().energy
                })
        
        print(f"    ✓ Completed: {agent_data['brain'].getTotalSpikeCount()} spikes")
    
    # Analyze agent differences
    print(f"\n=== Agent Comparison ===")
    
    # Create comparison table
    print(f"{'Agent':<20} {'Config':<15} {'Stage':<6} {'Curiosity':<10} {'Spikes':<8} {'Final Energy':<12}")
    print("-" * 70)
    
    for agent_data in agents:
        if agent_data['results']:
            final = agent_data['results'][-1]
            config_name = agent_data['description'].split(',')[0] if ',' in agent_data['description'] else agent_data['description']
            
            print(f"{agent_data['name']:<20} {config_name:<15} {final['stage']:<6} "
                  f"{final['curiosity']:<10.3f} {final['spikes']:<8} "
                  f"{final['energy']:<12.1f}")
    
    return agents

# Run multi-agent research
agent_results = multi_agent_research()
```

---

## API Reference for Advanced Features

### Configuration Presets

| Function | Description | Best For |
|----------|-------------|----------|
| `createDevelopmentConfig()` | Development-focused configuration with high learning | Research, education, development studies |
| `createExplorationConfig()` | Curiosity-driven configuration for exploration | Behavioral research, novelty detection |
| `createStableConfig()` | Mature, stable configuration | Production, long-term studies |

### Phase 6 Integration

| Function | Purpose | Usage |
|----------|---------|-------|
| `createPhase6Demo()` | Complete Phase 6 setup helper | Quick start for research |
| `enableAllFeatures(agent)` | Enable all advanced features | Maximum functionality |
| `runSimulationLoop()` | Simplified simulation wrapper | Basic simulation setup |

### Agent Features

| Method | Description | When to Use |
|--------|-------------|-------------|
| `isRewardModulationEnabled()` | Check if reward modulation is on | Feature validation |
| `isStructuralPlasticityEnabled()` | Check if structural plasticity is on | Network growth research |
| `isDevelopmentEnabled()` | Check if development is active | Maturation studies |
| `isCuriosityEnabled()` | Check if curiosity system is active | Exploration research |

### Statistics Collection

| Method | Returns | Use Case |
|--------|---------|----------|
| `getCuriosityLevel()` | Current curiosity motivation | Exploration monitoring |
| `getNoveltyLevel()` | Novelty detection signal | Adaptation studies |
| `getPredictionError()` | Learning signal error | Reinforcement learning |
| `getNeuromodulationLevel()` | Overall neuromodulation | System state monitoring |
| `getDevelopmentalStage()` | Current developmental stage | Development tracking |

---

## Installation and Setup

### Prerequisites

```bash
# Install Python dependencies
pip install pynlm
pip install matplotlib numpy scipy pandas
```

### Quick Start

```python
import pynlm

# Test installation
print("Testing NLM installation...")
version = pynlm.getVersionInfo()
print(f"NLM Version: {version}")

# Create a simple test
config, brain, world, agent = pynlm.createPhase6Demo()
print(f"Test brain created: {brain.getTotalNeuronCount()} neurons")

# Run quick simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print("✓ Installation test successful!")
print("You can now start using NLM for advanced research.")
```

---

## Troubleshooting and Support

### Common Issues

**"Installation failed"**
```bash
pip install --force-reinstall .
```

**"Features not working"**
```python
# Ensure all features are enabled
pynlm.enableAllFeatures(agent)

# Verify features
assert agent.isRewardModulationEnabled()
assert agent.isDevelopmentEnabled()
```

**"Simulation too slow"**
```python
# Use faster configuration
config = pynlm.createStableConfig()  # Less complex, faster
```

**"Development not progressing"**
```python
# Use development configuration
config = pynlm.createDevelopmentConfig()
```

### Getting Help

1. **Documentation**: Check the README and documentation files
2. **Examples**: Run the example scripts provided
3. **Community**: Join the NLM research community
4. **Issues**: Report bugs on the GitHub repository

---

## Advanced Research Applications

### 1. Neural Development Studies

```python
# Study how brains develop over time
import pynlm

dev_config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(dev_config)
brain.initialize()

# Track developmental progression
development_stages = []
for step in range(1000):
    brain.step(step)
    development_stages.append(brain.getDevelopmentalStage())

# Analyze development trajectory
from collections import Counter
stage_distribution = Counter(development_stages)
print("Developmental stage distribution:", stage_distribution)
```

### 2. Learning and Plasticity Research

```python
# Study learning dynamics with exploration
import pynlm

expl_config = pynlm.createExplorationConfig()
brain = pynlm.createBrain(expl_config)
brain.initialize()

# Monitor learning signals
learning_metrics = []
for step in range(500):
    neuromod = brain.getDopamine() if hasattr(brain, 'getDopamine') else None
    learning_metrics.append({
        'step': step,
        'prediction_error': neuromod.getLevel() if neuromod else 0,
        'learning_rate': brain.getLearningRate() if hasattr(brain, 'getLearningRate') else 0
    })
```

### 3. Memory System Research

```python
# Study memory formation and consolidation
import pynlm

stable_config = pynlm.createStableConfig()
brain = pynlm.createBrain(stable_config)
brain.initialize()

# Track memory-related activity
memory_activities = []
for step in replay_interval in range(0, 1000, replay_interval):
    memory_activities.append({
        'step': step,
        'replay_activity': brain.getReplayActivity() if hasattr(brain, 'getReplayActivity') else 0,
        'consolidation': brain.getConsolidationStrength() if hasattr(brain, 'getConsolidationStrength') else 0
    })
```

---

## Conclusion

NLM Phase 6 provides a comprehensive framework for advanced neural network research with:

1. **Multiple configuration presets** for different research needs
2. **Phase 6 integration tools** for simplified setup
3. **Advanced feature management** for maximum functionality
4. **Comprehensive simulation tools** for various research scenarios
5. **Extensive documentation** and examples for easy adoption

The advanced features enable researchers to study:
- Neural development and maturation
- Learning and plasticity mechanisms
- Memory formation and consolidation
- Curiosity-driven exploration
- Reward-based learning
- Multi-agent interactions
- Behavioral adaptation and learning

Get started today and explore the full capabilities of NLM Phase 6 for your research!