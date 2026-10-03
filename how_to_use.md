# NLM (熙然) — Advanced Usage Guide

**An Experimental Artificial Developmental Brain**

## Quick Start for Advanced Users

This guide provides comprehensive examples and advanced features for using NLM beyond basic usage.

## 1. Advanced Brain Configuration

### Custom Development Stages

```python
import pynlm

# Create custom development stages
config = pynlm.createDefaultConfig()
config.set("development_stage_initial", 0.0)      # Stage start age
config.set("development_stage_critical", 60.0)    # Critical period end
config.set("development_stage_maturation", 300.0) # Maturation end
config.set("development_stage_adult", 900.0)      # Adult period start

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Neuromodulation

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable and configure neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Set neuromodulation parameters (examples)
# These would be accessed through brain.getDopamine(), etc.
# if dopamine = brain.getDopamine():
#     dopamine.setBaseline(0.1)
#     dopamine.setSensitivity(2.0)
```

## 2. Advanced Agent Configuration

### Custom Sensory Processing

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure sensory processing parameters
# This would require access to internal agent settings
# agent.setVisionRange(8.0)
# agent.setMemoryCapacity(5000)
```

### Advanced Action Selection

```python
# Custom curiosity-driven exploration
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action with enhanced curiosity
    action = agent.getActionWithCuriosityExploration()
    
    # Apply action with momentum
    agent.applyActionWithInertia(action, 0.1)
    world.applyMotorCommand(action, world.getSimulationTime())
```

## 3. Memory System Integration

### Working Memory Management

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access working memory
workingMem = brain.getWorkingMemory()

# Configure working memory
if workingMem:
    workingMem.setCapacity(500)      # Number of traces
    workingMem.setDecayRate(0.005)   # Decay per timestep
    workingMem.strengthenMemory(1.2) # Rehearse important traces

# Monitor working memory
for step in range(100):
    brain.step(step)
    if workingMem:
        traces = workingMem.getActiveTraces()
        activity = workingMem.getMemoryActivity()
        print(f"Step {step}: {traces} traces, activity {activity:.3f}")
```

### Episodic Memory Management

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access episodic memory
episodicMem = brain.getEpisodicMemory()

# Configure episodic memory
if episodicMem:
    episodicMem.setMaxEpisodes(5000)
    episodicMem.setConsolidationThreshold(0.7)

# Monitor memory storage
for step in range(200):
    brain.step(step)
    if episodicMem:
        episodes = episodicMem.getEpisodeCount()
        print(f"Step {step}: {episodes} episodes stored")
```

## 4. Development and Maturation

### Track Developmental Progress

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor development
for step in range(1000):
    brain.step(step)
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: Developmental stage {stage}")
    
    # Get plasticity modifier
    if agent := getAgentFromBrain(brain):
        plasticity = agent.getPlasticityModifier()
        print(f"  Plasticity modifier: {plasticity:.3f}")
```

## 5. Prediction and Planning

### Basic Prediction Integration

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access prediction system
predictionSystem = brain.getPredictionSystem()

if predictionSystem:
    # Configure prediction parameters
    predictionSystem.setPredictionHorizon(10)  # Steps ahead
    predictionSystem.setConfidenceThreshold(0.5)
    
    # Monitor prediction performance
    for step in range(100):
        brain.step(step)
        # Get prediction metrics
        # predictionError = predictionSystem.getCurrentError()
        # confidence = predictionSystem.getConfidence()
        # print(f"Step {step}: Prediction error {predictionError:.3f}, confidence {confidence:.3f}")
```

## 6. Checkpointing and Persistence

### Manual Checkpoint Management

```python
import tempfile
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create temporary directory for checkpoints
with tempfile.TemporaryDirectory() as tempDir:
    checkpointPath = os.path.join(tempDir, "brain_checkpoint.bin")
    
    # Run simulation for a while
    for step in range(500):
        brain.step(step)
        
        # Save checkpoint every 100 steps
        if step % 100 == 0:
            brain.save(checkpointPath)
            print(f"Saved checkpoint at step {step}")
    
    # Create new brain and load checkpoint
    savedBrain = pynlm.createBrain(pynlm.createDefaultConfig())
    savedBrain.initialize()
    
    if savedBrain.load(checkpointPath):
        print("Successfully loaded saved brain state")
        
        # Continue simulation with loaded brain
        for step in range(500, 1000):
            savedBrain.step(step)
    else:
        print("Failed to load checkpoint")
```

## 7. Advanced Simulation Patterns

### Pattern 1: Learning Environment

```python
def runLearningEnvironment(brain, world, agent, numEpisodes):
    """Environment where agent learns through interaction"""
    for episode in range(numEpisodes):
        # Reset environment
        world.reset()
        
        # Run episode
        for step in range(100):  # Max episode length
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check if episode ended
            if world.isDone():
                break
        
        # End of episode - log results
        if agent.getNoveltyLevel() > 0.5:
            print(f"Episode {episode}: High novelty, exploring new behaviors")
        
        yield episode, brain.getAverageFiringRate(), agent.getNeuromodulationLevel()

# Use the learning environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for episode, firingRate, dopamine in runLearningEnvironment(brain, world, agent, 20):
    print(f"Episode {episode}: Firing rate {firingRate:.2f}, Dopamine {dopamine:.3f}")
```

### Pattern 2: Memory Replay Simulation

```python
def runWithReplay(brain, world, agent, numSteps):
    """Run simulation with memory replay enabled"""
    replayInterval = 200  # Replay every 200 steps
    
    for step in range(numSteps):
        # Get observation and update brain
        percept = world.observe(brain.getRegions()[0].get())
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Every replayInterval, replay important memories
        if step % replayInterval == 0:
            replayImportantMemories(brain)
        
        # Get action and update world
        action = agent.decodeMotorCommand()
        world.applyAction(brain.getRegions()[0].get(), action)
        
        yield step, brain.getAverageFiringRate()

def replayImportantMemories(brain):
    """Replay recently encoded memories for consolidation"""
    episodicMem = brain.getEpisodicMemory()
    if episodicMem:
        # Get episodes for replay
        episodesToReplay = episodicMem.getEpisodesForReplay(3)
        for episode in episodesToReplay:
            # Replay the episode to strengthen connections
            episodicMem.replayEpisode(episode)
        print(f"Replayed {len(episodesToReplay)} memory episodes")
```

### Pattern 3: Developmental Trajectory

```python
def simulateDevelopment(brain, world, agent, numYears):
    """Simulate brain development over time"""
    stepsPerYear = 3600  # 1 hour per step
    
    for year in range(numYears):
        startStep = year * stepsPerYear
        endStep = (year + 1) * stepsPerYear
        
        print(f"\n--- Year {year + 1} ---")
        
        for step in range(startStep, endStep):
            # Update development
            agent.updateDevelopment(0.001)  # 1ms timestep
            
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyAction(brain.getRegions()[0].get(), action)
        
        # Log development statistics
        stage = brain.getDevelopmentalStage()
        plasticity = agent.getPlasticityModifier()
        firingRate = brain.getAverageFiringRate()
        
        print(f"  Stage: {stage}, Plasticity: {plasticity:.3f}, Firing rate: {firingRate:.2f}")
```

## 8. Performance Monitoring

### Real-time Monitoring

```python
class NLMMonitor:
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        self.metrics = []
    
    def collectMetrics(self, step):
        metrics = {
            'step': step,
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'dopamine': self.agent.getNeuromodulationLevel(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'working_memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodic_memory_episodes': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
        }
        self.metrics.append(metrics)
        return metrics
    
    def printSummary(self, steps):
        print(f"\n=== NLM Simulation Summary (Steps 0-{steps}) ===")
        if self.metrics:
            latest = self.metrics[-1]
            print(f"Final firing rate: {latest['firing_rate']:.2f}")
            print(f"Total spikes: {latest['total_spikes']}")
            print(f"Dopamine level: {latest['dopamine']:.3f}")
            print(f"Working memory traces: {latest['working_memory_traces']}")
            print(f"Episodic memory episodes: {latest['episodic_memory_episodes']}")
    
    def saveMetrics(self, filepath):
        # Save metrics to file for analysis
        import json
        with open(filepath, 'w') as f:
            json.dump(self.metrics, f, indent=2)

# Use the monitor
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create a simple world for testing
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent.initialize(world)

monitor = NLMMonitor(brain, agent)

# Run simulation with monitoring
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    metrics = monitor.collectMetrics(step)
    
    if step % 100 == 0:
        print(f"Step {step}: Firing rate {metrics['firing_rate']:.2f}, Dopamine {metrics['dopamine']:.3f}")

# Print final summary
monitor.printSummary(999)
```

## 9. Error Handling and Debugging

### Common Error Scenarios

```python
def safeBrainSimulation(brain, maxSteps=1000):
    """Simulation with error handling"""
    try:
        for step in range(maxSteps):
            # Check if brain is still valid
            if not brain or brain.getTotalNeuronCount() == 0:
                print(f"Step {step}: Brain in invalid state")
                break
            
            # Run step with error checking
            try:
                brain.step(step)
            except Exception as e:
                print(f"Step {step}: Error during brain step: {e}")
                break
            
            # Check for numerical instability
            firingRate = brain.getAverageFiringRate()
            if firingRate > 1000.0:
                print(f"Step {step}: High firing rate detected: {firingRate}")
                # Optionally reduce timestep or adjust parameters
    
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    
    except Exception as e:
        print(f"Unexpected error: {e}")
    
    finally:
        print(f"Simulation completed: {step + 1} steps")

# Use safe simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
safeBrainSimulation(brain, 500)
```

## 10. Integration with External Systems

### Basic External System Integration

```python
class ExternalSystemInterface:
    def __init__(self, brain):
        self.brain = brain
        self.externalState = None
    
    def updateExternalState(self, newState):
        """Update state from external system"""
        self.externalState = newState
        
        # Convert external state to sensory input
        if self.externalState:
            sensoryInput = self.encodeExternalToSensory(self.externalState)
            
            # Apply to brain
            if self.brain:
                self.brain.receiveSensoryInput(sensoryInput)
    
    def encodeExternalToSensory(self, externalState):
        """Convert external system state to NLM sensory input"""
        # This is system-specific - implement based on your external system
        # Example: convert sensor readings to brain current injections
        pass
    
    def getSensoryFromBrain(self):
        """Get sensory information from brain for external system"""
        # This would read from brain's sensory processing
        # and convert to format suitable for external system
        pass

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create external system interface
extInterface = ExternalSystemInterface(brain)

# Main simulation loop with external integration
for step in range(1000):
    # Update from external system
    newExternalState = readExternalSensorData()  # Your external system
    extInterface.updateExternalState(newExternalState)
    
    # Run NLM step
    brain.step(step)
    
    # Get brain output for external system
    brainOutput = extInterface.getSensoryFromBrain()
    sendToExternalSystem(brainOutput)  # Your external system
```

## 11. Batch Processing and Scripts

### Batch Processing Script

```python
#!/usr/bin/env python3
"""
NLM Batch Processing Script

Run multiple simulations with different configurations and collect results.
"""

import pynlm
import time
import json
import os

def runSingleSimulation(configName, config):
    """Run a single simulation with given configuration"""
    print(f"Running simulation: {configName}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    startTime = time.time()
    totalReward = 0.0
    
    for step in range(config.get("maxSteps", 1000)):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.computeReward(brain.getRegions()[0].get())
        totalReward += reward
    
    endTime = time.time()
    
    # Collect results
    results = {
        "config": configName,
        "total_reward": totalReward,
        "avg_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "duration": endTime - startTime,
        "final_dopamine": agent.getNeuromodulationLevel(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
    }
    
    print(f"  Completed: {step + 1} steps, Total reward: {totalReward:.2f}")
    return results

def main():
    """Main batch processing function"""
    # Define different configurations
    configurations = {
        "simple": {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.1,
            "maxSteps": 1000,
        },
        "complex": {
            "neuron_count": 2000,
            "region_count": 3,
            "connection_probability": 0.05,
            "maxSteps": 2000,
        },
        "highly_plastic": {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.2,
            "maxSteps": 1500,
        },
    }
    
    # Run all configurations
    allResults = []
    
    for configName, configDict in configurations.items():
        # Create config object
        config = pynlm.createDefaultConfig()
        
        # Set parameters
        config.set("neuron_count", configDict["neuron_count"])
        config.set("region_count", configDict["region_count"])
        config.set("connection_probability", configDict["connection_probability"])
        
        # Run simulation
        results = runSingleSimulation(configName, config)
        allResults.append(results)
    
    # Save results
    outputFile = "nlm_batch_results.json"
    with open(outputFile, 'w') as f:
        json.dump(allResults, f, indent=2)
    
    print(f"\nResults saved to: {outputFile}")
    
    # Print summary
    print("\n=== Batch Processing Summary ===")
    for result in allResults:
        print(f"{result['config']}: Reward {result['total_reward']:.2f}, "
              f"Firing rate {result['avg_firing_rate']:.2f}, "
              f"Time {result['duration']:.2f}s")

if __name__ == "__main__":
    main()
```

## 12. Performance Profiling

### Basic Performance Profiling

```python
import cProfile
import pstats
import io

def profileSimulation(brain, world, agent, numSteps):
    """Profile simulation performance"""
    
    def runSimulation():
        for step in range(numSteps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    # Profile the simulation
    pr = cProfile.Profile()
    pr.enable()
    runSimulation()
    pr.disable()
    
    # Print statistics
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats()
    
    print(s.getvalue())

# Use profiler
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

profileSimulation(brain, world, agent, 1000)
```

## 13. Advanced Configuration Options

### Configuration File Template

```python
# advanced_config.cfg

# Neural configuration
neuron_count = 3000
region_count = 4
connection_probability = 0.08

# Plasticity configuration
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0

# Development configuration
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002
development_stage_critical = 120.0
development_stage_maturation = 600.0

# Neuromodulation configuration
dopamine_baseline = 0.1
dopamine_sensitivity = 1.5
curiosity_decay_rate = 0.98
novelty_threshold = 0.3

# Memory configuration
working_memory_capacity = 1000
working_memory_decay_rate = 0.01
episodic_memory_max_episodes = 10000

# Simulation configuration
max_simulation_steps = 50000
simulation_timestep = 0.001
checkpoint_interval = 1000
replay_interval = 500
consolidation_interval = 5000

# World configuration
environment_name = "AdvancedGridWorld"
environment_width = 20
environment_height = 20
vision_width = 12
vision_height = 12

# Logging configuration
log_level = "DEBUG"
log_to_file = true
log_filename = "nlm_advanced.log"
log_rotation_size = 10485760  # 10MB
```

### Load Advanced Configuration

```python
import pynlm

# Load advanced configuration
config = pynlm.loadConfigFromFile("advanced_config.cfg")

# Create brain with advanced config
brain = pynlm.createBrain(config)
brain.initialize()

# Alternatively, apply advanced settings to default config
config = pynlm.createDefaultConfig()

# Apply advanced settings
config.set("neuron_count", 3000)
config.set("region_count", 4)
config.set("connection_probability", 0.08)
config.set("development_synaptogenesis_rate", 0.002)
config.set("development_pruning_rate", 0.0002)

brain = pynlm.createBrain(config)
brain.initialize()
```

## 14. Troubleshooting Common Issues

### Issue 1: Brain Not Learning

**Problem**: Brain firing rates are zero or learning doesn't occur.

**Solution**:

```python
def debugLearningIssue(brain, world, agent):
    # Check if brain is properly initialized
    if not brain or brain.getTotalNeuronCount() == 0:
        print("ERROR: Brain not properly initialized")
        return
    
    # Check for sensory input
    percept = world.getSensoryPercept()
    if all(abs(x) < 0.001 for x in percept.getVision()):
        print("WARNING: No visual input detected")
        # Check world configuration
        print(f"World size: {world.getWidth()}x{world.getHeight()}")
    
    # Check brain connectivity
    totalNeurons = brain.getTotalNeuronCount()
    totalSynapses = brain.getTotalSynapseCount()
    print(f"Brain: {totalNeurons} neurons, {totalSynapses} synapses")
    
    if totalSynapses == 0:
        print("ERROR: No synapses - connectivity issue")
        return
    
    # Check plasticity systems
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    
    if stdp:
        print(f"STDP LTP: {stdp.getLTPWeight()}, LTD: {stdp.getLTDWeight()}")
    
    # Run a few steps and monitor
    print("\nRunning debug steps...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Check for activity
        firingRate = brain.getAverageFiringRate()
        if firingRate > 0:
            print(f"Step {step}: Firing rate {firingRate:.2f} - OK")
        else:
            print(f"Step {step}: No firing - checking parameters")
            
            # Check neuron parameters
            region = brain.getRegion(brain.getRegionIds()[0])
            if region:
                neurons = region.getAllNeurons()
                for i, neuron in enumerate(neurons[:5]):  # Check first 5 neurons
                    state = neuron.getState()
                    print(f"  Neuron {i}: V={state.membranePotential:.2f}, "
                          f"thresh={state.threshold:.2f}, "
                          f"refractory={state.refractoryRemaining}")

# Use debugger
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=8, height=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

debugLearningIssue(brain, world, agent)
```

### Issue 2: Memory Systems Not Working

**Problem**: Working memory or episodic memory not storing information.

**Solution**:

```python
def debugMemoryIssues(brain):
    # Check working memory
    workingMem = brain.getWorkingMemory()
    if workingMem:
        print(f"Working memory capacity: {workingMem.getCapacity()}")
        print(f"Working memory decay rate: {workingMem.getDecayRate()}")
        
        # Store a test pattern
        testPattern = [0.5, 0.7, 0.3, 0.9, 0.2]
        workingMem.store(testPattern, 1.0)
        
        print(f"After storing pattern: {workingMem.getActiveTraces()} traces")
        print(f"Memory activity: {workingMem.getMemoryActivity():.3f}")
        
        # Check neurons in memory
        neurons = workingMem.getMemoryNeurons()
        print(f"Neurons stored in memory: {len(neurons)}")
        
        for i, neuron in enumerate(neurons[:3]):
            activation = workingMem.getNeuronActivation(neuron)
            print(f"  Neuron {i} activation: {activation:.3f}")
    
    else:
        print("ERROR: Working memory not available")
    
    # Check episodic memory
    episodicMem = brain.getEpisodicMemory()
    if episodicMem:
        print(f"Episodic memory max episodes: {episodicMem.getMaxEpisodes()}")
        
        # Check if any episodes exist
        episodes = episodicMem.getEpisodeCount()
        print(f"Current episodes: {episodes}")
        
        if episodes == 0:
            print("No episodes found - checking episode storage conditions")
            
            # Run a few steps to generate experience
            for step in range(20):
                brain.step(step)
                
                # Check if new episodes were created
                newEpisodes = episodicMem.getEpisodeCount()
                if newEpisodes > episodes:
                    print(f"New episode created at step {step}")
                    episodes = newEpisodes
    
    else:
        print("ERROR: Episodic memory not available")

# Use memory debugger
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
debugMemoryIssues(brain)
```

### Issue 3: Checkpoint Failures

**Problem**: Save/load operations failing.

**Solution**:

```python
def debugCheckpointIssues(brain):
    import tempfile
    import os
    
    print("Testing checkpoint functionality...")
    
    # Create temporary directory
    with tempfile.TemporaryDirectory() as tempDir:
        testPath = os.path.join(tempDir, "test_checkpoint.bin")
        
        # Test 1: Basic save
        print("Test 1: Saving brain state...")
        if brain.save(testPath):
            print("✓ Save successful")
            
            # Check file size
            fileSize = os.path.getsize(testPath)
            print(f"  File size: {fileSize} bytes")
            
            if fileSize == 0:
                print("WARNING: Empty checkpoint file")
        
        else:
            print("✗ Save failed")
            return
        
        # Test 2: Load into new brain
        print("\nTest 2: Loading brain state...")
        
        # Create new brain
        newBrain = pynlm.createBrain(brain.getConfig())
        newBrain.initialize()
        
        if newBrain.load(testPath):
            print("✓ Load successful")
            
            # Compare basic statistics
            originalNeurons = brain.getTotalNeuronCount()
            loadedNeurons = newBrain.getTotalNeuronCount()
            
            print(f"Original: {originalNeurons} neurons")
            print(f"Loaded: {loadedNeurons} neurons")
            
            if originalNeurons != loadedNeurons:
                print("WARNING: Neuron count mismatch")
        
        else:
            print("✗ Load failed")
        
        # Test 3: Round-trip test
        print("\nTest 3: Round-trip test...")
        
        # Save, create new brain, load
        if brain.save(testPath):
            brain2 = pynlm.createBrain(brain.getConfig())
            brain2.initialize()
            
            if brain2.load(testPath):
                print("✓ Round-trip test passed")
            else:
                print("✗ Round-trip test failed")
        
        else:
            print("✗ Could not save for round-trip test")

# Use checkpoint debugger
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps first
for step in range(100):
    brain.step(step)

debugCheckpointIssues(brain)
```

## 15. Frequently Asked Questions

### Q: How do I enable development stages?

**A**: Development is enabled by default. You can configure it through the config:

```python
config = pynlm.createDefaultConfig()
config.set("enable_development", True)

# Or set specific development stages
config.set("development_stage_initial", 0.0)
config.set("development_stage_critical", 60.0)
config.set("development_stage_maturation", 300.0)
```

### Q: How do I configure neuromodulation?

**A**: Neuromodulation systems are enabled by default:

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable specific neuromodulators
agent.enableRewardModulation(True)  # Dopamine
agent.enableCuriosity(True)        # Curiosity
agent.enableNoveltyDetection(True)  # Novelty
```

### Q: How do I optimize for performance?

**A**: Consider these optimization strategies:

1. **Use smaller configurations** for testing
2. **Disable unnecessary systems**:
   ```python
   agent.enableRewardModulation(False)  # If reward not needed
   agent.enableCuriosity(False)        # If exploration not needed
   ```
3. **Use checkpointing** to save state and resume later
4. **Monitor performance** and adjust parameters

### Q: How do I monitor simulation progress?

**A**: Use the NLMMonitor class or integrate logging:

```python
import time

startTime = time.time()
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        elapsed = time.time() - startTime
        print(f"Step {step}, {elapsed:.2f}s elapsed, "
              f"Firing rate {brain.getAverageFiringRate():.2f}")
```

## 16. API Reference

### Core Classes

- **Brain**: Main neural system with all integrated components
- **AgentBrain**: Interface between brain and environment
- **SimpleWorld**: Basic environment for testing
- **SensoryPercept**: Sensory data from world
- **Action**: Motor commands to world

### Configuration

- **Config**: Configuration management
- **Phase6Config**: Configuration for Phase 6 experiments

### Memory Systems

- **NeuralWorkingMemory**: Short-term memory using neural dynamics
- **NeuralEpisodicMemory**: Long-term memory of experiences
- **NeuralAssociativeMemory**: Pattern associations

### Neuromodulation

- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Prediction error signals

### Cognition

- **NeuralPlanner**: Action planning and decision making
- **ConceptFormation**: Pattern discovery and abstraction
- **AttentionalSelection**: Neural attention and focus

### Development

- **DevelopmentSystem**: Developmental stages and maturation
- **DevelopmentStage**: Developmental stage enum

### Performance

- **CheckpointManager**: Save/load functionality
- **Logger**: Logging system

## 17. Examples Repository

See the examples directory for complete working examples:

- `examples/basic_usage.py` - Basic NLM usage
- `examples/agent_simulation.py` - Agent-environment interaction
- `examples/memory_usage.py` - Memory system usage
- `examples/development_tracking.py` - Development monitoring
- `examples/checkpointing.py` - Checkpoint save/load
- `examples/learning_curves.py` - Learning progress tracking

## 18. Additional Resources

- **Documentation**: `docs/ARCHITECTURE.md` - Detailed architecture information
- **Science**: `docs/SCIENCE.md` - Scientific background and principles
- **Roadmap**: `docs/ROADMAP.md` - Project development roadmap
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions
- **Limitations**: `docs/LIMITATIONS.md` - Known limitations and future work
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Current integration status

## 19. Getting Help

For additional help:

1. **GitHub Issues**: Report bugs and request features
2. **Documentation**: Check the comprehensive documentation
3. **Examples**: Study the provided examples
4. **Community**: Join the NLM community for discussions

## 20. Contributing

To contribute to NLM:

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement your changes**
4. **Add tests and documentation**
5. **Submit a pull request**

NLM is an open-source research project. Contributions are welcome!

---

**Version**: 0.1.0  
**Phase**: 6 - Final Integration  
**License**: MIT  
**Copyright**: 2026 NLM Authors
EOF