# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

This document provides a comprehensive guide to using NLM (熙然) Phase 6, the final integration phase of this experimental brain project. NLM implements a biologically-inspired artificial brain that learns through experience and develops over time.

### Why Use NLM?

NLM offers a unique approach to artificial intelligence:

- **Neuromorphic Computing**: Implements real spiking neural dynamics using Leaky Integrate-and-Fire (LIF) neurons
- **Developmental Learning**: Systems develop and adapt through developmental stages
- **Integrated Memory**: Multiple memory systems (working, episodic, semantic, procedural) connected to neural processing
- **Neuromodulation**: Biological neuromodulators (dopamine, curiosity, novelty) guide learning
- **Predictive Processing**: Prediction systems integrated throughout the brain loop
- **Replay and Consolidation**: Memory replay during rest reinforces learning
- **Checkpoint Persistence**: State saving and loading for long-term learning

### Installation and Building

#### Prerequisites

- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19.2+)
- CMake 3.16 or higher
- Make or Ninja build system

#### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with 4 parallel jobs
make -j4

# Build all executables
make nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
```

### Running NLM Phase 6

#### Phase 6 Demo (Recommended)

The Phase 6 integration demo provides comprehensive testing of all brain systems:

```bash
# Run Phase 6 integration demo with default settings
./nlm_phase6_demo

# Run with custom configuration
./nlm_phase6_demo --neurons 2000 --steps 5000 --checkpoint ./my_checkpoint.bin

# Show help information
./nlm_phase6_demo --help
```

**Command Line Options:**

- `--neurons N`: Number of neurons (default: 500)
- `--steps S`: Number of simulation steps (default: 2000)
- `--regions R`: Number of brain regions (default: 1)
- `--checkpoint PATH`: Checkpoint file path (default: ./nlm_phase6_checkpoint.bin)
- `--log FILE`: Log file path (default: nlm_phase6_demo.log)
- `--no-checkpoint`: Disable checkpointing
- `--no-replay`: Disable replay system
- `--no-development`: Disable development system
- `--help`: Show help message

#### Phase 2 Legacy Demo (Deprecated)

The original Phase 2 demo is maintained for backward compatibility but is deprecated:

```bash
# Run Phase 2 tests (deprecated)
./nlm

# This will display a warning and recommend Phase 6 instead
```

### Understanding NLM Architecture

#### Core Brain Systems

1. **Memory Systems**
   - **Working Memory**: Transient information storage
   - **Episodic Memory**: Experience recording and replay
   - **Semantic Memory**: Knowledge and facts
   - **Procedural Memory**: Skills and habits
   - **Associative Memory**: Pattern associations

2. **Neuromodulation Systems**
   - **Dopamine**: Reward prediction and reinforcement
   - **Curiosity**: Exploration motivation
   - **Novelty**: Change detection
   - **Prediction Error**: Surprise and learning signals

3. **Prediction Systems**
   - **Sensory Prediction**: Predictive coding of sensory input
   - **Action Prediction**: Forward models for planning
   - **Internal Models**: Internal representations of the world

4. **Cognition Systems**
   - **Neural Planner**: Action sequence evaluation
   - **Concept Formation**: Pattern discovery and abstraction
   - **Attentional Selection**: Competitive selection of information
   - **Theory of Mind**: Simple social reasoning

5. **Development System**
   - **Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
   - **Structural Plasticity**: Synaptogenesis and pruning
   - **Experience-Dependent Development**: Environmental influence on development

#### Brain Loop Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Key Features and Capabilities

#### 1. Real Spiking Neural Dynamics

NLM implements biologically realistic neuron dynamics:

```cpp
// LIF neuron with spike-frequency adaptation
class Neuron {
public:
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    void injectCurrent(MembranePotential current);
    void receiveExcitatoryInput(MembranePotential weight);
    void recordSpike(Timestamp time);
};
```

**Key Features:**
- Leaky Integrate-and-Fire dynamics
- Refractory periods
- Spike-frequency adaptation
- Synaptic plasticity (STDP, Hebbian)
- Structural plasticity (synaptogenesis/pruning)

#### 2. Integrated Memory Systems

Memory systems work together with neural processing:

```cpp
// Working memory interface
class NeuralWorkingMemory {
public:
    void storeActiveTrace(const std::vector<float>& trace);
    std::vector<float> retrieveActiveTrace(size_t id);
    size_t getActiveTraces();
};

// Episodic memory with replay
class NeuralEpisodicMemory {
public:
    void recordExperience(const Experience& exp);
    void replayEpisodes(size_t count);
    std::vector<Experience> getRecentEpisodes();
};
```

**Key Features:**
- Working memory traces for active information
- Episodic memory episodes for experience storage
- Replay systems for memory consolidation
- Integration with neural dynamics

#### 3. Neuromodulation and Learning

Biological neuromodulators guide learning and behavior:

```cpp
// Neuromodulation system
class Neuromodulator {
public:
    void applyReward(float reward, float predictionError);
    void updateCuriosity(float novelty);
    void updateNovelty(float change);
    float getDopamineLevel() const;
};
```

**Key Features:**
- Reward prediction error signaling
- Curiosity-driven exploration
- Novelty detection
- Adaptive motivation

#### 4. Developmental Processes

Brains develop and adapt over time:

```cpp
// Development system
class DevelopmentSystem {
public:
    void update(double timestep);
    DevelopmentalStage getCurrentStage() const;
    void setPlasticityRate(float rate);
    bool isCriticalPeriod() const;
};
```

**Key Features:**
- Developmental stages (Initial → Critical → Maturation → Adult → Aging)
- Age-dependent plasticity
- Structural changes
- Experience-dependent development

#### 5. Checkpoint and Persistence

Long-term learning and state preservation:

```cpp
// Checkpoint system
class CheckpointSystem {
public:
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    bool isCheckpointAvailable() const;
};
```

**Key Features:**
- Save brain state to file
- Load from checkpoint
- Version compatibility
- Efficient serialization

### Configuration Management

NLM provides comprehensive configuration options for all systems:

#### Basic Configuration

```cpp
// Phase 6 configuration
Phase6Config config;
config.maxSteps = 10000;
config.neuronCount = 1000;
config.regionCount = 1;
config.connectionProbability = 0.1f;
config.enableCheckpointing = true;
config.enableReplay = true;
config.enableDevelopment = true;
```

#### Advanced Configuration

```cpp
// Advanced performance settings
Phase6Config config;
config.performance.enableVectorization = true;
config.performance.enableParallelProcessing = true;
config.performance.maxThreads = 8;
config.performance.enableSIMD = true;

// Memory configuration
config.memory.enableWorkingMemory = true;
config.memory.enableEpisodicMemory = true;
config.memory.replayFrequency = 3600.0;  // 1 hour in seconds

// Neuromodulation configuration
config.neuromodulation.enableDopamine = true;
config.neuromodulation.enableCuriosity = true;
config.neuromodulation.dopamineBaseline = 0.5f;
```

### Performance Tips

#### Optimizing Performance

1. **Use SIMD Instructions**
   - NLM automatically vectorizes operations
   - Configure with `--enable-vectorization`
   - Performance gains: 2-4x for vectorized operations

2. **Enable Parallel Processing**
   - Configure with `--enable-parallel-processing`
   - Set `--max-threads` for optimal performance
   - Gains: 4-8x for large networks

3. **Memory Pool Optimization**
   - Enable memory pools for better memory management
   - Reduces allocation overhead
   - Especially beneficial for long simulations

4. **Cache-Friendly Data Layout**
   - NLM uses cache-aligned data structures
   - Minimize pointer chasing
   - Use batch operations when possible

#### Scaling Guidelines

- **Small Networks** (< 1,000 neurons): Single-threaded, focus on accuracy
- **Medium Networks** (1,000-10,000 neurons): Vectorized operations
- **Large Networks** (> 10,000 neurons): Parallel processing recommended

### Example Usage

#### Example 1: Basic Learning Task

```cpp
#include "nlm/experiments/Phase6Demo.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Learning Example ===" << std::endl;
    
    // Configure Phase 6 experiment
    nlm::Phase6DemoConfig config;
    config.maxSteps = 5000;
    config.neuronCount = 1000;
    config.enableDevelopment = true;
    config.enableCuriosity = true;
    
    // Create and run demo
    nlm::Phase6Demo demo;
    bool success = demo.run(config);
    
    if (success) {
        std::cout << "✓ Learning experiment completed successfully!" << std::endl;
        demo.printResults();
    } else {
        std::cerr << "✗ Learning experiment failed!" << std::endl;
        return 1;
    }
    
    return 0;
}
```

#### Example 2: Checkpoint-Based Learning

```cpp
#include "nlm/experiments/Phase6Demo.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Checkpoint-Based Learning ===" << std::endl;
    
    // First run: train and save checkpoint
    nlm::Phase6DemoConfig config1;
    config1.maxSteps = 2000;
    config1.neuronCount = 500;
    config1.checkpointPath = "learning_session_1.bin";
    config1.enableCheckpointing = true;
    
    nlm::Phase6Demo demo1;
    demo1.run(config1);
    
    // Save results
    demo1.saveResults("results_session_1.txt");
    
    // Second run: resume from checkpoint and continue learning
    nlm::Phase6DemoConfig config2;
    config2.maxSteps = 3000;
    config2.neuronCount = 500;
    config2.checkpointPath = "learning_session_1.bin";
    config2.enableCheckpointing = true;
    
    nlm::Phase6Demo demo2;
    bool success = demo2.run(config2);
    
    if (success) {
        std::cout << "✓ Checkpoint-based learning completed!" << std::endl;
        demo2.saveResults("results_session_2.txt");
    }
    
    return 0;
}
```

#### Example 3: Integration Testing

```cpp
#include "nlm/experiments/Phase6Demo.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM System Integration Test ===" << std::endl;
    
    nlm::Phase6Demo demo;
    
    // Verify system integration
    bool integrationOK = demo.verifyIntegration();
    if (!integrationOK) {
        std::cerr << "ERROR: System integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ System integration verified!" << std::endl;
    
    // Test individual systems
    bool memoryOK = demo.testMemorySystem();
    bool neuromodulationOK = demo.testNeuromodulationSystem();
    bool predictionOK = demo.testPredictionSystem();
    bool developmentOK = demo.testDevelopmentSystem();
    
    std::cout << "Integration Test Results:" << std::endl;
    std::cout << "  Memory System: " << (memoryOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Neuromodulation: " << (neuromodulationOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Prediction System: " << (predictionOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Development System: " << (developmentOK ? "PASS" : "FAIL") << std::endl;
    
    // Run full integration experiment
    nlm::Phase6DemoConfig config;
    config.maxSteps = 1000;
    config.neuronCount = 200;
    
    bool experimentOK = demo.run(config);
    
    if (experimentOK) {
        std::cout << "✓ Integration experiment completed successfully!" << std::endl;
        demo.printResults();
    }
    
    return 0;
}
```

### Troubleshooting

#### Common Issues and Solutions

1. **Simulation Too Slow**
   - Enable vectorization and parallel processing
   - Reduce neuron count or simulation steps
   - Use checkpointing for longer simulations

2. **Memory Usage Too High**
   - Disable optional memory systems
   - Reduce memory pool size
   - Monitor memory usage with enhanced logging

3. **Checkpointing Issues**
   - Ensure write permissions to checkpoint path
   - Check available disk space
   - Verify configuration settings

4. **Learning Not Occurring**
   - Enable neuromodulation and plasticity
   - Check plasticity parameter settings
   - Verify reward signals are being computed

#### Enhanced Logging

Enable detailed logging for debugging:

```bash
# Enable debug logging
./nlm_phase6_demo --log debug.log

# Enable enhanced logging features
./nlm_phase6_demo --log performance.log --enable-profiling
```

### Advanced Features

#### 1. Agent-Based Interaction

NLM includes an agent system for embodied interaction:

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

// Create agent with brain
AgentBrain agent(brain);
agent.initialize(world);
agent.enableCuriosity(true);
agent.enableDevelopment(true);
```

#### 2. Performance Monitoring

Monitor system performance in real-time:

```cpp
#include "nlm/core/EnhancedLogger.hpp"

// Enable performance profiling
nlm::PerformanceProfiler::getInstance().start();

// Log performance metrics
nlm::PerformanceProfiler::getInstance().recordSample();

// Get performance report
std::cout << nlm::PerformanceProfiler::getInstance().getPerformanceReport();
```

#### 3. Custom Configuration

Create custom configurations for specific experiments:

```cpp
// Custom configuration for robotics experiments
nlm::Phase6Config config = nlm::Phase6Config::getPreset("robotics");
config.performance.enableVectorization = true;
config.performance.maxThreads = 8;
config.memory.enableWorkingMemory = true;
config.development.enableExperienceDependentDevelopment = true;
```

### Available Presets

NLM provides several pre-configured presets for common use cases:

- **small_test**: Minimal configuration for quick testing
- **medium_test**: Balanced configuration for typical experiments
- **large_test**: High-performance configuration for extensive research
- **quick_validation**: Fast validation with minimal setup
- **production**: Optimized for long-running experiments
- **debug**: Maximum logging and validation for debugging
- **benchmark**: Optimized for performance benchmarking
- **research**: Research-oriented configuration with experimental features

### References

#### Key Papers and Resources

1. **Marr, D. (1982). Vision**. W.H. Freeman.
2. **Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience**. MIT Press.
3. **Pyka, L., & Noble, J. (2022). Neuromorphic Computing**. Springer.
4. **Diekelmann, S., & Born, J. (2010). Systems consolidation of memory**. Neuron.
5. **Barto, A. G., & Sutton, R. S. (1998). Introduction to reinforcement learning**. Springer.

#### Software Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Examples**: https://github.com/nlm-project/nlm-examples
- **Issues and Discussions**: https://github.com/nlm-project/nlm/discussions

### Acknowledgments

This project builds on decades of research in computational neuroscience, neuromorphic engineering, and artificial intelligence. Special thanks to the open-source community for contributing to the foundation technologies used in NLM.

### License

MIT License. See LICENSE file for details.

---

*Last updated: October 2026*
*NLM Phase 6 Final Integration*

For additional documentation, examples, and support, visit the NLM project website and documentation.
