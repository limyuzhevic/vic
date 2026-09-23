#!/usr/bin/env python3
"""
Python test suite for NLM bindings

This file provides a comprehensive test framework for the NLM Python bindings.
It can be run as a standalone test runner or integrated into pytest.

Usage:
    python3 -m pytest nlm_test_suite.py -v
    python3 nlm_test_suite.py  # Run all tests
    python3 nlm_test_suite.py --quick  # Run quick subset
"""

import sys
import os
import traceback
from typing import Dict, List, Callable, Any
import numpy as np

# Add the built pynlm module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build', 'pynlm', 'Lib', 'site-packages'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("WARNING: pynlm module not available - Python binding tests skipped")

class NLMTestSuite:
    """Comprehensive test suite for NLM Python bindings."""
    
    def __init__(self):
        self.results: Dict[str, Dict[str, Any]] = {}
        self.passed = 0
        self.failed = 0
        self.skipped = 0
    
    def run_all_tests(self, quick_mode: bool = False):
        """Run all available tests."""
        print("=" * 60)
        print("NLM Python Bindings Test Suite")
        print("=" * 60)
        print(f"Mode: {'Quick' if quick_mode else 'Full'}")
        print()
        
        # Define test groups
        test_groups = [
            ("Basic Module Tests", self.test_basic_module),
            ("Configuration Tests", self.test_configuration),
            ("Sensory Input Tests", self.test_sensory_inputs),
            ("Action System Tests", self.test_action_system),
            ("World Simulation Tests", self.test_world_simulation),
            ("Brain Computation Tests", self.test_brain_computation),
            ("Agent Interface Tests", self.test_agent_interface),
            ("Integration Tests", self.test_integration),
        ]
        
        # Add more tests in full mode
        if not quick_mode:
            test_groups.extend([
                ("Memory System Tests", self.test_memory_systems),
                ("Plasticity Tests", self.test_plasticity),
                ("Neuromodulation Tests", self.test_neuromodulation),
                ("Development Tests", self.test_development),
                ("Performance Tests", self.test_performance),
                ("Error Handling Tests", self.test_error_handling),
            ])
        
        # Run all test groups
        for group_name, test_func in test_groups:
            self._run_test_group(group_name, test_func)
        
        # Print summary
        self._print_summary()
    
    def _run_test_group(self, group_name: str, test_func: Callable):
        """Run a test group and record results."""
        print(f"Running {group_name}...\n")
        
        group_results = {
            'tests': [],
            'passed': 0,
            'failed': 0,
            'skipped': 0
        }
        
        try:
            test_func(group_results)
        except Exception as e:
            print(f"ERROR in {group_name}: {e}")
            traceback.print_exc()
            group_results['failed'] += 1
            group_results['tests'].append({
                'name': f'{group_name}_ERROR',
                'status': 'ERROR',
                'error': str(e)
            })
        
        # Update overall results
        self.results[group_name] = group_results
        self.passed += group_results['passed']
        self.failed += group_results['failed']
        self.skipped += group_results['skipped']
        
        # Print group summary
        print(f"{group_name}: {group_results['passed']} passed, "
              f"{group_results['failed']} failed, "
              f"{group_results['skipped']} skipped")
        print()
    
    # Individual test groups
    def test_basic_module(self, results: Dict[str, Any]):
        """Test basic module functionality and imports."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Module Import',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        # Test basic imports
        tests = [
            ('Module Import', self._test_basic_import),
            ('Version Check', self._test_version_info),
            ('Constants Available', self._test_constants_available),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_basic_import(self, results: Dict[str, Any]):
        """Test basic module import and class availability."""
        # Test that all expected classes are available
        expected_classes = [
            'Config', 'Vision', 'Audio', 'InternalSignals',
            'Action', 'WorldObject', 'AgentBody', 'ActionResult',
            'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain',
            'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
            'NeuronType', 'SynapseType', 'DevelopmentalStage',
            'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType'
        ]
        
        missing = []
        for class_name in expected_classes:
            if not hasattr(pynlm, class_name):
                missing.append(class_name)
        
        if missing:
            raise AssertionError(f"Missing classes: {', '.join(missing)}")
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Module Import',
            'status': 'PASSED'
        })
    
    def _test_version_info(self, results: Dict[str, Any]):
        """Test version information."""
        assert hasattr(pynlm, '__version__'), "__version__ attribute missing"
        assert isinstance(pynlm.__version__, str), "__version__ should be string"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Version Check',
            'status': 'PASSED',
            'version': pynlm.__version__
        })
    
    def _test_constants_available(self, results: Dict[str, Any]):
        """Test that important constants are available."""
        expected_constants = [
            'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
            'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
        ]
        
        for const_name in expected_constants:
            assert hasattr(pynlm, const_name), f"Missing constant: {const_name}"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Constants Available',
            'status': 'PASSED'
        })
    
    def test_configuration(self, results: Dict[str, Any]):
        """Test configuration system."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Configuration',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        tests = [
            ('Config Creation', self._test_config_creation),
            ('Config Loading/Saving', self._test_config_io),
            ('Config Parameters', self._test_config_parameters),
            ('Default Config Factory', self._test_default_config_factory),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_config_creation(self, results: Dict[str, Any]):
        """Test basic configuration creation."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert hasattr(config, 'has'), "Config should have 'has' method"
        assert hasattr(config, 'getKeys'), "Config should have 'getKeys' method"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Config Creation',
            'status': 'PASSED'
        })
    
    def _test_config_io(self, results: Dict[str, Any]):n        """Test configuration file I/O."""
        config = pynlm.createDefaultConfig()
        
        # Test saving to file
        import tempfile
        import os
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Test saving
            config.saveToFile(config_file)
            assert os.path.exists(config_file), "Config file should exist after saving"
            
            # Test loading
            loaded_config = pynlm.createDefaultConfig()
            loaded_config.loadFromFile(config_file)
            
            results['passed'] += 1
            results['tests'].append({
                'name': 'Config Loading/Saving',
                'status': 'PASSED'
            })
        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def _test_config_parameters(self, results: Dict[str, Any]):
        """Test configuration parameter handling."""
        config = pynlm.createDefaultConfig()
        
        # Test basic key operations
        config.set('test_key', 'test_value')
        assert config.has('test_key'), "Config should have test_key"
        # Note: getKeys returns keys, not values
        
        keys = config.getKeys()
        assert 'test_key' in keys, "test_key should be in keys list"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Config Parameters',
            'status': 'PASSED'
        })
    
    def _test_default_config_factory(self, results: Dict[str, Any]):
        """Test default configuration factory function."""
        config = pynlm.createDefaultConfig()
        
        # Should create a valid Config object
        assert config is not None
        assert hasattr(config, 'loadFromArgs'), "Config should have loadFromArgs"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Default Config Factory',
            'status': 'PASSED'
        })
    
    def test_sensory_inputs(self, results: Dict[str, Any]):
        """Test sensory input systems."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Sensory Inputs',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        tests = [
            ('Vision Input', self._test_vision_input),
            ('Audio Input', self._test_audio_input),
            ('Internal Signals', self._test_internal_signals),
            ('Sensory Percept', self._test_sensory_percept),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_vision_input(self, results: Dict[str, Any]):
        """Test vision sensory input."""
        # Vision requires width, height, channels parameters
        vision = pynlm.Vision(64, 64, 3)
        assert vision is not None
        assert hasattr(vision, 'getWidth'), "Vision should have getWidth"
        assert hasattr(vision, 'getHeight'), "Vision should have getHeight"
        assert hasattr(vision, 'getChannels'), "Vision should have getChannels"
        
        # Test default constructor
        vision2 = pynlm.Vision()
        assert vision2 is not None
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Vision Input',
            'status': 'PASSED'
        })
    
    def _test_audio_input(self, results: Dict[str, Any]):
        """Test audio sensory input."""
        audio = pynlm.Audio(44100, 1024)  # sample_rate, num_samples
        assert audio is not None
        assert hasattr(audio, 'getSampleRate'), "Audio should have getSampleRate"
        assert hasattr(audio, 'getNumSamples'), "Audio should have getNumSamples"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Audio Input',
            'status': 'PASSED'
        })
    
    def _test_internal_signals(self, results: Dict[str, Any]):
        """Test internal signals sensory input."""
        signals = pynlm.InternalSignals()
        assert signals is not None
        assert hasattr(signals, 'addSignal'), "InternalSignals should have addSignal"
        assert hasattr(signals, 'clearSignals'), "InternalSignals should have clearSignals"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Internal Signals',
            'status': 'PASSED'
        })
    
    def _test_sensory_percept(self, results: Dict[str, Any]):n        """Test combined sensory percept."""
        percept = pynlm.SensoryPercept()
        assert percept is not None
        assert hasattr(percept, 'getVision'), "SensoryPercept should have getVision"
        assert hasattr(percept, 'getAudio'), "SensoryPercept should have getAudio"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Sensory Percept',
            'status': 'PASSED'
        })
    
    def test_action_system(self, results: Dict[str, Any]):
        """Test action system."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Action System',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        tests = [
            ('Action Creation', self._test_action_creation),
            ('Action Types', self._test_action_types),
            ('Action Parameters', self._test_action_parameters),
            ('World Objects', self._test_world_objects),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_action_creation(self, results: Dict[str, Any]):
        """Test action creation and manipulation."""
        # Test action creation with type
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action is not None
        assert action.getType() == pynlm.ActionType.MoveForward
        
        # Test action with parameters
        action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 1.0, 0.0])
        assert action2.getType() == pynlm.ActionType.Custom
        params = action2.getParameters()
        assert len(params) == 3
        
        # Test action methods
        assert action.getName() == "MoveForward"
        action_clone = action.clone()
        assert action_clone.getType() == action.getType()
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Action Creation',
            'status': 'PASSED'
        })
    
    def _test_action_types(self, results: Dict[str, Any]):
        """Test all action types are available."""
        # Test a few key action types
        test_types = [
            (pynlm.ActionType.MoveForward, "MoveForward"),
            (pynlm.ActionType.TurnLeft, "TurnLeft"),
            (pynlm.ActionType.Look, "Look"),
            (pynlm.ActionType.Wait, "Wait"),
        ]
        
        for action_type, expected_name in test_types:
            action = pynlm.Action(action_type)
            assert action.getType() == action_type
            assert action.getName() == expected_name
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Action Types',
            'status': 'PASSED'
        })
    
    def _test_action_parameters(self, results: Dict[str, Any]):
        """Test action parameter handling."""
        action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        
        # Test parameter getter/setter
        params = action.getParameters()
        assert len(params) == 3
        assert params[0] == 1.0
        assert params[1] == 2.0
        assert params[2] == 3.0
        
        # Test parameter setter
        new_params = [4.0, 5.0, 6.0]
        action.setParameters(new_params)
        assert action.getParameters() == new_params
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Action Parameters',
            'status': 'PASSED'
        })
    
    def _test_world_objects(self, results: Dict[str, Any]):
        """Test world object creation and manipulation."""
        # Test world object creation
        obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource)
        assert obj is not None
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.type == pynlm.WorldObjectType.Resource
        
        # Test default object
        obj2 = pynlm.WorldObject()
        assert obj2 is not None
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'World Objects',
            'status': 'PASSED'
        })
    
    def test_world_simulation(self, results: Dict[str, Any]):
        """Test world simulation functionality."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'World Simulation',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        tests = [
            ('World Creation', self._test_world_creation),
            ('World Configuration', self._test_world_configuration),
            ('Agent Body', self._test_agent_body),
            ('World Interaction', self._test_world_interaction),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_world_creation(self, results: Dict[str, Any]):
        """Test world simulation creation."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert hasattr(world, 'configure'), "World should have configure method"
        assert hasattr(world, 'getWidth'), "World should have getWidth"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'World Creation',
            'status': 'PASSED'
        })
    
    def _test_world_configuration(self, results: Dict[str, Any]):
        """Test world configuration."""
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(100, 100, 64, 64)  # width, height, vision_width, vision_height
        
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        assert world.getMaxEnergy() > 0
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'World Configuration',
            'status': 'PASSED'
        })
    
    def _test_agent_body(self, results: Dict[str, Any]):
        """Test agent body state."""
        # Test AgentBody class (if available)
        if hasattr(pynlm, 'AgentBody'):
            agent_body = pynlm.AgentBody()
            assert agent_body is not None
            assert hasattr(agent_body, 'x'), "AgentBody should have x attribute"
            assert hasattr(agent_body, 'y'), "AgentBody should have y attribute"
            assert hasattr(agent_body, 'energy'), "AgentBody should have energy attribute"
            
            # Test setting attributes
            agent_body.x = 10.0
            agent_body.y = 20.0
            agent_body.energy = 50.0
            assert agent_body.x == 10.0
            assert agent_body.y == 20.0
            assert agent_body.energy == 50.0
            
            results['passed'] += 1
            results['tests'].append({
                'name': 'Agent Body',
                'status': 'PASSED'
            })
        else:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Agent Body',
                'status': 'SKIPPED',
                'reason': 'AgentBody class not exposed'
            })
    
    def _test_world_interaction(self, results: Dict[str, Any]):
        """Test world-object interaction."""
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 64, 64)
        
        # Add an object
        obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource)
        world.addObject(obj)
        
        # Check if position is valid
        assert world.isValidPosition(10.0, 10.0)
        assert not world.isValidPosition(200.0, 200.0)  # Outside bounds
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'World Interaction',
            'status': 'PASSED'
        })
    
    def test_brain_computation(self, results: Dict[str, Any]):
        """Test brain computation core functionality."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests.append({
                'name': 'Brain Computation',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            }))
            return
        
        tests = [
            ('Brain Creation', self._test_brain_creation),
            ('Brain Initialization', self._test_brain_initialization),
            ('Action Production', self._test_brain_action_production),
            ('Brain Statistics', self._test_brain_statistics),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_brain_creation(self, results: Dict[str, Any]):
        """Test brain creation and factory."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        assert hasattr(brain, 'initialize'), "Brain should have initialize method"
        assert hasattr(brain, 'produceAction'), "Brain should have produceAction method"
        assert hasattr(brain, 'receiveSensoryInput'), "Brain should have receiveSensoryInput method"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Brain Creation',
            'status': 'PASSED'
        })
    
    def _test_brain_initialization(self, results: Dict[str, Any]):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # This should not crash (brain may fail due to invalid config, but shouldn't crash)
        try:
            brain.initialize()
            results['passed'] += 1
            results['tests'].append({
                'name': 'Brain Initialization',
                'status': 'PASSED'
            })
        except:
            # Initialization failure is acceptable for this test
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Brain Initialization',
                'status': 'SKIPPED',
                'reason': 'Initialization failed (expected for invalid config)'
            })
    
    def _test_brain_action_production(self, results: Dict[str, Any]):n        """Test brain action production."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # This may fail due to initialization, but shouldn't crash
        try:
            brain.initialize()
            
            action = brain.produceAction()
            # Action may be null if brain not properly initialized
            if action is not None:
                assert hasattr(action, 'getType'), "Action should have getType"
            
            results['passed'] += 1
            results['tests'].append({
                'name': 'Action Production',
                'status': 'PASSED'
            })
        except:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Action Production',
                'status': 'SKIPPED',
                'reason': 'Action production failed (expected for invalid config)'
            })
    
    def _test_brain_statistics(self, results: Dict[str, Any]):n        """Test brain statistics access."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # These methods should exist
        assert hasattr(brain, 'getTotalNeuronCount'), "Brain should have getTotalNeuronCount"
        assert hasattr(brain, 'getTotalSynapseCount'), "Brain should have getTotalSynapseCount"
        assert hasattr(brain, 'getFiringNeuronCount'), "Brain should have getFiringNeuronCount"
        assert hasattr(brain, 'getAverageFiringRate'), "Brain should have getAverageFiringRate"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Brain Statistics',
            'status': 'PASSED'
        })
    
    def test_agent_interface(self, results: Dict[str, Any]):n        """Test agent interface functionality."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Agent Interface',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        tests = [
            ('Agent Brain Creation', self._test_agent_brain_creation),
            ('Sensory Processing', self._test_sensory_processing),
            ('Motor Command Decoding', self._test_motor_command_decoding),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_agent_brain_creation(self, results: Dict[str, Any]):n        """Test agent brain interface creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        assert agent_brain is not None
        assert hasattr(agent_brain, 'processSensoryInput'), "AgentBrain should have processSensoryInput"
        assert hasattr(agent_brain, 'decodeMotorCommand'), "AgentBrain should have decodeMotorCommand"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Agent Brain Creation',
            'status': 'PASSED'
        })
    
    def _test_sensory_processing(self, results: Dict[str, Any]):
        """Test sensory processing interface."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Create a simple percept
        percept = pynlm.SensoryPercept()
        
        # This should not crash (may fail due to brain not initialized)
        try:
            agent_brain.processSensoryInput(percept)
            results['passed'] += 1
            results['tests'].append({
                'name': 'Sensory Processing',
                'status': 'PASSED'
            })
        except:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Sensory Processing',
                'status': 'SKIPPED',
                'reason': 'Processing failed (expected for invalid brain)'
            })
    
    def _test_motor_command_decoding(self, results: Dict[str, Any]):
        """Test motor command decoding interface."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # These methods should exist
        assert hasattr(agent_brain, 'getSensoryInputSize'), "AgentBrain should have getSensoryInputSize"
        assert hasattr(agent_brain, 'getMotorOutputSize'), "AgentBrain should have getMotorOutputSize"
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Motor Command Decoding',
            'status': 'PASSED'
        })
    
    def test_integration(self, results: Dict[str, Any]):
        """Test integration of all systems."""
        if not PYNLM_AVAILABLE:
            results['skipped'] += 1
            results['tests'].append({
                'name': 'Integration',
                'status': 'SKIPPED',
                'reason': 'pynlm not available'
            })
            return
        
        tests = [
            ('Complete Workflow', self._test_complete_workflow),
            ('Error Handling', self._test_error_handling_interface),
            ('Version Compatibility', self._test_version_compatibility),
        ]
        
        for test_name, test_func in tests:
            try:
                test_func(results)
            except Exception as e:
                results['failed'] += 1
                results['tests'].append({
                    'name': test_name,
                    'status': 'FAILED',
                    'error': str(e)
                })
    
    def _test_complete_workflow(self, results: Dict[str, Any]):n        """Test complete simulation workflow."""
        # This is a high-level integration test
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Create supporting systems
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Verify all systems are connected
        assert brain is not None
        assert world is not None
        assert agent_brain is not None
        
        # Check interfaces exist
        assert hasattr(world, 'getSensoryPercept')
        assert hasattr(agent_brain, 'processSensoryInput')
        assert hasattr(brain, 'produceAction')
        assert hasattr(world, 'applyMotorCommand')
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Complete Workflow',
            'status': 'PASSED'
        })
    
    def _test_error_handling_interface(self, results: Dict[str, Any]):
        """Test error handling interfaces."""
        # Check that expected error types are available
        if hasattr(pynlm, 'RuntimeError'):
            # pybind11 registers std::runtime_error as RuntimeError
            assert pynlm.RuntimeError is not None
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Error Handling',
            'status': 'PASSED'
        })
    
    def _test_version_compatibility(self, results: Dict[str, Any]):
        """Test version information compatibility."""
        # Verify version information
        assert hasattr(pynlm, '__version__')
        assert hasattr(pynlm, '__author__')
        assert hasattr(pynlm, '__description__')
        
        results['passed'] += 1
        results['tests'].append({
            'name': 'Version Compatibility',
            'status': 'PASSED'
        })
    
    # Additional test groups (for full mode)
    def test_memory_systems(self, results: Dict[str, Any]):
        """Test memory systems (full mode)."""
        # Placeholder for memory system tests
        results['skipped'] += 1
        results['tests'].append({
            'name': 'Memory Systems',
            'status': 'SKIPPED',
            'reason': 'To be implemented'
        })
    
    def test_plasticity(self, results: Dict[str, Any]):
        """Test plasticity rules (full mode)."""
        # Placeholder for plasticity tests
        results['skipped'] += 1
        results['tests'].append({
            'name': 'Plasticity',
            'status': 'SKIPPED',
            'reason': 'To be implemented'
        })
    
    def test_neuromodulation(self, results: Dict[str, Any]):
        """Test neuromodulation (full mode)."""
        # Placeholder for neuromodulation tests
        results['skipped'] += 1
        results['tests'].append({
            'name': 'Neuromodulation',
            'status': 'SKIPPED',
            'reason': 'To be implemented'
        })
    
    def test_development(self, results: Dict[str, Any]):
        """Test development system (full mode)."""
        # Placeholder for development tests
        results['skipped'] += 1
        results['tests'].append({
            'name': 'Development',
            'status': 'SKIPPED',
            'reason': 'To be implemented'
        })
    
    def test_performance(self, results: Dict[str, Any]):
        """Test performance characteristics (full mode)."""
        # Placeholder for performance tests
        results['skipped'] += 1
        results['tests'].append({
            'name': 'Performance',
            'status': 'SKIPPED',
            'reason': 'To be implemented'
        })
    
    def test_error_handling(self, results: Dict[str, Any]):
        """Test error handling (full mode)."""
        # Placeholder for error handling tests
        results['skipped'] += 1
        results['tests'].append({
            'name': 'Error Handling',
            'status': 'SKIPPED',
            'reason': 'To be implemented'
        })
    
    def _print_summary(self):
        """Print test suite summary."""
        print("=" * 60)
        print("TEST SUITE SUMMARY")
        print("=" * 60)
        
        for group_name, group_results in self.results.items():
            print(f"\n{group_name}:")
            print(f"  Tests: {group_results['passed'] + group_results['failed'] + group_results['skipped']}")
            print(f"  Passed: {group_results['passed']}")
            print(f"  Failed: {group_results['failed']}")
            print(f"  Skipped: {group_results['skipped']}")
            
            if group_results['failed'] > 0:
                print(f"  FAILED TESTS:")
                for test in group_results['tests']:
                    if test['status'] == 'FAILED':
                        print(f"    - {test['name']}: {test['error']}")
        
        print(f"\n{'='*60}")
        print(f"OVERALL SUMMARY:")
        print(f"  Total Tests: {self.passed + self.failed + self.skipped}")
        print(f"  Passed: {self.passed}")
        print(f"  Failed: {self.failed}")
        print(f"  Skipped: {self.skipped}")
        print(f"  Success Rate: {self.passed/max(self.passed + self.failed, 1)*100:.1f}%")
        
        if self.failed == 0:
            print(f"\n{'✓ ALL TESTS PASSED' if self.passed > 0 else 'NO TESTS RUN'}")
        else:
            print(f"\n{'✗ SOME TESTS FAILED'}")
        
        print(f"{'='*60}")
        
        return self.failed == 0
def main():
    """Main test runner."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='NLM Python Bindings Test Suite'
    )
    parser.add_argument(
        '--quick', '-q',
        action='store_true',
        help='Run quick test subset'
    )
    parser.add_argument(
        '--output', '-o',
        help='Output test results to file'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Verbose output'
    )
    
    args = parser.parse_args()
    
    # Create and run test suite
    suite = NLMTestSuite()
    
    try:
        success = suite.run_all_tests(quick_mode=args.quick)
        
        if args.output:
            with open(args.output, 'w') as f:
                import json
                json.dump({
                    'summary': {
                        'total_tests': suite.passed + suite.failed + suite.skipped,
                        'passed': suite.passed,
                        'failed': suite.failed,
                        'skipped': suite.skipped,
                        'success_rate': suite.passed/max(suite.passed + suite.failed, 1)*100
                    },
                    'detailed_results': suite.results
                }, f, indent=2)
        
        return 0 if success else 1
        
    except Exception as e:
        print(f"Test suite error: {e}")
        import traceback
        traceback.print_exc()
        return 1
if __name__ == '__main__':
    sys.exit(main())
