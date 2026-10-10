# NLM Build Guide - Phase 6 Integration

## Overview
This guide covers building and running the NLM (Neural Learning Machine) Phase 6 integration. Phase 6 represents the final integration of all brain systems into a coherent artificial brain.

## Quick Start

### Building the Project

1. **Create build directory and configure CMake:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

2. **Build with parallel compilation:**
   ```bash
   make -j$(nproc)
   ```

3. **Run Phase 6 integration demo:**
   ```bash
   ./nlm_phase6_demo
   ```

4. **Or run Phase 2 demo (legacy):**
   ```bash
   ./nlm
   ```

### Project Structure

```
NLM/
├── build/
│   ├── nlm                     # Main Phase 2 executable
│   ├── nlm_phase3_demo         # Phase 3 experiments
│   ├── nlm_phase4_demo         # Phase 4 experiments
│   └── nlm_phase6_demo         # Phase 6 integration demo (NEW)
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── memory/        # Memory systems
│   ├── agent/          # Agent brain interface
│   ├── world/          # World system
│   ├── experiments/    # Experiment framework
│   └── ...
├── configs/            # Configuration files
│   └── default.cfg     # Default configuration
└── docs/               # Documentation
```

## Configuration Options

### Default Configuration (`configs/default.cfg`)

The default configuration is optimized for Phase 2 demos. For Phase 6 integration, consider adjusting:

```bash
cfg->set("neuron_count", 1000);      # Increase for Phase 6
cfg->set("region_count", 1);         # Single region for simplicity
cfg->set("connection_probability", 0.1f);
cfg->set("stdp_ltp_weight", 0.02f);
cfg->set("stdp_ltd_weight", 0.015f);
```

### Custom Configuration

Create a custom config file:

```bash
./nlm --config=my_config.cfg
```

Or programmatically set values in your code:

```cpp
cfg->set("neuron_count", 5000);
cfg->set("enable_checkpointing", true);
cfg->set("enable_replay", true);
```

## Running Different Demos

### Phase 2: Basic Neural Computation

```bash
# Run Phase 2 demo (legacy)
./nlm

# Features: LIF neurons, basic plasticity, STDP learning
```

### Phase 3: World Interaction

```bash
# Run Phase 3 demo
./nlm_phase3_demo

# Features: Sensorimotor loop, reward prediction
```

### Phase 4: Cognitive Integration

```bash
# Run Phase 4 demo
./nlm_phase4_demo

# Features: Working memory, concept formation, planning
```

### Phase 6: Complete Integration

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# Features:
# - All memory systems integrated
# - Full neuromodulation (dopamine, curiosity, novelty)
# - Prediction system integration
# - Developmental stages
# - Checkpoint save/load
# - Memory replay and consolidation
```

## Configuration for Phase 6

For optimal Phase 6 integration, adjust these settings in `configs/default.cfg`:

```ini
# Simulation settings
max_simulation_steps = 50000
simulation_time_limit = 1800.0

# Memory settings
working_memory_capacity = 1000
 episodic_memory_capacity = 1000

# Development settings
enable_development = true
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation
enable_curiosity = true
enable_novelty_detection = true

# Replay settings
replay_interval = 1000.0
consolidation_threshold = 0.8
```

## Build Options

### Debug vs Release

```bash
# Debug build (with assertions and logging)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release

# RelWithDebInfo (optimized with debug info)
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Additional CMake Options

```bash
# Enable visualization
cmake .. -DENABLE_VISUALIZATION=ON

# Enable benchmark mode
cmake .. -DENABLE_BENCHMARKS=ON

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/nlm
```

## Running Scripts

### build.sh (Bash)

A convenience script for building the project:

```bash
#!/bin/bash
set -e

BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "Build completed successfully!"
echo "Executables available:"
echo "  - nlm (Phase 2 demo)"
echo "  - nlm_phase3_demo (Phase 3 experiments)"
echo "  - nlm_phase4_demo (Phase 4 experiments)"
echo "  - nlm_phase6_demo (Phase 6 integration demo)"
```

### run_phase6.sh (Bash)

Run Phase 6 integration with custom settings:

```bash
#!/bin/bash

set -e

BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please run ./build.sh first."
    exit 1
fi

cd "$BUILD_DIR"

# Run with custom settings
./nlm_phase6_demo \
    --neuron_count=2000 \
    --region_count=2 \
    --enable_checkpointing=true \
    --enable_replay=true \
    --enable_development=true
```

## Testing

### Unit Tests

Run tests with CTest:

```bash
cd build
ctest
ctest --output-on-failure
```

### Integration Tests

Phase 6 integration includes comprehensive testing:

- Integration verification: All systems connected
- Memory integration: Working + episodic memory functional
- Neuromodulation integration: Dopamine, curiosity active
- Checkpoint testing: Save/load functionality
- Replay testing: Memory consolidation functional

## Performance Optimization

### Memory Configuration

For large-scale Phase 6 experiments:

```cpp
// In Phase6Config or brain configuration
cfg->set("max_working_memory_traces", 10000);
cfg->set("max_episodic_memory_episodes", 5000);
cfg->set("enable_memory_pools", true);
```

### Parallel Processing

The build system supports parallel compilation:

```bash
# Use all available cores
make -j$(nproc)

# Specify number of cores
make -j8
```

## Troubleshooting

### Build Errors

**Common issue:** Missing CMake version
```bash
# CMake 3.16 or higher required
# Update your package manager or install from source
```

**Common issue:** C++20 features not supported
```bash
# Ensure your compiler supports C++20
# GCC 9+, Clang 7+, MSVC 2019+
```

**Common issue:** Missing dependencies
```bash
# Required dependencies:
# - CMake 3.16+
# - GCC/Clang with C++20 support
# - Make or Ninja build system
```

### Runtime Errors

**Memory allocation issues:**
```bash
# Increase system memory or reduce neuron count
./nlm_phase6_demo --neuron_count=1000
```

**Configuration errors:**
```bash
# Check configs/default.cfg for syntax errors
# Use --config flag to specify custom config
./nlm_phase6_demo --config=custom.cfg
```

**Logging issues:**
```bash
# Enable file logging
# Set log_to_file = true in config
cat nlm.log | grep "ERROR"
```

## Advanced Usage

### Checkpointing

Phase 6 supports checkpoint save/load:

```cpp
// Save checkpoint
brain->save("/tmp/nlm_checkpoint.bin");

// Load checkpoint
brain->load("/tmp/nlm_checkpoint.bin");
```

### Replay and Consolidation

Memory replay during simulation:

```cpp
// Replay important experiences
brain->getEpisodicMemory()->replayEpisodes(10);

// Consolidate memories
brain->getEpisodicMemory()->consolidateImportantEpisodes();
```

### Custom Environments

Extend the world system for custom experiments:

```cpp
class CustomWorld : public SimpleWorld {
public:
    void applyCustomAction(MotorCommand cmd) override {
        // Custom action logic
    }
    
    float computeCustomReward() override {
        // Custom reward calculation
        return baseReward + bonusFactor;
    }
};
```

## Development Workflow

1. **Create feature branch:**
   ```bash
   git checkout -b feature/phase6-enhancement
   ```

2. **Make changes:**
   ```bash
   # Edit source files
   # Test with ./build/nlm_phase6_demo
   ```

3. **Commit changes:**
   ```bash
   git add .
   git commit -m "Add Phase 6 integration feature"
   ```

4. **Run tests:**
   ```bash
   cd build && ctest --output-on-failure
   ```

5. **Build and verify:**
   ```bash
   cd build && make -j$(nproc)
   ./nlm_phase6_demo
   ```

## Further Reading

- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md) - Audit report of Phase 6 integration
- [Architecture Documentation](docs/ARCHITECTURE.md) - System architecture overview
- [Science Documentation](docs/SCIENCE.md) - Scientific background and principles
- [Phase 6 Roadmap](docs/ROADMAP.md) - Future development roadmap

## License

MIT License - See LICENSE file for details.