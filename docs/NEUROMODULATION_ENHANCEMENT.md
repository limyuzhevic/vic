# Neuromodulation System Enhancement Documentation

## Overview

The NLM neuromodulation system has been enhanced with realistic implementations of all major neuromodulators. This document describes the new functionality and provides examples for users.

## What's New

### 1. Complete Neuromodulator Suite

**Previously:** Only Dopamine, Novelty, and Curiosity were implemented
**Now:** Full suite of biologically-inspired neuromodulators:

- **Dopamine (DA)** - Reward prediction error signaling
- **Acetylcholine (ACh)** - Attention and memory consolidation  
- **Norepinephrine (NE)** - Arousal and signal-to-noise enhancement
- **Serotonin (5-HT)** - Mood, impulsivity, and social behavior
- **Novelty** - Novelty detection
- **Curiosity** - Exploration motivation
- **PredictionError** - Prediction error signaling

### 2. Neuromodulation Helper Functions

The NeuromodulatorHelpers namespace provides coordination functions:

- `applyAllNeuromodulators()` - Main coordination function
- `applyNeuromodulationToExcitability()` - Neuronal firing modulation
- `applyNeuromodulationToPlasticity()` - Synaptic modification scaling
- `applyNeuromodulationToMemory()` - Memory encoding/consolidation enhancement
- `applyNeuromodulationToAttention()` - Attentional focus modulation
- `applyNeuromodulationToPrediction()` - Prediction error enhancement

## Usage Examples

### Basic Neuromodulation Usage

```python
import pynlm

# Create brain with neuromodulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check neuromodulation levels
dopamine = brain.getDopamine()
if dopamine:
    print(f"Dopamine level: {dopamine.getLevel():.2f}")

# Check all neuromodulators
from src.neuromodulation import NeuromodulatorHelpers

state = NeuromodulatorHelpers.getAllNeuromodulationState(brain)
print("Neuromodulation state:")
for name, level in state.items():
    print(f"  {name}: {level:.2f}")
```

### Accessing Neuromodulators in AgentBrain

```python
import pynlm

# Create full agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulation subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# The system now uses:
# - Dopamine for reward prediction errors
# - Acetylcholine for attention/memory
# - Norepinephrine for arousal/vigilance  
# - Serotonin for mood/impulsivity
# - Novelty for unexpected input detection
# - Curiosity for exploration motivation
```

### Neuromodulation-based Action Selection

```python
def runAgentWithNeuromodulation(agent, world, steps):
    """Run agent with enhanced neuromodulation effects"""
    for step in range(steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes with neuromodulation
        brain = agent.getBrain()
        brain.step(step)
        
        # Enhanced action selection with neuromodulation
        action = agent.decodeMotorCommand()
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Check neuromodulation effects
        dopamine_level = agent.getNeuromodulationLevel()
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        
        print(f"Step {step}: DA={dopamine_level:.2f}, Cur={curiosity_level:.2f}, Nov={novelty_level:.2f}")
        
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Neuromodulation for Advanced Agent Behavior

```python
def neuromodulationAwareAgent(agent, world, steps):
    """Agent that adapts behavior based on neuromodulation state"""
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain = agent.getBrain()
        brain.step(step)
        
        # Neuromodulation-based decision making
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        
        action = agent.decodeMotorCommand()
        
        # High curiosity → more exploration
        if curiosity > 0.7:
            action = agent.selectWithCuriosity(action)
        
        # Low dopamine → more risk taking
        if dopamine < 0.2:
            # Conservative behavior
            pass
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation feedback
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, reward * 0.9)  # Predict reward from previous
```

## Technical Details

### Neuromodulation Coordination Order

The `NeuromodulatorHelpers.applyAllNeuromodulators()` function processes neuromodulators in this order:

1. **Acetylcholine** - First for attention/memory enhancement
2. **Norepinephrine** - Second for arousal and signal-to-noise
3. **Serotonin** - Third for mood/impulsivity regulation
4. **Dopamine** - Fourth for reward prediction error
5. **Novelty** - Fifth for sensory change detection
6. **Curiosity** - Sixth for exploration motivation

### Neuroscientific Models

Each neuromodulator follows biologically-inspired dynamics:

**Inverted-U Curves**: Most effects follow inverted-U relationships (optimal at intermediate levels)
**Phasic/Tonic Dynamics**: Neuromodulators have both phasic (event-driven) and tonic (baseline) components
**Cross-Modulation**: Neuromodulators interact and modulate each other's effects
**State-Dependent Effects**: Neuromodulator impact depends on current brain state

### Integration Points

Neuromodulators enhance the following systems:

- **Neural Excitability**: Modulates neuronal firing thresholds and responsiveness
- **Plasticity**: Scales synaptic modification based on learning signals
- **Memory**: Enhances encoding and consolidation processes
- **Attention**: Modulates focus and information selection
- **Prediction**: Improves prediction error signaling
- **Emotion/Motivational State**: Influences drive and valence

## Advanced Features

### Custom Neuromodulation Profiles

```python
# Create agent with custom neuromodulation weights
def createCustomNeuromodAgent():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Custom neuromodulation parameters
    acetylcholine = brain.getAcetylcholine()
    acetylcholine.setAttentionGain(1.5f)
    acetylcholine.setMemoryConsolidationRate(0.8f)
    
    norepinephrine = brain.getNorepinephrine()
    norepinephrine.setArousalBaseline(0.3f)
    norepinephrine.setSignalNoiseRatio(1.2f)
    
    serotonin = brain.getSerotonin()
    serotonin.setRiskAversion(0.5f)
    serotonin.setSocialLearningRate(0.7f)
    
    dopamine = brain.getDopamine()
    dopamine.setPredictionErrorSensitivity(1.0f)
    dopamine.setLearningRate(0.01f)
    
    return agent
```

### Neuromodulation-based Learning

The enhanced neuromodulation system enables:

- **Error-driven Learning**: Accurate reward prediction error signaling
- **State-dependent Plasticity**: Neuromodulator-dependent synaptic modification
- **Memory Prioritization**: Neuromodulator-based memory allocation
- **Exploration/Exploitation Balance**: Neuromodulator-guided behavioral strategies
- **Cross-system Integration**: Neuromodulators coordinating multiple brain systems

## Performance Considerations

### Computational Complexity

- Neuromodulation calculations are lightweight and linear
- Each neuromodulator operates independently with minimal overhead
- Neuromodulation updates occur during regular simulation steps
- Memory usage minimal (only neuromodulator state stored)

### Scaling Considerations

- Neuromodulators scale to large brain networks
- Each neuron type can receive different neuromodulatory inputs
- Neuromodulation effects are distributed and parallelizable
- No significant bottlenecks in neuromodulation processing

## Troubleshooting

### Common Issues

1. **Neuromodulators not showing effects**
   - Ensure neuromodulators are initialized in brain
   - Check that neuromodulation helpers are called
   - Verify neuromodulator levels are non-zero

2. **Overly strong neuromodulation effects**
   - Check neuromodulator gain parameters
   - Adjust inverted-U curve parameters
   - Consider normalization of neuromodulator levels

3. **Neuromodulation not integrating with other systems**
   - Ensure Brain::step() calls neuromodulation helpers
   - Check that AgentBrain properly applies neuromodulation
   - Verify that memory systems receive neuromodulatory inputs

4. **Neuromodulator crashes**
   - Check for null neuromodulator pointers
   - Verify proper initialization sequence
   - Ensure all neuromodulators are properly configured

## Summary

The enhanced neuromodulation system provides:

- **Biological realism**: Comprehensive set of neuromodulators with realistic dynamics
- **Enhanced functionality**: Neuromodulation helper functions for system coordination
- **Integration**: Seamless integration with existing memory, prediction, and cognitive systems
- **Customization**: Configurable neuromodulation parameters for different behaviors
- **Performance**: Lightweight and scalable implementation
- **Documentation**: Clear examples and technical details for users

This enhancement makes NLM significantly more capable of simulating realistic brain-like learning and behavior with proper neuromodulatory control of attention, memory, motivation, and plasticity.
