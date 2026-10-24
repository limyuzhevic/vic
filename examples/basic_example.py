#!/usr/bin/env python3
"""
NLM Basic Usage Example

This example demonstrates the fundamental usage patterns for the NLM (Neural Learning Machine)
framework, covering basic brain operations, memory systems, and simulation control.

Usage:
    python basic_example.py [--neurons N] [--steps S]

Example outputs are saved to 'basic_example_output/' for analysis.
"""

import sys
import os
import argparse
from pathlib import Path

# Add current directory to path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
except ImportError:
    print("ERROR: pynlm module not found. Please install it first.")
    print("Run: pip install pynlm")
    sys.exit(1)

class NLMExample:
    """Basic NLM usage example with comprehensive functionality."""
    
    def __init__(self, neuron_count=500, region_count=1, connection_prob=0.1):
        """Initialize the example with basic configuration."""
        self.neuron_count = neuron_count
        self.region_count = region_count
        self.connection_prob = connection_prob
        self.output_dir = Path("basic_example_output")
        self.output_dir.mkdir(exist_ok=True)
        
        # Initialize components
        self.brain = None
        self.world = None
        self.agent = None
        self.experiment = None
        
    def setup_brain(self):
        """Create and initialize the brain."""
        print(f"Creating brain with {self.neuron_count} neurons...")
        
        # Create brain with custom configuration
        self.brain = pynlm.Brain(
            neuron_count=self.neuron_count,
            region_count=self.region_count,
            connection_prob=self.connection_prob
        )
        
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        print(f"✓ Brain initialized successfully")
        print(f"  - Total neurons: {self.brain.get_total_neuron_count()}")
        print(f"  - Total synapses: {self.brain.get_total_synapse_count()}")
        
        return self.brain
    
    def setup_world(self):
        """Create and initialize the environment world."""
        print("Creating environment world...")
        
        self.world = pynlm.World()
        self.world.initialize(16, 16)  # 16x16 grid world
        
        print(f"✓ World initialized successfully")
        print(f"  - World size: {self.world.getWidth()}x{self.world.getHeight()}")
        print(f"  - Environment: {self.world.get_environment_name()}")
        
        return self.world
    
    def setup_agent(self):
        """Create and initialize the agent."""
        if not self.brain:
            raise RuntimeError("Brain must be initialized first")
            
        print("Creating agent...")
        
        self.agent = pynlm.Agent(self.brain)
        self.agent.initialize(self.world)
        
        # Enable various agent features
        self.agent.enable_reward_modulation(True)
        self.agent.enable_structural_plasticity(True)
        self.agent.enable_development(True)
        self.agent.enable_curiosity(True)
        
        print(f"✓ Agent initialized successfully")
        print(f"  - Reward modulation: enabled")
        print(f"  - Structural plasticity: enabled")
        print(f"  - Development: enabled")
        print(f"  - Curiosity: enabled")
        
        return self.agent
    
    def run_simulation(self, steps):
        """Run the basic simulation loop."""
        print(f"\nRunning simulation for {steps} steps...")
        
        # Performance tracking
        total_reward = 0.0
        total_firing_rate = 0.0
        spike_counts = []
        
        for step in range(steps):
            # Get observation from world
            percept = self.agent.observe()
            
            # Process sensory input
            self.agent.process_sensory_input(percept)
            
            # Execute brain step
            self.brain.step(step, step * 0.001)
            
            # Get motor command
            cmd = self.agent.decode_motor_command()
            
            # Apply action to world
            self.world.apply_action(self.brain.get_regions()[0], cmd)
            
            # Compute reward
            reward = self.world.compute_reward(self.brain.get_regions()[0])
            total_reward += reward
            
            # Apply reward modulation
            self.agent.apply_reward_modulation(reward, 0.0)
            
            # Update development
            self.agent.update_development(0.001)
            
            # Collect metrics
            total_firing_rate += self.brain.get_average_firing_rate()
            spike_counts.append(self.brain.get_total_spike_count())
            
            # Print progress every 10% of steps
            if step % max(1, steps // 10) == 0:
                print(f"  Step {step}/{steps}: "
                      f"Reward={reward:.3f}, "
                      f"Firing={self.brain.get_average_firing_rate():.2f}Hz, "
                      f"Spikes={self.brain.get_total_spike_count()}")
        
        # Calculate average metrics
        avg_firing_rate = total_firing_rate / steps if steps > 0 else 0.0
        avg_reward = total_reward / steps if steps > 0 else 0.0
        
        print(f"✓ Simulation completed")
        print(f"  - Total reward: {total_reward:.3f}")
        print(f"  - Average reward: {avg_reward:.3f}")
        print(f"  - Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"  - Final spike count: {spike_counts[-1] if spike_counts else 0}")
        
        return {
            'total_reward': total_reward,
            'avg_reward': avg_reward,
            'avg_firing_rate': avg_firing_rate,
            'final_spike_count': spike_counts[-1] if spike_counts else 0,
            'spike_counts': spike_counts
        }
    
    def analyze_memory_systems(self):
        """Analyze and display memory system status."""
        print(f"\nAnalyzing memory systems...")
        
        # Working memory analysis
        working_memory = self.brain.get_working_memory()
        if working_memory:
            active_traces = working_memory.getActiveTraces()
            print(f"Working Memory: {active_traces} active traces")
            
            # Export working memory state
            with open(self.output_dir / "working_memory.txt", "w") as f:
                f.write(f"Working Memory State\n")
                f.write(f"Active Traces: {active_traces}\n")
        else:
            print(f"Working Memory: Not available")
        
        # Episodic memory analysis
        episodic_memory = self.brain.get_episodic_memory()
        if episodic_memory:
            episode_count = episodic_memory.getEpisodeCount()
            print(f"Episodic Memory: {episode_count} episodes stored")
            
            # Export episodic memory state
            with open(self.output_dir / "episodic_memory.txt", "w") as f:
                f.write(f"Episodic Memory State\n")
                f.write(f"Episode Count: {episode_count}\n")
                
                # Get sample episodes for analysis
                if episode_count > 0:
                    episodes = episodic_memory.getEpisodesForReplay(min(3, episode_count))
                    f.write(f"Sample Episodes Available: {len(episodes)}\n")
        else:
            print(f"Episodic Memory: Not available")
        
        # Semantic memory analysis
        semantic_memory = self.brain.get_semantic_memory()
        if semantic_memory:
            concepts = semantic_memory.getConceptCount()
            print(f"Semantic Memory: {concepts} concepts formed")
        
        # Procedural memory analysis
        procedural_memory = self.brain.get_procedural_memory()
        if procedural_memory:
            skills = procedural_memory.getSkillCount()
            print(f"Procedural Memory: {skills} skills learned")
        
        # Associative memory analysis
        associative_memory = self.brain.get_associative_memory()
        if associative_memory:
            associations = associative_memory.getAssociationCount()
            print(f"Associative Memory: {associations} associations formed")
        
        return {
            'working_memory_traces': working_memory.getActiveTraces() if working_memory else 0,
            'episodic_memory_episodes': episodic_memory.getEpisodeCount() if episodic_memory else 0,
            'semantic_memory_concepts': semantic_memory.getConceptCount() if semantic_memory else 0,
            'procedural_memory_skills': procedural_memory.getSkillCount() if procedural_memory else 0,
            'associative_memory_associations': associative_memory.getAssociationCount() if associative_memory else 0
        }
    
    def analyze_neuromodulation(self):
        """Analyze neuromodulation system status."""
        print(f"\nAnalyzing neuromodulation systems...")
        
        # Dopamine system
        dopamine = self.brain.get_dopamine()
        if dopamine:
            dopamine_level = dopamine.getLevel()
            prediction_error = dopamine.getPredictionError()
            print(f"Dopamine System: Level={dopamine_level:.3f}, Prediction Error={prediction_error:.3f}")
        
        # Curiosity system
        curiosity = self.brain.get_curiosity()
        if curiosity:
            curiosity_level = curiosity.getLevel()
            exploration_drive = curiosity.getExplorationDrive()
            print(f"Curiosity System: Level={curiosity_level:.3f}, Exploration={exploration_drive:.3f}")
        
        # Novelty system
        novelty = self.brain.get_novelty()
        if novelty:
            novelty_level = novelty.getNoveltyLevel()
            print(f"Novelty System: Level={novelty_level:.3f}")
        
        # Reward system
        reward = self.brain.get_reward()
        if reward:
            reward_level = reward.getLevel()
            discount_factor = reward.getDiscountFactor()
            print(f"Reward System: Level={reward_level:.3f}, Discount={discount_factor:.3f}")
        
        return {
            'dopamine_level': dopamine.getLevel() if dopamine else 0.0,
            'curiosity_level': curiosity.getLevel() if curiosity else 0.0,
            'novelty_level': novelty.getNoveltyLevel() if novelty else 0.0,
            'reward_level': reward.getLevel() if reward else 0.0
        }
    
    def export_results(self, simulation_results, memory_results, neuromodulation_results):
        """Export all results to files for analysis."""
        print(f"\nExporting results to {self.output_dir}...")
        
        # Export simulation results
        with open(self.output_dir / "simulation_results.txt", "w") as f:
            f.write("Simulation Results\n")
            f.write("===================\n\n")
            f.write(f"Configuration:\n")
            f.write(f"  - Neurons: {self.neuron_count}\n")
            f.write(f"  - Regions: {self.region_count}\n")
            f.write(f"  - Connection Probability: {self.connection_prob}\n\n")
            
            f.write(f"Simulation Metrics:\n")
            f.write(f"  - Total Steps: {len(simulation_results['spike_counts'])}\n")
            f.write(f"  - Total Reward: {simulation_results['total_reward']:.3f}\n")
            f.write(f"  - Average Reward: {simulation_results['avg_reward']:.3f}\n")
            f.write(f"  - Average Firing Rate: {simulation_results['avg_firing_rate']:.2f} Hz\n")
            f.write(f"  - Final Spike Count: {simulation_results['final_spike_count']}\n")
        
        # Export memory results
        with open(self.output_dir / "memory_results.txt", "w") as f:
            f.write("Memory System Analysis\n")
            f.write("======================\n\n")
            f.write(f"Working Memory: {memory_results['working_memory_traces']} active traces\n")
            f.write(f"Episodic Memory: {memory_results['episodic_memory_episodes']} episodes\n")
            f.write(f"Semantic Memory: {memory_results['semantic_memory_concepts']} concepts\n")
            f.write(f"Procedural Memory: {memory_results['procedural_memory_skills']} skills\n")
            f.write(f"Associative Memory: {memory_results['associative_memory_associations']} associations\n")
        
        # Export neuromodulation results
        with open(self.output_dir / "neuromodulation_results.txt", "w") as f:
            f.write("Neuromodulation Analysis\n")
            f.write("=========================\n\n")
            f.write(f"Dopamine Level: {neuromodulation_results['dopamine_level']:.3f}\n")
            f.write(f"Curiosity Level: {neuromodulation_results['curiosity_level']:.3f}\n")
            f.write(f"Novelty Level: {neuromodulation_results['novelty_level']:.3f}\n")
            f.write(f"Reward Level: {neuromodulation_results['reward_level']:.3f}\n")
        
        # Create summary report
        with open(self.output_dir / "summary.txt", "w") as f:
            f.write("NLM Basic Example Summary\n")
            f.write("=========================\n\n")
            f.write(f"Generated at: {pynlm.get_version()}\n")
            f.write(f"Python bindings: Available\n")
            f.write(f"Simulation steps: {len(simulation_results['spike_counts'])}\n")
            f.write(f"Peak spike count: {max(simulation_results['spike_counts']) if simulation_results['spike_counts'] else 0}\n")
            f.write(f"Total active traces: {memory_results['working_memory_traces']}\n")
            f.write(f"Total episodes: {memory_results['episodic_memory_episodes']}\n")
        
        print(f"✓ Results exported successfully")
        
        # List exported files
        print(f"\nExported files:")
        for file in self.output_dir.glob("*.txt"):
            print(f"  - {file.name}")
    
    def run_complete_example(self, steps):
        """Run the complete example with all components."""
        print("=" * 60)
        print("NLM Basic Usage Example")
        print("=" * 60)
        
        try:
            # Setup all components
            self.setup_brain()
            self.setup_world()
            self.setup_agent()
            
            # Run simulation
            simulation_results = self.run_simulation(steps)
            
            # Analyze memory systems
            memory_results = self.analyze_memory_systems()
            
            # Analyze neuromodulation
            neuromodulation_results = self.analyze_neuromodulation()
            
            # Export results
            self.export_results(simulation_results, memory_results, neuromodulation_results)
            
            print(f"\n" + "=" * 60)
            print("Example completed successfully!")
            print("=" * 60)
            
            return True
            
        except Exception as e:
            print(f"\nERROR: Example failed with exception: {e}")
            import traceback
            traceback.print_exc()
            return False

def main():
    """Main function with command-line interface."""
    parser = argparse.ArgumentParser(
        description="NLM Basic Usage Example",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python basic_example.py                    # Run with default settings (500 neurons, 1000 steps)
  python basic_example.py --neurons 1000     # Run with 1000 neurons
  python basic_example.py --steps 5000       # Run for 5000 steps
  python basic_example.py --neurons 2000 --steps 2000  # Custom settings
        """
    )
    
    parser.add_argument(
        "--neurons", type=int, default=500,
        help="Number of neurons (default: 500)"
    )
    
    parser.add_argument(
        "--steps", type=int, default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    
    parser.add_argument(
        "--regions", type=int, default=1,
        help="Number of brain regions (default: 1)"
    )
    
    parser.add_argument(
        "--prob", type=float, default=0.1,
        help="Connection probability (default: 0.1)"
    )
    
    args = parser.parse_args()
    
    # Create and run example
    example = NLMExample(
        neuron_count=args.neurons,
        region_count=args.regions,
        connection_prob=args.prob
    )
    
    success = example.run_complete_example(args.steps)
    
    if success:
        print(f"\n✓ Example completed successfully!")
        print(f"Results are saved in 'basic_example_output/'")
        return 0
    else:
        print(f"\n✗ Example failed!")
        return 1

if __name__ == "__main__":
    exit(main())