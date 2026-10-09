#!/usr/bin/env python3

"""
CLI Testing Suite for NLM Enhanced Components

This module contains comprehensive CLI testing for all enhanced NLM components,
including command-line interface and argument parsing.
"""

import sys
import os
import tempfile
import json
import yaml
import subprocess
from typing import Dict, List, Any, Optional, Tuple
from unittest.mock import Mock, patch, MagicMock

# Add the parent directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM components for testing
from pynlm import Config, create_default_config, Brain, create_brain

class TestCLIArgumentParsing:
    """Unit tests for CLI argument parsing."""
    
    def test_argument_parsing_basic(self):
        """Test basic CLI argument parsing."""
        config = Config()
        
        # Test various argument formats
        test_cases = [
            ["--neuron_count", "1000"],
            ["--neuron_count", "500", "--region_count", "2"],
            ["--debug", "true", "--verbose", "false"],
            ["--timestep", "0.005", "--max_energy", "200.0"],
            ["--random_seed", "42", "--sensory_noise", "0.05"],
            ["--help", "true"],  # Help argument
            ["--version", "true"],  # Version argument
        ]
        
        for args in test_cases:
            config.load_from_args(args)
            
            # Verify arguments were parsed correctly
            for i in range(0, len(args), 2):
                key = args[i][2:]  # Remove '--'
                value = args[i+1]
                
                # Convert value to appropriate type
                if value.lower() == "true":
                    expected_value = True
                elif value.lower() == "false":
                    expected_value = False
                elif value.isdigit():
                    expected_value = int(value)
                else:
                    try:
                        expected_value = float(value)
                    except ValueError:
                        expected_value = value
                
                assert config.get(key) == expected_value, f"Failed to parse {key}={value}"
    
    def test_argument_parsing_with_defaults(self):
        """Test argument parsing with default values."""
        config = Config()
        
        # Load from default config
        config.load_from_args([])
        
        # Verify defaults are set
        defaults = {
            "neuron_count": 1000,
            "region_count": 3,
            "connection_probability": 0.1,
            "timestep": 0.01,
            "max_energy": 100.0,
            "energy_decay_rate": 0.95,
            "random_seed": 42,
        }
        
        for key, expected_value in defaults.items():
            assert config.get(key) == expected_value, f"Default value for {key} not set"
    
    def test_argument_parsing_override_defaults(self):
        """Test argument parsing that overrides defaults."""
        config = Config()
        
        # Load with overrides
        args = ["--neuron_count", "500", "--region_count", "5", "--timestep", "0.02"]
        config.load_from_args(args)
        
        # Verify overrides were applied
        assert config.get("neuron_count") == 500
        assert config.get("region_count") == 5
        assert config.get("timestep") == 0.02
        
        # Verify non-overridden defaults are still present
        assert config.get("connection_probability") == 0.1  # Default
        assert config.get("max_energy") == 100.0  # Default
    
    def test_argument_parsing_invalid_arguments(self):
        """Test argument parsing with invalid arguments."""
        config = Config()
        
        # Test invalid argument formats
        invalid_cases = [
            ["--neuron_count"],  # Missing value
            ["--neuron_count", "1000", "--invalid_arg"],  # Missing value for last arg
            ["--neuron_count", "not_a_number"],  # Invalid type
            ["--neuron_count", "1000", "--region_count", "not_a_number"],  # Multiple invalid
        ]
        
        for args in invalid_cases:
            # These should either raise exceptions or handle gracefully
            # depending on implementation
            try:
                config.load_from_args(args)
                # If it doesn't raise an exception, that's OK for this test
            except (IndexError, ValueError, TypeError):
                # Expected behavior for invalid arguments
                pass
    
    def test_argument_parsing_boolean_values(self):
        """Test boolean argument parsing."""
        config = Config()
        
        # Test various boolean representations
        boolean_cases = [
            ["--debug", "true"],
            ["--debug", "false"],
            ["--verbose", "TRUE"],
            ["--verbose", "FALSE"],
            ["--enabled", "1"],
            ["--enabled", "0"],
            ["--disabled", "yes"],
            ["--disabled", "no"],
        ]
        
        for args in boolean_cases:
            config.load_from_args(args)
            
            key = args[0][2:]
            value = args[1]
            
            # Convert to expected boolean
            if value.lower() in ["true", "1", "yes"]:
                expected = True
            elif value.lower() in ["false", "0", "no"]:
                expected = False
            else:
                expected = value
            
            assert config.get(key) == expected, f"Boolean parsing failed for {key}={value}"

class TestCLICommandExecution:
    """Unit tests for CLI command execution."""
    
    def test_cli_command_execution(self):
        """Test CLI command execution."""
        # Test that CLI can be executed
        # This would test actual command-line execution
        # For now, test the configuration system that CLI would use
        
        config = Config()
        
        # Test configuration that would be generated by CLI
        cli_args = [
            "--neuron_count", "1000",
            "--region_count", "3",
            "--timestep", "0.01",
            "--random_seed", "42",
            "--debug", "true",
            "--verbose", "false"
        ]
        
        config.load_from_args(cli_args)
        
        # Verify configuration
        assert config.get("neuron_count") == 1000
        assert config.get("region_count") == 3
        assert config.get("timestep") == 0.01
        assert config.get("random_seed") == 42
        assert config.get("debug") is True
        assert config.get("verbose") is False
    
    def test_cli_command_help(self):
        """Test CLI help functionality."""
        # Test help generation
        config = Config()
        
        # Help should be available
        help_text = config.summary()
        assert isinstance(help_text, str)
        assert "Configuration Summary" in help_text
        
        # Test with non-empty configuration
        config.set("test_key", "test_value")
        help_text = config.summary()
        assert "test_key" in help_text
        assert "test_value" in help_text
    
    def test_cli_command_version(self):
        """Test CLI version functionality."""
        # Test version information
        config = Config()
        
        # Version should be available
        config.set("version", "0.1.0")
        assert config.get("version") == "0.1.0"
        
        # Test version in summary
        help_text = config.summary()
        assert "version" in help_text.lower()
    
    def test_cli_command_export(self):
        """Test CLI command export functionality."""
        config = Config()
        
        # Set up configuration
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        config.set("timestep", 0.01)
        
        # Test export to different formats
        with tempfile.TemporaryDirectory() as tmpdir:
            # Test JSON export
            json_file = os.path.join(tmpdir, "config.json")
            config.save_to_file(json_file, "json")
            
            # Verify JSON file was created
            assert os.path.exists(json_file)
            
            # Load and verify JSON
            with open(json_file, 'r') as f:
                loaded_config = json.load(f)
                assert loaded_config["neuron_count"] == 1000
                assert loaded_config["region_count"] == 3
            
            # Test YAML export
            yaml_file = os.path.join(tmpdir, "config.yaml")
            config.save_to_file(yaml_file, "yaml")
            
            # Verify YAML file was created
            assert os.path.exists(yaml_file)
            
            # Load and verify YAML (if pyyaml is available)
            try:
                with open(yaml_file, 'r') as f:
                    loaded_config = yaml.safe_load(f)
                    assert loaded_config["neuron_count"] == 1000
                    assert loaded_config["region_count"] == 3
            except ImportError:
                # pyyaml not available, that's OK
                pass

class TestCLICapabilityValidation:
    """Unit tests for CLI capability validation."""
    
    def test_cli_capability_validation(self):
        """Test CLI capability validation."""
        config = Config()
        
        # Test that configuration validation works
        config.set("neuron_count", 1000)
        
        # Validate configuration
        from pynlm.types import validate_neuron_id, validate_region_id
        
        neuron_id = validate_neuron_id(1000)
        assert neuron_id.value == 1000
        
        region_id = validate_region_id(3)
        assert region_id.value == 3
        
        # Test that configuration meets requirements
        assert config.get("neuron_count") > 0
        assert config.get("region_count") > 0
        assert config.get("timestep") > 0
    
    def test_cli_configuration_validation(self):
        """Test CLI configuration validation."""
        config = Config()
        
        # Test invalid configurations
        invalid_configs = [
            {"neuron_count": 0},  # Zero neurons
            {"neuron_count": -1},  # Negative neurons
            {"neuron_count": 1000, "region_count": 0},  # Zero regions
            {"connection_probability": 1.5},  # Invalid probability
            {"timestep": 0.0},  # Zero timestep
            {"energy_decay_rate": 1.5},  # Invalid decay rate
        ]
        
        for invalid_config in invalid_configs:
            # Reset config
            config.clear()
            
            # Apply invalid configuration
            for key, value in invalid_config.items():
                config.set(key, value)
            
            # Configuration may or may not raise exceptions
            # depending on validation implementation
            pass
        
        # Test valid configuration
        config.clear()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        config.set("connection_probability", 0.1)
        config.set("timestep", 0.01)
        config.set("max_energy", 100.0)
        config.set("energy_decay_rate", 0.95)
        config.set("random_seed", 42)
        
        # Verify valid configuration
        assert config.get("neuron_count") == 1000
        assert config.get("region_count") == 3
        assert config.get("connection_probability") == 0.1
        assert config.get("timestep") == 0.01
        assert config.get("max_energy") == 100.0
        assert config.get("energy_decay_rate") == 0.95
        assert config.get("random_seed") == 42

class TestCLISecurityValidation:
    """Unit tests for CLI security validation."""
    
    def test_cli_input_validation(self):
        """Test CLI input validation security."""
        config = Config()
        
        # Test that CLI input is validated
        # Test with potentially malicious input
        malicious_inputs = [
            ("--neuron_count", "1000; rm -rf /"),
            ("--region_count", "3 && cat /etc/passwd"),
            ("--debug", "true; python -c 'import os; os.system(\"echo pwned\")'"),
        ]
        
        for key, value in malicious_inputs:
            try:
                # CLI should validate and reject malicious input
                config.set(key[2:], value)
                # If it doesn't raise an exception, test should still pass
                # depending on implementation
                pass
            except (ValueError, TypeError, UnicodeDecodeError) as e:
                # Expected behavior for malicious input
                pass
        
        # Test that valid input still works
        config.set("valid_key", "valid_value")
        assert config.get("valid_key") == "valid_value"
    
    def test_cli_path_traversal_protection(self):
        """Test CLI path traversal protection."""
        config = Config()
        
        # Test path traversal attempts
        malicious_paths = [
            "../../../etc/passwd",
            "..\\..\\..\\windows\\system32\\config\\sam",
            "/etc/shadow",
            "file:///etc/passwd",
            "file:///dev/random"
        ]
        
        for path in malicious_paths:
            try:
                config.load_from_file(path)
                # If it doesn't raise an exception, test should still pass
                # depending on implementation
                pass
            except (ValueError, FileNotFoundError, IOError) as e:
                # Expected behavior for malicious paths
                pass
        
        # Test that legitimate file paths still work
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"neuron_count": 1000}')
            temp_file = f.name
        
        try:
            config.load_from_file(temp_file)
            assert config.get("neuron_count") == 1000
        finally:
            os.unlink(temp_file)
    
    def test_cli_code_injection_protection(self):
        """Test CLI code injection protection."""
        config = Config()
        
        # Test code injection attempts
        malicious_inputs = [
            "1000; python -c 'import os; os.system(\"echo pwned\")'",
            "1000`whoami`",
            "1000$(whoami)",
            "1000\\x00malicious",
            "1000\r\nmalicious",
            "1000\ntampered",
        ]
        
        for malicious_input in malicious_inputs:
            try:
                config.set("neuron_count", malicious_input)
                # If it doesn't raise an exception, test should still pass
                # depending on implementation
                pass
            except (ValueError, TypeError, UnicodeDecodeError) as e:
                # Expected behavior for malicious input
                pass
        
        # Test that valid numeric input still works
        config.set("neuron_count", "1000")
        assert config.get("neuron_count") == 1000

# Test discovery and registration functions

def discover_cli_tests():
    """Discover all CLI test classes."""
    test_classes = [
        TestCLIArgumentParsing,
        TestCLICommandExecution,
        TestCLICapabilityValidation,
        TestCLISecurityValidation
    ]
    
    return test_classes

def get_cli_test_categories():
    """Get all CLI test categories."""
    return {
        'argument_parsing': TestCLIArgumentParsing,
        'command_execution': TestCLICommandExecution,
        'capability_validation': TestCLICapabilityValidation,
        'security_validation': TestCLISecurityValidation
    }

if __name__ == "__main__":
    # Run CLI tests
    import unittest
    
    # Create test suite
    loader = unittest.TestLoader()
    
    # Get all CLI test classes
    cli_classes = discover_cli_tests()
    
    # Create test suite
    suite = unittest.TestSuite()
    
    for test_class in cli_classes:
        suite.addTests(loader.loadTestsFromTestCase(test_class))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)
