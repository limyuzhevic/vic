# NLM (熙然) — Neural Learning Machine

## Overview

NLM is an experimental artificial developmental brain project that implements a **modular, biologically-inspired neural computation system**. Unlike conventional deep learning frameworks (Transformers, RNNs, etc.), NLM is built from first principles of neuroscience, implementing real spiking neural computation with:

- **Leaky Integrate-and-Fire (LIF) neurons** with event-driven dynamics
- **Synaptic transmission** with delays and plasticity
- **Spike-based communication** and **emergent cognition**
- **Experience-driven development** without pre-programmed knowledge

The project follows the **6-Phase Development Roadmap**, each building on the previous phase to create increasingly sophisticated cognitive capabilities while maintaining the core neural architecture.

## Key Principles

1. **Neural Foundation**: All computation is based on individual neurons, synapses, and spike-based communication
2. **No ML Frameworks**: NLM implements its own neural computation from the ground up
3. **No Transformer Architecture**: Uses competitive attention and spiking dynamics, NOT transformer QKV attention
4. **Biological Inspiration**: Designed to be incrementally more biologically accurate
5. **Experience-Driven**: All knowledge emerges from interaction with environment
6. **Deterministic**: Reproducible experiments via explicit random seeds
7. **Modular**: Clear interfaces between components for extensibility

## What NLM Is (and Isn't)

### NLM **IS**:
- A spiking neural network with real LIF neuron dynamics
- An artificial developmental brain that learns through experience
- A modular framework for cognitive science research
- A biologically-inspired alternative to deep learning
- A system capable of forming concepts, planning actions, and social learning

### NLM **IS NOT**:
- A transformer architecture
- An LLM replacement
- A pretrained knowledge system
- "LLM + memory" or "LLM + tools"
- A conventional deep learning framework

If the architecture begins to resemble a transformer, it should be redirected or the project renamed.

## Current Status: Phase 5 Complete

NLM is currently at **Phase 5: Scaling + Research Platform**, with comprehensive support for large-scale neural simulations and experimental research.

### **Core Accomplishments:**

#### **Phase 1 — Skeleton (COMPLETE)**
- Modular project structure with clear component boundaries
- C++20 compilation with modern tooling
- Core types (NeuronId, SynapseId, etc.)
- Configuration system and logging infrastructure
- Test suite and documentation

#### **Phase 2 — Working Neural Core (COMPLETE)**
- **Real LIF neuron dynamics** with membrane potential, refractory periods
- **Event-driven spike processing** with synaptic delays
- **STDP and Hebbian plasticity rules** implemented
- **Structural plasticity** (synaptogenesis/pruning)
- Working neural computation that actually learns

#### **Phase 3 — Development + Environment (COMPLETE)**
- **World interface** (SimpleWorld) with sensory observations and motor commands
- **Basic sensory processing** (Vision, Touch, Internal, Proprioception)
- **Basic motor output** (Action decoding)
- **Developmental system** with stages that modify plasticity
- **Reward computation** and neuromodulation (dopamine-like signals)
- **Agent/Body system** with energy management

#### **Phase 4 — Emerging Cognition (COMPLETE)**
- **Prediction systems** (NeuralPrediction, PredictionErrorSignal)
- **Memory systems** (Working Memory, Episodic Memory, Semantic Memory, Procedural Memory)
- **Cognitive mechanisms** (Attention, Planning, Self-Model, Social Learning)
- **Comprehensive experiment suite** for all cognitive capabilities

#### **Phase 5 — Scaling + Research Platform (IMPLEMENTED)**
- **Performance infrastructure** (MemoryPool, EventQueue, SparseConnectivity, SIMD vectorization)
- **Research infrastructure** (CheckpointSystem, AblationSystem, ScalingBenchmark)
- **Biological fidelity improvements** (Adaptive models, homeostatic plasticity)
- **Scalability features** (Event-driven processing, sparse connectivity, multithreading)

## Key Features

### **Neural Core**
```
LIF Neuron → Synapse → SpikeSystem → Plasticity → Development
```

### **Cognition Systems**
1. **Prediction**: Learns temporal sequences, predicts action consequences
2. **Memory**: Forms episodic memories, maintains working memory, discovers concepts
3. **Attention**: Competitive selection without transformer architecture
4. **Planning**: Multi-step action selection based on predictions
5. **Self-Model**: Sensorimotor self-awareness and body schema
6. **Social Learning**: Observation and imitation of other agents

### **World Interaction**
```
WORLD → SensoryPercept → AgentBrain → NLM Brain → MotorCommand → WORLD
```

### **Neuromodulation**
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration motivation
- **Novelty**: Surprise detection
- **PredictionError**: Learning signals for model updating

### **Developmental Plasticity**
- **Stages**: Initial → CriticalPeriod → Maturation → Adult
- **Plasticity modifiers**: Stage-dependent learning rates
- **Homeostatic mechanisms**: Stability and resource management

## System Architecture

### **Core Layers**
1. **Core**: Types, Config, Random, Logger, Clock
2. **Brain**: Neuron, Synapse, Population, Region, Brain
3. **Dynamics**: NeuralDynamics, SpikeSystem
4. **Plasticity**: PlasticityRule, Hebbian, STDP, StructuralPlasticity
5. **Development**: DevelopmentSystem, Synaptogenesis, Pruning, Maturation
6. **Neuromodulation**: Neuromodulator, Dopamine, Novelty, Curiosity, PredictionError
7. **Memory**: WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory, AttentionalSelection
8. **Prediction**: NeuralPrediction, ActionConsequencePredictor
9. **Cognition**: NeuralPlanner, ConceptFormation, SelfModel, SocialLearning, SpatialRepresentation, TemporalRelation
10. **Sensory**: SensoryInput, Vision, Audio, InternalSignals
11. **Motor**: Action, MotorSystem
12. **Environment**: Environment, Observation
13. **Experiments**: Experiment, ExperimentRunner, Metrics

### **Data Flow (Phase 3/4)**
```
WORLD → SENSORY PERCEPT → ATTENTION → NEURAL PROCESSING
                   ↓              ↓
                PREDICTION     WORKING MEMORY
                   ↓              ↓
                EPISODIC MEMORY → CONCEPT FORMATION
                   ↓              ↓
                PLANNING → SELF-MODEL
                   ↓
                 MOTOR ACTION → WORLD
                   ↓
                REWARD / ERROR → NEUROMODULATION
                                   ↓
                                PLASTICITY
                                   ↓
                             SOCIAL LEARNING
```

## Technical Specifications

### **Neural Properties**
- **Neurons**: Leaky Integrate-and-Fire with refractory periods
- **Synapses**: Excitatory/inhibitory with configurable delays
- **Connectivity**: Sparse random or structured
- **Plasticity**: STDP, Hebbian, reward-modulated, structural
- **Development**: Stage-dependent plasticity, homeostatic regulation

### **Performance Features**
- **Event-driven processing**: Skip inactive neurons
- **Sparse connectivity**: Adjacency list representation
- **Memory pooling**: Pre-allocated neuron/synapse pools
- **SIMD vectorization**: Parallel neuron state updates
- **Multithreading**: Parallel processing support
- **Streaming**: Large checkpoint I/O

### **Research Capabilities**
- **Checkpointing**: Full brain state serialization with validation
- **Ablation studies**: Mechanism isolation framework
- **Scaling benchmarks**: Performance measurement across sizes
- **Lifelong learning**: Continual adaptation to new experiences
- **Multi-agent studies**: Social interaction analysis

### **Biological Fidelity**
- **Adaptive neurons**: Parameter adjustments based on experience
- **Short-term plasticity**: Facilitation and depression
- **Inhibitory circuits**: Configurable connectivity
- **Homeostatic mechanisms**: Resource balance and stability
- **Resource management**: Energy, attention, working memory constraints

## Usage

### **Basic Execution**
```bash
# Run neural computation demo
./nlm

# Load custom configuration
./nlm --config myconfig.cfg

# Show help information
./nlm --help

# Save current brain state
./nlm --save brain_state.nlm

# Load previous brain state
./nlm --load saved_brain.nlm

# Enter interactive mode
./nlm --interactive
```

### **Advanced Command Line Options**
- `--show-state`: Display detailed brain state information
- `--quiet`: Suppress output (error messages only)
- `--version`: Show version and phase information

### **Interactive Mode Commands**
In interactive mode, you can:
- `help`: Show available commands
- `state`: Show current brain state
- `save <file>`: Save brain state to file
- `load <file>`: Load brain state from file
- `step [n]`: Advance simulation by n steps
- `reset`: Reset brain to initial state
- `exit` or `quit`: Exit interactive mode

### **Configuration**
NLM uses a configuration file (`configs/default.cfg`) with settings for:
- Random seed for deterministic experiments
- Simulation timestep
- Network size (neurons, regions)
- Connection probability
- Plasticity parameters
- Neuromodulation levels
- Memory system capacities
- Checkpoint settings

## Research Applications

### **Experimental Paradigm**
NLM supports a comprehensive experimental framework for studying:
1. **Learning**: Synaptic weight changes through experience
2. **Memory**: Retention, recall, and consolidation
3. **Decision Making**: Action selection under uncertainty
4. **Social Cognition**: Theory of mind and social learning
5. **Development**: Behavioral development across stages
6. **Generalization**: Transfer of learned behaviors to novel situations

### **Data Collection**
- Neural activity (spike counts, firing rates)
- Synaptic weights (before/after learning)
- Behavioral outputs (actions, rewards)
- Memory performance (recall accuracy, reaction times)
- Cognitive abilities (problem solving, planning)

### **Analysis Tools**
- **Metrics collection**: Quantitative measures of cognitive performance
- **Representation analysis**: Neural pattern analysis
- **Scaling benchmarks**: Performance measurement across network sizes
- **Ablation studies**: Mechanism isolation and contribution analysis

## Scientific Value

### **Research Questions Addressed**
1. **How does memory integration work?** How do working memory, episodic memory, and prediction interact?
2. **What is developmental progression?** How do learning capabilities change across stages?
3. **How does attention coordinate cognition?** Without transformer attention?
4. **Can artificial brains develop general intelligence?** Through intrinsic motivation and experience?

### **Novel Contributions**
1. **Integrated cognitive architecture**: Multiple systems working together as a coherent whole
2. **Experience-driven development**: Knowledge emerges from interaction, not preprogramming
3. **Neuromodulation-based learning**: Reward and prediction error signals guide plasticity
4. **Biological plausibility**: Event-driven dynamics and spike-based communication
5. **Scalable implementation**: Supports thousands to millions of neurons

### **Scientific Method**
- **Hypothesis testing**: Clear experimental protocols
- **Controlled variables**: Deterministic random seeds
- **Measurable outcomes**: Quantitative performance metrics
- **Reproducible results**: Comprehensive documentation and test suite

## Documentation

### **Key Documents**
- **ARCHITECTURE.md**: Component descriptions and data flow
- **ROADMAP.md**: Phase-by-phase development progress
- **SCIENCE.md**: Scientific background and theoretical foundations
- **EXPERIMENTS.md**: Experimental protocols and results
- **PHASE6_FINAL_AUDIT.md**: Current integration status and gaps

### **Performance Documentation**
- **PERFORMANCE.md**: Optimization strategies and results
- **BIOLOGICAL_FIDELITY.md**: Biological modeling details
- **SCALING.md**: Scalability analysis and results
- **RESEARCH_METHODS.md**: Scientific methodology and protocols

## Future Directions (Phase 6)

### **Current Integration Gaps (Addressed in This Implementation)**
1. ✅ **Memory systems**: NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory, SemanticMemory, ProceduralMemory all integrated
2. ✅ **Prediction system**: NeuralPrediction integrated into action selection
3. ✅ **Cognition systems**: NeuralPlanner and ConceptFormation integrated into brain loop
4. ✅ **Neuromodulation**: Dopamine connected to memory consolidation
5. ✅ **Development**: Stage-dependent effects on neural excitability and attention
6. ✅ **Performance**: Memory pools, event queues, sparse connectivity integrated

### **Next Steps**
- Enhanced biological fidelity (Hodgkin-Huxley neurons, calcium-based plasticity)
- Anatomically constrained connectivity
- Glial cell interactions
- Hierarchical planning structures
- Reinforcement learning integration
- Distributed simulation capabilities

## Contact and Contribution

NLM is an open research project. Contributions are welcome for:
- Algorithm improvements
- Performance optimizations
- Biological model extensions
- Documentation enhancements
- New experiment protocols

## Conclusion

NLM represents a **substantial step towards biologically-inspired artificial intelligence**. While not a replacement for transformer-based systems in all applications, NLM demonstrates that **neural computation from first principles can produce learning, memory, and cognition** without requiring pre-existing knowledge or transformer architectures.

The project successfully implements **Phase 2-5**, creating a functioning artificial brain capable of:
- Learning through experience
- Storing and recalling episodic memories
- Forming abstract concepts
- Planning multi-step actions
- Adapting behavior through neuromodulation
- Developing over time with changing plasticity

**The primary goal of Phase 6 is integration** — making the existing systems work together as a coherent cognitive architecture that can learn, remember, and reason about the world through interaction, just like a biological brain.

NLM is not just neural networks; it's **an experimental artificial developmental brain** that investigates fundamental questions about how intelligence emerges from neural computation, experience, and development.
