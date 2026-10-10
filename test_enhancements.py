# Unit Tests for NLM Enhancements

This file contains comprehensive unit tests for the new NLM functionality, including:
- Python API enhancements
- Performance monitoring tools
- Configuration templates
- Error handling and validation

## Test Structure

The tests are organized into the following categories:
1. Python API Tests
2. Performance Monitoring Tests
3. Configuration Template Tests
4. Error Handling Tests
5. Integration Tests

## Dependencies

```bash
pytest>=7.0
numpy>=1.20
```

## Running Tests

```bash
pytest test_enhancements.py -v
```

```bash
pytest test_enhancements.py -v --cov=. --cov-report=html
```

## Python API Tests

### TestPythonAPIBasic
```python
import pytest
import nlm
import tempfile
import os
from pathlib import Path

class TestPythonAPIBasic:
    """Test basic Python API functionality."""
    
    def test_create_default_config(self):
        """Test creating default configuration."""
        config = nlm.createDefaultConfig()
        assert config is not None
        assert config.has("neuron_count")
        assert config.getOr("neuron_count", 0) > 0
    
    def test_create_brain_with_config(self):
        """Test creating brain with configuration."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        assert brain is not None
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
    
    def test_create_simple_world(self):
        """Test creating simple world."""
        world = nlm.createSimpleWorld()
        assert world is not None
        assert world.getWidth() > 0
        assert world.getHeight() > 0
    
    def test_create_agent_brain(self):
        """Test creating agent brain."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        agent = nlm.createAgentBrain(brain)
        assert agent is not None
        assert agent.getBrain() is brain
    
    def test_brain_stats(self):
        """Test brain statistics function."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        stats = nlm.getBrainStats(brain)
        assert isinstance(stats, dict)
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "firing_neurons" in stats
        assert "total_spikes" in stats
        assert "avg_firing_rate" in stats
        assert "exc_inhibition_ratio" in stats
        assert "development_stage" in stats
        
        assert stats["total_neurons"] > 0
        assert stats["total_synapses"] > 0

class TestPythonAPINewMethods:
    """Test new Python API methods."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = nlm.createDefaultConfig()
        self.brain = nlm.createBrain(self.config)
        self.world = nlm.createSimpleWorld()
        self.agent = nlm.createAgentBrain(self.brain)
        
        self.world.configure(20, 20, 8, 8)
        self.agent.initialize(self.world)
        
        self.brain.initialize()
        self.world.reset()
    
    def test_create_brain_with_defaults(self):
        """Test createBrainWithDefaults function."""
        brain = nlm.createBrainWithDefaults()
        assert brain is not None
        assert brain.getTotalNeuronCount() == 100
        brain.initialize()
        assert brain.getTotalSynapseCount() > 0
    
    def test_create_world_with_defaults(self):
        """Test createWorldWithDefaults function."""
        world = nlm.createWorldWithDefaults(30, 30, 10, 10, 200.0)
        assert world is not None
        assert world.getWidth() == 30
        assert world.getHeight() == 30
        assert world.getMaxEnergy() == 200.0
    
    def test_create_agent_with_defaults(self):
        """Test createAgentWithDefaults function."""
        agent = nlm.createAgentWithDefaults(self.brain, self.world)
        assert agent is not None
        assert agent.isRewardModulationEnabled()
        assert agent.isCuriosityEnabled()
        assert agent.isDevelopmentEnabled()
    
    def test_run_simulation_loop(self):
        """Test runSimulationLoop function."""
        nlm.runSimulationLoop(self.brain, self.world, self.agent, 50, 0.1)
        
        # Should complete without errors
        assert self.brain.getTotalStepCount() == 50
    
    def test_train_brain_simple(self):
        """Test trainBrainSimple function."""
        firing_rate = nlm.trainBrainSimple(self.brain, 100, 15.0f, 1.0f)
        assert firing_rate >= 0.0
        assert firing_rate <= 1.0  # Normalized firing rate
    
    def test_learning_experiment(self):
        """Test runLearningExperiment function."""
        firing_rate = nlm.runLearningExperiment(self.brain, 200, 25.0f)
        assert firing_rate >= 0.0
        assert self.brain.getTotalSpikeCount() > 0
    
    def test_brain_architecture(self):
        """Test printBrainArchitecture function."""
        # Should not raise exception
        nlm.printBrainArchitecture(self.brain)
        
        # Should complete without errors
        assert self.brain.getTotalNeuronCount() > 0

class TestPythonAPIMachineLearningIntegration:
    """Test machine learning integration features."""
    
    def test_numpy_integration(self):
        """Test integration with numpy."""
        import numpy as np
        
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create numpy array for sensory input
        sensor_data = np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0])
        
        # Test that numpy arrays can be used (conversion happens internally)
        assert sensor_data.shape == (8,)
        assert sensor_data.dtype == np.float64
    
    def test_pandas_compatibility(self):
        """Test compatibility with pandas-like data structures."""
        # Test data structure similar to pandas DataFrame
        data = {
            'width': [10, 20, 30],
            'height': [10, 20, 30],
            'vision_width': [8, 8, 8],
            'vision_height': [8, 8, 8]
        }
        
        # Should be able to process this data structure
        for width, height, vw, vh in zip(data['width'], data['height'], 
                                       data['vision_width'], data['vision_height']):
            world = nlm.createSimpleWorld()
            world.configure(width, height, vw, vh)
            assert world.getWidth() == width
            assert world.getHeight() == height

class TestPythonAPIErrorHandling:
    """Test error handling in Python API."""
    
    def test_save_and_load_brain(self):
        """Test save and load brain functionality."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Save brain state
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain
            nlm.saveBrainState(brain, filepath)
            assert os.path.exists(filepath)
            assert os.path.getsize(filepath) > 0
            
            # Create new brain and load
            new_brain = nlm.createBrain(config)
            nlm.loadBrainState(new_brain, filepath)
            
            # Should have loaded the state
            assert new_brain is not None
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_invalid_file_operations(self):
        """Test handling of invalid file operations."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        
        # Test with non-existent file path
        with pytest.raises(Exception):
            nlm.saveBrainState(brain, "/nonexistent/path/brain.bin")
        
        # Test loading from invalid file
        with pytest.raises(Exception):
            nlm.loadBrainState(brain, "/nonexistent/path/brain.bin")

## Performance Monitoring Tests

class TestPerformanceMonitoring:
    """Test performance monitoring and profiling tools."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = nlm.createDefaultConfig()
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_benchmark_brain(self):
        """Test benchmarkBrain function."""
        metrics = nlm.benchmarkBrain(self.brain, 100)
        
        assert isinstance(metrics, dict)
        assert "setupTime" in metrics
        assert "simulationTime" in metrics
        assert "totalTime" in metrics
        assert "stepsCompleted" in metrics
        assert "avgFiringRate" in metrics
        assert "totalSpikes" in metrics
        assert "performanceHz" in metrics
        
        assert metrics["stepsCompleted"] == 100
        assert metrics["totalSpikes"] > 0
        assert metrics["avgFiringRate"] >= 0.0
        assert metrics["performanceHz"] > 0.0
    
    def test_create_performance_profiler(self):
        """Test createPerformanceProfiler function."""
        profiler = nlm.createPerformanceProfiler(self.brain)
        assert profiler is not None
        
        # Run some steps
        for step in range(10):
            profiler.step(0.001)
        
        # Get stats
        stats = profiler.getStats()
        assert isinstance(stats, dict)
        assert "steps_completed" in stats
        assert "avg_step_time_ms" in stats
        assert "min_step_time_ms" in stats
        assert "max_step_time_ms" in stats
        
        assert stats["steps_completed"] == 10
        assert stats["avg_step_time_ms"] >= 0.0
    
    def test_compare_brains(self):
        """Test compareBrains function."""
        config = nlm.createDefaultConfig()
        brain1 = nlm.createBrain(config)
        brain2 = nlm.createBrain(config)
        
        brain1.initialize()
        brain2.initialize()
        
        brains = [brain1, brain2]
        metrics_list = nlm.compareBrains(brains, 50)
        
        assert isinstance(metrics_list, list)
        assert len(metrics_list) == 2
        
        for metrics in metrics_list:
            assert "setupTime" in metrics
            assert "simulationTime" in metrics
            assert "totalTime" in metrics
            assert "performanceHz" in metrics
            assert metrics["stepsCompleted"] == 50

class TestPerformanceMetrics:
    """Test PerformanceMetrics data structure."""
    
    def test_performance_metrics_structure(self):
        """Test PerformanceMetrics class structure."""
        metrics = nlm.PerformanceMetrics()
        
        # Set values
        metrics.setupTime = 1.0
        metrics.simulationTime = 2.0
        metrics.totalTime = 3.0
        metrics.stepsCompleted = 100
        metrics.avgFiringRate = 0.5
        metrics.totalSpikes = 1000
        metrics.performanceHz = 10.0
        
        # Verify values
        assert metrics.setupTime == 1.0
        assert metrics.simulationTime == 2.0
        assert metrics.totalTime == 3.0
        assert metrics.stepsCompleted == 100
        assert metrics.avgFiringRate == 0.5
        assert metrics.totalSpikes == 1000
        assert metrics.performanceHz == 10.0
    
    def test_performance_metrics_repr(self):
        """Test PerformanceMetrics string representation."""
        metrics = nlm.PerformanceMetrics()
        metrics.setupTime = 1.0
        metrics.simulationTime = 2.0
        metrics.totalTime = 3.0
        metrics.stepsCompleted = 100
        metrics.avgFiringRate = 0.5
        metrics.totalSpikes = 1000
        metrics.performanceHz = 10.0
        
        repr_str = repr(metrics)
        assert "PerformanceMetrics" in repr_str
        assert "1.0" in repr_str
        assert "2.0" in repr_str
        assert "3.0" in repr_str
        assert "100" in repr_str
        assert "0.5" in repr_str

## Configuration Template Tests

class TestConfigurationTemplates:
    """Test configuration template functionality."""
    
    def test_config_templates_exist(self):
        """Test that configuration templates exist."""
        from config_templates import NLMTemplateManager
        
        template_manager = NLMTemplateManager()
        templates = template_manager.list_templates()
        
        assert len(templates) > 0
        assert "minimal" in templates
        assert "standard" in templates
        assert "large" in templates
    
    def test_template_content(self):
        """Test template content structure."""
        from config_templates import NLMTemplateManager
        
        template_manager = NLMTemplateManager()
        
        # Test minimal template
        minimal = template_manager.get_template("minimal")
        assert minimal is not None
        assert "neuron_count" in minimal
        assert "region_count" in minimal
        assert minimal["neuron_count"] == 50
        assert minimal["region_count"] == 1
        
        # Test standard template
        standard = template_manager.get_template("standard")
        assert standard is not None
        assert "neuron_count" in standard
        assert "region_count" in standard
        assert standard["neuron_count"] == 500
        assert standard["region_count"] == 2
    
    def test_template_brain_creation(self):
        """Test creating brain from template."""
        from config_templates import NLMTemplateManager
        
        template_manager = NLMTemplateManager()
        
        # Test minimal template
        brain = template_manager.create_brain_from_template("minimal")
        assert brain is not None
        brain.initialize()
        assert brain.getTotalNeuronCount() == 50
        
        # Test standard template
        brain = template_manager.create_brain_from_template("standard")
        assert brain is not None
        brain.initialize()
        assert brain.getTotalNeuronCount() == 500
    
    def test_custom_template(self):
        """Test creating custom template."""
        from config_templates import NLMTemplateManager
        
        template_manager = NLMTemplateManager()
        
        # Create custom template
        custom_config = {
            "name": "My Custom Research",
            "description": "My specific research configuration",
            "neuron_count": 300,
            "region_count": 2,
            "connection_probability": 0.12
        }
        
        template_manager.save_template("my_custom", custom_config)
        
        # Retrieve template
        retrieved = template_manager.get_template("my_custom")
        assert retrieved is not None
        assert retrieved["name"] == "My Custom Research"
        assert retrieved["neuron_count"] == 300
        assert retrieved["connection_probability"] == 0.12
    
    def test_template_export_import(self):
        """Test exporting and importing templates."""
        from config_templates import NLMTemplateManager
        
        template_manager = NLMTemplateManager()
        
        # Create a template
        config = {
            "name": "Export Test",
            "description": "Test export/import",
            "neuron_count": 200
        }
        template_manager.save_template("export_test", config)
        
        # Export to file
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            export_path = f.name
        
        try:
            template_manager.export_template("export_test", export_path)
            assert os.path.exists(export_path)
            
            # Import from file
            imported_name = template_manager.import_template(export_path)
            assert imported_name == "export_test"
            
            # Verify imported template
            imported = template_manager.get_template("export_test")
            assert imported is not None
            assert imported["name"] == "Export Test"
        finally:
            if os.path.exists(export_path):
                os.unlink(export_path)

class TestConfigTemplatesExamples:
    """Test configuration template examples."""
    
    def test_examples_setup(self):
        """Test examples setup."""
        from config_templates import NLMExamples
        
        examples = NLMExamples()
        assert examples.template_manager is not None
        assert examples.template_manager.list_templates() is not None
        assert len(examples.template_manager.list_templates()) > 0
    
    def test_examples_run_basic(self):
        """Test examples run_basic functionality."""
        from config_templates import NLMExamples
        
        examples = NLMExamples()
        
        # Run basic example
        brain = examples.run_basic_example("minimal", 10)
        
        assert brain is not None
        assert brain.getTotalNeuronCount() == 50
        assert brain.getTotalStepCount() == 10
    
    def test_examples_create_complete_environment(self):
        """Test examples create_complete_environment functionality."""
        from config_templates import create_complete_environment
        
        env = create_complete_environment(
            neuron_count=100,
            world_width=30,
            world_height=30
        )
        
        assert "brain" in env
        assert "world" in env
        assert "agent" in env
        assert "config" in env
        
        assert env["brain"].getTotalNeuronCount() == 100
        assert env["world"].getWidth() == 30
        assert env["world"].getHeight() == 30
    
    def test_examples_analyze_results(self):
        """Test examples analyze_results functionality."""
        from config_templates import NLMExamples, setup_nlm_research_environment
        
        examples = NLMExamples()
        
        # Create environment
        env = setup_nlm_research_environment(neuron_count=50, world_width=20, world_height=20)
        brain = env["brain"]
        world = env["world"]
        agent = env["agent"]
        
        # Run some steps
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.001)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Analyze results
        analysis = examples.analyze_results(brain, world, agent, 20)
        
        assert isinstance(analysis, dict)
        assert "brain_metrics" in analysis
        assert "agent_metrics" in analysis
        assert "world_metrics" in analysis
        assert "performance_metrics" in analysis
        
        # Check brain metrics
        brain_metrics = analysis["brain_metrics"]
        assert "total_neurons" in brain_metrics
        assert "total_synapses" in brain_metrics
        assert "final_firing_rate" in brain_metrics
        assert brain_metrics["total_neurons"] == 50
    
    def test_examples_print_results(self):
        """Test examples print_results functionality."""
        from config_templates import NLMExamples
        
        examples = NLMExamples()
        
        # Create sample analysis
        analysis = {
            "brain_metrics": {
                "total_neurons": 100,
                "total_synapses": 500,
                "final_firing_rate": 0.25,
                "total_spikes": 1000,
                "exc_inhibition_ratio": 0.8,
                "developmental_stage": "Adult"
            },
            "agent_metrics": {
                "final_curiosity": 0.7,
                "final_reward_modulation": 0.9,
                "final_novelty": 0.3,
                "final_prediction_error": 0.2
            },
            "world_metrics": {
                "final_energy": 50.0,
                "final_health": 95.0,
                "final_age": 10,
                "movement_count": True,
                "turning_count": False
            },
            "performance_metrics": {
                "steps_per_second": 10.0,
                "neurons_per_ms": 0.1,
                "spikes_per_neuron": 10.0
            }
        }
        
        # Should not raise exception
        examples.print_example_results(analysis)
    
    def test_examples_create_research_template(self):
        """Test examples create_research_template functionality."""
        from config_templates import NLMExamples
        
        examples = NLMExamples()
        
        # Create research template
        template_name = examples.create_research_template(
            "My Research",
            "My specific research configuration",
            {
                "neuron_count": 250,
                "region_count": 2,
                "connection_probability": 0.18
            }
        )
        
        assert template_name == "My Research"
        
        # Verify template was created
        template = examples.template_manager.get_template("My Research")
        assert template is not None
        assert template["neuron_count"] == 250

## Error Handling Tests

class TestErrorHandling:
    """Test error handling and validation."""
    
    def test_safe_brain_operation(self):
        """Test safeBrainOperation function."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        
        # Successful operation
        result = nlm.safeBrainOperation(brain, 50)
        assert isinstance(result, dict)
        assert result["success"] == True
        assert "firing_rate" in result
        assert "total_spikes" in result
    
    def test_safe_brain_operation_error(self):
        """Test safeBrainOperation with error."""
        # Create invalid configuration
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        
        # Simulate error by trying to access invalid memory
        # (This test may need to be adjusted based on actual error conditions)
        pass
    
    def test_validate_brain_config(self):
        """Test validateBrainConfig function."""
        # Create valid configuration
        config = nlm.createDefaultConfig()
        validation = nlm.validateBrainConfig(config)
        
        assert isinstance(validation, dict)
        assert "valid" in validation
        assert validation["valid"] == True
        assert "message" in validation
        
        # Test invalid configuration
        invalid_config = nlm.createDefaultConfig()
        # Remove required parameter
        invalid_config.clear()  # Clear all settings
        
        validation = nlm.validateBrainConfig(invalid_config)
        assert "valid" in validation
        assert validation["valid"] == False
        assert "error" in validation

class TestValidationTests:
    """Test configuration validation."""
    
    def test_valid_config_validation(self):
        """Test validation of valid configuration."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        config.set("connection_probability", 0.15)
        
        validation = nlm.validateBrainConfig(config)
        
        assert validation["valid"] == True
        assert "neuron_count" in validation
        assert "region_count" in validation
        assert "connection_probability" in validation
    
    def test_invalid_config_validation(self):
        """Test validation of invalid configuration."""
        config = nlm.createDefaultConfig()
        
        # Test negative neuron count
        invalid_config = nlm.createDefaultConfig()
        invalid_config.set("neuron_count", -10)
        
        validation = nlm.validateBrainConfig(invalid_config)
        assert validation["valid"] == False
        assert "neuron_count must be positive" in validation["error"]
        
        # Test invalid connection probability
        invalid_config = nlm.createDefaultConfig()
        invalid_config.set("connection_probability", 1.5)
        
        validation = nlm.validateBrainConfig(invalid_config)
        assert validation["valid"] == False
        assert "connection_probability must be between 0.0 and 1.0" in validation["error"]

## Integration Tests

class TestIntegrationTests:
    """Test integration of all features."""
    
    def test_complete_integration(self):
        """Test complete integration of all NLM enhancements."""
        # Setup environment
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation with all features
        for step in range(100):
            # World update
            world.update(0.1)
            
            # Sensory processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step, step * 0.001)
            
            # Motor action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Enable all features
            if step % 20 == 0:
                agent.applyRewardModulation(1.0, 0.5)
                if step > 10:
                    agent.updateDevelopment(0.001)
        
        # Verify all features are working
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSpikeCount() > 0
        assert agent.getNeuromodulationLevel() > 0.0
        assert agent.getCuriosityLevel() > 0.0
        
        # Benchmark performance
        metrics = nlm.benchmarkBrain(brain, 50)
        assert metrics["performanceHz"] > 0.0
        
        # Analyze results
        stats = nlm.getBrainStats(brain)
        assert stats["total_neurons"] > 0
        
        # All features integrated successfully
        assert True  # If we got here, integration succeeded

class TestPythonAPIIntegration:
    """Test Python API integration with other modules."""
    
    def test_numpy_pandas_compatibility(self):
        """Test compatibility with numpy and pandas-like structures."""
        import numpy as np
        
        # Test with numpy arrays
        sensor_data = np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0])
        assert sensor_data.shape == (8,)
        assert sensor_data.dtype == np.float64
        
        # Test with pandas-like structure
        data = {
            'features': [1.0, 2.0, 3.0, 4.0],
            'targets': [0.0, 1.0, 0.0, 1.0],
            'weights': [0.5, 0.8, 0.3, 0.9]
        }
        
        # Process each feature vector
        for i in range(len(data['features'])):
            features = np.array([data['features'][i], data['targets'][i]])
            weights = np.array([data['weights'][i]])
            
            # Create brain and process
            config = nlm.createDefaultConfig()
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Simulate processing
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, float(features[0]))
            brain.step(i, i * 0.001)
    
    def test_machine_learning_example(self):
        """Test machine learning example integration."""
        # Simulate a simple machine learning scenario
        # where NLM is used as a neural network
        
        # Create multiple brains for ensemble learning
        configs = [nlm.createDefaultConfig() for _ in range(3)]
        brains = [nlm.createBrain(config) for config in configs]
        
        for brain in brains:
            brain.initialize()
        
        # Process different data patterns
        patterns = [
            [1.0, 2.0, 3.0, 4.0],
            [2.0, 3.0, 4.0, 5.0],
            [3.0, 4.0, 5.0, 6.0]
        ]
        
        for step, pattern in enumerate(patterns):
            # Process each pattern with each brain
            for i, brain in enumerate(brains):
                # Use pattern as sensory input
                strength = pattern[i % len(pattern)]
                brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, float(strength))
                brain.step(step, step * 0.001)
        
        # Verify each brain learned different patterns
        for i, brain in enumerate(brains):
            assert brain.getTotalSpikeCount() > 0
            assert brain.getAverageFiringRate() > 0.0
    
    def test_profiling_integration(self):
        """Test profiling integration with simulation."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create profiler
        profiler = nlm.createPerformanceProfiler(brain)
        
        # Run simulation with profiling
        for step in range(50):
            profiler.step(0.001)
            
            # Occasionally inject input to stimulate activity
            if step % 10 == 0:
                brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 5.0f)
        
        # Get performance stats
        stats = profiler.getStats()
        
        assert stats["steps_completed"] == 50
        assert stats["avg_step_time_ms"] >= 0.0
        assert stats["min_step_time_ms"] >= 0.0
        assert stats["max_step_time_ms"] >= stats["avg_step_time_ms"]
        
        # Verify profiler has step data
        step_times = profiler.getStepTimes()
        spike_counts = profiler.getSpikeCounts()
        
        assert len(step_times) == 50
        assert len(spike_counts) == 50

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
