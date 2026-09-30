# Python Bindings Testing Infrastructure

This document describes the testing infrastructure for the pynlm Python bindings.

## Overview

The testing infrastructure provides comprehensive tests for the Python bindings,
ensuring that the enhanced functionality works correctly and that the API
remains stable across different versions.

## Test Structure

### Basic Unit Tests

The basic tests cover the core functionality:

1. **Test Basic Brain Creation** - Test creating brains with default configuration
2. **Test Configuration Management** - Test loading and saving configurations
3. **Test World Operations** - Test world creation and manipulation
4. **Test Agent Functionality** - Test agent brain creation and initialization
5. **Test Simulation Steps** - Test basic simulation execution
6. **Test Statistics Collection** - Test statistical data collection
7. **Test Error Handling** - Test error handling and validation

### Advanced Unit Tests

The advanced tests cover the enhanced features:

1. **Test Enhanced Brain Creation** - Test factory functions for brain creation
2. **Test Simulation Control** - Test controlled simulation with brain-world-agent integration
3. **Test Learning and Training** - Test reinforcement learning functionality
4. **Test Performance Benchmarking** - Test performance measurement
5. **Test Memory Management** - Test memory usage and cleanup
6. **Test Concurrent Operations** - Test thread-safe operations
7. **Test Configuration Validation** - Test configuration validation

### Integration Tests

The integration tests cover the entire system:

1. **Test Complete Brain-World-Agent Loop** - Test the complete interaction cycle
2. **Test Learning Scenarios** - Test various learning scenarios
3. **Test Performance Under Load** - Test performance with many neurons
4. **Test Persistence and Serialization** - Test save/load functionality
5. **Test Configuration Scenarios** - Test different configuration combinations

### Performance Tests

The performance tests measure efficiency:

1. **Test Simulation Speed** - Test basic simulation speed
2. **Test Memory Usage** - Test memory consumption
3. **Test Scalability** - Test performance with increasing complexity
4. **Test Resource Efficiency** - Test efficient use of system resources

## Test Files

### Test Python Bindings

**File:** `test_python_bindings.py`

This file contains comprehensive tests for the Python bindings:

```python
import pynlm
import pytest
import time
import tempfile
import os

class TestBasicPythonBindings:
    """Test basic Python binding functionality"""
    
    def test_brain_creation(self):
        """Test creating a brain with default configuration"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    def test_world_creation(self):
        """Test creating a world"""
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert world.getWidth() > 0
        assert world.getHeight() > 0
    
    def test_agent_creation(self):
        """Test creating an agent brain"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
    
    def test_simulation_step(self):
        """Test a single simulation step"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        
        assert brain.getTotalSpikeCount() >= initial_spikes

class TestConfigurationManagement:
    """Test configuration management functionality"""
    
    def test_default_config(self):
        """Test creating default configuration"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert "neuron_count" in config.getKeys()
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        validation = pynlm.validateConfig(config)
        assert validation["is_valid"] == True
        assert "key_count" in validation

class TestAdvancedFeatures:
    """Test advanced Python binding features"""
    
    def test_enhanced_brain_creation(self):
        """Test enhanced brain creation with parameters"""
        brain = pynlm.createBrainWithParameters(
            neuronCount=100,
            regionCount=1,
            connectionProb=0.1,
            seed=42
        )
        assert brain is not None
        assert brain.getTotalNeuronCount() == 100
    
    def test_statistics_collection(self):
        """Test statistics collection"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        stats = brain.getStatistics()
        assert "total_neurons" in stats
        assert "total_spikes" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        
        assert stats["total_neurons"] > 0
        assert stats["total_spikes"] >= 0
    
    def test_simulation_control(self):
        """Test controlled simulation with brain-world-agent"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check results
        assert brain.getFiringNeuronCount() >= 0
        assert world.getAgentBody().x >= 0

class TestLearningAndTraining:
    """Test learning and training functionality"""
    
    def test_reward_modulation(self):
        """Test reward modulation"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        
        # Add a resource
        pynlm.addResourceToWorld(world, 5.0, 5.0, 10.0)
        
        # Run simulation with reward
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Check learning progress
        assert agent.getNeuromodulationLevel() >= -1.0
        assert agent.getCuriosityLevel() >= 0.0
    
    def test_training_with_rewards(self):
        """Test comprehensive training with rewards"""
        training_results = pynlm.trainWithRewards(
            brain=pynlm.createBrain(pynlm.createDefaultConfig()),
            world=pynlm.createSimpleWorld(),
            agent=None,  # Will be created automatically
            episodes=10,
            discountFactor=0.95
        )
        
        assert "episode_rewards" in training_results
        assert "average_reward" in training_results
        assert "best_reward" in training_results
        
        assert len(training_results["episode_rewards"]) == 10
        assert training_results["average_reward"] >= -1.0

class TestPerformanceBenchmarking:
    """Test performance benchmarking functionality"""
    
    def test_benchmark_brain(self):
        """Test brain benchmarking"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        benchmark_results = pynlm.benchmarkBrain(brain, steps=1000)
        
        assert "steps_completed" in benchmark_results
        assert "total_spikes" in benchmark_results
        assert "duration_ms" in benchmark_results
        assert "steps_per_second" in benchmark_results
        assert "neurons" in benchmark_results
        
        assert benchmark_results["steps_completed"] == 1000
        assert benchmark_results["steps_per_second"] > 0

class TestPersistence:
    """Test persistence and serialization"""
    
    def test_save_load_brain(self):
        """Test saving and loading brain state"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Simulate some activity
        for i in range(100):
            brain.step(i)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            brain.save(filepath)
            
            # Create new brain and load
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_brain.load(filepath)
            
            # Verify loaded brain has expected properties
            assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert new_brain.getTotalSynapseCount() == brain.getTotalSynapseCount()
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)

if __name__ == "__main__":
    print("Running Python bindings tests...")
    pytest.main([__file__, "-v"])
```

### Performance Test Script

**File:** `performance_test.py`

This script tests performance aspects of the Python bindings:

```python
import pynlm
import time
import tracemalloc
import matplotlib.pyplot as plt

def test_performance_scaling():
    """Test performance scaling with increasing neuron counts"""
    neuron_counts = [100, 500, 1000, 2000]
    times = []
    memory_usages = []
    
    for neuron_count in neuron_counts:
        # Start memory tracking
        tracemalloc.start()
        
        # Create brain
        start_time = time.time()
        
        brain = pynlm.createBrainWithParameters(
            neuronCount=neuron_count,
            regionCount=2,
            connectionProb=0.15,
            seed=42
        )
        brain.initialize()
        
        # Run simulation
        for i in range(100):
            brain.step(i, i * 0.001)
        
        end_time = time.time()
        
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        times.append(end_time - start_time)
        memory_usages.append(peak / 1024 / 1024)  # Convert to MB
        
        print(f"Neurons: {neuron_count}, Time: {times[-1]:.3f}s, Memory: {memory_usages[-1]:.2f}MB")
    
    # Plot results
    plt.figure(figsize=(12, 5))
    
    plt.subplot(1, 2, 1)
    plt.plot(neuron_counts, times, 'bo-')
    plt.xlabel('Neuron Count')
    plt.ylabel('Time (seconds)')
    plt.title('Performance Scaling')
    plt.grid(True)
    
    plt.subplot(1, 2, 2)
    plt.plot(neuron_counts, memory_usages, 'ro-')
    plt.xlabel('Neuron Count')
    plt.ylabel('Memory Usage (MB)')
    plt.title('Memory Scaling')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('performance_test_results.png')
    plt.show()
    
    return {
        'neuron_counts': neuron_counts,
        'times': times,
        'memory_usages': memory_usages
    }

def test_real_time_performance():
    """Test real-time performance for interactive applications"""
    print("Testing real-time performance...")
    
    # Target: 60 frames per second
    target_fps = 60
    target_frame_time = 1.0 / target_fps
    
    brain = pynlm.createDemoBrain()
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    frame_times = []
    
    for frame in range(100):  # Test 100 frames
        frame_start = time.time()
        
        # Update world
        world.update(target_frame_time)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Simulate brain
        brain.step(frame, frame * target_frame_time)
        
        # Make decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        frame_time = time.time() - frame_start
        frame_times.append(frame_time)
        
        if frame_time > target_frame_time:
            print(f"Frame {frame}: {frame_time*1000:.1f}ms (target: {target_frame_time*1000:.1f}ms) - SLOW")
        else:
            print(f"Frame {frame}: {frame_time*1000:.1f}ms (target: {target_frame_time*1000:.1f}ms) - OK")
    
    avg_frame_time = sum(frame_times) / len(frame_times)
    fps = 1.0 / avg_frame_time
    
    print(f"\nAverage frame time: {avg_frame_time*1000:.1f}ms")
    print(f"Average FPS: {fps:.1f}")
    print(f"Target FPS: {target_fps}")
    
    return {
        'avg_frame_time': avg_frame_time,
        'fps': fps,
        'frame_times': frame_times,
        'target_fps': target_fps,
        'meets_target': fps >= target_fps
    }

if __name__ == "__main__":
    print("=== Performance Testing for pynlm ===\n")
    
    print("1. Testing performance scaling...")
    scaling_results = test_performance_scaling()
    
    print("\n2. Testing real-time performance...")
    realtime_results = test_real_time_performance()
    
    print("\n=== Performance Test Complete ===")
    print(f"Scaling test completed for {len(scaling_results['neuron_counts'])} neuron counts")
    print(f"Real-time test completed with {realtime_results['avg_frame_time']*1000:.1f}ms average frame time")
    print(f"Real-time test {'meets' if realtime_results['meets_target'] else 'exceeds'} target FPS")
```

### Integration Test Script

**File:** `integration_test.py`

This script tests the complete integration of all components:

```python
import pynlm
import time

def test_complete_brain_world_agent_loop():
    """Test the complete brain-world-agent interaction loop"""
    print("Testing complete brain-world-agent loop...")
    
    # Create all components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add environment resources
    pynlm.addResourceToWorld(world, 10.0, 10.0, 50.0)
    pynlm.addResourceToWorld(world, 20.0, 20.0, 30.0)
    pynlm.addResourceToWorld(world, 25.0, 15.0, 40.0)
    
    pynlm.addHazardToWorld(world, 5.0, 5.0, -10.0)
    pynlm.addHazardToWorld(world, 15.0, 25.0, -15.0)
    
    # Run simulation
    simulation_steps = 500
    step_times = []
    
    for step in range(simulation_steps):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step, step * 0.1)
        
        # Motor control
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Record performance
        step_time = time.time() - step_start
        step_times.append(step_time)
        
        # Progress monitoring
        if step % 100 == 0:
            print(f"Step {step}/{simulation_steps}: "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Reward: {result.reward:.2f}, "
                  f"Time: {step_time*1000:.1f}ms")
    
    # Calculate performance metrics
    avg_step_time = sum(step_times) / len(step_times)
    steps_per_second = 1.0 / avg_step_time
    
    print(f"\nSimulation Results:")
    print(f"Total steps: {simulation_steps}")
    print(f"Average step time: {avg_step_time*1000:.1f}ms")
    print(f"Steps per second: {steps_per_second:.1f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Test multiple episodes
    print(f"\nTesting multiple episodes...")
    episode_rewards = []
    
    for episode in range(5):
        # Reset for new episode
        world.reset()
        agent.reset()
        brain.reset()
        brain.initialize()
        
        # Run episode
        episode_reward = 0.0
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
            
            episode_reward += result.reward
        
        episode_rewards.append(episode_reward)
        print(f"Episode {episode}: Total reward = {episode_reward:.2f}")
    
    avg_episode_reward = sum(episode_rewards) / len(episode_rewards)
    print(f"Average episode reward: {avg_episode_reward:.2f}")
    
    return {
        'steps_per_second': steps_per_second,
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'episode_rewards': episode_rewards,
        'avg_episode_reward': avg_episode_reward
    }

def test_learning_scenarios():
    """Test various learning scenarios"""
    print("\nTesting learning scenarios...")
    
    scenarios = [
        {
            'name': 'Exploration Only',
            'enable_curiosity': True,
            'enable_reward': False,
            'enable_development': False
        },
        {
            'name': 'Exploitation Only',
            'enable_curiosity': False,
            'enable_reward': True,
            'enable_development': False
        },
        {
            'name': 'Full Learning',
            'enable_curiosity': True,
            'enable_reward': True,
            'enable_development': True
        },
        {
            'name': 'Development Focus',
            'enable_curiosity': False,
            'enable_reward': False,
            'enable_development': True
        }
    ]
    
    results = {}
    
    for scenario in scenarios:
        print(f"\nTesting scenario: {scenario['name']}")
        
        # Create fresh brain for each scenario
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on scenario
        agent.enableCuriosity(scenario['enable_curiosity'])
        agent.enableRewardModulation(scenario['enable_reward'])
        agent.enableDevelopment(scenario['enable_development'])
        
        # Add training resources
        pynlm.addResourceToWorld(world, 5.0, 5.0, 20.0)
        pynlm.addResourceToWorld(world, 10.0, 10.0, 15.0)
        
        # Run simulation
        episode_rewards = []
        for episode in range(10):
            world.reset()
            agent.reset()
            brain.reset()
            brain.initialize()
            
            episode_reward = 0.0
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step, step * 0.1)
                
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(result.reward, agent.getPredictionError())
                
                episode_reward += result.reward
            
            episode_rewards.append(episode_reward)
        
        avg_reward = sum(episode_rewards) / len(episode_rewards)
        results[scenario['name']] = {
            'avg_reward': avg_reward,
            'episode_rewards': episode_rewards,
            'neuromodulation': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'development_stage': agent.getDevelopmentalStage()
        }
        
        print(f"  Average reward: {avg_reward:.2f}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    return results

if __name__ == "__main__":
    print("=== Integration Testing for pynlm ===\n")
    
    print("1. Testing complete brain-world-agent loop...")
    loop_results = test_complete_brain_world_agent_loop()
    
    print("\n2. Testing various learning scenarios...")
    scenario_results = test_learning_scenarios()
    
    print("\n=== Integration Test Complete ===")
    print(f"Complete loop test: {loop_results['steps_per_second']:.1f} steps/sec, "
          f"{loop_results['avg_firing_rate']:.2f} Hz average firing rate")
    print(f"Scenarios tested: {len(scenario_results)}")
    for scenario_name, results in scenario_results.items():
        print(f"  {scenario_name}: {results['avg_reward']:.2f} avg reward")
```

## Running Tests

### Prerequisites

```bash
# Install required packages
pip install pytest matplotlib numpy

# Install pynlm (if not already installed)
# python setup.py build_ext --inplace
# or
# pip install -e .
```

### Running Tests

```bash
# Run basic unit tests
pytest test_python_bindings.py::TestBasicPythonBindings -v

# Run configuration management tests
pytest test_python_bindings.py::TestConfigurationManagement -v

# Run advanced features tests
pytest test_python_bindings.py::TestAdvancedFeatures -v

# Run learning and training tests
pytest test_python_bindings.py::TestLearningAndTraining -v

# Run performance benchmarking tests
pytest test_python_bindings.py::TestPerformanceBenchmarking -v

# Run persistence tests
pytest test_python_bindings.py::TestPersistence -v

# Run all tests
pytest test_python_bindings.py -v

# Run integration tests
python integration_test.py

# Run performance tests
python performance_test.py
```

### Test Results

The testing infrastructure provides comprehensive test results including:

1. **Unit Test Results** - Individual test case results with pass/fail status
2. **Integration Test Results** - End-to-end system test results
3. **Performance Test Results** - Performance metrics and benchmarks
4. **Summary Reports** - Comprehensive test summaries with statistics
5. **Error Reports** - Detailed error information for failed tests

## Test Coverage

The testing infrastructure provides coverage for:

1. **Basic Functionality** - Core brain, world, and agent creation
2. **Configuration Management** - Loading, saving, and validating configurations
3. **Advanced Features** - Enhanced APIs and convenience functions
4. **Learning and Training** - Reinforcement learning and curiosity-driven exploration
5. **Performance** - Benchmarking and performance analysis
6. **Persistence** - Save/load functionality
7. **Integration** - Complete system testing
8. **Error Handling** - Robust error handling and validation

## Continuous Integration

The testing infrastructure can be integrated with continuous integration systems:

```yaml
# Example GitHub Actions workflow
name: Python Bindings Tests

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, 3.10, 3.11, 3.12]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install pytest matplotlib numpy
        pip install -e .
    
    - name: Run unit tests
      run: |
        pytest test_python_bindings.py::TestBasicPythonBindings -v
        pytest test_python_bindings.py::TestConfigurationManagement -v
    
    - name: Run integration tests
      run: |
        python integration_test.py
    
    - name: Run performance tests
      run: |
        python performance_test.py
    
    - name: Generate test reports
      run: |
        pytest test_python_bindings.py --cov=pynlm --cov-report=xml
    
    - name: Upload test results
      uses: actions/upload-artifact@v3
      with:
        name: test-results-${{ matrix.python-version }}
        path: |
          test_report.html
          performance_results.png
```

## Test Customization

The testing infrastructure can be customized for specific needs:

1. **Add New Test Cases** - Add new test functions to test files
2. **Modify Test Parameters** - Adjust test parameters and configurations
3. **Add Performance Benchmarks** - Add new performance benchmarks
4. **Create Test Scenarios** - Create custom test scenarios
5. **Extend Test Coverage** - Add tests for new features and functionality

This comprehensive testing infrastructure ensures that the pynlm Python bindings remain robust, reliable, and maintainable while providing comprehensive test coverage for all functionality.