# Phase 6 Artificial Brain - Quick Start Guide

## Getting Started

### What is Phase 6?

Phase 6 represents the **final integration** of the NLM (熙然) artificial brain system. After 6 years of development (Phases 1-5), Phase 6 delivers a complete, working artificial brain with all major systems integrated:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine (reward), curiosity, novelty detection
- **Prediction**: Future state prediction and prediction errors
- **Cognition**: Planning, concept formation, attention
- **Development**: Age-dependent brain maturation
- **Persistence**: Checkpoint save/load capabilities
- **Replay**: Memory consolidation and replay

This guide covers everything you need to get started with Phase 6.

## Prerequisites

### System Requirements

- **CMake 3.10 or higher**
- **C++17 compatible compiler** (GCC 9+, Clang 10+, MSVC 19.2+)
- **4GB RAM** (recommended for full 10k neuron simulation)
- **500MB disk space** for build and checkpoint storage

### Platform Support

Phase 6 runs on:
- **Linux** (Ubuntu, Fedora, CentOS)
- **macOS** (Intel and ARM)
- **Windows** (Visual Studio 2019+ with CMake)

## Installation

### Option 1: Build from Source

```bash
# Clone the repository
cd /path/to/your/workspace
# (Assuming the repository is already cloned)

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all components (4-8 cores)
make -j4

# Build with optimizations (faster)
make -j$(nproc)
```

### Option 2: Build with Tests

```bash
# Build with test suite
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON
make -j4

# Run tests
ctest --output-on-failure
```

### Option 3: Quick Demo Build

For a faster build focused on the demo:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_MINIMAL_BUILD=ON
make nlm_phase6_demo
```

## First Run

### Running the Phase 6 Integration Demo

```bash
# Navigate to build directory
cd build

# Run the comprehensive integration demo
./nlm_phase6_demo
```

### What You'll See

The demo outputs:

```
=== NLM Phase 6 Integration Demo ===
Testing the integrated artificial brain...

--- Integration Verification ---
[PASS] Working memory is integrated
[PASS] Episodic memory is integrated
[PASS] Dopamine system is integrated
[PASS] Curiosity system is integrated
[PASS] Prediction system is integrated
[PASS] Planner is integrated
[PASS] Concept formation is integrated
[PASS] Attention is integrated
[PASS] Development system is integrated

Integration verification passed!

--- Memory Integration Test ---
[PASS] Working memory has active traces: X
[PASS] Episodic memory has episodes: Y

--- Neuromodulation Integration Test ---
[PASS] Neuromodulation systems are functional

--- Checkpoint Test ---
Brain1 avg firing: 12.34
Brain1 total spikes: 567
Checkpoint saved successfully
Checkpoint loaded successfully
[PASS] Checkpoint save/load completed

--- Replay Test ---
[PASS] Episodes available for replay: Z
[PASS] Replay system can retrieve episodes

--- Full Integration Experiment ---
Step 0 | Reward: 0.0 | Firing: 8.9 | WorkingMem: 0
...

=== FINAL RESULTS ===
Total reward: 0.45
Avg firing rate: 12.3
Episodes stored: 245
Dopamine level: 0.78

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING

Wall clock time: 45.2s
```

## Exploring Brain Systems

### Using the Interactive Brain Browser

```cpp
// This code shows how to explore the integrated brain systems

#include "Phase6IntegratedExperiment.hpp"

int main() {
    Phase6IntegratedExperiment experiment;
    
    // Create experiment with custom configuration
    Phase6Config config;
    config.neuronCount = 1000;
    config.maxSteps = 5000;
    config.enableCheckpointing = true;
    
    // Run integration experiment
    auto result = experiment.run(config);
    
    // Explore the results
    std::cout << "=== Brain System Analysis ===" << std::endl;
    std::cout << "Total reward per step: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    
    // Check system integration
    std::cout << "\n=== System Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    
    return 0;
}
```

### Command Line Options

```bash
./nlm_phase6_demo [options]

Options:
  --help                    Show help message
  --config <file>          Load configuration from JSON file
  --steps <num>            Number of simulation steps (default: 10000)
  --neurons <num>          Number of neurons (default: 1000)
  --regions <num>          Number of neural regions (default: 1)
  --checkpoint <path>      Checkpoint file path (default: ./checkpoint.bin)
  --no-checkpointing       Disable checkpointing
  --no-replay              Disable memory replay
  --no-development         Disable brain development
  --verbose                Enable verbose output
```

## Saving and Loading Checkpoints

### Saving Checkpoints

```cpp
#include "Performance/CheckpointSystem.hpp"

// Save brain state at any point
brain->save("./my_checkpoint.bin");

// Save with custom parameters
CheckpointWriter writer;
writer.create("./custom_checkpoint.bin", CompressionLevel::Balanced);
// ... write data ...
writer.finalize();
```

### Loading Checkpoints

```cpp
// Load previous brain state
brain->load("./my_checkpoint.bin");

// Load with version checking
CheckpointReader reader;
if (reader.open("./my_checkpoint.bin")) {
    if (reader.validate()) {
        // Checkpoint is valid and compatible
        // ... load data ...
    } else {
        std::cerr << "Invalid checkpoint!" << std::endl;
    }
}
```

## Example Configuration Files

### Configuration Files Directory

```bash
/configs/
├── minimal.json          # Small brain for testing
├── standard.json         # Balanced configuration
├── research.json         # Large brain for research
└── performance.json      # Optimized for speed
```

### minimal.json Example

```json
{
    "neuronCount": 500,
    "maxSteps": 2000,
    "regionCount": 1,
    "connectionProbability": 0.05,
    "enableCheckpointing": false,
    "enableReplay": false,
    "enableDevelopment": false,
    "checkpointPath": "./minimal_checkpoint.bin"
}
```

## Getting Help

### Documentation Resources

1. **User Guide**: `docs/USER_GUIDE.md` - Installation and basic usage
2. **Developer Guide**: `docs/DEVELOPER_GUIDE.md` - Code structure and customization
3. **Advanced Usage**: `docs/ADVANCED_USAGE.md` - Custom configurations and examples
4. **Quick Start**: This guide - Getting started quickly

### Support

If you need help:

1. **Check the FAQ**: Look for common issues in the documentation
2. **Run tests**: `ctest` will show if basic functionality works
3. **Examine logs**: Look at `build/debug.log` for detailed error information
4. **Report issues**: Submit issues to the NLM repository

### Troubleshooting

#### "Program not found" error

```bash
# Make sure you're in the build directory
./nlm_phase6_demo
```

#### "Cannot find CMakeLists.txt"

```bash
# Navigate to project root first
cd /path/to/nlm
mkdir build
cd build
cmake ..
```

#### Low performance or high memory usage

```bash
# Try a smaller configuration
cmake .. -DNLM_MINIMAL_BUILD=ON
make -j4
```

## Next Steps

### After Running the Demo

1. **Explore the code**: Look at `src/experiments/Phase6IntegratedExperiment.cpp` to understand how systems integrate
2. **Try custom configurations**: Modify the Phase6Config structure in your own experiments
3. **Study memory systems**: Examine `src/memory/` for working memory and episodic memory implementations
4. **Understand neuromodulation**: Look at `src/neuromodulation/` for dopamine, curiosity, and novelty systems
5. **Learn prediction**: Study `src/prediction/` for the prediction system

### For Advanced Users

1. **Customize brain architecture**: Modify `src/brain/` to add new neural populations
2. **Create custom plasticity rules**: Extend `src/plasticity/` with new learning mechanisms
3. **Design experiments**: Build on the Phase6IntegratedExperiment framework
4. **Performance optimization**: Use SIMD and parallel processing in `src/performance/`

### Project Development

1. **Join the project**: Contribute to NLM development
2. **Run benchmarks**: Use `src/benchmark.cpp` to compare configurations
3. **Study research papers**: Refer to `docs/SCIENCE.md` for the scientific background
4. **Documentation**: Update this guide with your discoveries

## About This Project

### NLM Project Status

- **Phase 1 (Complete)**: Project skeleton, core types, neural interfaces
- **Phase 2 (Complete)**: Real LIF dynamics, event-driven spikes, plasticity
- **Phase 3 (Complete)**: World interaction, sensory/motor, reward, development
- **Phase 4 (Complete)**: Prediction, memory, cognition, attention
- **Phase 5 (Complete)**: Performance optimizations, checkpoint system
- **Phase 6 (Complete)**: Final integration, all systems working together

### Scientific Background

NLM investigates whether artificial brains can develop complex behaviors through experience. The project makes no claims about consciousness, intelligence, or human-like cognition, but explores whether brain-inspired architectures can achieve coherent, integrated behavior.

Phase 6 demonstrates that:
- Memory systems can integrate with neural dynamics
- Neuromodulation can coordinate plasticity across systems
- Development can shape learning trajectories
- Prediction can organize behavior
- Complete brain loops can function coherently

### License

MIT License. See `LICENSE` for details.

---

**Ready to explore the artificial brain?** Run `./nlm_phase6_demo` in the build directory to see the integrated systems working together!
