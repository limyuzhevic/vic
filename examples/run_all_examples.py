#!/usr/bin/env python3

"""
NLM Example Scripts

Collection of example scripts demonstrating different use cases for the
Neural Learning Machine (NLM) neural simulation framework.

These examples showcase:
- Basic brain operations
- Agent-world interaction
- Learning and development
- Performance benchmarking
- Configuration management

To run examples, change to this directory and run:
    python example_name.py
"""

import sys
import os

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import time
import json
import argparse
from pathlib import Path

class NLMExampleRunner:
    """Runner for NLM example scripts."""
    
    def __init__(self):
        self.results_dir = Path("results")
        self.results_dir.mkdir(exist_ok=True)
    
    def example_basic_brain(self) -> None:
        """Example 1: Basic Brain Operations"""
        print("=== Example 1: Basic Brain Operations ===")
        
        # Create brain
        brain = pynlm.create_brain()
        brain.initialize()
        
        print(f"Created brain with {brain.get_total_neuron_count()} neurons")
        print(f"and {brain.get_total_synapse_count()} synapses")
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Get statistics
        spikes = brain.get_total_spike_count()
        firing = brain.get_firing_neuron_count()
        rate = brain.get_average_firing_rate()
        
        print(f"\nSimulation Results:")
        print(f"  Total spikes: {spikes}")
        print(f"  Firing neurons: {firing}")
        print(f"  Average firing rate: {rate:.2f}")
        print(f"  Excitement/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")
        
        # Save results
        results = {
            "example": "basic_brain",
            "total_spikes": spikes,
            "firing_neurons": firing,
            "average_firing_rate": rate,
            "excitation_inhibition_ratio": brain.get_excitation_inhibition_ratio()
        }
        
        with open(self.results_dir / "basic_brain_results.json", "w") as f:
            json.dump(results, f, indent=2)
        
        print(f"\n✓ Basic brain example completed!")
        print(f"  Results saved to: {self.results_dir / 'basic_brain_results.json'}")
    
    def example_agent_world(self) -> None:
        """Example 2: Agent-World Interaction"""
        print("\n=== Example 2: Agent-World Interaction ===")
        
        # Setup
        brain = pynlm.create_brain()
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.set_agent_start(10.0, 10.0)
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        print(f"Setup complete:")
        print(f"  World size: {world.get_width()}x{world.get_height()}")
        print(f"  Agent start: (10.0, 10.0)")
        print(f"  Enabled subsystems: Reward, Structural Plasticity, Development, Curiosity")
        
        # Run simulation
        total_reward = 0.0
        episodes = 5
        steps_per_episode = 50
        
        for episode in range(episodes):
            print(f"\nEpisode {episode + 1}:")
            
            for step in range(steps_per_episode):
                world.update(0.1)
                
                # Get percept and process
                percept = world.get_sensory_percept()
                agent.process_sensory_input(percept)
                
                # Brain step
                brain.step(step)
                
                # Get action and execute
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
                
                # Compute simple reward
                body = world.get_agent_body()
                reward = 1.0 if (0 <= body.x < world.get_width() and 
                               0 <= body.y < world.get_height()) else 0.0
                total_reward += reward
                
                # Apply neuromodulation
                agent.apply_reward_modulation(reward, 0.0)
                agent.update_development(0.1)
                
                # Print progress every 10 steps
                if step % 10 == 0:
                    print(f"  Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                          f"Reward({reward:.1f}), "
                          f"Curiosity({agent.get_curiosity_level():.2f})")
        
        # Get final statistics
        final_curiosity = agent.get_curiosity_level()
        final_novelty = agent.get_novelty_level()
        final_dopamine = agent.get_neuromodulation_level()
        
        print(f"\nEpisode Results:")
        print(f"  Total reward: {total_reward:.1f}")
        print(f"  Average reward per step: {total_reward / (episodes * steps_per_episode):.3f}")
        print(f"  Final curiosity: {final_curiosity:.3f}")
        print(f"  Final novelty: {final_novelty:.3f}")
        print(f"  Final dopamine level: {final_dopamine:.3f}")
        print(f"  Development stage: {brain.get_developmental_stage()}")
        
        # Save results
        results = {
            "example": "agent_world",
            "episodes": episodes,
            "steps_per_episode": steps_per_episode,
            "total_reward": total_reward,
            "average_reward_per_step": total_reward / (episodes * steps_per_episode),
            "final_curiosity": final_curiosity,
            "final_novelty": final_novelty,
            "final_dopamine": final_dopamine,
            "final_development_stage": str(brain.get_developmental_stage())
        }
        
        with open(self.results_dir / "agent_world_results.json", "w") as f:
            json.dump(results, f, indent=2)
        
        print(f"\n✓ Agent-world example completed!")
        print(f"  Results saved to: {self.results_dir / 'agent_world_results.json'}")
    
    def example_training_loop(self) -> None:
        """Example 3: Training Loop with Exploration"""
        print("\n=== Example 3: Training Loop with Exploration ===")
        
        # Create brain
        brain = pynlm.create_brain()
        brain.initialize()
        
        print(f"Created training brain with {brain.get_total_neuron_count()} neurons")
        
        # Training parameters
        episodes = 10
        steps_per_episode = 100
        exploration_rate = 0.2  # 20% exploration
        
        # Training loop
        episode_rewards = []
        episode_steps = []
        
        for episode in range(episodes):
            episode_reward = 0.0
            episode_steps_count = 0
            
            print(f"\nTraining Episode {episode + 1}:")
            
            for step in range(steps_per_episode):
                # Exploration vs exploitation
                explore = (hash(str(episode) + str(step)) % 100) < (exploration_rate * 100)
                
                # Inject input based on exploration strategy
                input_strength = 40.0 if explore else 20.0
                brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.SENSORY, 
                    input_strength)
                
                # Brain step
                brain.step(step, step * 0.001)
                
                # Collect statistics
                episode_reward += 1.0  # Simple reward
                episode_steps_count += 1
                
                # Print progress every 25 steps
                if step % 25 == 0:
                    print(f"  Step {step}: Exploration({explore}), "
                          f"Firing({brain.get_firing_neuron_count()}), "
                          f"Spikes({brain.get_total_spike_count()})")
            
            episode_rewards.append(episode_reward)
            episode_steps.append(episode_steps_count)
            
            # Apply some plasticity
            brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.Motor, 10.0f)
        
        # Calculate training statistics
        avg_reward = sum(episode_rewards) / len(episode_rewards)
        avg_steps = sum(episode_steps) / len(episode_steps)
        
        print(f"\nTraining Results:")
        print(f"  Episodes: {episodes}")
        print(f"  Steps per episode: {steps_per_episode}")
        print(f"  Average reward per episode: {avg_reward:.1f}")
        print(f"  Total steps completed: {sum(episode_steps)}")
        print(f"  Exploration rate: {exploration_rate * 100}%")
        
        # Final brain status
        print(f"\nFinal Brain Status:")
        print(f"  Total neurons: {brain.get_total_neuron_count()}")
        print(f"  Total synapses: {brain.get_total_synapse_count()}")
        print(f"  Total spikes: {brain.get_total_spike_count()}")
        print(f"  Average firing rate: {brain.get_average_firing_rate():.2f}")
        print(f"  Development stage: {brain.get_developmental_stage()}")
        
        # Save results
        results = {
            "example": "training_loop",
            "episodes": episodes,
            "steps_per_episode": steps_per_episode,
            "exploration_rate": exploration_rate,
            "episode_rewards": episode_rewards,
            "average_reward_per_episode": avg_reward,
            "total_steps_completed": sum(episode_steps),
            "final_brain_stats": {
                "total_neurons": brain.get_total_neuron_count(),
                "total_synapses": brain.get_total_synapse_count(),
                "total_spikes": brain.get_total_spike_count(),
                "average_firing_rate": brain.get_average_firing_rate(),
                "development_stage": str(brain.get_developmental_stage())
            }
        }
        
        with open(self.results_dir / "training_loop_results.json", "w") as f:
            json.dump(results, f, indent=2)
        
        print(f"\n✓ Training loop example completed!")
        print(f"  Results saved to: {self.results_dir / 'training_loop_results.json'}")
    
    def example_configuration(self) -> None:
        """Example 4: Configuration Management"""
        print("\n=== Example 4: Configuration Management ===")
        
        # Create configuration
        config = pynlm.create_default_config()
        
        print(f"Created default configuration")
        
        # Modify configuration
        config.set("neuron_count", 2000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.05)
        config.set("stdp_ltp_weight", 0.015f)
        config.set("stdp_ltd_weight", 0.012f)
        
        print(f"Modified configuration:")
        print(f"  Neuron count: {config.get_or('neuron_count', 1000)}")
        print(f"  Region count: {config.get_or('region_count', 1)}")
        print(f"  Connection probability: {config.get_or('connection_probability', 0.05)}")
        
        # Validate configuration
        warnings = pynlm.utils.validate_config(config)
        
        if warnings:
            print(f"\nConfiguration warnings:")
            for warning in warnings:
                print(f"  - {warning}")
        else:
            print(f"\nConfiguration is valid ✓")
        
        # Save configuration
        config_path = self.results_dir / "example_config.json"
        pynlm.save_config(config, str(config_path))
        print(f"\nConfiguration saved to: {config_path}")
        
        # Load configuration
        loaded_config = pynlm.load_config(str(config_path))
        print(f"\nLoaded configuration from file:")
        print(f"  Keys: {loaded_config.get_keys()}")
        
        # Create brain with loaded config
        brain = pynlm.create_brain(loaded_config)
        brain.initialize()
        
        print(f"\nCreated brain with loaded configuration:")
        print(f"  Neurons: {brain.get_total_neuron_count()}")
        print(f"  Synapses: {brain.get_total_synapse_count()}")
        
        # Run simulation with new config
        for step in range(50):
            brain.step(step)
        
        print(f"\n✓ Configuration example completed!")
        print(f"  Results saved to: {self.results_dir / 'configuration_results.json'}")
    
    def example_benchmarking(self) -> None:
        """Example 5: Performance Benchmarking"""
        print("\n=== Example 5: Performance Benchmarking ===")
        
        # Test different brain sizes
        brain_sizes = [100, 500, 1000, 2000]
        
        print(f"Running benchmarks for different brain sizes...")
        
        benchmark_results = []
        
        for neurons in brain_sizes:
            print(f"\nBenchmarking {neurons} neurons...")
            
            # Create configuration
            config = pynlm.create_default_config()
            config.set("neuron_count", neurons)
            
            # Create brain
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            # Run benchmark
            steps = 200
            start_time = time.time()
            
            for step in range(steps):
                brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.SENSORY, 10.0f)
                brain.step(step, step * 0.001)
            
            end_time = time.time()
            total_time = end_time - start_time
            steps_per_second = steps / total_time
            spikes_per_step = brain.get_total_spike_count() / steps
            
            benchmark_results.append({
                "neurons": neurons,
                "synapses": brain.get_total_synapse_count(),
                "total_spikes": brain.get_total_spike_count(),
                "total_time": total_time,
                "steps_per_second": steps_per_second,
                "spikes_per_step": spikes_per_step
            })
            
            print(f"  Completed: {total_time:.2f}s, {steps_per_second:.0f} steps/s, "
                  f"{spikes_per_step:.2f} spikes/step")
        
        # Print benchmark summary
        print(f"\nBenchmark Summary:")
        print(f"{'Neurons':<10} {'Synapses':<12} {'Spikes':<10} {'Time(s)':<10} {'Steps/s':<10} {'Spikes/step':<12}")
        print(f"{'-' * 70}")
        
        for result in benchmark_results:
            print(f"{result['neurons']:<10} {result['synapses']:<12} {result['total_spikes']:<10} "
                  f"{result['total_time']:<10.2f} {result['steps_per_second']:<10.0f} "
                  f"{result['spikes_per_step']:<12.2f}")
        
        # Save benchmark results
        benchmark_path = self.results_dir / "benchmark_results.json"
        with open(benchmark_path, "w") as f:
            json.dump(benchmark_results, f, indent=2)
        
        print(f"\n✓ Benchmarking example completed!")
        print(f"  Results saved to: {benchmark_path}")
    
    def example_developmental(self) -> None:
        """Example 6: Developmental Processes"""
        print("\n=== Example 6: Developmental Processes ===")
        
        # Create brain
        brain = pynlm.create_brain()
        brain.initialize()
        
        print(f"Created developmental brain")
        print(f"  Initial developmental stage: {brain.get_developmental_stage()}")
        
        # Simulate development over time
        developmental_stages = []
        developmental_ages = []
        
        for timestep in range(0, 1000, 100):
            # Get current developmental stage
            stage = brain.get_developmental_stage()
            developmental_stages.append(str(stage))
            
            # Get developmental age (simulation time)
            developmental_ages.append(timestep)
            
            # Simulate some development changes
            if timestep % 300 == 0:
                print(f"\nDevelopment checkpoint at timestep {timestep}:")
                print(f"  Stage: {stage}")
                print(f"  Neurons: {brain.get_total_neuron_count()}")
                print(f"  Synapses: {brain.get_total_synapse_count()}")
            
            # Run brain step with development
            brain.step(timestep, timestep * 0.001)
            
            # Apply some neuromodulation to affect development
            brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.Modulatory, 5.0f)
        
        # Get final developmental status
        final_stage = brain.get_developmental_stage()
        final_plasticity = brain.get_config().get_or("plasticity.structural.enable", True)
        
        print(f"\nDevelopment complete:")
        print(f"  Final developmental stage: {final_stage}")
        print(f"  Structural plasticity enabled: {final_plasticity}")
        print(f"  Final neuron count: {brain.get_total_neuron_count()}")
        print(f"  Final synapse count: {brain.get_total_synapse_count()}")
        
        # Save developmental results
        results = {
            "example": "developmental",
            "developmental_stages": developmental_stages,
            "developmental_ages": developmental_ages,
            "final_stage": str(final_stage),
            "final_plasticity": final_plasticity,
            "final_neuron_count": brain.get_total_neuron_count(),
            "final_synapse_count": brain.get_total_synapse_count()
        }
        
        with open(self.results_dir / "developmental_results.json", "w") as f:
            json.dump(results, f, indent=2)
        
        print(f"\n✓ Developmental example completed!")
        print(f"  Results saved to: {self.results_dir / 'developmental_results.json'}")
    
    def run_all_examples(self) -> None:
        """Run all examples."""
        print("Running all NLM examples...")
        print(f"Results will be saved to: {self.results_dir}")
        
        try:
            self.example_basic_brain()
            self.example_agent_world()
            self.example_training_loop()
            self.example_configuration()
            self.example_benchmarking()
            self.example_developmental()
            
            print(f"\n{'=' * 60}")
            print(f"✓ All examples completed successfully!")
            print(f"✓ Results saved to: {self.results_dir}")
            print(f"{'=' * 60}")
            
        except Exception as e:
            print(f"\n✗ Error running examples: {e}")
            raise


def main():
    """Main entry point for example runner."""
    print("NLM Example Scripts")
    print("=" * 60)
    
    runner = NLMExampleRunner()
    
    # Check if NLM is available
    try:
        import pynlm
        print("✓ NLM is available")
    except ImportError as e:
        print(f"✗ NLM not available: {e}")
        print("Please ensure NLM is properly installed.")
        sys.exit(1)
    
    # Run all examples
    runner.run_all_examples()
if __name__ == "__main__":
    main()