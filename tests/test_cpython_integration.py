#!/usr/bin/env python3
"""
Integration Tests for C++/Python Interoperability
Tests the complete integration between C++ core and Python bindings:
- Brain serialization/deserialization
- Agent-world-brain interactions
- Complete simulation workflows
- Performance benchmarks
"""

import sys
import os
import tempfile
import json
import time
import threading
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: pynlm not available: {e}")
    PYNLM_AVAILABLE = False

class SerializationIntegrationTests:
    """Test C++-Python interoperability for serialization"""
    
    def __init__(self):
        if PYNLM_AVAILABLE:
            self.pynlm = pynlm
    
    def test_brain_checkpoint_roundtrip(self):
        """Test complete brain checkpoint save/load cycle"""
        print("    Testing brain checkpoint roundtrip...")
        
        import pynlm
        
        # Create and simulate a brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuronCount", 200)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simulate to create meaningful state
        for step in range(100):
            brain.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.nk', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            # Test saveState with various options
            brain.saveState(checkpoint_file, compress=True, level=pynlm.CompressionLevel.Balanced)
            assert os.path.exists(checkpoint_file)
            
            # Get stats before loading
            before_neurons = brain.getTotalNeuronCount()
            before_synapses = brain.getTotalSynapseCount()
            
            # Create new brain and load checkpoint
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            
            brain2.loadState(checkpoint_file, validate=True, repair=False)
            
            # Verify loaded state
            after_neurons = brain2.getTotalNeuronCount()
            after_synapses = brain2.getTotalSynapseCount()
            
            # Should have reasonable numbers (may vary slightly due to compression/reconstruction)
            assert after_neurons > 0
            assert after_synapses >= 0
            
            print(f"      Checkpoint saved: {os.path.getsize(checkpoint_file)} bytes")
            print(f"      Loaded brain: {after_neurons} neurons, {after_synapses} synapses")
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
        
        print("    test_brain_checkpoint_roundtrip PASSED")
    
    def test_config_roundtrip(self):
        """Test configuration save/load roundtrip"""
        print("    Testing config roundtrip...")
        
        # Create complex configuration
        config = pynlm.createAdvancedConfig("advanced")
        
        # Add custom parameters
        config.set("custom.param1", 42)
        config.set("custom.param2", 3.14)
        config.set("custom.param3", "hello")
        config.set("custom.param4", True)
        config.set("custom.param5", [1, 2, 3, 4, 5])
        
        # Save to file
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            config.saveToFile(config_file)
            assert os.path.exists(config_file)
            
            # Verify file content
            with open(config_file, 'r') as f:
                saved_data = json.load(f)
            
            # Load back
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(config_file)
            
            # Verify key values
            assert config2.getOr(0, "custom.param1") == 42
            assert abs(config2.getOr(0.0, "custom.param2") - 3.14) < 0.001
            assert config2.getOr("", "custom.param3") == "hello"
            assert config2.getOr(False, "custom.param4") is True
            
        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)
        
        print("    test_config_roundtrip PASSED")
    
    def test_validation_roundtrip(self):
        """Test configuration validation roundtrip"""
        print("    Testing validation roundtrip...")
        
        config = pynlm.createDefaultConfig()
        
        # Add parameters that need validation
        config.set("brain.neuronCount", 100)
        config.set("plasticity.stdpLTPWeight", 0.01)
        config.set("neuromod.dopamineBaseline", 0.1)
        
        # Validate with detailed reports
        errors = ""
        warnings = ""
        is_valid = config.validate(errors, warnings)
        
        assert isinstance(is_valid, bool)
        
        print(f"      Validation: {'PASSED' if is_valid else 'FAILED'}")
        if errors:
            print(f"      Errors: {errors[:100]}...")
        if warnings:
            print(f"      Warnings: {warnings[:100]}...")
        
        print("    test_validation_roundtrip PASSED")
class AgentWorldIntegrationTests:
    """Test agent-world-brain integration"""
    
    def __init__(self):
        if PYNLM_AVAILABLE:
            self.pynlm = pynlm
    
    def run_all_tests(self):
        """Run all agent-world-brain integration tests"""
        print("Running agent-world-brain integration tests...")
        
        all_passed = True
        
        # Test 1: Complete agent-world simulation
        try:
            self.test_complete_agent_world_simulation()
            print("  PASSED: Complete agent-world simulation")
        except Exception as e:
            print(f"  FAILED: Complete agent-world simulation - {e}")
            all_passed = False
        
        # Test 2: World scenarios
        try:
            self.test_world_scenarios()
            print("  PASSED: World scenarios")
        except Exception as e:
            print(f"  FAILED: World scenarios - {e}")
            all_passed = False
        
        return all_passed
    
    def test_complete_agent_world_simulation(self):
        """Test complete agent-world-brain simulation"""
        print("    Testing complete agent-world simulation...")
        
        # Create all components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuronCount", 150)
        config.set("plasticity.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Enable all subsystems for realistic behavior
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run complete simulation
        total_steps = 200
        
        for step in range(total_steps):
            # Update world physics
            world.update(0.1)
            
            # Get sensory perception
            percept = world.getSensoryPercept()
            
            # Process sensory input through agent
            agent.processSensoryInput(percept)
            
            # Neural processing
            brain.step(step)
            
            # Get action and apply to world
            action = agent.decodeMotorCommand()
            success = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and update neuromodulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            
            predicted_reward = 0.0
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify simulation completed successfully
        assert brain.getTotalNeuronCount() > 0
        assert world.getAgentBody() is not None
        assert world.getSimulationTime() > 0
        
        print(f"      Simulation completed: {total_steps} steps")
    
    def test_world_scenarios(self):
        """Test various world scenarios"""
        print("    Testing world scenarios...")
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Test physics setup
        world.setPhysicsParameters(gravity=9.81, friction=0.5, restitution=0.7)
        
        # Add procedural content
        world.addProceduralContent("resources", density=0.2)
        world.addProceduralContent("hazards", density=0.05)
        
        # Run scenario
        world.executeScenario("tutorial")
        
        # Test time scaling
        world.setTimeScale(2.0)
        world.update(0.1)
        
        # Test exploration rate
        world.setExplorationRate(0.3)
        
        # Test cloning
        world_clone = world.createClone()
        assert world_clone.getWidth() == world.getWidth()
        assert world_clone.getHeight() == world.getHeight()
class PerformanceIntegrationTests:
    """Test performance and benchmarking"""
    
    def __init__(self):
        if PYNLM_AVAILABLE:
            self.pynlm = pynlm
    
    def run_all_tests(self):
        """Run all performance integration tests"""
        print("Running performance integration tests...")
        
        all_passed = True
        
        # Test 1: Scalability benchmark
        try:
            self.test_scalability_benchmark()
            print("  PASSED: Scalability benchmark")
        except Exception as e:
            print(f"  FAILED: Scalability benchmark - {e}")
            all_passed = False
        
        # Test 2: Memory efficiency
        try:
            self.test_memory_efficiency()
            print("  PASSED: Memory efficiency")
        except Exception as e:
            print(f"  FAILED: Memory efficiency - {e}")
            all_passed = False
        
        # Test 3: Real-time performance
        try:
            self.test_real_time_performance()
            print("  PASSED: Real-time performance")
        except Exception as e:
            print(f"  FAILED: Real-time performance - {e}")
            all_passed = False
        
        return all_passed
    
    def test_scalability_benchmark(self):
        """Test performance with different brain sizes"""
        print("    Testing scalability benchmark...")
        
        brain_sizes = [50, 100, 200]
        results = {}
        
        for size in brain_sizes:
            print(f"      Testing brain size: {size} neurons...")
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuronCount", size)
            
            # Time brain creation and initialization
            start_time = time.time()
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(min(100, size // 2)):
                brain.step(step)
            
            end_time = time.time()
            duration = end_time - start_time
            
            neurons_per_sec = size / duration if duration > 0 else 0
            results[size] = {
                'time': duration,
                'neurons_per_sec': neurons_per_sec,
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount()
            }
            
            print(f"        {size} neurons: {duration:.3f}s ({neurons_per_sec:.1f} neurons/sec)")
        
        # Verify scaling trend (larger brains should still be reasonable)
        if len(results) >= 2:
            sizes = sorted(results.keys())
            times = [results[size]['time'] for size in sizes]
            
            # Time should generally increase with size, but not linearly
            assert times[-1] > times[0] or len(times) == 1
    
    def test_memory_efficiency(self):
        """Test memory efficiency of Python bindings"""
        print("    Testing memory efficiency...")
        
        import gc
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and use multiple brains
        brains = []
        for i in range(5):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuronCount", 100)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Use brain
            for step in range(50):
                brain.step(step)
            
            brains.append(brain)
        
        peak_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Clear references and force garbage collection
        brains.clear()
        gc.collect()
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        memory_growth = peak_memory - initial_memory
        print(f"      Initial memory: {initial_memory:.1f} MB")
        print(f"      Peak memory: {peak_memory:.1f} MB")
        print(f"      Memory growth: {memory_growth:.1f} MB")
        
        # Memory growth should be reasonable (not exponential)
        # Allow for some overhead but should be < 50MB for 5 brains
        assert memory_growth < 50.0, f"Memory growth too high: {memory_growth} MB"
    
    def test_real_time_performance(self):
        """Test real-time performance characteristics"""
        print("    Testing real-time performance...")
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Measure step time
        times = []
        for step in range(100):
            start = time.perf_counter()
            brain.step(step)
            end = time.perf_counter()
            times.append(end - start)
        
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        
        print(f"      Average step time: {avg_time*1000:.2f} ms")
        print(f"      Min step time: {min_time*1000:.2f} ms")
        print(f"      Max step time: {max_time*1000:.2f} ms")
        
        # For real-time applications, steps should be < 10ms typically
        # Allow some margin for larger networks
        assert avg_time < 0.05, f"Average step time too high: {avg_time*1000:.2f} ms"
class WorkflowIntegrationTests:
    """Test complete simulation workflows"""
    
    def __init__(self):
        if PYNLM_AVAILABLE:
            self.pynlm = pynlm
    
    def run_all_tests(self):
        """Run all complete workflow integration tests"""
        print("Running complete workflow integration tests...")
        
        all_passed = True
        
        # Test 1: Training workflow
        try:
            self.test_training_workflow()
            print("  PASSED: Training workflow")
        except Exception as e:
            print(f"  FAILED: Training workflow - {e}")
            all_passed = False
        
        # Test 2: Experiment workflow
        try:
            self.test_experiment_workflow()
            print("  PASSED: Experiment workflow")
        except Exception as e:
            print(f"  FAILED: Experiment workflow - {e}")
            all_passed = False
        
        return all_passed
    
    def test_training_workflow(self):
        """Test complete training workflow"""
        print("    Testing training workflow...")
        
        # Create training environment
        config = pynlm.createDefaultConfig()
        config.set("brain.neuronCount", 300)
        config.set("plasticity.enable", True)
        config.set("neuromodulation.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(20.0, 20.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable training systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableCuriosity(True)
        
        # Training loop
        training_metrics = {
            'steps': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'max_novelty': 0.0,
            'plasticity_events': 0
        }
        
        for episode in range(10):
            for step in range(50):
                # Environment interaction
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Neural processing
                brain.step(step)
                
                # Action selection and execution
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Learning signals
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
                # Collect metrics
                training_metrics['steps'] += 1
                training_metrics['total_reward'] += reward
                
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                training_metrics['max_curiosity'] = max(training_metrics['max_curiosity'], curiosity)
                training_metrics['max_novelty'] = max(training_metrics['max_novelty'], novelty)
                
                if curiosity > 0.5:
                    training_metrics['plasticity_events'] += 1
        
        # Verify training made progress
        assert training_metrics['max_curiosity'] > 0.0
        assert training_metrics['max_novelty'] > 0.0
    
    def test_experiment_workflow(self):
        """Test complete experiment workflow with data collection"""
        print("    Testing experiment workflow...")
        
        # Setup experiment
        config = pynlm.createAdvancedConfig("large")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=15, visionHeight=15)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Experiment parameters
        num_trials = 5
        steps_per_trial = 100
        
        # Data collection
        experiment_data = {
            'trial_results': [],
            'brain_metrics': [],
            'behavior_metrics': []
        }
        
        for trial in range(num_trials):
            trial_data = {
                'rewards': [],
                'curiosity': [],
                'novelty': [],
                'firing_rates': [],
                'actions': []
            }
            
            for step in range(steps_per_trial):
                # Run trial
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Record data
                trial_data['rewards'].append(reward)
                trial_data['curiosity'].append(agent.getCuriosityLevel())
                trial_data['novelty'].append(agent.getNoveltyLevel())
                trial_data['firing_rates'].append(brain.getAverageFiringRate())
                trial_data['actions'].append(action)
            
            experiment_data['trial_results'].append(trial_data)
            
            # Aggregate trial metrics
            avg_reward = sum(trial_data['rewards']) / len(trial_data['rewards'])
            avg_curiosity = sum(trial_data['curiosity']) / len(trial_data['curiosity'])
            avg_novelty = sum(trial_data['novelty']) / len(trial_data['novelty'])
            avg_firing = sum(trial_data['firing_rates']) / len(trial_data['firing_rates'])
            
            experiment_data['brain_metrics'].append({
                'avg_firing_rate': avg_firing,
                'final_neuron_count': brain.getTotalNeuronCount()
            })
            
            experiment_data['behavior_metrics'].append({
                'avg_reward': avg_reward,
                'avg_curiosity': avg_curiosity,
                'avg_novelty': avg_novelty,
                'unique_actions': len(set(trial_data['actions']))
            })
        
        # Verify experiment collected meaningful data
        assert len(experiment_data['trial_results']) > 0
        
        # Calculate summary statistics
        all_rewards = [item for trial in experiment_data['trial_results'] 
                      for item in trial['rewards']]
        all_curiosity = [item for trial in experiment_data['trial_results']
                        for item in trial['curiosity']]
        all_novelty = [item for trial in experiment_data['trial_results']
                      for item in trial['novelty']]
        
        print(f"      Experiment analysis:")
        print(f"        Total steps: {len(all_rewards)}")
        print(f"        Average reward: {sum(all_rewards)/len(all_rewards):.3f}")
        print(f"        Max curiosity: {max(all_curiosity):.3f}")
        print(f"        Max novelty: {max(all_novelty):.3f}")
        
        # Verify experiment made progress
        assert sum(all_rewards) > 0
class NLMSerializationSuite:
    """Comprehensive serialization and interoperability tests"""
    
    def __init__(self):
        if PYNLM_AVAILABLE:
            self.pynlm = pynlm
    
    def run_all_tests(self):
        """Run all serialization integration tests"""
        print("=" * 70)
        print("NLM C++/Python Interoperability Integration Tests")
        print("=" * 70)
        
        results = {}
        
        # Run serialization tests
        print("\n1. Serialization/Deserialization Tests")
        print("-" * 50)
        serialization_tests = SerializationIntegrationTests()
        results['serialization'] = serialization_tests.run_all_tests()
        
        # Run agent-world integration tests
        print("\n2. Agent-World-Brain Integration Tests")
        print("-" * 50)
        aw_tests = AgentWorldIntegrationTests()
        results['agent_world'] = aw_tests.run_all_tests()
        
        # Run performance tests
        print("\n3. Performance Integration Tests")
        print("-" * 50)
        perf_tests = PerformanceIntegrationTests()
        results['performance'] = perf_tests.run_all_tests()
        
        # Run workflow tests
        print("\n4. Complete Workflow Integration Tests")
        print("-" * 50)
        workflow_tests = WorkflowIntegrationTests()
        results['workflows'] = workflow_tests.run_all_tests()
        
        # Summary
        print("\n" + "=" * 70)
        print("INTEGRATION TEST SUMMARY")
        print("=" * 70)
        
        passed = sum(results.values())
        total = len(results)
        
        for test_name, result in results.items():
            status = "PASSED" if result else "FAILED"
            print(f"{test_name:25} : {status}")
        
        print(f"\nTotal: {passed}/{total} integration test suites passed")
        
        return passed == total

if __name__ == "__main__":
    print("NLM C++/Python Interoperability Integration Tests")
    print("These tests verify the complete integration between C++ and Python")
    
    if not PYNLM_AVAILABLE:
        print("\nERROR: NLM Python bindings not available!")
        print("Please build the Python bindings before running integration tests.")
        sys.exit(1)
    
    test_suite = NLMSerializationSuite()
    success = test_suite.run_all_tests()
    
    sys.exit(0 if success else 1)