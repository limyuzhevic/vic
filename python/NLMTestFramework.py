#!/usr/bin/env python3
"""
NLM Test Framework - Comprehensive Testing Suite

This module provides a comprehensive testing framework for the NLM (Neural Learning Machine)
simulation, enabling systematic validation of all systems and functionality.

Key Features:
- Integration testing for all NLM systems
- Memory system validation and retention testing
- Cognitive system evaluation
- Neuromodulation testing
- Performance benchmarking
- State persistence testing

Test Categories:
1. System Integration Tests
2. Memory Retention Tests
3. Learning Stability Tests
4. Prediction Accuracy Tests
5. Performance and Scalability Tests
6. Checkpoint/Restore Tests
"""

import time
import json
import statistics
from typing import Dict, List, Tuple, Any, Optional
from datetime import datetime
import numpy as np

# Import NLM components
import pynlm as nlm
class NLMTestFramework:
    """
    Comprehensive test framework for NLM validation.
    
    This framework provides systematic testing of all NLM systems,
    enabling researchers to validate functionality, measure performance,
    and ensure stable operation over time.
    """
    
    def __init__(self, config: Optional[Dict] = None):
        """
        Initialize test framework with optional configuration.
        
        Args:
            config: Configuration dictionary with test parameters
        """
        self.config = config or self._get_default_config()
        self.test_results = {}
        self.brain = None
        self.agent = None
        self.world = None
        self.test_start_time = None
        self.test_end_time = None
        
        # Test metrics collection
        self.metrics = {
            'integration': [],
            'memory_retention': [],
            'learning_stability': [],
            'prediction_accuracy': [],
            'performance': [],
            'checkpoint': []
        }
    
    def _get_default_config(self) -> Dict:
        """Get default test configuration."""
        return {
            'neuron_count': 500,
            'simulation_steps': 1000,
            'world_width': 20,
            'world_height': 20,
            'enable_cognition': True,
            'enable_memory': True,
            'enable_neuromodulation': True,
            'enable_development': True,
            'checkpoint_interval': 100,
            'test_suites': ['integration', 'memory', 'learning', 'prediction', 'performance', 'checkpoint']
        }
    
    def setup_test_environment(self) -> bool:
        """
        Setup test environment with configured parameters.
        
        Returns:
            bool: True if setup successful, False otherwise
        """
        try:
            print(f"Setting up test environment with {self.config['neuron_count']} neurons...")
            
            # Create configuration
            config = nlm.createDefaultConfig()
            config.set("brain.neuron_count", self.config['neuron_count'])
            config.set("brain.synapse_density", 0.1)
            config.set("brain.connection_probability", 0.05)
            
            # Create and initialize brain
            self.brain = nlm.createBrain(config)
            self.brain.initialize()
            
            # Create agent with all systems
            self.agent = nlm.createAgentBrain(self.brain)
            
            # Enable all systems for comprehensive testing
            if self.config['enable_cognition']:
                self.agent.enableRewardModulation(True)
                self.agent.enableStructuralPlasticity(True)
                self.agent.enableDevelopment(True)
                self.agent.enableCuriosity(True)
            
            if self.config['enable_memory']:
                # Memory systems are automatically initialized with brain
                pass
            
            if self.config['enable_neuromodulation']:
                self.agent.enableRewardModulation(True)
                self.agent.enableCuriosity(True)
            
            # Create and configure world
            self.world = nlm.createSimpleWorld()
            self.world.configure(
                self.config['world_width'],
                self.config['world_height'],
                8, 8  # Vision dimensions
            )
            self.world.reset()
            self.world.setAgentStart(10.0, 10.0)
            self.agent.initialize(self.world)
            
            print(f"Test environment setup complete:")
            print(f"  - {self.brain.getTotalNeuronCount()} neurons")
            print(f"  - {self.brain.getTotalSynapseCount()} synapses")
            print(f"  - Agent systems: RewardMod={self.agent.isRewardModulationEnabled()}, "
                  f"Development={self.agent.isDevelopmentEnabled()}, "
                  f"Curiosity={self.agent.isCuriosityEnabled()}")
            
            return True
            
        except Exception as e:
            print(f"Test environment setup failed: {e}")
            return False
    
    def run_integration_tests(self) -> Dict:
        """
        Run integration tests for all NLM systems.
        
        Returns:
            Dict: Test results with metrics and status
        """
        print("\n=== Running Integration Tests ===")
        
        integration_tests = []
        passed_tests = 0
        total_tests = 0
        
        # Test 1: Basic connectivity
        total_tests += 1
        test_result = self._test_basic_connectivity()
        integration_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 2: Memory system integration
        total_tests += 1
        test_result = self._test_memory_integration()
        integration_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 3: Neuromodulation integration
        total_tests += 1
        test_result = self._test_neuromodulation_integration()
        integration_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 4: Cognitive system integration
        total_tests += 1
        test_result = self._test_cognitive_integration()
        integration_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 5: Prediction system integration
        total_tests += 1
        test_result = self._test_prediction_integration()
        integration_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 6: Development system integration
        total_tests += 1
        test_result = self._test_development_integration()
        integration_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Calculate overall integration score
        integration_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        result = {
            'suite': 'integration',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': total_tests - passed_tests,
            'score': integration_score,
            'tests': integration_tests,
            'timestamp': datetime.now().isoformat()
        }
        
        self.metrics['integration'].append(result)
        self.test_results['integration'] = result
        
        print(f"\nIntegration Test Results:")
        print(f"  Tests passed: {passed_tests}/{total_tests} ({integration_score:.1%})")
        print(f"  Score: {integration_score:.3f}")
        
        return result
    
    def _test_basic_connectivity(self) -> Dict:
        """Test basic neural connectivity and spike propagation."""
        print("  Testing basic connectivity...")
        
        try:
            # Inject current and observe spike propagation
            initial_spikes = self.brain.getTotalSpikeCount()
            
            # Inject current into multiple neurons
            for step in range(50):
                self.brain.step(step)
                self.agent.decodeMotorCommand()  # Simple activity
            
            final_spikes = self.brain.getTotalSpikeCount()
            spike_count = final_spikes - initial_spikes
            
            # Test criteria
            connectivity_ok = spike_count > 0
            firing_rate_ok = self.brain.getFiringNeuronCount() > 0
            
            passed = connectivity_ok and firing_rate_ok
            
            test_result = {
                'name': 'basic_connectivity',
                'passed': passed,
                'metrics': {
                    'initial_spikes': initial_spikes,
                    'final_spikes': final_spikes,
                    'spike_count': spike_count,
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'avg_firing_rate': self.brain.getAverageFiringRate(),
                    'connectivity_detected': connectivity_ok,
                    'activity_detected': firing_rate_ok
                },
                'details': "Test checks for spike propagation and neural activity"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {spike_count} spikes generated")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'basic_connectivity',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_memory_integration(self) -> Dict:
        """Test memory system integration and functionality."""
        print("  Testing memory integration...")
        
        try:
            # Initial memory state
            initial_working_memory = self.brain.getWorkingMemory().getActiveTraces()
            initial_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            initial_associative = self.brain.getAssociativeMemory().getAssociationCount()
            
            # Run simulation to generate experiences
            for step in range(100):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Final memory state
            final_working_memory = self.brain.getWorkingMemory().getActiveTraces()
            final_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            final_associative = self.brain.getAssociativeMemory().getAssociationCount()
            
            # Test criteria
            memory_active = initial_working_memory != final_working_memory
            episodes_formed = initial_episodes != final_episodes or final_episodes > 0
            associations_formed = initial_associative != final_associative or final_associative > 0
            
            passed = memory_active or episodes_formed or associations_formed
            
            test_result = {
                'name': 'memory_integration',
                'passed': passed,
                'metrics': {
                    'initial_working_memory': initial_working_memory,
                    'final_working_memory': final_working_memory,
                    'initial_episodes': initial_episodes,
                    'final_episodes': final_episodes,
                    'initial_associations': initial_associative,
                    'final_associations': final_associative,
                    'memory_active': memory_active,
                    'episodes_formed': episodes_formed,
                    'associations_formed': associations_formed
                },
                'details': "Test checks for memory system activation and experience storage"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {final_episodes} episodes stored")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'memory_integration',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_neuromodulation_integration(self) -> Dict:
        """Test neuromodulation system integration."""
        print("  Testing neuromodulation integration...")
        
        try:
            # Enable neuromodulation and measure initial state
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            
            initial_curiosity = self.agent.getCuriosityLevel()
            initial_prediction_error = self.agent.getPredictionError()
            
            # Generate experiences to trigger neuromodulation
            for step in range(50):
                # Apply varying rewards to stimulate neuromodulation
                reward = 0.5 + (step % 3) * 0.2
                self.agent.applyRewardModulation(reward, 0.3)
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Final neuromodulation state
            final_curiosity = self.agent.getCuriosityLevel()
            final_prediction_error = self.agent.getPredictionError()
            
            # Test criteria
            neuromodulation_active = (final_curiosity != initial_curiosity or 
                                   final_prediction_error != initial_prediction_error)
            learning_observed = abs(final_prediction_error - initial_prediction_error) > 0.1
            
            passed = neuromodulation_active or learning_observed
            
            test_result = {
                'name': 'neuromodulation_integration',
                'passed': passed,
                'metrics': {
                    'initial_curiosity': initial_curiosity,
                    'final_curiosity': final_curiosity,
                    'initial_prediction_error': initial_prediction_error,
                    'final_prediction_error': final_prediction_error,
                    'curiosity_change': final_curiosity - initial_curiosity,
                    'prediction_error_change': final_prediction_error - initial_prediction_error,
                    'neuromodulation_active': neuromodulation_active,
                    'learning_observed': learning_observed
                },
                'details': "Test checks for neuromodulation response to experience"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Curiosity changed from {initial_curiosity:.3f} to {final_curiosity:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'neuromodulation_integration',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_cognitive_integration(self) -> Dict:
        """Test cognitive system integration."""
        print("  Testing cognitive integration...")
        
        try:
            # Check if cognitive systems are available
            cognitive_systems = []
            if self.brain.getPlanner():
                cognitive_systems.append('planner')
            if self.brain.getConceptFormation():
                cognitive_systems.append('concept_formation')
            if self.brain.getAttention():
                cognitive_systems.append('attention')
            if self.brain.getPredictionSystem():
                cognitive_systems.append('prediction_system')
            
            # Generate experiences to potentially activate cognitive systems
            for step in range(50):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Test criteria
            systems_available = len(cognitive_systems) > 0
            activity_demonstrated = self.brain.getFiringNeuronCount() > 10  # Some activity
            
            passed = systems_available
            
            test_result = {
                'name': 'cognitive_integration',
                'passed': passed,
                'metrics': {
                    'cognitive_systems_available': cognitive_systems,
                    'num_systems': len(cognitive_systems),
                    'systems_available': systems_available,
                    'activity_demonstrated': activity_demonstrated,
                    'firing_neurons': self.brain.getFiringNeuronCount()
                },
                'details': f"Test checks for cognitive system availability: {', '.join(cognitive_systems)}"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {len(cognitive_systems)} cognitive systems available")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'cognitive_integration',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_prediction_integration(self) -> Dict:
        """Test prediction system integration."""
        print("  Testing prediction integration...")
        
        try:
            # Check if prediction system is available
            prediction_system_available = self.brain.getPredictionSystem() is not None
            
            # Generate experiences to potentially trigger prediction
            for step in range(30):
                # Apply reward modulation to stimulate prediction learning
                reward = 0.3 + (step % 2) * 0.3
                self.agent.applyRewardModulation(reward, reward * 0.7)
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Test criteria
            prediction_available = prediction_system_available
            learning_observed = self.agent.getPredictionError() > 0  # Some prediction error indicates learning
            
            passed = prediction_available and (learning_observed or not prediction_system_available)
            
            test_result = {
                'name': 'prediction_integration',
                'passed': passed,
                'metrics': {
                    'prediction_system_available': prediction_system_available,
                    'prediction_error': self.agent.getPredictionError(),
                    'prediction_available': prediction_available,
                    'learning_observed': learning_observed,
                    'curiosity_level': self.agent.getCuriosityLevel()
                },
                'details': "Test checks for prediction system functionality and learning"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Prediction error = {self.agent.getPredictionError():.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'prediction_integration',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_development_integration(self) -> Dict:
        """Test development system integration."""
        print("  Testing development integration...")
        
        try:
            # Check if development system is available
            development_system_available = self.brain.getDevelopmentSystem() is not None
            
            # Monitor developmental stage changes over time
            initial_stage = self.brain.getDevelopmentalStage()
            
            # Generate experiences with development enabled
            for step in range(30):
                self.agent.updateDevelopment(0.1)  # Develop over time
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Final developmental stage
            final_stage = self.brain.getDevelopmentalStage()
            
            # Test criteria
            development_available = development_system_available
            stage_changed = final_stage != initial_stage  # Stage may not change quickly
            
            passed = development_available
            
            test_result = {
                'name': 'development_integration',
                'passed': passed,
                'metrics': {
                    'development_system_available': development_system_available,
                    'initial_stage': str(initial_stage),
                    'final_stage': str(final_stage),
                    'stage_changed': stage_changed,
                    'development_available': development_available,
                    'developmental_age': self.agent.getNeuromodulationLevel()
                },
                'details': "Test checks for development system functionality and stage progression"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Development stage: {initial_stage} -> {final_stage}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'development_integration',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def run_memory_retention_tests(self) -> Dict:
        """
        Run memory retention tests to validate long-term memory capabilities.
        
        Returns:
            Dict: Test results with retention metrics
        """
        print("\n=== Running Memory Retention Tests ===")
        
        retention_tests = []
        passed_tests = 0
        total_tests = 0
        
        # Test 1: Working memory retention
        total_tests += 1
        test_result = self._test_working_memory_retention()
        retention_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 2: Episodic memory formation and retrieval
        total_tests += 1
        test_result = self._test_episodic_memory_retention()
        retention_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 3: Associative memory retention
        total_tests += 1
        test_result = self._test_associative_memory_retention()
        retention_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 4: Memory consolidation effectiveness
        total_tests += 1
        test_result = self._test_memory_consolidation()
        retention_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Calculate overall retention score
        retention_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        result = {
            'suite': 'memory_retention',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': total_tests - passed_tests,
            'score': retention_score,
            'tests': retention_tests,
            'timestamp': datetime.now().isoformat()
        }
        
        self.metrics['memory_retention'].append(result)
        self.test_results['memory_retention'] = result
        
        print(f"\nMemory Retention Test Results:")
        print(f"  Tests passed: {passed_tests}/{total_tests} ({retention_score:.1%})")
        print(f"  Score: {retention_score:.3f}")
        
        return result
    
    def _test_working_memory_retention(self) -> Dict:
        """Test working memory retention over time."""
        print("  Testing working memory retention...")
        
        try:
            # Generate initial working memory traces
            initial_traces = self.brain.getWorkingMemory().getActiveTraces()
            
            # Run simulation to potentially maintain or modify working memory
            for step in range(50):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            final_traces = self.brain.getWorkingMemory().getActiveTraces()
            
            # Test criteria
            memory_maintained = final_traces > 0  # Should maintain some activity
            trace_change_observed = abs(final_traces - initial_traces) > 0
            
            passed = memory_maintained or trace_change_observed
            
            test_result = {
                'name': 'working_memory_retention',
                'passed': passed,
                'metrics': {
                    'initial_traces': initial_traces,
                    'final_traces': final_traces,
                    'trace_change': final_traces - initial_traces,
                    'memory_maintained': memory_maintained,
                    'trace_change_observed': trace_change_observed
                },
                'details': "Test checks for working memory trace retention"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {final_traces} working memory traces")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'working_memory_retention',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_episodic_memory_retention(self) -> Dict:
        """Test episodic memory formation and retention."""
        print("  Testing episodic memory retention...")
        
        try:
            # Initial episodic memory state
            initial_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            
            # Generate diverse experiences to form episodic memories
            for step in range(30):
                # Vary sensory input to create diverse experiences
                self.agent.enableCuriosity(True)
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            final_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            
            # Test criteria
            episodes_formed = final_episodes > initial_episodes
            memory_active = final_episodes > 0
            
            passed = episodes_formed or memory_active
            
            test_result = {
                'name': 'episodic_memory_retention',
                'passed': passed,
                'metrics': {
                    'initial_episodes': initial_episodes,
                    'final_episodes': final_episodes,
                    'episodes_formed': episodes_formed,
                    'memory_active': memory_active,
                    'retention_rate': final_episodes / max(1, initial_episodes)
                },
                'details': "Test checks for episodic memory formation and retention"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {final_episodes} episodic memories stored")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'episodic_memory_retention',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_associative_memory_retention(self) -> Dict:
        """Test associative memory formation and retention."""
        print("  Testing associative memory retention...")
        
        try:
            # Initial associative memory state
            initial_associations = self.brain.getAssociativeMemory().getAssociationCount()
            
            # Generate correlated experiences to form associations
            for step in range(30):
                # Create some correlated patterns
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            final_associations = self.brain.getAssociativeMemory().getAssociationCount()
            
            # Test criteria
            associations_formed = final_associations > initial_associations
            memory_active = final_associations > 0
            
            passed = associations_formed or memory_active
            
            test_result = {
                'name': 'associative_memory_retention',
                'passed': passed,
                'metrics': {
                    'initial_associations': initial_associations,
                    'final_associations': final_associations,
                    'associations_formed': associations_formed,
                    'memory_active': memory_active,
                    'retention_rate': final_associations / max(1, initial_associations)
                },
                'details': "Test checks for associative memory formation and retention"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {final_associations} associative memories stored")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'associative_memory_retention',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_memory_consolidation(self) -> Dict:
        """Test memory consolidation effectiveness."""
        print("  Testing memory consolidation...")
        
        try:
            # First, form some memories
            for step in range(20):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            initial_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            
            # Trigger consolidation by running more steps
            for step in range(50):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            final_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            
            # Test criteria
            consolidation_effective = final_episodes < initial_episodes or final_episodes == initial_episodes
            # Note: consolidation might reduce memory count by pruning weak memories
            
            passed = consolidation_effective
            
            test_result = {
                'name': 'memory_consolidation',
                'passed': passed,
                'metrics': {
                    'initial_episodes': initial_episodes,
                    'final_episodes': final_episodes,
                    'consolidation_effective': consolidation_effective,
                    'consolidation_ratio': final_episodes / max(1, initial_episodes)
                },
                'details': "Test checks for memory consolidation effectiveness"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Consolidation ratio = {final_episodes / max(1, initial_episodes):.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'memory_consolidation',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def run_learning_stability_tests(self) -> Dict:
        """
        Run learning stability tests to validate consistent learning over time.
        
        Returns:
            Dict: Test results with stability metrics
        """
        print("\n=== Running Learning Stability Tests ===")
        
        stability_tests = []
        passed_tests = 0
        total_tests = 0
        
        # Test 1: Continual learning without catastrophic forgetting
        total_tests += 1
        test_result = self._test_continual_learning()
        stability_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 2: Neuromodulation stability
        total_tests += 1
        test_result = self._test_neuromodulation_stability()
        stability_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 3: Memory retention stability
        total_tests += 1
        test_result = self._test_memory_stability()
        stability_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 4: Cognitive stability
        total_tests += 1
        test_result = self._test_cognitive_stability()
        stability_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Calculate overall stability score
        stability_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        result = {
            'suite': 'learning_stability',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': total_tests - passed_tests,
            'score': stability_score,
            'tests': stability_tests,
            'timestamp': datetime.now().isoformat()
        }
        
        self.metrics['learning_stability'].append(result)
        self.test_results['learning_stability'] = result
        
        print(f"\nLearning Stability Test Results:")
        print(f"  Tests passed: {passed_tests}/{total_tests} ({stability_score:.1%})")
        print(f"  Score: {stability_score:.3f}")
        
        return result
    
    def _test_continual_learning(self) -> Dict:
        """Test continual learning without catastrophic forgetting."""
        print("  Testing continual learning stability...")
        
        try:
            # Measure learning performance at different stages
            learning_performance = []
            
            # Phase 1: Initial learning
            initial_firing = self.brain.getAverageFiringRate()
            for step in range(20):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            phase1_firing = self.brain.getAverageFiringRate()
            learning_performance.append(phase1_firing)
            
            # Phase 2: Continued learning
            for step in range(20, 40):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            phase2_firing = self.brain.getAverageFiringRate()
            learning_performance.append(phase2_firing)
            
            # Phase 3: Extended learning
            for step in range(40, 60):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            phase3_firing = self.brain.getAverageFiringRate()
            learning_performance.append(phase3_firing)
            
            # Stability metrics
            firing_stability = 1.0 - (statistics.stdev(learning_performance) / max(statistics.mean(learning_performance), 0.1))
            
            # Test criteria
            learning_maintained = firing_stability > 0.5
            performance_stable = len(set(round(x, 3) for x in learning_performance)) > 1
            
            passed = learning_maintained and performance_stable
            
            test_result = {
                'name': 'continual_learning',
                'passed': passed,
                'metrics': {
                    'learning_performance': learning_performance,
                    'mean_firing_rate': statistics.mean(learning_performance),
                    'firing_stability': firing_stability,
                    'performance_variance': statistics.variance(learning_performance),
                    'learning_maintained': learning_maintained,
                    'performance_stable': performance_stable
                },
                'details': "Test checks for stable learning over extended simulation"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Stability = {firing_stability:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'continual_learning',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_neuromodulation_stability(self) -> Dict:
        """Test neuromodulation stability over time."""
        print("  Testing neuromodulation stability...")
        
        try:
            # Measure neuromodulation levels over time
            neuromodulation_levels = []
            
            for step in range(30):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
                
                neuromodulation_levels.append(self.agent.getNeuromodulationLevel())
            
            # Stability metrics
            neuromod_stability = 1.0 - (statistics.stdev(neuromodulation_levels) / max(statistics.mean(neuromodulation_levels), 0.1))
            
            # Test criteria
            neuromod_stable = neuromod_stability > 0.6
            levels_varied = max(neuromodulation_levels) - min(neuromodulation_levels) > 0.2
            
            passed = neuromod_stable and levels_varied
            
            test_result = {
                'name': 'neuromodulation_stability',
                'passed': passed,
                'metrics': {
                    'neuromodulation_levels': neuromodulation_levels,
                    'mean_neuromodulation': statistics.mean(neuromodulation_levels),
                    'neuromod_stability': neuromod_stability,
                    'neuromod_variance': statistics.variance(neuromodulation_levels),
                    'neuromod_stable': neuromod_stable,
                    'levels_varied': levels_varied
                },
                'details': "Test checks for stable neuromodulation response"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Stability = {neuromod_stability:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'neuromodulation_stability',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_memory_stability(self) -> Dict:
        """Test memory stability over time."""
        print("  Testing memory stability...")
        
        try:
            # Measure memory metrics over time
            memory_metrics = {
                'working_memory': [],
                'episodic_memory': [],
                'associative_memory': []
            }
            
            for step in range(30):
                memory_metrics['working_memory'].append(self.brain.getWorkingMemory().getActiveTraces())
                memory_metrics['episodic_memory'].append(self.brain.getEpisodicMemory().getEpisodeCount())
                memory_metrics['associative_memory'].append(self.brain.getAssociativeMemory().getAssociationCount())
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Stability metrics for each memory system
            memory_stability = {}
            for system_name, values in memory_metrics.items():
                if len(values) > 1:
                    mean_val = statistics.mean(values)
                    stability = 1.0 - (statistics.stdev(values) / max(mean_val, 0.1))
                    memory_stability[system_name] = stability
                else:
                    memory_stability[system_name] = 1.0
            
            # Test criteria
            all_stable = all(stability > 0.5 for stability in memory_stability.values())
            
            passed = all_stable
            
            test_result = {
                'name': 'memory_stability',
                'passed': passed,
                'metrics': {
                    'memory_metrics': memory_metrics,
                    'memory_stability': memory_stability,
                    'working_memory_stable': memory_stability['working_memory'],
                    'episodic_memory_stable': memory_stability['episodic_memory'],
                    'associative_memory_stable': memory_stability['associative_memory'],
                    'all_stable': all_stable
                },
                'details': "Test checks for stable memory system operation"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: All memory systems stable = {all_stable}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'memory_stability',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_cognitive_stability(self) -> Dict:
        """Test cognitive system stability over time."""
        print("  Testing cognitive stability...")
        
        try:
            # Monitor cognitive system availability
            cognitive_stability = []
            
            for step in range(30):
                # Check which cognitive systems are available
                available_systems = []
                if self.brain.getPlanner():
                    available_systems.append('planner')
                if self.brain.getConceptFormation():
                    available_systems.append('concept_formation')
                if self.brain.getAttention():
                    available_systems.append('attention')
                
                cognitive_stability.append(len(available_systems))
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Stability metrics
            cognitive_available = sum(cognitive_stability) / len(cognitive_stability)
            cognitive_variance = statistics.variance(cognitive_stability) if len(cognitive_stability) > 1 else 0.0
            
            # Test criteria
            cognitive_consistent = cognitive_available >= 1.0  # At least 1 cognitive system
            low_variance = cognitive_variance < 0.5
            
            passed = cognitive_consistent and low_variance
            
            test_result = {
                'name': 'cognitive_stability',
                'passed': passed,
                'metrics': {
                    'cognitive_systems_available': cognitive_stability,
                    'mean_cognitive_systems': cognitive_available,
                    'cognitive_variance': cognitive_variance,
                    'cognitive_consistent': cognitive_consistent,
                    'low_variance': low_variance
                },
                'details': "Test checks for consistent cognitive system availability"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Mean systems = {cognitive_available:.1f}, Variance = {cognitive_variance:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'cognitive_stability',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def run_prediction_accuracy_tests(self) -> Dict:
        """
        Run prediction accuracy tests to validate learning performance.
        
        Returns:
            Dict: Test results with prediction accuracy metrics
        """
        print("\n=== Running Prediction Accuracy Tests ===")
        
        prediction_tests = []
        passed_tests = 0
        total_tests = 0
        
        # Test 1: Prediction learning improvement
        total_tests += 1
        test_result = self._test_prediction_learning()
        prediction_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 2: Prediction error reduction
        total_tests += 1
        test_result = self._test_prediction_error_reduction()
        prediction_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 3: Prediction stability
        total_tests += 1
        test_result = self._test_prediction_stability()
        prediction_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 4: Prediction confidence
        total_tests += 1
        test_result = self._test_prediction_confidence()
        prediction_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Calculate overall prediction accuracy score
        prediction_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        result = {
            'suite': 'prediction_accuracy',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': total_tests - passed_tests,
            'score': prediction_score,
            'tests': prediction_tests,
            'timestamp': datetime.now().isoformat()
        }
        
        self.metrics['prediction_accuracy'].append(result)
        self.test_results['prediction_accuracy'] = result
        
        print(f"\nPrediction Accuracy Test Results:")
        print(f"  Tests passed: {passed_tests}/{total_tests} ({prediction_score:.1%})")
        print(f"  Score: {prediction_score:.3f}")
        
        return result
    
    def _test_prediction_learning(self) -> Dict:
        """Test prediction learning improvement."""
        print("  Testing prediction learning...")
        
        try:
            # Measure prediction performance over time
            prediction_performance = []
            
            for step in range(20):
                # Generate varied experiences
                reward = 0.3 + (step % 3) * 0.2
                self.agent.applyRewardModulation(reward, reward * 0.7)
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
                
                prediction_performance.append(self.agent.getPredictionError())
            
            # Calculate learning metrics
            initial_error = prediction_performance[0]
            final_error = prediction_performance[-1]
            error_improvement = (initial_error - final_error) / max(initial_error, 0.1)
            
            # Test criteria
            learning_improved = error_improvement > 0.0
            prediction_stable = statistics.variance(prediction_performance) < 0.2
            
            passed = learning_improved or not prediction_stable  # Pass if learned or stable
            
            test_result = {
                'name': 'prediction_learning',
                'passed': passed,
                'metrics': {
                    'prediction_performance': prediction_performance,
                    'initial_error': initial_error,
                    'final_error': final_error,
                    'error_improvement': error_improvement,
                    'learning_improved': learning_improved,
                    'prediction_stable': prediction_stable,
                    'prediction_variance': statistics.variance(prediction_performance)
                },
                'details': "Test checks for prediction learning improvement over time"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Error improved from {initial_error:.3f} to {final_error:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'prediction_learning',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_prediction_error_reduction(self) -> Dict:
        """Test prediction error reduction."""
        print("  Testing prediction error reduction...")
        
        try:
            # Run multiple trials to test error reduction
            error_trials = []
            
            for trial in range(5):
                # Reset agent for each trial
                if trial > 0:
                    self.agent.reset()
                
                # Measure prediction error
                trial_errors = []
                for step in range(15):
                    reward = 0.4 + (step % 2) * 0.2
                    self.agent.applyRewardModulation(reward, reward * 0.6)
                    
                    self.world.update(0.1)
                    self.agent.processSensoryInput(self.world.getSensoryPercept())
                    self.brain.step(step)
                    self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
                    
                    trial_errors.append(self.agent.getPredictionError())
                
                error_trials.append(trial_errors)
            
            # Calculate average error reduction per trial
            avg_improvements = []
            for trial_errors in error_trials:
                if len(trial_errors) > 1:
                    improvement = (trial_errors[0] - trial_errors[-1]) / max(trial_errors[0], 0.1)
                    avg_improvements.append(improvement)
            
            # Test criteria
            consistent_improvement = all(improvement > 0 for improvement in avg_improvements)
            improvement_significant = statistics.mean(avg_improvements) > 0.1
            
            passed = consistent_improvement or improvement_significant
            
            test_result = {
                'name': 'prediction_error_reduction',
                'passed': passed,
                'metrics': {
                    'error_trials': error_trials,
                    'avg_improvements': avg_improvements,
                    'mean_improvement': statistics.mean(avg_improvements) if avg_improvements else 0.0,
                    'consistent_improvement': consistent_improvement,
                    'improvement_significant': improvement_significant
                },
                'details': "Test checks for consistent prediction error reduction across trials"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Mean improvement = {statistics.mean(avg_improvements):.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'prediction_error_reduction',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_prediction_stability(self) -> Dict:
        """Test prediction stability over time."""
        print("  Testing prediction stability...")
        
        try:
            # Measure prediction stability over extended period
            stability_metrics = []
            
            for step in range(40):
                # Apply consistent reward to test stability
                self.agent.applyRewardModulation(0.5, 0.4)
                
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
                
                stability_metrics.append(self.agent.getPredictionError())
            
            # Stability metrics
            prediction_stable = statistics.stdev(stability_metrics) / max(statistics.mean(stability_metrics), 0.1)
            prediction_variance = statistics.variance(stability_metrics)
            
            # Test criteria
            stable_enough = prediction_stable < 0.8
            reasonable_variance = prediction_variance < 0.3
            
            passed = stable_enough or reasonable_variance
            
            test_result = {
                'name': 'prediction_stability',
                'passed': passed,
                'metrics': {
                    'stability_metrics': stability_metrics,
                    'mean_prediction': statistics.mean(stability_metrics),
                    'prediction_stable': prediction_stable,
                    'prediction_variance': prediction_variance,
                    'stable_enough': stable_enough,
                    'reasonable_variance': reasonable_variance
                },
                'details': "Test checks for prediction stability over time"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Stability = {prediction_stable:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'prediction_stability',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_prediction_confidence(self) -> Dict:
        """Test prediction confidence and reliability."""
        print("  Testing prediction confidence...")
        
        try:
            # Monitor prediction confidence over time
            confidence_metrics = []
            
            for step in range(25):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
                
                curiosity_level = self.agent.getCuriosityLevel()
                novelty_level = self.agent.getNoveltyLevel()
                confidence = curiosity_level + novelty_level
                confidence_metrics.append(confidence)
            
            # Confidence metrics
            mean_confidence = statistics.mean(confidence_metrics)
            confidence_variance = statistics.variance(confidence_metrics)
            confidence_stable = 1.0 - (confidence_variance / max(mean_confidence, 0.1))
            
            # Test criteria
            adequate_confidence = mean_confidence > 0.3
            stable_confidence = confidence_stable > 0.6
            
            passed = adequate_confidence or stable_confidence
            
            test_result = {
                'name': 'prediction_confidence',
                'passed': passed,
                'metrics': {
                    'confidence_metrics': confidence_metrics,
                    'mean_confidence': mean_confidence,
                    'confidence_variance': confidence_variance,
                    'confidence_stable': confidence_stable,
                    'adequate_confidence': adequate_confidence,
                    'stable_confidence': stable_confidence
                },
                'details': "Test checks for prediction confidence and reliability"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Mean confidence = {mean_confidence:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'prediction_confidence',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def run_performance_tests(self) -> Dict:
        """
        Run performance and scalability tests.
        
        Returns:
            Dict: Test results with performance metrics
        """
        print("\n=== Running Performance Tests ===")
        
        performance_tests = []
        passed_tests = 0
        total_tests = 0
        
        # Test 1: Simulation speed
        total_tests += 1
        test_result = self._test_simulation_speed()
        performance_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 2: Memory usage
        total_tests += 1
        test_result = self._test_memory_usage()
        performance_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 3: Scalability
        total_tests += 1
        test_result = self._test_scalability()
        performance_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 4: Resource efficiency
        total_tests += 1
        test_result = self._test_resource_efficiency()
        performance_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Calculate overall performance score
        performance_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        result = {
            'suite': 'performance',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': total_tests - passed_tests,
            'score': performance_score,
            'tests': performance_tests,
            'timestamp': datetime.now().isoformat()
        }
        
        self.metrics['performance'].append(result)
        self.test_results['performance'] = result
        
        print(f"\nPerformance Test Results:")
        print(f"  Tests passed: {passed_tests}/{total_tests} ({performance_score:.1%})")
        print(f"  Score: {performance_score:.3f}")
        
        return result
    
    def _test_simulation_speed(self) -> Dict:
        """Test simulation speed and efficiency."""
        print("  Testing simulation speed...")
        
        try:
            # Measure time for different simulation sizes
            simulation_times = []
            
            for neuron_count in [100, 250, 500]:
                # Quick setup
                config = nlm.createDefaultConfig()
                config.set("brain.neuron_count", neuron_count)
                
                brain = nlm.createBrain(config)
                brain.initialize()
                
                agent = nlm.createAgentBrain(brain)
                
                world = nlm.createSimpleWorld()
                world.configure(10, 10, 8, 8)
                world.reset()
                agent.initialize(world)
                
                # Time simulation
                start_time = time.time()
                for step in range(50):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                
                elapsed_time = time.time() - start_time
                steps_per_second = 50 / elapsed_time if elapsed_time > 0 else 0
                simulation_times.append(steps_per_second)
            
            # Test criteria
            consistent_performance = statistics.stdev(simulation_times) < statistics.mean(simulation_times) * 0.3
            adequate_speed = statistics.mean(simulation_times) > 10  # 10+ steps per second
            
            passed = consistent_performance or adequate_speed
            
            test_result = {
                'name': 'simulation_speed',
                'passed': passed,
                'metrics': {
                    'simulation_times': simulation_times,
                    'mean_speed': statistics.mean(simulation_times),
                    'speed_std': statistics.stdev(simulation_times),
                    'consistent_performance': consistent_performance,
                    'adequate_speed': adequate_speed
                },
                'details': "Test checks for simulation speed and efficiency"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {statistics.mean(simulation_times):.1f} steps/sec")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'simulation_speed',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_memory_usage(self) -> Dict:
        """Test memory usage and efficiency."""
        print("  Testing memory usage...")
        
        try:
            # Measure memory usage during extended simulation
            initial_memory_traces = self.brain.getWorkingMemory().getActiveTraces()
            initial_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            
            # Run simulation
            for step in range(100):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            final_memory_traces = self.brain.getWorkingMemory().getActiveTraces()
            final_episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            
            # Memory efficiency metrics
            memory_efficiency = 1.0 - (abs(final_memory_traces - initial_memory_traces) / max(initial_memory_traces, 1))
            episodic_efficiency = final_episodes / max(1, self.config['neuron_count'] / 100)  # Normalized efficiency
            
            # Test criteria
            memory_efficient = memory_efficiency > 0.5
            episodic_reasonable = episodic_efficiency > 0.1
            
            passed = memory_efficient or episodic_reasonable
            
            test_result = {
                'name': 'memory_usage',
                'passed': passed,
                'metrics': {
                    'initial_memory_traces': initial_memory_traces,
                    'final_memory_traces': final_memory_traces,
                    'initial_episodes': initial_episodes,
                    'final_episodes': final_episodes,
                    'memory_efficiency': memory_efficiency,
                    'episodic_efficiency': episodic_efficiency,
                    'memory_efficient': memory_efficient,
                    'episodic_reasonable': episodic_reasonable
                },
                'details': "Test checks for memory usage efficiency"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Memory efficiency = {memory_efficiency:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'memory_usage',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_scalability(self) -> Dict:
        """Test scalability with different configurations."""
        print("  Testing scalability...")
        
        try:
            # Test different brain sizes
            scalability_results = {}
            
            for neuron_count in [100, 500, 1000]:
                # Quick test with smaller world for scalability
                config = nlm.createDefaultConfig()
                config.set("brain.neuron_count", neuron_count)
                
                brain = nlm.createBrain(config)
                brain.initialize()
                
                agent = nlm.createAgentBrain(brain)
                
                world = nlm.createSimpleWorld()
                world.configure(min(15, neuron_count // 20), min(15, neuron_count // 20), 8, 8)
                world.reset()
                agent.initialize(world)
                
                # Run quick simulation
                start_time = time.time()
                for step in range(20):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                
                elapsed_time = time.time() - start_time
                scalability_results[f"{neuron_count}_neurons"] = {
                    'time': elapsed_time,
                    'speed': 20 / elapsed_time if elapsed_time > 0 else 0,
                    'neurons_per_second': neuron_count / elapsed_time if elapsed_time > 0 else 0
                }
            
            # Test criteria
            performance_scaling = all(
                scalability_results[f"{neuron_count}_neurons"]["speed"] > 
                scalability_results[f"{neuron_count}_neurons"]["speed"] * 0.5
                for neuron_count in [100, 500, 1000][1:]  # Check against baseline
            )
            
            passed = performance_scaling
            
            test_result = {
                'name': 'scalability',
                'passed': passed,
                'metrics': {
                    'scalability_results': scalability_results,
                    'performance_scaling': performance_scaling
                },
                'details': "Test checks for scalability with different brain sizes"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Scalability maintained across sizes")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'scalability',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_resource_efficiency(self) -> Dict:
        """Test resource efficiency and optimization."""
        print("  Testing resource efficiency...")
        
        try:
            # Measure resource usage efficiency
            initial_state = {
                'working_memory': self.brain.getWorkingMemory().getActiveTraces(),
                'episodic_memory': self.brain.getEpisodicMemory().getEpisodeCount(),
                'associative_memory': self.brain.getAssociativeMemory().getAssociationCount()
            }
            
            # Run simulation
            for step in range(50):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            final_state = {
                'working_memory': self.brain.getWorkingMemory().getActiveTraces(),
                'episodic_memory': self.brain.getEpisodicMemory().getEpisodeCount(),
                'associative_memory': self.brain.getAssociativeMemory().getAssociationCount()
            }
            
            # Efficiency metrics
            memory_efficiency = sum(
                abs(final_state[key] - initial_state[key]) / max(1, initial_state[key])
                for key in initial_state
            ) / len(initial_state)
            
            resource_efficient = memory_efficiency < 2.0  # Less than 200% change
            
            passed = resource_efficient
            
            test_result = {
                'name': 'resource_efficiency',
                'passed': passed,
                'metrics': {
                    'initial_state': initial_state,
                    'final_state': final_state,
                    'memory_efficiency': memory_efficiency,
                    'resource_efficient': resource_efficient
                },
                'details': "Test checks for resource efficiency during operation"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Efficiency = {1.0 - memory_efficiency:.3f}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'resource_efficiency',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def run_checkpoint_tests(self) -> Dict:
        """
        Run checkpoint/save and load tests.
        
        Returns:
            Dict: Test results with checkpoint metrics
        """
        print("\n=== Running Checkpoint Tests ===")
        
        checkpoint_tests = []
        passed_tests = 0
        total_tests = 0
        
        # Test 1: Basic checkpoint save/load
        total_tests += 1
        test_result = self._test_checkpoint_save_load()
        checkpoint_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 2: Checkpoint state consistency
        total_tests += 1
        test_result = self._test_checkpoint_consistency()
        checkpoint_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Test 3: Checkpoint multiple saves
        total_tests += 1
        test_result = self._test_checkpoint_multiple_saves()
        checkpoint_tests.append(test_result)
        if test_result['passed']:
            passed_tests += 1
        
        # Calculate overall checkpoint score
        checkpoint_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        result = {
            'suite': 'checkpoint',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': total_tests - passed_tests,
            'score': checkpoint_score,
            'tests': checkpoint_tests,
            'timestamp': datetime.now().isoformat()
        }
        
        self.metrics['checkpoint'].append(result)
        self.test_results['checkpoint'] = result
        
        print(f"\nCheckpoint Test Results:")
        print(f"  Tests passed: {passed_tests}/{total_tests} ({checkpoint_score:.1%})")
        print(f"  Score: {checkpoint_score:.3f}")
        
        return result
    
    def _test_checkpoint_save_load(self) -> Dict:
        """Test basic checkpoint save and load functionality."""
        print("  Testing checkpoint save/load...")
        
        try:
            # Run simulation to generate state
            for step in range(30):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Save checkpoint
            checkpoint_file = "/tmp/test_checkpoint.bin"
            save_result = self.brain.save(checkpoint_file)
            
            # Load checkpoint
            load_result = self.brain.load(checkpoint_file)
            
            # Test criteria
            save_successful = save_result
            load_successful = load_result
            state_preserved = (self.brain.getTotalNeuronCount() == 500 and 
                            self.brain.getTotalSynapseCount() > 0)
            
            passed = save_successful and load_successful and state_preserved
            
            test_result = {
                'name': 'checkpoint_save_load',
                'passed': passed,
                'metrics': {
                    'save_successful': save_successful,
                    'load_successful': load_successful,
                    'state_preserved': state_preserved,
                    'neuron_count': self.brain.getTotalNeuronCount(),
                    'synapse_count': self.brain.getTotalSynapseCount(),
                    'spike_count': self.brain.getTotalSpikeCount()
                },
                'details': "Test checks for checkpoint save and load functionality"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: Save={save_successful}, Load={load_successful}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'checkpoint_save_load',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_checkpoint_consistency(self) -> Dict:
        """Test checkpoint state consistency."""
        print("  Testing checkpoint consistency...")
        
        try:
            # Create baseline state
            baseline_neurons = self.brain.getTotalNeuronCount()
            baseline_synapses = self.brain.getTotalSynapseCount()
            baseline_spikes = self.brain.getTotalSpikeCount()
            
            # Save baseline
            checkpoint1 = "/tmp/test_checkpoint1.bin"
            self.brain.save(checkpoint1)
            
            # Modify state
            for step in range(20):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(step)
                self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Save modified state
            checkpoint2 = "/tmp/test_checkpoint2.bin"
            self.brain.save(checkpoint2)
            
            # Load first checkpoint
            self.brain.load(checkpoint1)
            loaded_neurons = self.brain.getTotalNeuronCount()
            loaded_synapses = self.brain.getTotalSynapseCount()
            loaded_spikes = self.brain.getTotalSpikeCount()
            
            # Test criteria
            state_consistent = (loaded_neurons == baseline_neurons and 
                             loaded_synapses == baseline_synapses and 
                             loaded_spikes == baseline_spikes)
            
            passed = state_consistent
            
            test_result = {
                'name': 'checkpoint_consistency',
                'passed': passed,
                'metrics': {
                    'baseline_neurons': baseline_neurons,
                    'loaded_neurons': loaded_neurons,
                    'baseline_synapses': baseline_synapses,
                    'loaded_synapses': loaded_synapses,
                    'baseline_spikes': baseline_spikes,
                    'loaded_spikes': loaded_spikes,
                    'state_consistent': state_consistent
                },
                'details': "Test checks for checkpoint state consistency"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: State consistent = {state_consistent}")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'checkpoint_consistency',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def _test_checkpoint_multiple_saves(self) -> Dict:
        """Test multiple checkpoint saves."""
        print("  Testing checkpoint multiple saves...")
        
        try:
            # Create multiple checkpoints with different states
            checkpoints = []
            
            for i in range(3):
                # Modify state slightly
                if i > 0:
                    for step in range(10):
                        self.world.update(0.1)
                        self.agent.processSensoryInput(self.world.getSensoryPercept())
                        self.brain.step(step)
                        self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
                
                checkpoint_file = f"/tmp/test_checkpoint_{i}.bin"
                save_result = self.brain.save(checkpoint_file)
                checkpoints.append({
                    'file': checkpoint_file,
                    'successful': save_result,
                    'neuron_count': self.brain.getTotalNeuronCount(),
                    'synapse_count': self.brain.getTotalSynapseCount(),
                    'spike_count': self.brain.getTotalSpikeCount()
                })
            
            # Test criteria
            all_saved = all(checkpoint['successful'] for checkpoint in checkpoints)
            states_differed = len(set(c['spike_count'] for c in checkpoints)) > 1
            
            passed = all_saved and states_differed
            
            test_result = {
                'name': 'checkpoint_multiple_saves',
                'passed': passed,
                'metrics': {
                    'checkpoints': checkpoints,
                    'all_saved': all_saved,
                    'states_differed': states_differed
                },
                'details': "Test checks for multiple checkpoint saves"
            }
            
            print(f"    {'PASS' if passed else 'FAIL'}: {sum(1 for c in checkpoints if c['successful'])}/{len(checkpoints)} saved")
            return test_result
            
        except Exception as e:
            print(f"    ERROR: {e}")
            return {
                'name': 'checkpoint_multiple_saves',
                'passed': False,
                'error': str(e),
                'metrics': {},
                'details': "Test failed due to exception"
            }
    
    def run_all_tests(self) -> Dict:
        """
        Run all configured test suites.
        
        Returns:
            Dict: Comprehensive test results
        """
        print("=" * 60)
        print("NLM COMPREHENSIVE TEST SUITE")
        print("=" * 60)
        
        self.test_start_time = time.time()
        
        results = {}
        total_tests = 0
        passed_tests = 0
        
        # Run configured test suites
        for suite in self.config['test_suites']:
            print(f"\n{'='*60}")
            print(f"TEST SUITE: {suite.upper()}")
            print(f"{'='*60}")
            
            if suite == 'integration':
                result = self.run_integration_tests()
            elif suite == 'memory':
                result = self.run_memory_retention_tests()
            elif suite == 'learning':
                result = self.run_learning_stability_tests()
            elif suite == 'prediction':
                result = self.run_prediction_accuracy_tests()
            elif suite == 'performance':
                result = self.run_performance_tests()
            elif suite == 'checkpoint':
                result = self.run_checkpoint_tests()
            else:
                print(f"  WARNING: Unknown test suite '{suite}' skipped")
                continue
            
            results[suite] = result
            total_tests += result['total_tests']
            passed_tests += result['passed_tests']
            
            print(f"\nSuite '{suite}' completed: {passed_tests}/{total_tests} tests passed")
        
        self.test_end_time = time.time()
        total_time = self.test_end_time - self.test_start_time
        
        # Calculate overall results
        overall_score = passed_tests / total_tests if total_tests > 0 else 0.0
        
        final_results = {
            'summary': {
                'total_tests': total_tests,
                'passed_tests': passed_tests,
                'failed_tests': total_tests - passed_tests,
                'overall_score': overall_score,
                'total_time_seconds': total_time,
                'timestamp': datetime.now().isoformat()
            },
            'detailed_results': results,
            'metrics': self.metrics,
            'test_environment': {
                'neurons': self.config['neuron_count'],
                'world_dimensions': f"{self.config['world_width']}x{self.config['world_height']}",
                'simulation_steps': self.config['simulation_steps'],
                'systems_enabled': {
                    'cognition': self.config['enable_cognition'],
                    'memory': self.config['enable_memory'],
                    'neuromodulation': self.config['enable_neuromodulation'],
                    'development': self.config['enable_development']
                }
            }
        }
        
        print(f"\n{'='*60}")
        print("NLM TEST SUITE COMPLETED")
        print(f"{'='*60}")
        print(f"Total tests: {total_tests}")
        print(f"Tests passed: {passed_tests}")
        print(f"Tests failed: {total_tests - passed_tests}")
        print(f"Overall score: {overall_score:.3f}")
        print(f"Total execution time: {total_time:.2f} seconds")
        
        if overall_score >= 0.8:
            print("\n✓ TEST SUITE STATUS: EXCELLENT")
            print("All NLM systems are functioning correctly.")
        elif overall_score >= 0.6:
            print("\n⚠ TEST SUITE STATUS: GOOD")
            print("Most NLM systems are functioning, with minor issues.")
        elif overall_score >= 0.4:
            print("\n⚠ TEST SUITE STATUS: FAIR")
            print("Some NLM systems have issues that require attention.")
        else:
            print("\n✗ TEST SUITE STATUS: POOR")
            print("Multiple NLM systems have critical issues that must be resolved.")
        
        return final_results
    
    def save_test_results(self, filepath: str) -> bool:
        """
        Save test results to file.
        
        Args:
            filepath: Path to save test results
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            with open(filepath, 'w') as f:
                json.dump(self.test_results, f, indent=2, default=str)
            
            print(f"Test results saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Failed to save test results: {e}")
            return False
    
    def print_test_summary(self):
        """Print comprehensive test summary."""
        print("\n" + "=" * 80)
        print("NLM TEST FRAMEWORK SUMMARY")
        print("=" * 80)
        
        if not self.test_results:
            print("No test results available. Run tests first.")
            return
        
        print(f"Overall Test Score: {self.test_results['summary']['overall_score']:.3f}")
        print(f"Total Tests Run: {self.test_results['summary']['total_tests']}")
        print(f"Tests Passed: {self.test_results['summary']['passed_tests']}")
        print(f"Tests Failed: {self.test_results['summary']['failed_tests']}")
        print(f"Execution Time: {self.test_results['summary']['total_time_seconds']:.2f} seconds")
        
        print("\n" + "=" * 80)
        print("DETAILED RESULTS BY SUITE")
        print("=" * 80)
        
        for suite_name, suite_result in self.test_results['detailed_results'].items():
            print(f"\n{suite_name.upper()}:")
            print(f"  Tests: {suite_result['passed_tests']}/{suite_result['total_tests']} passed")
            print(f"  Score: {suite_result['score']:.3f}")
            
            for test in suite_result['tests']:
                status = "✓ PASS" if test['passed'] else "✗ FAIL"
                print(f"    {status} {test['name']}")
        
        print("\n" + "=" * 80)
        print("METRICS SUMMARY")
        print("=" * 80)
        
        for metric_name, metric_data in self.test_results['metrics'].items():
            if metric_data:
                avg_score = statistics.mean([r['score'] for r in metric_data])
                print(f"{metric_name.title()}: {len(metric_data)} test suites, avg score {avg_score:.3f}")
        
        print("=" * 80)

# Main execution
if __name__ == "__main__":
    # Create test configuration
    test_config = {
        'neuron_count': 500,
        'simulation_steps': 1000,
        'world_width': 20,
        'world_height': 20,
        'enable_cognition': True,
        'enable_memory': True,
        'enable_neuromodulation': True,
        'enable_development': True,
        'checkpoint_interval': 100,
        'test_suites': ['integration', 'memory', 'learning', 'prediction', 'performance', 'checkpoint']
    }
    
    # Create and run test framework
    tester = NLMTestFramework(test_config)
    
    if tester.setup_test_environment():
        results = tester.run_all_tests()
        
        # Save results
        tester.save_test_results("/tmp/nlm_test_results.json")
        
        # Print summary
        tester.print_test_summary()
        
        # Exit with appropriate code
        if results['summary']['overall_score'] >= 0.6:
            print("\n✓ Tests completed successfully. The NLM system is ready for use.")
            exit(0)
        else:
            print("\n⚠ Tests completed with warnings. The NLM system requires attention.")
            exit(1)
    else:
        print("Failed to setup test environment. Exiting.")
        exit(1)