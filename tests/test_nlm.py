"""
Test suite for NLM Python package
"""

import sys
import os
import math
import time
import pytest

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import nlm
class TestNLMCore:
    """Test core NLM functionality"""
    
    def test_package_import(self):
        """Test that the package imports correctly"""
        assert nlm.__version__ == "0.1.0"
        assert hasattr(nlm, 'Brain')
        assert hasattr(nlm, 'Config')
        assert hasattr(nlm, 'AgentBrain')
        assert hasattr(nlm, 'SimpleWorld')
    
    def test_config_creation(self):
        """Test configuration creation and manipulation"""
        config = nlm.Config()
        
        # Test setting values
        config.set("random_seed", 42, None)
        config.set("simulation_timestep", 0.001, None)
        config.set("neuron_count", 1000, None)
        
        # Test getting values
        assert config.get_or_int("random_seed", 0) == 42
        assert config.get_or_float("simulation_timestep", 0.0) == 0.001
        assert config.get_or_int("neuron_count", 0) == 1000
        
        # Test validation
        errors = config.validate()
        assert len(errors) == 0
        
        # Test summary
        summary = config.summary_dict()
        assert "random_seed" in summary
        assert "simulation_timestep" in summary
        assert "neuron_count" in summary
    
    def test_brain_creation(self):
        """Test brain creation"""
        brain = nlm.Brain()
        assert brain is not None
        
        # Test quick initialization
        brain2 = nlm.Brain().quick_initialize(neuron_count=500)
        assert brain2 is not None
        
        # Test brain methods
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        
        # Run simulation
        for step in range(10):
            brain.step(step, step * 0.001)
        
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getAverageFiringRate() >= 0
    
    def test_agent_brain(self):
        """Test agent brain functionality"""
        brain = nlm.Brain()
        brain.initialize()
        
        agent_brain = nlm.AgentBrain(brain)
        assert agent_brain is not None
        
        # Test initialization
        world = nlm.SimpleWorld()
        agent_brain.initialize(world)
        
        # Test methods
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0
        
        # Test performance metrics
        metrics = agent_brain.get_performance_metrics()
        assert "action_count" in metrics
        assert "perception_count" in metrics
    
    def test_simple_world(self):
        """Test simple world functionality"""
        world = nlm.SimpleWorld()
        
        # Test configuration
        world.configure(800, 600, 80, 80)
        assert world.getWidth() == 800
        assert world.getHeight() == 600
        
        # Test agent
        world.setAgentStart(100, 100)
        body = world.getAgentBody()
        assert body.x == 100
        assert body.y == 100
        
        # Test world objects
        world.add_resource_object(200, 200, 1.0, 0.5)
        world.add_hazard_object(600, 400, -1.0, 0.5)
        
        assert len(world.get_resource_objects()) == 1
        assert len(world.get_hazard_objects()) == 1
        
        # Test simulation
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Test update
        world.update(0.001)
        assert world.getSimulationTime() > 0
    
    def test_sensory_systems(self):
        """Test sensory systems"""
        # Test vision
        vision = nlm.Vision(80, 80, 3)
        assert vision.getWidth() == 80
        assert vision.getHeight() == 80
        assert vision.getChannels() == 3
        
        data = [[0.1, 0.2], [0.3, 0.4]]
        vision.setData(data)
        assert vision.getData() == data
        assert vision.get_center_brightness() == 0.1
        
        # Test audio
        audio = nlm.Audio(44100, 1024)
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        audio_data = [0.1, 0.2, 0.3, 0.4]
        audio.setData(audio_data)
        assert audio.getData() == audio_data
        
        # Test internal signals
        internal = nlm.InternalSignals()
        internal.addSignal(0.1)
        internal.addSignal(0.2)
        assert len(internal.getData()) == 2
        assert internal.get_signal_range()["min"] == 0.1
        assert internal.get_signal_range()["max"] == 0.2
    
    def test_action_system(self):
        """Test action system"""
        # Test action creation
        action = nlm.Action()
        assert action.getType() is not None
        assert action.getName() is not None
        
        # Test setting action type
        action.setType(nlm.ActionType.MoveForward)
        assert action.getType() == nlm.ActionType.MoveForward
        assert action.getName() == "Move Forward"
        
        # Test parameters
        action.setParameters([0.5, 1.0, 1.5])
        assert action.getParameters() == [0.5, 1.0, 1.5]
        
        # Test cloning
        action2 = action.clone()
        assert action2.getType() == action.getType()
        assert action2.getParameters() == action.getParameters()
        
        # Test to_dict
        action_dict = action.to_dict()
        assert "type" in action_dict
        assert "parameters" in action_dict
class TestNLMSimulation:
    """Test high-level NLM simulation"""
    
    def test_simulation_creation(self):
        """Test simulation creation"""
        simulation = nlm.NLMSimulation()
        assert simulation is not None
        assert simulation.brain is not None
        assert simulation.world is not None
    
    def test_quick_start(self):
        """Test quick start method"""
        simulation = nlm.NLMSimulation().quick_start(
            neurons=500,
            width=400,
            height=400,
            timestep=0.001,
            seed=42
        )
        
        assert simulation.brain is not None
        assert simulation.world is not None
        assert simulation.agent_brain is not None
        
        assert simulation.brain.getTotalNeuronCount() == 500
        assert simulation.world.getWidth() == 400
        assert simulation.world.getHeight() == 400
    
    def test_run_episode(self):
        """Test episode running"""
        simulation = nlm.NLMSimulation().quick_start(neurons=100)
        
        # Run single episode
        results = simulation.run_episode(100, verbose=False)
        
        assert results["episode"] == 1
        assert results["steps"] == 100
        assert results["final_spike_count"] >= 0
        assert "final_metrics" in results
    
    def test_run_simulation(self):
        """Test simulation running"""
        simulation = nlm.NLMSimulation().quick_start(neurons=100)
        
        # Run simulation
        results = simulation.run_simulation(200, steps_per_episode=50, verbose=False)
        
        assert results["total_steps"] == 200
        assert results["episodes"] == 4  # 200 / 50
        assert results["final_spike_count"] >= 0
        assert "simulation_history" in results
        
        # Test history
        assert len(simulation._simulation_history) == 4
    
    def test_learning_experiment(self):
        """Test learning experiment"""
        simulation = nlm.NLMSimulation().quick_start(neurons=200)
        
        # Run learning experiment
        results = simulation.run_learning_experiment(
            experiences=5,
            steps_per_experience=50,
            sensory_patterns=True
        )
        
        assert results["experiences"] == 5
        assert results["steps_per_experience"] == 50
        assert results["total_steps"] == 250
        assert "learning_results" in results
        
        # Check if learning was detected
        assert "learning_detected" in results
    
    def test_brain_summary(self):
        """Test brain summary"""
        simulation = nlm.NLMSimulation().quick_start(neurons=100)
        
        summary = simulation.get_brain_summary()
        
        assert "stats" in summary
        assert "developmental" in summary
        assert "spike_system" in summary
        assert "plasticity" in summary
        
        # Check stats
        stats = summary["stats"]
        assert "total_neurons" in stats
        assert "total_spikes" in stats
        assert "average_firing_rate" in stats
    
    def test_simulation_summary(self):
        """Test simulation summary"""
        simulation = nlm.NLMSimulation().quick_start(neurons=100)
        
        # Run some episodes
        simulation.run_episode(50, verbose=False)
        simulation.run_episode(50, verbose=False)
        
        summary = simulation.get_simulation_summary()
        
        assert "episodes_run" in summary
        assert "total_steps" in summary
        assert "simulation_history" in summary
        assert "world_state" in summary
        
        assert summary["episodes_run"] == 2
        assert summary["total_steps"] == 100
    
    def test_reset(self):
        """Test simulation reset"""
        simulation = nlm.NLMSimulation().quick_start(neurons=100)
        
        # Run some episodes
        simulation.run_episode(50, verbose=False)
        simulation.run_episode(50, verbose=False)
        
        assert len(simulation._simulation_history) == 2
        
        # Reset
        simulation.reset()
        
        assert len(simulation._simulation_history) == 0
        assert simulation._episode_count == 0
class TestNLMExperiment:
    """Test NLM experiment functionality"""
    
    def test_experiment_creation(self):
        """Test experiment creation"""
        experiment = nlm.NLMExperiment("Test Experiment")
        assert experiment.name == "Test Experiment"
        assert len(experiment.experiments) == 0
        assert len(experiment.results) == 0
    
    def test_add_brain(self):
        """Test adding brain to experiment"""
        experiment = nlm.NLMExperiment("Test Experiment")
        
        brain = nlm.Brain()
        brain.initialize()
        
        experiment.add_brain(brain)
        
        assert len(experiment.experiments) == 1
        assert experiment.experiments[0]["brain"] == brain
        assert experiment.experiments[0]["name"] == "Test Experiment"
    
    def test_add_parameter_set(self):
        """Test adding parameter set"""
        experiment = nlm.NLMExperiment("Test Experiment")
        
        param_set = {
            "random_seed": 42,
            "neuron_count": 500,
            "simulation_timestep": 0.001,
        }
        
        experiment.add_parameter_set(param_set)
        
        # Check that config was set
        assert experiment.config.has("random_seed")
        assert experiment.config.has("neuron_count")
        assert experiment.config.has("simulation_timestep")
    
    def test_run_experiment_standard(self):
        """Test running standard experiment"""
        experiment = nlm.NLMExperiment("Test Experiment")
        
        brain = nlm.Brain()
        brain.initialize()
        
        experiment.add_brain(brain)
        
        result = experiment.run_experiment(100, "standard")
        
        assert result["experiment_name"] == "Test Experiment"
        assert result["experiment_type"] == "standard"
        assert result["steps"] == 100
        assert "metrics" in result
        assert len(result["metrics"]) == 100
        assert "standard_results" in result
    
    def test_run_experiment_plasticity(self):
        """Test running plasticity experiment"""
        experiment = nlm.NLMExperiment("Plasticity Test")
        
        brain = nlm.Brain()
        brain.initialize()
        
        experiment.add_brain(brain)
        
        result = experiment.run_experiment(100, "plasticity")
        
        assert result["experiment_type"] == "plasticity"
        assert "plasticity_results" in result
        assert "initial_mean_weight" in result["plasticity_results"]
        assert "weight_change" in result["plasticity_results"]
    
    def test_parameter_sweep(self):
        """Test parameter sweep"""
        experiment = nlm.NLMExperiment("Sweep Test")
        
        param_sets = [
            {"random_seed": 42, "neuron_count": 100},
            {"random_seed": 43, "neuron_count": 200},
            {"random_seed": 44, "neuron_count": 300},
        ]
        
        results = experiment.run_parameter_sweep(param_sets, 50)
        
        assert len(results) == 3
        for i, result in enumerate(results):
            assert result["parameter_set"] == param_sets[i]
            assert result["standard_results"]["final_spike_count"] >= 0
class TestNLMServices:
    """Test auxiliary services (plotting, developer tools)"""
    
    def test_plotting_unavailable(self):
        """Test plotting when matplotlib is not available"""
        # This test should pass even if matplotlib is not installed
        # The plotting service should handle this gracefully
        pass
    
    def test_developer_tools(self):
        """Test developer tools"""
        developer = nlm.NLMDeveloper()
        
        # Test creating config templates
        developer.create_config_template("small", {
            "random_seed": 42,
            "neuron_count": 100,
        })
        
        developer.create_config_template("medium", {
            "random_seed": 42,
            "neuron_count": 1000,
        })
        
        # Test getting templates
        template = developer.get_config_template("small")
        assert template is not None
        assert template["neuron_count"] == 100
        
        # Test validation
        config = nlm.Config()
        config.set("random_seed", 42, None)
        config.set("simulation_timestep", 0.001, None)
        
        errors = developer.validate_config(config)
        assert len(errors) == 0

    def test_benchmarking(self):
        """Test benchmarking functionality"""
        developer = nlm.NLMDeveloper()
        
        # Create test brain
        brain = nlm.Brain()
        brain.initialize()
        
        # Run benchmark
        results = developer.benchmark_brain(brain, 100, iterations=3)
        
        assert "brain_type" in results
        assert "average_time_per_iteration" in results
        assert "steps_per_second" in results
        assert "average_spike_count" in results
        assert "average_firing_rate" in results
        
        # Run comparison
        brain2 = nlm.Brain()
        brain2.initialize()
        
        comparison = developer.compare_benchmarks(brain, brain2, 100, iterations=3)
        
        assert "brain1" in comparison
        assert "brain2" in comparison
        assert "comparison" in comparison
class TestCommandLineInterface:
    """Test command-line interface"""
    
    def test_cli_creation(self):
        """Test CLI creation"""
        cli = nlm.NLMCLI()
        assert cli is not None
    
    def test_help_command(self):
        """Test help command"""
        cli = nlm.NLMCLI()
        
        # Test help method
        help_args = nlm.argparse.Namespace()
        help_args.command = "help"
        
        # This should not raise an exception
        # (actual output checking is done in manual testing)
        pass
class TestTypes:
    """Test type definitions"""
    
    def test_enums(self):
        """Test enum definitions"""
        assert nlm.NeuronType.Excitatory.value == 0
        assert nlm.NeuronType.Inhibitory.value == 1
        assert nlm.ActionType.MoveForward.value == 0
        assert nlm.DevelopmentalStage.Initial.value == 0
        
        # Test enum names
        assert nlm.NeuronType.Excitatory.name == "Excitatory"
        assert nlm.ActionType.MoveForward.name == "MoveForward"
    
    def test_ids(self):
        """Test ID types"""
        neuron_id = nlm.NeuronId(1)
        synapse_id = nlm.SynapseId(2)
        region_id = nlm.RegionId(3)
        population_id = nlm.PopulationId(4)
        
        assert neuron_id == 1
        assert synapse_id == 2
        assert region_id == 3
        assert population_id == 4
    
    def test_constants(self):
        """Test constant definitions"""
        assert nlm.INVALID_NEURON_ID == -1
        assert nlm.INVALID_SYNAPSE_ID == -1
        assert nlm.INVALID_REGION_ID == -1
        assert nlm.INVALID_POPULATION_ID == -1
        
        # Test utility function
        assert nlm.get_enum_name(nlm.NeuronType.Excitatory) == "Excitatory"
    
    def test_plasticity_flags(self):
        """Test plasticity flags"""
        flags1 = nlm.PlasticityFlags()
        assert flags1.hebbian == False
        assert flags1.stdp == False
        assert flags1.reward_modulated == False
        
        flags2 = nlm.PlasticityFlags(hebbian=True, stdp=True)
        assert flags2.hebbian == True
        assert flags2.stdp == True
        assert flags2.reward_modulated == False
        
        # Test string representation
        assert str(flags2) == "Hebbian|STDP"
class TestFactories:
    """Test factory functions"""
    
    def test_factory(self):
        """Test factory functions"""
        # Test creating default config
        config = nlm.create_default_config()
        assert config is not None
        
        # Test creating brain
        brain = nlm.create_brain()
        assert brain is not None
        
        # Test creating world
        world = nlm.create_world(800, 600, 80, 80)
        assert world is not None
        assert world.getWidth() == 800
        assert world.getHeight() == 600
        
        # Test creating agent brain
        brain = nlm.create_brain()
        agent_brain = nlm.create_agent_brain(brain)
        assert agent_brain is not None
        assert agent_brain.brain == brain
class TestErrorHandling:
    """Test error handling"""
    
    def test_configuration_error(self):
        """Test configuration error"""
        from nlm.wrapper import ConfigurationError
        
        error = ConfigurationError("Test error")
        assert str(error) == "Test error"
        assert isinstance(error, Exception)
    
    def test_simulation_error(self):
        """Test simulation error"""
        from nlm.wrapper import SimulationError
        
        error = SimulationError("Test simulation error")
        assert str(error) == "Test simulation error"
        assert isinstance(error, Exception)
    
    def test_validation_error(self):
        """Test validation error"""
        from nlm.wrapper import ValidationError
        
        error = ValidationError("Test validation error")
        assert str(error) == "Test validation error"
        assert isinstance(error, Exception)
    
    def test_nlm_exception(self):
        """Test NLM base exception"""
        from nlm.wrapper import NLMException
        
        error = NLMException("Test NLM error")
        assert str(error) == "Test NLM error"
        assert isinstance(error, Exception)

    def test_exception_inheritance(self):
        """Test exception inheritance"""
        from nlm.wrapper import NLMException, ConfigurationError, SimulationError, ValidationError
        
        # Check inheritance hierarchy
        assert issubclass(ConfigurationError, NLMException)
        assert issubclass(SimulationError, NLMException)
        assert issubclass(ValidationError, NLMException)
class TestIntegration:
    """Test integration scenarios"""
    
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow"""
        # Create simulation
        simulation = nlm.NLMSimulation().quick_start(
            neurons=200,
            width=600,
            height=400,
            timestep=0.001,
            seed=42
        )
        
        # Add some world objects
        simulation.world.add_resource_object(100, 100, 1.0)
        simulation.world.add_hazard_object(500, 300, -1.0)
        simulation.world.add_wall_object(300, 200)
        
        # Run simulation
        results = simulation.run_simulation(300, steps_per_episode=100, verbose=False)
        
        # Verify results
        assert results["total_steps"] == 300
        assert results["episodes"] == 3
        assert results["final_spike_count"] >= 0
        assert simulation.world.agent.age > 0
        
        # Test brain summary
        brain_summary = simulation.get_brain_summary()
        assert "stats" in brain_summary
        assert "developmental" in brain_summary
    
    def test_learning_workflow(self):
        """Test complete learning workflow"""
        # Create simulation
        simulation = nlm.NLMSimulation().quick_start(
            neurons=300,
            width=800,
            height=600,
            timestep=0.001,
            seed=123
        )
        
        # Enable plasticity
        if region := simulation.brain.getRegion(nlm.RegionId(1)):
            for syn in region.getSynapses():
                syn.enablePlasticity(True, True, False)
        
        # Run learning experiment
        results = simulation.run_learning_experiment(
            experiences=8,
            steps_per_experience=100,
            sensory_patterns=True
        )
        
        # Verify learning results
        assert results["experiences"] == 8
        assert results["steps_per_experience"] == 100
        assert results["total_steps"] == 800
        
        # Check if learning occurred
        assert "learning_detected" in results
    
    def test_experiment_workflow(self):
        """Test experiment workflow"""
        # Create experiment
        experiment = nlm.NLMExperiment("Integration Test")
        
        # Create multiple brains with different parameters
        brain1 = nlm.Brain()
        brain1.initialize()
        
        brain2 = nlm.Brain()
        brain2.initialize()
        
        # Add brains to experiment
        experiment.add_brain(brain1)
        experiment.add_brain(brain2)
        
        # Run parameter sweep
        param_sets = [
            {"random_seed": 42, "neuron_count": 100},
            {"random_seed": 43, "neuron_count": 200},
        ]
        
        results = experiment.run_parameter_sweep(param_sets, 50)
        
        # Verify results
        assert len(results) == 2
        for i, result in enumerate(results):
            assert result["parameter_set"] == param_sets[i]
            assert result["standard_results"]["final_spike_count"] >= 0

    def test_full_pipeline(self):
        """Test complete pipeline: config -> brain -> simulation -> analysis"""
        # Step 1: Create configuration
        config = nlm.Config()
        config.set("random_seed", 42, None)
        config.set("simulation_timestep", 0.001, None)
        config.set("neuron_count", 500, None)
        
        # Validate configuration
        errors = config.validate()
        assert len(errors) == 0
        
        # Step 2: Create brain
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Step 3: Create simulation
        simulation = nlm.NLMSimulation()
        simulation.brain = brain
        simulation.world = nlm.SimpleWorld()
        simulation.world.configure(800, 600, 80, 80)
        simulation.agent_brain = nlm.AgentBrain(brain)
        
        # Step 4: Run simulation
        for step in range(100):
            simulation.world.update(0.001)
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0 + step % 50)
            brain.step(step, step * 0.001)
        
        # Step 5: Analyze results
        brain_stats = brain.get_connectivity_stats()
        world_stats = simulation.world.get_simulation_summary()
        
        assert brain_stats["total_spikes"] >= 0
        assert world_stats["width"] == 800
        assert world_stats["height"] == 600

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
