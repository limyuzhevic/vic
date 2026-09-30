#!/usr/bin/env python3
"""
Comprehensive examples for the NLM Python API.

This file provides practical examples demonstrating the use of the enhanced
NLM Python API, including high-level abstractions, configuration management,
and advanced simulation features.
"""

import os
import json
import time
from typing import Dict, Any

# Import NLM core modules
from nlm import BrainSimulator, WorldSimulator, AgentSimulator, MemoryManager
from nlm.advanced import AdvancedExperiment, CustomEnvironment
from nlm.experiments import Experiment, ExperimentSuite
from nlm.profiling import PerformanceProfiler
def quick_start_example():
    """
    Quick start example: Create a complete simulation with minimal code.
    
    This example demonstrates the easiest way to get started with NLM:
    1. Create a brain simulator with default configuration
    2. Run a simulation with performance monitoring
    3. Analyze the results
    """
    print("=== Quick Start Example ===")
    
    # Create brain simulator with minimal configuration
    config = {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.15,
        "random.seed": 42,
        "energy.decay_rate": 0.01
    }
    
    # Create simulator (this also initializes the brain)
    simulator = BrainSimulator(config_dict=config)
    
    print(f"Created BrainSimulator: {simulator.simulation_id}")
    print(f"Configuration: {config}")
    
    # Run simulation with progress monitoring
    results = simulator.run_simulation(
        num_steps=100,
        save_checkpoint=True,
        checkpoint_interval=50
    )
    
    print(f"\nSimulation Results:")
    print(f"  Duration: {results['duration']:.2f} seconds")
    print(f"  Steps executed: {results['steps_executed']}")
    print(f"  Total neurons: {results['metrics'].get('brain_stats', {}).get('total_neurons', 0)}")
    print(f"  Total synapses: {results['metrics'].get('brain_stats', {}).get('total_synapses', 0)}")
    
    # Save results
    simulator.save_simulation_state("quick_start_simulation.json")
    
    return results
def world_simulation_example():
    """
    World simulation example: Create and simulate a complete environment.
    
    This example demonstrates:
    1. Creating a world with specific dimensions
    2. Configuring the environment
    3. Adding agents to the world
    4. Running step-by-step simulation
    """
    print("\n=== World Simulation Example ===")
    
    # Create world with specific configuration
    world_config = {
        "world.width": 200,
        "world.height": 150,
        "world.vision_width": 15,
        "world.vision_height": 10,
        "world.max_energy": 200.0,
        "world.energy_decay_rate": 0.005,
        "random.seed": 123
    }
    
    # Create world simulator
    world_sim = WorldSimulator(
        config_dict=world_config,
        width=200,
        height=150,
        simulation_id="world_demo"
    )
    
    print(f"Created WorldSimulator: {world_sim.simulation_id}")
    print(f"World dimensions: {world_sim.width}x{world_sim.height}")
    
    # Configure world
    world_sim.configure(
        world_width=200,
        world_height=150,
        vision_width=20,
        vision_height=15,
        world_max_energy=200.0,
        world_energy_decay_rate=0.005
    )
    
    # Run world simulation
    world_results = world_sim.run_simulation(num_steps=50)
    
    print(f"\nWorld Simulation Results:")
    print(f"  Total steps: {len(world_results)}")
    print(f"  Final world state: {world_results[-1].get('world_state', {})}")
    
    return world_results
def agent_simulation_example():
    """
    Agent simulation example: Create and run agent-environment interaction.
    
    This example demonstrates:
    1. Creating a brain and agent
    2. Setting up a world environment
    3. Running agent episodes
    4. Analyzing agent behavior
    """
    print("\n=== Agent Simulation Example ===")
    
    # Create agent brain
    agent_config = {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.12,
        "random.seed": 456
    }
    
    brain = BrainSimulator(config_dict=agent_config).brain
    
    # Create world environment
    world_sim = WorldSimulator(width=100, height=100, simulation_id="agent_world")
    
    # Create agent simulator
    agent_sim = AgentSimulator(brain, world_sim)
    
    print(f"Created AgentSimulator with brain and world")
    print(f"Brain: {brain.getConfig().summary()}")
    
    # Run agent episode with checkpoints
    episode_results = agent_sim.run_episode(
        num_steps=30,
        save_checkpoint=True,
        checkpoint_interval=10
    )
    
    # Analyze episode results
    analysis = agent_sim.analyze_episode()
    
    print(f"\nAgent Episode Results:")
    print(f"  Total steps: {analysis['total_steps']}")
    print(f"  Success rate: {analysis['success_rate']:.2%}")
    print(f"  Average step time: {analysis['average_step_time_ms']:.2f} ms")
    print(f"  Peak neurons: {analysis['brain_stats']['peak_active_neurons']}")
    print(f"  Peak spikes: {analysis['brain_stats']['peak_total_spikes']}")
    
    # Save episode results
    agent_sim.save_results("agent_episode.json")
    
    return episode_results, analysis
def memory_management_example():
    """
    Memory management example: Demonstrate working with different memory systems.
    
    This example shows:
    1. Creating a memory manager
    2. Storing and retrieving working memory
    3. Managing episodic memory
    4. Managing associative memory
    """
    print("\n=== Memory Management Example ===")
    
    # Create a simple brain for demonstration
    config = {"neuron_count": 500, "region_count": 1, "random.seed": 789}
    brain = BrainSimulator(config_dict=config).brain
    
    # Create memory manager
    memory_manager = MemoryManager(brain)
    
    print("Created MemoryManager for brain")
    
    # Working memory operations
    print("\n--- Working Memory Operations ---")
    memory_manager.store_working_memory("location", "at position (10, 20)")
    memory_manager.store_working_memory("energy", "75%")
    memory_manager.store_working_memory("health", "good")
    
    # Retrieve working memory
    location = memory_manager.get_working_memory("location")
    energy = memory_manager.get_working_memory("energy")
    health = memory_manager.get_working_memory("health")
    
    print(f"Stored location: {location}")
    print(f"Stored energy: {energy}")
    print(f"Stored health: {health}")
    
    # Clear working memory
    memory_manager.clear_working_memory()
    print("Cleared working memory")
    
    # Episodic memory operations
    print("\n--- Episodic Memory Operations ---")
    episode_events = [
        "Agent started at (0, 0)",
        "Moved to (10, 20)",
        "Picked up item",
        "Moved to (15, 25)",
        "Placed item",
        "Reached goal"
    ]
    
    memory_manager.store_episodic_memory("episode_1", episode_events)
    
    # Recall episode
    recalled_events = memory_manager.get_episodic_memory("episode_1")
    print(f"Stored and recalled episode: {len(recalled_events)} events")
    
    # Clear episodic memory
    memory_manager.clear_episodic_memory()
    print("Cleared episodic memory")
    
    # Associative memory operations
    print("\n--- Associative Memory Operations ---")
    memory_manager.store_association("apple", "red", 0.9)
    memory_manager.store_association("apple", "fruit", 0.8)
    memory_manager.store_association("carrot", "orange", 0.7)
    memory_manager.store_association("carrot", "vegetable", 0.8)
    
    apple_red = memory_manager.get_association("apple", "red")
    apple_fruit = memory_manager.get_association("apple", "fruit")
    carrot_orange = memory_manager.get_association("carrot", "orange")
    carrot_veg = memory_manager.get_association("carrot", "vegetable")
    
    print(f"Apple red association: {apple_red}")
    print(f"Apple fruit association: {apple_fruit}")
    print(f"Carrot orange association: {carrot_orange}")
    print(f"Carrot vegetable association: {carrot_veg}")
    
    # Memory statistics
    print("\n--- Memory Statistics ---")
    stats = memory_manager.get_memory_stats()
    print(f"Working memory size: {stats['working_memory']['size']}")
    print(f"Working memory capacity: {stats['working_memory']['capacity']}")
    print(f"Episodic memory episodes: {stats['episodic_memory']['episode_count']}")
    print(f"Peak memory usage: {stats['performance']['peak_memory_mb']:.2f} MB")
    
    return memory_manager
class AdvancedExperimentExample:
    """Example demonstrating AdvancedExperiment usage."""
    
    def __init__(self, name: str = "advanced_experiment"):
        self.name = name
        self.experiment = AdvancedExperiment(name)
    
    def run_parameter_sweep(self):
        """Run a parameter sweep experiment."""
        print(f"\n=== Advanced Experiment: Parameter Sweep ===")
        
        # Define parameter grid
        param_grid = {
            "world_width": [50, 100, 150],
            "num_steps": [50, 100],
            "connection_probability": [0.1, 0.2]
        }
        
        def brain_factory(params: Dict[str, Any]):
            """Factory function to create brain with specific parameters."""
            config = {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": params.get("connection_probability", 0.1),
                "random.seed": 999,
                "world.width": params.get("world_width", 100),
                "world.height": params.get("world_width", 100)
            }
            brain = BrainSimulator(config_dict=config).brain
            return brain
        
        # Run parameter sweep
        results = self.experiment.run_parameter_sweep(param_grid, brain_factory)
        
        print(f"Parameter sweep completed with {len(results)} combinations")
        return results
    
    def run_multi_environment(self):
        """Run multi-environment experiment."""
        print(f"\n=== Advanced Experiment: Multi-Environment ===")
        
        # Define environments
        environments = [
            {"name": "small", "width": 50, "height": 50, "steps": 30},
            {"name": "medium", "width": 100, "height": 100, "steps": 50},
            {"name": "large", "width": 150, "height": 150, "steps": 80}
        ]
        
        def brain_factory(env_config: Dict[str, Any]):
            """Factory function for environment-specific brain."""
            config = {
                "neuron_count": 800,
                "region_count": 2,
                "connection_probability": 0.12,
                "random.seed": env_config.get("name", "env").lower()
            }
            brain = BrainSimulator(config_dict=config).brain
            return brain
        
        # Run multi-environment experiment
        results = self.experiment.run_multi_environment(environments, brain_factory)
        
        print(f"Multi-environment experiment completed with {len(results)} environments")
        return results
class ExperimentSuiteExample:
    """Example demonstrating ExperimentSuite usage."""
    
    def __init__(self, name: str = "experiment_suite"):
        self.suite = ExperimentSuite(name)
    
    def create_and_add_experiments(self):
        """Create and add different types of experiments."""
        print(f"\n=== Experiment Suite: Creating Experiments ===")
        
        # Create basic experiment
        basic_config = {
            "name": "basic_brain_simulation",
            "description": "Basic brain simulation with default parameters",
            "experiment_type": "basic",
            "parameters": {"num_steps": 100, "neuron_count": 500}
        }
        
        basic_experiment = Experiment(ExperimentConfig(**basic_config))
        
        # Set factories for basic experiment
        def brain_factory_basic(params):
            config = {"neuron_count": params.get("neuron_count", 500), "region_count": 1}
            return BrainSimulator(config_dict=config).brain
            
        def world_factory_basic(params):
            return WorldSimulator(width=params.get("world_width", 100), height=params.get("world_height", 100))
        
        basic_experiment.set_factories(brain_factory_basic, world_factory_basic)
        
        # Create parameter sweep experiment
        sweep_config = {
            "name": "parameter_sweep",
            "description": "Sweep different neuron counts",
            "experiment_type": "parameter_sweep",
            "parameters": {
                "parameter_grid": {"neuron_count": [100, 500, 1000]}
            }
        }
        
        sweep_experiment = Experiment(ExperimentConfig(**sweep_config))
        sweep_experiment.set_factories(brain_factory_basic, world_factory_basic)
        
        # Add experiments to suite
        self.suite.add_experiment(basic_experiment)
        self.suite.add_experiment(sweep_experiment)
        
        print(f"Added {len(self.suite.experiments)} experiments to suite")
        return self.suite
    
    def run_suite(self):
        """Run all experiments in the suite."""
        print(f"\n=== Experiment Suite: Running Suite ===")
        
        # Run all experiments
        results = self.suite.run_all()
        
        # Get suite summary
        summary = self.suite.get_suite_summary()
        
        print(f"\nExperiment Suite Results:")
        print(f"  Total experiments: {summary['total_experiments']}")
        print(f"  Successful experiments: {summary['successful_experiments']}")
        print(f"  Failed experiments: {summary['failed_experiments']}")
        print(f"  Success rate: {summary['success_rate']:.2%}")
        
        # Save suite results
        self.suite.save_suite_results("experiment_suite_results.json")
        
        return results, summary
def create_custom_environment_example():
    """
    Example demonstrating CustomEnvironment creation and usage.
    
    This shows how to create custom environments with unique rules and dynamics.
    """
    print("\n=== Custom Environment Example ===")
    
    # Create custom environment
    custom_env = CustomEnvironment(
        name="resource_collection",
        world_config={
            "width": 80,
            "height": 80,
            "vision_width": 8,
            "vision_height": 8,
            "max_energy": 150.0,
            "energy_decay_rate": 0.02
        }
    )
    
    print(f"Created CustomEnvironment: {custom_env.name}")
    print(f"Environment dimensions: {custom_env.world.getWidth()}x{custom_env.world.getHeight()}")
    
    # Add custom rule
    def no_boundary_violation(world, agent):
        """Rule: Agents cannot move outside world boundaries."""
        body = world.getAgentBody()
        if body.x < 0 or body.x >= world.getWidth() or body.y < 0 or body.y >= world.getHeight():
            return False
        return True
    
    custom_env.add_rule(no_boundary_violation, "No boundary violation")
    
    # Add dynamic behavior
    custom_env.add_dynamic("resource_decay", {"decay_rate": 0.01})
    
    # Configure environment
    custom_env.configure(
        width=80,
        height=80,
        vision_width=10,
        vision_height=10,
        max_energy=150.0,
        energy_decay_rate=0.02
    )
    
    print("\nCustom environment configured with:")
    print("  - Boundary violation rule")
    print("  - Resource decay dynamic")
    print("  - Custom dimensions and parameters")
    
    return custom_env
def performance_profiling_example():
    """
    Example demonstrating performance profiling with NLM.
    
    This shows how to monitor and profile simulation performance.
    """
    print("\n=== Performance Profiling Example ===")
    
    # Create performance profiler
    profiler = PerformanceProfiler(sampling_interval=0.1)
    
    print("Created PerformanceProfiler with 0.1s sampling interval")
    
    # Create brain and world for testing
    config = {"neuron_count": 500, "region_count": 1, "random.seed": 333}
    brain = BrainSimulator(config_dict=config).brain
    world = WorldSimulator(width=50, height=50)
    
    # Start profiling
    profiler.start_profiling()
    
    print("Started performance profiling")
    
    # Run some simulations to profile
    def test_simulation():
        brain.reset()
        world.reset()
        
        for step in range(20):
            world.step(0.01)
    
    # Profile the simulation
    result = profiler.benchmark_function(test_simulation, "test_simulation")
    
    # Run benchmark suite
    def quick_test():
        time.sleep(0.01)
        
    def medium_test():
        time.sleep(0.05)
        
    def heavy_test():
        time.sleep(0.1)
        
    test_functions = [quick_test, medium_test, heavy_test]
    suite_results = profiler.run_suite(test_functions, "performance_tests")
    
    # Stop profiling
    profiler.stop_profiling()
    
    # Generate report
    report = profiler.generate_report()
    
    print(f"\nPerformance Profiling Results:")
    print(f"  Benchmark result: {result.execution_time:.4f}s, {result.memory_usage:.2f}MB")
    print(f"  Suite results: {len(suite_results['tests'])} tests run")
    print(f"  Report generated:\n{report}")
    
    # Compare results
    if len(profiler.benchmark_results) >= 2:
        comparison = profiler.compare_results(
            profiler.benchmark_results[0],
            profiler.benchmark_results[1]
        )
        print(f"  Comparison: {comparison}")
    
    return profiler
def main():
    """
    Main function to run all examples.
    
    This function demonstrates the complete workflow of using the enhanced
    NLM Python API, from quick start to advanced features.
    """
    print("NLM Python API - Comprehensive Examples")
    print("=" * 50)
    
    examples = [
        ("Quick Start", quick_start_example),
        ("World Simulation", world_simulation_example),
        ("Agent Simulation", agent_simulation_example),
        ("Memory Management", memory_management_example),
        ("Custom Environment", create_custom_environment_example),
        ("Performance Profiling", performance_profiling_example)
    ]
    
    results = {}
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*20} {name} {'='*20}")
            result = example_func()
            results[name] = result
            print(f"✓ {name} completed successfully")
        except Exception as e:
            print(f"✗ {name} failed: {e}")
            results[name] = {"error": str(e)}
    
    # Advanced examples
    print(f"\n{'='*20} Advanced Examples {'='*20}")
    
    # Advanced experiment example
    try:
        advanced_exp = AdvancedExperimentExample()
        print("\n--- Parameter Sweep ---")
        advanced_exp.run_parameter_sweep()
        
        print("\n--- Multi-Environment ---")
        advanced_exp.run_multi_environment()
        print("✓ Advanced experiment examples completed")
    except Exception as e:
        print(f"✗ Advanced experiment examples failed: {e}")
    
    # Experiment suite example
    try:
        exp_suite = ExperimentSuiteExample()
        exp_suite.create_and_add_experiments()
        
        print("\n--- Running Experiment Suite ---")
        exp_suite.run_suite()
        print("✓ Experiment suite examples completed")
    except Exception as e:
        print(f"✗ Experiment suite examples failed: {e}")
    
    # Summary
    print(f"\n{'='*50}")
    print("EXAMPLE EXECUTION SUMMARY")
    print(f"{'='*50}")
    
    successful = sum(1 for result in results.values() if "error" not in result)
    total = len(results)
    
    print(f"Successful examples: {successful}/{total}")
    print(f"Success rate: {successful/total:.1%}")
    
    print(f"\nAll example results saved to:")
    print(f"  - quick_start_simulation.json")
    print(f"  - agent_episode.json")
    print(f"  - experiment_suite_results.json")
    
    return results
if __name__ == "__main__":
    # Run all examples
    all_results = main()
    
    # Save summary
    summary = {
        "execution_time": time.time(),
        "examples_run": len(all_results),
        "successful_examples": sum(1 for result in all_results.values() if "error" not in result),
        "results": {name: "success" if "error" not in result else f"error: {result['error']}" 
                   for name, result in all_results.items()}
    }
    
    with open("examples_summary.json", "w") as f:
        json.dump(summary, f, indent=2)
    
    print(f"\nSummary saved to examples_summary.json")
