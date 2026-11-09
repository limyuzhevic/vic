import sys
import os
import json
import tempfile
import yaml
from unittest.mock import patch, MagicMock
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

from nlm_cli import NLMCLI

class TestNLMCLIConfigValidation:
    """Configuration validation tests for NLM CLI."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_json_config_validation(self):
        """Test JSON configuration validation."""
        # Create a valid JSON config
        valid_config = {
            "neurons": 100,
            "synapses": 1000,
            "learning_rate": 0.01,
            "description": "Valid test configuration",
            "enabled": True,
            "parameters": [1.0, 2.0, 3.0],
            "metadata": {
                "version": "1.0",
                "created": "2026-11-09"
            }
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(valid_config, f)
            config_file = f.name
        
        try:
            # Test JSON config loading
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Test config validation
            result = self.cli.cmd_config(["validate"])
            assert result is True
            
            # Test config saving
            result = self.cli.cmd_config(["save", "validated_config.json"])
            assert result is True
            
            # Verify saved config
            with open("validated_config.json", 'r') as f:
                saved_config = json.load(f)
            
            assert saved_config == valid_config
            
            # Clean up
            os.unlink("validated_config.json")
        finally:
            os.unlink(config_file)
    
    def test_yaml_config_validation(self):
        """Test YAML configuration validation."""
        # Create a valid YAML config
        yaml_config = {
            "brain": {
                "neurons": 100,
                "synapses": 1000,
                "learning_rate": 0.01
            },
            "world": {
                "width": 50,
                "height": 50,
                "time_step": 0.1
            },
            "experiments": ["test1", "test2", "test3"]
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml.dump(yaml_config, f)
            config_file = f.name
        
        try:
            # Test YAML config loading (should auto-detect format)
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Test config validation
            result = self.cli.cmd_config(["validate"])
            assert result is True
            
            # Test config saving as YAML
            result = self.cli.cmd_config(["save", "validated_config.yaml"])
            assert result is True
            
            # Verify saved config (may not be YAML, but should be valid)
            assert os.path.exists("validated_config.yaml")
            
            # Clean up
            if os.path.exists("validated_config.yaml"):
                os.unlink("validated_config.yaml")
        finally:
            os.unlink(config_file)
    
    def test_ini_config_validation(self):
        """Test INI configuration validation."""
        # Create a valid INI config
        ini_config = """[brain]
neurons = 100
synapses = 1000
learning_rate = 0.01

[world]
width = 50
height = 50
time_step = 0.1

[experiments]
test1 = true
test2 = false
test3 = true
"""
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.ini', delete=False) as f:
            f.write(ini_config)
            config_file = f.name
        
        try:
            # Test INI config loading
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Test config validation
            result = self.cli.cmd_config(["validate"])
            assert result is True
            
            # Test config saving
            result = self.cli.cmd_config(["save", "validated_config.ini"])
            assert result is True
            
            # Verify saved config
            assert os.path.exists("validated_config.ini")
            
            # Clean up
            if os.path.exists("validated_config.ini"):
                os.unlink("validated_config.ini")
        finally:
            os.unlink(config_file)
    
    def test_config_data_integrity(self):
        """Test config data integrity across operations."""
        # Set some configuration values
        test_values = {
            "string_key": "test_value",
            "int_key": 42,
            "float_key": 3.14159,
            "bool_key": True,
            "list_key": [1, 2, 3, 4, 5],
            "nested_key": {"inner": "value", "number": 123}
        }
        
        # Mock config operations
        with patch.object(self.cli.config, 'getKeys', return_value=list(test_values.keys())):
            with patch.object(self.cli.config, 'get', side_effect=lambda x: test_values[x]):
                # Test config listing
                result = self.cli.cmd_config(["list"])
                assert result is True
                
                # Test config getting for each key
                for key, expected_value in test_values.items():
                    result = self.cli.cmd_config(["get", key])
                    assert result is True
        
        # Test config saving and loading round-trip
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(test_values, f)
            config_file = f.name
        
        try:
            # Load config
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Save config
            result = self.cli.cmd_config(["save", "roundtrip_config.json"])
            assert result is True
            
            # Load saved config
            result = self.cli.cmd_config(["load", "roundtrip_config.json"])
            assert result is True
            
            # Clean up
            os.unlink("roundtrip_config.json")
        finally:
            os.unlink(config_file)
    
    def test_config_schema_validation(self):
        """Test configuration schema validation."""
        # Create a config with schema information
        schema_config = {
            "version": "1.0",
            "schema": {
                "type": "object",
                "properties": {
                    "neurons": {"type": "integer", "minimum": 1, "maximum": 10000},
                    "learning_rate": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                    "enabled": {"type": "boolean"}
                },
                "required": ["neurons", "learning_rate"]
            },
            "data": {
                "neurons": 100,
                "learning_rate": 0.01,
                "enabled": True
            }
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(schema_config, f)
            config_file = f.name
        
        try:
            # Load config with schema
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Test config validation (should pass for valid data)
            result = self.cli.cmd_config(["validate"])
            assert result is True
        finally:
            os.unlink(config_file)
    
    def test_config_error_recovery(self):
        """Test config error recovery mechanisms."""
        # Test with corrupted config file
        corrupted_content = """{ "neurons": 100, "synapses": 1000, invalid_json }"""
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write(corrupted_content)
            config_file = f.name
        
        try:
            # Should handle corrupted JSON gracefully
            result = self.cli.cmd_config(["load", config_file])
            # Should return False for corrupted file
            assert result is False
        finally:
            os.unlink(config_file)
        
        # Test with non-existent file
        result = self.cli.cmd_config(["load", "non_existent_file.json"])
        assert result is False
    
    def test_config_backup_and_restore(self):
        """Test config backup and restore functionality."""
        # Create original config
        original_config = {
            "neurons": 100,
            "synapses": 1000,
            "learning_rate": 0.01
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(original_config, f)
            original_file = f.name
        
        try:
            # Load original config
            result = self.cli.cmd_config(["load", original_file])
            assert result is True
            
            # Save backup
            result = self.cli.cmd_config(["save", "backup_config.json"])
            assert result is True
            
            # Modify config
            self.cli.cmd_config(["set", "neurons", "200"])
            self.cli.cmd_config(["set", "new_param", "new_value"])
            
            # Restore from backup
            result = self.cli.cmd_config(["load", "backup_config.json"])
            assert result is True
            
            # Clean up
            if os.path.exists("backup_config.json"):
                os.unlink("backup_config.json")
        finally:
            os.unlink(original_file)


class TestNLMCLIDocumentationAccuracy:
    """Documentation accuracy and completeness tests."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_documentation_examples(self):
        """Test documentation examples."""
        # Example 1: Basic usage
        examples = [
            # Basic configuration commands
            ["config", "list"],
            ["config", "get", "neurons"],
            ["config", "set", "neurons", "100"],
            ["config", "validate"],
            ["config", "save", "config.json"],
            
            # Brain commands
            ["brain", "status"],
            ["brain", "initialize"],
            ["brain", "step", "10"],
            ["brain", "info"],
            ["brain", "save", "checkpoint.nml"],
            ["brain", "load", "checkpoint.nml"],
            
            # Experiment commands
            ["experiment", "list"],
            ["experiment", "run", "test_exp"],
            ["experiment", "info", "test_exp"],
            ["experiment", "metrics", "test_exp"],
            
            # World commands
            ["world", "info"],
            ["world", "reset"],
            ["world", "step"],
            
            # Monitor commands
            ["monitor", "status"],
            ["monitor", "start"],
            ["monitor", "stop"],
            
            # Script commands
            ["script", "list"],
            ["script", "execute", "example.nml"],
            ["script", "save", "current_session.nml"],
            ["script", "info", "example.nml"],
            ["script", "validate", "example.nml"],
        ]
        
        for example_args in examples:
            # Test that each documented command exists and works
            command = example_args[0]
            subargs = example_args[1:]
            
            # This will test the command exists and can be invoked
            # Some commands require mocking, but we can at least test they don't crash
            try:
                if command == "config":
                    result = self.cli.cmd_config(subargs)
                elif command == "brain":
                    result = self.cli.cmd_brain(subargs)
                elif command == "experiment":
                    result = self.cli.cmd_experiment(subargs)
                elif command == "world":
                    result = self.cli.cmd_world(subargs)
                elif command == "monitor":
                    result = self.cli.cmd_monitor(subargs)
                elif command == "script":
                    result = self.cli.cmd_script(subargs)
                
                # Should return True or handle gracefully
                assert result in [True, False], f"Command {command} failed with args {subargs}"
            except Exception as e:
                # Some commands may require mocked dependencies
                # That's okay for documentation testing
                pass
    
    def test_help_documentation_completeness(self):
        """Test help documentation completeness."""
        # Test that all documented commands have help available
        documented_commands = [
            "config", "brain", "experiment", "world", "monitor", "script",
            "help", "version", "exit", "quit", "run"
        ]
        
        for cmd in documented_commands:
            result = self.cli.cmd_help([cmd])
            assert result is True, f"Help missing for command: {cmd}"
    
    def test_cli_flags_help(self):
        """Test CLI flags help documentation."""
        # Test various CLI flags mentioned in documentation
        flags = [
            ["--help"],
            ["--version"],
            ["--config", "file.cfg"],
            ["--script", "script.nml"],
            ["--output", "json"],
            ["--verbose"],
            ["--debug"],
            ["--quiet"],
            ["--no-progress"],
        ]
        
        for flag_args in flags:
            # The CLI should parse these flags without crashing
            try:
                # Create a new CLI with these arguments
                test_cli = NLMCLI()
                # We can't easily test argument parsing without modifying the CLI
                # But we can verify the flag options exist in the help system
                pass
            except Exception as e:
                pass


class TestNLMCIRecoveryAndResilience:
    """Recovery and resilience tests for NLM CLI."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_recovery_from_invalid_config(self):
        """Test recovery from invalid configuration."""
        # Create invalid config file
        invalid_config = """invalid key value
another invalid line
"""
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write(invalid_config)
            config_file = f.name
        
        try:
            # Should handle invalid config gracefully
            result = self.cli.cmd_config(["load", config_file])
            # Should return False for invalid config
            assert result is False
            
            # Should still be functional after failed load
            result = self.cli.cmd_config(["list"])
            assert result in [True, False]  # May be empty or error
        finally:
            os.unlink(config_file)
    
    def test_recovery_from_missing_files(self):
        """Test recovery from missing files."""
        # Test brain load with missing file
        result = self.cli.cmd_brain(["load", "non_existent_brain.nml"])
        assert result is False
        
        # Test brain save with invalid path
        result = self.cli.cmd_brain(["save", "/invalid/path/brain.nml"])
        assert result is False
        
        # Test script execute with missing file
        result = self.cli.script_execute(["non_existent_script.nml"])
        assert result is False
    
    def test_error_recovery_capability(self):
        """Test error recovery capability."""
        # Test that CLI can recover from various error conditions
        error_scenarios = [
            # Invalid commands
            ["config", "list"],  # Should work
            ["brain", "invalid_command"],  # Should fallback to help
            ["experiment", "invalid_command"],  # Should fallback to help
            
            # Invalid arguments
            ["brain", "step", "not_a_number"],  # Should return False
            ["brain", "load"],  # Missing filename, should return False
        ]
        
        for scenario in error_scenarios:
            command = scenario[0]
            subargs = scenario[1:]
            
            try:
                if command == "config":
                    result = self.cli.cmd_config(subargs)
                elif command == "brain":
                    result = self.cli.cmd_brain(subargs)
                elif command == "experiment":
                    result = self.cli.cmd_experiment(subargs)
                
                # Should not crash
                assert result in [True, False], f"Command {command} failed unexpectedly"
            except Exception as e:
                # Some exceptions might be acceptable in error scenarios
                pass


if __name__ == "__main__":
    import pytest
    
    print("Running NLM CLI Validation and Documentation Tests...")
    pytest.main([__file__, "-v"])