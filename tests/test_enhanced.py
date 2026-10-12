#!/usr/bin/env python3
"""
Python tests for enhanced NLM Python bindings

This file tests the new functionality in the enhanced Python bindings,
including custom exceptions, enhanced configuration, and high-level utilities.
"""

import pytest
import tempfile
import json
import os
from pathlib import Path

# Import NLM modules
import pynlm
from pynlm import Config, Brain, SimpleWorld, AgentBrain
from pynlm import NLMBrainError, NLMInvalidParameter, NLMConfigurationError, NLMFileNotFound
class TestEnhancedConfiguration:
    """Test enhanced configuration functionality."""
    
    def test_config_builder_pattern(self):
        """Test configuration builder pattern."""
        config = (Config()
                 .withTimestep(0.001)
                 .withRandomSeed(42)
                 .withNeuronCount(100)
                 .withRegionCount(2))
        
        assert config is not None
        assert config.validate()  # Should validate
        
        # Test that config has the right values
        config_dict = config.to_dict()
        assert "simulation_timestep" in config_dict
        assert "random_seed" in config_dict
        assert "neuron_count" in config_dict
        assert "region_count" in config_dict
    
    def test_config_validation(self):
        """Test configuration validation."""
        # Valid config should pass
        valid_config = Config().withNeuronCount(50)
        assert valid_config.validate()
        
        # Invalid configs should fail validation
        invalid_configs = [
            {"simulation_timestep": -1.0},  # Negative timestep
            {"neuron_count": 0},           # Zero neurons
        ]
        
        for invalid_dict in invalid_configs:
            try:
                config = Config.from_dict(invalid_dict)
                # If validation doesn't throw, it should fail
                if config.validate():
                    pytest.fail(f"Expected invalid config {invalid_dict} to fail validation")
            except (NLMConfigurationError, Exception):
                pass  # Expected to fail
    
    def test_config_save_load(self):
        """Test configuration save and load."""
        config = Config().withNeuronCount(75)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            temp_file = f.name
        
        try:
            # Save with backup
            result = config.saveWithBackup(temp_file)
            assert result is True
            assert os.path.exists(temp_file)
            assert os.path.exists(temp_file + ".backup")
            
            # Load from file
            config2 = Config()
            loaded = config2.loadFromFile(temp_file)
            assert loaded is True
            assert config2.validate()
            
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)
            if os.path.exists(temp_file + ".backup"):
                os.unlink(temp_file + ".backup")
    
    def test_config_to_from_dict(self):
        """Test configuration dictionary conversion."""
        original_config = Config().withNeuronCount(200).withRegionCount(3)
        
        # Convert to dictionary
        config_dict = original_config.to_dict()
        assert isinstance(config_dict, dict)
        assert "neuron_count" in config_dict
        assert "region_count" in config_dict
        
        # Create from dictionary
        restored_config = Config.from_dict(config_dict)
        assert restored_config.validate()
        
        # Compare values
        assert config_dict["neuron_count"] == restored_config.getOr<int64_t>("neuron_count", 0)
    
    def test_config_summary(self):
        """Test configuration summary method."""
        config = Config().withNeuronCount(150)
        summary = config.getSummary()
        
        assert isinstance(summary, str)
        assert "neuron_count" in summary
        assert "150" in summary
class TestEnhancedBrain:
    """Test enhanced brain functionality."""
    
    def test_brain_creation(self):
        """Test brain creation with enhanced config."""
        config = Config().withNeuronCount(50)
        brain = Brain(config)
        
        assert brain is not None
        assert brain.getTotalNeuronCount() == 50
        assert brain.getRegionCount() >= 1
    
    def test_brain_metrics(self):
        """Test brain metrics collection."""
        brain = Brain().withNeuronCount(100)
        brain.initialize()
        
        # Run simulation
        brain.runEpisode(100)
        
        # Collect metrics
        metrics = brain.collectMetrics()
        
        assert isinstance(metrics, dict)
        assert "total_neurons" in metrics
        assert "total_synapses" in metrics
        assert "active_neurons" in metrics
        assert "firing_neurons" in metrics
        assert "average_firing_rate" in metrics
        assert "total_spikes" in metrics
        assert "excitation_inhibition_ratio" in metrics
        
        assert metrics["total_neurons"] == 100
    
    def test_brain_connectivity_analysis(self):
        """Test brain connectivity analysis."""
        brain = Brain().withNeuronCount(100)
        brain.initialize()
        
        # Run simulation to generate connections
        brain.runEpisode(50)
        
        # Analyze connectivity
        connectivity = brain.analyzeConnectivity()
        
        assert isinstance(connectivity, dict)
        assert "total_connections" in connectivity
        assert "strong_connections" in connectivity
        assert "weak_connections" in connectivity
        
        total = connectivity["total_connections"]
        strong = connectivity["strong_connections"]
        weak = connectivity["weak_connections"]
        
        assert strong + weak <= total  # Some connections might be medium strength
    
    def test_brain_benchmark(self):
        """Test brain performance benchmark."""
        brain = Brain().withNeuronCount(75)
        brain.initialize()
        
        # Run benchmark
        results = brain.benchmark(steps=200, warmupSteps=50)
        
        assert isinstance(results, dict)
        assert "steps" in results
        assert "warmup_steps" in results
        assert "total_time_ms" in results
        assert "steps_per_second" in results
        assert "spikes_generated" in results
        assert "final_firing_rate" in results
        
        assert results["steps"] == 200
        assert results["warmup_steps"] == 50
        assert results["steps_per_second"] > 0
    
    def test_brain_memory_info(self):
        """Test brain memory information."""
        brain = Brain().withNeuronCount(100)
        brain.initialize()
        
        # Get memory info
        memory_info = brain.getMemoryInfo()
        
        assert isinstance(memory_info, dict)
        assert "total_neurons" in memory_info
        assert "total_synapses" in memory_info
        assert "active_neurons" in memory_info
        assert "memory_usage_estimate_kb" in memory_info
        
        assert memory_info["total_neurons"] == 100
        assert memory_info["memory_usage_estimate_kb"] > 0
    
    def test_brain_find_neurons(self):
        """Test finding neurons by type."""
        brain = Brain().withNeuronCount(100)
        brain.initialize()
        
        # Find neurons by type (if the brain has neurons with types)
        try:
            excitatory_neurons = brain.findNeurons(pynlm.NeuronType.Excitatory)
            # Should return a list
            assert isinstance(excitatory_neurons, list)
        except (AttributeError, TypeError):
            # May not be available depending on brain implementation
            pass
    
    def test_brain_export_import(self):
        """Test brain state export and import."""
        # Create and run brain
        brain = Brain()
        brain.initialize()
        brain.runEpisode(100)
        
        # Export for analysis
        analysis_data = brain.exportForAnalysis()
        
        assert isinstance(analysis_data, dict)
        assert "total_spikes" in analysis_data
        assert "regions" in analysis_data
        
        # Create new brain and import
        new_brain = Brain()
        new_brain.importFromAnalysis(analysis_data)
        
        # New brain should be reset
        assert new_brain is not None
    
    def test_brain_validation(self):
        """Test brain validation."""
        # Valid brain should pass validation
        brain = Brain()
        brain.initialize()
        
        assert brain.validateBrain()
        
        # Reset brain (should still be valid)
        brain.reset()
        brain.initialize()
        assert brain.validateBrain()
class TestEnhancedAgentBrain:
    """Test enhanced AgentBrain functionality."""
    
    def test_agent_creation(self):
        """Test agent creation."""
        brain = Brain()
        agent = AgentBrain.createCuriosityDrivenAgent(brain)
        
        assert agent is not None
        assert agent.isCuriosityEnabled()
        assert agent.isRewardModulationEnabled()
    
    def test_agent_reward_based(self):
        """Test reward-based agent creation."""
        brain = Brain()
        agent = AgentBrain.createRewardBasedAgent(
            brain, enableRewardModulation=True, enableDevelopment=True
        )
        
        assert agent is not None
        assert agent.isRewardModulationEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()  # Should still be enabled by default
    
    def test_agent_exploration(self):
        """Test exploration agent creation."""
        brain = Brain()
        agent = AgentBrain.createExplorationAgent(brain, explorationRate=0.5)
        
        assert agent is not None
        assert agent.isCuriosityEnabled()
    
    def test_agent_run_episode_with_metrics(self):
        """Test agent episode with metrics."""
        brain = Brain()
        agent = AgentBrain.createCuriosityDrivenAgent(brain)
        world = SimpleWorld.create(100, 100)
        
        # Initialize agent
        agent.initialize(world)
        
        # Run episode with metrics
        results = agent.runEpisodeWithMetrics(world, steps=50, enableProfiler=False)
        
        assert isinstance(results, dict)
        assert "steps" in results
        assert "final_reward" in results
        assert "curiosity" in results
        assert "novelty" in results
        
        assert results["steps"] == 50
        assert results["curiosity"] >= 0.0
        assert results["novelty"] >= 0.0
class TestEnhancedWorld:
    """Test enhanced SimpleWorld functionality."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = SimpleWorld.create(200, 150, 10, 10)
        
        assert world is not None
        assert world.getWidth() == 200
        assert world.getHeight() == 150
        assert world.getVisionWidth() == 10
        assert world.getVisionHeight() == 10
    
    def test_world_configuration(self):
        """Test world configuration."""
        world = SimpleWorld()
        world.configure(300, 200, 15, 15)
        
        assert world.getWidth() == 300
        assert world.getHeight() == 200
        assert world.getVisionWidth() == 15
        assert world.getVisionHeight() == 15
    
    def test_world_object_management(self):
        """Test world object management."""
        world = SimpleWorld.create(100, 100)
        
        # Add object with validation
        result = world.addObjectWithValidation(25, 25, pynlm.WorldObjectType.Resource, 1.0, 0.5)
        
        assert result["success"] is True
        assert result["position"][0] == 25
        assert result["position"][1] == 25
        assert result["type"] == int(pynlm.WorldObjectType.Resource)
        
        # Check object exists
        objects = world.getObjects()
        assert len(objects) == 1
        assert objects[0].x == 25
        assert objects[0].y == 25
    
    def test_world_validation(self):
        """Test world validation."""
        world = SimpleWorld.create(50, 50)
        
        # Add agent
        world.setAgentStart(10, 10)
        
        # Add object
        world.addObjectWithValidation(20, 20, pynlm.WorldObjectType.Resource, 1.0)
        
        # Validate world
        validation = world.validateWorld()
        
        assert isinstance(validation, dict)
        assert "valid" in validation
        # Should be valid in this case
        assert validation["valid"] is True
    
    def test_world_template(self):
        """Test world templates."""
        templates = ["empty", "maze", "resource_harvest"]
        
        for template_name in templates:
            world = SimpleWorld.generateFromTemplate(template_name)
            assert world is not None
            assert world.validateWorld()["valid"]
    
    def test_world_episode(self):
        """Test world episode running."""
        world = SimpleWorld.create(100, 100)
        
        # Add agent
        world.setAgentStart(10, 10)
        
        # Run episode
        results = world.runEpisode(steps=100, recordHistory=False)
        
        assert isinstance(results, dict)
        assert "total_time_ms" in results
        assert "steps_per_second" in results
        assert "final_time" in results
        
        assert results["steps_per_second"] > 0
class TestSimulationManager:
    """Test SimulationManager functionality."""
    
    def test_simulation_creation(self):
        """Test simulation creation."""
        world = SimpleWorld.create(100, 100)
        brain = Brain().withNeuronCount(50)
        agent = AgentBrain.createCuriosityDrivenAgent(brain)
        
        # Create simulation
        simulation = pynlm.SimulationManager.createSimulation(world, brain, agent)
        
        assert isinstance(simulation, dict)
        assert "world_initialized" in simulation
        assert "brain_initialized" in simulation
        assert "agent_initialized" in simulation
        assert "ready" in simulation
        
        assert simulation["world_initialized"] is True
        assert simulation["brain_initialized"] is True
        assert simulation["agent_initialized"] is True
        assert simulation["ready"] is True
    
    def test_complete_simulation(self):
        """Test complete simulation."""
        world = SimpleWorld.create(150, 150)
        brain = Brain().withNeuronCount(75)
        agent = AgentBrain.createCuriosityDrivenAgent(brain)
        
        # Run complete simulation
        results = pynlm.SimulationManager.runCompleteSimulation(
            world, brain, agent, steps=200
        )
        
        assert isinstance(results, dict)
        assert "setup_time_ms" in results
        assert "simulation_time_ms" in results
        assert "total_steps" in results
        assert "brain_metrics" in results
        assert "world_metrics" in results
        
        assert results["total_steps"] == 200
        assert results["brain_metrics"]["total_neurons"] == 75
    
    def test_experiment(self):
        """Test experiment running."""
        world = SimpleWorld.create(200, 200)
        brain = Brain().withNeuronCount(100)
        agent = AgentBrain.createCuriosityDrivenAgent(brain)
        
        # Run experiment
        experiment_results = pynlm.SimulationManager.runExperiment(
            world, brain, agent,
            trials=3,
            stepsPerTrial=100,
            randomizeSeeds=True
        )
        
        assert isinstance(experiment_results, dict)
        assert "trials" in experiment_results
        assert "total_trials" in experiment_results
        assert "steps_per_trial" in experiment_results
        
        assert experiment_results["total_trials"] == 3
        assert experiment_results["steps_per_trial"] == 100
        assert len(experiment_results["trials"]) == 3
    
    def test_benchmark_suite(self):
        """Test benchmark suite."""
        brain = Brain().withNeuronCount(80)
        
        # Create benchmark
        benchmark_results = pynlm.SimulationManager.runBenchmark(
            brain,
            step_counts=[100, 200, 300],
            warmup_steps=[10, 20, 30]
        )
        
        assert isinstance(benchmark_results, dict)
        assert "benchmarks" in benchmark_results
        assert "total_benchmarks" in benchmark_results
        
        assert benchmark_results["total_benchmarks"] == 3
        assert len(benchmark_results["benchmarks"]) == 3
        
        # Check each benchmark
        for i, bench in enumerate(benchmark_results["benchmarks"]):
            assert "benchmark" in bench
            assert bench["benchmark"] == i
            assert "success" in bench
            assert "results" in bench
    
    def test_analysis_report(self):
        """Test analysis report generation."""
        # Create mock experiment results
        experiment_results = {
            "trials": [
                {
                    "brain_metrics": {
                        "total_time_ms": 100.0,
                        "steps_per_second": 1000.0,
                        "spikes_generated": 500
                    }
                },
                {
                    "brain_metrics": {
                        "total_time_ms": 200.0,
                        "steps_per_second": 1200.0,
                        "spikes_generated": 800
                    }
                }
            ]
        }
        
        # Generate analysis report
        report = pynlm.SimulationManager.generateAnalysisReport(experiment_results)
        
        assert isinstance(report, str)
        assert "NLM Experiment Analysis Report" in report
        assert "Average Time (ms)" in report
        assert "Average Steps/sec" in report
        assert "Total Spikes Generated" in report
class TestFactory:
    """Test Factory functionality."""
    
    def test_create_simulation_ecosystem(self):
        """Test creating simulation ecosystem."""
        ecosystem = pynlm.Factory.createSimulationEcosystem({
            'neuron_count': 150,
            'region_count': 2,
            'auto_initialize': True
        })
        
        assert isinstance(ecosystem, dict)
        assert "created" in ecosystem
        assert "config" in ecosystem
        assert "brain" in ecosystem
        assert "world" in ecosystem
        assert "agent" in ecosystem
        
        assert ecosystem["created"] is True
        assert ecosystem["brain"].getTotalNeuronCount() == 150
    
    def test_create_experiment_setup(self):
        """Test creating experiment setup."""
        experiment_types = ["default", "curiosity_driven", "reward_based", "exploration"]
        
        for exp_type in experiment_types:
            setup = pynlm.Factory.createExperimentSetup(exp_type)
            
            assert isinstance(setup, dict)
            assert "config" in setup
            assert "agent_type" in setup
            assert "description" in setup
            assert "created" in setup
            assert "experiment_type" in setup
            
            assert setup["created"] is True
            assert setup["experiment_type"] == exp_type
    
    def test_create_world_template(self):
        """Test creating world templates."""
        template_names = ["empty", "maze", "resource_harvest"]
        
        for template_name in template_names:
            world = pynlm.Factory.createWorldTemplate(template_name)
            
            assert world is not None
            assert world.validateWorld()["valid"]
    
    def test_create_benchmark_config(self):
        """Test creating benchmark configurations."""
        benchmark_types = ["quick", "thorough", "scalability", "standard"]
        
        for bench_type in benchmark_types:
            config = pynlm.Factory.createBenchmarkConfig(bench_type)
            
            assert isinstance(config, dict)
            assert "step_counts" in config
            assert "warmup_steps" in config
            assert "description" in config
            assert "benchmark_type" in config
            assert "created" in config
            
            assert config["created"] is True
            assert config["benchmark_type"] == bench_type
class TestCustomExceptions:
    """Test custom exception handling."""
    
    def test_custom_exception_classes(self):
        """Test custom exception classes."""
        # Test NLMBrainError
        brain_error = NLMBrainError("Test brain error")
        assert isinstance(brain_error, Exception)
        assert "NLM Brain Error" in str(brain_error)
        
        # Test NLMInvalidParameter
        param_error = NLMInvalidParameter("Test invalid parameter")
        assert isinstance(param_error, Exception)
        assert "NLM Invalid Parameter" in str(param_error)
        
        # Test NLMConfigurationError
        config_error = NLMConfigurationError("Test configuration error")
        assert isinstance(config_error, Exception)
        assert "NLM Configuration Error" in str(config_error)
        
        # Test NLMFileNotFound
        file_error = NLMFileNotFound("Test file not found")
        assert isinstance(file_error, Exception)
        assert "NLM File Not Found" in str(file_error)
    
    def test_exception_inheritance(self):
        """Test exception inheritance chain."""
        # All custom exceptions should inherit from Exception
        assert issubclass(NLMBrainError, Exception)
        assert issubclass(NLMInvalidParameter, Exception)
        assert issubclass(NLMConfigurationError, Exception)
        assert issubclass(NLMFileNotFound, Exception)
        
        # NLMBrainError should be a subclass of the base exception
        base_error = pynlm.NLMException("Base error")
        assert isinstance(base_error, Exception)
    
    def test_exception_in_simulation(self):
        """Test exceptions in simulation scenarios."""
        brain = Brain()
        
        # Test invalid parameter handling
        try:
            # This should trigger an invalid parameter error
            config = Config()
            config.set("simulation_timestep", -1.0, pynlm.ConfigSource.User)
            config.validate()  # Should fail validation
            pytest.fail("Expected validation to fail for negative timestep")
        except (NLMConfigurationError, Exception):
            pass  # Expected to fail
    
    def test_exception_messages(self):
        """Test exception messages are informative."""
        # Test that exception messages contain useful information
        brain_error = NLMBrainError("Failed to save brain state")
        assert "Failed to save brain state" in str(brain_error)
        
        param_error = NLMInvalidParameter("Invalid neuron count")
        assert "Invalid neuron count" in str(param_error)
        
        config_error = NLMConfigurationError("Missing required parameter")
        assert "Missing required parameter" in str(config_error)
class TestWorldTemplates:
    """Test world template functionality."""
    
    def test_empty_world(self):
        """Test empty world template."""
        world = pynlm.Factory.createWorldTemplate("empty")
        
        assert world is not None
        assert world.getWidth() > 0
        assert world.getHeight() > 0
        assert world.validateWorld()["valid"] is True
    
    def test_maze_world(self):
        """Test maze world template."""
        world = pynlm.Factory.createWorldTemplate("maze")
        
        assert world is not None
        assert world.validateWorld()["valid"] is True
        
        # Maze should have walls
        objects = world.getObjects()
        wall_count = sum(1 for obj in objects if obj.type == pynlm.WorldObjectType.Wall)
        assert wall_count > 0
    
    def test_resource_harvest_world(self):
        """Test resource harvest world template."""
        world = pynlm.Factory.createWorldTemplate("resource_harvest")
        
        assert world is not None
        assert world.validateWorld()["valid"] is True
        
        # Should have resources
        objects = world.getObjects()
        resource_count = sum(1 for obj in objects if obj.type == pynlm.WorldObjectType.Resource)
        assert resource_count > 0
    
    def test_world_template_consistency(self):
        """Test that world templates are consistent."""
        for template_name in ["empty", "maze", "resource_harvest"]:
            world = pynlm.Factory.createWorldTemplate(template_name)
            validation = world.validateWorld()
            
            assert validation["valid"], f"Template {template_name} validation failed"
            assert len(validation["errors"]) == 0, f"Template {template_name} has validation errors"
class TestIntegration:
    """Integration tests for complete workflows."""
    
    def test_complete_workflow(self):
        """Test complete simulation workflow."""
        # 1. Create ecosystem
        ecosystem = pynlm.Factory.createSimulationEcosystem({
            'neuron_count': 100,
            'region_count': 2,
            'curiosity_enabled': True
        })
        
        brain = ecosystem['brain']
        world = ecosystem['world']
        agent = ecosystem['agent']
        
        # 2. Run simulation
        simulation_results = pynlm.SimulationManager.runCompleteSimulation(
            world, brain, agent, steps=200
        )
        
        # 3. Run experiment
        experiment_results = pynlm.SimulationManager.runExperiment(
            world, brain, agent,
            trials=2,
            stepsPerTrial=150
        )
        
        # 4. Generate report
        report = pynlm.SimulationManager.generateAnalysisReport(experiment_results)
        
        # Verify results
        assert simulation_results["total_steps"] == 200
        assert len(experiment_results["trials"]) == 2
        assert "NLM Experiment Analysis Report" in report
    
    def test_experiment_with_analysis(self):
        """Test experiment with analysis and export."""
        # Create world and agents
        world = pynlm.Factory.createWorldTemplate("resource_harvest")
        brain = pynlm.Brain()
        agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)
        
        # Run experiment
        experiment = pynlm.SimulationManager.runExperiment(
            world, brain, agent,
            trials=2,
            stepsPerTrial=100
        )
        
        # Analyze each trial
        for trial_result in experiment["trials"]:
            brain_metrics = trial_result["brain_metrics"]
            assert brain_metrics["total_neurons"] == brain.getTotalNeuronCount()
            assert brain_metrics["steps_per_second"] > 0
    
    def test_performance_benchmark_integration(self):
        """Test performance benchmark integration."""
        brain = pynlm.Brain().withNeuronCount(150)
        
        # Run benchmark
        benchmark_results = pynlm.SimulationManager.runBenchmark(
            brain,
            step_counts=[100, 200, 300],
            warmup_steps=[10, 20, 30]
        )
        
        # Verify all benchmarks completed successfully
        successful_benchmarks = [
            bench for bench in benchmark_results["benchmarks"]
            if bench.get("success", False)
        ]
        
        assert len(successful_benchmarks) > 0
        
        # Check performance scaling
        for bench in successful_benchmarks:
            results = bench["results"]
            assert results["steps_per_second"] > 0
            assert results["spikes_generated"] > 0
class TestErrorHandling:
    """Test error handling scenarios."""
    
    def test_configuration_error_handling(self):
        """Test configuration error handling."""
        # Test invalid configuration
        invalid_configs = [
            {"simulation_timestep": -1.0},  # Invalid timestep
            {"neuron_count": 0},           # Invalid neuron count
            {"region_count": 0},          # Invalid region count
        ]
        
        for config_dict in invalid_configs:
            try:
                config = Config.from_dict(config_dict)
                if config.validate():
                    pytest.fail(f"Expected config validation to fail for {config_dict}")
            except NLMConfigurationError:
                pass  # Expected
    
    def test_file_error_handling(self):
        """Test file error handling."""
        # Test loading non-existent file
        config = Config()
        loaded = config.loadFromFile("/nonexistent/path/file.cfg")
        assert loaded is False  # Should return False, not throw
    
    def test_simulation_error_handling(self):
        """Test simulation error handling."""
        brain = Brain()
        
        # Valid brain should not throw errors
        try:
            brain.initialize()
            brain.runEpisode(10)
            # If we get here, the simulation is working
            assert True
        except NLMBrainError:
            pytest.fail("Valid brain should not throw NLMBrainError")
    
    def test_world_error_handling(self):
        """Test world error handling."""
        world = SimpleWorld.create(100, 100)
        
        # Add object and validate
        result = world.addObjectWithValidation(50, 50, pynlm.WorldObjectType.Resource, 1.0)
        assert result["success"] is True
        
        # Validate world
        validation = world.validateWorld()
        assert validation["valid"] is True
        assert len(validation["errors"]) == 0
class TestPerformanceAndMemory:
    """Test performance and memory management."""
    
    def test_memory_usage(self):
        """Test memory usage reporting."""
        brain = Brain().withNeuronCount(200)
        brain.initialize()
        
        memory_info = brain.getMemoryInfo()
        
        assert memory_info["total_neurons"] == 200
        assert memory_info["memory_usage_estimate_kb"] > 0
        assert memory_info["memory_usage_estimate_kb"] < 10000  # Should be reasonable
    
    def test_performance_scaling(self):
        """Test performance scaling with network size."""
        sizes = [50, 100, 200]
        results = []
        
        for size in sizes:
            brain = Brain().withNeuronCount(size)
            
            # Run benchmark
            bench_results = brain.benchmark(steps=100, warmupSteps=10)
            bench_results["size"] = size
            results.append(bench_results)
        
        # Check that larger networks are not dramatically slower
        for i in range(1, len(results)):
            ratio = results[i]["steps_per_second"] / results[i-1]["steps_per_second"]
            # Performance should scale roughly linearly, but could be sublinear
            assert ratio > 0.1, f"Performance degradation too severe: ratio = {ratio}"
    
    def test_benchmark_consistency(self):
        """Test benchmark consistency."""
        brain = Brain().withNeuronCount(100)
        brain.initialize()
        
        # Run multiple benchmarks
        benchmark_results = brain.benchmark(steps=200, warmupSteps=50)
        
        assert benchmark_results["steps_per_second"] > 0
        assert benchmark_results["total_time_ms"] > 0
        assert benchmark_results["spikes_generated"] > 0
    
    def test_memory_efficiency(self):
        """Test memory efficiency."""
        # Small network should use less memory
        small_brain = Brain().withNeuronCount(50)
        small_memory = small_brain.getMemoryInfo()["memory_usage_estimate_kb"]
        
        # Large network should use more memory
        large_brain = Brain().withNeuronCount(500)
        large_memory = large_brain.getMemoryInfo()["memory_usage_estimate_kb"]
        
        assert large_memory > small_memory
        assert large_memory / small_memory < 20  # Should scale roughly linearly
if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])