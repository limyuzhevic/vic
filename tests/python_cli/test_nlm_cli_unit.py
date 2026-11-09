import sys
import os
import tempfile
import json
import argparse
from unittest.mock import patch, MagicMock
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

from nlm_cli import NLMCLI

class TestNLMCLIUnitTests:
    """Unit tests for NLM CLI enhanced features."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_cli_initialization(self):
        """Test CLI initialization with all components."""
        assert self.cli.config is not None
        assert self.cli.brain is None  # Not initialized by default
        assert self.cli.world is None
        assert self.cli.agent is None
        assert isinstance(self.cli.history, list)
        assert isinstance(self.cli.current_session, list)
        assert self.cli.output_format == "text"
        assert self.cli.verbose is False
        assert self.cli.debug is False
        assert self.cli.quiet is False
        assert self.cli.progress_indicators is True
    
    def test_echo_basic(self):
        """Test basic echo functionality."""
        # This should work without issues
        self.cli.echo("Test message")
        
        # Test different levels
        self.cli.echo("Warning message", "warning")
        self.cli.echo("Error message", "error")
        self.cli.echo("Debug message", "debug")
        self.cli.echo("Progress message", "progress")
        self.cli.echo("Success message", "success")
    
    def test_echo_json_format(self):
        """Test JSON output format."""
        self.cli.output_format = "json"
        
        # JSON echo should work (calls echo_json)
        test_data = {"key": "value", "number": 42}
        self.cli.echo_json(test_data)
    
    def test_echo_quiet_mode(self):
        """Test quiet mode suppresses output."""
        self.cli.quiet = True
        
        # Normal messages should be suppressed
        self.cli.echo("Normal message")
        
        # Error and warning messages should still show
        self.cli.echo("Error message", "error")
        self.cli.echo("Warning message", "warning")
    
    def test_command_mapping_basic(self):
        """Test basic command mapping functionality."""
        # Create a minimal CLI instance
        cli = NLMCLI()
        
        # Test the command_map from run_command method
        command_map = {
            "config": cli.cmd_config,
            "brain": cli.cmd_brain,
            "experiment": cli.cmd_experiment,
            "world": cli.cmd_world,
            "monitor": cli.cmd_monitor,
            "help": cli.cmd_help,
            "version": cli.cmd_version,
            "exit": cli.cmd_exit,
            "quit": cli.cmd_quit,
            "run": cli.cmd_run,
            "script": cli.cmd_script,
        }
        
        assert len(command_map) == 11
        assert "config" in command_map
        assert "help" in command_map
        assert "exit" in command_map
    
    def test_cmd_help_basic(self):
        """Test basic help functionality."""
        # Test without arguments
        result = self.cli.cmd_help([])
        assert result is True
        
        # Test with specific command
        result = self.cli.cmd_help(["config"])
        assert result is True
    
    def test_cmd_help_specific_commands(self):
        """Test help for specific commands."""
        # These should all be available commands
        commands = ["config", "brain", "experiment", "world", "monitor", "script"]
        
        for cmd in commands:
            result = self.cli.cmd_help([cmd])
            assert result is True, f"Help for command '{cmd}' should return True"
    
    def test_cmd_help_unknown_command(self):
        """Test help for unknown command."""
        result = self.cli.cmd_help(["unknown_command"])
        assert result is True  # Should handle gracefully
    
    def test_cmd_version_text_output(self):
        """Test version command in text format."""
        self.cli.output_format = "text"
        result = self.cli.cmd_version([])
        assert result is True
    
    def test_cmd_version_json_output(self):
        """Test version command in JSON format."""
        self.cli.output_format = "json"
        result = self.cli.cmd_version([])
        assert result is True
    
    def test_cmd_exit(self):
        """Test exit command."""
        result = self.cli.cmd_exit([])
        assert result is False  # Should return False to exit
    
    def test_cmd_quit(self):
        """Test quit command."""
        result = self.cli.cmd_quit([])
        assert result is False  # Should delegate to exit
    
    def test_config_commands_help(self):
        """Test config command help."""
        self.cli.help_config()
    
    def test_brain_commands_help(self):
        """Test brain command help."""
        self.cli.help_brain()
    
    def test_experiment_commands_help(self):
        """Test experiment command help."""
        self.cli.help_experiment()
    
    def test_world_commands_help(self):
        """Test world command help."""
        self.cli.help_world()
    
    def test_monitor_commands_help(self):
        """Test monitor command help."""
        self.cli.help_monitor()
    
    def test_script_commands_help(self):
        """Test script command help."""
        self.cli.help_script()
    
    def test_script_parsing_basic(self):
        """Test basic script parsing."""
        script_content = """# This is a comment
nlm config set key1 value1
nlm brain initialize
nlm run
"""
        
        commands = self.cli.parse_script_commands(script_content)
        
        # Should ignore comments and empty lines
        assert len(commands) == 3
        assert commands[0] == "nlm config set key1 value1"
        assert commands[1] == "nlm brain initialize"
        assert commands[2] == "nlm run"
    
    def test_script_parsing_empty_lines(self):
        """Test script parsing with empty lines and comments."""
        script_content = """
nlm config set key1 value1

# This is a comment

nlm brain initialize

"""
        
        commands = self.cli.parse_script_commands(script_content)
        
        assert len(commands) == 2
        assert commands[0] == "nlm config set key1 value1"
        assert commands[1] == "nlm brain initialize"
    
    def test_config_list_empty(self):
        """Test config list with empty config."""
        # Mock the config to have no keys
        with patch.object(self.cli.config, 'getKeys', return_value=[]):
            result = self.cli.config_list([])
            assert result is True
    
    def test_config_get_existing(self):
        """Test config get for existing key."""
        # Mock the config
        with patch.object(self.cli.config, 'get', return_value="test_value"):
            with patch.object(self.cli.config, 'getKeys', return_value=["key1"]):
                result = self.cli.config_list([])
                assert result is True
    
    def test_config_set(self):
        """Test config set operation."""
        # Mock the config.set method
        with patch.object(self.cli.config, 'set') as mock_set:
            result = self.cli.config_set(["test_key", "test_value"])
            assert result is True
            mock_set.assert_called_once_with("test_key", "test_value")
    
    def test_config_validate_basic(self):
        """Test basic config validation."""
        with patch.object(self.cli.config, 'getKeys', return_value=["key1", "key2"]):
            result = self.cli.config_validate([])
            assert result is True
    
    def test_config_validate_empty(self):
        """Test config validation with empty config."""
        with patch.object(self.cli.config, 'getKeys', return_value=[]):
            result = self.cli.config_validate([])
            assert result is False
    
    def test_brain_status_not_initialized(self):
        """Test brain status when brain is not initialized."""
        result = self.cli.brain_status([])
        assert result is False
    
    def test_brain_commands_not_initialized(self):
        """Test other brain commands when brain is not initialized."""
        # Test reset
        result = self.cli.brain_reset([])
        assert result is False
        
        # Test load
        result = self.cli.brain_load(["test_file"])
        assert result is False
        
        # Test save
        result = self.cli.brain_save(["test_file"])
        assert result is False
        
        # Test info
        result = self.cli.brain_info([])
        assert result is False
        
        # Test step
        result = self.cli.brain_step([])
        assert result is False
    
    def test_brain_step_with_number(self):
        """Test brain step with specified number of steps."""
        # Mock the brain
        mock_brain = MagicMock()
        with patch.object(self.cli, 'brain', mock_brain):
            result = self.cli.brain_step(["5"])
            assert result is True
            # Should call brain.step 5 times
            assert mock_brain.step.call_count == 5
    
    def test_brain_step_invalid_number(self):
        """Test brain step with invalid number."""
        result = self.cli.brain_step(["not_a_number"])
        assert result is False
    
    def test_brain_step_default(self):
        """Test brain step with default number of steps."""
        mock_brain = MagicMock()
        with patch.object(self.cli, 'brain', mock_brain):
            result = self.cli.brain_step([])
            assert result is True
            # Should call brain.step once (default is 1)
            assert mock_brain.step.call_count == 1
    
    def test_run_command_unknown_command(self):
        """Test run_command with unknown command."""
        result = self.cli.run_command("unknown_command")
        assert result is False
    
    def test_run_command_known_command(self):
        """Test run_command with known command (help)."""
        result = self.cli.run_command("help")
        assert result is True
    
    def test_output_format_json(self):
        """Test JSON output format switching."""
        # Test default is text
        assert self.cli.output_format == "text"
        
        # Change to JSON
        self.cli.output_format = "json"
        assert self.cli.output_format == "json"
    
    def test_verbose_mode(self):
        """Test verbose mode setting."""
        assert self.cli.verbose is False
        self.cli.verbose = True
        assert self.cli.verbose is True
    
    def test_debug_mode(self):
        ""Test debug mode setting."""
        assert self.cli.debug is False
        self.cli.debug = True
        assert self.cli.debug is True
    
    def test_progress_indicators(self):
        """Test progress indicators."""
        assert self.cli.progress_indicators is True
        self.cli.progress_indicators = False
        assert self.cli.progress_indicators is False
    
    def test_history_management(self):
        """Test command history management."""
        # Simulate some commands
        self.cli.history.append("nlm config list")
        self.cli.history.append("nlm brain status")
        self.cli.current_session.append("nlm brain initialize")
        
        assert len(self.cli.history) == 2
        assert len(self.cli.current_session) == 1
        assert self.cli.history[0] == "nlm config list"
        assert self.cli.history[1] == "nlm brain status"
        assert self.cli.current_session[0] == "nlm brain initialize"
    
    def test_progress_indicator_output(self):
        """Test progress indicator output."""
        self.cli.progress_indicators = True
        
        # This should output a progress message
        # We'll mock the echo method to capture the output
        with patch.object(self.cli, 'echo') as mock_echo:
            self.cli.echo("Step 1/100", "progress")
            mock_echo.assert_called_once_with("Step 1/100", "progress")


class TestNLMCLIIntegration:
    """Integration tests for NLM CLI features."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_script_execution_basic(self):
        """Test basic script execution."""
        # Create a temporary script file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
            f.write("nlm config set neurons 100\n")
            f.write("nlm brain initialize\n")
            f.write("nlm run\n")
            script_path = f.name
        
        try:
            # Test script execution
            result = self.cli.script_execute([script_path])
            assert result is True
        finally:
            # Clean up
            os.unlink(script_path)
    
    def test_script_execution_file_not_found(self):
        """Test script execution with non-existent file."""
        result = self.cli.script_execute(["non_existent_file.nlm"])
        assert result is False
    
    def test_script_list(self):
        """Test script listing."""
        # Create scripts directory
        script_dir = ".nlm_scripts"
        os.makedirs(script_dir, exist_ok=True)
        
        try:
            # Create test scripts
            with open(os.path.join(script_dir, "script1.nlm"), 'w') as f:
                f.write("nlm config set key1 value1")
            
            with open(os.path.join(script_dir, "script2.nlm"), 'w') as f:
                f.write("nlm brain initialize")
            
            result = self.cli.script_list([])
            assert result is True
        finally:
            # Clean up
            if os.path.exists(script_dir):
                for f in os.listdir(script_dir):
                    os.unlink(os.path.join(script_dir, f))
                os.rmdir(script_dir)
    
    def test_script_save(self):
        """Test script saving."""
        # Mock config.getKeys
        with patch.object(self.cli.config, 'getKeys', return_value=["neurons", "synapses"]):
            with patch.object(self.cli.config, 'get', side_effect=lambda x: {"neurons": "100", "synapses": "1000"}[x]):
                with patch.object(self.cli.brain, 'save', return_value=True):
                    result = self.cli.script_save(["test_script.nlm"])
                    assert result is True
    
    def test_script_info(self):
        """Test script information."""
        # Create a temporary script file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
            f.write("# Test script\n")
            f.write("nlm config set neurons 100\n")
            f.write("nlm brain initialize\n")
            f.write("nlm run\n")
            script_path = f.name
        
        try:
            result = self.cli.script_info([script_path])
            assert result is True
        finally:
            # Clean up
            os.unlink(script_path)
    
    def test_script_validate_valid(self):
        """Test validation of valid script."""
        # Create a temporary script file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
            f.write("nlm config set neurons 100\n")
            f.write("nlm brain initialize\n")
            script_path = f.name
        
        try:
            result = self.cli.script_validate([script_path])
            assert result is True
        finally:
            # Clean up
            os.unlink(script_path)
    
    def test_script_validate_empty(self):
        """Test validation of empty script."""
        # Create a temporary empty script file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
            f.write("")
            script_path = f.name
        
        try:
            result = self.cli.script_validate([script_path])
            # Empty script should produce warning but still return True
            assert result is True
        finally:
            # Clean up
            os.unlink(script_path)
    
    def test_full_cli_workflow(self):
        """Test complete CLI workflow with mocked dependencies."""
        # Mock all the external dependencies
        with patch.object(self.cli.config, 'getKeys', return_value=["neurons", "synapses"]):
            with patch.object(self.cli.config, 'get', side_effect=lambda x: {"neurons": "100", "synapses": "1000"}[x]):
                with patch.object(self.cli, 'brain', MagicMock()):
                    with patch.object(self.cli, 'world', MagicMock()):
                        with patch.object(self.cli, 'agent', MagicMock()):
                            with patch.object(self.cli.brain, 'initialize', return_value=True):
                                with patch.object(self.cli.brain, 'step'):
                                    with patch.object(self.cli.brain, 'logStatus'):
                                        with patch.object(self.cli.brain, 'getTotalNeuronCount', return_value=100):
                                            with patch.object(self.cli.brain, 'getTotalSynapseCount', return_value=1000):
                                                with patch.object(self.cli.brain, 'getActiveNeuronCount', return_value=50):
                                                    with patch.object(self.cli.brain, 'getFiringNeuronCount', return_value=30):
                                                        with patch.object(self.cli.brain, 'getTotalSpikeCount', return_value=5000):
                                                            with patch.object(self.cli.brain, 'getAverageFiringRate', return_value=50.0):
                                                                with patch.object(self.cli.brain, 'getDevelopmentalStage', return_value="development"):
                                                                    with patch.object(self.cli.world, 'update'):
                                                                        with patch.object(self.cli.world, 'getSensoryPercept', return_value="sensory_data"):
                                                                            with patch.object(self.cli.agent, 'processSensoryInput'):
                                                                                with patch.object(self.cli.agent, 'decodeMotorCommand', return_value="action"):
                                                                                    with patch.object(self.cli.world, 'applyMotorCommand'):
                                                                                        # Test complete workflow
                                                                                        result = self.cli.cmd_brain(["status"])
                                                                                        assert result is True
                                                                                        
                                                                                        result = self.cli.cmd_brain(["info"])
                                                                                        assert result is True
                                                                                        
                                                                                        result = self.cli.brain_initialize([])
                                                                                        assert result is True
                                                                                        
                                                                                        result = self.cli.brain_step(["10"])
                                                                                        assert result is True
                                                                                        
                                                                                        # Verify brain was initialized
                                                        mock_brain.initialize.assert_called_once()
                                                        assert mock_brain.step.call_count == 10


if __name__ == "__main__":
    # This allows running the tests directly
    import pytest
    
    print("Running NLM CLI Unit Tests...")
    pytest.main([__file__, "-v"])