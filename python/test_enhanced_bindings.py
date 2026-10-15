#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings.

This script tests the enhanced Python bindings for NLM, including:
- Basic functionality
- NumPy integration
- Visualization
- Performance profiling
- Checkpoint/restore
- Learning and development examples
- Neuromodulation examples
"""

import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import the enhanced Python bindings
from nlm import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Classes
    Config, Vision, Audio, InternalSignals, Action, WorldObject,
    AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain,
    
    # Helper functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Enhanced classes
    NeuralSystem, SensorData, BrainMetrics, DataManager,
    PerformanceProfiler, Visualizer,
    
    # Example modules
    LearningExamples, DevelopmentExamples, NeuromodulationExamples,
    EnvironmentExamples, PerformanceExamples, CheckpointExamples,
    
    # Type aliases
    tensor, epoch, step, neuron_id, synapse_id, population_id, region_id,
    
    # Exception classes
    NLMError, ValidationError, ConfigurationError, DeviceError,
    
    # Context managers
    Timer, BrainSession, WorldSession,
    
    # Utility functions
    configure_logging, set_config, get_config,
    raise_validation_error, raise_configuration_error, raise_device_error
)

import numpy as np

# Mock IPython display for testing
class DisplayMock:
    def __call__(self, *args, **kwargs):
        pass

display = DisplayMock()

#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings.

This script tests the enhanced Python bindings for NLM, including:
- Basic functionality
- NumPy integration
- Visualization
- Performance profiling
- Checkpoint/restore
- Learning and development examples
- Neuromodulation examples
"""

import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import the enhanced Python bindings
from nlm import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Classes
    Config, Vision, Audio, InternalSignals, Action, WorldObject,
    AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain,
    
    # Helper functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Enhanced classes
    NeuralSystem, SensorData, BrainMetrics, DataManager,
    PerformanceProfiler, Visualizer,
    
    # Example modules
    LearningExamples, DevelopmentExamples, NeuromodulationExamples,
    EnvironmentExamples, PerformanceExamples, CheckpointExamples,
    
    # Type aliases
    tensor, epoch, step, neuron_id, synapse_id, population_id, region_id,
    
    # Exception classes
    NLMError, ValidationError, ConfigurationError, DeviceError,
    
    # Context managers
    Timer, BrainSession, WorldSession,
    
    # Utility functions
    configure_logging, set_config, get_config,
    raise_validation_error, raise_configuration_error, raise_device_error
)

import numpy as np

# Mock IPython display for testing
class DisplayMock:
    def __call__(self, *args, **kwargs):
        pass

display = DisplayMock()

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("\n=== Testing Basic Functionality ===")
    
    # Test 1: Create neural system
    print("1. Testing NeuralSystem creation...")
    brain = NeuralSystem()
    assert brain is not None
    print("   ✓ NeuralSystem created successfully")
    
    # Test 2: Create world
    print("2. Testing world creation...")
    world = brain.create_world(width=100.0, height=100.0, vision_width=32, vision_height=32)
    assert world is not None
    assert world.getWidth() == 100.0
    assert world.getHeight() == 100.0
    print("   ✓ World created successfully")
    
    # Test 3: Create agent
    print("3. Testing agent creation...")
    agent = brain.create_agent(0.0, 0.0)
    assert agent is not None
    print("   ✓ Agent created successfully")
    
    # Test 4: Get metrics
    print("4. Testing metrics collection...")
    metrics = brain.get_metrics()
    assert isinstance(metrics, BrainMetrics)
    assert metrics.total_neurons >= 0
    print(f"   ✓ Metrics collected: {metrics.total_neurons} neurons")
    
    # Test 5: Run episode
    print("5. Testing episode execution...")
    episode, total_reward = brain.run_episode(max_steps=10)
    assert isinstance(episode, list)
    assert isinstance(total_reward, (int, float))
    print(f"   ✓ Episode executed: {len(episode)} steps, reward {total_reward}")
    
    print("✓ Basic functionality tests passed")

def test_numpy_integration():
    """Test NumPy integration."""
    print("\n=== Testing NumPy Integration ===")
    
    # Test 1: Create vision data
    print("1. Testing vision data creation...")
    vision_data = np.random.rand(32, 32, 3).astype(np.float32)
    sensor_data = SensorData.from_vision(vision_data)
    assert sensor_data.vision is not None
    assert sensor_data.vision.shape == (3, 32, 32)  # CHW format
    print("   ✓ Vision data created successfully")
    
    # Test 2: Create audio data
    print("2. Testing audio data creation...")
    audio_data = np.random.rand(16000).astype(np.float32)
    sensor_data = SensorData(audio=audio_data)
    assert sensor_data.audio is not None
    assert sensor_data.audio.shape == (16000,)
    print("   ✓ Audio data created successfully")
    
    # Test 3: Create internal data
    print("3. Testing internal data creation...")
    internal_data = np.random.rand(100).astype(np.float32)
    sensor_data = SensorData(internal=internal_data)
    assert sensor_data.internal is not None
    assert sensor_data.internal.shape == (100,)
    print("   ✓ Internal data created successfully")
    
    # Test 4: Create touch data
    print("4. Testing touch data creation...")
    touch_data = np.random.rand(10, 10).astype(np.float32)
    sensor_data = SensorData(touch=touch_data)
    assert sensor_data.touch is not None
    assert sensor_data.touch.shape == (10, 10)
    print("   ✓ Touch data created successfully")
    
    print("✓ NumPy integration tests passed")

def test_visualization():
    """Test visualization capabilities."""
    print("\n=== Testing Visualization ===")
    
    # Create test data
    metrics = [
        BrainMetrics(total_neurons=100, active_neurons=10, firing_rate=2.5, efficiency=0.1),
        BrainMetrics(total_neurons=150, active_neurons=15, firing_rate=2.8, efficiency=0.1),
        BrainMetrics(total_neurons=200, active_neurons=20, firing_rate=3.0, efficiency=0.1),
        BrainMetrics(total_neurons=250, active_neurons=25, firing_rate=3.2, efficiency=0.1),
        BrainMetrics(total_neurons=300, active_neurons=30, firing_rate=3.5, efficiency=0.1),
    ]
    
    # Test 1: Matplotlib visualization
    print("1. Testing Matplotlib visualization...")
    visualizer = Visualizer(backend='matplotlib')
    visualizer.plot_metrics(metrics)
    print("   ✓ Matplotlib visualization completed")
    
    # Test 2: Plotly visualization
    print("2. Testing Plotly visualization...")
    visualizer = Visualizer(backend='plotly')
    visualizer.plot_metrics(metrics)
    print("   ✓ Plotly visualization completed")
    
    print("✓ Visualization tests passed")

def test_performance_profiling():
    """Test performance profiling."""
    print("\n=== Testing Performance Profiling ===")
    
    # Test 1: Create profiler
    print("1. Testing PerformanceProfiler creation...")
    profiler = PerformanceProfiler()
    assert profiler is not None
    print("   ✓ PerformanceProfiler created successfully")
    
    # Test 2: Start/stop profiling
    print("2. Testing start/stop profiling...")
    profiler.start_profiling()
    # Simulate some work
    time.sleep(0.1)
    profiler.end_profiling()
    
    assert profiler.avg_time > 0
    print(f"   ✓ Profiling completed: avg time {profiler.avg_time:.4f}s")
    
    # Test 3: Multiple profiling runs
    print("3. Testing multiple profiling runs...")
    for i in range(3):
        profiler.start_profiling()
        time.sleep(0.05)
        profiler.end_profiling()
    
    print(f"   ✓ Multiple profiling runs completed: avg {profiler.avg_time:.4f}s")
    
    print("✓ Performance profiling tests passed")

def test_checkpoint_restore():
    """Test checkpoint/restore functionality."""
    print("\n=== Testing Checkpoint/Restore ===")
    
    # Create neural system
    brain = NeuralSystem()
    world = brain.create_world()
    
    # Test 1: Save checkpoint
    print("1. Testing checkpoint save...")
    checkpoint_path = brain.save_checkpoint("test_checkpoint.pth")
    assert os.path.exists(checkpoint_path)
    print(f"   ✓ Checkpoint saved to: {checkpoint_path}")
    
    # Test 2: Run more simulation
    print("2. Running simulation after checkpoint...")
    for i in range(5):
        episode, reward = brain.run_episode(max_steps=10)
    
    # Test 3: Load checkpoint
    print("3. Testing checkpoint load...")
    success = brain.load_checkpoint(checkpoint_path)
    assert success
    print("   ✓ Checkpoint loaded successfully")
    
    # Test 4: Verify restoration
    print("4. Verifying restoration...")
    metrics = brain.get_metrics()
    print(f"   ✓ Restored brain metrics: {metrics.total_neurons} neurons")
    
    # Clean up
    if os.path.exists(checkpoint_path):
        os.remove(checkpoint_path)
    
    print("✓ Checkpoint/restore tests passed")

def test_learning_examples():
    """Test learning examples."""
    print("\n=== Testing Learning Examples ===")
    
    brain = NeuralSystem()
    world = brain.create_world()
    
    # Test 1: Goal-directed learning
    print("1. Testing goal-directed learning...")
    LearningExamples.goal_directed_learning(brain, world, None)
    print("   ✓ Goal-directed learning example executed")
    
    # Test 2: Habit formation
    print("2. Testing habit formation...")
    LearningExamples.habit_formation(brain, 1.0, None)
    print("   ✓ Habit formation example executed")
    
    # Test 3: Memory systems
    print("3. Testing memory systems...")
    # Access memory systems
    working_memory = brain.brain.getWorkingMemory()
    episodic_memory = brain.brain.getEpisodicMemory()
    associative_memory = brain.brain.getAssociativeMemory()
    print(f"   ✓ Memory systems accessed: working={working_memory.getTotalNeuronCount()},")
    print(f"     episodic={episodic_memory.getCapacity()},")
    print(f"     associative={associative_memory.getPatternCount()}")
    
    print("✓ Learning examples tests passed")

def test_development_examples():
    """Test development examples."""
    print("\n=== Testing Development Examples ===")
    
    brain = NeuralSystem()
    
    # Test 1: Critical period
    print("1. Testing critical period...")
    DevelopmentExamples.critical_period(brain, DevelopmentalStage.CriticalPeriod)
    print("   ✓ Critical period example executed")
    
    # Test 2: Maturation
    print("2. Testing maturation...")
    DevelopmentExamples.maturation(brain, DevelopmentalStage.Maturation)
    print("   ✓ Maturation example executed")
    
    print("✓ Development examples tests passed")

def test_neuromodulation_examples():
    """Test neuromodulation examples."""
    print("\n=== Testing Neuromodulation Examples ===")
    
    brain = NeuralSystem()
    world = brain.create_world()
    agent = brain.create_agent(0.0, 0.0)
    agent_brain = createAgentBrain(brain)
    
    # Test 1: Reward modulation
    print("1. Testing reward modulation...")
    NeuromodulationExamples.reward_modulation(agent_brain, 1.0, 0.8)
    print("   ✓ Reward modulation example executed")
    
    # Test 2: Curiosity
    print("2. Testing curiosity...")
    NeuromodulationExamples.curiosity(agent_brain, 0.5)
    print("   ✓ Curiosity example executed")
    
    # Test 3: Novelty
    print("3. Testing novelty...")
    NeuromodulationExamples.novelty(agent_brain, 0.3)
    print("   ✓ Novelty example executed")
    
    print("✓ Neuromodulation examples tests passed")

def test_context_managers():
    """Test context managers."""
    print("\n=== Testing Context Managers ===")
    
    # Test 1: Timer context manager
    print("1. Testing Timer context manager...")
    with Timer("Test timer"):
        time.sleep(0.1)
    print("   ✓ Timer context manager executed")
    
    # Test 2: BrainSession context manager
    print("2. Testing BrainSession context manager...")
    brain = NeuralSystem()
    with BrainSession(brain):
        print("   ✓ BrainSession context manager executed")
    
    # Test 3: WorldSession context manager
    print("3. Testing WorldSession context manager...")
    world = SimpleWorld()
    with WorldSession(world):
        print("   ✓ WorldSession context manager executed")
    
    print("✓ Context managers tests passed")

def test_utility_functions():
    """Test utility functions."""
    print("\n=== Testing Utility Functions ===")
    
    # Test 1: configure_logging
    print("1. Testing configure_logging...")
    configure_logging("INFO")
    print("   ✓ configure_logging executed")
    
    # Test 2: set_config/get_config
    print("2. Testing set_config/get_config...")
    set_config('test_key', 'test_value')
    value = get_config('test_key')
    assert value == 'test_value'
    print("   ✓ set_config/get_config executed")
    
    # Test 3: Exception raising
    print("3. Testing exception raising...")
    try:
        raise_validation_error("Test validation error")
        assert False, "Expected exception"
    except ValidationError:
        print("   ✓ ValidationError raised successfully")
    
    try:
        raise_configuration_error("Test configuration error")
        assert False, "Expected exception"
    except ConfigurationError:
        print("   ✓ ConfigurationError raised successfully")
    
    try:
        raise_device_error("Test device error")
        assert False, "Expected exception"
    except DeviceError:
        print("   ✓ DeviceError raised successfully")
    
    print("✓ Utility functions tests passed")

def test_data_manager():
    """Test data manager."""
    print("\n=== Testing DataManager ===")
    
    # Test 1: Create data manager
    print("1. Testing DataManager creation...")
    data_manager = DataManager()
    assert data_manager is not None
    print("   ✓ DataManager created successfully")
    
    # Test 2: Add episode data
    print("2. Testing episode data addition...")
    data_manager.add_episode_data(1, {
        'rewards': [1.0, 2.0, 1.5],
        'actions': [],
        'steps': []
    })
    print("   ✓ Episode data added successfully")
    
    # Test 3: Get average reward
    print("3. Testing average reward calculation...")
    avg_reward = data_manager.get_average_reward([1])
    assert avg_reward == 1.5
    print(f"   ✓ Average reward calculated: {avg_reward}")
    
    # Test 4: Get reward distribution
    print("4. Testing reward distribution...")
    distribution = data_manager.get_reward_distribution([1], bins=5)
    assert len(distribution) == 5
    print(f"   ✓ Reward distribution calculated: {len(distribution)} bins")
    
    print("✓ DataManager tests passed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 60)
    
    try:
        # Run all tests
        test_basic_functionality()
        test_numpy_integration()
        test_visualization()
        test_performance_profiling()
        test_checkpoint_restore()
        test_learning_examples()
        test_development_examples()
        test_neuromodulation_examples()
        test_context_managers()
        test_utility_functions()
        test_data_manager()
        
        print("\n" + "=" * 60)
        print("✓ All tests passed successfully!")
        print("=" * 60)
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())

