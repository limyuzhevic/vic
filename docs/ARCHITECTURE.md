# =============================================================================
# NLM Project Structure Overview
# =============================================================================

## Project Architecture

The NLM (Neural Learning Machine) project follows a modular, layered architecture designed to implement a complete artificial brain system. The project is organized into several key subsystems that work together to create an autonomous learning agent.

## Directory Structure

```
NLM/
├── src/
│   ├── brain/              # Core neural computation (neurons, synapses, regions)
│   │   ├── Brain.cpp       # Central brain coordinator
│   │   ├── Brain.hpp       # Brain class interface
│   │   ├── Neuron.cpp      # Individual neuron implementation
│   │   ├── Neuron.hpp      # LIF neuron model
│   │   ├── Synapse.cpp     # Synaptic connections
│   │   └── Synapse.hpp     # Synapse model
│   │   └── ... (other neural components)
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules (STDP, Hebbian, structural)
│   ├── memory/             # Memory systems
│   ├── neuromodulation/     # Neuromodulators
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── cognition/          # Cognitive mechanisms
│   ├── prediction/         # Prediction systems
│   ├── development/        # Developmental system
│   ├── agent/              # Agent brain interface
│   ├── world/              # World simulation
│   └── visualization/       # Visualization interfaces
│   └── experiments/         # Experiment framework
│   └── performance/         # Performance optimization
├── python/                 # Python bindings
│   ├── bindings.cpp        # Original Python bindings
│   ├── bindings_enhanced.cpp # Enhanced Python bindings
│   └── CMakeLists.txt      # Python bindings build system
├── tests/                  # Unit tests
│   ├── test_*.cpp          # Individual test cases
│   └── test_main.cpp       # Test runner
├── configs/                # Configuration files
│   └── default.cfg         # Default configuration
├── docs/                   # Documentation
│   └── *.md                # Technical documentation
├── CMakeLists.txt          # Main build system
├── pyproject.toml         # Python project configuration
└── README.md               # Project overview
```

## Core Subsystems

### 1. Neural Computation Layer (brain/)
**Purpose:** Implement real spiking neural computation with event-driven dynamics

**Components:**
- **Neuron:** Leaky Integrate-and-Fire (LIF) model with realistic dynamics
- **Synapse:** Event-driven synaptic transmission with delays and plasticity
- **Neural Population:** Groups of homogeneous neurons
- **Neural Region:** Functional brain regions with specialized populations
- **Brain:** Central coordinator integrating all subsystems

**Key Features:**
- Real-time simulation with millisecond precision
- Event-driven spike propagation
- Multiple plasticity rules (STDP, Hebbian, structural)
- Neuromodulation effects on neural dynamics

### 2. Memory Systems Layer (memory/)
**Purpose:** Store and integrate with neural processing for learning and memory

**Components:**
- **Working Memory:** Short-term active information storage
- **Episodic Memory:** Experience recording and replay
- **Associative Memory:** Pattern associations and recall
- **Semantic Memory:** Abstract knowledge representation

**Integration:**
- Memory systems connected to neural processing
- Replay mechanisms during rest/sleep cycles
- Consolidation of important memories
- Attention-mediated memory access

### 3. Neuromodulation Layer (neuromodulation/)
**Purpose:** Provide motivational and regulatory signals for behavior

**Components:**
- **Dopamine:** Reward prediction error and reinforcement
- **Curiosity:** Exploration motivation
- **Novelty:** Novelty detection and surprise
- **Prediction Error:** Discrepancy between expected and actual

**Functions:**
- Modulate neural excitability
- Drive exploratory behavior
- Gate plasticity and learning
- Influence decision making

### 4. Cognitive Layer (cognition/)
**Purpose:** Implement higher-level cognitive functions emerging from neural dynamics

**Components:**
- **Neural Planner:** Action sequence evaluation and selection
- **Concept Formation:** Abstract pattern discovery
- **Attentional Selection:** Competitive selection among alternatives

**Emergent Properties:**
- Planning and goal-directed behavior
- Abstraction and generalization
- Attention and focus

### 5. Prediction Layer (prediction/)
**Purpose:** Internal models of the world and prediction error signals

**Components:**
- **Prediction System:** Forward models and prediction errors
- **Neural Prediction:** Predictive coding in neural populations

**Functions:**
- Generate predictions about sensory input
- Compute prediction errors
- Update internal models
- Guide learning and behavior

### 6. Development Layer (development/)
**Purpose:** Implement developmental processes and maturation

**Components:**
- **Development System:** Stage-based development
- **Synaptogenesis:** Formation of new connections
- **Pruning:** Elimination of unused connections
- **Maturation:** Tuning of neural parameters

**Key Features:**
- Critical periods for learning
- Plasticity modulation by age
- Structural changes over time
- Adaptive behavior with development

### 7. Sensory/Motor Layer
**Purpose:** Interface with external environment

**Components:**
- **Sensory Input:** External and internal state sensing
- **Motor Output:** Action generation and execution
- **World Model:** Internal representation of environment

### 8. Agent Layer (agent/)
**Purpose:** Connect brain to world with behavioral control

**Components:**
- **Agent Brain:** Interface between brain and world
- **Sensory Percept:** What the agent perceives
- **Agent Body:** Physical embodiment state

## Brain Loop Architecture

The NLM brain operates as a closed-loop system following the perception-action-feedback loop:

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

## Simulation Process

Each simulation step involves:

1. **Process pending delayed spike events** (deliver synaptic input)
2. **Update all neurons** (LIF dynamics)
3. **Detect spikes and schedule spike events**
4. **Update working memory**
5. **Apply neuromodulation effects**
6. **Apply plasticity rules**
7. **Update episodic memory**
8. **Update prediction system**
9. **Update attention system**
10. **Update concept formation**
11. **Apply structural plasticity**
12. **Replay important memories**
13. **Apply development effects**
14. **Collect statistics**

## Phase-Based Development

The project follows a phase-based development approach:

- **Phase 1:** Project skeleton and core types
- **Phase 2:** Real neural computation (LIF, STDP, Hebbian)
- **Phase 3:** World interaction loop
- **Phase 4:** Advanced cognitive and memory systems
- **Phase 5:** Performance optimizations and scaling
- **Phase 6:** Final integration (current status)

## Key Design Principles

### 1. Biological Plausibility
- Based on computational neuroscience models
- Uses LIF neuron dynamics
- Implements STDP and other biological learning rules
- Neuromodulation inspired by brain systems

### 2. Modularity and Separation of Concerns
- Each subsystem has clear boundaries
- Interfaces defined through abstract base classes
- Implementation details hidden behind headers
- Easy to extend and modify

### 3. Event-Driven Architecture
- Spike events processed asynchronously
- Delayed synaptic transmission
- Asynchronous updates for efficiency

### 4. Realistic Dynamics
- Non-linear neural dynamics
- Stochastic elements for variability
- Context-dependent behavior
- Emergent properties from interactions

### 5. Integration Over Specialization
- All systems work together
- Cross-system feedback loops
- Shared resources and state
- Global coordination through Brain class

## Technical Specifications

### Neural Properties
- **Neuron Count:** Configurable (default: 1000)
- **Region Count:** Configurable (default: 1)
- **Connection Probability:** Configurable (default: 0.1)
- **Time Step:** 1ms (configurable)
- **Neuron Types:** Sensory, Motor, Internal, Excitatory, Inhibitory, Modulatory

### Plasticity Parameters
- **STDP LTP Weight:** 0.01
- **STDP LTD Weight:** 0.012
- **STDP Time Constant:** 20ms
- **Synaptogenesis Rate:** 0.0001
- **Pruning Rate:** 0.00001

### Memory Systems
- **Working Memory Capacity:** neuron_count/10
- **Episodic Memory:** 1000 episodes max
- **Replay Interval:** 100 steps
- **Consolidation Interval:** 1000 steps

### Neuromodulation
- **Dopamine Range:** -1.0 to 1.0
- **Curiosity:** 0.0 to 1.0
- **Novelty:** 0.0 to 1.0
- **Prediction Error:** -1.0 to 1.0

## Build Configuration

### Build Options
```bash
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

### Python Module Structure
```
Python Modules:
├── pynlm_original    # Original comprehensive bindings
└── pynlm_enhanced    # Enhanced Pythonic bindings
```

### Key Python Classes
- **Config:** Configuration management
- **Brain:** Neural simulation engine
- **SimpleWorld:** Environment simulation
- **AgentBrain:** Agent interface
- **SensoryPercept:** Sensory data
- **Vision, Audio, InternalSignals:** Specific sensory types
- **Action:** Motor output
- **WorldObject:** Environmental objects
- **AgentBody:** Agent physical state
```
