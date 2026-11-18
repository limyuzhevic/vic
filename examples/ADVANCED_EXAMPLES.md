# Advanced NLM Example Applications

This document provides an overview of the advanced example applications available in the NLM (Neural Learning Machine) system.

## Overview

NLM provides several advanced example applications that demonstrate different aspects of the brain simulation system. These examples are organized by Phase and demonstrate the capabilities of the integrated artificial brain.

## Available Example Applications

### Phase 3: Basic World Interaction

**File:** `src/experiments/Phase3Demo.cpp`

**Description:** Demonstrates basic sensory-motor integration and world interaction.

**Key Features:**
- Simple 2D world simulation
- Basic vision and action systems
- Reward prediction error learning
- Simple agent behavior

**Use Case:** Introduction to NLM world interaction and basic learning

### Phase 4: Advanced Cognition

**File:** `src/experiments/Phase4Demo.cpp`

**Description:** Demonstrates advanced cognitive capabilities including prediction, planning, and attention.

**Key Features:**
- Neural prediction system
- Working memory and episodic memory
- Concept formation
- Neural attention mechanisms
- Predictive planning
- Self-model development

**Use Case:** Understanding advanced cognitive architectures

### Phase 5: Performance Optimizations

**File:** `src/experiments/Phase5Demo.cpp`

**Description:** Demonstrates performance optimizations and advanced features.

**Key Features:**
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

**Use Case:** Performance benchmarking and optimization studies

### Phase 6: Complete Integration

**File:** `src/experiments/Phase6Demo.cpp`

**Description:** Comprehensive integration test demonstrating all connected brain systems.

**Key Features:**
- Complete brain system integration
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Development and structural plasticity
- Checkpoint save/load functionality
- Memory replay and consolidation

**Use Case:** Verifying complete brain functionality

### Phase 6 Text Demo

**File:** `src/experiments/Phase6TextDemo.cpp`

**Description:** Demonstrates text-as-a-sense capability and language association learning.

**Key Features:**
- Text-as-a-sense processing
- Language association learning
- Concept formation from text
- Social learning capabilities

**Use Case:** Studying language and concept learning in artificial brains

## Additional Example Applications

### Complex Environment Simulation

**File:** `examples/ComplexWorldDemo.cpp` (to be created)

**Description:** Advanced world simulation with complex physics and multiple object types.

**Features:**
- Complex physics simulation
- Multiple interacting object types
- Advanced sensory processing
- Group behavior and swarm intelligence

**Use Case:** Studying emergent behaviors in complex environments

### Learning Agent Studies

**File:** `examples/LearningAgentStudy.cpp` (to be created)

**Description:** Comparative studies of different learning algorithms and plasticity rules.

**Features:**
- Multiple learning algorithm comparisons
- Different plasticity rule configurations
- Performance benchmarking
- Statistical analysis of learning outcomes

**Use Case:** Researching optimal learning strategies

### Network Analysis Tools

**File:** `examples/NetworkAnalysis.cpp` (to be created)

**Description:** Tools for analyzing brain connectivity and dynamics.

**Features:**
- Graph theory analysis
- Network topology analysis
- Connection pattern analysis
- Performance correlation studies

**Use Case:** Understanding brain network organization

### Multi-Agent Systems

**File:** `examples/MultiAgentSystem.cpp` (to be created)

**Description:** Studies of multiple interacting artificial agents.

**Features:**
- Multiple agent coordination
- Social learning and imitation
- Competitive and cooperative behaviors
- Group decision making

**Use Case:** Studying social dynamics and collective intelligence

### Real-world Scenario Simulation

**File:** `examples/RealWorldScenario.cpp` (to be created)

**Description:** Simulation of real-world problems and scenarios.

**Features:**
- Resource management problems
- Predator-prey dynamics
- Traffic flow simulation
- Economic behavior modeling

**Use Case:** Applying NLM to real-world challenges

## Running Examples

### Building Examples

All examples require building the NLM project first:

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running Specific Examples

```bash
# Run Phase 3 demo
./nlm_phase3_demo

# Run Phase 4 demo
./nlm_phase4_demo

# Run Phase 5 demo
./nlm_phase5_demo

# Run Phase 6 demo
./nlm_phase6_demo

# Run Phase 6 text demo
./nlm_phase6_text_demo
```

### Running with Custom Configurations

Examples can be run with custom configuration files:

```bash
# Run with custom configuration
./nlm --config my_config.cfg --steps 5000
```

### Running with Python Bindings

Python bindings provide additional examples and functionality:

```python
import pynlm

# Create and run complex simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()

# Run complex simulation
for step in range(10000):
    brain.step(step)

print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
```

## Example Application Directory Structure

```
examples/
├── ComplexWorldDemo.cpp              # Complex environment simulation
├── LearningAgentStudy.cpp            # Comparative learning studies
├── MultiAgentSystem.cpp              # Multi-agent coordination
├── NetworkAnalysis.cpp               # Network topology analysis
├── RealWorldScenario.cpp             # Real-world problem simulation
├── PerformanceOptimizationStudy.cpp   # Performance benchmarking
├── ResourceManagementScenario.cpp    # Resource allocation problems
├── PredatorPreyDynamics.cpp          # Ecosystem simulation
├── TrafficFlowSimulation.cpp         # Traffic modeling
└── EconomicBehaviorModeling.cpp      # Economic system simulation
```

## Integration with Installation Scripts

All example applications are integrated with the NLM installation scripts:

### Build Script

```bash
# Build all examples
./install.sh --python-bindings --tests

# Build specific example
./install.sh --build-only --target ComplexWorldDemo
```

### Installation Script

```bash
# Install examples
./install.sh --install-only --examples

# Install examples to custom location
./install.sh --install-only --examples --prefix /opt/nlm-examples
```

## Example Application Categories

### 1. Basic Functionality Examples
- Demonstrate core NLM capabilities
- Educational and introductory
- Simple, well-documented code

### 2. Advanced Feature Examples
- Demonstrate complex NLM features
- Show integration of multiple systems
- Research and analysis focused

### 3. Performance Examples
- Focus on performance optimization
- Benchmarking and profiling
- System resource management

### 4. Research Examples
- Enable research and experimentation
- Support comparative studies
- Provide analysis tools

### 5. Application Examples
- Real-world problem solving
- Domain-specific applications
- Industry use cases

## Getting Help with Examples

### Documentation

Each example includes:
- Comprehensive comments
- Usage instructions
- Expected output
- Configuration options
- Performance characteristics

### Additional Resources

- **NLM Tutorial:** Complete tutorial covering all example applications
- **API Reference:** Detailed documentation of NLM API
- **Architecture Guide:** System design and component interactions
- **Configuration Guide:** Setting up and customizing examples

### Troubleshooting

Common issues and solutions:

**Example compilation errors**
- Ensure all dependencies are installed
- Check for required header files
- Verify build configuration

**Example runtime errors**
- Check configuration file paths
- Verify input parameters
- Review example-specific requirements

**Example performance issues**
- Reduce neuron counts for faster execution
- Use debug builds for development
- Profile with system profiling tools

## Example Application Templates

The following templates are available for creating new example applications:

### Basic Example Template

```cpp
// Template for new example applications
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"

int main() {
    // 1. Create and configure brain
    auto config = std::make_shared<Config>();
    // ... configuration settings ...
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // 2. Create agent and world
    auto agent = std::make_shared<AgentBrain>(brain);
    auto world = std::make_shared<SimpleWorld>();
    
    // 3. Initialize systems
    agent->initialize(*world);
    
    // 4. Run simulation
    for (int step = 0; step < numSteps; ++step) {
        // ... simulation loop ...
    }
    
    // 5. Analyze and output results
    // ... analysis ...
    
    return 0;
}
```

### Python Example Template

```python
import pynlm

def run_example(config_file=None):
    """Template for new Python examples"""
    
    # 1. Create configuration
    if config_file:
        config = pynlm.load_config(config_file)
    else:
        config = pynlm.createDefaultConfig()
    
    # 2. Configure as needed
    config.set("brain.neuron_count", 1000)
    
    # 3. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 4. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # 5. Enable features as needed
    agent.enableAllFeatures()
    
    # 6. Initialize systems
    agent.initialize(world)
    
    # 7. Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # 8. Analyze results
    print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
    
    return brain, agent, world

if __name__ == "__main__":
    run_example()
```

## Future Example Applications

New example applications are planned:

### 1. Reinforcement Learning Examples
- Multi-armed bandit problems
- Grid world reinforcement learning
- Continuous control tasks

### 2. Cognitive Development Examples
- Developmental trajectory studies
- Critical period exploration
- Maturation process modeling

### 3. Social Learning Examples
- Cultural transmission studies
- Group learning dynamics
- Knowledge diffusion models

### 4. Economic Behavior Examples
- Market behavior simulation
- Game theory applications
- Resource allocation studies

## Conclusion

The NLM system provides a comprehensive set of example applications that demonstrate the full range of capabilities of the artificial brain. These examples serve as:

1. **Educational tools** for learning NLM concepts
2. **Research platforms** for experimentation
3. **Performance benchmarks** for optimization studies
4. **Application showcases** for real-world problem solving
5. **Documentation references** for API usage

Users can easily extend these examples to create custom applications for their specific research needs and requirements.