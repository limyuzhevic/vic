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
- **Working Memory with Sensory Encoding**: Working memory now directly encodes sensory input as persistent activity patterns. The `processSensoryInput()` method extracts all sensory data (vision, touch, internal signals, proprioception) and stores them in working memory for immediate neural processing and pattern completion.

- **Episodic Memory with Consolidation**: Episodic memory now implements sophisticated consolidation with relevance evaluation. The `consolidate()` method prunes low-relevance episodes while preserving important ones, using multi-factor relevance scores (reward, age, novelty, prediction error) to guide consolidation.

- **Associative Memory with Pattern Completion**: Pattern completion through associative memory enables recall of related memories and filling in missing information.

- All memory systems connected to neural processing for bidirectional interaction.

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
- **Concept Formation with Working Memory**: Concept formation now leverages working memory patterns and associative memory for enhanced concept discovery.
- **Attention with Competitive Selection Dynamics**: Attention mechanisms now incorporate working memory patterns and bottom-up salience from prediction errors.
- All cognition systems connected to perception and action.

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- **Episodic Memory Replay**: Episodic memory replay now integrates with associative memory to strengthen neural connections and update concepts.
- **Memory Consolidation for Important Episodes**: Enhanced consolidation mechanism preserves high-relevance experiences for long-term learning.
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
│   │   ├── WorkingMemory: Stores sensory input patterns
│   │   ├── EpisodicMemory: Conserves experiences with relevance evaluation
│   │   └── AssociativeMemory: Connects related patterns
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Integration Details

### Enhanced Working Memory Encoding
The working memory system now directly encodes all sensory input:

```cpp
// processSensoryInput() now extracts and stores sensory data:
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Extract vision (256), touch (8), internal (4), proprioception (6)
    std::vector<float> sensoryData;
    sensoryData.reserve(274); // Total input size
    
    // Store in working memory for immediate use
    if (!sensoryData.empty()) {
        brain_->getWorkingMemory()->store(sensoryData, 1.0f);
    }
}
```

### Sophisticated Episodic Memory Consolidation
The episodic memory consolidation now uses multi-factor relevance evaluation:

```cpp
void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Calculate multi-factor relevance:
    // - Reward (40%): Most important for learning
    // - Age (20%): Older experiences more relevant
    // - Time-based (20%): Recent experiences prioritized
    // - Novelty (20%): Novel experiences preserved
    
    for (const auto& episode : pImpl->episodes) {
        float relevance = episode.reward * 0.4f +
                         std::min(1.0f, static_cast<float>(episode.age) / 1000.0f) * 0.2f +
                         timeFactor * 0.2f +
                         noveltyFactor * 0.2f;
        
        // Keep high-relevance or recent episodes
        if (relevance > relevanceThreshold || episode.age < 500) {
            keptEpisodes.push_back(episode);
        }
    }
}
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
- **Working memory with sensory encoding**
- **Episodic memory with consolidation**
- Concept formation from experience patterns
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
- **Working memory directly encodes sensory input**
- **Episodic memory implements sophisticated consolidation**
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- **Replay and consolidation functional**
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
- Whether working memory can directly encode sensory input as persistent patterns
- Whether episodic memory can implement sophisticated consolidation with relevance evaluation
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system with enhanced memory integration:

```
WORLD
  ↓
SENSORY INPUT → WORKING MEMORY (direct encoding)
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION (with consolidation)
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
MEMORY / DEVELOPMENT (with consolidation)
  ↓
CHANGED BRAIN (replayed, consolidated)
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
