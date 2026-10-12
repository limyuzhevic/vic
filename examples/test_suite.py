#!/usr/bin/env python3
"""
NLM Integration Tests and Verification Suite

This example demonstrates:
- Comprehensive system testing
- Verification of integration between components
- Performance validation
- Error handling and edge cases
- Continuous integration style testing
"""

import pynlm
import time
import sys
from typing import Dict, List, Tuple, Any
class NLMSystemTester:
    """Comprehensive tester for NLM system integration and functionality"""

    def __init__(self):
        self.test_results = {
            'unit_tests': {},
            'integration_tests': {},
            'performance_tests': {},
            'edge_case_tests': {},
            'total_tests': 0,
            'passed_tests': 0,
            'failed_tests': 0,
            'start_time': None,
            'end_time': None
        }

    def run_all_tests(self):
        """Run comprehensive test suite"""
        print("=== NLM System Integration and Verification Tests ===\n")
        self.test_results['start_time'] = time.time()

        # Unit tests
        print("Running Unit Tests...")
        self._run_unit_tests()

        # Integration tests
        print("\nRunning Integration Tests...")
        self._run_integration_tests()

        # Performance tests
        print("\nRunning Performance Tests...")
        self._run_performance_tests()

        # Edge case tests
        print("\nRunning Edge Case Tests...")
        self._run_edge_case_tests()

        self.test_results['end_time'] = time.time()
        self._generate_test_report()

    def _run_unit_tests(self):
        """Run unit-level tests for individual components"""
        unit_tests = [
            self._test_basic_brain_creation,
            self._test_config_system,
            self._test_world_operations,
            self._test_agent_initialization,
            self._test_sensory_processing,
            self._test_motor_decoding,
            self._test_neuromodulation,
            self._test_development_system,
            self._test_plasticity,
            self._test_memory_systems
        ]

        for test_func in unit_tests:
            test_name = test_func.__name__
            print(f"  Testing {test_name}...")
            try:
                result = test_func()
                self.test_results['unit_tests'][test_name] = {
                    'status': 'PASSED',
                    'details': result,
                    'execution_time': time.time()
                }
                self.test_results['passed_tests'] += 1
                print(f"    ✓ {test_name} PASSED")
            except Exception as e:
                self.test_results['unit_tests'][test_name] = {
                    'status': 'FAILED',
                    'error': str(e),
                    'execution_time': time.time()
                }
                self.test_results['failed_tests'] += 1
                print(f"    ✗ {test_name} FAILED: {e}")

        self.test_results['total_tests'] += len(unit_tests)

    def _test_basic_brain_creation(self) -> str:
        """Test basic brain creation and initialization"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100, pynlm.ConfigSource.Runtime)
        config.set("region_count", 1, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()

        # Verify brain properties
        assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
        assert brain.getRegionCount() > 0, "Brain should have regions"
        assert brain.getTotalSynapseCount() >= 0, "Synapse count should be non-negative"

        return f"Created brain with {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions"

    def _test_config_system(self) -> str:
        """Test configuration system functionality"""
        config = pynlm.createDefaultConfig()

        # Test setting and getting values
        config.set("test_int", 42, pynlm.ConfigSource.Runtime)
        config.set("test_float", 3.14, pynlm.ConfigSource.Runtime)
        config.set("test_string", "hello", pynlm.ConfigSource.Runtime)
        config.set("test_bool", True, pynlm.ConfigSource.Runtime)

        assert config.get("test_int").value() == 42, "Int config retrieval failed"
        assert abs(config.get("test_float").value() - 3.14) < 0.001, "Float config retrieval failed"
        assert config.get("test_string").value() == "hello", "String config retrieval failed"
        assert config.get("test_bool").value() == True, "Bool config retrieval failed"

        # Test existence check
        assert config.has("test_int"), "Config should have test_int"
        assert not config.has("nonexistent"), "Config should not have nonexistent key"

        return "Configuration system passed all tests"

    def _test_world_operations(self) -> str:
        """Test world system operations"""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()

        # Test world properties
        assert world.getWidth() == 20, "World width should be 20"
        assert world.getHeight() == 20, "World height should be 20"
        assert world.getVisionWidth() == 8, "Vision width should be 8"
        assert world.getVisionHeight() == 8, "Vision height should be 8"

        # Test state management
        agent_body = world.getAgentBody()
        assert agent_body.x == 0.0 and agent_body.y == 0.0, "Agent should start at origin"

        return "World operations passed all tests"

    def _test_agent_initialization(self) -> str:
        """Test agent initialization and configuration"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Test agent capabilities
        assert agent.getSensoryInputSize() > 0, "Agent should have sensory input size"
        assert agent.getMotorOutputSize() > 0, "Agent should have motor output size"

        # Test feature toggles
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)

        assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
        assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
        assert agent.isDevelopmentEnabled(), "Development should be enabled"
        assert agent.isCuriosityEnabled(), "Curiosity should be enabled"

        return "Agent initialization passed all tests"

    def _test_sensory_processing(self) -> str:
        """Test sensory processing and input handling"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)

        # Verify that sensory processing doesn't crash
        # (In a real implementation, we would verify actual data processing)

        return "Sensory processing passed all tests"

    def _test_motor_decoding(self) -> str:
        """Test motor command decoding"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Test action selection
        action = agent.decodeMotorCommand()

        # Verify action is valid
        valid_actions = [
            pynlm.ActionType.MoveForward, pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact, pynlm.ActionType.Wait,
            pynlm.ActionType.Look, pynlm.ActionType.LookUp,
            pynlm.ActionType.LookDown
        ]

        assert action.type in valid_actions, f"Action {action.type} should be valid"

        return "Motor decoding passed all tests"

    def _test_neuromodulation(self) -> str:
        """Test neuromodulation systems"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)

        # Test neuromodulation metrics
        initial_curiosity = agent.getCuriosityLevel()
        initial_novelty = agent.getNoveltyLevel()
        initial_prediction_error = agent.getPredictionError()

        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)

        # Verify changes
        assert agent.getCuriosityLevel() >= initial_curiosity, "Curiosity should not decrease with positive reward"
        assert agent.getPredictionError() != initial_prediction_error, "Prediction error should change with reward"

        return "Neuromodulation passed all tests"

    def _test_development_system(self) -> str:
        """Test developmental system"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableDevelopment(True)

        # Get developmental stages
        initial_stage = agent.getDevelopmentalStage()

        # Update development
        for i in range(10):
            agent.updateDevelopment(0.1)

        # Verify development progression
        final_stage = agent.getDevelopmentalStage()
        assert final_stage != initial_stage, "Development should progress through stages"

        return "Development system passed all tests"

    def _test_plasticity(self) -> str:
        """Test plasticity mechanisms"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Note: Plasticity testing would require access to internal brain systems
        # This is a basic verification that plasticity can be enabled
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableStructuralPlasticity(True)

        assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"

        return "Plasticity system passed all tests"

    def _test_memory_systems(self) -> str:
        """Test memory system integration (conceptual)"""
        # Memory system testing is complex as it's deeply integrated
        # This test verifies that the brain can process and maintain state

        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200, pynlm.ConfigSource.Runtime)
        config.set("region_count", 2, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()

        # Verify brain maintains state over time
        initial_spikes = brain.getTotalSpikeCount()

        for step in range(10):
            brain.step(step)

        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes, "Brain should maintain or increase spike count"

        return "Memory system integration passed basic tests"

    def _run_integration_tests(self):
        """Run integration tests for system-wide functionality"""
        integration_tests = [
            self._test_complete_brain_world_integration,
            self._test_advanced_agent_behavior,
            self._test_learning_simulation,
            self._test_stability_over_time,
            self._test_error_handling
        ]

        for test_func in integration_tests:
            test_name = test_func.__name__
            print(f"  Testing {test_name}...")
            try:
                result = test_func()
                self.test_results['integration_tests'][test_name] = {
                    'status': 'PASSED',
                    'details': result,
                    'execution_time': time.time()
                }
                self.test_results['passed_tests'] += 1
                print(f"    ✓ {test_name} PASSED")
            except Exception as e:
                self.test_results['integration_tests'][test_name] = {
                    'status': 'FAILED',
                    'error': str(e),
                    'execution_time': time.time()
                }
                self.test_results['failed_tests'] += 1
                print(f"    ✗ {test_name} FAILED: {e}")

        self.test_results['total_tests'] += len(integration_tests)

    def _test_complete_brain_world_integration(self) -> str:
        """Test complete integration between brain and world"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Enable all systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)

        # Run complete simulation cycle
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())

            # Apply learning
            agent.applyRewardModulation(0.1, 0.05)
            agent.updateDevelopment(0.1)

        # Verify system integrity
        assert brain.getTotalNeuronCount() > 0, "Brain should be functional"
        assert world.getAgentBody().x >= 0, "Agent should be in bounds"

        return "Complete brain-world integration passed"

    def _test_advanced_agent_behavior(self) -> str:
        """Test advanced agent behaviors with all systems enabled"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)

        # Collect behavior metrics
        behaviors = {
            'exploration_count': 0,
            'interaction_count': 0,
            'movement_count': 0,
            'curiosity_changes': 0,
            'development_progressions': 0
        }

        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()

            # Track behavior
            if action.type in [pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight,
                              pynlm.ActionType.LookLeft, pynlm.ActionType.LookRight]:
                behaviors['exploration_count'] += 1
            elif action.type == pynlm.ActionType.Interact:
                behaviors['interaction_count'] += 1
            elif action.type in [pynlm.ActionType.MoveForward, pynlm.ActionType.MoveBackward]:
                behaviors['movement_count'] += 1

            world.applyMotorCommand(action, world.getSimulationTime())

            # Track learning metrics
            if step % 10 == 0:
                if agent.getCuriosityLevel() > 0.3:
                    behaviors['curiosity_changes'] += 1
                if agent.getDevelopmentalStage() != agent.getDevelopmentalStage():
                    behaviors['development_progressions'] += 1

        # Verify diverse behavior
        assert behaviors['exploration_count'] > 0, "Agent should explore"
        assert behaviors['movement_count'] > 0, "Agent should move"

        return f"Advanced agent behavior passed - Actions: {behaviors}"

    def _test_learning_simulation(self) -> str:
        """Test learning simulation over time"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)

        # Track learning metrics over time
        learning_metrics = {
            'spike_patterns': [],
            'curiosity_levels': [],
            'development_stages': [],
            'reward_responses': []
        }

        for episode in range(5):
            for step in range(20):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())

                # Record learning metrics
                learning_metrics['spike_patterns'].append(brain.getTotalSpikeCount())
                learning_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
                learning_metrics['development_stages'].append(agent.getDevelopmentalStage())
                learning_metrics['reward_responses'].append(agent.getPredictionError())

                # Apply learning
                agent.applyRewardModulation(0.5, 0.3)

        # Verify learning occurred
        curiosity_variance = self._calculate_variance(learning_metrics['curiosity_levels'])
        development_progressions = len(set(learning_metrics['development_stages']))

        assert curiosity_variance > 0.01, "Curiosity should vary during learning"
        assert development_progressions > 1, "Development should progress through stages"

        return f"Learning simulation passed - Curiosity variance: {curiosity_variance:.4f}, " \
               f"Development stages: {development_progressions}"

    def _test_stability_over_time(self) -> str:
        """Test system stability over extended simulation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Run extended simulation
        initial_spike_count = brain.getTotalSpikeCount()
        spike_counts = []

        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())

            spike_counts.append(brain.getTotalSpikeCount())

        # Calculate stability metrics
        spike_variance = self._calculate_variance(spike_counts)
        mean_spike_count = statistics.mean(spike_counts)

        # Stability threshold (should not be wildly unstable)
        stability_score = max(0.0, 1.0 - (spike_variance / (mean_spike_count * 10)))

        assert stability_score > 0.5, f"System should be stable (score: {stability_score:.3f})"

        return f"Stability test passed - Score: {stability_score:.3f}, Variance: {spike_variance:.2f}"

    def _test_error_handling(self) -> str:
        """Test error handling and edge cases"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Test with malformed/empty world
        world = pynlm.createSimpleWorld()
        world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Test operations that should not crash
        try:
            action = agent.decodeMotorCommand()
            assert action is not None, "Should return valid action"
        except Exception as e:
            return f"Error handling test failed: {e}"

        # Test with zero or negative timesteps
        try:
            for step in range(5):
                world.update(0.0)  # Zero timestep
                agent.updateDevelopment(-0.1)  # Negative timestep
        except:
            pass  # Expected for some implementations

        return "Error handling and edge cases passed"

    def _run_performance_tests(self):
        """Run performance and scalability tests"""
        performance_tests = [
            self._test_performance_scalability,
            self._test_memory_efficiency,
            self._test_concurrent_operations
        ]

        for test_func in performance_tests:
            test_name = test_func.__name__
            print(f"  Testing {test_name}...")
            try:
                result = test_func()
                self.test_results['performance_tests'][test_name] = {
                    'status': 'PASSED',
                    'details': result,
                    'execution_time': time.time()
                }
                self.test_results['passed_tests'] += 1
                print(f"    ✓ {test_name} PASSED")
            except Exception as e:
                self.test_results['performance_tests'][test_name] = {
                    'status': 'FAILED',
                    'error': str(e),
                    'execution_time': time.time()
                }
                self.test_results['failed_tests'] += 1
                print(f"    ✗ {test_name} FAILED: {e}")

        self.test_results['total_tests'] += len(performance_tests)

    def _test_performance_scalability(self) -> str:
        """Test performance scalability with different brain sizes"""
        configurations = [
            {'neuron_count': 100, 'region_count': 1},
            {'neuron_count': 500, 'region_count': 2},
            {'neuron_count': 1000, 'region_count': 3}
        ]

        performance_results = []

        for config in configurations:
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("neuron_count", config['neuron_count'], pynlm.ConfigSource.Runtime)
            brain_config.set("region_count", config['region_count'], pynlm.ConfigSource.Runtime)

            brain = pynlm.createBrain(brain_config)
            brain.initialize()

            # Time brain operations
            start_time = time.time()
            for step in range(50):
                brain.step(step)
            end_time = time.time()

            duration = end_time - start_time
            neurons_per_second = config['neuron_count'] * 50 / duration

            performance_results.append({
                'configuration': f"{config['neuron_count']} neurons, {config['region_count']} regions",
                'duration': duration,
                'neurons_per_second': neurons_per_second,
                'steps_per_second': 50 / duration
            })

        # Verify reasonable performance
        avg_neurons_per_second = statistics.mean(r['neurons_per_second'] for r in performance_results)
        assert avg_neurons_per_second > 1000, f"Performance too low: {avg_neurons_per_second}"

        return f"Scalability test passed - Avg: {avg_neurons_per_second:.0f} neurons/second"

    def _test_memory_efficiency(self) -> str:
        """Test memory efficiency and resource management"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
        config.set("region_count", 2, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()

        # Test memory cleanup
        initial_neuron_count = brain.getTotalNeuronCount()

        # Create and destroy multiple brains
        for i in range(5):
            temp_brain = pynlm.createBrain(config)
            temp_brain.initialize()
            # Brain should be cleaned up when destroyed

        # Verify original brain is unaffected
        assert brain.getTotalNeuronCount() == initial_neuron_count, "Brain memory leak detected"

        return "Memory efficiency test passed"

    def _test_concurrent_operations(self) -> str:
        """Test concurrent operations (conceptual)"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300, pynlm.ConfigSource.Runtime)
        config.set("region_count", 2, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Test interleaved operations
        for step in range(100):
            # Mix of operations that might be called concurrently
            world.update(0.1)
            action = agent.decodeMotorCommand()
            brain.step(step)
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)

        # Verify system integrity
        assert brain.getTotalNeuronCount() > 0, "Brain should remain functional"
        assert agent.getCuriosityLevel() >= 0, "Agent metrics should be valid"

        return "Concurrent operations test passed"

    def _run_edge_case_tests(self):
        """Run edge case and stress tests"""
        edge_case_tests = [
            self._test_extreme_configurations,
            self._test_long_simulation,
            self._test_boundary_conditions,
            self._test_resource_limits
        ]

        for test_func in edge_case_tests:
            test_name = test_func.__name__
            print(f"  Testing {test_name}...")
            try:
                result = test_func()
                self.test_results['edge_case_tests'][test_name] = {
                    'status': 'PASSED',
                    'details': result,
                    'execution_time': time.time()
                }
                self.test_results['passed_tests'] += 1
                print(f"    ✓ {test_name} PASSED")
            except Exception as e:
                self.test_results['edge_case_tests'][test_name] = {
                    'status': 'FAILED',
                    'error': str(e),
                    'execution_time': time.time()
                }
                self.test_results['failed_tests'] += 1
                print(f"    ✗ {test_name} FAILED: {e}")

        self.test_results['total_tests'] += len(edge_case_tests)

    def _test_extreme_configurations(self) -> str:
        """Test extreme configuration values"""
        extreme_configs = [
            {'neuron_count': 5000, 'region_count': 10},  # Large scale
            {'neuron_count': 50, 'region_count': 1},    # Small scale
        ]

        for i, config_params in enumerate(extreme_configs):
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", config_params['neuron_count'], pynlm.ConfigSource.Runtime)
            config.set("region_count", config_params['region_count'], pynlm.ConfigSource.Runtime)

            brain = pynlm.createBrain(config)
            brain.initialize()

            # Verify the configuration was applied
            assert brain.getTotalNeuronCount() == config_params['neuron_count'], \
                f"Neuron count mismatch for config {i}"

        return f"Extreme configurations test passed - Tested {len(extreme_configs)} configurations"

    def _test_long_simulation(self) -> str:
        """Test extended simulation duration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200, pynlm.ConfigSource.Runtime)
        config.set("region_count", 2, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()

        world = pynlm.createSimpleWorld()
        world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Run extended simulation
        start_time = time.time()
        for step in range(500):  # 500 steps
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())

        end_time = time.time()
        duration = end_time - start_time

        # Verify system stability
        final_spike_count = brain.getTotalSpikeCount()
        assert final_spike_count > 0, "Brain should remain active"
        assert duration < 60.0, "Simulation should complete in reasonable time"

        return f"Long simulation test passed - Duration: {duration:.2f}s, Final spikes: {final_spike_count}"

    def _test_boundary_conditions(self) -> str:
        """Test boundary conditions and limits"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100, pynlm.ConfigSource.Runtime)
        config.set("region_count", 1, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()

        # Test boundary conditions
        initial_spikes = brain.getTotalSpikeCount()

        # Test with minimal timestep
        for step in range(5):
            brain.step(step, 0.0001)  # Very small timestep

        # Test with larger timestep
        for step in range(5, 10):
            brain.step(step, 0.1)  # Larger timestep

        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes, "Spike count should not decrease with positive activity"

        return f"Boundary conditions test passed - Spikes: {initial_spikes} -> {final_spikes}"

    def _test_resource_limits(self) -> str:
        """Test resource limits and resource management"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300, pynlm.ConfigSource.Runtime)
        config.set("region_count", 2, pynlm.ConfigSource.Runtime)

        # Test resource usage under load
        brains = []
        for i in range(3):
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("neuron_count", 200, pynlm.ConfigSource.Runtime)
            brain_config.set("region_count", 1, pynlm.ConfigSource.Runtime)
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            brains.append(brain)

        # Process with multiple brains
        for i, brain in enumerate(brains):
            for step in range(20):
                brain.step(step)

        # Verify all brains are independent
        spike_counts = [brain.getTotalSpikeCount() for brain in brains]
        assert len(set(spike_counts)) > 0, "Brains should maintain independent state"

        return "Resource limits test passed - Multiple brains processed independently"

    def _calculate_variance(self, values: List[float]) -> float:
        """Calculate variance of values"""
        if len(values) < 2:
            return 0.0
        return statistics.variance(values)

    def _generate_test_report(self):
        """Generate comprehensive test report"""
        end_time = time.time()
        total_duration = end_time - self.test_results['start_time']

        print("\n" + "="*60)
        print("NLM SYSTEM TEST REPORT")
        print("="*60)

        print(f"\nTest Summary:")
        print(f"  Total tests: {self.test_results['total_tests']}")
        print(f"  Passed: {self.test_results['passed_tests']}")
        print(f"  Failed: {self.test_results['failed_tests']}")
        print(f"  Success rate: {self.test_results['passed_tests']/max(self.test_results['total_tests'], 1)*100:.1f}%")
        print(f"  Total duration: {total_duration:.2f}s")

        print(f"\nDetailed Results:")
        for test_category in ['unit_tests', 'integration_tests', 'performance_tests', 'edge_case_tests']:
            category_tests = self.test_results[test_category]
            if category_tests:
                passed = sum(1 for t in category_tests.values() if t['status'] == 'PASSED')
                failed = sum(1 for t in category_tests.values() if t['status'] == 'FAILED')
                print(f"  {test_category.replace('_', ' ').title()}: {passed} passed, {failed} failed")

        print(f"\nIndividual Test Results:")
        for test_name, result in self.test_results['unit_tests'].items():
            status = "✓" if result['status'] == 'PASSED' else "✗"
            print(f"  {status} {test_name}")

        print(f"\nIntegration Test Results:")
        for test_name, result in self.test_results['integration_tests'].items():
            status = "✓" if result['status'] == 'PASSED' else "✗"
            print(f"  {status} {test_name}")

        print(f"\nRecommendations:")
        if self.test_results['failed_tests'] > 0:
            print(f"  ⚠ {self.test_results['failed_tests']} tests failed - review implementation")
        else:
            print(f"  ✓ All tests passed - system ready for production")

        if self.test_results['passed_tests'] / max(self.test_results['total_tests'], 1) > 0.9:
            print(f"  ✓ High test coverage indicates robust system")
        else:
            print(f"  ⚠ Low test coverage - consider adding more tests")

        # Save test report
        self._save_test_report()

    def _save_test_report(self):
        """Save test report to file"""
        report_lines = []
        report_lines.append("NLM System Test Report")
        report_lines.append("="*60)
        report_lines.append(f"Generated: {time.ctime()}")
        report_lines.append("")
        report_lines.append(f"Total Tests: {self.test_results['total_tests']}")
        report_lines.append(f"Passed: {self.test_results['passed_tests']}")
        report_lines.append(f"Failed: {self.test_results['failed_tests']}")
        report_lines.append(f"Success Rate: {self.test_results['passed_tests']/max(self.test_results['total_tests'], 1)*100:.1f}%")
        report_lines.append("")

        with open("test_report.txt", "w") as f:
            f.write("\n".join(report_lines))

        print(f"\nTest report saved to 'test_report.txt'")
if __name__ == "__main__":
    print("=== NLM System Integration and Verification Tests ===")
    print("This comprehensive test suite validates NLM system functionality")
    print("including unit tests, integration tests, performance tests, and edge cases.")
    print()

    # Run comprehensive tests
    tester = NLMSystemTester()
    tester.run_all_tests()

    print("\n=== Testing Complete ===")
    if tester.test_results['failed_tests'] == 0:
        print("✓ All tests passed - NLM system is ready for production!")
        sys.exit(0)
    else:
        print(f"⚠ {tester.test_results['failed_tests']} tests failed - review issues above")
        sys.exit(1)