# NLM/熙然 - Developer Documentation

**Phase 6: Final Integration and Artificial Brain**

This document provides comprehensive guidance for developers working with the NLM/熙然 neural network/brain simulation project.

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Architecture Overview

### Core Components

1. **Brain (src/brain/)** - The central neural processing engine
   - LIF neuron dynamics and spike-based computation
   - Multiple integrated memory systems (working, episodic, associative)
   - Neuromodulation systems (dopamine, curiosity, novelty)
   - Prediction and cognitive systems
   - Development system

2. **Agent (src/agent/)** - Integration layer between brain and world
   - Sensory transduction (vision, touch, internal signals)
   - Motor decoding and action selection
   - Reward modulation and exploration

3. **World (src/world/)** - Environment interface
   - Simple 2D world simulation
   - Sensory input generation
   - Action application and reward computation

4. **Experiments (src/experiments/)** - Behavioral testing and analysis
   - Integration verification
   - Memory testing
   - Neuromodulation testing
   - Checkpoint and replay testing

### Design Principles

- **Experience-driven**: Learning emerges from interaction, not pre-training
- **Development-driven**: System evolves through developmental stages
- **Neural**: Uses spiking neurons with LIF dynamics
- **Plastic**: Continuous synaptic modification through STDP and Hebbian learning
- **Embodied**: Direct interaction with environment
- **Self-organizing**: Emergent structure through local learning rules
- **Continually learning**: No fixed endpoint, learns throughout lifetime

## Building the Project

### Prerequisites

- CMake 3.16 or higher
- C++20 compiler (GCC 9+, Clang 10+, MSVC 19.2+)
- pthread library (for parallel processing)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4

# Or use ninja if available
ninja
```

### Build Variants

```bash
# Debug build with enhanced logging
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Minimal build (core systems only)
cmake .. -DCORE_ONLY=ON

# With performance optimizations
cmake .. -DOPTIMIZE_PERFORMANCE=ON
```

### Installation

```bash
# Install to system directories (requires sudo)
sudo make install

# Or install to custom prefix
make install DESTDIR=/opt/nlm
```

## Running Demos

### Main Demo (Phase 6 Integration)

```bash
# Default integration demo (5 minutes)
./nlm

# Help and options
./nlm --help

# Different demo modes
./nlm --mode basic           # Basic connectivity test
./nlm --mode plasticity      # Plasticity learning experiment  
./nlm --mode stdp            # STDP verification
./nlm --mode integration     # Full integration demo (default)
./nlm --mode agent           # Neural agent in world

# Custom configuration
./nlm --mode integration \
      --neurons 1000 \
      --steps 5000 \
      --development \
      --checkpoint
```

### Phase 6 Demo (Integration Test)

```bash
# Run the comprehensive Phase 6 integration demo
./nlm_phase6_demo

# With custom configuration
./nlm_phase6_demo --config configs/custom.cfg
```

### Phase 3 and Phase 4 Demos

```bash
# Phase 3 experimental demo
./nlm_phase3_demo

# Phase 4 experimental demo  
./nlm_phase4_demo
```

## Configuration

### Configuration Files

The project uses JSON configuration files located in the `configs/` directory:

- `configs/default.cfg` - Default configuration
- `configs/development.cfg` - Development settings
- `configs/performance.cfg` - Performance optimizations

### Configuration Options

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "development_rate": 0.01,
    "checkpoint_dir": "./checkpoints",
    "checkpoint_interval": 1000,
    "max_checkpoints": 10
}
```

### Command Line Override

Command line arguments override configuration file settings:

```bash
./nlm --config configs/custom.cfg --neuron_count 500 --steps 2000
```

## API Reference

### Core Brain Interface

```cpp
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Neuromodulation
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
    // Development
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage();
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    
    // Logging and debugging
    void logStatus() const;
};
```

### Agent Interface

```cpp
class AgentBrain {
public:
    // Create agent with brain
    AgentBrain(std::shared_ptr<Brain> brain);
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
};
```

### Memory Systems

```cpp
class NeuralWorkingMemory {
public:
    // Store neural activity pattern
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Update working memory
    void update(double timestep);
    
    // Get active neurons in working memory
    std::vector<NeuronId> getMemoryNeurons() const;
    
    // Get number of active traces
    size_t getActiveTraces() const;
    
    // Set capacity
    void setCapacity(size_t capacity);
    
    // Clear working memory
    void clear();
};

class NeuralEpisodicMemory {
public:
    // Store episodic memory
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Get episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count);
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Replay episode
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Consolidate memories
    void consolidate(float threshold);
    
    // Clear episodic memory
    void clear();
};
```

## Extending the System

### Adding New Neural Populations

To add new neuron types to the brain:

1. **Define NeuronType** (in core/Types/Types.hpp)
2. **Create NeuralPopulation** (in src/brain/NeuralPopulation.cpp)
3. **Update Brain.cpp** to include new populations in initialization

Example:

```cpp
// In Brain::initialize()
// Add new population type
auto sensoryPopId = region->addPopulation(neuronsPerRegion / 5, NeuronType::NewType);
```

### Adding New Modulators

To add new neuromodulators:

1. **Implement modulator class** (e.g., Serotonin, Norepinephrine)
2. **Update Neuromodulator.hpp** and .cpp
3. **Initialize in Brain::Impl constructor**
4. **Integrate effects in Brain::step()**

Example:

```cpp
// In Brain::Impl constructor
acetylcholine = std::make_unique<Acetylcholine>();

// In Brain::step()
if (pImpl->acetylcholine) {
    pImpl->acetylcholine->update(pImpl->timestep);
    // Apply ACh effects on attention
    pImpl->attention->setAChLevel(pImpl->acetylcholine->getLevel());
}
```

### Adding New Behaviors

To implement new agent behaviors:

1. **Add new motor command types** (Action.hpp)
2. **Update AgentBrain** to handle new commands
3. **Implement behavior in decodeMotorCommand()**
4. **Add curiosity-based exploration for new behaviors**

Example:

```cpp
// In AgentBrain::decodeMotorCommand()
// Handle new motor commands
if (defaultCmd == MotorCommand::NewBehavior) {
    return selectWithCuriosity(selectNewBehavior());
}
```

### Custom Environments

To create new world types:

1. **Implement SimpleWorld interface** (src/world/SimpleWorld.hpp)
2. **Add sensing methods** (observe())
3. **Implement action application** (applyAction())
4. **Define reward function** (computeReward())

Example:

```cpp
class CustomWorld : public SimpleWorld {
public:
    void initialize(int width, int height) override;
    SensoryPercept observe(Brain* brain) override;
    void applyAction(Brain* brain, MotorCommand cmd) override;
    float computeReward(Brain* brain) override;
};
```

## Development Guidelines

### Code Style

- **Naming**: snake_case for variables, CamelCase for classes
- **Comments**: Doxygen-style for API documentation
- **Indentation**: 4 spaces, no tabs
- **Line length**: Maximum 100 characters
- **Error handling**: Use exceptions for errors, return bool for success/failure

### Testing

The project includes comprehensive test infrastructure:

1. **Unit Tests** (in tests/ directory)
   - Neural dynamics
   - Plasticity rules
   - Memory systems
   - Neuromodulation

2. **Integration Tests** (in experiments/ directory)
   - Phase 3: Basic neural computation
   - Phase 4: Complex behaviors
   - Phase 5: Performance optimizations
   - Phase 6: Complete integration

3. **Manual Testing**
   - Demos with visual output
   - Experiment parameters
   - Configuration validation

### Performance Considerations

- **Memory pools**: Use provided memory pools for efficiency
- **Event-driven**: Event queues for spike propagation
- **Sparse connectivity**: Exploit sparse synaptic connections
- **Parallel processing**: SIMD instructions where possible
- **Checkpointing**: Use for long-running experiments

### Debugging

1. **Logging levels**: Use NLM_LOG_INFO, NLM_LOG_WARNING, NLM_LOG_ERROR
2. **Visualization**: Use visualization interface for neural activity
3. **Checkpointing**: Save brain state for debugging
4. **Profiling**: Built-in performance metrics
5. **Memory debugging**: Memory pool leak detection

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build system
├── README.md                         # User documentation
├── docs/                             # Developer documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   ├── EXPERIMENTS.md                # Experimental protocols
│   ├── LIMITATIONS.md                # Known limitations
│   └── PHASE6_FINAL_AUDIT.md         # Current system audit
├── src/                              # Source code
│   ├── core/                         # Core utilities
│   │   ├── Config/                   # Configuration system
│   │   ├── Random/                   # Random number generators
│   │   ├── Logger/                   # Logging system
│   │   └── ...                       # Other core components
│   ├── brain/                        # Neural components
│   ├── agent/                        # Agent integration
│   ├── world/                        # Environment interface
│   ├── experiments/                  # Experiment framework
│   └── visualization/                # Visualization interface
├── tests/                            # Unit tests
├── configs/                         # Configuration files
├── python/                          # Python bindings (if any)
└── build/                           # Build directory (exclude from git)
```

## Contributing

### Reporting Issues

1. **File bugs** with detailed reproduction steps
2. **Provide configuration files** for reproducing issues
3. **Include logs** from failed runs
4. **Check existing issues** for duplicates

### Submitting Changes

1. **Fork the repository**
2. **Create feature branch** with descriptive name
3. **Follow coding standards** and guidelines
4. **Add tests** for new functionality
5. **Update documentation** as needed
6. **Submit pull request** with clear description

### Code Review Process

1. **Initial review** by project maintainers
2. **Code quality check** (style, documentation, tests)
3. **Integration testing** (existing tests and demos)
4. **Performance review** (if applicable)
5. **Documentation review** (API and user guides)

## License

MIT License - See LICENSE file for details.

## Authors

Research project - See individual source files for author information.

---

*This documentation is a work in progress. Contributions to improve and extend this documentation are welcome.*

Generated: $(date)
Version: 1.0 (Phase 6)