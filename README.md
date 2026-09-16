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

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

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

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── examples/          # Example scripts and applications
│   ├── simple_agent.py
│   ├── integrated_demo.py
│   └── checkpoint_demo.py
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
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
- Phase 6 integration experiment created

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

## Key Improvements in Phase 6 Integration

### 1. Complete System Integration
- **Before**: Systems existed but were disconnected
- **After**: All systems work together in coherent brain loop
- **Impact**: Brain functions as unified artificial intelligence

### 2. Memory Integration
- **Working Memory**: Real-time pattern storage, competition, decay
- **Episodic Memory**: Experience recording, replay, consolidation
- **Integration**: Connected to sensory input and action selection

### 3. Neuromodulation Integration
- **Dopamine**: Affects neural excitability and plasticity
- **Curiosity**: Drives exploration based on prediction error
- **Novelty**: Detects interesting experiences
- **Integration**: All connected to plasticity rules

### 4. Prediction Integration
- **Prediction System**: Next state anticipation
- **Prediction Error**: Learning signals for adaptation
- **Integration**: Connected to curiosity and memory

### 5. Cognitive Integration
- **Neural Planner**: Multi-step action evaluation
- **Concept Formation**: Pattern generalization
- **Attention**: Competitive selection dynamics
- **Integration**: Connected to perception and action

### 6. Development Integration
- **Developmental Stages**: Age-based plasticity modulation
- **Maturation**: Progressive refinement
- **Integration**: Affects all learning mechanisms

### 7. Persistence and Replay
- **Checkpoint System**: Life history tracking
- **Replay Mechanism**: Memory consolidation
- **Integration**: Connected to development and memory

## Example Scripts

The `examples/` directory contains comprehensive scripts demonstrating Phase 6 integration:

### Simple Agent Example
```python
# examples/simple_agent.py
# Basic agent demonstrating Phase 6 integrated systems
```

### Integration Demo
```python
# examples/integrated_demo.py
# Complete demonstration of all integrated systems working together
```

### Checkpoint Demo
```python
# examples/checkpoint_demo.py
# Demonstrates persistence and replay mechanisms
```

### Phase 6 Demo (C++)
```bash
./nlm_phase6_demo
# Comprehensive integration test
```

## Testing Integration

### Unit Tests
```bash
# Run Phase 6 integration tests
./nlm_test --filter=phase6

# Run specific integration tests
./nlm_test --test=memory_integration
./nlm_test --test=neuromodulation_integration
./nlm_test --test=prediction_integration
```

### Integration Verification
```python
# Verify all systems are properly integrated
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check key systems
print('Working Memory:', '✓' if brain.getWorkingMemory() else '✗')
print('Episodic Memory:', '✓' if brain.getEpisodicMemory() else '✗')
print('Dopamine:', '✓' if brain.getDopamine() else '✗')
print('Curiosity:', '✓' if brain.getCuriosity() else '✗')
print('Prediction:', '✓' if brain.getPredictionSystem() else '✗')
print('Development:', '✓' if brain.getDevelopmentSystem() else '✗')
print('Planning:', '✓' if brain.getPlanner() else '✗')
print('Concept Formation:', '✓' if brain.getConceptFormation() else '✗')
print('Attention:', '✓' if brain.getAttention() else '✗')
"
```

## Documentation

Comprehensive documentation is available in the `docs/` directory:

- **ARCHITECTURE.md**: System architecture overview
- **SCIENCE.md**: Scientific background and theory
- **ROADMAP.md**: Development roadmap and future plans
- **EXPERIMENTS.md**: Experiment descriptions and results
- **LIMITATIONS.md**: Current limitations and challenges
- **PHASE6_FINAL_AUDIT.md**: Detailed Phase 6 integration audit
- **MEMORY_INTEGRATION.md**: Memory system documentation
- **NEUROMODULATION_INTEGRATION.md**: Neuromodulation system documentation
- **PREDICTION_INTEGRATION.md**: Prediction system documentation
- **DEVELOPMENT_INTEGRATION.md**: Development system documentation

## Getting Started

1. **Build the project**: Follow the build instructions above
2. **Run Phase 6 demo**: `./nlm_phase6_demo` to test integration
3. **Explore examples**: Check `examples/` directory for usage examples
4. **Read documentation**: Start with `docs/ARCHITECTURE.md` for overview
5. **Run tests**: Use `./nlm_test` to verify system functionality

## Conclusion

Phase 6 represents the culmination of all previous development phases, creating a fully integrated artificial brain where neural dynamics, memory, neuromodulation, prediction, and cognition work together as a coherent system.

The integration demonstrates that:
- **Neural dynamics** drive perception and action
- **Memory systems** store and replay experiences
- **Neuromodulation** guides learning and exploration
- **Prediction** enables anticipation and planning
- **Cognition** extracts meaning and forms plans
- **Development** shapes the brain over time
- **Persistence** enables life history tracking

This comprehensive integration provides a foundation for artificial systems that learn, develop, and adapt like biological brains.
