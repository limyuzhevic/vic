import sys
import os
import tempfile
import json
import subprocess
from unittest.mock import patch, MagicMock, call
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

from nlm_cli import NLMCLI

class TestNLMCLIIntegration:
    """Integration tests for NLM CLI with C++ backend."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
        self.test_script_dir = ".nlm_scripts"
    
    def teardown_method(self):
        """Clean up after each test method."""
        # Clean up test scripts directory
        if os.path.exists(self.test_script_dir):
            for f in os.listdir(self.test_script_dir):
                os.unlink(os.path.join(self.test_script_dir, f))
            os.rmdir(self.test_script_dir)
    
    def test_cli_with_real_config_system(self):
        """Test CLI integration with the enhanced config system."""
        # Create a temporary config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("neurons = 100\n")
            f.write("synapses = 1000\n")
            f.write("learning_rate = 0.01\n")
            config_file = f.name
        
        try:
            # Test config load command
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Test config list
            result = self.cli.cmd_config(["list"])
            assert result is True
            
            # Test config get
            result = self.cli.cmd_config(["get", "neurons"])
            assert result is True
            
            # Test config set
            result = self.cli.cmd_config(["set", "new_param", "test_value"])
            assert result is True
            
            # Test config validate
            result = self.cli.cmd_config(["validate"])
            assert result is True
        finally:
            os.unlink(config_file)
    
    def test_cli_brain_workflow(self):
        """Test complete brain workflow through CLI."""
        # Mock the brain and related methods
        mock_brain = MagicMock()
        
        # Mock brain initialization
        mock_brain.initialize.return_value = True
        mock_brain.reset.return_value = None
        mock_brain.logStatus.return_value = None
        mock_brain.getTotalNeuronCount.return_value = 100
        mock_brain.getTotalSynapseCount.return_value = 1000
        mock_brain.getActiveNeuronCount.return_value = 50
        mock_brain.getFiringNeuronCount.return_value = 30
        mock_brain.getTotalSpikeCount.return_value = 5000
        mock_brain.getAverageFiringRate.return_value = 50.0
        mock_brain.getDevelopmentalStage.return_value = "development"
        
        # Mock brain step
        def step_side_effect(step_num):
            # Simulate brain stepping
            pass
        mock_brain.step.side_effect = step_side_effect
        
        with patch.object(self.cli, 'brain', mock_brain):
            # Test brain status
            result = self.cli.cmd_brain(["status"])
            assert result is True
            
            # Test brain info
            result = self.cli.cmd_brain(["info"])
            assert result is True
            
            # Test brain initialize
            result = self.cli.cmd_brain(["initialize"])
            assert result is True
            mock_brain.initialize.assert_called_once()
            
            # Test brain step
            result = self.cli.cmd_brain(["step", "5"])
            assert result is True
            assert mock_brain.step.call_count == 5
            
            # Test brain reset
            result = self.cli.cmd_brain(["reset"])
            assert result is True
            mock_brain.reset.assert_called_once()
            mock_brain.initialize.assert_called()
    
    def test_cli_experiment_workflow(self):
        """Test experiment workflow through CLI."""
        # Mock experiment-related functionality
        with patch.object(self.cli, 'echo') as mock_echo:
            # Test experiment list
            result = self.cli.cmd_experiment(["list"])
            assert result is True
            
            # Test experiment run (would require more complex mocking)
            # For now, just test the command structure
            result = self.cli.cmd_experiment(["run"])
            assert result is True
            
            # Test experiment info
            result = self.cli.cmd_experiment(["info", "test_experiment"])
            assert result is True
            
            # Test experiment metrics
            result = self.cli.cmd_experiment(["metrics", "test_experiment"])
            assert result is True
    
    def test_cli_world_workflow(self):
        """Test world workflow through CLI."""
        # Mock world-related functionality
        mock_world = MagicMock()
        mock_world.update.return_value = None
        mock_world.getSensoryPercept.return_value = {"sensor1": 0.5, "sensor2": 0.3}
        mock_world.getSimulationTime.return_value = 1.0
        
        with patch.object(self.cli, 'world', mock_world):
            # Test world info
            result = self.cli.cmd_world(["info"])
            assert result is True
            
            # Test world reset
            result = self.cli.cmd_world(["reset"])
            assert result is True
            
            # Test world step
            result = self.cli.cmd_world(["step"])
            assert result is True
            mock_world.update.assert_called_once_with(0.1)
    
    def test_cli_monitor_workflow(self):
        """Test monitor workflow through CLI."""
        # Mock monitor-related functionality
        with patch.object(self.cli, 'echo') as mock_echo:
            # Test monitor status
            result = self.cli.cmd_monitor(["status"])
            assert result is True
            
            # Test monitor start
            result = self.cli.cmd_monitor(["start"])
            assert result is True
            
            # Test monitor stop
            result = self.cli.cmd_monitor(["stop"])
            assert result is True
    
    def test_cli_script_workflow_integration(self):
        """Test script workflow integration."""
        # Create test script
        script_content = """# Test script
nlm config set neurons 100
nlm brain initialize
nlm brain step 10
nlm brain save checkpoint.nml
"""
        
        # Create temporary script file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nml', delete=False) as f:
            f.write(script_content)
            script_path = f.name
        
        try:
            # Mock the dependencies for script execution
            with patch.object(self.cli.config, 'getKeys', return_value=["neurons"]):
                with patch.object(self.cli.config, 'get', return_value="100"):
                    with patch.object(self.cli.brain, 'initialize', return_value=True):
                        with patch.object(self.cli.brain, 'step'):
                            with patch.object(self.cli.brain, 'save', return_value=True):
                                with patch.object(self.cli, 'run_command', return_value=True):
                                    result = self.cli.script_execute([script_path])
                                    assert result is True
        finally:
            os.unlink(script_path)
    
    def test_cli_help_system(self):
        """Test the complete help system."""
        # Test general help
        result = self.cli.cmd_help([])
        assert result is True
        
        # Test specific command help
        commands = ["config", "brain", "experiment", "world", "monitor", "script"]
        for cmd in commands:
            result = self.cli.cmd_help([cmd])
            assert result is True, f"Help for command '{cmd}' failed"
    
    def test_cli_version_system(self):
        """Test version information system."""
        # Test version command
        result = self.cli.cmd_version([])
        assert result is True
        
        # Test version in JSON format
        self.cli.output_format = "json"
        result = self.cli.cmd_version([])
        assert result is True
    
    def test_cli_error_handling(self):
        """Test error handling in CLI commands."""
        # Test invalid command
        result = self.cli.run_command("invalid_command")
        assert result is False
        
        # Test invalid config subcommand
        result = self.cli.cmd_config(["invalid_subcommand"])
        assert result is True  # Should fall back to help
        
        # Test invalid brain subcommand
        result = self.cli.cmd_brain(["invalid_subcommand"])
        assert result is True  # Should fall back to help
    
    def test_cli_config_serialization(self):
        """Test configuration serialization and deserialization."""
        # Create a temporary config file
        config_data = {
            "neurons": 100,
            "synapses": 1000,
            "learning_rate": 0.01,
            "description": "Test configuration"
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(config_data, f)
            config_file = f.name
        
        try:
            # Test JSON config load
            result = self.cli.cmd_config(["load", config_file])
            assert result is True
            
            # Test config save
            result = self.cli.cmd_config(["save", "output_config.json"])
            assert result is True
            
            # Verify file was created
            assert os.path.exists("output_config.json")
            
            # Clean up
            if os.path.exists("output_config.json"):
                os.unlink("output_config.json")
        finally:
            os.unlink(config_file)
    
    def test_cli_interaction_simulation(self):
        """Test realistic CLI interaction simulation."""
        # Mock all the necessary components
        mock_brain = MagicMock()
        mock_world = MagicMock()
        mock_agent = MagicMock()
        
        # Configure mocks
        mock_brain.initialize.return_value = True
        mock_brain.logStatus.return_value = None
        mock_brain.getTotalNeuronCount.return_value = 100
        mock_brain.getTotalSynapseCount.return_value = 1000
        
        mock_world.update.return_value = None
        mock_world.getSensoryPercept.return_value = "sensory_data"
        mock_world.getSimulationTime.return_value = 1.0
        
        mock_agent.processSensoryInput.return_value = None
        mock_agent.decodeMotorCommand.return_value = "move_forward"
        mock_world.applyMotorCommand.return_value = None
        
        with patch.object(self.cli, 'brain', mock_brain):
            with patch.object(self.cli, 'world', mock_world):
                with patch.object(self.cli, 'agent', mock_agent):
                    # Simulate user interaction sequence
                    with patch.object(self.cli, 'echo') as mock_echo:
                        # User types 'help'
                        result = self.cli.run_command("help")
                        assert result is True
                        
                        # User types 'brain status'
                        result = self.cli.run_command("brain status")
                        assert result is True
                        
                        # User types 'brain initialize'
                        result = self.cli.run_command("brain initialize")
                        assert result is True
                        mock_brain.initialize.assert_called_once()
                        
                        # User types 'brain step 10'
                        result = self.cli.run_command("brain step 10")
                        assert result is True
                        assert mock_brain.step.call_count == 10
                        
                        # User types 'world step'
                        result = self.cli.run_command("world step")
                        assert result is True
                        mock_world.update.assert_called()
                        
                        # User types 'brain info'
                        result = self.cli.run_command("brain info")
                        assert result is True
    
    def test_cli_script_execution_with_errors(self):
        """Test script execution error handling."""
        # Create a script with an error command
        script_content = """nlm config set neurons 100
nlm invalid_command
nlm brain initialize
"""
        
        # Create temporary script file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.nml', delete=False) as f:
            f.write(script_content)
            script_path = f.name
        
        try:
            # Mock run_command to simulate errors
            call_count = 0
            def run_command_side_effect(command, args=None):
                nonlocal call_count
                call_count += 1
                if call_count == 2:  # Second command (invalid_command)
                    return False  # Simulate failure
                return True
            
            with patch.object(self.cli, 'run_command', side_effect=run_command_side_effect):
                result = self.cli.script_execute([script_path])
                # Script execution should continue even if individual commands fail
                assert result is True
        finally:
            os.unlink(script_path)


class TestNLMSimulationIntegration:
    """Integration tests for complete NLM simulation through CLI."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow."""
        # Mock all simulation components
        mock_brain = MagicMock()
        mock_world = MagicMock()
        mock_agent = MagicMock()
        
        # Configure brain
        mock_brain.initialize.return_value = True
        mock_brain.step.return_value = None
        mock_brain.logStatus.return_value = None
        mock_brain.getTotalNeuronCount.return_value = 100
        mock_brain.getTotalSynapseCount.return_value = 1000
        mock_brain.getActiveNeuronCount.return_value = 50
        mock_brain.getFiringNeuronCount.return_value = 30
        mock_brain.getTotalSpikeCount.return_value = 5000
        mock_brain.getAverageFiringRate.return_value = 50.0
        mock_brain.getDevelopmentalStage.return_value = "development"
        
        # Configure world
        mock_world.update.return_value = None
        mock_world.getSensoryPercept.return_value = {"visual": 0.8, "auditory": 0.3}
        mock_world.getSimulationTime.return_value = 1.0
        mock_world.applyMotorCommand.return_value = None
        
        # Configure agent
        mock_agent.processSensoryInput.return_value = None
        mock_agent.decodeMotorCommand.return_value = {"action": "move", "power": 0.5}
        
        with patch.object(self.cli, 'brain', mock_brain):
            with patch.object(self.cli, 'world', mock_world):
                with patch.object(self.cli, 'agent', mock_agent):
                    with patch.object(self.cli, 'echo') as mock_echo:
                        # Run complete simulation
                        result = self.cli.cmd_run([])
                        assert result is True
                        
                        # Verify brain initialization
                        mock_brain.initialize.assert_called_once()
                        
                        # Verify 100 simulation steps
                        assert mock_brain.step.call_count == 100
                        
                        # Verify world updates
                        assert mock_world.update.call_count == 100
                        
                        # Verify sensory processing
                        assert mock_agent.processSensoryInput.call_count == 100
                        
                        # Verify action processing
                        assert mock_agent.decodeMotorCommand.call_count == 100
                        
                        # Verify action application
                        assert mock_world.applyMotorCommand.call_count == 100
    
    def test_simulation_with_brain_save_load(self):
        """Test simulation with brain checkpointing."""
        # Mock brain with save/load capabilities
        mock_brain = MagicMock()
        mock_brain.initialize.return_value = True
        mock_brain.step.return_value = None
        mock_brain.save.return_value = True
        mock_brain.load.return_value = True
        mock_brain.logStatus.return_value = None
        
        with patch.object(self.cli, 'brain', mock_brain):
            # Initialize brain
            result = self.cli.cmd_brain(["initialize"])
            assert result is True
            
            # Run some steps
            result = self.cli.cmd_brain(["step", "5"])
            assert result is True
            
            # Save brain state
            result = self.cli.cmd_brain(["save", "checkpoint1.nml"])
            assert result is True
            mock_brain.save.assert_called_with("checkpoint1.nml")
            
            # Load brain state
            result = self.cli.cmd_brain(["load", "checkpoint1.nml"])
            assert result is True
            mock_brain.load.assert_called_with("checkpoint1.nml")


if __name__ == "__main__":
    import pytest
    
    print("Running NLM CLI Integration Tests...")
    pytest.main([__file__, "-v"])