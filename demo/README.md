# NLM Demo Application

This directory contains the NLM (Neural Learning Machine) demo application.

## Available Demos

The NLM provides several demo modes that showcase different aspects of the Phase 6 integration:

### 1. **nlm_demo** (demo/nlm_demo.cpp)
- Command: `./nlm_demo [--demo MODE] [--steps STEPS] [--input STRENGTH]`
- **Phase 6 Integration Demo**: Comprehensive demonstration of all Phase 6 systems
- Shows the working neural brain with real LIF dynamics, plasticity, memory systems, prediction systems, and cognitive capabilities
- Tests all integrated components: memory (working, episodic, associative), prediction (neural, action consequence), cognition (planner, concept formation, attention)

### 2. **nlm** (src/main.cpp) - Basic Phase 2 Demo
- Command: `./nlm` or `./nlm --demo phase2`
- Simple demonstration of real neural computation with LIF neurons, STDP plasticity, and structural changes

### 3. **nlm_phase3_demo** - World Interaction Demo
- Command: `./nlm_phase3_demo`
- Shows the world interaction loop with sensory input and motor output

### 4. **nlm_phase4_demo** - Cognitive Capabilities Demo
- Command: `./nlm_phase4_demo`
- Demonstrates prediction, memory, and planning capabilities

### 5. **nlm_phase5_demo** - Performance and Checkpointing Demo
- Command: `./nlm_phase5_demo`
- Shows performance optimizations and checkpoint/restart functionality

### 6. **nlm_phase6_demo** - Integration Test
- Command: `./nlm_phase6_demo`
- Phase 6 integration verification (unchanged from original)

### 7. **nlm_phase6_text_demo** - Text as Sense Demo
- Command: `./nlm_phase6_text_demo`
- Phase 6 demo with text-as-a-sense (new in Phase 6)

### 8. **nlm_benchmark** - Performance Benchmark
- Command: `./nlm_benchmark`
- Performance testing with different network sizes

## Demo Usage Examples

### Basic Demo
```bash
./nlm_demo                    # Run Phase 6 integration demo with default settings
./nlm_demo --demo phase6       # Explicitly run Phase 6 demo
./nlm_demo --steps 5000       # Run with 5000 simulation steps
./nlm_demo --input 50.0       # Use stronger input stimulation
./nlm_demo --visualize        # Enable visualization mode
```

### Phase-Specific Demos
```bash
./nlm_demo --demo basic       # Phase 2: Basic neural computation
./nlm_demo --demo phase3       # Phase 3: World interaction
./nlm_demo --demo phase4       # Phase 4: Cognitive capabilities
./nlm_demo --demo phase5       # Phase 5: Performance optimizations
./nlm_demo --demo phase6       # Phase 6: Full integration
./nlm_demo --demo phase6_text  # Phase 6: Text as a sense
```

### Configuration Examples
```bash
./nlm_demo --demo phase6 --steps 2000 --input 25.0
./nlm_demo --config myconfig.cfg --demo phase6
./nlm_demo --demo phase6 --visualize --steps 10000
```

## Demo Features

### Real Neural Computation
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Integrated Memory Systems
- **Working Memory**: Persistent neural activity patterns
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations and Hebbian learning

### Prediction Systems
- **Temporal Prediction**: Sequence prediction using neural patterns
- **Action-Consequence Prediction**: Prediction of action outcomes
- **Prediction Error**: Neuromodulatory signal for learning

### Cognitive Capabilities
- **Neural Planner**: Action planning with sequence evaluation
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive selection and focus

### Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Novelty-driven exploration motivation
- **Novelty**: Stimulus-driven surprise detection

### Persistence
- **Checkpointing**: Save and load brain state
- **Replay**: Memory consolidation and learning

## Technical Details

The NLM demo demonstrates the complete artificial brain architecture with:

- **500-2000 neurons** with biologically realistic parameters
- **Realistic LIF neuron models** with adaptive thresholds and refractory periods
- **Event-driven spike processing** with synaptic delays
- **Multi-layered plasticity** including STDP, Hebbian, and structural changes
- **Integrated memory systems** with working, episodic, and associative components
- **Prediction systems** that use neural dynamics for temporal and causal inference
- **Cognitive architectures** that demonstrate planning, concept formation, and attention
- **Neuromodulation** that coordinates learning and exploration behavior
- **Persistence mechanisms** that allow the brain to retain learned behaviors across restarts

## System Requirements

- **C++20 compiler** with OpenMP support
- **At least 1GB RAM** for networks of 1000+ neurons
- **Linux/macOS** (Windows support possible with appropriate adjustments)

## Building the Demos

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

Run demos:
```bash
cd build
./nlm_demo              # Main demo
./nlm                  # Basic demo
./nlm_phase3_demo       # Phase 3 demo
./nlm_phase6_demo       # Phase 6 integration demo
./nlm_benchmark         # Performance benchmark
```

## Visualization

The `--visualize` or `-v` flag enables basic visualization mode.
- **Node visualization**: Shows active neurons with size proportional to activity
- **Connection visualization**: Displays synaptic connections between neurons
- **Spike visualization**: Tracks neural firing patterns over time
- **Memory visualization**: Shows memory traces and replay activity

Note: For advanced visualization, see the `src/visualization/` directory for the full visualization interface.

## Output

Each demo produces detailed console output showing:
- System initialization and configuration
- Neural activity patterns (spike counts, firing rates)
- Memory system status (working memory traces, episodic memories)
- Prediction system state (prediction errors, confidence levels)
- Cognitive system activity (planning, concept formation)
- Neuromodulation levels (dopamine, curiosity, novelty)
- Integration metrics (system connectivity, functionality)

## Troubleshooting

### "Failed to initialize brain!"
- Check system resources (memory, CPU)
- Ensure all dependencies are installed
- Try reducing `--steps` or `--input` values

### "Configuration file not found"
- Use absolute path: `--config /path/to/config.cfg`
- Or place config file in working directory

### Slow performance
- Increase `--steps` for better throughput measurement
- Check system OpenMP support
- Ensure compiler optimizations are enabled

### Demo not working
- Try running `./nlm` (basic demo) first
- Check console output for error messages
- Verify all required libraries are linked

For more information, refer to the NLM documentation in the `docs/` directory.
