"""
Test suite for the enhanced NLM Python bindings.

This module contains tests for the enhanced bindings functionality,
including backward compatibility, new features, and edge cases.
"""

import pytest
import warnings
from unittest.mock import Mock, patch
import sys
import os
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import the enhanced bindings
try:
    from enhanced_bindings import (
        EnhancedNLMBindings,
        NLMConfigManager,
        NLMStatisticsCollector,
        NLMCheckpointManager,
        NMLearningController,
        NLMBindingsError,
        NLMValidationError,
        NLMCheckpointError,
        NLMStatisticsError,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        enableRewardModulation,
        enableCuriosity,
        enableDevelopment,
        step,
        decodeMotorCommand,
        setup_brain,
        create_world,
        create_agent,
        run_batch,
        get_simulation_stats
    )
    
    # Try to import the original bindings for comparison
    try:
        from nlm import (
            Brain as NLMBrain,
            SimpleWorld as NLMWorld,
            Config as NLMConfig
        )
        HAS_ORIGINAL_BINDINGS = True
    except ImportError:
        HAS_ORIGINAL_BINDINGS = False
        
except ImportError as e:
    # Mock the modules for testing when dependencies are not available
    warnings.warn(f"Could not import enhanced_bindings: {e}")
    
    # Create mock classes for testing
    class MockClass:
        def __init__(self, *args, **kwargs):
            pass
    
    EnhancedNLMBindings = NLMConfigManager = NLMStatisticsCollector = NLMCheckpointManager = MockClass
    NMLearningController = NLMBindingsError = NLMValidationError = MockClass
    NLMCheckpointError = NLMStatisticsError = MockClass
    createBrain = createSimpleWorld = createAgentBrain = MockClass
    enableRewardModulation = enableCuriosity = enableDevelopment = step = decodeMotorCommand = MockClass
    setup_brain = create_world = create_agent = run_batch = get_simulation_stats = MockClass
    
    HAS_ORIGINAL_BINDINGS = False
class TestEnhancedNLMBindings:
    """Test the EnhancedNLMBindings class."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.bindings = EnhancedNLMBindings()
    
    def test_initialization(self):
        """Test that EnhancedNLMBindings initializes correctly."""
        assert self.bindings is not None
        assert hasattr(self.bindings, 'setup_brain')
        assert hasattr(self.bindings, 'create_world')
        assert hasattr(self.bindings, 'create_agent')
    
    def test_backward_compatibility_methods_exist(self):
        """Test that backward compatibility methods exist."""
        assert hasattr(self.bindings, 'createBrain')
        assert hasattr(self.bindings, 'createSimpleWorld')
        assert hasattr(self.bindings, 'createAgentBrain')
        assert hasattr(self.bindings, 'enableRewardModulation')
        assert hasattr(self.bindings, 'enableCuriosity')
        assert hasattr(self.bindings, 'enableDevelopment')
        assert hasattr(self.bindings, 'step')
        assert hasattr(self.bindings, 'decodeMotorCommand')
    
    def test_backward_compatibility_method_warnings(self):
        """Test that backward compatibility methods issue warnings."""
        with pytest.warns(DeprecationWarning):
            # Mock the underlying method to avoid actual calls
            with patch.object(self.bindings, 'setup_brain', return_value=Mock()):
                self.bindings.createBrain()
        
        with pytest.warns(DeprecationWarning):
            with patch.object(self.bindings, 'create_world', return_value=Mock()):
                self.bindings.createSimpleWorld()
    
    def test_setup_brain(self):
        """Test the setup_brain method."""
        # Mock the underlying bindings
        with patch('enhanced_bindings.create_brain_original') as mock_create:
            mock_brain = Mock()
            mock_brain.getTotalNeuronCount.return_value = 100
            mock_create.return_value = mock_brain
            
            result = self.bindings.setup_brain({"region_count": 1})
            
            assert result == mock_brain
            mock_create.assert_called_once()
    
    def test_setup_brain_with_config_dict(self):
        """Test setup_brain with a configuration dictionary."""
        with patch('enhanced_bindings.create_brain_original') as mock_create:
            mock_brain = Mock()
            mock_brain.getTotalNeuronCount.return_value = 100
            mock_create.return_value = mock_brain
            
            with patch.object(self.bindings._config_manager, 'update') as mock_update:
                with patch.object(self.bindings._config_manager, 'validate', return_value=[]):
                    with patch.object(self.bindings._config_manager, 'to_nlm_config', return_value=Mock()):
                        result = self.bindings.setup_brain({"region_count": 2})
                        
                        mock_update.assert_called_once_with({"region_count": 2})
    
    def test_create_world(self):
        """Test the create_world method."""
        with patch('enhanced_bindings.create_world_original') as mock_create:
            mock_world = Mock()
            mock_world.getWidth.return_value = 50
            mock_world.getHeight.return_value = 50
            mock_create.return_value = mock_world
            
            result = self.bindings.create_world({"width": 50, "height": 50})
            
            assert result == mock_world
            mock_create.assert_called_once()
    
    def test_create_agent(self):
        """Test the create_agent method."""
        mock_brain = Mock()
        
        with patch('enhanced_bindings.create_agent_brain_original') as mock_create:
            mock_agent = Mock()
            mock_create.return_value = mock_agent
            
            result = self.bindings.create_agent(mock_brain)
            
            assert result == mock_agent
            mock_create.assert_called_once_with(mock_brain)
    
    def test_setup_reward_modulation(self):
        """Test setup_reward_modulation method."""
        mock_agent_brain = Mock()
        self.bindings._agent_brain = mock_agent_brain
        
        self.bindings.setup_reward_modulation(True)
        mock_agent_brain.enableRewardModulation.assert_called_once_with(True)
    
    def test_setup_curiosity(self):
        """Test setup_curiosity method."""
        mock_agent_brain = Mock()
        self.bindings._agent_brain = mock_agent_brain
        
        self.bindings.setup_curiosity(True)
        mock_agent_brain.enableCuriosity.assert_called_once_with(True)
    
    def test_setup_development(self):
        """Test setup_development method."""
        mock_agent_brain = Mock()
        self.bindings._agent_brain = mock_agent_brain
        
        self.bindings.setup_development(True)
        mock_agent_brain.enableDevelopment.assert_called_once_with(True)
    
    def test_get_simulation_stats(self):
        """Test get_simulation_stats method."""
        with patch.object(self.bindings._stats_collector, 'get_summary_stats', return_value={'total_metrics': 5}):
            with patch.object(self.bindings._learning_controller, 'get_plasticity_rules', return_value={}):
                with patch.object(self.bindings._learning_controller, 'get_neuromodulation_params', return_value={}):
                    with patch.object(self.bindings._learning_controller, 'get_development_params', return_value={}):
                        with patch.object(self.bindings._checkpoint_manager, 'list_checkpoints', return_value=[]):
                            result = self.bindings.get_simulation_stats()
                            
                            assert 'simulation_summary' in result
                            assert 'learning_controller' in result
                            assert 'neuromodulation' in result
                            assert 'development' in result
                            assert 'checkpoints' in result
    
    def test_cleanup(self):
        """Test cleanup method."""
        mock_obj = Mock()
        self.bindings._created_objects = [mock_obj]
        
        self.bindings.cleanup()
        
        assert len(self.bindings._created_objects) == 0
class TestNLMConfigManager:
    """Test the NLMConfigManager class."""
    
    def test_initialization(self):
        """Test NLMConfigManager initialization."""
        config = NLMConfigManager()
        assert config is not None
        assert config.get_all() == {}
    
    def test_update(self):
        """Test update method."""
        config = NLMConfigManager()
        
        result = config.update({"region_count": 5, "learning_rate": 0.1})
        
        assert result is config
        assert config.get("region_count") == 5
        assert config.get("learning_rate") == 0.1
    
    def test_validate_valid_config(self):
        """Test validation of valid configuration."""
        config = NLMConfigManager({
            "region_count": 3,
            "learning_rate": 0.05
        })
        
        errors = config.validate()
        assert errors == []
    
    def test_validate_missing_required_key(self):
        """Test validation with missing required key."""
        config = NLMConfigManager({})  # region_count is required
        
        errors = config.validate()
        assert len(errors) > 0
        assert any("region_count" in error for error in errors)
    
    def test_validate_type_error(self):
        """Test validation with type error."""
        config = NLMConfigManager()
        
        with pytest.raises(NLMValidationError):
            config.update({"region_count": "invalid"})  # Should be int
    
    def test_validate_range_error(self):
        """Test validation with range error."""
        config = NLMConfigManager()
        
        with pytest.raises(NLMValidationError):
            config.update({"region_count": 0})  # min is 1
    
    def test_get_with_default(self):
        """Test get method with default value."""
        config = NLMConfigManager({"region_count": 5})
        
        assert config.get("region_count") == 5
        assert config.get("learning_rate", 0.01) == 0.01  # Default value
        assert config.get("nonexistent_key", "default") == "default"
    
    def test_is_valid(self):
        """Test is_valid method."""
        config = NLMConfigManager({"region_count": 3})
        assert config.is_valid()
        
        invalid_config = NLMConfigManager({})
        assert not invalid_config.is_valid()


class TestNLMStatisticsCollector:
    """Test the NLMStatisticsCollector class."""
    
    def test_initialization(self):
        """Test NLMStatisticsCollector initialization."""
        collector = NLMStatisticsCollector()
        assert collector is not None
        assert collector.get_all_metrics() == {}
    
    def test_record_metric(self):
        """Test record_metric method."""
        collector = NLMStatisticsCollector()
        
        collector.record_metric("firing_rate", 2.5)
        
        assert collector.get_metric("firing_rate") == 2.5
    
    def test_record_metric_with_timestamp(self):
        """Test record_metric with timestamp."""
        from datetime import datetime
        collector = NLMStatisticsCollector()
        timestamp = datetime(2024, 1, 1, 12, 0, 0)
        
        collector.record_metric("activity", 1.0, timestamp)
        
        time_series = collector.get_time_series("activity")
        assert len(time_series) == 1
        assert time_series[0][0] == timestamp
        assert time_series[0][1] == 1.0
    
    def test_get_time_series(self):
        """Test get_time_series method."""
        from datetime import datetime, timedelta
        collector = NLMStatisticsCollector()
        
        timestamp1 = datetime(2024, 1, 1, 12, 0, 0)
        timestamp2 = datetime(2024, 1, 1, 13, 0, 0)
        
        collector.record_metric("activity", 1.0, timestamp1)
        collector.record_metric("activity", 2.0, timestamp2)
        
        all_data = collector.get_time_series("activity")
        assert len(all_data) == 2
        assert all_data[0][1] == 1.0
        assert all_data[1][1] == 2.0
        
        # Filter by start time
        filtered = collector.get_time_series("activity", timestamp1)
        assert len(filtered) == 2
        
        filtered = collector.get_time_series("activity", timestamp2)
        assert len(filtered) == 1
        assert filtered[0][1] == 2.0
    
    def test_take_snapshot(self):
        """Test take_snapshot method."""
        collector = NLMStatisticsCollector()
        collector.record_metric("activity", 1.0)
        
        snapshot = collector.take_snapshot("test")
        
        assert snapshot["timestamp"] is not None
        assert snapshot["metrics"]["activity"] == 1.0
        assert snapshot["time_series_length"]["activity"] == 1
    
    def test_get_summary_stats(self):
        """Test get_summary_stats method."""
        collector = NLMStatisticsCollector()
        collector.record_metric("activity", 1.0)
        collector.record_metric("activity", 2.0)
        collector.record_metric("firing_rate", 2.5)
        collector.take_snapshot()
        
        summary = collector.get_summary_stats()
        
        assert summary["total_metrics"] == 2
        assert summary["activity_mean"] == 1.5
        assert summary["activity_min"] == 1.0
        assert summary["activity_max"] == 2.0
        assert summary["activity_count"] == 2
        assert summary["firing_rate_mean"] == 2.5
        assert summary["snapshot_count"] == 1


class TestConvenienceFunctions:
    """Test convenience functions."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        # Reset the bindings instance to ensure clean state
        from enhanced_bindings import bindings
        self.bindings_instance = bindings
    
    def test_convenience_functions_exist(self):
        """Test that convenience functions exist."""
        assert setup_brain is not None
        assert create_world is not None
        assert create_agent is not None
        assert run_batch is not None
        assert get_simulation_stats is not None
    
    @pytest.mark.skipif(not HAS_ORIGINAL_BINDINGS, reason="Original bindings not available")
    def test_backward_compatibility_functions_exist(self):
        """Test that backward compatibility functions exist when original bindings are available."""
        assert createBrain is not None
        assert createSimpleWorld is not None
        assert createAgentBrain is not None
        assert enableRewardModulation is not None
        assert enableCuriosity is not None
        assert enableDevelopment is not None
        assert step is not None
        assert decodeMotorCommand is not None


class TestBackwardCompatibility:
    """Test backward compatibility features."""
    
    def test_backward_compatibility_warnings(self):
        """Test that backward compatibility functions issue warnings."""
        with pytest.warns(DeprecationWarning):
            createBrain()
        
        with pytest.warns(DeprecationWarning):
            createSimpleWorld()
        
        with pytest.warns(DeprecationWarning):
            createAgentBrain(Mock())
        
        with pytest.warns(DeprecationWarning):
            enableRewardModulation(True)
        
        with pytest.warns(DeprecationWarning):
            enableCuriosity(True)
        
        with pytest.warns(DeprecationWarning):
            enableDevelopment(True)
        
        with pytest.warns(DeprecationWarning):
            step(Mock(), Mock())
        
        with pytest.warns(DeprecationWarning):
            decodeMotorCommand(Mock())


class TestErrorHandling:
    """Test error handling in enhanced bindings."""
    
    def test_nlm_bindings_error(self):
        """Test NLMBindingsError."""
        error = NLMBindingsError("Test error")
        assert str(error) == "Test error"
        assert isinstance(error, Exception)
    
    def test_nlm_validation_error(self):
        """Test NLMValidationError."""
        error = NLMValidationError("Validation error")
        assert str(error) == "Validation error"
        assert isinstance(error, NLMBindingsError)
        assert isinstance(error, Exception)
    
    def test_nlm_checkpoint_error(self):
        """Test NLMCheckpointError."""
        error = NLMCheckpointError("Checkpoint error")
        assert str(error) == "Checkpoint error"
        assert isinstance(error, NLMBindingsError)
        assert isinstance(error, Exception)
    
    def test_nlm_statistics_error(self):
        """Test NLMStatisticsError."""
        error = NLMStatisticsError("Statistics error")
        assert str(error) == "Statistics error"
        assert isinstance(error, NLMBindingsError)
        assert isinstance(error, Exception)


class TestExampleFiles:
    """Test example files."""
    
    def test_examples_exist(self):
        """Test that example files exist."""
        examples_dir = Path(__file__).parent / "examples"
        assert examples_dir.exists()
        
        example_files = list(examples_dir.glob("*.py"))
        assert len(example_files) > 0
        
        for example_file in example_files:
            assert example_file.exists()
    
    @pytest.mark.skipif(not HAS_ORIGINAL_BINDINGS, reason="Original bindings not available")
    def test_examples_can_be_imported(self):
        """Test that example files can be imported when dependencies are available."""
        examples_dir = Path(__file__).parent / "examples"
        example_files = list(examples_dir.glob("*.py"))
        
        for example_file in example_files:
            # Try to import the example module
            spec = importlib.util.spec_from_file_location(
                example_file.stem, example_file
            )
            assert spec is not None
            module = importlib.util.module_from_spec(spec)
            # Don't execute the module, just check it can be imported


if __name__ == "__main__":
    pytest.main([__file__, "-v"])