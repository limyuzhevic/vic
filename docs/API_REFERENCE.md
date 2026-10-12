# NLM API Reference

## Core Neural System API

This document provides detailed API documentation for the NLM (熙然) neural system.

## Configuration API

### Config Class

The Config class provides a key-value configuration system with support for multiple value types and sources.

**Files:** src/core/Config/Config.hpp, src/core/Config/Config.cpp

#### Key Methods:

- **`bool loadFromFile(const std::string& filepath)`**
  - Loads configuration from a JSON file
  - Returns true if successful, false on error

- **`bool loadFromArgs(int argc, char** argv)`**
  - Parses command line arguments and applies configuration
  - Returns true if successful, false on error

- **`template<typename T> std::optional<T> get(const std::string& key) const`**
  - Gets a configuration value of type T
  - Returns nullopt if key doesn't exist or type mismatch

- **`void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)`**
  - Sets a configuration value with specified source

#### Configuration Types:

- **Basic types:** int, int64_t, double, bool, string
- **Collections:** vector<int>, vector<double>, vector<string>

#### Config Sources:

- **Default:** Built-in defaults
- **File:** Loaded from JSON configuration file
- **CommandLine:** Parsed from command line arguments
- **Runtime:** Set programmatically during execution

## Brain Core API

### Brain Class

The Brain class is the central coordinator of the neural system, managing all neural components and their interactions.

**Files:** src/brain/Brain.hpp, src/brain/Brain.cpp

#### Key Methods:

- **`bool initialize()`**
  - Initializes the brain with configured components
  - Returns true if successful, false on initialization error

- **`void step(SimulationStep currentStep)`**
  - Executes one simulation step of the brain
  - Updates neural dynamics, plasticity, and memory systems

- **`void step(SimulationStep currentStep, Timestamp currentTime)`**
  - Executes one simulation step with explicit time
  - Alternative stepping method with time information

- **`void receiveSensoryInput(const SensoryInput& input)`**
  - Processes sensory input from the environment
  - Injects current into sensory neurons

- **`void injectCurrent(NeuronId neuron, MembranePotential current)`**
  - Injects direct current into a specific neuron

- **`void injectCurrentToNeurons(NeuronType type, MembranePotential current)`**
  - Injects current into all neurons of a specific type

#### Memory System Access:

- **`NeuralWorkingMemory* getWorkingMemory()`**
  - Accesses working memory system for temporary information storage

- **`NeuralEpisodicMemory* getEpisodicMemory()`**
  - Accesses episodic memory system for experience storage

- **`NeuralAssociativeMemory* getAssociativeMemory()`**
  - Accesses associative memory system for pattern associations

#### Plasticity System Access:

- **`STDP* getSTDP()`**
  - Accesses Spike-Timing-Dependent Plasticity system

- **`Hebbian* getHebbian()`**
  - Accesses Hebbian learning rule system

- **`StructuralPlasticity* getStructuralPlasticity()`**
  - Accesss structural plasticity for synapse formation/removal

## Neural Dynamics API

### IntegrateAndFireDynamics Class

Implements the Leaky Integrate-and-Fire (LIF) neuron model, the fundamental neural dynamics in NLM.

**Files:** src/dynamics/NeuralDynamics.hpp, src/dynamics/NeuralDynamics.cpp

#### Key Methods:

- **`void updateNeuron(Neuron* neuron, TimestepDuration dt)`**
  - Updates neuron membrane potential based on LIF equations
  - Handles spike generation when threshold is reached

- **`void applySpikeInput(Neuron* neuron, const Synapse* synapse)`**
  - Applies postsynaptic potential from spike input
  - Updates membrane potential based on synaptic conductance

- **`bool shouldFire(const Neuron* neuron) const`**
  - Checks if neuron has reached firing threshold
  - Returns true if neuron should fire in current step

- **`void setMembraneTimeConstant(float tau)`**
  - Sets the membrane time constant (τ) for LIF equation
  - Controls membrane potential decay rate

#### LIF Equation:

```
dV/dt = (-V + I_syn + I_ext) / τ
```

Where:
- V is membrane potential
- I_syn is synaptic current
- I_ext is external injected current
- τ is membrane time constant

## Plasticity Rules API

### STDP Class

Implements Spike-Timing-Dependent Plasticity, a Hebbian learning rule where synaptic strength depends on precise spike timing.

**Files:** src/plasticity/STDP.hpp, src/plasticity/STDP.cpp

#### Key Mechanisms:

- **Pre-before-Post LTP:** When presynaptic spike precedes postsynaptic spike, strengthen synapse
- **Post-before-Pre LTD:** When postsynaptic spike precedes presynaptic spike, weaken synapse
- **Temporal Window:** Learning decays with spike time difference

### Hebbian Class

Implements standard Hebbian learning, strengthening synapses that are co-activated.

**Files:** src/plasticity/Hebbian.hpp, src/plasticity/Hebbian.cpp

#### Key Mechanisms:

- **Hebbian Learning Rule:** Cooperating neurons strengthen synaptic connections
- ** Reward-Modulated Hebbian:** Learning modulated by reward prediction error

## Memory Systems API

### NeuralWorkingMemory Class

Implements working memory using persistent neural activity and recurrent connections, rather than traditional vector storage.

**Files:** src/memory/NeuralWorkingMemory.hpp, src/memory/NeuralWorkingMemory.cpp

#### Key Methods:

- **`void store(const std::vector<float>& pattern, float strength = 1.0f)`**
  - Stores information as neural activity pattern
  - Encodes information through persistent firing

- **`void storeToNeuron(NeuronId neuron, float activation)`**
  - Directly stores activation to specific neuron

- **`std::vector<float> retrieve() const`**
  - Retrieves current working memory content as activity levels

- **`void update(TimestepDuration dt)`**
  - Maintains memory traces with decay
  - Updates recurrent connections for maintenance

## Motor System API

### MotorSystem Class

Converts neural activity into actions through competitive selection and pattern analysis.

**Files:** src/motor/MotorSystem.hpp, src/motor/MotorSystem.cpp

#### Key Methods:

- **`std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion)`**
  - Selects action based on neural population activity
  - Implements competitive selection with confidence scoring

- **`std::vector<ActionScore> calculateActionScores(const NeuralRegion& motorRegion) const`**
  - Computes action scores based on neural firing patterns
  - Includes strength, confidence, coherence, and preference factors

#### ActionScore Structure:

```cpp
struct ActionScore {
    ActionType type;           // Action type
    float strength;           // Overall action strength
    float confidence;         // Confidence in selection
    float coherence;          // Neural coherence supporting action
    float preference;         // Learned preference (exploration bias)
};
```

## Sensory Processing API

### VisionProcessor Class

Processes visual input through feature extraction and neural representation.

**Files:** src/sensory/Vision.hpp, src/sensory/Vision.cpp

### AudioProcessor Class

Processes audio input through frequency analysis and neural representation.

**Files:** src/sensory/Audio.hpp, src/sensory/Audio.cpp

## Neuron API

### Neuron Class

Represents individual neurons implementing Leaky Integrate-and-Fire dynamics.

**Files:** src/brain/Neuron.hpp, src/brain/Neuron.cpp

#### Key Methods:

- **`bool stepLIF(Timestamp currentTime, TimestepDuration dt)`**
  - Executes one LIF dynamics step
  - Returns true if neuron fired

- **`bool checkThreshold() const`**
  - Checks if membrane potential exceeds firing threshold

- **`void recordSpike(Timestamp timestamp)`**
  - Records spike for STDP learning

- **`void initializeRandom(RandomGenerator& rng)`**
  - Initializes neuron parameters with random values

## Type Definitions

### Core Types:

- **NeuronId:** Unique identifier for neurons
- **SynapseId:** Unique identifier for synapses
- **RegionId:** Unique identifier for brain regions
- **PopulationId:** Unique identifier for neuron populations
- **MembranePotential:** Neuron membrane voltage (mV)
- **FiringRate:** Neural firing rate (Hz)
- **Timestamp:** Time in simulation steps
- **TimestepDuration:** Duration of simulation step

### Enumerations:

- **NeuronType:** Different types of neurons
- **FiringState:** Current firing state of neuron
- **LogLevel:** Logging verbosity levels

## Error Handling

NLM provides comprehensive error handling through:

- **Error codes** in various system headers
- **Assertion macros** for debugging
- **Logger class** for structured logging
- **Graceful degradation** when components fail to initialize

## Performance Characteristics

### Memory Requirements:

- **Neuron:** ~128 bytes per neuron (state + connections)
- **Synapse:** ~64 bytes per synapse
- **Working Memory:** O(capacity) for stored patterns

### Computational Complexity:

- **Neuron Update:** O(1) per neuron per timestep
- **Spike Processing:** O(incoming_synapses) per spike
- **STDP:** O(active_synapses) per timestep
- **Action Selection:** O(actions * neurons) per step

### Scalability:

- **Memory Pools:** Pre-allocated for real-time performance
- **Event-driven Processing:** Only processes active components
- **Parallel Processing:** Available in Phase 5+
- **SIMD Vectorization:** Used in neuron population operations

## Integration Guidelines

### Component Dependencies:

1. **Initialize Config** before creating Brain
2. **Create Brain** with configuration
3. **Initialize Brain** before using other components
4. **Use Brain** as primary interface for most operations
5. **Access subsystems** through Brain interface when possible

### Common Workflows:

1. **Basic Simulation:**
   ```cpp
   auto config = std::make_shared<Config>();
   config->loadFromFile("config.json");
   
   auto brain = std::make_shared<Brain>(config);
   brain->initialize();
   
   for (SimulationStep step = 0; step < maxSteps; ++step) {
       brain->step(step);
   }
   ```

2. **Sensory-Motor Loop:**
   ```cpp
   // Process environment percept
   brain->receiveSensoryInput(percept);
   
   // Let brain process sensory input
   brain->step(currentStep);
   
   // Get motor output
   auto action = brain->produceAction();
   environment->applyAction(action.get());
   ```

3. **Memory-Centric Processing:**
   ```cpp
   auto workingMemory = brain->getWorkingMemory();
   workingMemory->store(pattern, strength);
   
   // Later...
   auto retrieved = workingMemory->retrieve();
   ```

## Best Practices

1. **Thread Safety:** Use Brain as single-threaded coordinator
2. **Memory Management:** Prefer smart pointers for component ownership
3. **Error Handling:** Check return values for operations that can fail
4. **Performance:** Use Brain interface for most operations to leverage optimizations
5. **Extensibility:** Follow existing patterns when adding new components

## References

- [NLM Documentation](https://nlm.ai)
- [Phase 1-6 Reports](docs/PHASE6_FINAL_AUDIT.md)
- [Research Papers](docs/SCIENCE.md)
