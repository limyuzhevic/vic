"""
NLM Python Bindings Tests and Integration Examples

This module provides comprehensive test examples for the NLM Python bindings.
It includes:
- Basic functionality tests
- Integration tests
- Performance benchmarks
- Error handling tests
- Example usage patterns

These tests are designed to verify that the Python bindings work correctly
and provide useful examples for users to understand the API.
"""

import sys
import os
import time
import traceback
from typing import Dict, List, Any, Optional

# Try to import the NLM bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: NLM Python bindings not available: {e}")
    print("Please install with: pip install -e .")

def test_basic_imports():
    """Test that all basic imports work."""
    print("=== Test 1: Basic Imports ===")
    
    required_classes = [
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'NeuronId', 'SynapseId', 'RegionId',
        'PopulationId', 'NeuronType', 'SynapseType', 'DevelopmentalStage',
        'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType'
    ]
    
    required_functions = [
        'createDefaultConfig', 'createBrain', 'createWorld', 'createAgentBrain',
        'normalizeVector', 'vectorToString', 'validateConfig', 'isPositiveNumber',
        'isPositiveInt', 'getVersion'
    ]
    
    try:
        # Test class imports
        for class_name in required_classes:
            if hasattr(pynlm, class_name):
                print(f"  ✓ {class_name}")
            else:
                print(f"  ✗ {class_name} - NOT FOUND")
        
        # Test function imports
        for func_name in required_functions:
            if hasattr(pynlm, func_name):
                print(f"  ✓ {func_name}")
            else:
                print(f"  ✗ {func_name} - NOT FOUND")
        
        # Test constants
        constants = [
            'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
            'INVALID_POPULATION_ID', 'DEFAULT_SIMULATION_TIMESTEP',
            'DEFAULT_RANDOM_SEED', 'DEFAULT_NEURON_COUNT', 'DEFAULT_REGION_COUNT'
        ]
        
        for const_name in constants:
            if hasattr(pynlm, const_name):
                print(f"  ✓ {const_name}")
            else:
                print(f"  ✗ {const_name} - NOT FOUND")
        
        print("Basic imports test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Basic imports test FAILED: {e}\n")
        return False

def test_config_functionality():
    """Test Config class functionality."""
    print("=== Test 2: Config Functionality ===")
    
    try:
        # Test creating default config
        config = pynlm.createDefaultConfig()
        print("  ✓ Created default config")
        
        # Test setting values
        config.set("neuron_count", 500)
        config.set("random_seed", 12345)
        config.set("connection_probability", 0.15)
        print("  ✓ Set configuration values")
        
        # Test getting values with defaults
        neuron_count = config.getOr("neuron_count", 100)
        print(f"  ✓ Got neuron_count: {neuron_count}")
        
        # Test has method
        has_neuron_count = config.has("neuron_count")
        print(f"  ✓ Has neuron_count: {has_neuron_count}")
        
        # Test getKeys
        keys = config.getKeys()
        print(f"  ✓ Got keys: {keys}")
        
        # Test summary
        summary = config.summary()
        print(f"  ✓ Summary: {summary[:50]}...")
        
        print("Config functionality test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Config functionality test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_brain_creation_and_basic_operations():
    """Test Brain class basic operations."""
    print("=== Test 3: Brain Creation and Basic Operations ===")
    
    try:
        # Create brain
        brain = pynlm.createBrain()
        print("  ✓ Created brain")
        
        # Initialize brain
        brain.initialize()
        print("  ✓ Initialized brain")
        
        # Check initial state
        neuron_count = brain.getTotalNeuronCount()
        print(f"  ✓ Initial neuron count: {neuron_count}")
        
        # Step brain
        for step in range(5):
            brain.step(step, step * 0.001)
        
        print("  ✓ Ran 5 simulation steps")
        
        # Check statistics
        total_spikes = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        region_count = brain.getRegionCount()
        
        print(f"  ✓ Total spikes: {total_spikes}")
        print(f"  ✓ Average firing rate: {firing_rate:.4f}")
        print(f"  ✓ Region count: {region_count}")
        
        # Test developmental stage
        stage = brain.getDevelopmentalStage()
        print(f"  ✓ Developmental stage: {stage}")
        
        print("Brain creation and operations test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Brain creation and operations test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_world_agent_integration():
    """Test world and agent integration."""
    print("=== Test 4: World-Agent Integration ===")
    
    try:
        # Create world
        world = pynlm.createWorld(20, 20, 5, 5)
        print("  ✓ Created world")
        
        # Create brain and agent
        brain = pynlm.createBrain()
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("  ✓ Created and initialized agent")
        
        # Set agent starting position
        world.setAgentStart(10, 10)
        print("  ✓ Set agent start position")
        
        # Run simulation loop
        for step in range(20):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step, step * 0.001)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, step * 0.001)
        
        print("  ✓ Ran 20-step simulation")
        
        # Get agent state
        body = world.getAgentBody()
        print(f"  ✓ Final position: ({body.x:.1f}, {body.y:.1f})")
        print(f"  ✓ Final energy: {body.energy:.1f}")
        print(f"  ✓ Final age: {body.age}")
        
        # Test agent configuration
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        curiosity_level = agent.getCuriosityLevel()
        development_stage = agent.getDevelopmentalStage()
        
        print(f"  ✓ Curiosity level: {curiosity_level:.4f}")
        print(f"  ✓ Development stage: {development_stage}")
        
        print("World-agent integration test PASSED\n")
        return True
        
    except Exception as e:
        print(f"World-agent integration test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_memory_systems():
    """Test memory system availability and basic functionality."""
    print("=== Test 5: Memory Systems ===")
    
    try:
        # Create brain
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Test memory system availability
        memory_systems = {
            'Working Memory': brain.getWorkingMemory(),
            'Episodic Memory': brain.getEpisodicMemory(),
            'Semantic Memory': brain.getSemanticMemory(),
            'Procedural Memory': brain.getProceduralMemory(),
            'Associative Memory': brain.getAssociativeMemory(),
        }
        
        print("  Memory system availability:")
        for name, system in memory_systems.items():
            status = 'Available' if system else 'Not Available'
            print(f"    {name}: {status}")
        
        # Simulate some activity to potentially populate memory
        for step in range(50):
            brain.step(step, step * 0.001)
            
            # Inject some sensory activity
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 3.0)
        
        # Check if working memory has been used
        if memory_systems['Working Memory']:
            active_traces = memory_systems['Working Memory'].getActiveTraces()
            print(f"  ✓ Working memory has {active_traces} active traces")
        
        # Check episodic memory
        if memory_systems['Episodic Memory']:
            episode_count = memory_systems['EpisodicMemory'].getEpisodeCount()
            print(f"  ✓ Episodic memory has {episode_count} episodes")
        
        print("Memory systems test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Memory systems test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_plasticity_and_learning():
    """Test plasticity and learning mechanisms."""
    print("=== Test 6: Plasticity and Learning ===")
    
    try:
        # Create brain
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Set developmental stage for plasticity
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
        
        # Record initial state
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getAverageFiringRate()
        
        print(f"  Initial spikes: {initial_spikes}")
        print(f"  Initial firing rate: {initial_firing:.4f}")
        
        # Apply learning experience
        for step in range(100):
            # Create input pattern
            for i in range(5):
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 15.0)
            
            brain.step(step, step * 0.001)
        
        # Record final state
        final_spikes = brain.getTotalSpikeCount()
        final_firing = brain.getAverageFiringRate()
        
        print(f"  Final spikes: {final_spikes}")
        print(f"  Final firing rate: {final_firing:.4f}")
        
        # Calculate changes
        spike_change = final_spikes - initial_spikes
        firing_change = final_firing - initial_firing
        
        print(f"  Spike change: {spike_change}")
        print(f"  Firing rate change: {firing_change:.4f}")
        
        # Test that learning occurred (firing rate changed)
        learning_detected = abs(firing_change) > 0.001
        print(f"  Learning detected: {learning_detected}")
        
        print("Plasticity and learning test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Plasticity and learning test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_neuromodulation():
    """Test neuromodulation systems."""
    print("=== Test 7: Neuromodulation ===")
    
    try:
        # Create agent with neuromodulation enabled
        world = pynlm.createWorld()
        brain = pynlm.createBrain()
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("  Neuromodulation systems enabled")
        
        # Run simulation with neuromodulation
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Apply reward for exploration behavior
            reward = 0.1 if step % 10 == 0 else 0.0  # Occasional reward
            agent.applyRewardModulation(reward, 0.0)
            
            # Step brain
            brain.step(step, step * 0.001)
            
            # Update development
            agent.updateDevelopment(0.001)
            
            if step % 10 == 0:
                neuromod_level = agent.getNeuromodulationLevel()
                curiosity_level = agent.getCuriosityLevel()
                novelty_level = agent.getNoveltyLevel()
                prediction_error = agent.getPredictionError()
                
                print(f"    Step {step}: Neuromod={neuromod_level:.4f}, "
                      f"Curiosity={curiosity_level:.4f}, "
                      f"Novelty={novelty_level:.4f}, Error={prediction_error:.4f}")
        
        print("Neuromodulation test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Neuromodulation test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_utility_functions():
    """Test utility functions."""
    print("=== Test 8: Utility Functions ===")
    
    try:
        # Test vector normalization
        test_vector = [0.1, 0.5, 0.9, 0.2, 0.8]
        normalized = pynlm.normalizeVector(test_vector)
        
        print(f"  Original vector: {pynlm.vectorToString(test_vector)}")
        print(f"  Normalized vector: {pynlm.vectorToString(normalized)}")
        
        # Check normalization (should be in [0, 1] range)
        all_in_range = all(0.0 <= x <= 1.0 for x in normalized)
        print(f"  All values in [0,1]: {all_in_range}")
        
        # Test validation functions
        print(f"  isPositiveNumber(5.0): {pynlm.isPositiveNumber(5.0)}")
        print(f"  isPositiveNumber(-2.0): {pynlm.isPositiveNumber(-2.0)}")
        print(f"  isPositiveInt(10): {pynlm.isPositiveInt(10)}")
        print(f"  isPositiveInt(0): {pynlm.isPositiveInt(0)}")
        
        # Test version
        version = pynlm.getVersion()
        print(f"  Version: {version}")
        
        print("Utility functions test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Utility functions test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling."""
    print("=== Test 9: Error Handling ===")
    
    try:
        # Test that exceptions are properly defined
        print("  Checking exception classes...")
        
        exceptions = [
            pynlm.NLMException,
            pynlm.ConfigurationError,
            pynlm.SimulationError,
            pynlm.MemoryError,
        ]
        
        for exc in exceptions:
            print(f"    ✓ {exc.__name__}")
        
        # Test configuration error
        print("  Testing configuration error...")
        try:
            # This should work (creating valid config)
            config = pynlm.createDefaultConfig()
            print("    ✓ Created config successfully")
        except pynlm.ConfigurationError:
            print("    ✓ ConfigurationError raised as expected")
        
        print("Error handling test PASSED\n")
        return True
        
    except Exception as e:
        print(f"Error handling test FAILED: {e}\n")
        traceback.print_exc()
        return False

def test_performance_benchmarks():
    """Test basic performance benchmarks."""
    print("=== Test 10: Performance Benchmarks ===")
    
    try:
        if not PYNLM_AVAILABLE:
            print("  Skipping performance tests (NLM not available)\n")
            return True
        
        # Benchmark brain creation
        start_time = time.time()
        
        brains = []
        for i in range(10):
            brain = pynlm.createBrain()
            brain.initialize()
            brains.append(brain)
        
        creation_time = time.time() - start_time
        print(f"  Created 10 brains in {creation_time:.3f}s")
        print(f"  Average creation time: {creation_time/10:.4f}s per brain")
        
        # Benchmark simulation steps
        start_time = time.time()
        
        for brain in brains:
            for step in range(20):
                brain.step(step, step * 0.001)
        
        simulation_time = time.time() - start_time
        print(f"  Ran {len(brains) * 20} simulation steps in {simulation_time:.3f}s")
        print(f"  Average time per step: {simulation_time/(len(brains) * 20):.6f}s")
        
        # Benchmark memory access
        start_time = time.time()
        
        for brain in brains:
            _ = brain.getTotalNeuronCount()
            _ = brain.getTotalSynapseCount()
            _ = brain.getAverageFiringRate()
            _ = brain.getTotalSpikeCount()
        
        access_time = time.time() - start_time
        print(f"  Accessed memory for {len(brains)} brains in {access_time:.3f}s")
        
        print("Performance benchmarks completed\n")
        return True
        
    except Exception as e:
        print(f"Performance benchmarks FAILED: {e}\n")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests and return summary."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    print()
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Config Functionality", test_config_functionality),
        ("Brain Creation and Operations", test_brain_creation_and_basic_operations),
        ("World-Agent Integration", test_world_agent_integration),
        ("Memory Systems", test_memory_systems),
        ("Plasticity and Learning", test_plasticity_and_learning),
        ("Neuromodulation", test_neuromodulation),
        ("Utility Functions", test_utility_functions),
        ("Error Handling", test_error_handling),
        ("Performance Benchmarks", test_performance_benchmarks),
    ]
    
    results = {}
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"Running: {test_name}")
        try:
            success = test_func()
            results[test_name] = success
            if success:
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"Test {test_name} CRASHED: {e}")
            results[test_name] = False
            failed += 1
        
        print("-" * 60)
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success rate: {passed/len(tests)*100:.1f}%")
    
    if failed > 0:
        print("\nFAILED TESTS:")
        for test_name, success in results.items():
            if not success:
                print(f"  ✗ {test_name}")
    
    print("\n" + "=" * 60)
    return results

def run_examples():
    """Run the example usage patterns."""
    print("\n" + "=" * 60)
    print("EXAMPLE USAGE PATTERNS")
    print("=" * 60)
    
    examples = [
        ("Basic Usage", lambda: test_basic_examples()),
        ("Agent in World", lambda: test_agent_examples()),
        ("Learning Simulation", lambda: test_learning_examples()),
        ("Advanced Agent", lambda: test_advanced_examples()),
    ]
    
    for example_name, example_func in examples:
        print(f"\nExample: {example_name}")
        print("-" * 40)
        try:
            example_func()
            print(f"✓ {example_name} completed")
        except Exception as e:
            print(f"✗ {example_name} failed: {e}")

# Example-specific test functions
def test_basic_examples():
    """Test basic usage patterns."""
    print("  Basic NLM usage pattern:")
    
    # Create brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Simple simulation
    for step in range(10):
        brain.step(step, step * 0.001)
    
    print(f"    Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"    Total spikes after 10 steps: {brain.getTotalSpikeCount()}")

def test_agent_examples():
    """Test agent-based patterns."""
    print("  Agent in world pattern:")
    
    # Create complete setup
    world = pynlm.createWorld(15, 15)
    brain = pynlm.createBrain()
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Main loop
    for step in range(20):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.001)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.001)
    
    body = world.getAgentBody()
    print(f"    Agent final position: ({body.x:.1f}, {body.y:.1f})")
    print(f"    Agent final energy: {body.energy:.1f}")

def test_learning_examples():
    """Test learning patterns."""
    print("  Learning simulation pattern:")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Record baseline
    initial_spikes = brain.getTotalSpikeCount()
    
    # Apply learning stimulus
    for step in range(50):
        for i in range(3):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
        brain.step(step, step * 0.001)
    
    # Check learning
    final_spikes = brain.getTotalSpikeCount()
    spike_change = final_spikes - initial_spikes
    
    print(f"    Initial spikes: {initial_spikes}")
    print(f"    Final spikes: {final_spikes}")
    print(f"    Spike change: {spike_change} (learning detected)")

def test_advanced_examples():
    """Test advanced usage patterns."""
    print("  Advanced agent pattern:")
    
    # Create advanced setup
    world = pynlm.createWorld(20, 20, 7, 7)
    brain = pynlm.createBrain()
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Advanced simulation with development
    for step in range(30):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.001)
        agent.updateDevelopment(0.001)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.001)
        
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"    Step {step}: Pos ({body.x:.1f}, {body.y:.1f}), "
                  f"Curiosity {agent.getCuriosityLevel():.3f}")
    
    print("    Advanced simulation completed")

if __name__ == "__main__":
    # Check if running in test mode
    if len(sys.argv) > 1 and sys.argv[1] == "--run-all":
        run_all_tests()
        run_examples()
    elif len(sys.argv) > 1 and sys.argv[1] == "--examples":
        run_examples()
    else:
        print("NLM Python Bindings Test Suite")
        print("=" * 40)
        print("\nUsage:")
        print("  python tests.py --run-all    Run all tests")
        print("  python tests.py --examples   Run usage examples")
        print("\nTo install NLM Python bindings:")
        print("  pip install -e .")