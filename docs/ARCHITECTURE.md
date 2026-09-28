# NLM - Neural Learning Machine Documentation

## Overview

NLM (熙然) is a brain-inspired spiking neural network simulator written in C++20. It implements biologically plausible neural computation with:

- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural plasticity)
- Neuromodulation (dopamine, curiosity, novelty)
- Memory systems (working memory, episodic memory)
- Prediction and cognitive systems
- Interactive world simulation

## Quick Start

### Basic Usage

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

// Create configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000LL);
config->set("region_count", 2LL);
config->set("connection_probability", 0.1f);
config->set("random_seed", 42ULL);

// Create and initialize brain
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

// Simulate
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step, step * 0.001);
}
```

### Python Bindings

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
```

## Architecture Overview

### Core Components

1. **Core Layer** (`src/core/`)
   - `Types/`: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
   - `Config/`: Configuration management system (JSON/YAML support)
   - `Random/`: Deterministic random number generator
   - `Logger/`: Advanced logging with statistics and debugging
   - `SimulationClock/`: Time management for simulation

2. **Brain Layer** (`src/brain/`)
   - `Neuron/`: Individual neuron implementation (LIF dynamics)
   - `Synapse/`: Synaptic connection representation
   - `NeuralPopulation/`: Group of neurons with shared properties
   - `NeuralRegion/`: Brain region with multiple populations
   - `Brain/`: Central coordinator with all subsystems

3. **Dynamics Layer** (`src/dynamics/`)
   - `NeuralDynamics/`: Abstract base for neuron dynamics
   - `SpikeSystem/`: Event-driven spike processing

4. **Plasticity Layer** (`src/plasticity/`)
   - `PlasticityRule/`: Abstract base for plasticity rules
   - `Hebbian/`: Enhanced Hebbian learning rule
   - `STDP/`: Spike-Timing-Dependent Plasticity
   - `StructuralPlasticity/`: Synapse formation/removal (synaptogenesis/pruning)

5. **Development Layer** (`src/development/`)
   - `DevelopmentSystem/`: Orchestrates development stages
   - `Synaptogenesis/`: New synapse formation
   - `Pruning/`: Weak synapse removal
   - `Maturation/`: Adult property development

6. **Neuromodulation Layer** (`src/neuromodulation/`)
   - `Neuromodulator/`: Abstract base for neuromodulators
   - `Dopamine/`: Reward and reinforcement
   - `Novelty/`: Novelty detection
   - `Curiosity/`: Exploration motivation
   - `PredictionError/`: Error signals for learning

7. **Memory Layer** (`src/memory/`)
   - `NeuralWorkingMemory/`: Persistent activity for temporary storage
   - `NeuralEpisodicMemory/`: Experience encoding in neural patterns
   - `NeuralAssociativeMemory/`: Experience-based associations

8. **Prediction Layer** (`src/prediction/`)
   - `NeuralPrediction/`: Temporal sequence learning
   - `PredictionSystem/`: Neural prediction system

9. **Cognitive Layer** (`src/cognition/`)
   - `AttentionalSelection/`: Competitive attention mechanism
   - `NeuralPlanner/`: Predictive action selection
   - `ConceptFormation/`: Pattern discovery without labels

10. **Sensory Layer** (`src/sensory/`)
    - `SensoryInput/`: Base class for sensory data
    - `Vision/`: Visual input processing
    - `Audio/`: Audio input processing
    - `InternalSignals/`: Internal state signals

11. **Motor Layer** (`src/motor/`)
    - `MotorSystem/`: Neural-to-action conversion
    - `Action/`: Action representation

12. **Environment Layer** (`src/environment/`)
    - `Environment/`: Abstract environment interface
    - `Observation/`: Sensory observation wrapper
    - `Action/`: Action wrapper

13. **World Layer** (`src/world/`)
    - `SimpleWorld/`: Interactive world simulation

14. **Agent Layer** (`src/agent/`)
    - `AgentBrain/`: Interface between brain and world
    - `SensoryPercept/`: Sensory percept representation
    - `AgentBody/`: Agent body representation

15. **Experiments Layer** (`src/experiments/`)
    - `Phase4Experiment/`: Phase 4 cognitive experiments
    - `ExperimentRunner/`: Experiment execution framework
    - `Metrics/`: Data analysis and visualization

## Key Features

### 1. Biological Plausibility

- **LIF Neurons**: Real integrate-and-fire dynamics with adaptation
- **Synaptic Delays**: Realistic spike transmission delays
- **Multiple Plasticity Rules**: STDP, Hebbian, reward-modulated, structural
- **Biologically-Inspired Parameters**: Realistic ranges for biological parameters

### 2. Advanced Learning Capabilities

- **Event-Driven Computation**: Efficient spike processing
- **Real-Time Integration**: Variable timestep support
- **Multi-Timescale Processing**: Fast synaptic dynamics and slow plasticity
- **Hierarchical Learning**: Multiple levels of abstraction

### 3. Interactive Environment

- **World Simulation**: Interactive 2D environment
- **Sensory Processing**: Multi-modal sensory input
- **Motor Control**: Action execution and feedback
- **Social Interaction**: Observation and imitation

### 4. Cognitive Abilities

- **Prediction**: Temporal sequence learning and prediction
- **Memory**: Working memory, episodic memory, concept formation
- **Attention**: Selective processing through competition
- **Planning**: Multi-step action planning
- **Self-Model**: Sensorimotor self-awareness

### 5. Development and Maturation

- **Developmental Stages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Experience-Dependent Development**: Brain changes through experience
- **Synaptogenesis**: Activity-dependent synapse formation
- **Pruning**: Competition-based synapse elimination

## Advanced Features

### 1. Enhanced Configuration System

- **Multiple Formats**: Support for JSON, YAML, and simple key=value format
- **Runtime Configuration**: Dynamic parameter adjustment
- **Source Tracking**: Configuration source attribution (Default, File, CommandLine, Runtime)
- **Type Safety**: Strongly-typed configuration values

```cpp
// JSON configuration
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    }
}

// YAML configuration
neuron_count: 1000
region_count: 2
connection_probability: 0.1
plasticity:
  stdp:
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
```

### 2. Advanced Logging System

- **Real-Time Statistics**: Performance monitoring and analytics
- **Session Tracking**: Complete session reporting
- **Advanced Filtering**: Pattern-based log filtering
- **Performance Monitoring**: Log rate tracking and reporting
- **Multi-Destination**: Console, file, and custom logger support
- **NLM-Specific Logging**: Neural event, plasticity updates, experimental results

```cpp
// Configure advanced logging
logger.configureAdvancedFeatures(true, true, false, 1000);

// Neural event logging
logger.neuralEvent("spike", "neuron_42", "threshold reached, potential: -52.3mV");

// Plasticity update logging
logger.plasticityUpdate("STDP", 0.001234, "synapse_123");

// Performance reporting
logger.performanceReport("simulation_speed", 1500.0, "steps/second");

// Experimental results
logger.experimentalResult("phase4_cognitive", true, "Accuracy: 85.3%");
```

### 3. Enhanced Hebbian Plasticity

- **Covariance Learning**: Proper covariance calculation with time derivatives
- **Adaptive Learning Rate**: Activity-dependent learning rate adjustment
- **Eligibility Traces**: Support for reward-modulated learning
- **Synaptic Efficacy**: Use-dependent modulation of learning
- **Enhanced Bounds**: Proper weight clamping and stability mechanisms

### 4. Neuron Real-Time Integration

- **Variable Timestep**: Support for adaptive time stepping
- **Enhanced Dynamics**: Improved subthreshold dynamics and after-spike effects
- **Precision Timing**: Accurate spike timing and refractory period handling
- **Real-Time Clock Integration**: Synchronization with simulation time

### 5. Comprehensive Testing Suite

- **Unit Tests**: Individual component testing
- **Integration Tests**: System-level testing
- **Performance Tests**: Benchmarking and optimization validation
- **Regression Tests**: Change detection and prevention

## Building and Installation

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Building with Python Bindings

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python package
pip install .

# Or install in development mode
pip install -e .
```

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running Examples

### Phase 2: Neural Computation

```bash
./nlm
```

This runs the Phase 2 demonstration showing real neural computation with:
- LIF neuron dynamics
- STDP and Hebbian plasticity
- Structural plasticity
- Learning experiments

### Phase 3: World Interaction

```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

Example:
```bash
./nlm_phase3_demo 3 500
```

### Phase 4: Emerging Cognition

```bash
./nlm_phase4_demo [num_trials]
```

Example:
```bash
./nlm_phase4_demo 100
```

### Performance Benchmark

```bash
./nlm_benchmark
```

This runs performance benchmarks for different network sizes.

## API Documentation

### Core API

#### Configuration

```cpp
class Config {
public:
    // Create default configuration
    static std::shared_ptr<Config> createDefaultConfig();
    
    // Set configuration values
    void set(const std::string& key, const ConfigValue& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // Get configuration values
    template<typename T> std::optional<T> get(const std::string& key) const;
    template<typename T> T getOr(const std::string& key, const T& defaultValue) const;
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
};
```

#### Brain

```cpp
class Brain {
public:
    explicit Brain(std::shared_ptr<Config> config);
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Input/Output
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // Plasticity
    void updatePlasticity();
    void develop();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
};
```

#### Neuron

```cpp
class Neuron {
public:
    explicit Neuron(NeuronId id);
    bool step(Timestamp currentTime);
    bool step(Timestamp currentTime, TimestepDuration dt);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Inputs/Outputs
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    
    // Spike handling
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
};
```

#### Agent Brain (Phase 3/4)

```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    void initialize(SimpleWorld& world);
    
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development
    void updateDevelopment(TimestepDuration dt);
    
    // Subsystem control
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    // Statistics
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    float getNeuromodulationLevel() const;
};
```

### Memory Systems

#### Working Memory

```cpp
class NeuralWorkingMemory {
public:
    void store(const std::vector<float>& pattern, Timestamp timestamp);
    std::vector<float> retrieve(size_t index);
    void decay(float decayRate);
    size_t getCapacity() const;
};
```

#### Episodic Memory

```cpp
class NeuralEpisodicMemory {
public:
    void encode(const std::string& event, const std::vector<float>& pattern);
    std::vector<float> recall(const std::string& event);
    void replay();
    size_t getEpisodeCount() const;
};
```

### Prediction System

```cpp
class NeuralPrediction {
public:
    void learn(const std::vector<float>& sequence);
    std::vector<float> predict(size_t steps);
    float getPredictionError() const;
    void reset();
};
```

## Examples and Tutorials

### 1. Basic Neural Network

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500LL);
    config->set("region_count", 2LL);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42ULL);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step 
                      << ", Spikes: " << brain->getTotalSpikeCount()
                      << ", Firing: " << brain->getFiringNeuronCount()
                      << std::endl;
        }
    }
    
    return 0;
}
```

### 2. World Interaction (Phase 3)

```cpp
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/core/Config/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000LL);
    config->set("region_count", 2LL);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42ULL);
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain interface
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable subsystems
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    // Run episodes
    for (int episode = 0; episode < 5; ++episode) {
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        // Run episode steps
        for (int step = 0; step < 200; ++step) {
            // Get sensory percept from world
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agentBrain.processSensoryInput(percept);
            
            // Run brain step
            brain->step(step, step * 0.001);
            
            // Decode motor command
            nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // Apply motor command to world
            nlm::ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // Apply reward modulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // Update development
            agentBrain.updateDevelopment(0.01);
        }
    }
    
    return 0;
}
```

### 3. Cognitive Experiments (Phase 4)

```cpp
#include <nlm/experiments/Phase4Demo.hpp>

int main(int argc, char* argv[]) {
    size_t numTrials = 50;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    nlm::Phase4Demo demo;
    demo.run(numTrials);
    
    return 0;
}
```

## Best Practices

### 1. Configuration Management

- Use descriptive configuration keys
- Set appropriate random seeds for reproducibility
- Use default configurations as starting points
- Validate configuration parameters

### 2. Memory Management

- Use smart pointers for resource management
- Clear unused data structures regularly
- Monitor memory usage during long simulations
- Use pre-allocated buffers where possible

### 3. Performance Optimization

- Use event-driven processing for sparse activity
- Pre-allocate memory for known sizes
- Use efficient data structures (vectors, arrays)
- Profile bottlenecks regularly

### 4. Debugging and Testing

- Use comprehensive logging for debugging
- Write unit tests for individual components
- Use deterministic random seeds for reproducibility
- Monitor simulation statistics regularly

### 5. Biological Plausibility

- Use biologically realistic parameter ranges
- Validate that learning rates are reasonable
- Ensure that dynamics match known biological phenomena
- Test with different network sizes and configurations

## Troubleshooting

### Common Issues

#### "Failed to initialize brain"

Check that:
- Configuration parameters are valid
- Required files are present
- Memory is available
- System resources are sufficient

#### Performance Issues

Solution:
- Check for memory leaks
- Profile the simulation
- Optimize hot paths
- Use more efficient data structures

#### Numerical Instability

Solution:
- Check parameter ranges
- Add clamping for extreme values
- Use smaller time steps
- Add numerical stabilization

### Getting Help

1. Check the documentation for the specific component
2. Look for examples that match your use case
3. Check the build logs for compilation errors
4. Review the sample configuration files
5. Consult the architecture documentation

## Future Directions

### Planned Enhancements

1. **Extended Plasticity Rules**: More sophisticated learning rules
2. **Advanced Memory Systems**: Long-term memory and forgetting
3. **Social Learning**: Complex social interaction capabilities
4. **Active Vision**: Gaze control and attention
5. **Motor Learning**: Inverse kinematics and motor adaptation
6. **Meta-Learning**: Learning how to learn
7. **Transfer Learning**: Knowledge transfer across tasks
8. **Lifelong Learning**: Continuous adaptation and improvement

### Research Areas

1. **Neural Dynamics**: More realistic neuron models
2. **Developmental Processes**: More sophisticated developmental trajectories
3. **Multi-Modal Integration**: Integration of different sensory modalities
4. **Embodied Cognition**: Robot embodiment and interaction
5. **Social Cognition**: Theory of mind and empathy
6. **Value Learning**: Intrinsic motivation and curiosity
7. **Meta-Cognition**: Self-monitoring and self-regulation

## Citation

If you use NLM in your research, please cite:

```bibtex
@misc{nlm2024,
    title={NLM (熙然) - Neural Learning Machine},
    author={Various Authors},
    year={2024},
    note={An experimental artificial developmental brain with real spiking neural computation}
}
```

## License

NLM is distributed under an open source license. See LICENSE file for details.

## Acknowledgments

This work was inspired by research in computational neuroscience, artificial intelligence, and machine learning. Special thanks to the contributors and the open source community.

---

*Documentation generated automatically from NLM source code and project documentation.*
