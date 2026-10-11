# NLM Phase 6 Demo - Intermediate User Guide

## Overview

The NLM Phase 6 Integration Demo showcases the complete artificial brain system with all subsystems integrated. This demo is designed for intermediate users who want to:

- Understand how all brain components work together
- See real-world integration in action
- Configure and run comprehensive demonstrations
- Analyze system integration status

## Key Features

### 1. Integration Verification
The demo first verifies that all systems are properly connected:
- Memory systems (working + episodic memory)
- Neuromodulation systems (dopamine, curiosity, novelty)
- Cognitive systems (prediction, planning, attention)
- Development system
- Checkpoint infrastructure
- Memory replay system

### 2. Configuration Options
Configure the demo before running:

```bash
# Basic configuration
./nlm_phase6_demo

# Custom settings
./nlm_phase6_demo --neurons=1000 --steps=5000 --no-dev --no-checkpoint

# Show help
./nlm_phase6_demo --help
```

### 3. Detailed Output
The demo provides comprehensive output including:
- Performance metrics (reward, firing rates, memory usage)
- Timing information (wall clock time, efficiency analysis)
- System connectivity status (green checkmarks for connected systems)
- Integration assessment (excellent/good/needs work)

### 4. Progress Tracking
Monitor real-time progress during the demo:
- Integration test results
- Memory system activity
- Neuromodulation effects
- Checkpoint operations

## Running the Demo

### Basic Usage
```bash
# Run with default settings
nlm_phase6_demo

# View help
nlm_phase6_demo --help

# Custom configuration
nlm_phase6_demo --neurons=1000 --steps=3000 --quiet
```

### Output Examples

**Successful Integration:**
```
==================================================
  NLM Phase 6 Integration Demo
==================================================

This demo demonstrates Phase 6: Final Integration of the NLM brain.

Features demonstrated:
  - Real spiking neural computation (LIF neurons)
  - Event-driven spike propagation with synaptic delays
  - Multiple plasticity rules (STDP, Hebbian, structural)
  - Integrated memory systems (working, episodic)
  - Neuromodulation (dopamine, curiosity, novelty)
  - Prediction system with forward models
  - Cognitive systems (planning, attention)
  - Development and maturation over time
  - Checkpoint save/load and memory replay

Demo Configuration:
  Neurons: 500
  Max Steps: 2000
  Checkpointing: Enabled
  Replay: Enabled
  Development: Enabled

Running integration verification tests...

Test 1: Integration Verification
PASS: All systems are properly integrated

Test 2: Memory Integration
Memory systems working together

Test 3: Neuromodulation Integration
Neuromodulation affecting plasticity and dynamics

... (continuing with other tests) ...
```

**Final Results:**
```
=== FINAL RESULTS ===

Performance Metrics:
  Total Reward: 1.234567
  Average Firing Rate: 8.765432 Hz
  Synaptic Weight Mean: 0.1234567
  Episodes Stored: 42
  Novelty Level: 0.8912345
  Curiosity Level: 0.7654321
  Dopamine Level: 0.9876543

Timing:
  Wall Clock Time: 3.456 seconds
  Average Step Time: 0.001728 seconds/step

Performance Analysis:
  Reward per Step: 0.000617283
  Efficiency Score: 0.617283 (higher is better)

=== INTEGRATION STATUS ===

System Connectivity:
  Memory System:
    Working Memory: ✓ CONNECTED
    Episodic Memory: ✓ CONNECTED
  Neuromodulation:
    Dopamine System: ✓ CONNECTED
  Cognitive Systems:
    Prediction: ✓ CONNECTED
    Development: ✓ CONNECTED
  Infrastructure:
    Checkpointing: ✓ WORKING
    Memory Replay: ✓ WORKING

=== OVERALL INTEGRATION ===
Successfully connected systems: 7/7
✓ EXCELLENT: Near-complete integration achieved

The NLM brain now functions as a coherent artificial neural substrate
 capable of changing its own synaptic connections through experience.

==================================================
Phase 6 Integration Demo Complete!
==================================================
```

## Understanding the Results

### Integration Metrics
- **Connected Systems**: Count of successfully integrated brain subsystems
- **Performance**: Reward efficiency and neural activity
- **Timing**: Computational speed and resource usage

### System Status Indicators
- **✓ CONNECTED**: System is properly integrated
- **✗ DISCONNECTED**: System needs integration work
- **⚠ NOT WORKING**: Infrastructure has issues

### Integration Assessment
- **7/7 Connected (Excellent)**: Full system integration achieved
- **4-6 Connected (Good)**: Substantial integration, minor issues
- **<4 Connected (Needs Work)**: Major integration problems

## Advanced Configuration

### Command Line Arguments
```bash
--neurons=N        Number of neurons (default: 500)
--steps=S          Maximum simulation steps (default: 2000)
--no-checkpoint    Disable checkpointing
--no-replay        Disable memory replay
--no-dev           Disable development
--quiet            Reduce verbose output
--help, -h         Show this help
```

### Configuration Options
- **Neuron Count**: Affects brain complexity and computational load
- **Step Count**: Duration of the integration experiment
- **Checkpointing**: Enables save/load functionality
- **Replay**: Enables memory consolidation
- **Development**: Enables age-related plasticity changes
- **Verbose Mode**: Provides detailed progress information

## Troubleshooting

### Common Issues

**Build Errors:**
```bash
# Fix the build
sudo apt-get install build-essential cmake g++
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Runtime Errors:**
```bash
# Ensure dependencies are available
cd build
./nlm_phase6_demo
```

**Help Options:**
```bash
# View help
./nlm_phase6_demo --help

# Debug with verbose output
./nlm_phase6_demo --neurons=100 --steps=500
```

### Performance Tips

- For faster builds: Use `make -j$(nproc)`
- For debugging: Run with smaller settings: `--neurons=100 --steps=500`
- For production: Enable checkpointing for long runs: `--neurons=1000 --steps=10000`

## Next Steps

After understanding the Phase 6 demo:

1. **Explore Other Demos:**
   - `nlm_phase3_demo`: World interaction
   - `nlm_phase4_demo`: Emerging cognition
   - `nlm`: Basic neural computation

2. **Advanced Usage:**
   - Modify demo configuration
   - Add custom experiment logic
   - Integrate with external systems

3. **Research Applications:**
   - Study brain integration principles
   - Analyze learning emergence
   - Investigate neural plasticity

## Additional Resources

- **Phase 3 Demo**: Shows world interaction loop
- **Phase 4 Demo**: Demonstrates cognitive emergence
- **Phase 5 Experiments**: Comprehensive research experiments
- **Documentation**: Full documentation in docs/ directory

This guide provides everything needed to understand and effectively use the NLM Phase 6 Integration Demo.
