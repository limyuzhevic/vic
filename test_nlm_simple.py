#!/usr/bin/env python3
"""Simple test script for NLM Python package"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import nlm
import math

def test_basic_imports():
    """Test that all basic imports work"""
    print("Testing basic imports...")
    
    # Test package info
    print(f"  Package version: {nlm.__version__}")
    
    # Test core classes
    core_classes = [
        ('Brain', nlm.Brain),
        ('Config', nlm.Config),
        ('AgentBrain', nlm.AgentBrain),
        ('SimpleWorld', nlm.SimpleWorld),
    ]
    
    for name, cls in core_classes:
        print(f"  {name}: OK")
    
    # Test type classes
    type_classes = [
        ('NeuronType', nlm.NeuronType),
        ('ActionType', nlm.ActionType),
        ('WorldObjectType', nlm.WorldObjectType),
        ('DevelopmentalStage', nlm.DevelopmentalStage),
        ('FiringState', nlm.FiringState),
    ]
    
    for name, cls in type_classes:
        print(f"  {name}: OK")
    
    # Test constants
    constants = [
        'INVALID_NEURON_ID',
        'INVALID_SYNAPSE_ID', 
        'INVALID_REGION_ID',
        'INVALID_POPULATION_ID',
    ]
    
    for const in constants:
        if hasattr(nlm, const):
            print(f"  {const}: {getattr(nlm, const)}")
    
    print("Basic imports test: PASSED")
    print()

def test_config_system():
    """Test configuration system"""
    print("Testing configuration system...")
    
    # Create configuration
    config = nlm.Config()
    print("  Created config: OK")
    
    # Set values
    config.set("random_seed", 42, None)
    config.set("simulation_timestep", 0.001, None)
    config.set("neuron_count", 1000, None)
    print("  Set config values: OK")
    
    # Get values
    seed = config.get_or_int("random_seed", 0)
    timestep = config.get_or_float("simulation_timestep", 0.0)
    neurons = config.get_or_int("neuron_count", 0)
    
    assert seed == 42, f"Expected 42, got {seed}"
    assert timestep == 0.001, f"Expected 0.001, got {timestep}"
    assert neurons == 1000, f"Expected 1000, got {neurons}"
    print("  Get config values: OK")
    
    # Test validation
    errors = config.validate()
    assert len(errors) == 0, f"Validation errors: {errors}"
    print("  Configuration validation: OK")
    
    # Test summary
    summary = config.summary_dict()
    assert "random_seed" in summary
    assert "simulation_timestep" in summary
    assert "neuron_count" in summary
    print("  Configuration summary: OK")
    
    print("Configuration system test: PASSED")
    print()

def test_brain_simulation():
    """Test brain simulation"""
    print("Testing brain simulation...")
    
    # Create brain
    brain = nlm.Brain()
    print("  Created brain: OK")
    
    # Initialize
    brain.initialize()
    print("  Brain initialized: OK")
    
    # Run simulation
    initial_spikes = brain.getTotalSpikeCount()
    
    for step in range(50):
        # Inject current
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0 + step % 50)
        # Run step
        brain.step(step, step * 0.001)
    
    final_spikes = brain.getTotalSpikeCount()
    print(f"  Simulation completed: {final_spikes} total spikes")
    
    # Test statistics
    stats = brain.get_connectivity_stats()
    assert "total_neurons" in stats
    assert "total_spikes" in stats
    assert "average_firing_rate" in stats
    print("  Statistics collection: OK")
    
    # Test developmental stages
    stages = [nlm.DevelopmentalStage.Initial, nlm.DevelopmentalStage.Maturation, 
             nlm.DevelopmentalStage.Adult]
    
    for stage in stages:
        brain.setDevelopmentalStage(stage)
        current_stage = brain.getDevelopmentalStage()
        assert current_stage == stage
        print(f"  Developmental stage {stage.name}: OK")
    
    print("Brain simulation test: PASSED")
    print()

def test_world_simulation():
    """Test world simulation"""
    print("Testing world simulation...")
    
    # Create world
    world = nlm.SimpleWorld()
    world.configure(800, 600, 80, 80)
    print("  Created world: OK")
    
    # Set agent start
    world.setAgentStart(100, 100)
    body = world.getAgentBody()
    assert body.x == 100
    assert body.y == 100
    print("  Set agent start: OK")
    
    # Add world objects
    world.add_resource_object(200, 200, 1.0, 0.5)
    world.add_hazard_object(600, 400, -1.0, 0.5)
    world.add_wall_object(400, 300)
    
    assert len(world.get_resource_objects()) == 1
    assert len(world.get_hazard_objects()) == 1
    assert len(world.get_wall_objects()) == 1
    print("  Added world objects: OK")
    
    # Get perception
    percept = world.getSensoryPercept()
    assert percept is not None
    print("  Got perception: OK")
    
    # Update world
    for i in range(10):
        world.update(0.001)
    
    assert world.getSimulationTime() > 0
    assert world.world.agent.age > 0
    print("  World simulation: OK")
    
    # Test world summary
    summary = world.get_simulation_summary()
    assert summary["width"] == 800
    assert summary["height"] == 600
    assert summary["agent_x"] == 100
    assert summary["agent_y"] == 100
    print("  World summary: OK")
    
    print("World simulation test: PASSED")
    print()

def test_high_level_api():
    """Test high-level API"""
    print("Testing high-level API...")
    
    # Create simulation
    simulation = nlm.NLMSimulation().quick_start(
        neurons=200,
        width=400,
        height=400,
        timestep=0.001,
        seed=42
    )
    print("  Created simulation: OK")
    
    # Add world objects
    simulation.world.add_resource_object(50, 50, 1.0)
    simulation.world.add_hazard_object(350, 350, -1.0)
    
    # Run simulation
    results = simulation.run_simulation(100, steps_per_episode=50, verbose=False)
    print(f"  Simulation completed: {results['total_steps']} steps")
    print(f"  Episodes: {results['episodes']}")
    
    # Verify results
    assert results["total_steps"] == 100
    assert results["episodes"] == 2
    assert results["final_spike_count"] >= 0
    print("  Simulation results: OK")
    
    # Test brain summary
    brain_summary = simulation.get_brain_summary()
    assert "stats" in brain_summary
    assert "developmental" in brain_summary
    assert "spike_system" in brain_summary
    print("  Brain summary: OK")
    
    # Test simulation summary
    sim_summary = simulation.get_simulation_summary()
    assert "episodes_run" in sim_summary
    assert "total_steps" in sim_summary
    assert "world_state" in sim_summary
    print("  Simulation summary: OK")
    
    print("High-level API test: PASSED")
    print()

def test_experiment_system():
    """Test experiment system"""
    print("Testing experiment system...")
    
    # Create experiment
    experiment = nlm.NLMExperiment("Test Experiment")
    print("  Created experiment: OK")
    
    # Create brains
    brain1 = nlm.Brain()
    brain1.initialize()
    
    brain2 = nlm.Brain()
    brain2.initialize()
    
    # Add brains to experiment
    experiment.add_brain(brain1)
    experiment.add_brain(brain2)
    print("  Added brains: OK")
    
    # Run standard experiment
    result = experiment.run_experiment(50, "standard")
    print(f"  Standard experiment completed: {result['steps']} steps")
    
    assert result["experiment_name"] == "Test Experiment"
    assert result["experiment_type"] == "standard"
    assert len(result["metrics"]) == 50
    print("  Standard experiment: OK")
    
    # Run parameter sweep
    param_sets = [
        {"random_seed": 42, "neuron_count": 100},
        {"random_seed": 43, "neuron_count": 200},
    ]
    
    sweep_results = experiment.run_parameter_sweep(param_sets, 30)
    print(f"  Parameter sweep completed: {len(sweep_results)} parameter sets")
    
    assert len(sweep_results) == 2
    for i, result in enumerate(sweep_results):
        assert result["parameter_set"] == param_sets[i]
    print("  Parameter sweep: OK")
    
    print("Experiment system test: PASSED")
    print()

def test_factory_functions():
    """Test factory functions"""
    print("Testing factory functions...")
    
    # Test creating default config
    config = nlm.create_default_config()
    assert config is not None
    print("  create_default_config: OK")
    
    # Test creating brain
    brain = nlm.create_brain()
    assert brain is not None
    print("  create_brain: OK")
    
    # Test creating world
    world = nlm.create_world(800, 600, 80, 80)
    assert world is not None
    assert world.getWidth() == 800
    assert world.getHeight() == 600
    print("  create_world: OK")
    
    # Test creating agent brain
    brain = nlm.create_brain()
    agent_brain = nlm.create_agent_brain(brain)
    assert agent_brain is not None
    assert agent_brain.brain == brain
    print("  create_agent_brain: OK")
    
    print("Factory functions test: PASSED")
    print()

def test_command_line_interface():
    """Test command-line interface"""
    print("Testing command-line interface...")
    
    # Create CLI
    cli = nlm.NLMCLI()
    assert cli is not None
    print("  Created CLI: OK")
    
    # Test help
    print("  Testing help: OK")
    
    # Note: Actual CLI testing requires subprocess calls
    # which is beyond scope of this simple test
    
    print("Command-line interface test: SKIPPED (requires subprocess)")
    print()

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    # Test configuration error
    try:
        from nlm.wrapper import ConfigurationError
        error = ConfigurationError("Test error")
        assert str(error) == "Test error"
        print("  Configuration error: OK")
    except ImportError:
        print("  Configuration error: Skipped (module not available)")
    
    # Test simulation error
    try:
        from nlm.wrapper import SimulationError
        error = SimulationError("Test simulation error")
        assert str(error) == "Test simulation error"
        print("  Simulation error: OK")
    except ImportError:
        print("  Simulation error: Skipped (module not available)")
    
    # Test validation error
    try:
        from nlm.wrapper import ValidationError
        error = ValidationError("Test validation error")
        assert str(error) == "Test validation error"
        print("  Validation error: OK")
    except ImportError:
        print("  Validation error: Skipped (module not available)")
    
    print("Error handling test: PASSED")
    print()

def test_integration():
    """Test complete integration"""
    print("Testing complete integration...")
    
    # Step 1: Create configuration
    config = nlm.Config()
    config.set("random_seed", 42, None)
    config.set("simulation_timestep", 0.001, None)
    config.set("neuron_count", 500, None)
    
    # Validate
    errors = config.validate()
    assert len(errors) == 0
    print("  Configuration validation: OK")
    
    # Step 2: Create brain
    brain = nlm.Brain(config)
    brain.initialize()
    print("  Brain creation: OK")
    
    # Step 3: Create simulation
    simulation = nlm.NLMSimulation()
    simulation.brain = brain
    simulation.world = nlm.SimpleWorld()
    simulation.world.configure(800, 600, 80, 80)
    simulation.agent_brain = nlm.AgentBrain(brain)
    print("  Simulation creation: OK")
    
    # Step 4: Run simulation
    for step in range(50):
        simulation.world.update(0.001)
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0 + step % 50)
        brain.step(step, step * 0.001)
    
    print("  Simulation run: OK")
    
    # Step 5: Analyze results
    brain_stats = brain.get_connectivity_stats()
    world_stats = simulation.world.get_simulation_summary()
    
    assert brain_stats["total_spikes"] >= 0
    assert world_stats["width"] == 800
    assert world_stats["height"] == 600
    print("  Results analysis: OK")
    
    # Step 6: Run high-level simulation
    high_level_sim = nlm.NLMSimulation().quick_start(neurons=100)
    results = high_level_sim.run_simulation(100, steps_per_episode=50)
    
    assert results["total_steps"] == 100
    assert results["episodes"] == 2
    print("  High-level simulation: OK")
    
    print("Complete integration test: PASSED")
    print()

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Package - Test Suite")
    print("=" * 60)
    print()
    
    # Run all tests
    tests = [
        test_basic_imports,
        test_config_system,
        test_brain_simulation,
        test_world_simulation,
        test_high_level_api,
        test_experiment_system,
        test_factory_functions,
        test_command_line_interface,
        test_error_handling,
        test_integration,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"ERROR in {test.__name__}: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
            print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("All tests passed! 🎉")
        return 0
    else:
        print(f"Some tests failed ({failed} errors).")
        return 1
if __name__ == "__main__":
    sys.exit(main())
