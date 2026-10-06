# NLM - Neural Learning Machine

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Features

- **Real Spiking Neural Computation**: Implements Leaky Integrate-and-Fire (LIF) neuron dynamics
- **Multi-Modal Learning**: STDP, Hebbian, and reward-modulated plasticity
- **Integrated Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Cognitive Architecture**: Neural planning, concept formation, attention
- **Developmental System**: Age-dependent plasticity and maturation
- **Event-Driven Dynamics**: Spike propagation with delays
- **Structural Plasticity**: Synaptogenesis and pruning

## Core Components

### Brain Architecture
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

### Memory Systems

#### Working Memory
- **Function**: Transient active information storage
- **Mechanism**: Persistent neural activity with competitive dynamics
- **Integration**: Connected to neural processing and attention systems

#### Episodic Memory
- **Function**: Experience encoding and replay
- **Mechanism**: Spike-timing-dependent consolidation
- **Features**: Memory consolidation during rest/sleep cycles

#### Associative Memory
- **Function**: Pattern associations and recall
- **Mechanism**: Hebbian learning and pattern completion
- **Applications**: Concept formation and prediction

### Neuromodulation Systems

#### Dopamine
- **Role**: Reward prediction error and reinforcement learning
- **Function**: Modulates plasticity and neural excitability
- **Integration**: Connected to reward prediction and eligibility traces

#### Curiosity
- **Role**: Exploration motivation and intrinsic drive
- **Function**: Increases exploration based on novelty and prediction error
- **Behavior**: Balances exploitation vs. exploration

#### Novelty Detection
- **Role**: Detects unexpected sensory input
- **Function**: Triggers curiosity and learning
- **Integration**: Connected to sensory processing and neuromodulation

### Prediction System

#### Neural Prediction
- **Function**: Forward models and expectation generation
- **Mechanism**: Predictive coding and error minimization
- **Applications**: Action planning and decision making

#### Prediction Error
- **Role**: Signals prediction mismatches
- **Function**: Drives learning and adaptation
- **Integration**: Connected to neuromodulation and plasticity

### Cognitive Systems

#### Neural Planner
- **Function**: Action sequence evaluation and planning
- **Mechanism**: Hierarchical neural computation
- **Features**: Temporal abstraction and goal-directed behavior

#### Concept Formation
- **Function**: Pattern discovery from experience
- **Mechanism**: Associative memory and clustering
- **Applications**: Generalization and abstraction

#### Attentional Selection
- **Function**: Competitive neural selection
- **Mechanism**: Winner-take-all dynamics
- **Applications**: Focus and resource allocation

### Development System

#### Developmental Stages
- **Initial**: High plasticity, rapid learning
- **Critical Period**: Specialized learning windows
- **Maturation**: Reduced plasticity, stable behavior
- **Adult**: Fine-tuned, specialized responses

#### Synaptogenesis and Pruning
- **Mechanism**: Structural plasticity based on activity
- **Regulation**: Age-dependent rates and rules
- **Outcome**: Optimized network architecture

## API Documentation

### Core Types

#### Neuron States
- **FiringState**: Neuron activity state (Resting, Active, Refractory)
- **MembranePotential**: Membrane voltage (mV)
- **FiringRate**: Neural firing frequency (Hz)

#### Synapse Properties
- **SynapseType**: Connection type (Excitatory, Inhibitory, Modulatory)
- **PlasticityFlags**: Enable/disable plasticity rules
- **EligibilityTrace**: Reward-modulated learning signal

### Key Classes

#### nlm::Neuron
```cpp
class Neuron {
public:
    // Creation and Identity
    Neuron(NeuronId id);
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State Access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane Dynamics
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    void injectCurrent(MembranePotential current);
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Memory
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Reset and Initialize
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

#### nlm::Synapse
```cpp
class Synapse {
public:
    // Creation and Identity
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight Management
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Type and Delay
    SynapseType getType() const;
    void setType(SynapseType type);
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Spike Activity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Short-term Plasticity
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Step and Reset
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

#### nlm::Brain
```cpp
class Brain {
public:
    // Creation and Initialization
    explicit Brain(std::shared_ptr<Config> config);
    bool initialize();
    
    // Simulation
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Input and Output
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    
    // Plasticity
    void updatePlasticity();
    
    // Memory Access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    float getAverageFiringRate() const;
    
    // Development
    void develop();
    DevelopmentalStage getDevelopmentalStage() const;
};
```

#### nlm::Config
```cpp
class Config {
public:
    // Value Access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Value Setting
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Command Line
    bool loadFromArgs(int argc, char** argv);
};
```

### Configuration Options

#### Neural Parameters
- `neuron_count`: Total number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 2)
- `connection_probability`: Random connectivity probability (default: 0.1)

#### Plasticity Settings
- `plasticity_learning_rate`: General learning rate (default: 0.01)
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.012)
- `stdp_time_constant`: STDP time constant (default: 20.0)

#### Development Settings
- `development_synaptogenesis_rate`: Synaptogenesis rate (default: 0.001)
- `development_pruning_rate`: Pruning rate (default: 0.0001)

#### Neuromodulation Settings
- `dopamine_baseline`: Baseline dopamine level (default: 0.1)
- `curiosity_threshold`: Curiosity activation threshold (default: 0.3)
- `novelty_decay`: Novelty signal decay rate (default: 0.99)

#### Reward and Learning
- `reward_discount_factor`: Reward discount factor (default: 0.99)
- `prediction_error_gain`: Prediction error sensitivity (default: 1.0)

#### Simulation Control
- `max_simulation_steps`: Maximum simulation steps (default: 10000)
- `simulation_time_limit`: Time limit in seconds (default: 0.0, unlimited)
- `log_level`: Logging level (DEBUG, INFO, WARNING, ERROR)

### Usage Examples

#### Basic Brain Initialization
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    
    // Initialize brain
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    return 0;
}
```

#### Neuron Creation and Stimulation
```cpp
#include "brain/Neuron.hpp"

int main() {
    // Create a neuron
    Neuron neuron(NeuronId(1));
    neuron.setType(NeuronType::Excitatory);
    
    // Initialize with random parameters
    RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Inject current and simulate
    neuron.injectCurrent(50.0f);
    bool fired = neuron.stepLIF(0.0, 0.001);
    
    if (fired) {
        std::cout << "Neuron fired! Last spike time: " 
                  << neuron.getLastSpikeTime() << std::endl;
    }
    
    return 0;
}
```

#### Synapse Plasticity
```cpp
#include "brain/Synapse.hpp"

int main() {
    // Create a synapse
    Synapse synapse(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse.setType(SynapseType::Excitatory);
    
    // Enable STDP plasticity
    synapse.enablePlasticity(false, true, false);
    
    // Simulate pre-post spike timing for LTP
    synapse.recordPreSpike(0.0);    // Pre-synaptic spike
    synapse.recordPostSpike(0.1);   // Post-synaptic spike after 0.1ms
    
    // Step synapse (applies STDP)
    synapse.step(0.1);
    
    std::cout << "Synaptic weight after STDP: " << synapse.getWeight() << std::endl;
    
    return 0;
}
```

#### Complete Simulation Loop
```cpp
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Environment.hpp"

int main() {
    // Initialize configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("max_simulation_steps", 10000);
    
    // Create brain
    Brain brain(config);
    if (!brain.initialize()) {
        return 1;
    }
    
    // Create environment
    Environment environment;
    
    // Simulation loop
    for (SimulationStep step = 0; step < config->getOr("max_simulation_steps", 10000); ++step) {
        double time = step * config->getOr("simulation_timestep", 0.001);
        
        // Get sensory input
        auto sensoryInput = environment.getSensoryInput();
        
        // Receive input and step brain
        brain.receiveSensoryInput(sensoryInput);
        brain.step(step, time);
        
        // Produce action
        auto action = brain.produceAction();
        if (action) {
            // Execute action in environment
            environment.executeAction(*action);
        }
        
        // Log progress occasionally
        if (step % 1000 == 0) {
            std::cout << "Step " << step << ", Time: " << time 
                      << "s, Neurons: " << brain.getTotalNeuronCount()
                      << ", Synapses: " << brain.getTotalSynapseCount()
                      << ", Firing: " << brain.getFiringNeuronCount()
                      << std::endl;
        }
    }
    
    return 0;
}
```

#### Configuration File Format

The NLM configuration system supports multiple file formats:

##### Key-Value Format (`.cfg`)
```cfg
# NLM Configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15
plasticity_learning_rate = 0.01

# Plasticity Parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation
dopamine_baseline = 0.1
curiosity_threshold = 0.3
novelty_decay = 0.99

# Learning
reward_discount_factor = 0.99
prediction_error_gain = 1.0

# Simulation
max_simulation_steps = 10000
log_level = INFO
```

##### Command Line Arguments
```bash
./nlm_demo \
    --neuron_count=500 \
    --region_count=2 \
    --connection_probability=0.15 \
    --plasticity_learning_rate=0.01 \
    --stdp_ltp_weight=0.02 \
    --stdp_ltd_weight=0.015 \
    --max_simulation_steps=5000
```

## Performance Considerations

### Memory Usage
- Each neuron requires approximately 200 bytes of memory
- Each synapse requires approximately 100 bytes of memory
- Total memory ~ (neuron_count + synapse_count) * ~300 bytes

### Computational Complexity
- **Neuron Update**: O(1) per neuron per timestep
- **Synapse Update**: O(1) per synapse per timestep (event-driven)
- **Plasticity**: O(1) per plastic synapse per event
- **Memory Systems**: O(log n) for lookup operations

### Recommended Settings
- **Small Scale**: neuron_count < 1000, connection_probability < 0.1
- **Medium Scale**: neuron_count < 10000, connection_probability < 0.05
- **Large Scale**: neuron_count < 100000, connection_probability < 0.01

## Troubleshooting

### Common Issues

#### Brain Initialization Fails
```
Check that configuration parameters are valid:
- neuron_count > 0
- region_count > 0
- connection_probability in [0, 1]
```

#### Plasticity Not Working
```
Ensure plasticity flags are enabled:
brain->getSTDP()->enable(true);
brain->getHebbian()->enable(true);
```

#### Neurons Not Firing
```
Check neuron parameters:
- Threshold < resting potential
- Sufficient current injection
- Not in refractory period
```

#### Memory Integration Fails
```
Check memory system connections:
brain->getWorkingMemory()->connect(brain);
brain->getEpisodicMemory()->connect(brain);
```

### Debugging Tips

#### Enable Verbose Logging
```cpp
Logger::getInstance().setLevel(Logger::Level::Debug);
```

#### Monitor Neural Activity
```cpp
brain->logStatus();  // Print detailed brain statistics
```

#### Save and Load Brain State
```cpp
brain->save("brain_checkpoint.nlm");  // Save current state
brain->load("brain_checkpoint.nlm");  // Restore from checkpoint
```

## Future Directions

### Planned Features
1. **Advanced Plasticity Mechanisms**
   - Homeostatic plasticity
   - Reward-modulated plasticity
   - Anti-Hebbian learning

2. **Enhanced Cognitive Functions**
   - Working memory capacity expansion
   - Theory of mind development
   - Abstract reasoning

3. **Improved Learning Algorithms**
   - Meta-learning
   - Transfer learning
   - Continual learning

4. **Advanced User Interface**
   - Visualization tools
   - Interactive debugging
   - Performance monitoring

### Research Opportunities
- **Neuromodulation**: Detailed biological modeling
- **Development**: Life-cycle learning algorithms
- **Memory**: Long-term consolidation mechanisms
- **Cognition**: Goal-directed behavior
- **Social Learning**: Cultural transmission

## License

MIT License - See LICENSE file for details.

## Authors

NLM Research Team - See docs for scientific background.
