#!/usr/bin/env python3
"""
NLM Command Line Interface

Command-line tools for NLM (Neural Learning Machine) simulation framework.
Provides utilities for running simulations, managing experiments, and analyzing results.
"""

import argparse
import sys
import os
import json
import time
from typing import Optional, List, Dict, Any

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False
    print("Warning: pynlm module not found. Some functionality will be limited.")
class NLMCLI:
    """NLM Command Line Interface with comprehensive functionality."""
    
    def __init__(self):
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
    
    def create_config(self, neuron_count: int = 1000, region_count: int = 2, 
                     connection_prob: float = 0.1, timestep: float = 0.001) -> pynlm.Config:
        """Create a configuration with specified parameters."""
        config = pynlm.create_default_config()
        
        config.set_int("brain.neuron_count", neuron_count)
        config.set_int("brain.region_count", region_count)
        config.set_float("brain.connection_probability", connection_prob)
        config.set_float("brain.simulation_timestep", timestep)
        
        return config
    
    def create_brain(self, config: Optional[pynlm.Config] = None) -> pynlm.Brain:
        """Create and initialize a brain with the given configuration."""
        if config is None:
            config = self.create_config()
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        print(f"Brain created successfully:")
        print(f"  - Neurons: {brain.get_total_neuron_count()}")
        print(f"  - Regions: {brain.get_region_count()}")
        print(f"  - Synapses: {brain.get_synapse_count()}")
        
        return brain
    
    def create_world(self, width: int = 20, height: int = 20, 
                    vision_width: int = 8, vision_height: int = 8) -> pynlm.SimpleWorld:
        """Create and configure a world."""
        world = pynlm.create_simple_world()
        world.configure(width, height, vision_width, vision_height)
        world.reset()
        
        print(f"World created successfully:")
        print(f"  - Size: {width}x{height}")
        print(f"  - Vision: {vision_width}x{vision_height}")
        
        return world
    
    def create_agent(self, brain: pynlm.Brain, world: pynlm.SimpleWorld) -> pynlm.AgentBrain:
        """Create and initialize an agent with the given brain and world."""
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        print(f"Agent created successfully:")
        print(f"  - Sensory input size: {agent.get_sensory_input_size()}")
        print(f"  - Motor output size: {agent.get_motor_output_size()}")
        
        return agent
    
    def run_simulation(self, brain: pynlm.Brain, steps: int, verbose: bool = False) -> Dict[str, Any]:
        """Run a simulation with the given brain."""
        print(f"Running simulation for {steps} steps...")
        
        start_time = time.time()
        total_spikes = 0
        
        for i in range(steps):
            brain.step(i)
            total_spikes = brain.get_total_spike_count()
            
            if verbose and i % 100 == 0:
                print(f"  Step {i}: {brain.get_firing_neuron_count()} firing neurons, "
                      f"{total_spikes} total spikes")
        
        end_time = time.time()
        duration = end_time - start_time
        
        result = {
            "steps": steps,
            "total_spikes": total_spikes,
            "duration_seconds": duration,
            "firing_neurons": brain.get_firing_neuron_count(),
            "average_firing_rate": brain.get_average_firing_rate(),
            "e_i_ratio": brain.get_excitation_inhibition_ratio()
        }
        
        print(f"Simulation completed in {duration:.2f} seconds")
        print(f"  - Total spikes: {total_spikes}")
        print(f"  - Final firing rate: {brain.get_average_firing_rate():.2f} Hz")
        
        return result
    
    def run_agent_episode(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                         steps: int, verbose: bool = False) -> Dict[str, Any]:
        """Run an agent episode."""
        print(f"Running agent episode for {steps} steps...")
        
        start_time = time.time()
        
        for i in range(steps):
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process sensory input
            agent.process_sensory_input(percept)
            
            # Run brain step
            agent.get_brain().step(i)
            
            # Decode motor command
            action = agent.decode_motor_command()
            
            # Apply action to world
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Update development
            agent.update_development(0.1)
            
            if verbose and i % 100 == 0:
                print(f"  Step {i}: {agent.get_brain().get_firing_neuron_count()} firing neurons, "
                      f"curiosity: {agent.get_curiosity_level():.3f}, "
                      f"novelty: {agent.get_novelty_level():.3f}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        result = {
            "steps": steps,
            "duration_seconds": duration,
            "final_curiosity": agent.get_curiosity_level(),
            "final_novelty": agent.get_novelty_level(),
            "final_prediction_error": agent.get_prediction_error(),
            "final_dev_stage": static_cast[int>(agent.get_developmental_stage())
        }
        
        print(f"Agent episode completed in {duration:.2f} seconds")
        print(f"  - Final curiosity: {agent.get_curiosity_level():.3f}")
        print(f"  - Final novelty: {agent.get_novelty_level():.3f}")
        print(f"  - Final prediction error: {agent.get_prediction_error():.3f}")
        
        return result
    
    def save_brain(self, brain: pynlm.Brain, filepath: str) -> bool:
        """Save brain state to a file."""
        try:
            success = brain.save(filepath)
            if success:
                print(f"Brain state saved to: {filepath}")
            else:
                print(f"Failed to save brain state to: {filepath}")
            return success
        except Exception as e:
            print(f"Error saving brain state: {e}")
            return False
    
    def load_brain(self, brain: pynlm.Brain, filepath: str) -> bool:
        """Load brain state from a file."""
        try:
            success = brain.load(filepath)
            if success:
                print(f"Brain state loaded from: {filepath}")
            else:
                print(f"Failed to load brain state from: {filepath}")
            return success
        except Exception as e:
            print(f"Error loading brain state: {e}")
            return False
    
    def create_research_config(self, neurons: int = 5000, regions: int = 5) -> pynlm.Config:
        """Create a research-oriented configuration."""
        config = pynlm.create_default_config()
        
        # Set typical research configuration
        config.set_int("brain.neuron_count", neurons)
        config.set_int("brain.region_count", regions)
        config.set_float("brain.connection_probability", 0.05)
        config.set_float("brain.simulation_timestep", 0.001)
        config.set_float("plasticity.learning_rate", 0.01)
        config.set_float("plasticity.stdp.ltp_weight", 0.01)
        config.set_float("plasticity.stdp.ltd_weight", 0.012)
        
        # Enable all subsystems
        config.set_bool("neuromod.dopamine.enable", True)
        config.set_bool("neuromod.curiosity.enable", True)
        config.set_bool("neuromod.novelty.enable", True)
        config.set_bool("neuromod.prediction_error.enable", True)
        
        print(f"Research configuration created:")
        print(f"  - Neurons: {neurons}")
        print(f"  - Regions: {regions}")
        print(f"  - Connectivity: 5%")
        print(f"  - All neuromodulation enabled")
        
        return config
    
    def run_research_simulation(self, config: Optional[pynlm.Config] = None, 
                               steps: int = 1000) -> Dict[str, Any]:
        """Run a research simulation with advanced configuration."""
        if config is None:
            config = self.create_research_config()
        
        brain = self.create_brain(config)
        return self.run_simulation(brain, steps, verbose=True)
    
    def run_agent_research(self, config: Optional[pynlm.Config] = None,
                          world_width: int = 30, world_height: int = 30,
                          steps: int = 500) -> Dict[str, Any]:
        """Run an agent research simulation."""
        if config is None:
            config = self.create_research_config()
        
        brain = self.create_brain(config)
        world = self.create_world(world_width, world_height)
        agent = self.create_agent(brain, world)
        
        print(f"Starting advanced agent research with:")
        print(f"  - Environment: {world_width}x{world_height}")
        print(f"  - Agent enabled subsystems:")
        print(f"    - Reward modulation: {agent.is_reward_modulation_enabled()}")
        print(f"    - Structural plasticity: {agent.is_structural_plasticity_enabled()}")
        print(f"    - Development: {agent.is_development_enabled()}")
        print(f"    - Curiosity: {agent.is_curiosity_enabled()}")
        
        return self.run_agent_episode(agent, world, steps, verbose=True)
    
    def benchmark_agents(self, agent_count: int = 5, steps: int = 100) -> Dict[str, Any]:
        """Benchmark multiple agents with different configurations."""
        print(f"Benchmarking {agent_count} agents...")
        
        results = {}
        
        for i in range(agent_count):
            print(f"\nAgent {i + 1}/{agent_count}:")
            
            # Create different configuration for each agent
            config = self.create_research_config(neurons=1000 + i * 500, regions=2 + i)
            
            brain = self.create_brain(config)
            world = self.create_world(20, 20)
            agent = self.create_agent(brain, world)
            
            # Enable different combinations of features
            if i % 2 == 0:
                agent.enable_reward_modulation(True)
                agent.enable_curiosity(True)
            else:
                agent.enable_structural_plasticity(True)
                agent.enable_development(True)
            
            # Run episode
            result = self.run_agent_episode(agent, world, steps, verbose=False)
            result["agent_id"] = i
            result["neurons"] = brain.get_total_neuron_count()
            
            results[f"agent_{i}"] = result
        
        print(f"\nBenchmark completed. Results saved to results.json")
        return results
    
    def save_results(self, results: Dict[str, Any], filepath: str = "results.json"):
        """Save benchmark results to a JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Results saved to: {filepath}")
        except Exception as e:
            print(f"Error saving results: {e}")
    
    def load_config(self, filepath: str) -> pynlm.Config:
        """Load configuration from a file."""
        try:
            config = pynlm.create_default_config()
            config.load_from_file(filepath)
            print(f"Configuration loaded from: {filepath}")
            return config
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return self.create_default_config()
    
    def create_default_config(self) -> pynlm.Config:
        """Create a default configuration."""
        return self.create_config()
    
    def compare_configurations(self, configs: List[pynlm.Config]) -> Dict[str, Any]:
        """Compare multiple configurations."""
        comparison = {}
        
        for i, config in enumerate(configs):
            keys = config.get_keys()
            comparison[f"config_{i}"] = {
                "key_count": len(keys),
                "keys": keys,
                "has_neuron_count": config.has("brain.neuron_count"),
                "has_region_count": config.has("brain.region_count")
            }
        
        return comparison
    
    def analyze_performance(self, brain: pynlm.Brain, steps: int = 1000) -> Dict[str, Any]:
        """Analyze brain performance over time."""
        print(f"Analyzing brain performance over {steps} steps...")
        
        performance_data = {
            "steps": [],
            "firing_neurons": [],
            "firing_rates": [],
            "e_i_ratios": [],
            "total_spikes": []
        }
        
        for i in range(steps):
            brain.step(i)
            
            performance_data["steps"].append(i)
            performance_data["firing_neurons"].append(brain.get_firing_neuron_count())
            performance_data["firing_rates"].append(brain.get_average_firing_rate())
            performance_data["e_i_ratios"].append(brain.get_excitation_inhibition_ratio())
            performance_data["total_spikes"].append(brain.get_total_spike_count())
            
            if (i + 1) % 200 == 0:
                print(f"  Processed {i + 1}/{steps} steps")
        
        # Calculate statistics
        firing_stats = {
            "mean": sum(performance_data["firing_neurons"]) / len(performance_data["firing_neurons"]),
            "std": (sum((x - sum(performance_data["firing_neurons"]) / len(performance_data["firing_neurons"])) ** 2 
                    for x in performance_data["firing_neurons"]) / len(performance_data["firing_neurons"])) ** 0.5,
            "min": min(performance_data["firing_neurons"]),
            "max": max(performance_data["firing_neurons"])
        }
        
        return {
            "performance_data": performance_data,
            "firing_statistics": firing_stats,
            "final_results": {
                "total_spikes": brain.get_total_spike_count(),
                "average_firing_rate": brain.get_average_firing_rate(),
                "e_i_ratio": brain.get_excitation_inhibition_ratio()
            }
        }
    
    def create_visualization_script(self, brain: pynlm.Brain, steps: int = 100) -> str:
        """Create a Python script for visualizing brain activity."""
        script = """#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import pynlm

# Create brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
firing_counts = []
steps = """ + str(steps) + """

for i in range(steps):
    brain.step(i)
    firing_counts.append(brain.get_firing_neuron_count())

# Create visualization
plt.figure(figsize=(12, 8))

# Plot 1: Firing neuron count over time
plt.subplot(2, 2, 1)
plt.plot(firing_counts)
plt.title("Firing Neuron Count Over Time")
plt.xlabel("Step")
plt.ylabel("Firing Neurons")
plt.grid(True)

# Plot 2: Synaptic weight distribution
plt.subplot(2, 2, 2)
synapse_count = brain.get_synapse_count()
if synapse_count > 0:
    weights = np.random.random(synapse_count) * 2 - 1  # Random distribution
    plt.hist(weights, bins=50, alpha=0.7, edgecolor='black')
plt.title("Synaptic Weight Distribution")
plt.xlabel("Weight")
plt.ylabel("Frequency")
plt.grid(True)

# Plot 3: Development stage
plt.subplot(2, 2, 3)
development_stage = static_cast[int](brain.get_developmental_stage())
plt.text(0.5, 0.5, f"Development Stage: {development_stage}", 
         fontsize=14, ha='center', va='center',
         bbox=dict(boxstyle="round", facecolor="lightblue", alpha=0.8))
plt.title("Current Development Stage")
plt.axis('off')

# Plot 4: Network statistics
plt.subplot(2, 2, 4)
stats_text = f"Neurons: {brain.get_total_neuron_count()}\n"
stats_text += f"Synapses: {brain.get_total_synapse_count()}\n"
stats_text += f"Firing: {brain.get_firing_neuron_count()}\n"
stats_text += f"Avg Firing Rate: {brain.get_average_firing_rate():.2f} Hz\n"
stats_text += f"E/I Ratio: {brain.get_excitation_inhibition_ratio():.2f}"
plt.text(0.1, 0.5, stats_text, fontsize=10, fontfamily='monospace')
plt.title("Network Statistics")
plt.axis('off')

plt.tight_layout()
plt.savefig('brain_visualization.png', dpi=150, bbox_inches='tight')
plt.show()

print("Visualization saved as 'brain_visualization.png'")
"""
        return script
    
    def create_comprehensive_demo(self) -> Dict[str, Any]:
        """Create a comprehensive demonstration of all features."""
        print("Creating comprehensive NLM demonstration...")
        
        # Step 1: Create advanced configuration
        config = self.create_research_config(neurons=3000, regions=4)
        
        # Step 2: Create brain and world
        brain = self.create_brain(config)
        world = self.create_world(40, 40, 16, 16)
        
        # Step 3: Create and initialize agent
        agent = self.create_agent(brain, world)
        
        # Step 4: Enable all features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        # Step 5: Run a complex episode
        results = self.run_agent_episode(agent, world, 300, verbose=True)
        
        # Step 6: Save brain state
        self.save_brain(brain, "demo_brain.bin")
        
        # Step 7: Create visualization script
        viz_script = self.create_visualization_script(brain, 200)
        with open("visualize_brain.py", "w") as f:
            f.write(viz_script)
        
        return {
            "config": {
                "neurons": config.get_int("brain.neuron_count", 0),
                "regions": config.get_int("brain.region_count", 0),
                "connection_probability": config.get_float("brain.connection_probability", 0.0)
            },
            "simulation_results": results,
            "visualization_script": "visualize_brain.py",
            "brain_state": "demo_brain.bin"
        }
    
    def show_help(self):
        """Display help information."""
        help_text = """
NLM Command Line Interface - Help

Available commands:

Basic Operations:
  --create-brain [neurons] [regions]    Create a brain with specified neurons and regions
  --create-world [width] [height] [vision_width] [vision_height]  Create a world
  --create-agent                                              Create and initialize an agent
  --run-simulation [steps] [verbose]         Run simulation
  --run-episode [steps] [verbose]           Run agent episode
  --save-brain [filepath]                  Save brain state
  --load-brain [filepath]                  Load brain state

Configuration:
  --create-config [neurons] [regions] [conn_prob]    Create configuration
  --create-research-config                 Create research-oriented configuration
  --load-config [filepath]                Load configuration from file
  --compare-configs [configs]             Compare multiple configurations

Research:
  --run-research [steps]                 Run research simulation
  --run-agent-research [steps] [world_size]  Run agent research
  --benchmark [count] [steps]            Benchmark multiple agents
  --analyze-performance [steps]           Analyze brain performance over time

Advanced:
  --save-results [filepath]              Save benchmark results
  --create-visualization-script [steps]  Create visualization script
  --create-comprehensive-demo             Create comprehensive demonstration

Examples:
  nlm_cli --create-brain 1000 2
  nlm_cli --create-world 20 20 8 8
  nlm_cli --create-agent
  nlm_cli --run-episode 500 true
  nlm_cli --create-research-config
  nlm_cli --run-research 1000
  nlm_cli --benchmark 5 100

For more help on specific commands, use:
  nlm_cli --help [command_name]
"""
        print(help_text)
    
    def show_command_help(self, command: str):
        """Show help for a specific command."""
        help_text = f"""
{command.upper()} Command Help

This command is used to {command} in the NLM simulation.

Usage: nlm_cli {command} [options]

Available options:
  --help                                Show this help message
  --verbose                             Enable verbose output

For more information, visit the NLM documentation.
"""
        print(help_text)

def main():
    """Main entry point for the NLM CLI."""
    if len(sys.argv) == 1:
        print("Usage: nlm_cli <command> [options]")
        print("Type 'nlm_cli help' for more information.")
        return 1
    
    command = sys.argv[1].lower()
    
    cli = NLMCLI()
    
    try:
        if command == "help":
            if len(sys.argv) > 2:
                cli.show_command_help(sys.argv[2])
            else:
                cli.show_help()
        
        elif command == "create-brain":
            neurons = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
            regions = int(sys.argv[3]) if len(sys.argv) > 3 else 2
            verbose = "--verbose" in sys.argv
            
            config = cli.create_config(neurons, regions)
            brain = cli.create_brain(config)
            print(f"Created brain with {neurons} neurons and {regions} regions")
        
        elif command == "create-world":
            width = int(sys.argv[2]) if len(sys.argv) > 2 else 20
            height = int(sys.argv[3]) if len(sys.argv) > 3 else 20
            vision_width = int(sys.argv[4]) if len(sys.argv) > 4 else 8
            vision_height = int(sys.argv[5]) if len(sys.argv) > 5 else 8
            verbose = "--verbose" in sys.argv
            
            world = cli.create_world(width, height, vision_width, vision_height)
            print(f"Created world with dimensions {width}x{height}")
        
        elif command == "create-agent":
            # This command requires brain and world to be created first
            print("Please create a brain and world first, then create an agent")
            print("Usage: nlm_cli create-agent <brain_file> <world_file>")
        
        elif command == "run-simulation":
            steps = int(sys.argv[2]) if len(sys.argv) > 2 else 100
            verbose = "--verbose" in sys.argv
            
            # This command requires a brain object
            print("Please create a brain first, then run simulation")
            print("Usage: nlm_cli run-simulation <steps> [--verbose]")
        
        elif command == "run-episode":
            steps = int(sys.argv[2]) if len(sys.argv) > 2 else 100
            verbose = "--verbose" in sys.argv
            
            # This command requires an agent and world
            print("Please create an agent and world first, then run episode")
            print("Usage: nlm_cli run-episode <steps> [--verbose]")
        
        elif command == "save-brain":
            filepath = sys.argv[2] if len(sys.argv) > 2 else "brain.bin"
            print("Please create a brain first, then save it")
            print("Usage: nlm_cli save-brain <filepath>")
        
        elif command == "load-brain":
            filepath = sys.argv[2] if len(sys.argv) > 2 else "brain.bin"
            print("Please create a brain first, then load it")
            print("Usage: nlm_cli load-brain <filepath>")
        
        elif command == "create-config":
            neurons = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
            regions = int(sys.argv[3]) if len(sys.argv) > 3 else 2
            conn_prob = float(sys.argv[4]) if len(sys.argv) > 4 else 0.1
            
            config = cli.create_config(neurons, regions, conn_prob)
            print(f"Created configuration with {neurons} neurons, {regions} regions, {conn_prob} connection probability")
        
        elif command == "create-research-config":
            config = cli.create_research_config()
            print("Created research configuration")
        
        elif command == "load-config":
            filepath = sys.argv[2] if len(sys.argv) > 2 else "config.cfg"
            config = cli.load_config(filepath)
            print(f"Loaded configuration from {filepath}")
        
        elif command == "run-research":
            steps = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
            result = cli.run_research_simulation(None, steps)
            print(f"Research simulation completed in {result['duration_seconds']:.2f} seconds")
            print(f"  - Total spikes: {result['total_spikes']}")
            print(f"  - Average firing rate: {result['average_firing_rate']:.2f} Hz")
        
        elif command == "run-agent-research":
            steps = int(sys.argv[2]) if len(sys.argv) > 2 else 500
            world_size = int(sys.argv[3]) if len(sys.argv) > 3 else 30
            result = cli.run_agent_research(None, world_size, world_size, steps)
            print(f"Agent research completed in {result['duration_seconds']:.2f} seconds")
            print(f"  - Final curiosity: {result['final_curiosity']:.3f}")
            print(f"  - Final novelty: {result['final_novelty']:.3f}")
        
        elif command == "benchmark":
            count = int(sys.argv[2]) if len(sys.argv) > 2 else 5
            steps = int(sys.argv[3]) if len(sys.argv) > 3 else 100
            results = cli.benchmark_agents(count, steps)
            cli.save_results(results)
            print(f"Benchmark completed with {count} agents")
            print(f"Results saved to results.json")
        
        elif command == "analyze-performance":
            steps = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
            print("Performance analysis requires a brain object")
        
        elif command == "create-visualization-script":
            steps = int(sys.argv[2]) if len(sys.argv) > 2 else 100
            script = cli.create_visualization_script(None, steps)
            with open("brain_visualization.py", "w") as f:
                f.write(script)
            print(f"Visualization script created: brain_visualization.py")
        
        elif command == "create-comprehensive-demo":
            demo = cli.create_comprehensive_demo()
            print(f"Comprehensive demo created successfully!")
            print(f"  - Configuration: {demo['config']}")
            print(f"  - Visualization script: {demo['visualization_script']}")
            print(f"  - Brain state: {demo['brain_state']}")
        
        elif command == "version":
            print("NLM Command Line Interface - Version 1.0.0")
        
        else:
            print(f"Unknown command: {command}")
            print("Type 'nlm_cli help' for a list of available commands")
            return 1
        
        return 0
        
    except Exception as e:
        print(f"Error: {e}")
        return 1
if __name__ == "__main__":
    sys.exit(main())"