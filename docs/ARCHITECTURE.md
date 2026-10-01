# NLM - Neural Learning Machine Comprehensive Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements an artificial developmental brain system. The project follows a phased approach with Phase 6 representing the final integration of all existing systems into a coherent artificial brain.

## Architecture

### Core Components

#### 1. Brain System
The central neural processing unit that coordinates all cognitive functions:

- **Neural Regions**: Hierarchical organization of brain regions
- **Neural Populations**: Groups of interconnected neurons
- **Spike System**: Event-driven spike propagation with delays
- **Plasticity Rules**: STDP, Hebbian learning, structural plasticity

#### 2. Memory Systems
Integrated memory capabilities:

- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations
- **Semantic Memory**: Abstract knowledge representation

#### 3. Neuromodulation
Biological motivation systems:

- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection and surprise
- **Prediction Error**: Learning signal for prediction accuracy

#### 4. Cognition Systems
Higher-level cognitive functions:

- **Neural Planner**: Action planning and sequence evaluation
- **Concept Formation**: Pattern discovery and abstraction
- **Attention**: Competitive selection dynamics

#### 5. Development System
Developmental maturation over time:

- **Developmental Stages**: Initial → CriticalPeriod → Maturation → Adult
- **Plasticity Modulation**: Age-dependent learning rates
- **Synaptogenesis**: New synapse formation
- **Pruning**: Synapse elimination

#### 6. Sensory-Motor Interface
World interaction:

- **AgentBrain**: Connects brain to world
- **SensoryPercept**: Transduction of sensory input
- **MotorSystem**: Action generation and execution

#### 7. Environment
Simulation world:

- **SimpleWorld**: 2D grid world with physics
- **Observation**: Perceptual representation of world state

### Integration Loop

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓                    ↓               ↓                    ↓
MEMORY/PREDICTION ← MOTIVATION/NEUROMODULATION ← ACTION SELECTION
    ↓                    ↓               ↓                    ↓
ACTION → MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR
    ↓                    ↓               ↓                    ↓
PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

## API Reference

### Core Brain Class

#### Constructor
```cpp
Brain(std::shared_ptr<Config> config)
```

#### Methods

##### Initialization
```cpp
bool initialize()
```
Initializes the brain with configuration parameters and creates all neural components.

##### Simulation
```cpp
void step(SimulationStep currentStep)
void step(SimulationStep currentStep, Timestamp currentTime)
```
Advances the simulation by one time step.

##### Input Processing
```cpp
void receiveSensoryInput(const SensoryInput& input)
void injectCurrent(NeuronId neuron, MembranePotential current)
void injectCurrentToNeurons(NeuronType type, MembranePotential current)
```
Injects external input into the brain.

##### Output Generation
```cpp
std::unique_ptr<Action> produceAction()
```
Generates a motor action based on current neural activity.

##### Neuromodulation
```cpp
void applyNeuromodulation(const Neuromodulator& signal)
```
Applies neuromodulatory signals to influence neural processing.

##### Plasticity
```cpp
void updatePlasticity()
```
Updates synaptic weights based on learning rules.

##### Development
```cpp
void develop()
```
Applies developmental changes to plasticity and structure.

##### State Management
```cpp
void reset()
bool save(const std::string& filepath)
bool load(const std::string& filepath)
```
Manages brain state persistence.

##### Information Access
```cpp
SpikeSystem* getSpikeSystem()
STDP* getSTDP()
Hebbian* getHebbian()
StructuralPlasticity* getStructuralPlasticity()
```
Access to plasticity systems.

```cpp
NeuralWorkingMemory* getWorkingMemory()
NeuralEpisodicMemory* getEpisodicMemory()
NeuralAssociativeMemory* getAssociativeMemory()
```
Access to memory systems.

```cpp
PredictionSystem* getPredictionSystem()
NeuralPlanner* getPlanner()
ConceptFormation* getConceptFormation()
AttentionalSelection* getAttention()
```
Access to cognition systems.

```cpp
Dopamine* getDopamine()
Curiosity* getCuriosity()
Novelty* getNovelty()
PredictionError* getPredictionErrorSignal()
```
Access to neuromodulation systems.

### Memory System Classes

#### NeuralWorkingMemory
Manages transient active information storage for immediate processing.

#### NeuralEpisodicMemory
Stores experiences as episodic memories for later recall and integration.

#### NeuralAssociativeMemory
Maintains pattern associations between concepts and experiences.

### Neuromodulation Classes

#### Dopamine
Implements reward prediction error signal based on received vs. predicted rewards.

#### Curiosity
Generates exploration motivation based on novelty detection and uncertainty.

#### Novelty
Detects novel sensory patterns and generates novelty signals.

#### PredictionError
Computes prediction accuracy signals for learning.

### Cognition Classes

#### NeuralPlanner
Implements planning through action sequence evaluation and reward prediction.

#### ConceptFormation
Discovers abstract patterns and forms concepts from experience.

#### AttentionalSelection
Implements competitive attention mechanisms for focus selection.

## Configuration

### Configuration Files

NLM uses configuration files (JSON/YAML format) to set up brain parameters:

```json
{
    "neuron_count": 1000,
    "region_count": 4,
    "connection_probability": 0.1,
    "timestep": 0.001,
    "development_enabled": true,
    "reward_modulation_enabled": true,
    "curiosity_enabled": true,
    "checkpoint_dir": "./checkpoints",
    "random_seed": 42
}
```

### Configuration Parameters

#### Neural Configuration
- `neuron_count`: Total number of neurons in the brain
- `region_count`: Number of neural regions
- `connection_probability`: Probability of synapse formation between neurons
- `timestep`: Simulation time step duration (seconds)

#### Development Configuration
- `development_enabled`: Whether developmental stages affect plasticity
- `plasticity_modifier`: Base plasticity factor

#### Neuromodulation Configuration
- `reward_modulation_enabled`: Whether reward signals affect learning
- `curiosity_enabled`: Whether curiosity drives exploration
- `novelty_threshold`: Threshold for novelty detection

#### I/O Configuration
- `vision_width`: Width of visual field
- `vision_height`: Height of visual field
- `world_width`, `world_height`: World dimensions
- `checkpoint_dir`: Directory for saving checkpoints

## Command Line Interface

### Basic Usage

```bash
# Run basic simulation
nlm --run --steps 1000

# Load and continue from checkpoint
nlm --load brain.checkpoint --steps 500

# Save current state
nlm --save brain.checkpoint

# Run with custom world size
nlm --run --world-size 20 20 --steps 2000

# Run with advanced features
nlm --run --enable-learning --enable-curiosity --enable-development config.cfg
```

### Command Line Options

#### Commands
- `--run, -r`: Run simulation
- `--load, -L <file>`: Load brain state
- `--save, -S <file>`: Save brain state
- `--config, -c <file>`: Load configuration file

#### Simulation Options
- `--steps N`: Number of simulation steps
- `--world-size W H`: Set world dimensions
- `--world-width W`: Set world width
- `--world-height H`: Set world height
- `--world-vision-width W`: Set vision width
- `--world-vision-height H`: Set vision height

#### Feature Options
- `--enable-learning`: Enable reward-based learning
- `--enable-curiosity`: Enable curiosity-based exploration
- `--enable-development`: Enable developmental stages

#### Output Options
- `--verbose, -v`: Enable verbose output
- `--quiet, -q`: Quiet mode (errors only)

### Output Format

The CLI provides detailed output during simulation:

```
Step: 100/1000
  Neurons: 100
  Spikes: 456
  Firing: 12
  Avg firing rate: 2.34
  Dopamine: 0.12
  Curiosity: 0.45
  Novelty: 0.23

=== FINAL STATISTICS ===
Total simulation time: 150 seconds
Total neurons: 100
Total synapses: 1234
Total spikes: 4567
Firing neurons: 12
Average firing rate: 2.34
E/I ratio: 1.23
```

## Advanced Usage

### Custom Configuration

Create custom configuration files for specific experimental setups:

#### Example: Social Learning Environment
```json
{
    "neuron_count": 500,
    "region_count": 3,
    "connection_probability": 0.15,
    "vision_width": 12,
    "vision_height": 12,
    "development_enabled": true,
    "reward_modulation_enabled": true,
    "social_learning_enabled": true,
    "novelty_threshold": 0.3
}
```

#### Example: High Plasticity Environment
```json
{
    "neuron_count": 2000,
    "region_count": 6,
    "connection_probability": 0.2,
    "plasticity_modifier": 2.0,
    "development_enabled": true,
    "critical_period": 100.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002
}
```

### Running Experiments

#### Script for Long-term Simulation
```bash
#!/bin/bash

# Create output directory
mkdir -p experiments/run_$(date +%Y%m%d_%H%M%S)

# Run simulation with monitoring
nlm --run \
    --steps 10000 \
    --world-size 15 15 \
    --enable-learning \
    --enable-curiosity \
    --enable-development \
    --checkpoint experiments/run_$(date +%Y%m%d_%H%M%S)/checkpoint \
    > experiments/run_$(date +%Y%m%d_%H%M%S)/output.log 2>&1

# Process output for analysis
python analyze_results.py experiments/run_$(date +%Y%m%d_%H%M%S)/output.log
```

#### Python API Usage
```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("development_enabled", True)
config.set("curiosity_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Get statistics
    print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Avg firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")

# Save results
brain.save("experiment_results.checkpoint")
```

### Performance Monitoring

The NLM provides several performance metrics:

#### CPU Usage
- CPU utilization during simulation
- Memory allocation and deallocation patterns
- Cache hit/miss ratios

#### Neural Performance
- Spike rate distributions
- Network synchronization patterns
- Learning curve analysis
- Memory utilization statistics

#### Simulation Performance
- Steps per second (SPS)
- Wall-clock time vs. simulated time
- Checkpoint I/O performance
- Recovery time from checkpoints

## Error Handling

### Common Errors

#### Configuration Errors
```
Error: Configuration file not found: config.json
Error: Invalid parameter: neuron_count (must be integer)
Error: Missing required parameter: connection_probability
```

#### Simulation Errors
```
Error: Failed to initialize brain
Error: Invalid timestep value
Error: Memory allocation failed
```

#### Checkpoint Errors
```
Warning: Failed to load checkpoint: File not found
Warning: Checkpoint version incompatible
Warning: Checkpoint corrupted (checksum failed)
```

### Error Handling Strategies

1. **Graceful Degradation**: When optional features fail, continue without them
2. **Fallback Configurations**: Use reasonable defaults when parameters are missing
3. **Progress Reporting**: Regular status updates during long-running simulations
4. **Recovery Mechanisms**: Automatic recovery from corrupted checkpoints

## Troubleshooting

### Brain Not Firing

Check configuration:
```
# Ensure neurons have sufficient input
nlm --run --steps 100 --world-size 10 10

# Check that sensory neurons are properly connected
# Verify vision parameters are reasonable
```

### Learning Not Occurring

Check neuromodulation:
```
# Ensure reward modulation is enabled
nlm --run --enable-learning --steps 500

# Check that dopamine signals are being generated
# Verify prediction error computation
```

### Memory System Issues

Check memory capacity:
```
# Ensure sufficient memory capacity
nlm --run --steps 1000 --enable-development

# Verify memory system initialization
```

### Performance Problems

Check system resources:
```
# Monitor memory usage
nlm --run --steps 100 --verbose

# Check for memory leaks in long simulations
```

## Examples and Tutorials

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create simplest possible brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Let it think
for i in range(10):
    brain.step(i)

print("Simple brain test done!")
```

### Example 2: Brain in a World
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Complete Agent
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Research Applications

### Cognitive Modeling
NLM can model various cognitive phenomena:

- **Working Memory**: Active maintenance of information
- **Attention**: Competitive selection and focus
- **Learning**: Both supervised and unsupervised learning
- **Memory**: Multiple memory systems with different properties

### Developmental Robotics
NLM simulates developmental processes:

- **Sensitive Periods**: Critical periods for learning
- **Experience-Dependent Plasticity**: Learning shaped by experience
- **Structural Plasticity**: Growth and refinement of neural connections
- **Maturation**: Progressive specialization of brain regions

### Neuromodulation Modeling
NLM implements biological neuromodulation:

- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration-exploitation tradeoff
- **Novelty**: Detection of unexpected events
- **Prediction Error**: Comparison of expected vs. actual outcomes

## Future Directions

### Planned Enhancements

1. **Multi-Agent Systems**: Cooperative and competitive agent interactions
2. **Social Learning**: Learning from observing others
3. **Language Processing**: Natural language understanding and generation
4. **Emotions**: Affective states and emotional learning
5. **Consciousness**: Theoretical models of consciousness

### Research Opportunities

1. **Brain-Computer Interfaces**: Real-time brain state interpretation
2. **NeuroAI**: Integration of symbolic and subsymbolic reasoning
3. **Explainable AI**: Making neural decisions interpretable
4. **Ethics**: Developing moral reasoning systems
5. **Safety**: Robustness and safety guarantees

## References

### Scientific Papers

1. McClelland, J. L., McNaughton, B. L., & O'Reilly, R. C. (1995). 
   "Complementary learning systems in the brain and their implications for learning and behavior."
   *Cognition and Brain Theory*, 18(1), 3-70.

2. Friston, K. (2010). 
   "The free-energy principle: A unified brain theory?"
   *Nature Reviews Neuroscience*, 11(2), 127-138.

3. Yu, D., & Dayan, P. (2005). 
   "Expected utility maximization in the brain."
   *Journal of Neural Encoding*, 1(1), 39-53.

### Technical Documentation

1. “Neuromodulation and Learning in Artificial Neural Networks"
2. “Developmental Approaches to Machine Learning"
3. “Memory Systems in Cognitive Architectures"
4. "Predictive Processing in Brain Models"

## Support and Community

### Documentation
- Full API documentation available in `docs/` directory
- Examples and tutorials in `examples/` directory
- Community forums for discussion

### Bug Reports
- Submit bug reports to the NLM GitHub repository
- Include system information and reproduction steps
- Provide log files and configuration details

### Feature Requests
- Submit feature requests for new capabilities
- Request documentation improvements
- Propose research collaborations

### Contact
- Email: nlm-support@research.org
- GitHub: https://github.com/nlm-project/nlm
- Mailing list: nlm-discuss@lists.research.org

## Acknowledgments

This project builds on decades of research in computational neuroscience, artificial intelligence, and cognitive modeling. We acknowledge the contributions of the scientific community and the open-source software ecosystem that made this work possible.

## License

MIT License. See LICENSE file for details.
