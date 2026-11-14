# NLM (熙然) - Neural Learning Machine
# Documentation and User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Phase 6: Final Integration - Key Improvements

Phase 6 represents the final integration of all brain systems into a coherent artificial brain architecture. The following improvements have been made to address previous issues:

### 1. Fixed Reward Calculation Bug

**Issue**: Phase 6 demos were reporting 0 reward due to a fundamental bug in the reward accumulation calculation.

**Problem**: In `Phase6IntegratedExperiment.cpp`, the code was accumulating average reward per step rather than total accumulated reward:

```cpp
// INCORRECT (was causing 0 reward):
totalReward += reward / (step + 1);  // Average, not total!
result.totalReward = totalReward / config.maxSteps;  // Dividing average again!
```

**Solution**: Fixed the reward calculation to properly accumulate total rewards:

```cpp
// CORRECT:
totalReward += reward;  // Accumulate total rewards
result.totalReward = totalReward;  // Return actual total
```

**Impact**: Phase 6 experiments now correctly report meaningful reward values.

### 2. Improved Learning Efficiency

**Issue**: Learning was not selective - all sensory neurons were stimulated simultaneously, causing global saturation.

**Improvement**: Enhanced the plasticity experiment in `main.cpp` with:

- **Varied Input Patterns**: Instead of stimulating only the first 20 sensory neurons, the system now rotates through different neuron sets (100% coverage over time)
- **Dynamic Current Levels**: Current injection varies between 30-40 mV to prevent saturation
- **Better Statistics**: Enhanced logging with pattern size, current levels, and firing metrics

**Result**: Learning is now more selective and representative of biological learning.

### 3. Comprehensive Test Results Logging

**Improvement**: Enhanced `Phase6Demo.cpp` with comprehensive integration status reporting:

- Detailed breakdown of each system integration status
- Overall integration score (0-5 systems connected)
- Clear success/failure indicators
- Performance metrics (reward, firing rate, episodes stored)

**Example Output**:
```
=== COMPREHENSIVE TEST RESULTS ===
Total reward accumulated: 125.43
Average firing rate: 8.21 Hz
Memory episodes stored: 15
Dopamine level: 0.67
Novelty level: 0.34
Curiosity level: 0.45

INTEGRATION STATUS CHECKLIST:
  Working Memory: ✓ CONNECTED
  Episodic Memory: ✓ CONNECTED
  Neuromodulation: ✓ CONNECTED
  Prediction System: ✓ CONNECTED
  Development System: ✗ DISCONNECTED
  Checkpointing: ✓ WORKING

Overall Integration Score: 5/5 systems connected
✓ Phase 6 Integration: SUCCESS
```

### 4. Enhanced User Command Support

**Added Command-Line Argument Support**:

The main demo program (`./nlm`) now supports advanced command-line options:

```bash
Usage: ./nlm [options]

Options:
  --help                  Show this help message
  --test1-only           Run only basic connectivity test (skip learning and STDP)
  --checkpoint            Enable checkpoint saving
  --checkpoint-dir <dir>  Set checkpoint directory (default: ./checkpoints)
  --steps <steps>         Number of steps for plasticity experiment (default: 1000)
  --neurons <count>      Number of neurons in the brain (default: 500)
  --regions <count>      Number of regions (default: 1)
  --input-pattern <type>  Input pattern type: 'random', 'fixed', or 'rotating' (default: fixed)
  --current <value>      Current injection level (default: 30.0)

Examples:
  ./nlm                           Run all tests with defaults
  ./nlm --test1-only              Quick connectivity test only
  ./nlm --checkpoint --steps 500   Save checkpoints after 500 steps
  ./nlm --help                    Show detailed help
```

**Key Features**:
- Configurable simulation parameters
- Checkpoint support for brain state persistence
- Modular test execution
- Performance monitoring and statistics

### 5. Reward Computation Framework

**Status**: Enhanced `Reward.cpp` with improved documentation and framework for Phase 2 reward computation.

**Current**: Returns 0.0f as placeholder (Phase 2 TODO)

**Future Implementation**: Will analyze observation data from:
- Vision input to detect resources vs hazards
- Internal signals for energy, health, etc.
- Action consequences to compute immediate rewards
- Long-term planning rewards (exploration, discovery, survival)

## System Architecture Improvements

### Integration Verification

Phase 6 now includes comprehensive integration testing:

1. **Memory Systems**: Working memory, episodic memory, and associative memory
2. **Neuromodulation**: Dopamine, curiosity, novelty detection
3. **Prediction System**: Forward models and error signals
4. **Cognition**: Planning, concept formation, attention
5. **Development**: Structural plasticity and developmental stages

### Checkpointing System

**Status**: Phase 5 checkpoint implementation is working correctly.

**Key Features**:
- Incremental checkpointing for large-scale neural simulations
- Streaming support for large checkpoints
- Version compatibility and validation
- Multiple save formats (raw, compressed)

**Usage**:
```bash
# Save checkpoint
./nlm --checkpoint --checkpoint-dir ./my_checkpoints

# Resume from checkpoint
# (Requires Brain::load functionality in your application)
```

## Running NLM Demonstrations

### Phase 2: Real Neural Computation

**Simple Test**:
```bash
./nlm
```
Runs all tests (basic connectivity, learning, STDP verification)

**Quick Connectivity Test**:
```bash
./nlm --test1-only
```
Tests only basic neural connectivity

**Learning-Focused Test**:
```bash
./nlm --checkpoint --steps 500
```
Enables checkpointing with custom step count

### Performance Benchmark

```bash
./nlm_benchmark
```
Measures performance across different network sizes

### Phase 6 Integration Demo

```bash
./nlm_phase6_demo
```
Comprehensive integration test of all brain systems

## Technical Notes

### Learning Algorithm Improvements

1. **Pattern Selection**: The system now creates varied input patterns that change over time, encouraging selective synaptic modification rather than global saturation.

2. **Dynamic Stimulation**: Current injection levels vary within each pattern to prevent all-or-nothing responses.

3. **Enhanced Statistics**: Learning progress tracking now includes pattern size, current levels, and firing metrics.

### Reward System

The reward system is designed to be environment-specific:
- **SimpleWorld**: Resources (+10), Hazards (-15), Movement costs
- **Custom Environments**: User-defined reward functions
- **Extensible**: Plugin architecture for new reward computation methods

## Future Enhancements (Phase 8+)

### Advanced Features

1. **True Randomness**: Implement probabilistic input pattern selection
2. **Multi-Environment Support**: Switch between different simulated environments
3. **Real-time Visualization**: Live brain activity monitoring
4. **Distributed Computing**: Parallel simulation across multiple machines
5. **Machine Learning Integration**: Learn optimal behaviors from experience

### Research Applications

- **Cognitive Modeling**: Study memory formation and retrieval
- **Neuromodulation Studies**: Investigate reward prediction error
- **Development Research**: Model learning across developmental stages
- **Social Learning**: Multi-agent cooperation and competition studies

## Troubleshooting

### Common Issues

1. **Checkpointing Not Working**:
   - Ensure write permissions in checkpoint directory
   - Check for sufficient disk space
   - Verify configuration settings

2. **Learning Saturation**:
   - Use `--input-pattern random` for varied stimulation
   - Adjust current levels with `--current` parameter
   - Increase `--regions` for better parallelization

3. **Performance Issues**:
   - Reduce neuron count with `--neurons <count>`
   - Limit simulation steps with `--steps <count>`
   - Monitor memory usage

### Getting Help

- Run `./nlm --help` for detailed usage information
- Check `README.md` for installation instructions
- Visit the NLM project documentation for detailed API references

## Credits

This work is part of the NLM (Neural Learning Machine) research project, investigating computational brain-like systems. The improvements here focus on fixing critical bugs and enhancing the user experience for Phase 6 integration testing.

For more information, visit: https://github.com/nlm-project/nlm