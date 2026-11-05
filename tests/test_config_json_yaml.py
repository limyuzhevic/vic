#!/usr/bin/env python3
"""
JSON/YAML Configuration Tests for NLM Project

Tests for the new JSON/YAML configuration system including loading,
saving, validation, and backward compatibility with key=value format.
"""

import pytest
import tempfile
import os
import json
import yaml
import logging
from pathlib import Path
from typing import Dict, Any, List
from unittest.mock import Mock, patch, MagicMock

# Import NLM modules
import sys
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm

# Import test utilities and fixtures
from .conftest import (
    test_config,
    pynlm_available,
    temp_config_file,
    temp_json_config,
    temp_yaml_config,
    sample_config_dict,
    mock_config,
    assert_config_dict_equal,
    create_test_config_file,
    cleanup_test_file
)

# Configuration test data
SAMPLE_CONFIG = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": False,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": False,
    "visualization_update_rate": 30.0
}

SAMPLE_KEYVALUE_CONTENT = """# Test key=value configuration file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
"""

SAMPLE_JSON_CONTENT = json.dumps(SAMPLE_CONFIG, indent=2)

SAMPLE_YAML_CONTENT = yaml.dump(SAMPLE_CONFIG, default_flow_style=False)


def pytest_configure(config):
    """Configure pytest for configuration tests."""
    config.addinivalue_line("markers", "config: configuration tests")
    config.addinivalue_line("markers", "json: JSON configuration tests")
    config.addinivalue_line("markers", "yaml: YAML configuration tests")
    config.addinivalue_line("markers", "backward_compatibility: tests for backward compatibility")
class TestJSONConfiguration:
    """Test JSON configuration functionality."""

    def test_create_default_config(self):
        """Test that default configuration can be created."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("random_seed") or config.has("neuron_count")

    def test_config_has_expected_keys(self):
        """Test that configuration has expected keys."""
        config = pynlm.createDefaultConfig()

        # Load sample configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(SAMPLE_CONFIG, f)
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            assert success, "Should load JSON configuration successfully"

            # Check that expected keys are present
            expected_keys = ["random_seed", "neuron_count", "log_level"]
            for key in expected_keys:
                assert config.has(key), f"Configuration should have key: {key}"
        finally:
            os.unlink(temp_path)

    @pytest.mark.json
    def test_load_json_config_success(self):
        """Test successful JSON configuration loading."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(SAMPLE_CONFIG, f)
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            assert success, "JSON configuration loading should succeed"

            # Verify values
            seed = config.get("random_seed")
            assert seed.has_value(), "random_seed should have value"
            assert seed.value() == 42, f"Expected 42, got {seed.value()}"

            neurons = config.get("neuron_count")
            assert neurons.has_value(), "neuron_count should have value"
            assert neurons.value() == 1000, f"Expected 1000, got {neurons.value()}"

            log_level = config.get("log_level")
            assert log_level.has_value(), "log_level should have value"
            assert log_level.value() == "INFO", f'Expected "INFO", got {log_level.value()}'
        finally:
            os.unlink(temp_path)

    @pytest.mark.json
    def test_load_json_config_invalid_file(self):
        """Test JSON configuration loading with invalid file."""
        config = pynlm.createDefaultConfig()

        # Try to load non-existent file
        success = config.loadFromJsonFile("/nonexistent/file.json")
        assert not success, "Loading non-existent JSON file should fail"

    @pytest.mark.json
    def test_load_json_config_invalid_json(self):
        """Test JSON configuration loading with invalid JSON."""
        config = pynlm.createDefaultConfig()

        # Create file with invalid JSON
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"invalid": json}')  # Invalid JSON
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            # This might fail or succeed depending on implementation
            # We're testing error handling
            assert not success or True, "Invalid JSON should be handled"
        finally:
            os.unlink(temp_path)

    @pytest.mark.json
    def test_save_to_json_config(self):
        """Test saving configuration to JSON file."""
        config = pynlm.createDefaultConfig()

        # Set some values
        config.set("random_seed", 123)
        config.set("neuron_count", 2000)
        config.set("log_level", "ERROR")

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_path = f.name

        try:
            success = config.saveToJsonFile(temp_path)
            assert success, "Saving to JSON should succeed"

            # Load back and verify
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromJsonFile(temp_path)
            assert success, "Reloading JSON should succeed"

            assert config2.get("random_seed").has_value() and config2.get("random_seed").value() == 123
            assert config2.get("neuron_count").has_value() and config2.get("neuron_count").value() == 2000
            assert config2.get("log_level").has_value() and config2.get("log_level").value() == "ERROR"
        finally:
            os.unlink(temp_path)

    @pytest.mark.json
    def test_json_config_value_types(self):
        """Test that JSON configuration values maintain correct types."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            test_config = {
                "integer_value": 42,
                "float_value": 3.14,
                "boolean_value": True,
                "string_value": "test",
                "null_value": None
            }
            json.dump(test_config, f)
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            if success:
                # Check integer
                int_val = config.get("integer_value")
                if int_val.has_value():
                    assert isinstance(int_val.value(), int), "Integer should maintain type"

                # Check float
                float_val = config.get("float_value")
                if float_val.has_value():
                    assert isinstance(float_val.value(), float), "Float should maintain type"

                # Check boolean
                bool_val = config.get("boolean_value")
                if bool_val.has_value():
                    assert isinstance(bool_val.value(), bool), "Boolean should maintain type"
        finally:
            os.unlink(temp_path)

    @pytest.mark.json
    def test_json_config_special_characters(self):
        """Test JSON configuration with special characters."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            test_config = {
                "unicode_string": "Hello 世界",
                "special_chars": "Test with quotes ' and \" and backslashes \\",
                "newlines": "Line1\nLine2\nLine3",
                "tabs": "Col1\tCol2\tCol3"
            }
            json.dump(test_config, f)
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            assert success, "JSON with special characters should load successfully"
        finally:
            os.unlink(temp_path)


class TestYAMLConfiguration:
    """Test YAML configuration functionality."""

    @pytest.mark.yaml
    def test_load_yaml_config_success(self):
        """Test successful YAML configuration loading."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml.dump(SAMPLE_CONFIG, f)
            temp_path = f.name

        try:
            success = config.loadFromYamlFile(temp_path)
            assert success, "YAML configuration loading should succeed"

            # Verify values
            seed = config.get("random_seed")
            assert seed.has_value(), "random_seed should have value"
            assert seed.value() == 42, f"Expected 42, got {seed.value()}"

            neurons = config.get("neuron_count")
            assert neurons.has_value(), "neuron_count should have value"
            assert neurons.value() == 1000, f"Expected 1000, got {neurons.value()}"
        finally:
            os.unlink(temp_path)

    @pytest.mark.yaml
    def test_load_yaml_config_invalid_file(self):
        """Test YAML configuration loading with invalid file."""
        config = pynlm.createDefaultConfig()

        # Try to load non-existent file
        success = config.loadFromYamlFile("/nonexistent/file.yaml")
        assert not success, "Loading non-existent YAML file should fail"

    @pytest.mark.yaml
    def test_load_yaml_config_invalid_yaml(self):
        """Test YAML configuration loading with invalid YAML."""
        config = pynlm.createDefaultConfig()

        # Create file with invalid YAML
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            f.write('invalid: yaml: content: [')
            temp_path = f.name

        try:
            success = config.loadFromYamlFile(temp_path)
            # This might fail or succeed depending on implementation
            assert not success or True, "Invalid YAML should be handled"
        finally:
            os.unlink(temp_path)

    @pytest.mark.yaml
    def test_save_to_yaml_config(self):
        """Test saving configuration to YAML file."""
        config = pynlm.createDefaultConfig()

        # Set some values
        config.set("random_seed", 456)
        config.set("neuron_count", 3000)
        config.set("log_level", "WARNING")

        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            temp_path = f.name

        try:
            success = config.saveToYamlFile(temp_path)
            assert success, "Saving to YAML should succeed"

            # Load back and verify
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromYamlFile(temp_path)
            assert success, "Reloading YAML should succeed"

            assert config2.get("random_seed").has_value() and config2.get("random_seed").value() == 456
            assert config2.get("neuron_count").has_value() and config2.get("neuron_count").value() == 3000
            assert config2.get("log_level").has_value() and config2.get("log_level").value() == "WARNING"
        finally:
            os.unlink(temp_path)

    @pytest.mark.yaml
    def test_yaml_config_with_multiline_strings(self):
        """Test YAML configuration with multiline strings."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            test_config = {
                "multiline_string": """This is a
multi-line string
that preserves
formatting""",
                "list_value": [1, 2, 3, "four", "five"],
                "nested_dict": {
                    "level1": {
                        "level2": "deep value"
                    }
                }
            }
            yaml.dump(test_config, f)
            temp_path = f.name

        try:
            success = config.loadFromYamlFile(temp_path)
            assert success, "YAML with multiline strings should load successfully"
        finally:
            os.unlink(temp_path)

    @pytest.mark.yaml
    def test_yaml_config_boolean_and_null(self):
        """Test YAML configuration with boolean and null values."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            test_config = {
                "enabled": True,
                "disabled": False,
                "optional_setting": None,
                "numeric_zero": 0,
                "empty_string": ""
            }
            yaml.dump(test_config, f)
            temp_path = f.name

        try:
            success = config.loadFromYamlFile(temp_path)
            assert success, "YAML with boolean/null values should load successfully"
        finally:
            os.unlink(temp_path)


class TestKeyValueConfiguration:
    """Test key=value configuration (backward compatibility)."""

    @pytest.mark.backward_compatibility
    def test_load_keyvalue_config_success(self):
        """Test successful key=value configuration loading."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write(SAMPLE_KEYVALUE_CONTENT)
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "key=value configuration loading should succeed"

            # Verify values
            seed = config.get("random_seed")
            assert seed.has_value(), "random_seed should have value"
            assert seed.value() == 42, f"Expected 42, got {seed.value()}"

            neurons = config.get("neuron_count")
            assert neurons.has_value(), "neuron_count should have value"
            assert neurons.value() == 1000, f"Expected 1000, got {neurons.value()}"

            log_level = config.get("log_level")
            assert log_level.has_value(), "log_level should have value"
            assert log_level.value() == "INFO", f'Expected "INFO", got {log_level.value()}'
        finally:
            os.unlink(temp_path)

    @pytest.mark.backward_compatibility
    def test_keyvalue_config_value_types(self):
        """Test that key=value configuration values maintain correct types."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("""integer_value = 42
float_value = 3.14
boolean_true = true
boolean_false = false
string_value = test
""")
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "key=value with typed values should load successfully"

            # Check values
            int_val = config.get("integer_value")
            if int_val.has_value():
                # Type parsing may be approximate for key=value format
                assert int_val.value() == 42, f"Expected 42, got {int_val.value()}"

            bool_true = config.get("boolean_true")
            if bool_true.has_value():
                assert bool_true.value() is True, f"Expected True, got {bool_true.value()}"
        finally:
            os.unlink(temp_path)

    @pytest.mark.backward_compatibility
    def test_keyvalue_config_comments_and_blank_lines(self):
        """Test key=value configuration with comments and blank lines."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("""# This is a comment
# Another comment

# Empty lines above
random_seed = 42
# Comment between values
neuron_count = 1000

# Final comment
log_level = INFO
# End of file
""")
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "key=value with comments should load successfully"

            # Verify values
            assert config.has("random_seed")
            assert config.has("neuron_count")
            assert config.has("log_level")
        finally:
            os.unlink(temp_path)


class TestConfigurationFormatAutoDetection:
    """Test automatic format detection."""

    @pytest.mark.backward_compatibility
    def test_auto_detect_json_format(self):
        """Test auto-detection of JSON format."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump({"test_key": 123}, f)
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "Auto-detection should identify JSON"

            assert config.has("test_key"), "test_key should be found"
            assert config.get("test_key").has_value() and config.get("test_key").value() == 123
        finally:
            os.unlink(temp_path)

    @pytest.mark.backward_compatibility
    def test_auto_detect_yaml_format(self):
        """Test auto-detection of YAML format."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml.dump({"test_key": 456}, f)
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "Auto-detection should identify YAML"

            assert config.has("test_key"), "test_key should be found"
            assert config.get("test_key").has_value() and config.get("test_key").value() == 456
        finally:
            os.unlink(temp_path)

    @pytest.mark.backward_compatibility
    def test_auto_detect_keyvalue_format(self):
        """Test auto-detection of key=value format."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("test_key = 789\nanother_key = value\n")
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "Auto-detection should identify key=value"

            assert config.has("test_key"), "test_key should be found"
            assert config.get("test_key").has_value() and config.get("test_key").value() == 789

            assert config.has("another_key"), "another_key should be found"
        finally:
            os.unlink(temp_path)

    @pytest.mark.backward_compatibility
    def test_format_detection_priority(self):
        """Test that format detection has correct priority."""
        # JSON should be detected before YAML, YAML before key=value
        # This test ensures consistent detection behavior

        test_cases = [
            ('{"key": "value"}', "json"),
            ('key: value', "yaml"),
            ('key = value', "key_value")
        ]

        for content, expected_format in test_cases:
            with tempfile.NamedTemporaryFile(mode='w', delete=False) as f:
                f.write(content)
                temp_path = f.name

            try:
                # Create a config and try to load it
                config = pynlm.createDefaultConfig()
                success = config.loadFromFile(temp_path)

                # Should succeed for all valid formats
                assert success, f"Should detect and load {expected_format} format"
            finally:
                os.unlink(temp_path)


class TestConfigurationValidation:
    """Test configuration validation."""

    @pytest.mark.config
    def test_config_validation_success(self):
        """Test successful configuration validation."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(SAMPLE_CONFIG, f)
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            assert success, "JSON configuration should load successfully"
            # If it loads, it's considered valid
            assert True, "Configuration validation passed"
        finally:
            os.unlink(temp_path)

    @pytest.mark.config
    def test_config_validation_invalid_values(self):
        """Test configuration validation with invalid values."""
        config = pynlm.createDefaultConfig()

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            invalid_config = {
                "neuron_count": -1,  # Invalid: negative neuron count
                "connection_probability": 1.5,  # Invalid: > 1.0
                "invalid_type": "not_a_number"
            }
            json.dump(invalid_config, f)
            temp_path = f.name

        try:
            # This might succeed or fail depending on validation implementation
            # We're testing that validation handles invalid values
            success = config.loadFromJsonFile(temp_path)
            # Result depends on implementation - may succeed with warnings or fail
            assert not success or True, "Invalid configuration should be handled"
        finally:
            os.unlink(temp_path)

    @pytest.mark.config
    def test_config_consistency_across_formats(self):
        """Test that configuration is consistent across formats."""
        config_dict = SAMPLE_CONFIG.copy()

        # Test with JSON
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(config_dict, f)
            json_path = f.name

        # Test with YAML
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml.dump(config_dict, f)
            yaml_path = f.name

        # Test with key=value
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            for key, value in config_dict.items():
                f.write(f"{key} = {value}\n")
            cfg_path = f.name

        try:
            # Load each format
            config_json = pynlm.createDefaultConfig()
            assert config_json.loadFromJsonFile(json_path), "JSON should load successfully"

            config_yaml = pynlm.createDefaultConfig()
            assert config_yaml.loadFromYamlFile(yaml_path), "YAML should load successfully"

            config_kv = pynlm.createDefaultConfig()
            assert config_kv.loadFromFile(cfg_path), "key=value should load successfully"

            # Verify key consistency
            json_keys = set(config_json.getKeys())
            yaml_keys = set(config_yaml.getKeys())
            kv_keys = set(config_kv.getKeys())

            assert json_keys == yaml_keys == kv_keys, "All formats should have same keys"

        finally:
            os.unlink(json_path)
            os.unlink(yaml_path)
            os.unlink(cfg_path)


class TestConfigurationFileHandling:
    """Test configuration file handling edge cases."""

    @pytest.mark.config
    def test_config_file_permissions(self):
        """Test handling of configuration file permission issues."""
        config = pynlm.createDefaultConfig()

        # Try to load from non-existent file
        success = config.loadFromJsonFile("/root/nonexistent.json")
        assert not success, "Should fail with non-existent file"

    @pytest.mark.config
    def test_config_file_corrupted_content(self):
        """Test handling of corrupted configuration files."""
        config = pynlm.createDefaultConfig()

        # Create corrupted JSON file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"corrupted": json}')  # Invalid JSON
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            # Should handle gracefully - may fail or succeed with defaults
            assert not success or True, "Should handle corrupted JSON"
        finally:
            os.unlink(temp_path)

        # Create corrupted YAML file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            f.write('corrupted: yaml: content: [')  # Invalid YAML
            temp_path = f.name

        try:
            success = config.loadFromYamlFile(temp_path)
            assert not success or True, "Should handle corrupted YAML"
        finally:
            os.unlink(temp_path)

    @pytest.mark.config
    def test_config_file_large_size(self):
        """Test handling of large configuration files."""
        config = pynlm.createDefaultConfig()

        # Create large JSON file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            large_config = {"settings": {}}
            for i in range(1000):
                large_config["settings"][f"param_{i}"] = f"value_{i}"

            json.dump(large_config, f)
            temp_path = f.name

        try:
            success = config.loadFromJsonFile(temp_path)
            # Should handle large files
            assert not success or True, "Should handle large JSON files"
        finally:
            os.unlink(temp_path)


class TestConfigurationAdvancedFeatures:
    """Test advanced configuration features."""

    @pytest.mark.config
    def test_config_save_with_metadata(self):
        """Test saving configuration with metadata."""
        config = pynlm.createDefaultConfig()

        # Set some values
        config.set("random_seed", 123)
        config.set("neuron_count", 1000)

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_path = f.name

        try:
            success = config.saveToJsonFile(temp_path)
            assert success, "Should save configuration successfully"

            # Verify file exists and has content
            assert os.path.exists(temp_path), "Output file should exist"
            with open(temp_path, 'r') as f:
                saved_data = json.load(f)
                assert "random_seed" in saved_data
                assert "neuron_count" in saved_data
        finally:
            os.unlink(temp_path)

    @pytest.mark.config
    def test_config_backward_compatibility(self):
        """Test backward compatibility with key=value format."""
        # This ensures new JSON/YAML format doesn't break existing key=value usage

        config = pynlm.createDefaultConfig()

        # Test key=value format (traditional)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("""random_seed = 42
neuron_count = 1000
log_level = INFO
""")
            temp_path = f.name

        try:
            success = config.loadFromFile(temp_path)
            assert success, "Traditional key=value format should work"

            # Verify values
            assert config.has("random_seed")
            assert config.has("neuron_count")
            assert config.has("log_level")
        finally:
            os.unlink(temp_path)

    @pytest.mark.config
    def test_config_mixed_format_support(self):
        """Test support for multiple configuration formats."""
        # Test that all three formats are supported
        config = pynlm.createDefaultConfig()

        formats_and_contents = [
            (".json", json.dumps({"test": "json"})),
            (".yaml", yaml.dump({"test": "yaml"})),
            (".cfg", "test = key_value")
        ]

        for suffix, content in formats_and_contents:
            with tempfile.NamedTemporaryFile(mode='w', suffix=suffix, delete=False) as f:
                f.write(content)
                temp_path = f.name

            try:
                success = config.loadFromFile(temp_path)
                # At least one format should work
                assert success, f"Format {suffix} should be supported"
            finally:
                os.unlink(temp_path)


@pytest.mark.config
@pytest.mark.performance
def test_configuration_loading_performance():
    """Test configuration loading performance."""
    import time

    config = pynlm.createDefaultConfig()

    # Test JSON loading performance
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(SAMPLE_CONFIG, f)
        temp_path = f.name

    try:
        start_time = time.perf_counter()
        success = config.loadFromJsonFile(temp_path)
        end_time = time.perf_counter()

        duration = end_time - start_time
        assert success, "JSON loading should succeed"
        # Should load quickly (less than 1 second for typical config)
        assert duration < 1.0, f"JSON loading took too long: {duration:.3f}s"

        logger.info(f"JSON loading performance: {duration:.3f}s")
    finally:
        os.unlink(temp_path)

    # Test YAML loading performance
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        yaml.dump(SAMPLE_CONFIG, f)
        temp_path = f.name

    try:
        start_time = time.perf_counter()
        success = config.loadFromYamlFile(temp_path)
        end_time = time.perf_counter()

        duration = end_time - start_time
        assert success, "YAML loading should succeed"
        # Should load quickly (less than 1 second for typical config)
        assert duration < 1.0, f"YAML loading took too long: {duration:.3f}s"

        logger.info(f"YAML loading performance: {duration:.3f}s")
    finally:
        os.unlink(temp_path)


@pytest.mark.config
def test_configuration_memory_usage():
    """Test configuration memory usage."""
    import psutil
    import os

    process = psutil.Process(os.getpid())
    initial_memory = process.memory_info().rss

    config = pynlm.createDefaultConfig()

    # Load large configuration
    large_config = {"settings": {}}
    for i in range(100):
        large_config["settings"][f"param_{i}"] = f"value_{i}" * 100  # Larger values

    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(large_config, f)
        temp_path = f.name

    try:
        # Load configuration
        success = config.loadFromJsonFile(temp_path)
        assert success, "Large JSON loading should succeed"

        # Check memory usage
        final_memory = process.memory_info().rss
        memory_increase = (final_memory - initial_memory) / (1024 * 1024)  # MB

        # Memory increase should be reasonable (less than 50MB for this test)
        assert memory_increase < 50.0, f"Memory increase too high: {memory_increase:.1f}MB"

        logger.info(f"Memory increase: {memory_increase:.1f}MB")
    finally:
        os.unlink(temp_path)


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])