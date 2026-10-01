# NLM Tutorial System

This tutorial system provides comprehensive step-by-step instructions for learning NLM (Neural Learning Machine). It includes:

- **Tutorial Overview**: Tutorial system overview and structure
- **Tutorial 1**: Basic NLM usage and setup
- **Tutorial 2**: Brain configuration and customization
- **Tutorial 3**: Simulation control and monitoring
- **Tutorial 4**: Visualization and debugging
- **Tutorial 5**: Plugin development and usage

## 🎯 Why Tutorials Matter

Tutorials are crucial for NLM because:

1. **Learning**: Provide structured learning path for new users
2. **Skill Building**: Build skills through hands-on practice
3. **Understanding**: Deepen understanding through practical examples
4. **Application**: Apply knowledge through real-world examples
5. **Confidence**: Build confidence through step-by-step guidance

## 📚 Tutorial Overview

### Tutorial System Overview

NLM tutorial system provides comprehensive step-by-step instructions for learning NLM:

1. **Tutorial 1**: Basic NLM usage and setup
2. **Tutorial 2**: Brain configuration and customization
3. **Tutorial 3**: Simulation control and monitoring
4. **Tutorial 4**: Visualization and debugging
5. **Tutorial 5**: Plugin development and usage

### Tutorial Structure

Each tutorial follows a consistent structure:

1. **Introduction**: Tutorial introduction and objectives
2. **Prerequisites**: Prerequisites and requirements
3. **Setup**: Setup and preparation steps
4. **Tutorial**: Step-by-step tutorial instructions
5. **Conclusion**: Tutorial conclusion and next steps

## 📖 Tutorial 1: Basic NLM Usage

This tutorial provides step-by-step instructions for basic NLM usage and setup.

### Objectives

By the end of this tutorial, you will be able to:

1. **Install NLM**: Install NLM on your system
2. **Create a brain**: Create a brain instance
3. **Initialize brain**: Initialize the brain
4. **Run simulation**: Run a simulation
5. **Analyze results**: Analyze simulation results

### Prerequisites

Before you begin, ensure you have:

- **NLM installed**: NLM installed on your system
- **Python installed**: Python 3.8+ installed
- **CMake installed**: CMake 3.16+ installed
- **Build tools**: Make or Ninja installed

### Setup

#### Option A: Install from Source

This is recommended for full functionality:

```bash
# Clone the repository
git clone https://github.com/your-repo/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Install (optional)
# sudo make install
```

#### Option B: Quick Python Setup

If you only want the Python bindings:

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Build and install Python package
pip install .

# Or install in development mode
pip install -e .
```

#### Option C: Using Docker

If Docker is available, use the pre-built image:

```bash
# Pull Docker image
docker pull your-repo/nlm:latest

# Run container
docker run -it your-repo/nlm:latest /bin/bash
```

### Quick Start

If you want the absolute quickest way to test NLM:

```bash
# Clone and build
mkdir nlm_workspace && cd nlm_workspace
git clone https://github.com/your-repo/nlm.git
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Test the demo
./nlm_phase6_demo
```

That's it! You've just run the Phase 6 integration test.

### Basic NLM Usage

#### Create a Brain

Create a simple brain:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

print(f"✅ Brain created and initialized successfully")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")
print(f"   Total synapses: {brain.getTotalSynapseCount()}")
```

#### Run a Simulation

Run a simple simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Running simulation...")
for step in range(100):
    brain.step(step)
    
    # Print progress
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("✅ Simulation completed successfully!")
```

#### Create a World and Agent

Create a world and agent for environment interaction:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,           # World width
    height=20,          # World height
    visionWidth=8,      # Agent's visual field width
    visionHeight=8      # Agent's visual field height
)

# Reset world to initial state
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable important subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("✅ Environment and agent created successfully!")
```

#### Run a Complete Simulation

Run a complete simulation with agent and world:

```python
import pynlm

# Create everything
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Running complete agent simulation...")

# Run simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Do the action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("✅ Agent simulation completed successfully!")
```

### Analyze Results

Analyze simulation results:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Analyze results
print("Simulation Results:")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Get memory statistics
if brain.hasWorkingMemory():
    working_memory = brain.getWorkingMemory()
    print(f"Working memory active: {working_memory.isActive()}")

if brain.hasEpisodicMemory():
    episodic_memory = brain.getEpisodicMemory()
    print(f"Episodic memories: {episodic_memory.getMemoryCount()}")
```

### Troubleshooting

#### Common Issues

**Issue: "Brain.initialize() failed"**

**Solution**: Check configuration and dependencies:

```bash
# Check for configuration errors
nlm --validate-config config.json

# Fix configuration errors
cat config.json
# Check for:
# - Invalid values
# - Missing required parameters
# - Incorrect data types
```

**Issue: "ImportError: No module named 'pynlm'"**

**Solution**: Check Python installation:

```bash
# Check Python version
python3 --version

# Check if NLM is installed
python3 -c "import pynlm; print(pynlm.__version__)"

# Install NLM if not installed
pip3 install pynlm
```

**Issue: "No neurons firing"**

**Solution**: Check neuron configuration:

```python
import pynlm

# Create configuration with appropriate parameters
config = pynlm.createDefaultConfig()
config.set("brain.v_thresh", -60.0)  # Lower threshold (easier to fire)
config.set("brain.v_rest", -70.0)   # Rest potential

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Test with input
brain.receiveSensoryInput([0.1, 0.2, 0.3, 0.4])  # Provide input
brain.step(0)

print(f"Firing neurons after input: {brain.getFiringNeuronCount()}")
```

### Conclusion

This tutorial covered:

1. **Installation**: Installing NLM on your system
2. **Basic Usage**: Creating and using basic NLM components
3. **Simulation**: Running simulations with agents and worlds
4. **Analysis**: Analyzing simulation results
5. **Troubleshooting**: Common issues and solutions

### Next Steps

For more advanced tutorials:

1. **Tutorial 2**: Learn about brain configuration and customization
2. **Tutorial 3**: Learn about simulation control and monitoring
3. **Tutorial 4**: Learn about visualization and debugging
4. **Tutorial 5**: Learn about plugin development and usage

## 🔗 Related Resources

### Documentation
- **README.md**: Project overview and installation
- **getting-started.md**: Getting started guide
- **api-reference.md**: Complete API documentation
- **configuration.md**: Configuration guide
- **best-practices.md**: Best practices

### Examples
- **examples/visualization_example.py**: Visualization example

### Code
- **src/brain/**: Core neural components
- **src/agent/**: Agent integration layer
- **src/world/**: Environment simulation

## 📊 Tutorial Metrics

### Tutorial Completion Metrics

| Tutorial | Target Completion Time | Current Status |
|----------|------------------------|----------------|
| Tutorial 1 | < 30 minutes | ✅ Completed |
| Tutorial 2 | < 45 minutes | ⏳ In Progress |
| Tutorial 3 | < 60 minutes | ⏳ In Progress |
| Tutorial 4 | < 60 minutes | ⏳ In Progress |
| Tutorial 5 | < 90 minutes | ⏳ In Progress |

### User Satisfaction Metrics

| Metric | Target | Current |
|--------|--------|---------|
| User satisfaction | > 90% | - |
| Tutorial completion rate | > 80% | - |
| Tutorial difficulty | < medium | - |
| Tutorial clarity | > 90% | - |

This completes Tutorial 1. It provides comprehensive step-by-step instructions for basic NLM usage, including installation, setup, basic usage, simulation, and analysis.

## 🎯 Tutorial Summary

Tutorial 1 covers:

- **Installation**: Installing NLM on your system
- **Basic Usage**: Creating and using basic NLM components
- **Simulation**: Running simulations with agents and worlds
- **Analysis**: Analyzing simulation results
- **Troubleshooting**: Common issues and solutions

## 🔄 Next Steps

1. **Complete Tutorial 2**: Learn about brain configuration and customization
2. **Practice**: Practice what you've learned
3. **Experiment**: Experiment with different configurations
4. **Explore**: Explore advanced features

Following these steps will help you successfully complete Tutorial 1 and continue learning NLM.🧠✨