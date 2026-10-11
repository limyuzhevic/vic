"""
Test suite for Phase 4 features of NLM

This module contains comprehensive tests for all new Phase 4 features
including advanced configuration management, performance profiling,
batch processing, simulation control, and multi-language integration.
"""
import time
import tempfile
import os
import queue
from typing import Any
from enum import Enum

# Import Phase 4 modules
from .advanced_config import AdvancedConfigManager, ValidationSeverity, ValidationResult
from .profiling import AdvancedProfiler, ProfileLevel, ProfileSample, ProfileResult
from .batch import BatchProcessor, BatchStatus, ProcessingMode, SimulationTask, BatchResult
from .simulation_control import SimulationState, ControlMode, Checkpoint, ControlCommand, AdvancedSimulationController
from .multi_language_integration import MultiLanguageIntegration

def create_mock_brain():
    """Create mock brain object for testing."""
    class MockBrain:
        def __init__(self):
            self.state = "initialized"
            self.spike_count = 0
            self.region_count = 1
            self.config = {}
        
        def initialize(self):
            self.state = "running"
        
        def step(self, step, time):
            self.spike_count += 1
            return True
        
        def save(self, path):
            pass
        
        def load(self, path):
            pass
        
        def getTotalSpikeCount(self):
            return self.spike_count
        
        def getFiringNeuronCount(self):
            return 10
        
        def getTotalNeuronCount(self):
            return 100
        
        def getRegionCount(self):
            return self.region_count
    
    return MockBrain()
def create_mock_world():
    """Create mock world object for testing."""
    class MockWorld:
        def __init__(self):
            self.state = "initialized"
            self.time = 0.0
        
        def initialize(self):
            self.state = "running"
        
        def update(self, timestep):
            self.time += timestep
            return True
        
        def getSensoryPercept(self):
            return {"sensory": "input", "time": self.time}
        
        def reset(self):
            self.state = "reset"
            self.time = 0.0
    
    return MockWorld()
def create_mock_agent():
    """Create mock agent object for testing."""
    class MockAgent:
        def __init__(self):
            self.state = "initialized"
        
        def initialize(self, world):
            self.state = "running"
        
        def process_sensory_input(self, percept):
            pass
        
        def decode_motor_command(self):
            return {"action": "move", "direction": "forward"}
        
        def update(self, timestep):
            pass
        
        def reset(self):
            self.state = "reset"
    
    return MockAgent()
class TestAdvancedConfigManager(unittest.TestCase):
    """Test AdvancedConfigManager functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config_manager = AdvancedConfigManager()
    
    def test_initialization(self):
        """Test AdvancedConfigManager initialization."""
        self.assertIsInstance(self.config_manager.get_config(), dict)
        self.assertEqual(len(self.config_manager.get_config()), 0)
    
    def test_set_config(self):
        """Test setting configuration."""
        config = {"brain.region_count": 2, "simulation.max_steps": 2000}
        self.config_manager.set_config(config)
        self.assertEqual(self.config_manager.get_config()["brain.region_count"], 2)
    
    def test_create_template(self):
        """Test template creation."""
        template_config = {"brain.region_count": 1, "simulation.max_steps": 1000}
        self.config_manager.create_template("test_template", template_config)
        
        retrieved = self.config_manager.get_template("test_template")
        self.assertEqual(retrieved["brain.region_count"", 1)
    
    def test_apply_template(self):
        """Test template application."""
        template_config = {"brain.region_count": 1, "simulation.max_steps": 1000}
        self.config_manager.create_template("test_template", template_config)
        
        # Set current config
        self.config_manager.set_config({"brain.region_count": 2})
        
        # Apply template
        self.config_manager.apply_template("test_template")
        
        # Check that template overrides were applied
        self.assertEqual(self.config_manager.get_config()["brain.region_count"], 1)
        self.assertEqual(self.config_manager.get_config()["simulation.max_steps"], 1000)
    
    def test_compare_configs(self):
        """Test configuration comparison."""
        config1 = AdvancedConfigManager({"brain.region_count": 1, "simulation.max_steps": 1000})
        config2 = AdvancedConfigManager({"brain.region_count": 2, "simulation.max_steps": 1000})
        
        comparison = config1.compare_configs(config2)
        
        self.assertIn("differences", comparison)
        self.assertIn("similarities", comparison)
        self.assertIn("recommendations", comparison)
        
        self.assertEqual(comparison["differences"]["brain.region_count"]["config_a"], 1)
        self.assertEqual(comparison["differences"]["brain.region_count"]["config_b"], 2)
    
    def test_validate_advanced(self):
        """Test advanced validation."""
        config = {"brain.region_count": 2, "simulation.max_steps": 2000}
        self.config_manager.set_config(config)
        
        # Test strict validation
        results = self.config_manager.validate_advanced("strict")
        self.assertIsInstance(results, list)
        
        # Test fast validation
        results = self.config_manager.validate_advanced("fast")
        self.assertIsInstance(results, list)
        
        # Test comprehensive validation
        results = self.config_manager.validate_advanced("comprehensive")
        self.assertIsInstance(results, list)
    
    def test_optimize_config(self):
        """Test configuration optimization."""
        config = {"brain.region_count": 5, "simulation.max_steps": 5000}
        self.config_manager.set_config(config)
        
        constraints = {
            "memory_limit": 100,  # MB
            "performance_target": "fast",
            "compatibility_target": "backward"
        }
        
        optimized = self.config_manager.optimize_config(constraints)
        self.assertIsInstance(optimized, AdvancedConfigManager)
    
    def test_save_config_with_validation(self):
        """Test configuration saving with validation."""
        config = {"brain.region_count": 1, "simulation.max_steps": 1000}
        self.config_manager.set_config(config)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            result = self.config_manager.save_config_with_validation(filepath, "json")
            self.assertTrue(result)
            self.assertTrue(os.path.exists(filepath))
            
            # Check that validation report was created
            report_file = filepath + '.validation_report'
            self.assertTrue(os.path.exists(report_file))
        
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
            report_file = filepath + '.validation_report'
            if os.path.exists(report_file):
                os.unlink(report_file)
    
    def test_config_summary(self):
        """Test configuration summary."""
        config = {"brain.region_count": 2, "simulation.max_steps": 2000}
        self.config_manager.set_config(config)
        
        # Add a template
        self.config_manager.create_template("test_template", {"brain.region_count": 1})
        
        summary = self.config_manager.get_config_summary()
        
        self.assertIn('total_keys', summary)
        self.assertIn('config_hash', summary)
        self.assertIn('template_count', summary)
        self.assertIn('validators_count', summary)
        self.assertIn('categories', summary)
        
        self.assertEqual(summary['total_keys'], 2)
        self.assertEqual(summary['template_count'], 1)
class TestAdvancedProfiler(unittest.TestCase):
    """Test AdvancedProfiler functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.profiler = AdvancedProfiler(ProfileLevel.BASIC)
    
    def test_initialization(self):
        """Test AdvancedProfiler initialization."""
        self.assertIsInstance(self.profiler.get_all_profiles(), dict)
        self.assertEqual(len(self.profiler.get_all_profiles()), 0)
    
    def test_start_stop_profiling(self):
        """Test starting and stopping profiling."""
        self.assertIsNone(self.profiler.stop_profiling("test_profile"))
        
        result = self.profiler.start_profiling("test_profile")
        self.assertIsInstance(result, AdvancedProfiler)
        
        # Get profile
        profile = self.profiler.get_profile("test_profile")
        self.assertIsNotNone(profile)
        self.assertEqual(profile.samples, 0)
        
        # Stop profiling
        stopped_profile = self.profiler.stop_profiling("test_profile")
        self.assertIsNotNone(stopped_profile)
        self.assertIsInstance(stopped_profile, ProfileResult)
    
    def test_real_time_monitoring(self):
        """Test real-time monitoring."""
        self.profiler.enable_real_time_monitoring()
        time.sleep(0.5)  # Let monitoring run
        self.profiler.disable_real_time_monitoring()
        
        # Check that metrics were collected
        metrics = self.profiler.get_current_metrics()
        self.assertIsInstance(metrics, dict)
    
    def test_profile_function(self):
        """Test function profiling."""
        def test_function(x):
            time.sleep(0.01)
            return x * 2
        
        result = self.profiler.profile_function("test_function", test_function, 5)
        self.assertEqual(result, 10)
        
        profile = self.profiler.get_profile("test_function")
        self.assertIsNotNone(profile)
        self.assertIn('test_function', profile.function_times)
    
    def test_generate_report(self):
        """Test report generation."""
        # Add some profile data
        profile = self.profiler.start_profiling("report_test")
        profile.add_function_time("test_function", 0.1)
        profile.add_operation_time("test_operation", 0.5)
        self.profiler.stop_profiling("report_test")
        
        # Generate report
        report = self.profiler.generate_report("report_test")
        self.assertIsInstance(report, str)
        self.assertIn("Performance Profile Report", report)
        self.assertIn("test_function", report)
        self.assertIn("test_operation", report)
class TestBatchProcessor(unittest.TestCase):
    """Test BatchProcessor functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = {
            'max_workers': 2,
            'retry_count': 2,
            'timeout': 10.0,
        }
        
        self.processor = BatchProcessor(self.config)
        
        # Create mock objects
        self.mock_brain = create_mock_brain()
        self.mock_world = create_mock_world()
        self.mock_agent = create_mock_agent()
    
    def test_initialization(self):
        """Test BatchProcessor initialization."""
        self.assertEqual(self.processor.status, BatchStatus.QUEUED)
        self.assertEqual(len(self.processor.tasks), 0)
        self.assertEqual(len(self.processor.results), 0)
    
    def test_add_simulation(self):
        """Test adding simulation to batch."""
        task_id = self.processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 100
        )
        
        self.assertIsInstance(task_id, str)
        self.assertEqual(len(self.processor.tasks), 1)
        self.assertEqual(len(self.processor.queue.queue), 1)
    
    def test_process_sequential(self):
        """Test sequential processing."""
        # Add multiple tasks
        task_id1 = self.processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 50
        )
        task_id2 = self.processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 75
        )
        
        # Process batch
        results = self.processor.process_batch(ProcessingMode.SEQUENTIAL)
        
        self.assertEqual(len(results), 2)
        self.assertEqual(self.processor.status, BatchStatus.COMPLETED)
        
        # Check that all tasks completed successfully
        for result in results:
            self.assertTrue(result.success)
    
    def test_process_parallel(self):
        """Test parallel processing."""
        # Add tasks
        task_id1 = self.processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 50
        )
        task_id2 = self.processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 75
        )
        
        # Process batch in parallel
        results = self.processor.process_batch(ProcessingMode.PARALLEL)
        
        self.assertEqual(len(results), 2)
        self.assertEqual(self.processor.status, BatchStatus.COMPLETED)
    
    def test_analyze_results(self):
        """Test results analysis."""
        # Add and process tasks
        task_id = self.processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 100
        )
        
        results = self.processor.process_batch(ProcessingMode.SEQUENTIAL)
        self.assertEqual(len(results), 1)
        
        # Analyze results
        analysis = self.processor.analyze_results()
        
        self.assertIn('statistics', analysis)
        self.assertIn('performance', analysis)
        self.assertIn('bottlenecks', analysis)
        self.assertIn('recommendations', analysis)
        
        self.assertEqual(analysis['statistics']['total_tasks'], 1)
        self.assertEqual(analysis['statistics']['completed_tasks'], 1)
        self.assertEqual(analysis['statistics']['success_rate'], 1.0)
class TestAdvancedSimulationController(unittest.TestCase):
    """Test AdvancedSimulationController functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.mock_brain = create_mock_brain()
        self.mock_world = create_mock_world()
        self.mock_agent = create_mock_agent()
        
        self.config = {
            'enable_monitoring': False,
            'enable_logging': False,
            'checkpoint_interval': 50,
            'max_checkpoints': 5,
        }
        
        self.controller = AdvancedSimulationController(
            self.mock_brain, self.mock_world, self.mock_agent, self.config
        )
    
    def test_initialization(self):
        """Test AdvancedSimulationController initialization."""
        self.assertEqual(self.controller.state, SimulationState.STOPPED)
        self.assertEqual(self.controller.current_step, 0)
        self.assertEqual(self.controller.current_time, 0.0)
    
    def test_start_stop_simulation(self):
        """Test starting and stopping simulation."""
        self.controller.start_simulation()
        self.assertEqual(self.controller.state, SimulationState.RUNNING)
        
        self.controller.stop_simulation()
        self.assertEqual(self.controller.state, SimulationState.STOPPED)
    
    def test_pause_resume_simulation(self):
        """Test pausing and resuming simulation."""
        self.controller.start_simulation()
        self.assertEqual(self.controller.state, SimulationState.RUNNING)
        
        self.controller.pause_simulation()
        self.assertEqual(self.controller.state, SimulationState.PAUSED)
        
        self.controller.resume_simulation()
        self.assertEqual(self.controller.state, SimulationState.RUNNING)
    
    def test_step_simulation(self):
        """Test simulation stepping."""
        self.controller.start_simulation()
        
        # Execute one step
        success = self.controller.step_simulation(timestep=0.1)
        self.assertTrue(success)
        
        self.assertEqual(self.controller.current_step, 1)
        self.assertEqual(self.controller.current_time, 0.1)
    
    def test_add_control_command(self):
        """Test adding control commands."""
        self.controller.start_simulation()
        
        # Add pause command
        self.controller.add_control_command('pause', {})
        
        # Check that command was added
        self.assertGreater(len(self.controller.command_queue.queue), 0)
        
        # Add resume command
        self.controller.add_control_command('resume', {})
        
        # Execute steps to process commands
        for _ in range(10):
            self.controller.step_simulation(timestep=0.1)
    
    def test_checkpoint_functionality(self):
        """Test checkpoint save/load functionality."""
        self.controller.start_simulation()
        
        # Save checkpoint
        self.controller.save_checkpoint("test_checkpoint")
        
        # Check that checkpoint was saved
        checkpoints = self.controller.list_checkpoints()
        self.assertIn("test_checkpoint", checkpoints)
        
        # Load checkpoint
        success = self.controller.load_checkpoint("test_checkpoint")
        self.assertTrue(success)
    
    def test_performance_report(self):
        """Test performance report generation."""
        self.controller.start_simulation()
        
        # Execute some steps
        for i in range(10):
            self.controller.step_simulation(timestep=0.1)
        
        # Generate performance report
        report = self.controller.get_performance_report()
        
        self.assertIn('simulation_summary', report)
        self.assertIn('performance_metrics', report)
        self.assertIn('step_statistics', report)
        self.assertIn('checkpoint_statistics', report)
        self.assertIn('optimization_opportunities', report)
    
    def test_simulation_episode(self):
        """Test simulation episode execution."""
        steps_completed = 0
        
        def progress_callback(data):
            nonlocal steps_completed
            steps_completed = data['step']
        
        self.controller.run_simulation_episode(
            20,  # steps
            0.1,  # timestep
            progress_callback,  # progress_callback
            None  # checkpoint_callback
        )
        
        self.assertEqual(self.controller.state, SimulationState.STOPPED)
        self.assertGreater(steps_completed, 0)
class TestMultiLanguageIntegration(unittest.TestCase):
    """Test MultiLanguageIntegration functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.integration = MultiLanguageIntegration()
    
    def test_initialization(self):
        """Test integration initialization."""
        result = self.integration.initialize()
        self.assertTrue(result)
    
    def test_cpp_integration(self):
        """Test C++ integration."""
        mock_cpp_instance = create_mock_brain()
        
        result = self.integration.integrate_cpp("cpp_test", mock_cpp_instance)
        self.assertTrue(result)
        
        # Check integration status
        status = self.integration.get_integration_status("cpp_test")
        self.assertEqual(status['type'], 'cpp')
    
    def test_python_integration(self):
        """Test Python integration."""
        mock_python_instance = create_mock_brain()
        
        result = self.integration.integrate_python("python_test", mock_python_instance)
        self.assertTrue(result)
        
        # Check integration status
        status = self.integration.get_integration_status("python_test")
        self.assertEqual(status['type'], 'python')
    
    def test_send_receive_message(self):
        """Test message sending and receiving."""
        mock_cpp_instance = create_mock_brain()
        self.integration.initialize()
        self.integration.integrate_cpp("cpp_test", mock_cpp_instance)
        
        # Send message
        message = {"type": "test", "target": "cpp_test"}
        result = self.integration.send_message("cpp_test", message)
        self.assertTrue(result.success)
    
    def test_broadcast_message(self):
        """Test message broadcasting."""
        # Add multiple integrations
        mock_cpp = create_mock_brain()
        mock_python = create_mock_brain()
        
        self.integration.initialize()
        self.integration.integrate_cpp("cpp_test", mock_cpp)
        self.integration.integrate_python("python_test", mock_python)
        
        # Broadcast message
        message = {"type": "broadcast", "target": "all"}
        results = self.integration.broadcast_message(message)
        
        self.assertEqual(len(results), 2)
        for result in results:
            self.assertTrue(result.success)
    
    def test_integration_status(self):
        """Test integration status reporting."""
        self.integration.initialize()
        
        # Get status of all integrations
        status = self.integration.get_all_integration_status()
        
        self.assertIsInstance(status, dict)
        self.assertGreater(len(status), 0)
        
        # Check that each integration has status
        for integration_name, integration_status in status.items():
            self.assertIn('status', integration_status)
            self.assertIn('name', integration_status)
class TestIntegration(unittest.TestCase):
    """Integration tests for Phase 4 features."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.mock_brain = create_mock_brain()
        self.mock_world = create_mock_world()
        self.mock_agent = create_mock_agent()
    
    def test_all_phase4_features(self):
        """Test all Phase 4 features together."""
        # Initialize advanced config manager
        config_manager = AdvancedConfigManager({
            "brain.region_count": 2,
            "simulation.max_steps": 2000,
            "learning.enable_curiosity": True,
            "logging.level": "info"
        })
        
        # Create brain with config
        self.mock_brain.config = config_manager.get_config()
        self.mock_brain.initialize()
        
        # Create profiler
        profiler = AdvancedProfiler(ProfileLevel.DETAILED)
        
        # Profile brain step
        profiler.start_profiling("brain_step")
        
        # Execute brain step
        self.mock_brain.step(1, 0.1)
        
        profiler.stop_profiling("brain_step")
        
        # Analyze profile
        profile = profiler.get_profile("brain_step")
        self.assertIsNotNone(profile)
        
        # Create batch processor
        batch_processor = BatchProcessor({
            'max_workers': 2,
            'timeout': 5.0
        })
        
        # Add simulation to batch
        task_id = batch_processor.add_simulation(
            self.mock_brain, self.mock_world, self.mock_agent, 50
        )
        
        # Process batch
        results = batch_processor.process_batch(ProcessingMode.SEQUENTIAL)
        self.assertEqual(len(results), 1)
        self.assertTrue(results[0].success)
        
        # Analyze batch results
        analysis = batch_processor.analyze_results()
        self.assertIn('statistics', analysis)
        
        # Test simulation controller
        controller = AdvancedSimulationController(
            self.mock_brain, self.mock_world, self.mock_agent,
            {'enable_monitoring': False, 'enable_logging': False}
        )
        
        controller.start_simulation()
        
        # Execute some steps
        for i in range(5):
            controller.step_simulation(timestep=0.1)
        
        status = controller.get_simulation_status()
        self.assertEqual(status['state'], SimulationState.RUNNING)
        self.assertEqual(status['current_step'], 5)
        
        # Test multi-language integration
        integration = MultiLanguageIntegration()
        result = integration.initialize()
        self.assertTrue(result)
        
        # Clean up
        integration.shutdown()
if __name__ == '__main__':
    unittest.main()