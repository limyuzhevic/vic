#!/usr/bin/env python3
"""
Getting Started Tutorial - Basic NLM Python Usage

This tutorial provides a step-by-step introduction to using the enhanced NLM Python bindings.
It covers basic setup, fundamental operations, and common use cases.

## Prerequisites

- Python 3.8+
- NLM Python bindings installed
- Basic understanding of neural networks and simulation

## Installation

```bash
# Install from source
pip install -e .

# Or build and install
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e ..
```

## Getting Started

### 1. Basic Setup

This section demonstrates the minimal setup required to create and use an NLM brain.

```python
"""basic_setup.py - Minimal NLM setup"""
import nlm

def basic_setup_example():
    """Create basic NLM brain and agent components"""
    print("=== Basic NLM Setup ===")
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    print("✓ Created default configuration")
    
    # Create neural brain core
    brain = nlm.createBrain(config)
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create world simulation environment
    world = nlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=32, visionHeight=32)
    print("✓ Created and configured world simulation")
    
    # Create agent brain interface
    agent = nlm.createAgentBrain(brain)
    print(f"✓ Created agent interface")
    print(f"  - Expected sensory input size: {agent.getSensoryInputSize()}")
    print(f"  - Expected motor output size: {agent.getMotorOutputSize()}")
    
    return brain, world, agent

if __name__ == "__main__":
    brain, world, agent = basic_setup_example()
```

### 2. First Simulation Steps

This example shows how to run a basic simulation with sensory input and motor output.

```python
"""first_simulation.py - First simulation steps"""
import nlm
import time

def first_simulation_example(brain, world, agent):
    """Run first basic simulation"""
    print("\n=== First Simulation ===")
    
    # Initialize agent with world
    agent.initialize(world)
    print("✓ Agent initialized with world")
    
    # Create initial sensory percept
    percept = nlm.SensoryPercept()
    
    # Set vision data (simple pattern)
    vision_data = [0.1] * 256  # 16x16 vision
    percept.setVision(vision_data)
    
    # Set touch data (agent's limbs)
    touch_data = [0.0, 0.0]  # Two touch sensors
    percept.setTouch(touch_data)
    
    print("✓ Created initial sensory percept")
    
    # Process sensory input
    start_time = time.time()
    agent.processSensoryInput(percept, False)  # No novelty computation
    processing_time = time.time() - start_time
    
    print(f"✓ Processed sensory input in {processing_time*1000:.2f}ms")
    
    # Get motor command from brain activity
    motor_command = agent.decodeMotorCommand()
    print(f"✓ Decoded motor command: {motor_command}")
    
    # Apply action to world
    action_result = world.applyMotorCommand(motor_command, time.time())
    print(f"✓ Applied action to world")
    print(f"  - Success: {action_result.success}")
    print(f"  - Reward: {action_result.reward}")
    print(f"  - Message: {action_result.message}")
    
    return action_result

if __name__ == "__main__":
    brain, world, agent = basic_setup_example()
    result = first_simulation_example(brain, world, agent)
```

### 3. Episode Management

Learn how to manage learning episodes and track progress.

```python
"""episode_management.py - Episode-based learning"""
import nlm
import time

def episode_management_example(brain, world, agent):
    """Demonstrate episode management features"""
    print("\n=== Episode Management ===")
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning features
    agent.enableLearning(True)
    agent.setLearningRate(0.01)
    agent.setExplorationRate(0.1)
    agent.enableCuriosity(True)
    print("✓ Enabled learning and curiosity systems")
    
    # Create percept factory function
    def create_percept(step):
        """Generate perceptual input based on step"""
        percept = nlm.SensoryPercept()
        
        # Vision: create patterns that change over time
        vision_data = []
        for i in range(256):
            if step % 10 == 0:  # Occasional pattern change
                vision_data.append(0.5 + 0.3 * (i % 16) / 16.0)
            else:
                vision_data.append(0.1)
        
        percept.setVision(vision_data)
        
        # Touch: random movement patterns
        touch_data = [0.2 if step % 5 == i else 0.0 for i in range(2)]
        percept.setTouch(touch_data)
        
        # Internal: learning signals
        internal_data = [agent.getCuriosityLevel(), agent.getNoveltyLevel()]
        percept.setInternal(internal_data)
        
        return percept
    
    # Run learning episode
    agent.startEpisode(episodeId=0, episodeType="tutorial")
    print(f"✓ Started episode 0 (type: tutorial)")
    
    max_steps = 20
    total_reward = 0.0
    
    for step in range(max_steps):
        # Create and process sensory input
        percept = create_percept(step)
        agent.processSensoryInput(percept, True)  # Compute novelty
        
        # Get action from brain
        motor_command = agent.decodeMotorCommand()
        
        # Apply to world
        action_result = world.applyMotorCommand(motor_command, time.time())
        
        # Record episode with reward
        reward = action_result.reward + 0.1  # Small exploration bonus
        agent.recordEpisodeAction(motor_command, reward)
        total_reward += reward
        
        # End episode based on conditions
        if step % 10 == 9 or step == max_steps - 1:
            agent.endEpisode(reward)
            print(f"✓ Ended episode at step {step + 1}")
            break
    
    print(f"✓ Episode completed")
    print(f"  - Total reward: {total_reward:.2f}")
    print(f"  - Episode score: {agent.getEpisodeScore():.2f}")
    print(f"  - Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Final novelty: {agent.getNoveltyLevel():.3f}")
    
    return agent

if __name__ == "__main__":
    brain, world, agent = basic_setup_example()
    episode_agent = episode_management_example(brain, world, agent)
```

### 4. Development Stages

Understand how the brain develops over time and learning changes across stages.

```python
"""development_stages.py - Developmental progression"""
import nlm
import time

def development_stages_example(brain, world, agent):
    """Demonstrate developmental stage progression"""
    print("\n=== Development Stages ===")
    
    # Initialize agent
    agent.initialize(world)
    agent.enableDevelopment(True)
    print("✓ Enabled development system")
    
    # Developmental stage progression simulation
    stages = [
        ("Initial", 0.0, 0.01),      # Very high plasticity
        ("CriticalPeriod", 50, 0.05), # High plasticity
        ("Maturation", 100, 0.02),   # Moderate plasticity
        ("Adult", 150, 0.005),       # Low plasticity
        ("Aging", 200, 0.001),       # Minimal plasticity
    ]
    
    for stage_name, stage_step, expected_plasticity in stages:
        print(f"\n--- Simulating {stage_name} stage (step {stage_step}) ---")
        
        # Set developmental stage
        brain.setDevelopmentalStage(nlm.DevelopmentalStage.AgeAdult if stage_name == "Adult" else 
                                    nlm.DevelopmentalStage.Initial if stage_name == "Initial" else
                                    nlm.DevelopmentalStage.Maturation if stage_name == "Maturation" else
                                    nlm.DevelopmentalStage.Aging if stage_name == "Aging" else
                                    nlm.DevelopmentalStage.CriticalPeriod)
        
        # Create percept
        percept = nlm.SensoryPercept()
        vision_data = [0.2] * 256
        percept.setVision(vision_data)
        
        # Process with current stage
        agent.processSensoryInput(percept, False)
        motor_command = agent.decodeMotorCommand()
        
        # Update development
        agent.updateDevelopment(1.0)
        
        # Check neuromodulation levels
        neuromod_level = agent.getNeuromodulationLevel()
        learning_rate = agent.getLearningRate()
        
        print(f"  - Neuromodulation: {neuromod_level:.3f}")
        print(f"  - Learning rate: {learning_rate:.4f}")
        print(f"  - Motor command: {motor_command}")
        
        # Simulate time passing
        time.sleep(0.01)
    
    print(f"\n✓ Completed developmental stage progression")
    print(f"  - Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  - Current learning rate: {agent.getLearningRate():.4f}")

if __name__ == "__main__":
    brain, world, agent = basic_setup_example()
    development_stages_example(brain, world, agent)
```

### 5. Error Handling and Validation

Learn robust error handling patterns for production use.

```python
"""error_handling.py - Robust error handling"""
import nlm

def validate_and_handle_errors(brain, world, agent):
    """Demonstrate error handling and validation"""
    print("\n=== Error Handling and Validation ===")
    
    errors_handled = 0
    
    # Test 1: Validate sensory percept before processing
    print("\n1. Testing sensory percept validation...")
    valid_percept = nlm.SensoryPercept()
    vision_data = [0.1] * 256
    valid_percept.setVision(vision_data)
    
    if agent.validateSensoryPercept(valid_percept):
        agent.processSensoryInput(valid_percept, False)
        print("✓ Valid percept processed successfully")
    else:
        print("✗ Valid percept rejected")
        errors_handled += 1
    
    # Test 2: Handle invalid percept gracefully
    print("\n2. Testing invalid percept handling...")
    invalid_percept = nlm.SensoryPercept()
    
    # Create percept with wrong dimensions (empty vision)
    invalid_percept.setVision([])  # Empty vision data
    
    if not agent.validateSensoryPercept(invalid_percept):
        print("✓ Invalid percept correctly rejected by validation")
    else:
        print("✗ Invalid percept should have been rejected")
        errors_handled += 1
    
    # Test 3: Handle reward validation
    print("\n3. Testing reward validation...")
    valid_reward = 0.5
    invalid_reward = 1.5  # Outside typical [-1, 1] range
    
    if agent.validateRewardValue(valid_reward):
        agent.applyRewardModulation(valid_reward, 0.3)
        print("✓ Valid reward accepted and applied")
    else:
        print("✗ Valid reward rejected")
        errors_handled += 1
    
    if not agent.validateRewardValue(invalid_reward):
        print("✓ Invalid reward correctly rejected")
    else:
        print("✗ Invalid reward should have been rejected")
        errors_handled += 1
    
    # Test 4: Handle motor command validation
    print("\n4. Testing motor command validation...")
    valid_command = nlm.MotorCommand.MoveForward
    
    if agent.validateMotorCommand(valid_command):
        print("✓ Valid motor command accepted")
    else:
        print("✗ Valid motor command rejected")
        errors_handled += 1
    
    # Test 5: Handle world state validation
    print("\n5. Testing world state validation...")
    if agent.validateWorldState(world):
        print("✓ World state validated successfully")
    else:
        print("✗ World state validation failed")
        errors_handled += 1
    
    # Test 6: Handle brain state validation
    print("\n6. Testing brain state validation...")
    if agent.validateBrainState():
        print("✓ Brain state validated successfully")
    else:
        print("✗ Brain state validation failed")
        errors_handled += 1
    
    print(f"\n✓ Error handling test completed. {errors_handled} issues detected.")
    return errors_handled == 0

if __name__ == "__main__":
    brain, world, agent = basic_setup_example()
    agent.initialize(world)
    success = validate_and_handle_errors(brain, world, agent)
    print(f"\nTest {'PASSED' if success else 'FAILED'}")
```

### 6. Performance Monitoring

Monitor and profile simulation performance.

```python
"""performance_monitoring.py - Performance monitoring"""
import nlm
import time

def performance_monitoring_example(brain, world, agent):
    """Monitor and profile simulation performance"""
    print("\n=== Performance Monitoring ===")
    
    # Initialize agent
    agent.initialize(world)
    
    # Create percept generator
    def generate_percept(step):
        percept = nlm.SensoryPercept()
        
        # Generate vision based on step (pseudorandom)
        vision_data = []
        for i in range(256):
            vision_data.append(0.1 + 0.8 * ((i + step) % 256) / 256.0)
        
        percept.setVision(vision_data)
        return percept
    
    # Warm-up run
    print("Warming up simulation...")
    for _ in range(5):
        percept = generate_percept(0)
        agent.processSensoryInput(percept, False)
        agent.decodeMotorCommand()
    
    # Performance measurement
    print("Measuring performance...")
    num_steps = 50
    start_time = time.time()
    
    for step in range(num_steps):
        percept = generate_percept(step)
        agent.processSensoryInput(percept, False)
        
        motor_command = agent.decodeMotorCommand()
        
        # Update development occasionally
        if step % 5 == 0:
            agent.updateDevelopment(1.0)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Collect metrics
    agent.collectMetrics()
    
    # Get performance statistics
    metrics = agent.profilePerformance()
    
    print(f"\nPerformance Results ({num_steps} steps):")
    print(f"  - Total time: {elapsed:.2f}s")
    print(f"  - Steps per second: {num_steps / elapsed:.1f}")
    print(f"  - Average step time: {metrics.averageStepTime*1000:.2f}ms")
    print(f"  - Processing efficiency: {metrics.processingEfficiency:.3f}")
    print(f"  - Memory usage: {metrics.memoryUsageBytes / 1024:.1f} KB")
    print(f"  - Session uptime: {metrics.sessionDurationSeconds:.1f}s")
    
    # Get metrics summary
    summary = agent.getMetricsSummary()
    print(f"\nMetrics Summary:")
    print(summary)
    
    return metrics

if __name__ == "__main__":
    brain, world, agent = basic_setup_example()
    metrics = performance_monitoring_example(brain, world, agent)
```

## Summary

This tutorial covers:

1. **Basic Setup** - Creating configuration, brain, world, and agent
2. **First Simulation** - Processing sensory input and producing motor output
3. **Episode Management** - Running learning episodes with rewards and tracking
4. **Development Stages** - Understanding developmental progression and plasticity
5. **Error Handling** - Validating inputs and handling errors gracefully
6. **Performance Monitoring** - Measuring and profiling simulation performance

## Further Learning

After completing this tutorial, explore:

- `advanced_features.py` - Batch processing, serialization, profiling
- `error_handling_examples.py` - More complex error scenarios
- `performance_examples.py` - Advanced optimization techniques
- `integration_examples.py` - Connecting with external systems

These examples demonstrate real-world use cases and production-ready patterns for using NLM Python bindings effectively.
"""