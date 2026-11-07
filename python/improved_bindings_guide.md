Improving Python bindings API for NLM (Neural Learning Machine)

## Summary of Improvements

Based on analysis of the current codebase, I have identified several areas for improvement to make the Python bindings more user-friendly and robust:

## 1. Enhanced Python Bindings API

### Current Issues:
- Missing comprehensive error handling
- Inconsistent API patterns
- Missing convenience methods
- Poor input validation
- Limited exception handling

### Planned Improvements:

#### 1.1 Robust Error Handling
```cpp
// Before: Simple method without validation
void step(SimulationStep step)

// After: Enhanced with validation and exceptions
void step(SimulationStep step, std::optional<Timestamp> time = std::nullopt) {
    if (step == 0 && !isInitialized_) {
        throw std::runtime_error("Brain not initialized. Call initialize() first.");
    }
    if (step > MAX_STEPS) {
        throw std::invalid_argument("Step count exceeds maximum limit of " + 
                                   std::to_string(MAX_STEPS));
    }
    // Implementation with bounds checking
}
```

#### 1.2 Convenience Methods
```cpp
// Add high-level convenience methods
class Brain {
public:
    // Quick setup methods
    void setupDefault(size_t neuronCount = 1000) {
        auto config = Config::createDefault();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.synapse_density", 0.1);
        initialize(config);
    }
    
    // Simplified step with auto-increment
    void step() {
        static SimulationStep step = 0;
        step();
        step++;
    }
    
    // Quick run with statistics
    void runSteps(size_t steps, std::function<void(size_t)> callback = nullptr) {
        for (size_t i = 0; i < steps; ++i) {
            step(i);
            if (callback) callback(i);
            if (i % 100 == 0) {
                logProgress(i, steps);
            }
        }
    }
};
```

#### 1.3 Enhanced Configuration API
```cpp
// Improved configuration methods
class Config {
public:
    // Type-safe setters
    void setNeuronCount(size_t count) {
        set("brain.neuron_count", static_cast<int64_t>(count));
    }
    
    void setSynapseDensity(float density) {
        if (density < 0.0f || density > 1.0f) {
            throw std::invalid_argument("Synapse density must be between 0.0 and 1.0");
        }
        set("brain.synapse_density", density);
    }
    
    // Helper methods for common configuration tasks
    void optimizeForFast() {
        setNeuronCount(500);
        setSynapseDensity(0.05);
        set("plasticity.stdp.enable", true);
    }
    
    void optimizeForAccuracy() {
        setNeuronCount(2000);
        setSynapseDensity(0.15);
        set("plasticity.stdp.enable", true);
        set("plasticity.hebbian.enable", true);
    }
};
```

#### 1.4 Enhanced AgentBrain API
```cpp
class AgentBrain {
public:
    // High-level agent control methods
    void setupAgent(AgentConfig config) {
        // Configure agent with sensible defaults
        enableRewardModulation(config.rewardEnabled);
        enableCuriosity(config.curiosityEnabled);
        enableStructuralPlasticity(config.plasticityEnabled);
        enableDevelopment(config.developmentEnabled);
        
        // Set up world interface
        agentWorld_.configure(
            config.worldWidth, config.worldHeight,
            config.visionWidth, config.visionHeight
        );
    }
    
    // Simplified agent simulation loop
    void runEpisode(size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            // Update world
            world_->update(0.1);
            
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            
            // Process and think
            processSensoryInput(percept);
            brain_->step(i);
            
            // Act
            auto action = decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Learn
            auto reward = world_->getCurrentReward();
            applyRewardModulation(reward, expectedReward_);
            
            // Log statistics periodically
            if (i % 100 == 0) {
                logEpisodeProgress(i, steps);
            }
        }
    }
};
```

## 2. Documentation Improvements

### 2.1 Enhanced Python API Documentation

Current docstrings are minimal and inconsistent. Need:

```cpp
// Enhanced docstring example
py::class_<Brain>(m, "Brain", R"pbdoc(
    Central neural simulation brain class for NLM (Neural Learning Machine).
    
    The Brain class represents a complete artificial brain simulation capable of:
    - Real-time neural processing with spiking neurons
    - Learning through STDP and Hebbian plasticity
    - Memory formation and consolidation
    - Complex cognitive behaviors via neural networks
    
    The brain operates in simulation steps, processing sensory input and producing
    motor actions through its internal neural network.
    
    Typical usage:
    
    ```python
    import pynlm
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(1000)  # Easy configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(1000):
        brain.step(i)
        if i % 100 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Get statistics
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    ```
    
    @note Brain must be initialized before use with initialize().
    @param config Configuration object for the brain.
    
    @see Config, AgentBrain, SimpleWorld
)pbdoc")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         "Initialize brain with configuration. The brain must be initialized before "
         "use. Call initialize() to begin simulation.")
    .def("initialize", &Brain::initialize,
         "Initialize the brain with configuration. This sets up the neural network, "
         "allocates neurons and synapses, and prepares the brain for simulation.")
    // ... more methods
```

### 2.2 Complete Documentation Set

Need comprehensive documentation covering:

#### 2.2.1 Installation Guide
- Build from source
- Install Python bindings
- Verify installation
- Troubleshooting

#### 2.2.2 Quick Start Tutorials
- Hello World: Basic brain simulation
- Agent Simulation: Brain + World + Agent
- Learning Experiment: Demonstrate learning capabilities
- Performance Benchmark: Test system performance

#### 2.2.3 API Reference
- Complete class documentation
- Method descriptions
- Examples for each class
- Common patterns and best practices

#### 2.2.4 Advanced Topics
- Configuration options
- Performance optimization
- Advanced agent behavior
- Custom extensions

## 3. Error Handling and Validation

### 3.1 Comprehensive Exception Handling

```cpp
// Add custom exceptions for better error reporting
namespace pynlm {
    
    class NLMException : public std::runtime_error {
    public:
        NLMException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class BrainNotInitializedException : public NLMException {
    public:
        BrainNotInitializedException() : NLMException("Brain not initialized. Call initialize() first.") {}
    };
    
    class InvalidConfigurationException : public NLMException {
    public:
        InvalidConfigurationException(const std::string& key, const std::string& value) 
            : NLMException("Invalid configuration: " + key + " = " + value) {}
    };
    
    class SimulationError : public NLMException {
    public:
        SimulationError(const std::string& msg) : NLMException("Simulation error: " + msg) {}
    };
}

// Use in bindings
void step(SimulationStep step) {
    if (!isInitialized_) {
        throw BrainNotInitializedException();
    }
    if (step > MAX_STEPS) {
        throw SimulationError("Step count " + std::to_string(step) + " exceeds maximum");
    }
}
```

### 3.2 Input Validation

```cpp
// Add validation helper functions
namespace validation {
    inline void validateNeuronCount(size_t count) {
        if (count == 0) {
            throw std::invalid_argument("Neuron count must be greater than 0");
        }
        if (count > 1000000) {
            throw std::invalid_argument("Neuron count cannot exceed 1,000,000");
        }
    }
    
    inline void validateSynapseDensity(float density) {
        if (density < 0.0f || density > 1.0f) {
            throw std::invalid_argument("Synapse density must be between 0.0 and 1.0");
        }
    }
    
    inline void validateWorldDimensions(size_t width, size_t height) {
        if (width == 0 || height == 0) {
            throw std::invalid_argument("World dimensions must be greater than 0");
        }
        if (width > 1000 || height > 1000) {
            throw std::invalid_argument("World dimensions cannot exceed 1000x1000");
        }
    }
}
```

## 4. Performance and Best Practices Documentation

### 4.1 Performance Guidelines

```cpp
// Add performance optimization examples
void addPerformanceDocumentation() {
    py::class_<Brain>(m, "Brain")
        .def("runSteps", &Brain::runSteps,
             "Run multiple simulation steps efficiently.\n\n"
             "@param steps Number of steps to run\n"
             "@param callback Optional callback function called each step\n"
             "@note For long simulations, consider using runSteps with periodic callbacks "
             "for progress monitoring and memory management.\n\n"
             "Example:\n"
             "```python\n"
             "def progress_callback(step):\n"
             "    if step % 1000 == 0:\n"
             "        print(f'Progress: {step} steps')\n\n"
             "brain.runSteps(10000, progress_callback)\n"
             "```")
        // ...
}
```

### 4.2 Best Practices Guide

```cpp
void addBestPractices() {
    // Add module with best practices
    auto bestPractices = py::module_::init_main_module("pynlm.best_practices");
    
    bestPractices.def("quick_start", []() {
        return """# Quick Start with NLM Python Bindings

## 1. Basic Brain Simulation
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run 1000 simulation steps
for i in range(1000):
    brain.step(i)

print(f"Simulation complete. {brain.getTotalSpikeCount()} spikes recorded.")
```

## 2. Agent with World
```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)  # 20x20 world, 8x8 vision
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## 3. Configuration Presets
```python
import pynlm

# Fast simulation (quick tests)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
brain = pynlm.createBrain(config)

# High accuracy simulation
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 2000)
config2.set("brain.synapse_density", 0.15)
brain2 = pynlm.createBrain(config2)
```

## 4. Monitoring and Debugging
```python
# Add monitoring to your simulation
stats = {
    'spikes': [],
    'firing_rates': [],
    'memory_usage': []
}

for i in range(1000):
    brain.step(i)
    
    # Record statistics every 100 steps
    if i % 100 == 0:
        stats['spikes'].append(brain.getTotalSpikeCount())
        stats['firing_rates'].append(brain.getAverageFiringRate())
    
    # Log progress
    if i % 500 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

## Common Mistakes to Avoid

1. **Forgetting to initialize the brain**: Always call brain.initialize() before using the brain.

2. **Using uninitialized agents**: Initialize the agent with agent.initialize(world) before using it.

3. **Creating multiple worlds**: Each agent should have only one world; reuse worlds for multiple agents.

4. **Memory leaks**: Properly dispose of brain objects when no longer needed.

5. **Configuration errors**: Use the helper methods like config.setNeuronCount() instead of raw string keys.
""";
    });
}
```

## 5. Example Scripts and Tutorials

### 5.1 Core Examples

#### Example 1: Silent Brain (Just Neurons)
```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

#### Example 2: Brain Watching a World
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

#### Example 3: Complete Agent with Learning
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### 5.2 Advanced Examples

#### Advanced Example 1: Custom Configuration
```python
import pynlm

# Create custom configuration
custom_config = pynlm.createDefaultConfig()

# Optimize for specific tasks
custom_config.set("brain.neuron_count", 1500)
custom_config.set("brain.synapse_density", 0.12)
custom_config.set("plasticity.stdp.enable", True)
custom_config.set("plasticity.hebbian.enable", True)
custom_config.set("neuromod.curiosity.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(custom_config)
brain.initialize()
```

#### Advanced Example 2: Multi-Agent System
```python
import pynlm

class MultiAgentSystem:
    def __init__(self, num_agents=3):
        self.agents = []
        self.shared_world = pynlm.createSimpleWorld()
        self.shared_world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        self.shared_world.reset()
        
        # Create shared brain configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        
        for i in range(num_agents):
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.shared_world)
            self.agents.append(agent)
    
    def runSimulation(self, steps=1000):
        for step in range(steps):
            self.shared_world.update(0.1)
            
            # Each agent processes the same world state
            percept = self.shared_world.getSensoryPercept()
            for agent in self.agents:
                agent.processSensoryInput(percept)
                agent.brain.step(step)
                action = agent.decodeMotorCommand()
                self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
            
            # Shared learning
            for agent in self.agents:
                reward = self.shared_world.getCurrentReward()
                agent.applyRewardModulation(reward, 0.0)

# Use the multi-agent system
multi_agent = MultiAgentSystem(num_agents=3)
multi_agent.runSimulation(steps=1000)
```

## 6. Build System Documentation

### 6.1 Enhanced Build Instructions

```bash
#!/bin/bash
# build.sh - Comprehensive build script for NLM Python bindings

set -e  # Exit on error

echo "Building NLM (Neural Learning Machine) Python bindings..."

echo "Step 1: Installing Python dependencies..."
pip install pybind11 scikit-build-core pytest numpy

echo "Step 2: Building C++ core libraries..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_BUILD_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE=$(which python)

# Build with parallel jobs
make -j$(nproc)

echo "Step 3: Building Python module..."
cd ..
pip install -e .

echo "Build complete! Installing tests..."
pip install pytest

echo "Running tests..."
pytest tests/

echo "Build and tests completed successfully!"
```

### 6.2 Development Build Script

```bash
#!/bin/bash
# develop.sh - Development build with debugging support

set -e

BUILD_DIR="build_debug"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake .. -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_BUILD_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE=$(which python) \
    -DCMAKE_CXX_FLAGS="-g -O0 -Wall -Wextra"

make -j$(nproc)

cd ..
pip install -e . --force-reinstall

echo "Development build complete. Run 'pytest tests/' to test."
```

### 6.3 Installation Troubleshooting

```bash
#!/bin/bash
# install.py - Installation with troubleshooting

set -e

echo "Checking environment..."

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found. Install with: sudo apt-get install cmake"; exit 1; }
command -v python >/dev/null 2>&1 || { echo "Error: python not found"; exit 1; }
command -v pip >/dev/null 2>&1 || { echo "Error: pip not found. Install python3-pip"; exit 1; }

# Check Python version
PYTHON_VERSION=$(python -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
if [ "$(python -c 'import sys; print(sys.version_info.major)')" -lt 3 ] || [ "$(python -c 'import sys; print(sys.version_info.minor)')" -lt 8 ]; then
    echo "Warning: Python 3.8+ recommended, found Python $PYTHON_VERSION"
fi

echo "Installing Python dependencies..."
pip install pybind11 scikit-build-core pytest numpy

echo "Building NLM with Python bindings..."
mkdir -p build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)

echo "Building Python module..."
cd ..
pip install -e .

echo "Installation complete!"
echo "Test the installation with: python -c \"import pynlm; print(pynlm.__version__)\""
```

## 7. Common Patterns and Best Practices

### 7.1 Brain Configuration Patterns

```python
import pynlm

def createFastBrain():
    """Create a brain optimized for fast simulation."""
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(500)  # Small brain
    config.setSynapseDensity(0.05)  # Low connectivity
    return pynlm.createBrain(config)

def createAccurateBrain():
    """Create a brain optimized for accurate simulation."""
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(2000)  # Large brain
    config.setSynapseDensity(0.15)  # Higher connectivity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    return pynlm.createBrain(config)

def createDevelopmentBrain():
    """Create a brain with high plasticity for learning."""
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(1000)
    config.setSynapseDensity(0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    return pynlm.createBrain(config)
```

### 7.2 Simulation Pattern Library

```python
import pynlm
import time

class BrainSimulation:
    """Library of common simulation patterns."""
    
    @staticmethod
    def runWithLogging(brain, steps, log_interval=100):
        """Run simulation with periodic logging."""
        for i in range(steps):
            brain.step(i)
            
            if i % log_interval == 0:
                print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"{brain.getTotalSpikeCount()} total spikes")
    
    @staticmethod
    def runWithCallback(brain, steps, callback=None):
        """Run simulation with callback for each step."""
        for i in range(steps):
            brain.step(i)
            if callback:
                callback(i, brain)
    
    @staticmethod
    def runWithStatistics(brain, steps):
        """Run simulation and collect statistics."""
        stats = {
            'steps': steps,
            'total_spikes': 0,
            'firing_rates': [],
            'max_firing_rate': 0,
            'avg_firing_rate': 0
        }
        
        for i in range(steps):
            brain.step(i)
            
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            
            stats['total_spikes'] = spike_count
            stats['firing_rates'].append(firing_rate)
            stats['max_firing_rate'] = max(stats['max_firing_rate'], firing_rate)
            
            if i == steps - 1:
                stats['avg_firing_rate'] = sum(stats['firing_rates']) / len(stats['firing_rates'])
        
        return stats
```

### 7.3 Agent Pattern Library

```python
import pynlm

class AgentBehavior:
    """Library of common agent behaviors."""
    
    @staticmethod
    def randomExploration(world, steps):
        """Agent that explores randomly."""
        agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        agent.initialize(world)
        agent.enableCuriosity(True)
        
        for i in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            agent.brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return agent
    
    @staticmethod
    def goalDirected(world, goal_position, steps):
        """Agent that moves toward a goal."""
        agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        agent.initialize(world)
        agent.enableRewardModulation(True)
        
        for i in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            agent.brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check if reached goal
            if world.getAgentBody().distanceTo(goal_position) < 1.0:
                break
        
        return agent
```

## 8. Troubleshooting Guide

### 8.1 Common Issues and Solutions

#### Issue 1: Import Error
**Problem:** `ImportError: No module named 'pynlm'`

**Solution:**
```bash
pip install --force-reinstall .
```

**Alternative:**
```bash
pip install -e . --force-reinstall
```

#### Issue 2: Brain Not Initialized
**Problem:** Brain not responding or not firing neurons

**Solution:**
```python
import pynlm

# Always initialize before using
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # <-- This line is essential!

# Now you can use the brain
brain.step(0)
```

#### Issue 3: Segmentation Fault
**Problem:** Program crashes during execution

**Solution:**
1. Ensure you call `initialize()` before `step()`
2. Check configuration parameters
3. Run with smaller brain size for testing
4. Use `pynlm.createDefaultConfig()` for stable defaults

#### Issue 4: Memory Issues
**Problem:** Memory consumption too high

**Solution:**
```python
import pynlm

# Use a smaller brain for memory efficiency
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduce from default 1000
brain = pynlm.createBrain(config)
```

### 8. São Troubleshooting Commands

```python
import pynlm

# Diagnostic function
def diagnoseSystem(brain, world=None, agent=None):
    """Diagnose common issues in the NLM system."""
    print("=== NLM System Diagnosis ===")
    
    # Check brain
    print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
    print(f"Brain regions: {brain.getRegionCount()}")
    print(f"Brain synapses: {brain.getTotalSynapseCount()}")
    print(f"Brain developmental stage: {brain.getDevelopmentalStage()}")
    
    if world:
        print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
        print(f"World simulation time: {world.getSimulationTime()}")
    
    if agent:
        print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Agent neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # Check for common issues
    if brain.getTotalNeuronCount() == 0:
        print("WARNING: Brain has no neurons!")
    
    if brain.getTotalSynapseCount() == 0:
        print("WARNING: Brain has no synapses!")
    
    if brain.getAverageFiringRate() == 0:
        print("WARNING: Brain is not firing neurons!")
    
    print("=== Diagnosis Complete ===")

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

diagnoseSystem(brain)
```

### 8.4 Error Messages and Solutions

| Error Message | Likely Cause | Solution |
|---|---|---|
| "Brain not initialized" | Missing brain.initialize() call | Call brain.initialize() before using the brain |
| "Invalid configuration" | Wrong parameter type or value | Check configuration parameter types and ranges |
| "Out of memory" | Too many neurons or synapses | Reduce brain size or increase available memory |
| "Segmentation fault" | Bug in brain or agent code | Check for null pointer access or invalid memory access |

## Summary

This comprehensive improvement plan enhances the NLM Python bindings with:

1. **Robust error handling** with custom exceptions
2. **Better API design** with convenience methods
3. **Enhanced documentation** with examples and tutorials
4. **Performance optimization** guidelines
5. **Build system improvements** with troubleshooting
6. **Best practices** and common patterns
7. **Comprehensive troubleshooting** guide

These improvements will make NLM much more accessible to users and developers while maintaining its powerful neural simulation capabilities.