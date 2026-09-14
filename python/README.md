# NLM Python Bindings - Enhanced API Documentation

The NLM Python bindings have been significantly enhanced to provide comprehensive access to the NLM C++ neural simulation framework. This document covers all features including new additions for power users and advanced use cases.

## Overview

The `pynlm` module now provides **complete API coverage** for all NLM systems:

- **Full Brain & Neural Control**: 50+ Neuron methods, 15+ Synapse methods
- **Enhanced Sensory Processing**: Vision, Audio, InternalSignals with advanced utilities
- **Memory Systems**: Working, episodic, and associative memory access
- **Prediction & Cognition**: Full prediction system, neural planner, concept formation
- **Neuromodulation**: Complete dopamine, curiosity, novelty, prediction error systems
- **Development**: Advanced developmental stage management
- **Configuration**: Type-safe configuration with improved error handling

## Key Improvements

### 1. Complete Neuron API (50+ methods)

The Neuron class now provides comprehensive control over neural behavior:

```python
import pynlm as nlm

# Create brain and access neurons
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access and configure individual neurons
for region in brain.getRegions():
    for population in region.getPopulations():
        for neuron in population.getNeurons():
            # Configure neuron properties
            neuron.setType(nlm.NeuronType.Excitatory)
            neuron.setRestingPotential(-70.0)
            neuron.setThreshold(-55.0)
            neuron.setRefractoryPeriod(5)
            
            # Control neuron state
            neuron.addToMembranePotential(10.0)
            neuron.setFiringState(nlm.FiringState.Active)
            
            # Spike management
            if neuron.checkThreshold():
                neuron.recordSpike()
                neuron.getSpikeHistory()
                
            # State inspection
            state = neuron.getState()
            print(f"Neuron {neuron.getId()}: V={neuron.getMembranePotential()}, "
                  f"firing={neuron.isFiring()}")
```

### 2. Complete Synapse API (15+ methods)

Full synaptic connectivity and plasticity control:

```python
# Access and configure synapses
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get synapse information
for synapse in brain.getAllSynapses():
    # Basic properties
    source_id = synapse.getSourceNeuron()
    target_id = synapse.getDestinationNeuron()
    weight = synapse.getWeight()
    synapse_type = synapse.getSynapseType()
    
    # Plasticity control
    synapse.setWeight(0.5)
    synapse.addToWeight(0.01)
    synapse.recordPreSpike()
    synapse.recordPostSpike()
    
    # Eligibility traces for STDP
    eligibility = synapse.getEligibilityTrace()
    
    # Connection properties
    delay = synapse.getDelay()
    is_plastic = synapse.isPlastic()
```

### 3. Enhanced Sensory Input Processing

#### Vision (Advanced Image Processing)

```python
import pynlm as nlm
import numpy as np

# Create and process vision
vision = nlm.createVision(256, 256, 3)

# Generate test image
width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
data = np.random.rand(width * height * channels).astype(np.float32)
vision.setData(data)

# Advanced image processing pipeline
vision_normalized = vision.normalize()
vision_resized = vision.resizeBilinear(128, 128)
vision_contrast = vision.applyContrast(1.5)
vision_edge = vision.detectEdges()

# Check validity
if vision.isValid():
    print(f"Valid vision: {vision.getWidth()}x{vision.getHeight()}")
```

#### Audio (Sound Processing)

```python
# Audio processing with effects
audio = nlm.createAudio(sampleRate=44100, numSamples=44100)

# Generate sine wave
import math
for i in range(44100):
    audio.setSample(i, math.sin(2 * math.pi * 440 * i / 44100) * 0.5)

# Apply audio effects
audio_reversed = audio.applyEffect("reverse")
audio_echo = audio.applyEffect("echo", 0.3)
audio_normalized = audio.normalize(0.7)
audio_pitch = audio.pitchShift(1.2)

# Audio analysis
print(f"Duration: {audio.getDuration():.2f} seconds")
print(f"Peak amplitude: {audio.getPeakAmplitude()}")
```

#### InternalSignals (Statistical Processing)

```python
# Internal signal processing for neuromodulation
internal = nlm.createInternalSignals()

# Add various signals
for i in range(100):
    internal.addSignal(math.sin(i * 0.1))

# Statistical analysis
print(f"Max signal: {internal.getMaxSignal()}")
print(f"Min signal: {internal.getMinSignal()}")
print(f"Mean: {internal.getAverageSignal()}")
print(f"Std dev: {internal.getStdDev()}")

# Signal processing
internal_smoothed = internal.smooth(0.5)
internal_noisy = internal.addNoise(0.1)
internal_normalized = internal.normalize()
```

### 4. Memory Systems Access

#### Working Memory

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()

# Manage working memory
working_memory.clear()
working_memory.setCapacity(1000)
working_memory.addEntry("stimulus1", [0.1, 0.5, 0.9])
working_memory.addEntry("stimulus2", [0.3, 0.7, 0.2])

# Access entries
entry = working_memory.getEntry("stimulus1")
all_entries = working_memory.getAllEntries()

# Statistical operations
print(f"Usage: {working_memory.getUsage()}")
print(f"Capacity: {working_memory.getCapacity()}")
print(f"Efficiency: {working_memory.getEfficiency()}")
```

#### Episodic Memory

```python
episodic_memory = brain.getEpisodicMemory()

# Record experiences
episodic_memory.recordExperience(
    timestamp=0.0,
    sensory_data=[0.1, 0.5, 0.9],
    action_taken=action,
    reward_received=0.8
)

# Replay experiences
replay_data = episodic_memory.replayExperience(0.0, 100)

# Consolidation
episodic_memory.consolidate()
```

#### Associative Memory

```python
associative_memory = brain.getAssociativeMemory()

# Form associations
associative_memory.formAssociation(
    pattern1=[0.1, 0.5, 0.9],
    pattern2=[0.3, 0.7, 0.2],
    strength=0.8
)

# Retrieve associations
associations = associative_memory.getAssociations([0.1, 0.5, 0.9])
```

### 5. Prediction System

```python
prediction_system = brain.getPredictionSystem()

# Set up prediction
prediction_system.setPredictedValue(0.8)
prediction_system.setCurrentValue(0.6)

# Calculate prediction error
error = prediction_system.getPredictionError()
prediction_system.updatePrediction(error, 0.01)

# Access predictions
predictions = prediction_system.getAllPredictions()
```

### 6. Neuromodulation Systems

```python
# Access all neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionError()

# Configure and control
brain.enableRewardModulation(True)
brain.enableCuriosity(True)
brain.enableNovelty(True)

# Get current levels
print(f"Dopamine: {dopamine.getLevel()}")
print(f"Curiosity: {curiosity.getLevel()}")
print(f"Novelty: {novelty.getLevel()}")
print(f"Prediction Error: {prediction_error.getLevel()}")
```

### 7. Development System

```python
brain.enableDevelopment(True)
development_system = brain.getDevelopmentSystem()

# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")

# Configure development
development_system.setMaturationRate(0.01)
development_system.setPlasticityModifier(1.0)

# Update development
brain.develop()
```

## Advanced Features

### 1. Convenience Factory Functions

```python
# Enhanced factory functions
vision = pynlm.createVision(640, 480)
audio = pynlm.createAudio(22050, 22050)
internal = pynlm.createInternalSignals()
action = pynlm.createAction(pynlm.ActionType.MoveForward)
action_with_params = pynlm.createActionWithParams(
    pynlm.ActionType.Custom, 
    [0.1, 0.5, 0.9]
)

# ID creation utilities
neuron_id = pynlm.createNeuronId(123)
synapse_id = pynlm.createSynapseId(456)
region_id = pynlm.createRegionId(789)
population_id = pynlm.createPopulationId(999)
```

### 2. Mathematical Utilities

```python
# Math utilities for signal processing
value = pynlm.clamp(5.0, 0.0, 10.0)  # Returns 5.0
normalized = pynlm.normalize(5.0, 0.0, 10.0)  # Returns 0.5
lerped = pynlm.lerp(0.5, 0.0, 10.0)  # Returns 5.0

# Random number generation
random_val = pynlm.random_float()
random_range = pynlm.random_range(0.0, 1.0)
```

### 3. Error Handling and Validation

```python
# Validation utilities
pynlm.check_neuron_valid(neuron_id, "access")
pynlm.check_synapse_valid(synapse_id, "modify")
pynlm.check_config_key("brain.neuron_count", "get")

# Exception handling
try:
    brain.step(step)
except pynlm.NLMError as e:
    print(f"Error: {e}")
except pynlm.InvalidNeuronError as e:
    print(f"Invalid neuron: {e}")
```

### 4. AgentBrain Enhanced API

```python
# Enhanced agent methods
agent = pynlm.createAgentBrain(brain)

# Convenience wrappers
print(f"Total neurons: {agent.getTotalNeurons()}")
print(f"Total synapses: {agent.getTotalSynapses()}")
print(f"Active neurons: {agent.getActiveNeurons()}")
print(f"Firing neurons: {agent.getFiringNeurons()}")
print(f"Average firing rate: {agent.getAverageFiringRate()}")

# Advanced neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)
```

### 5. Configuration System (Type-Safe)

```python
config = pynlm.createDefaultConfig()

# Type-safe configuration
config.set_int("brain.neuron_count", 5000)
config.set_float("brain.synapse_density", 0.15)
config.set_string("brain.name", "AdvancedBrain")

# Type-safe getters with defaults
neuron_count = config.get_int("brain.neuron_count", 1000)
synapse_density = config.get_float("brain.synapse_density", 0.1)
brain_name = config.get_string("brain.name", "DefaultBrain")

# Enhanced config operations
if config.has("brain.neuron_count"):
    keys = config.getKeys()
```

## Performance Optimizations

### 1. Batch Operations

```python
# Process multiple neurons efficiently
neurons = brain.getAllNeurons()
for neuron in neurons:
    neuron.addToMembranePotential(0.1)

# Batch actions
actions = [pynlm.createAction(pynlm.ActionType.MoveForward)
          for _ in range(10)]
```

### 2. Memory Pooling

```python
# Use memory pools for better performance
memory_pool = brain.getMemoryPool()
neuron = memory_pool.getNeuron()
# ... use neuron ...
memory_pool.returnNeuron(neuron)
```

### 3. Parallel Processing

```python
# Parallel processing for large simulations
import concurrent.futures

def process_neuron(neuron):
    # Process individual neuron
    neuron.addToMembranePotential(0.1)

neurons = brain.getAllNeurons()
with concurrent.futures.ThreadPoolExecutor() as executor:
    executor.map(process_neuron, neurons)
```

## Advanced Tutorials

### 1. Building a Cognitive Agent

```python
def build_cognitive_agent(config):
    # Create and configure brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all cognitive systems
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Create world and environment
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
    agent.initialize(world)
    
    return agent, brain, world

# Configure for advanced cognition
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 10000)
config.set_float("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", "true")

agent, brain, world = build_cognitive_agent(config)
```

### 2. Memory-Enhanced Learning

```python
def memory_enhanced_learning(brain, world, agent):
    # Enable memory systems
    episodic = brain.getEpisodicMemory()
    working = brain.getWorkingMemory()
    
    # Run learning episode
    for step in range(1000):
        # Store in working memory
        percept = world.getSensoryPercept()
        working.addEntry(f"step_{step}", percept.getData())
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Record experience
        episodic.recordExperience(
            timestamp=step,
            sensory_data=percept.getData(),
            action=agent.decodeMotorCommand(),
            reward=percept.getInternal()[0] if percept.getInternal() else 0.0
        )
        
        # Replay and consolidate occasionally
        if step % 100 == 0:
            episodic.consolidate()
    
    return brain.getTotalSpikeCount()
```

## Installation and Setup

### Standard Installation

```bash
pip install pynlm
```

### Development Installation

```bash
pip install -e .
```

### Building from Source

```bash
mkdir build
cd build
cmake ..
make -j4
```

## Migration Guide

### From Old API

If you're migrating from the previous API version, note these changes:

**Removed Functions:**
- `brain.getWorkingMemory()` - now returns proper memory interface
- `brain.getEpisodicMemory()` - now returns proper memory interface
- `agent.getTotalNeurons()` - added as convenience wrapper

**New Functions:**
- Added 50+ Neuron methods for full neural control
- Added 15+ Synapse methods for complete synaptic control
- Added type-safe config methods
- Added mathematical utility functions

**Changed Functions:**
- `config.get()` - now has type-safe variants: `get_int()`, `get_float()`, `get_string()`
- `action.getParameters()` - now has `getParameter(index, default)` method

## Troubleshooting

### Common Issues

**Import Errors:**
```python
# Ensure pynlm is installed correctly
import pynlm
print(pynlm.__version__)  # Should print "0.1.0"
```

**Memory Issues:**
```python
# Monitor memory usage
brain = pynlm.createBrain(config)
brain.initialize()

# Process in chunks to avoid memory issues
for batch_start in range(0, total_neurons, batch_size):
    process_neurons(brain, batch_start, batch_size)
```

**Performance Issues:**
```python
# Enable optimizations
config.set("performance.optimizations", "enabled")
config.set("simulation.parallel", "true")
brain.initialize()
```

## License

MIT License

## Support

For advanced support and custom implementations, contact the NLM team at https://github.com/nlm-project/nlm
