# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Status (Updated August 2026)

**INTEGRATION COMPLETE - Major Cognitive Systems Now Connected:**

#### ✅ Memory Systems - FULLY INTEGRATED
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay  
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing via Brain::step() integration

#### ✅ Neuromodulation Integration - ENHANCED
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### ✅ Prediction System - INTEGRATED
- Prediction system now updated in brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### ✅ Cognition Systems - CONNECTED
- Neural planner now integrated with action selection
- Concept formation processes experiences from episodic memory
- Attention system processes working memory and episodic content
- All cognition systems connected to perception and action

#### ✅ Development Integration - MAINTAINED
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### ✅ Persistence - IMPLEMENTED
- Checkpoint save/load implemented via CheckpointSystem
- Brain state serialization working
- Can resume from saved checkpoints

#### ✅ Replay and Consolidation - FUNCTIONAL
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Key Improvements (Recent Updates)

### 🔗 **System Integration (CRITICAL FIX)**
- **Before:** Memory, cognition, and prediction systems existed but were disconnected
- **After:** All systems now connected via Brain::step() with proper integration points:
  - Working memory ↔ Episodic memory transfer (every 20 steps)
  - Continuous episodic memory relevance updates
  - Attention system integrates with episodic memory
  - Neural associative memory connects working and episodic patterns

### ⚙️ **Configuration System Modernization**
- Added JSON/YAML parser support with automatic format detection
- Implemented nested configuration keys (e.g., `brain.neuron_count`)
- Added comprehensive validation rules for critical parameters
- Implemented runtime parameter changes with source tracking
- Full backward compatibility maintained

### 📚 **Documentation Updates**
- Phase 6 status corrected to reflect actual integration progress
- Added technical API documentation for configuration system
- Provided examples of advanced usage patterns
- Clear separation between implemented and incomplete systems

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Advanced Configuration

### JSON Configuration File
```json
{
    "brain": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001
    },
    "neuromodulation": {
        "dopamine": {
            "level": 0.5,
            "plasticity_factor": 1.0
        },
        "curiosity": {
            "exploration_rate": 0.3
        }
    },
    "log": {
        "level": "INFO",
        "file": "nlm.log"
    }
}
```

### Command Line Configuration
```bash
./nlm --config=config.json \
     --brain.neuron_count=1000 \
     --neuromodulation.dopamine.level=0.8 \
     --log.level=DEBUG
```

### Runtime Configuration Changes
```cpp
auto config = brain->getConfig();
config->set("brain.connection_probability", 0.2f, ConfigSource::Runtime);
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities (Types, Config, Random, Logger)
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics (LIF neurons, spikes)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian)
│   ├── development/    # Developmental system (stages, maturation)
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
    ├── default.cfg (backward compatibility)
    ├── default.json (modern format)
    └── default.yaml (modern format)
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- **NEW: Comprehensive cognitive integration implemented**

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics ✅ **INTEGRATED**
- Whether neuromodulation can affect plasticity in a coordinated way ✅ **WORKING**
- Whether prediction can become a central organizing principle ✅ **CONNECTED**
- Whether developmental stages can modulate learning ✅ **FUNCTIONAL**
- Whether replay can reinforce memory consolidation ✅ **IMPLEMENTED**
- Whether the complete brain loop functions coherently ✅ **VERIFIED**

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

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

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
