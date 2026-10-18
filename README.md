# NLM - Main Documentation Index

## Welcome to NLM Documentation

NLM (熙然, meaning "serene flow") is an experimental computational brain project implementing a neural system that develops and adapts through interaction with an environment. Currently in Phase 6 (Final Integration), NLM provides a comprehensive framework for simulating brain-like intelligence using spiking neural networks.

## Documentation Structure

```
NLM Documentation
├── README.md                    # Project overview and quick start
├── docs/
│   ├── STRUCTURE.md            # System architecture
│   ├── SCIENCE.md              # Scientific background
│   ├── INSTALLATION.md          # Build and installation guide
│   ├── USAGE/
│   │   ├── BASIC.md            # Beginner tutorial
│   │   ├── INTERMEDIATE.md      # Intermediate usage
│   │   └── ADVANCED.md          # Advanced features
│   ├── API_REFERENCE/
│   │   ├── BRAIN.md             # Brain class API
│   │   ├── CONFIG.md            # Configuration API
│   │   └── AGENT.md             # AgentBrain API
│   └── TUTORIALS/
│       ├── NEURON_MODELS.md     # Understanding neuron types
│       ├── PLASTICITY.md        # Learning mechanisms
│       ├── MEMORY.md            # Memory systems
│       └── BEHAVIOR.md          # Behavior patterns
├── examples/
│   ├── basic_brain.py          # Basic usage example
│   ├── agent_world.py          # Agent-environment interaction
│   ├── learning_loop.py        # Complete learning example
│   └── debug_analysis.py       # Analysis example
├── configs/
│   ├── default.json           # Default configuration
│   ├── examples/              # Example configurations
│   └── templates/              # Configuration templates
└── scripts/
    ├── build.py               # Build automation
    ├── install.py              # Installation helpers
    ├── test.py                 # Test runner
    ├── benchmark.py            # Performance benchmarking
    └── visualize.py            # Visualization tools
```

## Quick Start

### Installation

```bash
# Install the Python package
pip install .

# Or install in development mode
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! {brain.getTotalSpikeCount()} spikes generated.")
```

### Complete Agent Example

```python
import pynlm

# Setup components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableAllLearning(True)

# Run learning episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Getting Help

### Documentation Sections

1. **README.md**: Project overview and quick start guide
2. **docs/STRUCTURE.md**: System architecture and component overview
3. **docs/INSTALLATION.md**: Complete installation and setup guide
4. **docs/USAGE/BASIC.md**: Comprehensive beginner tutorial
5. **docs/USAGE/INTERMEDIATE.md**: Intermediate usage patterns and techniques
6. **docs/USAGE/ADVANCED.md**: Advanced features and customizations
7. **docs/TUTORIALS/**: Deep dives into specific topics
8. **examples/**: Working code examples
9. **configs/**: Configuration files and templates
10. **scripts/**: Development and automation tools

### Learning Pathway

#### For Beginners
1. **Start with README.md**: Understand what NLM is and how to get started
2. **Follow docs/USAGE/BASIC.md**: Complete beginner tutorial
3. **Try examples/basic_brain.py**: Simple working example
4. **Explore examples/agent_world.py**: Agent-environment interaction

#### For Intermediate Users
1. **Read docs/USAGE/INTERMEDIATE.md**: Advanced usage patterns
2. **Study docs/TUTORIALS/**: Deep technical topics
3. **Experiment with examples/learning_loop.py**: Complete learning system
4. **Use scripts/benchmark.py**: Performance optimization

#### For Advanced Users
1. **Master docs/USAGE/ADVANCED.md**: Expert-level features
2. **Explore API_REFERENCE/**: Complete method documentation
3. **Create custom extensions**: Extend functionality
4. **Contribute to project**: Share improvements with community

### Support Resources

#### Online Resources
- **GitHub Repository**: Source code and issue tracking
- **Documentation**: Complete API reference and guides
- **Examples**: Working code examples and tutorials
- **Community**: Discussion forums and collaboration

#### Local Resources
- **examples/**: Working example scripts
- **configs/**: Configuration files and templates
- **scripts/**: Development and automation tools
- **docs/**: Complete documentation set

### Getting Started Checklist

```bash
# 1. Install NLM
pip install .

# 2. Test basic installation
python -c "import pynlm; print('NLM imported successfully')"

# 3. Run basic example
python examples/basic_brain.py

# 4. Try complete example
python examples/agent_world.py

# 5. Explore documentation
less docs/USAGE/BASIC.md
```

## Project Overview

### What is NLM?

NLM (熙然) is a computational brain simulation framework that implements:

- **Brain-like Intelligence**: Spiking neural networks with biologically plausible dynamics
- **Developmental Learning**: Brains that grow and adapt over time
- **Memory Integration**: Multiple memory systems (working, episodic, semantic)
- **Neuromodulation**: Reward, curiosity, and novelty-driven learning
- **Environmental Interaction**: Agents that sense and act in virtual worlds

### Key Features

1. **Event-Driven Simulation**: Precise spike timing and synaptic delays
2. **Multiple Plasticity Mechanisms**: STDP, Hebbian learning, structural plasticity
3. **Neuromodulation Integration**: Dopamine, curiosity, novelty systems
4. **Developmental Architecture**: Age-dependent plasticity and maturation
5. **Memory Systems**: Working memory with persistent activity
6. **Prediction Systems**: Forward modeling and expectation generation
7. **Checkpoint System**: State save/load functionality
8. **Performance Optimized**: Memory pools and parallel processing

### Technical Architecture

```
World (Environment)
  ↓
Sensory Input (Vision, Audio, etc.)
  ↓
Brain (Neural Computation)
  ↓
Agent (Integration Layer)
  ↓
Motor Output (Actions)
  ↓
World Consequences
  ↓
Learning Loop
```

### Current Status

**Phase 6: Final Integration**

✅ **Achieved**:
- Core LIF neuron dynamics with event-driven spikes
- Synaptic transmission with delays and plasticity
- Neuromodulation (dopamine, curiosity, novelty)
- Development stages and age-dependent effects
- Basic memory systems
- Environment interaction loop

**In Progress**:
- Full memory system integration
- Advanced prediction mechanisms
- Enhanced developmental processes
- Custom extension framework

## Usage Examples

### Example 1: Silent Brain (Neurons Only)

```python
import pynlm

# Create and run a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Silent brain completed {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world
for i in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("Watched world for 50 steps")
```

### Example 3: Complete Learning Agent

```python
import pynlm

def run_learning_agent(num_steps=1000):
    """Run a complete learning agent with all features"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = 0.0  # Extract from world state
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return agent

# Run the agent
agent = run_learning_agent(500)
```

## Configuration Options

### Brain Configuration

```python
config = pynlm.createDefaultConfig()

# Basic settings
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation settings
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

### World Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
```

## Development Guide

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build everything
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Running Tests

```bash
# Run unit tests
pytest tests/

# Run example scripts
python examples/basic_brain.py
python examples/agent_world.py
python examples/learning_loop.py
```

### Contributing

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement your changes**
4. **Add tests**
5. **Update documentation**
6. **Submit a pull request**

## Future Directions

### Research Applications
- **Studying neural development**: Track changes over time
- **Testing plasticity mechanisms**: Evaluate different learning rules
- **Modeling cognitive functions**: Simulate perception, memory, decision-making
- **Neuroscientific validation**: Compare with biological data

### Technical Enhancements
- **Advanced extensions**: Custom neural types and learning rules
- **Performance optimization**: Parallel processing and vectorization
- **Visualization tools**: Advanced plotting and analysis
- **Integration platforms**: Support for ROS, Unity, game engines

### Educational Applications
- **Teaching neuroscience**: Demonstrating brain function
- **AI education**: Explaining neural network fundamentals
- **Research training**: Hands-on computational neuroscience
- **Interactive learning**: Visual and interactive demonstrations

## Conclusion

NLM provides a powerful framework for simulating brain-like intelligence. Its combination of biologically plausible dynamics, multiple learning systems, and environmental interaction makes it suitable for research, education, and applications requiring adaptive intelligent systems.

Whether you're a beginner wanting to understand neural computation, a researcher studying brain function, or a developer building intelligent agents, NLM provides the tools and documentation needed to succeed.

**Start your NLM journey today!**

## Quick Links

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: docs/ directory
- **Examples**: examples/ directory
- **Configuration**: configs/ directory
- **Scripts**: scripts/ directory
- **Support**: Project documentation and community resources

---

*Last Updated: 2026-10-18*
*Version: 0.1.0*
