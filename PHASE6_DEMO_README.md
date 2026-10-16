# NLM Phase 6 Demo - Quick Start Guide

## Welcome to the NLM Phase 6 Integration Demo!

This is an improved version of the Phase 6 integration test that demonstrates the complete artificial brain system with all advanced features working together.

## What You Will See

The Phase 6 demo showcases:

- ✅ **Working Memory** - Active memory traces during simulation
- ✅ **Episodic Memory** - Experience storage and replay
- ✅ **Neuromodulation** - Dopamine, curiosity, and reward systems
- ✅ **Prediction System** - Forward-looking neural predictions
- ✅ **Development** - Brain maturation over time
- ✅ **Checkpointing** - Save/load simulation state
- ✅ **Replay** - Memory consolidation through replay
- ✅ **Advanced Features** - Curiosity-driven exploration, reward learning, structural plasticity

## Quick Start

### Option 1: Using the Build Script

1. **Build the project**
   ```bash
   chmod +x run_phase6_demo.sh
   ./run_phase6_demo.sh
   ```

2. **Quick Test** (fast, minimal output)
   ```bash
   ./run_phase6_demo.sh --quick-run
   ```

### Option 2: Using CMake Directly

1. **Create and build**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the demo**
   ```bash
   ./nlm_phase6_demo
   ```

## Command-Line Options

Run the demo with:
```bash
./build/nlm_phase6_demo --help
```

**Available options:**

- `-h, --help` - Show this help message
- `-n, --neurons <count>` - Number of neurons (default: 500)
- `-s, --steps <count>` - Number of simulation steps (default: 2000)
- `-r, --regions <count>` - Number of brain regions (default: 1)
- `-c, --connection <rate>` - Connection probability (default: 0.1)
- `-p, --checkpoint <path>` - Checkpoint file path (default: ./checkpoint.bin)
- `--no-checkpointing` - Disable checkpoint save/load
- `--no-replay` - Disable memory replay
- `--no-development` - Disable development
- `--enable-curiosity` - Enable curiosity-driven exploration
- `--enable-reward` - Enable reward-based learning
- `--enable-structural` - Enable structural plasticity

## Example Commands

### 1. Basic Configuration
```bash
./build/nlm_phase6_demo
```

### 2. Larger Simulation
```bash
./build/nlm_phase6_demo -n 1000 -s 5000
```

### 3. Advanced Features
```bash
./build/nlm_phase6_demo -n 800 -s 3000 --enable-curiosity --enable-reward --enable-structural
```

### 4. Fast Development Run
```bash
./build/nlm_phase6_demo -n 200 -s 1000 --no-checkpointing --no-replay --no-development
```

## What You Will See in the Output

The demo provides detailed output showing:

1. **Integration Verification** - Confirms all brain systems are connected
2. **Memory Integration Tests** - Shows working and episodic memory
3. **Neuromodulation Tests** - Demonstrates curiosity and reward systems
4. **Checkpoint Tests** - Verifies save/load functionality
5. **Replay Tests** - Shows memory consolidation
6. **Full Integration Experiment** - Complete brain simulation with all features

## Understanding the Output

The demo output includes:

- **Metrics**: Reward, firing rates, memory storage
- **Integration Status**: Shows which systems are connected
- **System Performance**: Dopamine levels, curiosity, novelty
- **Timing Information**: Wall clock time for the simulation

## Technical Details

The Phase 6 demo runs a complete neural network simulation with:

- **500-1000 neurons** by default
- **2000 steps** by default
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity** for learning
- **Structural plasticity** for growing new connections
- **Developmental stages** affecting plasticity rates
- **Checkpoint save/load** for state preservation
- **Memory replay** for consolidation

## Troubleshooting

### "My brain isn't doing anything"
- Did you run `make` to build the project?
- Try increasing the number of steps: `-s 5000`
- Ensure the simulation has time to develop: `-n 1000 -s 5000`

### "The demo is taking too long"
- Use `--quick-run` mode for faster testing
- Disable checkpointing: `--no-checkpointing`
- Use smaller neuron count: `-n 100`
- Reduce steps: `-s 500`

### "Help shows unknown options"
- Make sure you're using the correct executable path: `./build/nlm_phase6_demo`
- The help is built into the demo itself

### "Checkpoint fails"
- Ensure write permissions to current directory
- Check available disk space
- Use a different path: `--checkpoint /tmp/checkpoint.bin`

## Advanced Features

### Curiosity-Driven Exploration
When enabled, the brain automatically explores novel situations and environments, leading to more diverse learning experiences.

### Reward-Based Learning
The brain learns from rewards and punishments, adjusting its behavior to maximize long-term rewards.

### Structural Plasticity
The brain can grow new connections and prune unused ones, leading to more efficient neural architectures.

### Development Stages
The brain matures over time, with different phases affecting learning speed and plasticity.

## Performance Tips

1. **For quick testing**: Use `--quick-run` or smaller settings
2. **For detailed analysis**: Use larger simulations with checkpointing
3. **For development**: Enable all advanced features
4. **For production**: Use optimal settings and checkpointing

## Further Reading

- **IMPROVEMENTS_SUMMARY.md** - Detailed documentation of all improvements
- **README.md** - Project overview and build instructions
- **easy_usage.md** - Beginner-friendly usage guide

## Getting Help

If you encounter issues:

1. Check the error messages in the output
2. Try running with `--help` to see all options
3. Review the troubleshooting section above
4. Check the logs for detailed error information

## Changes from Previous Version

This improved version includes:

1. **Fixed Phase inconsistencies** - Phase 6 demo is now the main integration demo
2. **Enhanced CLI** - Comprehensive command-line interface with validation
3. **Better error handling** - Clear error messages and graceful failure
4. **Advanced features** - Curiosity, reward, and structural plasticity
5. **Configuration validation** - Parameter checking and helpful error messages
6. **Improved documentation** - Comprehensive help and examples
7. **Code quality** - Fixed unsafe array access and missing includes

## License

This project is part of the NLM (Neural Learning Machine) research project. See LICENSE.md for details.
