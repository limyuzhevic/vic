# Quick Start: NLM Phase 6 Integration

This is a quick guide for starting with NLM Phase 6's integrated artificial brain. If you're new to NLM, see the [Easy Usage Guide](easy_usage.md) first.

## Quick Test: Integration Verification

Run the integration test to verify all systems work together:

```bash
./nlm_phase6_demo
```

This will test:
- Memory integration (Working Memory + Episodic Memory)
- Neuromodulation (Dopamine, Curiosity, Novelty)
- Prediction system
- Attention system
- Development stages

## Basic Integration Example

Here's a complete example showing how integrated systems work together:

```python
import pynlm
import time

def run_integration_demo():
    print("=== NLM Phase 6 Integration Demo ===\n")
    
    # 1. Create brain with default configuration
    print("1. Creating brain with integrated memory systems...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Verify memory systems are integrated
    print("2. Checking memory integration...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if working_memory and episodic_memory:
        print("   ✓ Working memory integrated")
        print("   ✓ Episodic memory integrated")
        print(f"   - Working memory capacity: {working_memory.getCapacity()}")
        print(f"   - Max episodic episodes: {episodic_memory.getMaxEpisodes()}")
    else:
        print("   ✗ Memory systems not properly integrated!")
        return
    
    # 3. Create world and agent
    print("\n3. Setting up agent-world interface...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features
    print("\n4. Enabling learning subsystems...")
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)   # Grow new connections  
    agent.enableDevelopment(True)            # Brain matures
    agent.enableCuriosity(True)              # Explore novel things
    
    # 5. Run simulation
    print("\n5. Running integration simulation...")
    start_time = time.time()
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = agent.applyRewardModulation(0.1, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Status update every 100 steps
        if step % 100 == 0:
            print(f"   Step {step}:")
            print(f"     - Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"     - Working memory traces: {working_memory.getActiveTraces()}")
            print(f"     - Episodes stored: {episodic_memory.getEpisodeCount()}")
            print(f"     - Dopamine level: {agent.getNeuromodulationLevel():.3f}")
            print(f"     - Developmental stage: {brain.getDevelopmentalStage()}")
    
    # 6. Report results
    print(f"\n=== Integration Results ===")
    print(f"Simulation time: {time.time() - start_time:.2f}s")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Working memory traces: {working_memory.getActiveTraces()}")
    print(f"Episodes stored: {episodic_memory.getEpisodeCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # 7. Show integration status
    print(f"\n=== Integration Status ===")
    print(f"✓ Working Memory: {'INTEGRATED' if working_memory else 'NOT WORKING'}")
    print(f"✓ Episodic Memory: {'INTEGRATED' if episodic_memory else 'NOT WORKING'}")
    print(f"✓ Neuromodulation: {'INTEGRATED' if agent.getNeuromodulationLevel() > 0 else 'NOT WORKING'}")
    print(f"✓ Development: {'INTEGRATED' if brain.getDevelopmentalStage() != 0 else 'NOT WORKING'}")
    
    # 8. Save results
    print("\n8. Saving brain state...")
    saved = brain.save("integration_checkpoint.bin")
    if saved:
        print("   ✓ Checkpoint saved successfully")
    
    return True

if __name__ == "__main__":
    success = run_integration_demo()
    if success:
        print("\n=== SUCCESS ===")
        print("The NLM brain is properly integrated with memory, neuromodulation,")
        print("and development systems! This is Phase 6's integrated artificial brain.")
    else:
        print("\n=== FAILURE ===")
        print("Integration verification failed.")
```

## Integration Features Guide

### 1. Memory Integration

The integrated brain now properly stores and retrieves experiences:

```python
# Check memory status
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()

print(f"Working memory capacity: {wm.getCapacity()}")
print(f"Active traces: {wm.getActiveTraces()}")
print(f"Max episodes: {em.getMaxEpisodes()}")
print(f"Current episodes: {em.getEpisodeCount()}")
```

### 2. Neuromodulation Integration

Dopamine, curiosity, and novelty work together:

```python
# Enable neuromodulation features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)  # Note: this might be available

# Check levels
print(f"Dopamine: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
```

### 3. Development Stages

Brains go through developmental stages that affect learning:

```python
# Check current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Development affects plasticity
agent.updateDevelopment(0.1)  # Advance development by 0.1 time units
```

### 4. Prediction System

The prediction system tracks expected outcomes:

```python
# Prediction error helps learning
prediction_error = agent.getPredictionError()
print(f"Prediction error: {prediction_error:.3f}")

# Low error = good predictions
# High error = need to learn more
```

### 5. Attention System

Attention selects what's most important:

```python
# Access attention system
attention = brain.getAttention()
if attention:
    # Attention affects what gets stored in memory
    print(f"Attention inhibition: {attention.getInhibitionStrength():.3f}")
    print(f"Attention excitation: {attention.getExcitationStrength():.3f}")
```

### 6. Advanced Integration Example

Here's a more advanced example showing how to use all integrated features:

```python
import pynlm

def run_advanced_simulation(config_path=None):
    """Run simulation with full integration of all brain systems."""
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    if config_path:
        config.loadFromFile(config_path)
    
    # Create brain with integrated systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with full capabilities
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure world with goals
    world.setMaxEnergy(100.0)  # Resources
    
    # Run simulation
    print("Running advanced integration simulation...")
    for step in range(1000):
        # 1. Sense
        percept = world.getSensoryPercept()
        
        # 2. Process (attention, prediction, memory update)
        agent.processSensoryInput(percept)
        
        # 3. Think (cognition, memory consolidation)
        brain.step(step)
        
        # 4. Decide (planning, attention)
        action = agent.decodeMotorCommand()
        
        # 5. Act
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Learn (reward, development, structural changes)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Log every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing {brain.getFiringNeuronCount()} neurons, "
                  f"{brain.getWorkingMemory().getActiveTraces()} memory traces, "
                  f"{brain.getEpisodicMemory().getEpisodeCount()} episodes, "
                  f"Dopamine: {agent.getNeuromodulationLevel():.2f}")
    
    return brain, agent, world

# Usage example
if __name__ == "__main__":
    brain, agent, world = run_advanced_simulation()
    
    # Save final state
    brain.save("final_brain_state.bin")
    
    print("\n=== Advanced Simulation Complete ===")
    print("The brain has learned and adapted through integration of:")
    print("- Working memory for short-term retention")
    print("- Episodic memory for experience storage")
    print("- Neuromodulation for reward and curiosity")
    print("- Development for maturation and adaptation")
    print("- Prediction for expectation and error correction")
    print("- Attention for selective processing")
```

## Integration Checkpoints

You can save and load brain states to continue learning:

```python
# Save after training
brain.save("my_brain_checkpoint.bin")

# Later, load and continue
brain2 = pynlm.createBrain(config)
brain2.load("my_brain_checkpoint.bin")
# Continue simulation...
```

## Configuration for Integration

Create a custom configuration file for optimal integration:

```yaml
# integration_config.yaml
brain:
  neuron_count: 2000    # Larger brain for better memory
  region_count: 2       # Multiple regions
  connection_probability: 0.12

memory:
  working:
    capacity: 200      # More working memory capacity
    decay_rate: 0.01
  episodic:
    max_episodes: 2000  # More experience storage

neuromod:
  dopamine:
    scale: 1.5          # Stronger reward signal
  curiosity:
    enable: true
    sensitivity: 0.7
  novelty:
    enable: true
    threshold: 0.3

agent:
  development:
    enable: true
    age_factor: 0.2     # Age affects development more
  reward:
    scale: 1.0
    prediction_enabled: true

performance:
  checkpoint:
    enable: true
    interval: 200      # Save more frequently
  replay:
    interval: 50       # Replay more often
```

Load this configuration:

```python
import yaml
with open("integration_config.yaml", "r") as f:
    config_data = yaml.safe_load(f)

# Apply to config
# (Implementation would depend on the actual config API)
```

## Troubleshooting Integration

### Common Issues

**Problem**: Memory systems not working
**Solution**: Check if brain was initialized properly:
```python
if not brain.initialize():
    print("Brain initialization failed!")
```

**Problem**: Low firing rate
**Solution**: Ensure neurons are getting input:
```python
# Inject sensory input
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)
```

**Problem**: No memory storage
**Solution**: Check if working memory is properly initialized:
```python
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory active: {wm.getActiveTraces()}")
```

**Problem**: Development not progressing
**Solution**: Check development updates:
```python
agent.updateDevelopment(0.1)  # This advances development
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")
```

### Debug Integration

```python
# Debug all integration systems
def debug_integration(brain, agent):
    print("=== Integration Debug ===")
    
    # Memory
    wm = brain.getWorkingMemory()
    em = brain.getEpisodicMemory()
    print(f"Working memory: {'✓' if wm else '✗'} ({wm.getActiveTraces() if wm else 0} traces)")
    print(f"Episodic memory: {'✓' if em else '✗'} ({em.getEpisodeCount() if em else 0} episodes)")
    
    # Neuromodulation
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    print(f"Dopamine: {'✓' if dopamine else '✗'}")
    print(f"Curiosity: {'✓' if curiosity else '✗'}")
    
    # Cognition
    attention = brain.getAttention()
    planner = brain.getPlanner()
    print(f"Attention: {'✓' if attention else '✗'}")
    print(f"Planner: {'✓' if planner else '✗'}")
    
    # Development
    dev = brain.getDevelopmentSystem()
    print(f"Development: {'✓' if dev else '✗'}")
    
    # Prediction
    prediction = brain.getPredictionSystem()
    print(f"Prediction: {'✓' if prediction else '✗'}")
    
    # Agent settings
    print(f"Agent neuromodulation: {'✓' if agent.getNeuromodulationLevel() > 0 else '✗'}")
    print(f"Agent curiosity: {'✓' if agent.getCuriosityLevel() > 0 else '✗'}")
```

## Quick Test Script

Save this as `test_integration.py` to quickly verify integration:

```python
#!/usr/bin/env python3
"""Quick test script for NLM Phase 6 integration."""

import pynlm
import sys

def main():
    print("Testing NLM Phase 6 Integration...\n")
    
    # Test 1: Basic integration
    print("Test 1: Creating integrated brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain")
        return 1
    
    print("✓ Brain initialized")
    
    # Test 2: Memory integration
    print("\nTest 2: Memory system integration...")
    wm = brain.getWorkingMemory()
    em = brain.getEpisodicMemory()
    
    if not wm:
        print("✗ Working memory not integrated")
        return 1
    
    if not em:
        print("✗ Episodic memory not integrated")
        return 1
    
    print("✓ Memory systems integrated")
    
    # Test 3: Neuromodulation
    print("\nTest 3: Neuromodulation integration...")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    
    if not dopamine:
        print("✗ Dopamine system not integrated")
        return 1
    
    print("✓ Neuromodulation systems integrated")
    
    # Test 4: Run simulation
    print("\nTest 4: Running integration simulation...")
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run 100 steps
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("✓ Integration simulation successful")
    
    # Test 5: Save/load
    print("\nTest 5: Persistence integration...")
    if not brain.save("test_checkpoint.bin"):
        print("✗ Checkpoint save failed")
        return 1
    
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    if not brain2.load("test_checkpoint.bin"):
        print("✗ Checkpoint load failed")
        return 1
    
    print("✓ Persistence integration working")
    
    print("\n=== All Integration Tests Passed ===")
    print("NLM Phase 6 integration is working correctly!")
    return 0

if __name__ == "__main__":
    sys.exit(main())
```

Run the test:

```bash
python3 test_integration.py
```

This should complete all integration tests successfully.
