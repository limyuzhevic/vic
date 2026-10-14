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

### Main NLM Application (Advanced Features)
```bash
./nlm                    # Basic run with default configuration
./nlm --interactive      # Interactive mode for real-time control
./nlm --enable-planner --enable-prediction --enable-neuromod  # Advanced integration
./nlm --checkpoint=saved_brain.bin --duration=5000         # Load/save brain state
./nlm --help             # Display all available options
```

## Advanced Features (Phase 6 Integration)

### 1. Enhanced Command-Line Interface

The main NLM application now provides comprehensive command-line options:

```bash
# Basic usage
./nlm

# Interactive mode for real-time control
./nlm --interactive

# Advanced integration with all systems enabled
./nlm --enable-planner --enable-prediction --enable-neuromod --enable-dev

# Checkpointing for persistence
./nlm --checkpoint=saved_brain.bin --duration=5000

# Custom simulation duration
./nlm --duration=10000

# Verbose output for debugging
./nlm --verbose
```

### 2. Interactive Mode

```
Enter interactive mode? (y/n): y

=== NLM Interactive Mode ===
Commands:
  status    - Display brain status
  save <file> - Save brain state to file
  load <file> - Load brain state from file
  step N     - Run N simulation steps
  inject <target> <current> - Inject current into neurons
    <target>: neuron_id (e.g., 42) or neuron_type (sensory, internal, motor)
  help       - Show help message
  quit/q     - Exit interactive mode

>>> status
=== NLM Brain Status ===
Regions: 1
Total neurons: 500
Total synapses: 12845
Active neurons: 12
Firing neurons (this step): 3
Total spikes: 42471
Average firing rate: 85.94 Hz
E/I ratio: 2.15

Memory Systems:
  Working Memory: 12 traces
  Episodic Memory: 45 episodes
  Associative Memory: 89 associations

Cognitive Systems:
  Neural Planner: INTEGRATED
  Concept Formation: INTEGRATED
  Attention: INTEGRATED

Prediction System: INTEGRATED
Neuromodulation:
  Dopamine: 0.78
  Curiosity: 0.65
  Novelty: 0.42

Development: 2 (Maturation)

=== End Status ===
```

### 3. Memory Systems Integration

Phase 6 provides comprehensive memory integration:

**Working Memory**
- Capacity: Enhanced with configurable limits (default: 100 traces)
- Mechanisms: Persistent activity, recurrent connections, competition
- Storage: Neural activity patterns, encoding/decoding via recurrent networks

**Episodic Memory**
- Capacity: Configurable episode limits (default: 2000 episodes)
- Storage: Experience encoding with temporal ordering
- Replay: Automatic replay during rest periods for consolidation
- Consolidation: Relevance-weighted consolidation of important episodes

**Associative Memory**
- Mechanisms: Co-activation Hebbian plasticity, pattern completion
- Retrieval: Spreading activation from partial cues
- Integration: Connected to working memory and concept formation

### 4. Neuromodulation Integration

Enhanced neuromodulation with five key systems:

**Dopamine (DA)**
- Reward prediction error signaling
- Neural excitability modulation
- Memory consolidation enhancement
- Plasticity factor adjustment

**Acetylcholine (ACh)**
- Attention and focus modulation
- Memory encoding enhancement
- Neural gain control

**Norepinephrine (NE)**
- Arousal and vigilance regulation
- Exploration-exploitation balance
- Stress response integration

**Serotonin (5-HT)**
- Mood state modulation
- Impulsivity control
- Social behavior regulation

**Curiosity** (integrated with NE)
- Exploration motivation
- Novelty-driven learning
- Intrinsic reward system

### 5. Cognitive Integration

**Neural Planner**
- Action sequence evaluation with planning depth
- Forward modeling of action consequences
- Confidence-weighted decision making
- Connected to concept formation and attention

**Concept Formation**
- Pattern recognition and categorization
- Concept stability and generalization
- Learning from experience over time
- Connected to memory systems and planning

**Attention System**
- Competitive selection among neural populations
- Winner-take-all dynamics
- Top-down and bottom-up integration
- Enhancement by neuromodulators

### 6. Prediction System Integration

**Forward Model**
- Prediction of next sensory states
- Prediction error computation
- Confidence tracking
- Integration with working memory

**Prediction Error**
- Reward prediction error signaling
- Novelty detection
- Learning rate modulation
- Connection to dopaminergic systems

### 7. Development Integration

**Developmental Stages**
- Initial: High plasticity, exploratory behavior
- Critical period: Rapid learning, strong plasticity
- Maturation: Moderate plasticity, stable behavior
- Adult: Low plasticity, specialized functions

**Structural Plasticity**
- Synaptogenesis during development
- Pruning of unused connections
- Myelin-like stabilization processes
- Adaptive to environmental demands

### 8. Checkpointing System

**Save/Load Functionality**
- Persistent brain state across simulations
- Hierarchical checkpoint organization
- Incremental saving for large-scale learning
- Integration with memory replay systems

**Checkpoint Features**
- Neural state serialization
- Synapse weight preservation
- Memory system storage
- Development state tracking

### 9. Performance Optimizations

**Memory Management**
- Working memory capacity limits
- Episodic memory consolidation
- Efficient storage of neural patterns

**Event-Driven Processing**
- Spike propagation with synaptic delays
- Plasticity rule application
- Neuromodulator effect calculation

**Parallel Processing**
- Multi-region neural processing
- Independent memory system updates
- Concurrent planning and execution

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

## License

MIT

## Authors

Research project — See docs for scientific background.

## Further Reading

For detailed information about Phase 6 integration, see:
- `docs/PHASE6_FINAL_AUDIT.md` - Comprehensive audit of integration achievements
- `docs/PHASE6_ARCHITECTURE.md` - Detailed architecture documentation
- `docs/PHASE6_FINAL_RESULTS.md` - Scientific results and evaluations
