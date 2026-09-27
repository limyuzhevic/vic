import unittest
import pytest
import time
import sys
import os
from pathlib import Path
from typing import Any, Dict, List, Optional
import pynlm

class NLMServer:
    """Test server fixture for NLM Python bindings testing."""
    
    def __init__(self, config_path: Optional[str] = None):
        self.config_path = config_path
        self.brain = None
        self.world = None
        self.agent = None
        self.test_results = {}
    
    def setup(self):
        """Set up test environment."""
        try:
            # Create default configuration
            self.config = pynlm.createDefaultConfig()
            
            # Configure brain
            self.config.set("brain.neuron_count", 100)
            self.config.set("brain.synapse_density", 0.1)
            
            # Create brain
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            # Create world
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
            self.world.setAgentStart(10.0, 10.0)
            
            # Create agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable learning
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            
            return True
        except Exception as e:
            print(f"Setup failed: {e}")
            return False
    
    def teardown(self):
        """Clean up test environment."""
        try:
            # Clean up agent
            self.agent = None
            
            # Clean up world
            self.world = None
            
            # Clean up brain
            self.brain = None
            
            return True
        except Exception as e:
            print(f"Teardown failed: {e}")
            return False
    
    def run_simulation(self, num_steps: int) -> Dict[str, Any]:
        """Run simulation and collect results."""
        try:
            results = {
                'steps': num_steps,
                'success': True,
                'errors': [],
                'metrics': {},
                'timing': {},
                'neurons_firing': 0,
                'total_spikes': 0,
                'average_firing_rate': 0.0,
                'curiosity_levels': [],
                'novelty_levels': [],
                'developmental_stages': []
            }
            
            start_time = time.time()
            
            for step in range(num_steps):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode motor command
                action = self.agent.decodeMotorCommand()
                
                # Apply motor command
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Collect metrics
                results['neurons_firing'] = self.brain.getFiringNeuronCount()
                results['total_spikes'] = self.brain.getTotalSpikeCount()
                results['average_firing_rate'] = self.brain.getAverageFiringRate()
                results['curiosity_levels'].append(self.agent.getCuriosityLevel())
                results['novelty_levels'].append(self.agent.getNoveltyLevel())
                results['developmental_stages'].append(
                    self.brain.getDevelopmentalStage().name)
                
                # Record timing
                step_time = time.time() - start_time
                results['timing'][step] = step_time
                start_time = time.time()
            
            # Calculate averages
            results['average_neurons_firing'] = (
                sum(results['neurons_firing']) / len(results['neurons_firing'])
                if results['neurons_firing'] else 0)
            
            results['average_total_spikes'] = (
                sum(results['total_spikes']) / len(results['total_spikes'])
                if results['total_spikes'] else 0)
            
            results['average_average_firing_rate'] = (
                sum(results['average_firing_rate']) / len(results['average_firing_rate'])
                if results['average_firing_rate'] else 0)
            
            results['average_curiosity'] = (
                sum(results['curiosity_levels']) / len(results['curiosity_levels'])
                if results['curiosity_levels'] else 0)
            
            results['average_novelty'] = (
                sum(results['novelty_levels']) / len(results['novelty_levels'])
                if results['novelty_levels'] else 0)
            
            return results
            
        except Exception as e:
            results['success'] = False
            results['errors'].append(str(e))
            return results
    
    def test_basic_functionality(self) -> Dict[str, Any]:
        """Test basic API functionality."""
        test_result = {
            'test_name': 'Basic Functionality',
            'status': 'passed',
            'details': {},
            'execution_time': 0
        }
        
        start_time = time.time()
        
        try:
            # Test configuration creation
            config = pynlm.createDefaultConfig()
            test_result['details']['config_created'] = config is not None
            
            # Test brain creation
            brain = pynlm.createBrain(config)
            test_result['details']['brain_created'] = brain is not None
            
            # Test brain initialization
            brain.initialize()
            test_result['details']['brain_initialized'] = True
            
            # Test brain statistics
            neurons = brain.getTotalNeuronCount()
            synapses = brain.getTotalSynapseCount()
            test_result['details']['brain_stats'] = {
                'neurons': neurons,
                'synapses': synapses,
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'rate': brain.getAverageFiringRate()
            }
            
            # Test brain step
            brain.step(0)
            test_result['details']['brain_step_executed'] = True
            
            # Test world creation
            world = pynlm.createSimpleWorld()
            test_result['details']['world_created'] = world is not None
            
            # Test world configuration
            world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
            test_result['details']['world_configured'] = True
            
            # Test world update
            world.update(0.1)
            test_result['details']['world_updated'] = True
            
            # Test agent creation
            agent = pynlm.createAgentBrain(brain)
            test_result['details']['agent_created'] = agent is not None
            
            # Test agent initialization
            agent.initialize(world)
            test_result['details']['agent_initialized'] = True
            
            # Test agent methods
            test_result['details']['agent_methods'] = {
                'reward_modulation': agent.enableRewardModulation,
                'curiosity': agent.enableCuriosity,
                'structural_plasticity': agent.enableStructuralPlasticity,
                'development': agent.enableDevelopment
            }
            
        except Exception as e:
            test_result['status'] = 'failed'
            test_result['details']['error'] = str(e)
        
        test_result['execution_time'] = time.time() - start_time
        return test_result
    
    def test_configuration_functionality(self) -> Dict[str, Any]:
        """Test configuration functionality."""
        test_result = {
            'test_name': 'Configuration Functionality',
            'status': 'passed',
            'details': {},
            'execution_time': 0
        }
        
        start_time = time.time()
        
        try:
            # Test default configuration
            config = pynlm.createDefaultConfig()
            test_result['details']['default_config'] = config is not None
            
            # Test configuration setting
            config.set("test.key1", "value1")
            config.set("test.key2", 42)
            config.set("test.key3", 3.14)
            config.set("test.key4", True)
            
            # Test configuration getting
            test_result['details']['config_values'] = {
                'string': config.get("test.key1"),
                'int': config.get("test.key2"),
                'float': config.get("test.key3"),
                'bool': config.get("test.key4")
            }
            
            # Test configuration existence
            test_result['details']['config_existence'] = {
                'key1': config.has("test.key1"),
                'key2': config.has("test.key2"),
                'nonexistent': config.has("nonexistent.key")
            }
            
            # Test configuration keys
            test_result['details']['config_keys'] = config.getKeys()
            
            # Test configuration removal
            config.remove("test.key1")
            test_result['details']['config_removal'] = not config.has("test.key1")
            
        except Exception as e:
            test_result['status'] = 'failed'
            test_result['details']['error'] = str(e)
        
        test_result['execution_time'] = time.time() - start_time
        return test_result
    
    def test_simulation_integration(self) -> Dict[str, Any]:
        """Test complete simulation integration."""
        test_result = {
            'test_name': 'Simulation Integration',
            'status': 'passed',
            'details': {},
            'execution_time': 0
        }
        
        start_time = time.time()
        
        try:
            # Setup complete simulation environment
            if not self.setup():
                test_result['status'] = 'failed'
                test_result['details']['error'] = "Setup failed"
                return test_result
            
            # Run simulation
            simulation_results = self.run_simulation(50)
            
            # Validate simulation results
            test_result['details']['simulation_results'] = {
                'success': simulation_results['success'],
                'steps': simulation_results['steps'],
                'errors': simulation_results['errors'],
                'final_neurons_firing': simulation_results['neurons_firing'],
                'final_total_spikes': simulation_results['total_spikes'],
                'final_average_firing_rate': simulation_results['average_firing_rate'],
                'final_curiosity': simulation_results['average_curiosity'],
                'final_novelty': simulation_results['average_novelty'],
                'final_development_stage': simulation_results['developmental_stages'][-1]
            }
            
            # Check if simulation completed successfully
            if simulation_results['success'] and not simulation_results['errors']:
                test_result['details']['simulation_quality'] = {
                    'stability': 'stable' if simulation_results['average_firing_rate'] > 0.1 else 'unstable',
                    'learning': 'active' if simulation_results['average_curiosity'] > 0.3 else 'inactive',
                    'development': 'progressing' if simulation_results['developmental_stages'][-1] != 'Initial' else 'not started'
                }
            
        except Exception as e:
            test_result['status'] = 'failed'
            test_result['details']['error'] = str(e)
        
        finally:
            self.teardown()
        
        test_result['execution_time'] = time.time() - start_time
        return test_result
    
    def run_all_tests(self) -> Dict[str, Any]:
        """Run all tests and collect results."""
        print("Running NLM Python Bindings Tests")
        print("=" * 50)
        
        test_results = {
            'total_tests': 0,
            'passed_tests': 0,
            'failed_tests': 0,
            'skipped_tests': 0,
            'total_execution_time': 0,
            'test_details': [],
            'performance_summary': {},
            'error_summary': []
        }
        
        # Define test suite
        test_suite = [
            ('Basic Functionality', self.test_basic_functionality),
            ('Configuration Functionality', self.test_configuration_functionality),
            ('Simulation Integration', self.test_simulation_integration),
        ]
        
        for test_name, test_func in test_suite:
            test_result = test_func()
            test_result['test_name'] = test_name
            test_results['test_details'].append(test_result)
            test_results['total_tests'] += 1
            
            if test_result['status'] == 'passed':
                test_results['passed_tests'] += 1
            else:
                test_results['failed_tests'] += 1
                test_results['error_summary'].append({
                    'test': test_name,
                    'error': test_result['details'].get('error', 'Unknown error'),
                    'execution_time': test_result['execution_time']
                })
            
            test_results['total_execution_time'] += test_result['execution_time']
        
        # Calculate performance summary
        if test_results['total_tests'] > 0:
            test_results['performance_summary'] = {
                'average_execution_time': (
                    test_results['total_execution_time'] / test_results['total_tests']
                ),
                'pass_rate': (test_results['passed_tests'] / test_results['total_tests']) * 100,
                'fail_rate': (test_results['failed_tests'] / test_results['total_tests']) * 100
            }
        
        return test_results

if __name__ == '__main__':
    # Run tests
    server = NLMServer()
    results = server.run_all_tests()
    
    # Print results
    print("\n" + "=" * 60)
    print("TEST RESULTS SUMMARY")
    print("=" * 60)
    print(f"Total Tests: {results['total_tests']}")
    print(f"Passed: {results['passed_tests']}")
    print(f"Failed: {results['failed_tests']}")
    print(f"Skipped: {results['skipped_tests']}")
    print(f"Total Execution Time: {results['total_execution_time']:.2f} seconds")
    
    if results['passed_tests'] == results['total_tests']:
        print("\n✅ ALL TESTS PASSED!")
    else:
        print("\n❌ SOME TESTS FAILED")
        print("\nFailed Tests:")
        for error in results['error_summary']:
            print(f"  - {error['test']}: {error['error']}")
    
    print(f"\nPerformance Summary:")
    for key, value in results['performance_summary'].items():
        print(f"  {key}: {value:.2f}")
    
    # Exit with appropriate code
    exit_code = 0 if results['failed_tests'] == 0 else 1
    sys.exit(exit_code)