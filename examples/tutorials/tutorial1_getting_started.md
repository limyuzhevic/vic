# Tutorial 1: Getting Started with NLM

This tutorial provides a comprehensive introduction to the NLM (Neural Learning Machine) Python API. It covers basic setup, core concepts, and your first working examples.

## Learning Objectives

By the end of this tutorial, you will be able to:

- Create and initialize an NLM brain
- Set up a simple simulation environment
- Understand the agent-brain interface
- Run your first neural simulations
- Save and load brain states

## 1. Introduction to NLM

NLM is an experimental artificial developmental brain that combines neural computation with learning systems. It features:

- **Neural networks** with real-time dynamics
- **Multiple memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty)
- **Prediction and planning** capabilities
- **Developmental stages** affecting learning
- **Structural plasticity** for growth and pruning

## 2. Setting Up Your Environment

### 2.1 Installation

First, ensure you have the NLM Python bindings installed:

```bash
pip install pynlm
```

### 2.2 Basic Imports

```python
import pynlm
import numpy as np
from datetime import datetime
```

## 3. Creating Your First Brain

### 3.1 Brain Creation

The brain is the central component of NLM:

```python
# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain for simulation
brain.initialize()
```

### 3.2 Brain Configuration

```python
# You can customize configuration
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("plasticity_learning_rate", 0.02)

brain = pynlm.createBrain(config)
brain.initialize()
```

## 4. Simple Simulation Setup

### 4.1 Creating the World

```python
# Create a simple 2D world for simulation
world = pynlm.createSimpleWorld()

# Configure world dimensions and agent's view
world.configure(width=20, height=20, visionWidth=12, visionHeight=12)

# Reset world to initial state
world.reset()
```

### 4.2 Creating the Agent

```python
# Create an agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize the agent with the world
agent.initialize(world)
```

## 5. Your First Simulation Loop

```python
def run_first_simulation():
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 10, 10)
    world.reset()
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(100):
        # Update world physics
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent and brain
        agent.processSensoryInput(percept)
        
        # Perform brain computation
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Log progress
        if step % 20 == 0:
            print(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, world, agent
```

## 6. Exploring Your Brain

### 6.1 Brain Statistics

```python
def explore_brain(brain):
    print("Brain Statistics:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
```

### 6.2 Agent Information

```python
def explore_agent(agent):
    print("Agent Information:")
    print(f"  - Sensory input size: {agent.getSensoryInputSize()}")
    print(f"  - Motor output size: {agent.getMotorOutputSize()}")
    print(f"  - Current developmental stage: {agent.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

## 7. Saving and Loading

### 7.1 Save Brain State

```python
def save_brain_state(brain, filename="my_brain.nld"):
    brain.save(filename)
    print(f"Brain state saved to {filename}")
```

### 7.2 Load Brain State

```python
def load_brain_state(filename):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.load(filename)
    print(f"Brain state loaded from {filename}")
    return brain
```

## 8. Complete Example

Here's a complete working example that combines all the concepts:

```python
#!/usr/bin/env python3
import pynlm
from datetime import datetime

class FirstSimulation:
    def run(self):
        print("Starting NLM First Simulation")
        print("=" * 50)
        
        # 1. Create world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # 2. Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 4. Display initial state
        print("Initial State:")
        print(f"  - World: {world.getWidth()}x{world.getHeight()} cells")
        print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
        print(f"  - Agent: Sensory={agent.getSensoryInputSize()}, "
              f"Motor={agent.getMotorOutputSize()}")
        print()
        
        # 5. Run simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                print(f"Step {step}: Action executed")
        
        # 6. Display final statistics
        print("\nFinal Statistics:")
        self.explore_brain(brain)
        self.explore_agent(agent)
        
        # 7. Save results
        save_brain_state(brain, "first_simulation_brain.nld")
        
        print("\nFirst simulation completed successfully!")
        
    def explore_brain(self, brain):
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Synapses: {brain.getTotalSynapseCount()}")
        print(f"  - Firing rate: {brain.getAverageFiringRate():.3f}")
        
    def explore_agent(self, agent):
        print(f"  - Neuromodulation: {agent.getNeuromodulationLevel():.3f}")

if __name__ == "__main__":
    sim = FirstSimulation()
    sim.run()
```

## 9. Next Steps

### 9.1 Explore Advanced Examples

Now that you understand the basics, explore the advanced examples:

- `working_memory_example.py` - Working memory and attention
- `neuromodulation_example.py` - Dopamine, curiosity, novelty systems
- `prediction_planning_example.py` - Prediction and planning integration
- `memory_consolidation_example.py` - Memory consolidation
- `development_example.py` - Developmental stages
- `structural_plasticity_example.py` - Brain growth and pruning

### 9.2 Read Tutorial 2

Move on to **Tutorial 2: Understanding Memory Systems** to learn about the different memory systems in NLM.

### 9.3 Configure Your Brain

Check out the **Configuration Guide** for advanced configuration options.

## 10. Common Issues and Solutions

### 10.1 Brain Initialization Errors

**Problem:** Brain initialization fails.
**Solution:** Ensure configuration is valid and required dependencies are installed.

### 10.2 Simulation Crashes

**Problem:** Simulation crashes after some steps.
**Solution:** Check for numerical overflow and ensure proper bounds.

### 10.3 Performance Issues

**Problem:** Simulation is slow.
**Solution:** Reduce world size or step count, or check for inefficient code.

## 11. Further Reading

- [NLM Architecture Documentation](https://kilo.ai/docs/architecture)
- [Biological Fidelity Guide](https://kilo.ai/docs/biological-fidelity)
- [Phase 6 Final Audit](https://kilo.ai/docs/phase6)

## 12. Conclusion

Congratulations! You've completed Tutorial 1. You now have a solid foundation in NLM and can create and run your own neural simulations. The next tutorial will explore the memory systems in more detail.

Remember: The best way to learn is by experimenting with the examples and modifying them to suit your needs.

---

*Tutorial 1 completed successfully! Ready for Tutorial 2.*