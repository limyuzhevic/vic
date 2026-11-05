NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Current Status (Working Version)

Phase 6: FINAL INTEGRATION - All core systems operational

- ✅ Real spiking neural computation with LIF neurons
- ✅ Event-driven spike propagation with delays
- ✅ STDP and Hebbian plasticity
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Neuromodulation systems working (dopamine, curiosity, novelty)
- ✅ Memory systems integrated (working memory, episodic memory)
- ✅ Prediction systems operational
- ✅ Development integration
- ✅ Checkpoint save/load working
- ✅ Replay and consolidation functional

## Key Improvements Made

### 1. Neuromodulation & Reward Systems - FIXED
- Implemented real reward computation based on observation data
- Dopamine system now responds to actual rewards from environment
- Curiosity drives exploration based on novelty detection
- Reward prediction error signaling implemented
- All neuromodulators now have real dynamics and effects

### 2. Activity Level Control - ADDED
- Commands to monitor and limit neural firing rates
- Configurable activity thresholds
- Real-time activity monitoring tools

### 3. Memory Integration - ENHANCED
- Working memory now properly tracks active traces
- Episodic memory stores actual experience data
- Memory systems connected to neural plasticity
- Replay system functional for memory consolidation

### 4. Advanced Configuration - ADDED
- Comprehensive configuration options for power users
- Runtime configuration without rebuilding
- Command-line argument parsing for all parameters

### 5. Monitoring Tools - ADDED
- System health monitoring commands
- Performance metrics tracking
- Neural activity visualization tools
- Checkpoint status reporting

### 6. Bug Fixes
- Fixed reward computation (was always 0.0f)
- Fixed working memory active trace tracking
- Fixed checkpoint file handling
- Fixed memory system integration

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running Demos

Build all targets first. The entry points are:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase5_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_phase6_text_demo # Text-as-a-sense demo
./nlm_benchmark
```

## New Commands (Phase 6)

The Phase 6 build includes new commands for monitoring and control:

### System Monitoring
- `nlm_monitor` - Real-time system monitoring
- `nlm_status` - Quick system status check
- `nlm_config` - Configuration management

### Development Tools
- `nlm_explore` - Explore brain state
- `nlm_profile` - Performance profiling
- `nlm_debug` - Debug mode with detailed logging

## Example Usage

### Check System Status
```bash
./nlm_status
```

### Run with Custom Configuration
```bash
./nlm_phase6_demo \
  --neuron_count=2000 \
  --max_steps=5000 \
  --enable_checkpointing=true \
  --enable_replay=true \
  --enable_development=true
```

### Monitor During Runtime
```bash
./nlm_monitor --port=8080 --update_interval=1.0
```

## Architecture Philosophy

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

## License

MIT

## Authors

Research project — See docs for scientific background.