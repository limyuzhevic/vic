# NLM - Getting Started Guide

## Welcome to NLM

NLM (熙然) is an experimental artificial developmental brain project that investigates computational brain-like systems. This guide will help you get started with NLM, from basic setup to running your first simulations.

## Quick Start

If you just want to see NLM in action quickly, follow these steps:

### 1. Quick Installation (Recommended)

Using the Python bindings is the easiest way to start with NLM:

```bash
# Clone the repository
mkdir nlm-workspace
cd nlm-workspace
git clone https://github.com/nlm-project/nlm

# Navigate to the project
cd nlm

# Install the Python bindings (this builds everything automatically)
pip install -e .

# Run the demo script
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print(f'Brain created: {brain.getTotalNeuronCount()} neurons')"
```

### 2. Build from Source (Advanced)

For development work or custom configurations:

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm
cd nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### 3. Run a Demo

Once installed, you can run various demos:

```bash
# Run the Phase 6 integration demo
cd /path/to/nlm
./build/nlm_phase6_demo

# Or use Python for simple simulations
python examples/simple_brain.py
python examples/agent_world.py
python examples/research_config.py
```

## Basic Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 1000 simulation steps
for step in range(1000):
    brain.step(step)

# Check statistics
print(f"Neurons created: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 2: Brain with World Interaction

```python
import pynlm

def run_agent_simulation():
    """Complete example showing brain, world, and agent interaction."""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create the world environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create the agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 5. Run the simulation
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain what it senses
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress occasionally
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.2f}")
    
    print("Agent simulation complete!")
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

### Example 3: Research Configuration

```python
import pynlm

def create_research_brain():
    """Create a research-grade brain with advanced configuration."""
    
    # Use the advanced configuration from the CLI
    config = pynlm.createAdvancedConfig()  # Available in pynlm module
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Research brain created:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Working memory: {'Enabled' if brain.getWorkingMemory() else 'Disabled'}")
    print(f"  Prediction system: {'Enabled' if brain.getPredictionSystem() else 'Disabled'}")
    
    return brain

# Create research brain
brain = create_research_brain()
```

### Example 4: Brain Analysis

```python
import pynlm

def analyze_brain(checkpoint_path=None, neuron_count=500):
    """Analyze a brain using the NLM CLI tools."""
    
    # Create or load brain
    if checkpoint_path:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        if brain.initialize() and brain.load(checkpoint_path):
            print(f"Loaded brain from {checkpoint_path}")
        else:
            print(f"Failed to load {checkpoint_path}, creating new brain")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
    else:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print(f"Created new brain with {neuron_count} neurons")
    
    # Run some analysis
    print("\nBrain Analysis Results:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain

# Analyze a brain
brain = analyze_brain(neuron_count=1000)
```

## Advanced Features

### Custom Configuration

```python
import pynlm

# Create a custom configuration
custom_config = pynlm.createDefaultConfig()

# Modify parameters
custom_config.set("brain.neuron_count", 2000)
custom_config.set("brain.synapse_density", 0.05)
custom_config.set("plasticity.stdp.enable", true)
custom_config.set("neuromod.dopamine.scale", 1.5f)
custom_config.set("development.enabled", true)

# Create brain with custom config
brain = pynlm.createBrain(custom_config)
brain.initialize()
print(f"Custom brain created with {brain.getTotalNeuronCount()} neurons")
```

### Simulation Control

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run with custom settings
num_steps = 2000
timestep = 0.05

for step in range(num_steps):
    # Update world
    world.update(timestep)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(timestep)
    
    # Print statistics
    if step % 500 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
              f"curiosity: {agent.getCuriosityLevel():.2f}, "
              f"dev: {brain.getDevelopmentalStage()}")
```

## Common Patterns

### Pattern 1: Running a Simulation

```python
def run_simulation(brain, world, agent, num_steps, timestep=0.1):
    """Standard pattern for agent simulations."""
    for step in range(num_steps):
        # 1. Update world
        world.update(timestep)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process input in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # 4. Decode action
        action = agent.decodeMotorCommand()
        
        # 5. Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Apply learning
        reward = calculate_reward(world)  # Your reward function
        agent.applyRewardModulation(reward, 0.0)
        
        # Optional: Update development
        agent.updateDevelopment(timestep)
        
        # Optional: Log statistics
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Pattern 2: Getting Brain Statistics

```python
def log_brain_stats(brain, agent=None, step=0):
    """Log comprehensive brain statistics."""
    stats = {
        'step': step,
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': str(brain.getDevelopmentalStage()),
        'total_spikes': brain.getTotalSpikeCount(),
    }
    
    if agent:
        stats['curiosity'] = agent.getCuriosityLevel()
        stats['novelty'] = agent.getNoveltyLevel()
        stats['dopamine'] = agent.getNeuromodulationLevel()
    
    return stats
```

### Pattern 3: Enabling Brain Features

```python
def enable_brain_features(agent, enable_reward=True, enable_curiosity=True, 
                         enable_development=True, enable_structural=True):
    """Enable all learning and development features."""
    if enable_reward:
        agent.enableRewardModulation(True)
        print("✓ Reward modulation enabled")
    
    if enable_curiosity:
        agent.enableCuriosity(True)
        print("✓ Curiosity enabled")
    
    if enable_development:
        agent.enableDevelopment(True)
        print("✓ Development enabled")
    
    if enable_structural:
        agent.enableStructuralPlasticity(True)
        print("✓ Structural plasticity enabled")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "My brain isn't doing anything"

```python
# Solution: Always call initialize()
brain = pynlm.createBrain(config)
brain.initialize()  # <-- Add this!

# Also make sure to run enough steps
for step in range(100):  # <-- More steps
    brain.step(step)
```

**Issue: "The agent isn't moving"

```python
# Solution: Make sure all components are set up
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)  # <-- Make sure agent is initialized

# Make sure world.update() is called in your loop
for step in range(num_steps):
    world.update(0.1)  # <-- This is required!
    # ... your simulation loop
```

**Issue: "Everything is 0"

```python
# Solution: Brains need time to "warm up"
for step in range(500):  # <-- More steps
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
```

### Getting Help

1. **Documentation**: Check the `docs/` directory for detailed information
2. **Examples**: Look at `examples/` for complete working examples
3. **GitHub**: Visit https://github.com/nlm-project/nlm for issues and discussions
4. **Community**: Join the NLM research community for discussions

## Further Reading

- **ARCHITECTURE.md**: Detailed system architecture and component descriptions
- **SCIENCE.md**: Scientific background and biological inspiration
- **EXPERIMENTS.md**: Details of all Phase 4 experiments
- **ROADMAP.md**: Project roadmap and future directions
- **LIMITATIONS.md**: Known limitations and areas for improvement

## Getting Involved

NLM is an open research project. You can:

1. **Contribute Code**: Submit pull requests to improve functionality
2. **Report Issues**: File issues for bugs or feature requests
3. **Run Experiments**: Use NLM for your own research projects
4. **Teach**: Use NLM as an educational tool
5. **Document**: Help improve documentation and tutorials

This guide will be updated as NLM continues to develop. If you have questions or suggestions, please contribute to the project!

---

**Last Updated**: October 8, 2026
**Version**: 0.1.0 (Phase 6)
**License**: MIT

> "NLM is not a replacement for neuroscience, but a tool to explore and test hypotheses about how brains might work." — NLM Research Team
