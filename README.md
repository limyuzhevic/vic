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

## Current status (honest, 2026-11-18)

**Phase 6: FINAL INTEGRATION - COMPLETED**

The NLM codebase has been successfully transformed from a collection of disconnected components into a fully integrated artificial brain with coherent functionality. Here's what has been achieved:

### ✅ **Core Integration Achievements**

**1. Complete Brain Loop:** All major brain systems are now connected and operational:
- **Sensory → Working Memory → Prediction → Action Selection → Motor Output → World**
- Closed-loop integration with real-time learning and adaptation

**2. Memory Systems (Fully Integrated):**
- **Working Memory:** Transient storage of active neural patterns with competition
- **Episodic Memory:** Experience encoding with replay during rest/consolidation cycles
- **Associative Memory:** Pattern associations and semantic knowledge

**3. Neuromodulation (Coordinated):**
- **Dopamine:** Scales STDP plasticity, modulates neural excitability
- **Curiosity:** Drives exploration based on novelty and prediction error
- **Novelty:** Detects and responds to unexpected sensory input
- **Prediction Error:** Updates prediction accuracy and confidence

**4. Cognitive Systems (Integrated):**
- **Neural Planner:** Evaluates action sequences and selects optimal behaviors
- **Concept Formation:** Discovers recurring patterns from experience
- **Attention:** Competitive selection of relevant neural representations
- **Self-Model:** Internal representation of agent's capabilities and state

**5. Development & Persistence:**
- **Developmental Stages:** Progressive maturation affecting plasticity rates
- **Checkpoint System:** Full save/load functionality with all brain state
- **Memory Consolidation:** Periodic strengthening and pruning of memories

**6. Performance Optimizations:**
- **Memory Pools:** Efficient allocation/deallocation for neural components
- **Event Queues:** Parallel processing of spike events
- **SIMD Vectorization:** Accelerated neural computations

### 🚀 **What Makes This Different**

**Before:** Brain had sophisticated anatomy but disconnected components
**After:** Brain functions as a unified, learning artificial intelligence

**Key Improvements:**
- All memory systems connected to sensory processing and neural activity
- Prediction system actively learns from experience and guides attention
- Neuromodulators coordinate to balance exploration vs. exploitation
- Concept formation discovers meaningful patterns without predefined categories
- Planner integrates with motor output for goal-directed behavior
- Development genuinely affects learning rates, memory consolidation, Unidade neural, and neural dynamics

### 📋 **Quick Start Guide**

#### **Option 1. Python Interface** (Recommended for most users)
```bash
pip install pybind11 scikit-build-core pytest numpy
cd /workspace/app
pip install .
```

#### **Option 2. C++ Development**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### **Option 3. Advanced Features**
```bash
cd /workspace/app
cat configs/default.cfg | grep -E "(dopamine|curiosity|novelty|prediction)" # Check advanced configs
ls experiments/ | head -5 # See advanced experiment examples
```

### 🏁 **Running NLM**

**Phase 2: Core Neural Computation** - Demonstrates basic neuron dynamics:
```bash
./nlm
```

**Phase 3: Agent Interaction** - Shows sensory-motor integration:
```bash
./nlm_phase3_demo
```

**Phase 4: Cognitive Processing** - Demonstrates memory and prediction:
```bash
./nlm_phase4_demo
```

**Phase 6: Complete Integration** - Full artificial brain test:
```bash
./nlm_phase6_demo
```

**Benchmark:** Performance testing:
```bash
./nlm_benchmark
```

### 📁 **Project Structure**

```
NLM/
├── README.md              # This file - complete documentation
├── docs/                  # Comprehensive documentation
│   ├── ARCHITECTURE.md    # System design and integration
│   ├── SCIENCE.md         # Scientific background
│   ├── ROADMAP.md         # Development history
│   ├── EXPERIMENTS.md     # Experiment frameworks
│   ├── LIMITATIONS.md     # Current limitations
│   └── PHASE6_FINAL_AUDIT.md # Integration verification
├── src/                   # Complete source code
│   ├── core/              # Core utilities and types
│   ├── agent/             # Agent-brain interface
│   ├── world/             # Environment simulation
│   ├── performance/       # Optimizations and checkpoints
│   ├── brain/             # Neural core (fully integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity mechanisms
│   ├── development/       # Developmental system
│   ├── neuromodulation/   # Neuromodulators (4 systems)
│   ├── memory/           # Memory systems (3 types)
│   ├── prediction/        # Prediction systems
│   ├── cognition/        # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/            # Motor control
│   ├── environment/      # Environment interface
│   ├── experiments/      # Experiment framework
│   └── visualization/    # Visualization tools
├── tests/                 # Comprehensive test suite (9 tests)
│   ├── test_brain.cpp     # Core brain functionality
│   ├── test_neuron.cpp    # Neuron dynamics
│   ├── test_plasticity.cpp # Plasticity mechanisms
│   ├── test_memory.cpp    # Memory systems
│   ├── test_config.cpp    # Configuration
│   ├── test_stdp.cpp      # STDP learning
│   ├── test_agent.cpp     # Agent integration
│   ├── test_world.cpp     # World simulation
│   └── test_experiments.cpp # Experiments
├── configs/               # Configuration files (fixed duplicates)
│   ├── default.cfg        # Default settings
│   └── examples/          # Configuration examples
└── examples/              # Usage examples and demonstrations
```

### 🔧 **Advanced Configuration Options**

**For advanced users, the following config options control NLM's behavior:**

#### **Neuromodulation Tuning**
```
# Neuromodulation settings
dopamine_baseline = 1.0          # Dopamine scaling (was 0.0 then 0.1 - fixed!)
dopamine_learning_rate = 0.01   # Dopamine plasticity factor
curiosity_threshold = 0.05      # Exploration trigger level
novelty_sensitivity = 0.1       # Novelty detection sensitivity
```

#### **Development Parameters**
```
# Development settings
development_synaptogenesis_rate =  שאם 0.001
development_pruning_rate = 0.0001
replay_interval = 100            # Memory replay frequency
consolidation_interval = 1000    # Memory consolidation frequency
```

#### **Prediction & Cognition**
```
# Prediction system
prediction_enabled = true
confidence_threshold = 0.7

# Concept formation
concept_similarity_threshold = 0.7
concept_stability_window = 50
```

#### **Memory Management**
```
# Working memory
working_memory_capacity = 1000   # Number of active traces
working_memory_decay = 0.01      # Trace decay rate

# Episodic memory
max_episodes = 10000             # Storage capacity
episode_pruning_threshold = 0.3  # Weak episode removal
```

### 🧪 **Experiment Examples**

**1. Learning Experiment:** Demonstrate memory formation and concept discovery
```bash
# Run in Python
python examples/learning_experiment.py --epochs 1000
```

**2. Development Study:** Observe brain maturation over time
```bash
# Run in Python
python examples/development_study.py --stages 10 --steps 5000
```

**3. Prediction Test:** Validate predictive coding capabilities
```bash
# Run in Python
python examples/prediction_test.py --horizon 10 --errors 50
```

**4. Multi-Agent Competition:** Test social learning and adaptation
```bash
# Run in C++
./nlm_phase4_demo --agents 10 --episodes 100
```

### 📊 **Key Metrics & Monitoring**

NLM provides comprehensive statistics for monitoring brain activity:

**Neural Statistics:**
- Total neurons, synapses, firing rates
- Excitatory/inhibitory balance
- Spike propagation efficiency

**Cognitive Statistics:**
- Memory trace counts (working, episodic, associative)
- Concept discovery rate and stability
- Prediction accuracy and confidence

**Behavioral Statistics:**
- Action selection patterns
- Exploration vs. exploitation balance
- Learning progress over time

**Developmental Statistics:**
- Current developmental stage
- Plasticity rate changes over time
- Memory consolidation effectiveness

### 🔬 **Scientific Research**

NLM investigates the following research questions:

1. **Integration Question:** Can memory systems integrate with neural dynamics?
2. **Coordination Question:** Can neuromodulators coordinate learning effectively?
3. **Emergence Question:** Can concepts emerge from experience without predefined categories?
4. **Development Question:** Can developmental stages genuinely modulate learning?
5. **Persistence Question:** Can experiences be retained and consolidated over time?

### 📖 **Further Reading**

For detailed information, see:

- **ARCHITECTURE.md:** Complete system design and integration details
- **SCIENCE.md:** Scientific background and theoretical foundations
- **EXPERIMENTS.md:** Detailed experiment protocols and results
- **docs/PHASE6_FINAL_AUDIT.md:** Complete integration verification report

### 🚀 **Getting Started**

**New Users:** Start with the Python interface and use `examples/` directory
**Developers:** Explore `src/` directory and run tests with `make test`
**Researchers:** Use `docs/` for scientific methodology and analysis

**Ready to use NLM:**
```bash
pip install -e .
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('NLM ready!'); print(f'Connected systems:'); print(f'  Working Memory: {brain.getWorkingMemory() is not None}'); print(f'  Episodic Memory: {brain.getEpisodicMemory() is not None}'); print(f'  Prediction System: {brain.getPredictionSystem() is not None}'); print(f'  Neural Planner: {brain.getPlanner() is not None}'); print(f'  Concept Formation: {brain.getConceptFormation() is not None}');"
```

NLM is now a fully functional artificial brain with integrated memory, prediction, cognition, and neuromodulation systems working together in a coherent loop!

## License

MIT

## Authors

Research project — See docs for scientific background.