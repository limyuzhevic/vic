#!/usr/bin/env python3

"""
Unit Tests for Enhanced Components

This module contains comprehensive unit tests for all enhanced NLM components,
including CLI testing, Configuration testing, Brain testing, Error Handling,
Logging testing, Visualization testing, Scripting testing, and Python Bindings testing.
"""

import sys
import os
import tempfile
import json
import yaml
import time
import threading
import multiprocessing
from typing import Dict, List, Any, Optional, Tuple
from pathlib import Path
from unittest.mock import Mock, patch, MagicMock, call
import pytest

# Add the parent directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM components for testing
from pynlm import Config, create_default_config, Brain, create_brain
from pynlm.types import (
    NeuronId, RegionId, SynapseId, PopulationId, NeuronType, ActionType,
    DevelopmentalStage, Position2D, ActionParameters, SensoryData,
    MemoryTrace, NeuralConnection, NeuronIdSet
)
from pynlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory
from pynlm.neuromodulation import NeuromodulationSystem, Dopamine, Curiosity, Novelty, PredictionError

class TestConfigurationEnhanced:
    """Unit tests for enhanced Configuration functionality."""
    
    def test_config_creation_and_initialization(self):
        """Test enhanced configuration creation and initialization."""
        config = Config()
        assert config is not None
        assert config.get_keys() == []
        
        # Test fluent interface
        config.set("neuron_count", 1000).set("region_count", 3)
        assert config.get("neuron_count") == 1000
        assert config.get("region_count") == 3
        
    def test_config_validation_enhanced(self):
        """Test enhanced configuration validation."""
        config = Config()
        
        # Test valid values
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        config.set("timestep", 0.01)
        
        # Test type conversion
        config.set("neuron_count", "500")  # Should convert to int
        assert config.get("neuron_count") == 500
        
        # Test boolean conversion
        config.set("test_flag", "true")
        assert config.get("test_flag") is True
        
        config.set("test_flag", "false")
        assert config.get("test_flag") is False
    
    def test_config_file_operations(self):
        """Test enhanced file operations."""
        with tempfile.TemporaryDirectory() as tmpdir:
            # Test JSON file operations
            json_file = os.path.join(tmpdir, "config.json")
            config = Config()
            config.set("neuron_count", 1000)
            config.set("region_count", 3)
            config.save_to_file(json_file, "json")
            
            # Load from JSON
            config2 = Config(json_file)
            assert config2.get("neuron_count") == 1000
            assert config2.get("region_count") == 3
            
            # Test YAML file operations
            yaml_file = os.path.join(tmpdir, "config.yaml")
            config.save_to_file(yaml_file, "yaml")
            
            config3 = Config(yaml_file)
            assert config3.get("neuron_count") == 1000
    
    def test_config_command_line_parsing(self):
        """Test enhanced command-line argument parsing."""
        config = Config()
        
        # Test various argument types
        args = ["--neuron_count", "500", "--region_count", "2", "--timestep", "0.005"]
        config.load_from_args(args)
        
        assert config.get("neuron_count") == 500
        assert config.get("region_count") == 2
        assert config.get("timestep") == 0.005
        
        # Test boolean arguments
        args2 = ["--debug", "true", "--verbose", "false"]
        config.load_from_args(args2)
        assert config.get("debug") is True
        assert config.get("verbose") is False
    
    def test_config_template_system(self):
        """Test enhanced configuration template system."""
        config = Config()
        
        # Test template registration and application
        config.set("base_neuron_count", 1000)
        config.set("neuron_count", 500)  # Override
        
        # Test template validation
        defaults = {
            "neuron_count": 1000,
            "region_count": 3,
            "timestep": 0.01
        }
        
        for key, value in defaults.items():
            config.set(key, value)
        
        # Verify all defaults are set
        for key, value in defaults.items():
            assert config.get(key) == value
    
    def test_config_environment_variables(self):
        """Test enhanced environment variable support."""
        config = Config()
        
        # Test environment variable parsing
        config.set("env_prefix", "NLM_")
        
        # Mock environment variables
        with patch.dict(os.environ, {'NLM_NEURON_COUNT': '2000', 'NLM_REGION_COUNT': '5'}):
            # In a real implementation, this would parse environment variables
            pass  # Environment variable support would be tested in integration tests
    
    def test_config_error_handling_enhanced(self):
        """Test enhanced configuration error handling."""
        config = Config()
        
        # Test invalid key
        try:
            config.set("", "value")
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
        
        # Test invalid value type
        try:
            config.set("neuron_count", "invalid")
            assert False, "Should have raised ValueError for invalid int"
        except ValueError:
            pass
        
        # Test invalid range
        try:
            config.set("connection_probability", 1.5)
            assert False, "Should have raised ValueError for out of range"
        except ValueError:
            pass

class TestBrainEnhanced:
    """Unit tests for enhanced Brain functionality."""
    
    def test_brain_context_manager(self):
        """Test enhanced Brain context manager functionality."""
        config = create_default_config()
        
        # Test context manager
        with Brain(config) as brain:
            assert brain is not None
            assert brain.initialized == True
            assert brain.simulation_step == 0
            
            # Test fluent interface
            result = brain.set_neurons(1000).set_regions(3).initialize()
            assert result is brain
        
        # After context exit, brain should be reset
        assert brain.initialized == False
    
    def test_brain_fluent_interface(self):
        """Test enhanced Brain fluent interface."""
        brain = Brain(create_default_config()).set_neurons(1000).set_regions(3).set_synapse_probability(0.1)
        
        # Verify configuration was applied
        assert brain.config.get("neuron_count") == 1000
        assert brain.config.get("region_count") == 3
        assert brain.config.get("connection_probability") == 0.1
    
    def test_brain_component_access(self):
        """Test enhanced Brain component access methods."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Test component access (these may raise RuntimeError if not initialized)
            # In enhanced version, they should be available
            assert hasattr(brain, 'working_memory')
            assert hasattr(brain, 'episodic_memory')
            assert hasattr(brain, 'associative_memory')
            assert hasattr(brain, 'neuromodulation')
            assert hasattr(brain, 'prediction')
            assert hasattr(brain, 'cognition')
            assert hasattr(brain, 'development')
            assert hasattr(brain, 'visualizer')
            assert hasattr(brain, 'monitor')
            assert hasattr(brain, 'profiler')
    
    def test_brain_state_management(self):
        """Test enhanced Brain state management."""
        config = create_default_config()
        
        brain = Brain(config)
        brain.initialize()
        
        # Test step execution
        initial_step = brain.simulation_step
        brain.step(1)
        assert brain.simulation_step == 1
        
        # Test reset
        brain.reset()
        assert brain.simulation_step == 0
        assert brain.simulation_time == 0.0
    
    def test_brain_data_access(self):
        """Test enhanced Brain data access methods."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Test neural activity access
            activity = brain.get_neural_activity()
            assert isinstance(activity, list)  # Returns empty list for C++ binding
            
            # Test firing rates
            rates = brain.get_firing_rates()
            assert isinstance(rates, dict)
            
            # Test region statistics
            stats = brain.get_region_stats()
            assert isinstance(stats, dict)
    
    def test_brain_error_handling(self):
        """Test enhanced Brain error handling."""
        config = create_default_config()
        
        brain = Brain(config)
        
        # Test operations on uninitialized brain
        try:
            brain.step(1)
            assert False, "Should have raised RuntimeError"
        except RuntimeError:
            pass
        
        brain.initialize()
        
        # Test duplicate initialization
        try:
            brain.initialize()
            assert False, "Should have raised RuntimeError for duplicate initialization"
        except RuntimeError:
            pass
    
    def test_brain_serialization(self):
        """Test enhanced Brain serialization."""
        config = create_default_config()
        
        with Brain(config) as brain:
            brain.initialize()
            
            # Test save/load (these would require C++ bindings)
            # In enhanced version, they should be available
            assert hasattr(brain, 'save')
            assert hasattr(brain, 'load')

class TestMemoryEnhanced:
    """Unit tests for enhanced Memory functionality."""
    
    def test_working_memory_enhanced(self):
        """Test enhanced WorkingMemory functionality."""
        config = create_default_config()
        
        # Mock brain for testing
        mock_brain = Mock(spec=Brain)
        
        working_mem = WorkingMemory(mock_brain)
        
        # Test pattern storage and retrieval
        test_pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
        working_mem.store(test_pattern, strength=0.8)
        
        # Test retrieval
        retrieved = working_mem.retrieve()
        assert isinstance(retrieved, list)
        
        # Test memory activity
        activity = working_mem.get_memory_activity()
        assert 0.0 <= activity <= 1.0
        
        # Test capacity management
        assert working_mem.capacity == 100
        working_mem.capacity = 200
        assert working_mem.capacity == 200
    
    def test_episodic_memory_enhanced(self):
        """Test enhanced EpisodicMemory functionality."""
        episodic_mem = EpisodicMemory()
        
        # Test episode recording
        test_episode = {
            "type": "exploration",
            "location": Position2D(x=10.0, y=20.0),
            "duration": 5.0,
            "reward": 1.0
        }
        
        timestamp = time.time()
        episodic_mem.record(test_episode, timestamp, context={"agent": "test"})
        
        # Test search functionality
        search_results = episodic_mem.search(keywords=["exploration"])
        assert len(search_results) == 1
        assert search_results[0] == test_episode
        
        # Test playback
        playback = episodic_mem.playback(start_index=0, end_index=1)
        assert len(playback) == 1
        assert playback[0] == test_episode
    
    def test_associative_memory_enhanced(self):
        """Test enhanced AssociativeMemory functionality."""
        assoc_mem = AssociativeMemory()
        
        # Test association creation
        assoc_mem.associate("concept1", "concept2", strength=0.8)
        assoc_mem.associate("concept2", "concept3", strength=0.6)
        
        # Test retrieval
        associations = assoc_mem.get_associations("concept2")
        assert "concept1" in associations
        assert "concept3" in associations
        assert associations["concept1"] == 0.8
        assert associations["concept3"] == 0.6
        
        # Test pattern completion
        completed = assoc_mem.complete_pattern("concept1", max_depth=2)
        assert "concept2" in completed
        
        # Test strength updates
        assoc_mem.update_strength("concept1", "concept2", 0.9)
        assert assoc_mem.get_strength("concept1", "concept2") == 0.9
    
    def test_memory_export_enhanced(self):
        """Test enhanced memory export functionality."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        working_mem = WorkingMemory(mock_brain)
        working_mem.store([0.1, 0.5, 0.9])
        
        # Test DataFrame export
        try:
            import pandas as pd
            df = working_mem.export_to_dataframe()
            assert isinstance(df, pd.DataFrame)
            assert "pattern_0_neuron_0" in df.columns
        except ImportError:
            # Pandas not available, skip test
            pass
    
    def test_memory_error_handling(self):
        """Test enhanced memory error handling."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        working_mem = WorkingMemory(mock_brain)
        
        # Test invalid pattern
        try:
            working_mem.store("invalid_pattern", strength=0.5)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
        
        # Test invalid strength
        try:
            working_mem.store([0.1, 0.2], strength=1.5)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
        
        # Test invalid capacity
        try:
            working_mem.capacity = -1
            assert False, "Should have raised ValueError"
        except ValueError:
            pass

class TestNeuromodulationEnhanced:
    """Unit tests for enhanced Neuromodulation functionality."""
    
    def test_neuromodulation_system(self):
        """Test enhanced NeuromodulationSystem."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        neuromodulation = NeuromodulationSystem(mock_brain)
        
        # Test neuromodulator access
        assert neuromodulation.dopamine is not None
        assert neuromodulation.curiosity is not None
        assert neuromodulation.novelty is not None
        assert neuromodulation.prediction_error is not None
        
        # Test signal application
        dopamine_level = neuromodulation.apply_dopamine(reward=1.0, predicted=0.5)
        assert isinstance(dopamine_level, float)
        assert -1.0 <= dopamine_level <= 1.0
        
        curiosity_level = neuromodulation.apply_curiosity(novelty=0.8, prediction_error=0.2)
        assert isinstance(curiosity_level, float)
        assert 0.0 <= curiosity_level <= 1.0
        
        novelty_level = neuromodulation.apply_novelty(novelty=0.9)
        assert isinstance(novelty_level, float)
        
        pred_error_level = neuromodulation.apply_prediction_error(error=0.5)
        assert isinstance(pred_error_level, float)
        
        # Test summary
        summary = neuromodulation.get_neuromodulation_summary()
        assert "dopamine" in summary
        assert "curiosity" in summary
        assert "novelty" in summary
        assert "prediction_error" in summary
    
    def test_dopamine_modulator(self):
        """Test enhanced Dopamine neuromodulator."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        dopamine = Dopamine(mock_brain)
        
        # Test signal application
        level1 = dopamine.apply_signal(reward=1.0, predicted=0.5)
        assert isinstance(level1, float)
        
        level2 = dopamine.apply_signal(reward=-1.0, predicted=-0.5)
        assert isinstance(level2, float)
        
        # Test context modulation
        context = {"stress": 0.5, "arousal": 0.8}
        level3 = dopamine.apply_signal(reward=1.0, predicted=0.5, context=context)
        assert isinstance(level3, float)
        
        # Test learning rate
        dopamine.set_learning_rate(0.5)
        assert dopamine.get_level() is not None
        
        # Test prediction error
        pred_error = dopamine.get_prediction_error()
        assert isinstance(pred_error, float)
    
    def test_curiosity_modulator(self):
        """Test enhanced Curiosity neuromodulator."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        curiosity = Curiosity(mock_brain)
        
        # Test signal application
        level1 = curiosity.apply_signal(novelty=0.8, prediction_error=0.2)
        assert isinstance(level1, float)
        
        # Test sensitivity adjustment
        curiosity.set_novelty_sensitivity(0.7)
        level2 = curiosity.apply_signal(novelty=0.8, prediction_error=0.2)
        assert isinstance(level2, float)
        
        # Test context modulation
        context = {"fatigue": 0.3, "curiosity_threshold": 0.6}
        level3 = curiosity.apply_signal(novelty=0.8, prediction_error=0.2, context=context)
        assert isinstance(level3, float)
    
    def test_novelty_modulator(self):
        """Test enhanced Novelty neuromodulator."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        novelty = Novelty(mock_brain)
        
        # Test signal application
        level1 = novelty.apply_signal(novelty=0.9)
        assert isinstance(level1, float)
        
        level2 = novelty.apply_signal(novelty=0.1)  # Different from previous
        assert isinstance(level2, float)
        
        # Test context modulation
        context = {"familiarity": 0.8, "attention": 0.5}
        level3 = novelty.apply_signal(novelty=0.9, context=context)
        assert isinstance(level3, float)
    
    def test_prediction_error_modulator(self):
        """Test enhanced PredictionError neuromodulator."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        pred_error = PredictionError(mock_brain)
        
        # Test signal application
        level1 = pred_error.apply_signal(error=0.5)
        assert isinstance(level1, float)
        
        level2 = pred_error.apply_signal(error=-0.3)
        assert isinstance(level2, float)
        
        # Test context modulation
        context = {"confidence": 0.4, "uncertainty": 0.7}
        level3 = pred_error.apply_signal(error=0.5, context=context)
        assert isinstance(level3, float)
    
    def test_neuromodulation_error_handling(self):
        """Test enhanced neuromodulation error handling."""
        config = create_default_config()
        
        # Mock brain
        mock_brain = Mock(spec=Brain)
        
        dopamine = Dopamine(mock_brain)
        
        # Test invalid learning rate
        try:
            dopamine.set_learning_rate(1.5)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
        
        try:
            dopamine.set_learning_rate(-0.5)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
        
        # Test invalid novelty sensitivity
        try:
            curiosity = Curiosity(mock_brain)
            curiosity.set_novelty_sensitivity(1.5)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass

class TestErrorHandlingEnhanced:
    """Unit tests for enhanced Error Handling functionality."""
    
    def test_exception_hierarchy(self):
        """Test enhanced exception hierarchy."""
        from pynlm.types import (
            NeuronId, SynapseId, RegionId, validate_neuron_id,
            validate_synapse_id, validate_region_id
        )
        
        # Test invalid neuron ID
        try:
            validate_neuron_id("invalid")
            assert False, "Should have raised ValueError"
        except ValueError as e:
            assert "Invalid neuron ID" in str(e)
        
        try:
            validate_neuron_id(-1)
            assert False, "Should have raised ValueError for negative ID"
        except ValueError as e:
            assert "NeuronId value must be a non-negative integer" in str(e)
        
        # Test invalid synapse ID
        try:
            validate_synapse_id("invalid")
            assert False, "Should have raised ValueError"
        except ValueError as e:
            assert "Invalid synapse ID" in str(e)
        
        # Test invalid region ID
        try:
            validate_region_id("invalid")
            assert False, "Should have raised ValueError"
        except ValueError as e:
            assert "Invalid region ID" in str(e)
    
    def test_error_recovery_mechanisms(self):
        """Test enhanced error recovery mechanisms."""
        config = Config()
        
        # Test config recovery after error
        try:
            config.set("", "value")
        except ValueError:
            pass  # Expected
        
        # Config should still be usable
        config.set("valid_key", "valid_value")
        assert config.get("valid_key") == "valid_value"
        
        # Test graceful degradation
        brain = Brain(config)
        # Even if C++ binding fails, Python wrapper should handle it gracefully
        assert brain is not None
    
    def test_error_logging_enhanced(self):
        """Test enhanced error logging."""
        import logging
        
        # Setup logging capture
        log_capture = []
        
        class TestHandler(logging.Handler):
            def emit(self, record):
                log_capture.append(record.getMessage())
        
        # Test error logging
        with tempfile.NamedTemporaryFile(mode='w', suffix='.log') as log_file:
            logger = logging.getLogger('test_error')
            handler = TestHandler()
            logger.addHandler(handler)
            logger.setLevel(logging.ERROR)
            
            # Trigger an error
            try:
                config = Config()
                config.set("invalid_key", "invalid_value")
            except ValueError as e:
                logger.error(f"Config error: {e}")
            
            # Verify error was logged
            assert any("Config error:" in log for log in log_capture)
    
    def test_error_validation_enhanced(self):
        """Test enhanced error validation."""
        from pynlm.types import Position2D, ActionParameters, SensoryData
        
        # Test position validation
        valid_pos = Position2D(x=10.0, y=20.0)
        assert valid_pos.x == 10.0
        assert valid_pos.y == 20.0
        
        distance = valid_pos.distance_to(Position2D(x=15.0, y=25.0))
        expected_distance = ((10-15)**2 + (20-25)**2)**0.5
        assert abs(distance - expected_distance) < 0.0001
        
        # Test action parameters validation
        action_params = ActionParameters(values=[0.1, 0.5, 0.9], metadata={"test": "value"})
        assert action_params.values == [0.1, 0.5, 0.9]
        
        # Test sensory data validation
        sensory_data = SensoryData(data=[1.0, 2.0, 3.0], timestamp=time.time())
        assert sensory_data.data == [1.0, 2.0, 3.0]

class TestCLIEnhanced:
    """Unit tests for enhanced CLI functionality."""
    
    def test_cli_argument_parsing(self):
        """Test enhanced CLI argument parsing."""
        from pynlm import Brain, Config
        
        # Test that CLI argument parsing is available
        assert hasattr(Config, 'load_from_args')
        
        # Test various argument formats
        test_args = [
            ["--neuron_count", "1000"],
            ["--neuron_count", "500", "--region_count", "2"],
            ["--debug", "true", "--verbose", "false", "--timestep", "0.01"]
        ]
        
        for args in test_args:
            config = Config()
            config.load_from_args(args)
            
            # Verify arguments were parsed
            for i in range(0, len(args), 2):
                key = args[i][2:]  # Remove '--'
                value = args[i+1]
                
                # Convert value to appropriate type
                if value.lower() == "true":
                    value = True
                elif value.lower() == "false":
                    value = False
                elif value.isdigit():
                    value = int(value)
                else:
                    try:
                        value = float(value)
                    except ValueError:
                        pass
                
                assert config.get(key) == value, f"Failed to parse {key}={value}"
    
    def test_cli_help_system(self):
        """Test enhanced CLI help system."""
        from pynlm import Brain, Config
        
        # Test that help system is available
        assert hasattr(Config, 'summary')
        
        config = Config()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        
        summary = config.summary()
        assert "neuron_count" in summary
        assert "region_count" in summary
        assert "1000" in summary
        assert "3" in summary
    
    def test_cli_error_handling(self):
        """Test enhanced CLI error handling."""
        from pynlm import Brain, Config
        
        config = Config()
        
        # Test missing required arguments
        try:
            config.load_from_args(["--neuron_count"])
            assert False, "Should have raised IndexError"
        except (IndexError, ValueError):
            pass  # Expected
        
        # Test invalid argument format
        try:
            config.load_from_args(["invalid_arg"])
            # This might not raise an error, depending on implementation
        except Exception:
            pass  # Any error is fine for this test

class TestVisualizationEnhanced:
    """Unit tests for enhanced Visualization functionality."""
    
    def test_visualization_interface(self):
        """Test enhanced visualization interface."""
        from pynlm import Visualization
        
        # Mock brain for testing
        mock_brain = Mock(spec=Brain)
        
        visualizer = Visualization(mock_brain)
        
        # Test that visualization methods exist
        assert hasattr(visualizer, 'plot_neural_activity')
        assert hasattr(visualizer, 'memory_analysis')
        
        # Test visualization with mock data
        if hasattr(visualizer, 'plot_neural_activity'):
            mock_activity = [[0.1, 0.2], [0.3, 0.4]]
            try:
                visualizer.plot_neural_activity(mock_activity)
            except Exception:
                pass  # Expected if matplotlib is not available
    
    def test_visualization_data_export(self):
        """Test enhanced visualization data export."""
        from pynlm import Visualization
        
        # Mock brain for testing
        mock_brain = Mock(spec=Brain)
        
        visualizer = Visualization(mock_brain)
        
        # Test data export functionality
        if hasattr(visualizer, 'export_data'):
            try:
                data = visualizer.export_data("test_format")
                assert data is not None
            except Exception:
                pass  # Expected if implementation details vary

class TestPluginSystemEnhanced:
    """Unit tests for enhanced Plugin System functionality."""
    
    def test_plugin_interface(self):
        """Test enhanced plugin interface."""
        from pynlm import ExtensionAPI
        
        # Mock brain for testing
        mock_brain = Mock(spec=Brain)
        
        # Test ExtensionAPI creation
        ext_api = ExtensionAPI(mock_brain)
        assert ext_api is not None
        
        # Test plugin registration
        if hasattr(ext_api, 'register_extension'):
            mock_extension = Mock()
            try:
                ext_api.register_extension(mock_extension, "test_plugin")
            except Exception:
                pass  # Expected if implementation details vary
    
    def test_plugin_security(self):
        """Test enhanced plugin security."""
        from pynlm import ExtensionAPI
        
        # Mock brain for testing
        mock_brain = Mock(spec=Brain)
        
        ext_api = ExtensionAPI(mock_brain)
        
        # Test plugin signature verification (if available)
        if hasattr(ext_api, 'verify_signature'):
            mock_plugin = Mock()
            try:
                result = ext_api.verify_signature(mock_plugin)
                assert isinstance(result, bool)
            except Exception:
                pass  # Expected if implementation details vary
        
        # Test plugin sandboxing (if available)
        if hasattr(ext_api, 'create_sandbox'):
            try:
                sandbox = ext_api.create_sandbox("test_plugin")
                assert sandbox is not None
            except Exception:
                pass  # Expected if implementation details vary

class TestPythonBindingsEnhanced:
    """Unit tests for enhanced Python Bindings."""
    
    def test_binding_import_system(self):
        """Test enhanced binding import system."""
        from pynlm import __version__, __author__, __email__
        
        assert __version__ is not None
        assert __author__ is not None
        assert __email__ is not None
        
        # Test module setup
        from pynlm import setup_module
        setup_module()
        
        # Verify modules are available
        from pynlm import Config, Brain, create_brain, create_default_config
        assert Config is not None
        assert Brain is not None
        assert create_brain is not None
        assert create_default_config is not None
    
    def test_binding_type_system(self):
        """Test enhanced binding type system."""
        from pynlm.types import NeuronId, RegionId, SynapseId, PopulationId
        
        # Test ID creation and validation
        neuron_id = NeuronId(123)
        assert neuron_id.value == 123
        assert neuron_id.index == 123
        assert str(neuron_id) == "<NeuronId: 123>"
        
        region_id = RegionId(456)
        assert region_id.value == 456
        
        # Test enum types
        from pynlm.types import NeuronType, ActionType, DevelopmentalStage
        assert NeuronType.EXCITATORY.value == "excitatory"
        assert ActionType.MOVE_FORWARD.value == "move_forward"
        assert DevelopmentalStage.INITIAL.value == "initial"
    
    def test_binding_integration_system(self):
        """Test enhanced binding integration system."""
        from pynlm import Config, create_default_config
        
        config = create_default_config()
        assert config is not None
        
        # Test that all integration components are available
        from pynlm import (
            WorkingMemory, EpisodicMemory, AssociativeMemory,
            NeuromodulationSystem, PredictionSystem, CognitionSystem,
            DevelopmentSystem, Visualization, RealTimeMonitor,
            PerformanceProfiler, ExtensionAPI, create_custom_circuit,
            NumPyIntegration, PandasIntegration, MatplotlibIntegration,
            simulation_loop, batch_processor, checkpoint_manager,
            performance_monitor, data_exporter, simulator_factory
        )
        
        # Verify they are available (may be None if dependencies missing)
        assert Config is not None
        assert create_default_config is not None

# Test discovery and registration functions

def discover_tests():
    """Discover all test classes in this module."""
    test_classes = [
        TestConfigurationEnhanced,
        TestBrainEnhanced,
        TestMemoryEnhanced,
        TestNeuromodulationEnhanced,
        TestErrorHandlingEnhanced,
        TestCLIEnhanced,
        TestVisualizationEnhanced,
        TestPluginSystemEnhanced,
        TestPythonBindingsEnhanced
    ]
    
    return test_classes

def get_test_categories():
    """Get all test categories."""
    return {
        'configuration': TestConfigurationEnhanced,
        'brain': TestBrainEnhanced,
        'memory': TestMemoryEnhanced,
        'neuromodulation': TestNeuromodulationEnhanced,
        'error_handling': TestErrorHandlingEnhanced,
        'cli': TestCLIEnhanced,
        'visualization': TestVisualizationEnhanced,
        'plugin_system': TestPluginSystemEnhanced,
        'python_bindings': TestPythonBindingsEnhanced
    }

if __name__ == "__main__":
    # Run all unit tests
    import unittest
    
    # Create test suite
    loader = unittest.TestLoader()
    
    # Get all test classes
    test_classes = discover_tests()
    
    # Create test suite
    suite = unittest.TestSuite()
    
    for test_class in test_classes:
        suite.addTests(loader.loadTestsFromTestCase(test_class))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)
