# Advanced NLM Usage Examples

## Overview

This document provides advanced usage examples for the NLM framework, covering complex simulation scenarios and feature combinations. These examples build upon the basic concepts from `easy_usage.md` and demonstrate how to leverage NLM's full capabilities for sophisticated neural simulations.

## Example 1: Advanced Agent with Learning and Development

### Description
A complete agent that learns through reward modulation, develops over time, and exhibits curiosity-driven exploration.

```python
import pynlm
import time

def create_advanced_agent_config():
    """Create a configuration optimized for learning and development."""
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.regionCount", 4)
    config.set("brain.plasticity.enableSTDP", True)
    config.set("brain.plasticity.enableHebbian", True)
    config.set("brain.development.enableSynaptogenesis", True)
    config.set("brain.development.enablePruning", True)
    config.set("brain.development.enableMaturation", True)
    
    # World configuration
    config.set("world.width", 50.0)
    config.set("world.height", 50.0)
    config.set("world.visionWidth", 16)
    config.set("world.visionHeight", 16)
    config.set("world.maxEnergy", 100.0)
    
    # Agent configuration
    config.set("agent.rewardModulation.enable", True)
    config.set("agent.curiosity.enable", True)
    config.set("agent.novelty.enable", True)
    config.set("agent.prediction.enable", True)
    
    return config

def run_advanced_learning_simulation():
    """Run an advanced simulation with learning and development."""
    # Create components
    config = create_advanced_agent_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(50.0, 50.0, 16, 16)
    world.setAgentStart(25.0, 25.0)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Place some resources and hazards
    import random
    for _ in range(20):
        x = random.uniform(5.0, 45.0)
        y = random.uniform(5.0, 45.0)
        if random.random() < 0.7:
            obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0)
        else:
            obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-5.0)
        world.addObject(obj)
    
    # Create walls around the perimeter
    for x in [0, 50]:
        for y in range(0, 51, 2):
            obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall)
            world.addObject(obj)
    for y in [0, 50]:
        for x in range(0, 51, 2):
            obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall)
            world.addObject(obj)
    
    print("Starting advanced learning simulation...")
    print(f"Initial state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    stats = {
        'steps': 0,
        'totalReward': 0.0,
        'noveties': 0,
        'episodes': 0
    }
    
    maxSteps = 1000
    episodeLength = 100
    
    for step in range(maxSteps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Agent action with curiosity
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = result.reward if hasattr(result, 'reward') else 0.0
        stats['totalReward'] += reward
        stats['episodes'] += 1 if step % episodeLength == 0 else 0
        
        # Check for novel experiences (curiosity)
        if agent.getNoveltyLevel() > 0.5:
            stats['noveties'] += 1
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Action={action}, Reward={reward:.2f}, "
                  f"Neuromod={agent.getNeuromodulationLevel():.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}")
    
    print(f"\nSimulation completed!")
    print(f"Total steps: {maxSteps}")
    print(f"Total reward: {stats['totalReward']:.2f}")
    print(f"Episodes: {stats['episodes']}")
    print(f"Novel experiences: {stats['noveties']}")
    print(f"Final neuron count: {brain.getTotalNeuronCount()}")
    print(f"Final synapse count: {brain.getTotalSynapseCount()}")
    
    return stats

if __name__ == "__main__":
    stats = run_advanced_learning_simulation()
```

## Example 2: Custom Plasticity Rules and Structural Learning

### Description
Demonstrates custom plasticity implementation with structural synaptic changes and Hebbian-STDP combination.

```python
import pynlm
import math

def create_plasticity_config():
    """Create configuration with advanced plasticity rules."""
    config = pynlm.createDefaultConfig()
    
    # Enable advanced plasticity
    config.set("brain.plasticity.enableSTDP", True)
    config.set("brain.plasticity.enableHebbian", True)
    config.set("brain.plasticity.enableStructural", True)
    config.set("brain.plasticity.stdp.alpha", 0.01)
    config.set("brain.plasticity.stdp.eta_pre", 0.005)
    config.set("brain.plasticity.stdp.eta_post", 0.005)
    config.set("brain.plasticity.hebbian.learningRate", 0.02)
    
    # Structural plasticity parameters
    config.set("brain.plasticity.structural.maxSynapses", 10000)
    config.set("brain.plasticity.structural.growthRate", 0.001)
    config.set("brain.plasticity.structural.pruningRate", 0.005)
    
    return config

def run_plasticity_experiment():
    """Run experiment with custom plasticity rules."""
    config = create_plasticity_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add regions for different plasticity types
    region1 = brain.addRegion("SensoryCortex")
    region2 = brain.addRegion("MotorCortex")
    region3 = brain.addRegion("Associative")
    
    # Create world with varied stimuli
    world = pynlm.createSimpleWorld()
    world.configure(30.0, 30.0, 12, 12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableStructuralPlasticity(True)
    
    # Create stimulation pattern
    stimulationPattern = []
    for i in range(100):
        # Create correlated sensory input
        pattern = [0.0] * 144  # 12x12 vision
        center_x, center_y = 6 + (i % 12), 6 + ((i * 2) % 12)
        
        # Gaussian patch around center
        for dx in [-3, -2, -1, 0, 1, 2, 3]:
            for dy in [-3, -2, -1, 0, 1, 2, 3]:
                x = center_x + dx
                y = center_y + dy
                if 0 <= x < 12 and 0 <= y < 12:
                    dist = math.sqrt(dx*dx + dy*dy)
                    strength = math.exp(-dist * dist / 2.0)
                    pattern[y * 12 + x] = strength
        
        stimulationPattern.append(pattern)
    
    print("Running plasticity experiment...")
    print(f"Initial synapses: {brain.getTotalSynapseCount()}")
    
    # Run stimulation
    for step in range(500):
        # Apply correlated stimulation
        if step < 100:
            percept = pynlm.SensoryPercept()
            vision = pynlm.Vision(12, 12, 3)
            vision.setData(stimulationPattern[step])
            percept.setVision(vision)
            agent.processSensoryInput(percept)
        else:
            # Random stimulation
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Print statistics
        if step % 100 == 0:
            stdp = brain.getSTDP()
            hebbian = brain.getHebbian()
            structural = brain.getStructuralPlasticity()
            
            print(f"Step {step}:")
            print(f"  Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            if stdp:
                print(f"  STDP weight: {stdp.getAverageWeight():.4f}")
            if structural:
                print(f"  Structural plasticity active: {structural.isActive()}")
    
    print(f"\nFinal synapse count: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    
    return brain

if __name__ == "__main__":
    final_brain = run_plasticity_experiment()
```

## Example 3: Memory System Integration and Episodic Learning

### Description
Demonstrates integration of working memory, episodic memory, and associative memory systems.

```python
import pynlm
import numpy as np

def create_memory_config():
    """Create configuration with memory systems enabled."""
    config = pynlm.createDefaultConfig()
    
    # Enable memory systems
    config.set("brain.memory.working.enable", True)
    config.set("brain.memory.episodic.enable", True)
    config.set("brain.memory.associative.enable", True)
    config.set("brain.memory.working.capacity", 1000)
    config.set("brain.memory.episodic.maxEpisodes", 500)
    config.set("brain.memory.associative.patterns", 200)
    
    return config

def run_memory_integration_experiment():
    """Run experiment with memory systems integration."""
    config = create_memory_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(40.0, 40.0, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Starting memory integration experiment...")
    
    # Get memory systems
    workingMemory = brain.getWorkingMemory()
    episodicMemory = brain.getEpisodicMemory()
    associativeMemory = brain.getAssociativeMemory()
    
    # Create structured exploration pattern
    explorationData = []
    for episode in range(50):
        episodeData = {
            'vision': [],
            'action': None,
            'reward': 0.0,
            'position': (0.0, 0.0)
        }
        
        # Create episode with pattern
        for step in range(20):
            # Create simple pattern based on episode and step
            pattern = np.zeros(64)  # 8x8 vision
            
            # Create spatial pattern
            center_x = 4 + (step * 0.3) % 8
            center_y = 4 + (episode * 0.5) % 8
            
            for i in range(8):
                for j in range(8):
                    dist = np.sqrt((i - center_x)**2 + (j - center_y)**2)
                    pattern[i * 8 + j] = np.exp(-dist * dist / 2.0)
            
            episodeData['vision'].append(pattern.tolist())
            
            # Simulate world update
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(episode * 20 + step)
        
        explorationData.append(episodeData)
    
    print(f"Exploration completed with {len(explorationData)} episodes")
    
    # Analyze working memory
    if workingMemory:
        print(f"Working memory capacity: {workingMemory.getCapacity()}")
        print(f"Working memory usage: {workingMemory.getUsage()}")
    
    # Analyze episodic memory
    if episodicMemory:
        print(f"Episodic memory episodes stored: {episodicMemory.getEpisodeCount()}")
        print(f"Episodic memory patterns: {episodicMemory.getPatternCount()}")
    
    # Analyze associative memory
    if associativeMemory:
        print(f"Associative memory patterns: {associativeMemory.getPatternCount()}")
        print(f"Associative memory strength: {associativeMemory.getAverageStrength():.4f}")
    
    # Test pattern completion (association)
    if associativeMemory and episodicMemory:
        print("\nTesting pattern completion...")
        
        # Get last episode's final pattern
        lastPattern = explorationData[-1]['vision'][-1]
        
        # Find similar patterns
        similar = associativeMemory.findSimilarPatterns(lastPattern, threshold=0.7)
        print(f"Found {len(similar)} similar patterns for completion")
    
    return {
        'brain': brain,
        'workingMemory': workingMemory,
        'episodicMemory': episodicMemory,
        'associativeMemory': associativeMemory
    }

if __name__ == "__main__":
    memoryResults = run_memory_integration_experiment()
```

## Example 4: Neuromodulation and Prediction System

### Description
Demonstrates advanced neuromodulation with dopamine, novelty, and prediction error systems.

```python
import pynlm

def create_neuromodulation_config():
    """Create configuration with advanced neuromodulation."""
    config = pynlm.createDefaultConfig()
    
    # Enable neuromodulation systems
    config.set("brain.neuromodulation.dopamine.enable", True)
    config.set("brain.neuromodulation.novelty.enable", True)
    config.set("brain.neuromodulation.curiosity.enable", True)
    config.set("brain.prediction.enable", True)
    config.set("brain.prediction.horizon", 10)
    
    return config

def run_neuromodulation_experiment():
    """Run experiment with neuromodulation and prediction."""
    config = create_neuromodulation_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(35.0, 35.0, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get neuromodulation systems
    dopamine = brain.getDopamine()
    novelty = brain.getNovelty()
    curiosity = brain.getCuriosity()
    predictionError = brain.getPredictionErrorSignal()
    
    print("Starting neuromodulation experiment...")
    
    # Create a target pattern for prediction
    targetPattern = np.random.randn(100)  # 10x10 vision
    targetPattern = (targetPattern - targetPattern.mean()) / (targetPattern.std() + 1e-8)
    
    # Create prediction system
    predictionSystem = brain.getPredictionSystem()
    if predictionSystem:
        predictionSystem.setTargetPattern(targetPattern)
    
    # Run experiment
    predictionHistory = []
    neuromodulationHistory = []
    
    for step in range(300):
        # Create sensory input (sometimes similar to target)
        if step % 3 == 0:  # 1/3 chance of target-like input
            inputPattern = targetPattern + np.random.randn(100) * 0.1
        else:
            # Random input
            inputPattern = np.random.randn(100)
        
        percept = pynlm.SensoryPercept()
        vision = pynlm.Vision(10, 10, 3)
        vision.setData(inputPattern.tolist())
        percept.setVision(vision)
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get predictions
        prediction = predictionSystem.getPrediction() if predictionSystem else None
        prediction_error = predictionError.getLevel() if predictionError else 0.0
        
        predictionHistory.append(prediction_error)
        neuromodulationHistory.append({
            'dopamine': dopamine.getLevel() if dopamine else 0.0,
            'novelty': novelty.getLevel() if novelty else 0.0,
            'curiosity': curiosity.getLevel() if curiosity else 0.0
        })
        
        # Apply reward based on prediction error
        reward = prediction_error * 10.0
        agent.applyRewardModulation(reward, prediction_error * 0.5)
        
        # Print neuromodulation state
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Prediction error: {prediction_error:.4f}")
            print(f"  Dopamine level: {dopamine.getLevel() if dopamine else 0.0:.4f}")
            print(f"  Novelty level: {novelty.getLevel() if novelty else 0.0:.4f}")
            print(f"  Curiosity level: {curiosity.getLevel() if curiosity else 0.0:.4f}")
    
    # Analyze results
    avgPredictionError = np.mean(predictionHistory)
    avgDopamine = np.mean([h['dopamine'] for h in neuromodulationHistory])
    avgNovelty = np.mean([h['novelty'] for h in neuromodulationHistory])
    avgCuriosity = np.mean([h['curiosity'] for h in neuromodulationHistory])
    
    print(f"\nNeuromodulation analysis:")
    print(f"Average prediction error: {avgPredictionError:.4f}")
    print(f"Average dopamine level: {avgDopamine:.4f}")
    print(f"Average novelty level: {avgNovelty:.4f}")
    print(f"Average curiosity level: {avgCuriosity:.4f}")
    
    return {
        'predictionHistory': predictionHistory,
        'neuromodulationHistory': neuromodulationHistory
    }

if __name__ == "__main__":
    neuromodResults = run_neuromodulation_experiment()
```

## Example 5: Development System and Phase Transitions

### Description
Demonstrates brain development with phase transitions and maturation.

```python
import pynlm

def create_development_config():
    """Create configuration with development system."""
    config = pynlm.createDefaultConfig()
    
    # Enable development
    config.set("brain.development.enable", True)
    config.set("brain.development.criticalPeriod", True)
    config.set("brain.development.maturation", True)
    config.set("brain.development.pruning", True)
    config.set("brain.development.synaptogenesis", True)
    
    return config

def run_development_experiment():
    """Run experiment with brain development."""
    config = create_development_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(25.0, 25.0, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get development system
    developmentSystem = brain.getDevelopmentSystem()
    
    print("Starting brain development experiment...")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Run developmental phases
    phaseDurations = {
        'initial': 100,
        'criticalPeriod': 200,
        'maturation': 300,
        'adult': 500
    }
    
    developmentalHistory = []
    
    currentPhase = 'initial'
    stepsInPhase = 0
    maxPhaseSteps = phaseDurations[currentPhase]
    
    for step in range(800):
        # Update development
        agent.updateDevelopment(0.1)
        
        # Get current state
        stage = brain.getDevelopmentalStage()
        synapticDensity = developmentSystem.getSynapticDensity() if developmentSystem else 0.0
        neuronCount = brain.getTotalNeuronCount()
        
        developmentalHistory.append({
            'step': step,
            'phase': currentPhase,
            'stage': str(stage),
            'synapticDensity': synapticDensity,
            'neuronCount': neuronCount
        })
        
        # Change phase if time
        if stepsInPhase >= maxPhaseSteps:
            # Cycle through phases
            phases = list(phaseDurations.keys())
            currentPhase = phases[(phases.index(currentPhase) + 1) % len(phases)]
            maxPhaseSteps = phaseDurations[currentPhase]
            stepsInPhase = 0
            
            print(f"\nTransitioning to '{currentPhase}' phase!")
            print(f"Developmental stage: {brain.getDevelopmentalStage()}")
        
        stepsInPhase += 1
        
        # Update world and agent
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Phase: {currentPhase}")
            print(f"  Stage: {brain.getDevelopmentalStage()}")
            print(f"  Neurons: {neuronCount}")
            print(f"  Synaptic density: {synapticDensity:.4f}")
    
    # Analyze development
    print(f"\nDevelopment analysis:")
    print(f"Total steps: {len(developmentalHistory)}")
    print(f"Initial neurons: {developmentalHistory[0]['neuronCount']}")
    print(f"Final neurons: {developmentalHistory[-1]['neuronCount']}")
    print(f"Neuron growth: {developmentalHistory[-1]['neuronCount'] - developmentalHistory[0]['neuronCount']}")
    
    # Check for critical period effects
    criticalPeriodEffects = [d for d in developmentalHistory if d['phase'] == 'criticalPeriod']
    if criticalPeriodEffects:
        print(f"Critical period phase had {len(criticalPeriodEffects)} steps")
    
    return developmentalHistory

if __name__ == "__main__":
    developmentHistory = run_development_experiment()
```

## Example 6: Performance Tuning and Optimization

### Description
Demonstrates advanced performance tuning techniques and optimization strategies.

```python
import pynlm
import time

def create_performance_config():
    """Create configuration optimized for performance."""
    config = pynlm.createDefaultConfig()
    
    # Performance optimizations
    config.set("brain.performance.enableSIMD", True)
    config.set("brain.performance.enableParallel", True)
    config.set("brain.performance.enableCaching", True)
    config.set("brain.performance.memoryPool.enable", True)
    config.set("brain.performance.eventQueue.enable", True)
    
    # Memory optimizations
    config.set("brain.performance.memoryPool.maxSize", 10000)
    config.set("brain.performance.eventQueue.maxSize", 1000)
    
    # Neural optimizations
    config.set("brain.plasticity.sparseConnectivity.enable", True)
    config.set("brain.plasticity.sparseConnectivity.threshold", 0.1)
    
    return config

def run_performance_optimization_experiment():
    """Run experiment with performance optimizations."""
    config = create_performance_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(60.0, 60.0, 20, 20)  # Larger world for more computation
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get performance systems
    performance = brain.getPerformance()
    
    print("Starting performance optimization experiment...")
    print(f"Initial performance mode: {performance.getOptimizationMode() if performance else 'Default'}")
    
    # Run performance benchmark
    timings = []
    spikeCounts = []
    memoryUsage = []
    
    numSteps = 500
    warmupSteps = 50
    
    for step in range(numSteps):
        stepStart = time.time()
        
        # Run simulation
        world.update(0.05)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        stepEnd = time.time()
        stepTime = stepEnd - stepStart
        
        # Collect metrics
        if step >= warmupSteps:
            timings.append(stepTime)
            spikeCounts.append(brain.getTotalSpikeCount())
        
        # Get memory usage if available
        if performance:
            # Note: Actual memory measurement depends on performance system API
            memoryUsage.append(0.0)  # Placeholder
        
        # Print progress
        if step % 100 == 0:
            avgTime = np.mean(timings) if timings else 0.0
            avgSpikes = np.mean(spikeCounts) if spikeCounts else 0.0
            print(f"Step {step}:")
            print(f"  Step time: {stepTime:.4f}s")
            print(f"  Average step time: {avgTime:.4f}s")
            print(f"  Average spikes: {avgSpikes:.0f}")
    
    # Performance analysis
    if timings:
        avgStepTime = np.mean(timings)
        stdStepTime = np.std(timings)
        minStepTime = np.min(timings)
        maxStepTime = np.max(timings)
        
        print(f"\nPerformance analysis:")
        print(f"Average step time: {avgStepTime:.4f}s (±{stdStepTime:.4f}s)")
        print(f"Min step time: {minStepTime:.4f}s")
        print(f"Max step time: {maxStepTime:.4f}s")
        print(f"Throughput: {1.0 / avgStepTime:.2f} steps/second")
        
        if performance:
            print(f"Performance mode: {performance.getOptimizationMode()}")
            print(f"SIMD enabled: {performance.isSIMDEnabled()}")
            print(f"Parallel processing: {performance.isParallelEnabled()}")
    
    return {
        'timings': timings,
        'spikeCounts': spikeCounts,
        'memoryUsage': memoryUsage,
        'performance': performance
    }

if __name__ == "__main__":
    perfResults = run_performance_optimization_experiment()
```

## Example 7: Experiment Design and Data Collection

### Description
Demonstrates how to design and run experiments with proper data collection and analysis.

```python
import pynlm
import json
from datetime import datetime

def create_experiment_config():
    """Create configuration for experimental data collection."""
    config = pynlm.createDefaultConfig()
    
    # Enable experiment recording
    config.set("experiment.enableRecording", True)
    config.set("experiment.recordBatched", True)
    config.set("experiment.recordCompressed", True)
    
    # Performance monitoring
    config.set("performance.monitoring.enable", True)
    config.set("performance.monitoring.sampleRate", 10)  # Hz
    
    return config

def run_experiments_suite():
    """Run a suite of experiments with proper data collection."""
    config = create_experiment_config()
    
    experiments = []
    
    # Experiment 1: Simple exploration
    exp1_config = pynlm.createDefaultConfig()
    exp1_config.set("world.width", 30.0)
    exp1_config.set("world.height", 30.0)
    exp1_config.set("world.visionWidth", 8)
    exp1_config.set("world.visionHeight", 8)
    
    experiments.append({
        'name': 'SimpleExploration',
        'config': exp1_config,
        'duration': 200,
        'description': 'Basic exploration of simple world'
    })
    
    # Experiment 2: Learning with reward
    exp2_config = pynlm.createDefaultConfig()
    exp2_config.set("world.width", 40.0)
    exp2_config.set("world.height", 40.0)
    exp2_config.set("agent.rewardModulation.enable", True)
    exp2_config.set("agent.curiosity.enable", True)
    
    experiments.append({
        'name': 'RewardLearning',
        'config': exp2_config,
        'duration': 400,
        'description': 'Learning with reward and curiosity'
    })
    
    # Experiment 3: Development
    exp3_config = pynlm.createDefaultConfig()
    exp3_config.set("brain.development.enable", True)
    exp3_config.set("brain.development.criticalPeriod", True)
    
    experiments.append({
        'name': 'BrainDevelopment',
        'config': exp3_config,
        'duration': 600,
        'description': 'Brain development with phase transitions'
    })
    
    # Run experiments
    results = {}
    startTime = datetime.now()
    
    for i, exp in enumerate(experiments):
        print(f"\nRunning experiment {i+1}/{len(experiments)}: {exp['name']}")
        print(f"Description: {exp['description']}")
        
        # Create components
        brain = pynlm.createBrain(exp['config'])
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(
            exp['config'].get("world.width", 30.0),
            exp['config'].get("world.height", 30.0),
            exp['config'].get("world.visionWidth", 8),
            exp['config'].get("world.visionHeight", 8)
        )
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features
        if exp['config'].get("agent.rewardModulation.enable", False):
            agent.enableRewardModulation(True)
        if exp['config'].get("agent.curiosity.enable", False):
            agent.enableCuriosity(True)
        if exp['config'].get("brain.development.enable", False):
            agent.enableDevelopment(True)
        
        # Run experiment
        experimentStart = datetime.now()
        dataPoints = []
        
        for step in range(exp['duration']):
            # Record data
            dataPoint = {
                'step': step,
                'time': world.getSimulationTime(),
                'neuronCount': brain.getTotalNeuronCount(),
                'firingCount': brain.getFiringNeuronCount(),
                'spikeCount': brain.getTotalSpikeCount(),
                'averageFiringRate': brain.getAverageFiringRate(),
                'stage': str(brain.getDevelopmentalStage()),
                'dopamine': agent.getNeuromodulationLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel()
            }
            
            dataPoints.append(dataPoint)
            
            # Update simulation
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
        
        experimentTime = datetime.now() - experimentStart
        
        results[exp['name']] = {
            'data': dataPoints,
            'metadata': {
                'startTime': experimentStart.isoformat(),
                'endTime': datetime.now().isoformat(),
                'duration': experimentTime.total_seconds(),
                'steps': exp['duration'],
                'finalState': {
                    'neuronCount': brain.getTotalNeuronCount(),
                    'stage': str(brain.getDevelopmentalStage())
                }
            }
        }
        
        print(f"Completed experiment {exp['name']} in {experimentTime.total_seconds():.2f}s")
        print(f"Collected {len(dataPoints)} data points")
    
    totalTime = datetime.now() - startTime
    
    # Save results
    experimentResults = {
        'experiments': experiments,
        'results': results,
        'summary': {
            'totalTime': totalTime.total_seconds(),
            'numExperiments': len(experiments),
            'totalSteps': sum(len(exp_results['data']) for exp_results in results.values())
        }
    }
    
    # Save to JSON
    with open(f"experiment_results_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json", 'w') as f:
        json.dump(experimentResults, f, indent=2)
    
    print(f"\nExperiment suite completed!")
    print(f"Total time: {totalTime.total_seconds():.2f}s")
    print(f"Results saved to experiment_results_*.json")
    
    return experimentResults

if __name__ == "__main__":
    experimentSuite = run_experiments_suite()
```

## Best Practices and Tips

### 1. Configuration Management
- Always create configurations with `createDefaultConfig()` before modifications
- Use descriptive key names for easy retrieval
- Save configurations to file after setup for reproducibility

### 2. Memory Management
- Monitor memory usage during long simulations
- Use checkpointing (`brain.save()`) for long-running experiments
- Clear configurations when no longer needed

### 3. Simulation Best Practices
- Call `brain.initialize()` before any operations
- Include `world.update(timestep)` in simulation loop
- Enable necessary features before using them
- Monitor system state regularly

### 4. Experiment Design
- Define clear experimental hypotheses
- Use consistent timestep sizes
- Record all relevant metrics
- Include warm-up phases before data collection

### 5. Performance Optimization
- Use larger timesteps for faster simulation
- Enable parallel processing when available
- Monitor and adjust optimization modes
- Consider sparse connectivity for large networks

## Further Reading

For more advanced topics, see:
- `docs/ARCHITECTURE.md` - System architecture
- `docs/RESEARCH_METHODS.md` - Research background
- `docs/PERFORMANCE.md` - Performance optimization
- `docs/LIMITATIONS.md` - Known limitations
