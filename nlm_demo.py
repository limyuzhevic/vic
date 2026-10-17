#!/usr/bin/env python3
"""
NLM Demo System - Simplified and Modular

This is a simplified, modular demo system for the NLM neural simulation framework.
It provides an easy-to-use interface for demonstrating NLM capabilities without
complex configuration or setup requirements.

Features:
- Modular demo components (Basic, Intermediate, Advanced)
- Command-line interface for easy execution
- Consistent output format and metrics
- Automatic cleanup and error handling
- Support for both scripted and interactive usage

Usage:
    python3 nlm_demo.py --mode basic [--steps 100] [--neurons 500]
    python3 nlm_demo.py --mode intermediate --episodes 10
    python3 nlm_demo.py --mode advanced --config custom_config.json
    python3 nlm_demo.py --help  # Show all options
"""

import sys
import time
import json
import argparse
from typing import Dict, Any, Optional, List
from enum import Enum

# Try to import NLM bindings
try:
    import pynlm
    NLM_AVAILABLE = True
    print("✓ NLM Python bindings loaded successfully")
except ImportError as e:
    NLM_AVAILABLE = False
    print(f"✗ NLM Python bindings not available: {e}")
    print("Please build NLM with Python bindings first.")
    sys.exit(1)

class DemoMode(Enum):
    BASIC = "basic"
    INTERMEDIATE = "intermediate"
    ADVANCED = "advanced"
    WORLD_INTERACTION = "world"
    LEARNING_DEMO = "learning"

class NLMDemo:
    """Main demo manager for NLM demonstrations"""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or self._get_default_config()
        self.brain = None
        self.world = None
        self.agent_brain = None
        self.demo_start_time = time.time()
        self.metrics = {}
        
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default configuration for demos"""
        return {
            "neuron_count": 500,
            "region_count": 2,
            "connection_probability": 0.15,
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "enable_plasticity": True,
            "enable_neuromodulation": True,
            "enable_development": True,
            "enable_curiosity": True,
            "world_width": 64,
            "world_height": 64,
            "vision_width": 16,
            "vision_height": 16,
        }
    
    def _update_config(self, updates: Dict[str, Any]) -> None:
        """Update configuration with provided values"""
        self.config.update(updates)
    
    def create_brain(self) -> None:
        """Create and initialize the NLM brain"""
        print(f"Creating brain with {self.config['neuron_count']} neurons...")
        
        # Create default config and customize
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", self.config["neuron_count"], pynlm.ConfigSource.Runtime)
        config.set("region_count", self.config["region_count"], pynlm.ConfigSource.Runtime)
        config.set("connection_probability", self.config["connection_probability"], 
                   pynlm.ConfigSource.Runtime)
        config.set("random_seed", self.config["random_seed"], pynlm.ConfigSource.Runtime)
        config.set("simulation_timestep", self.config["simulation_timestep"], 
                   pynlm.ConfigSource.Runtime)
        
        # Enable plasticity if requested
        if self.config["enable_plasticity"]:
            config.set("stdp_enabled", True, pynlm.ConfigSource.Runtime)
            config.set("hebbian_enabled", True, pynlm.ConfigSource.Runtime)
        
        self.brain = pynlm.createBrain(config)
        
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        print(f"✓ Brain initialized with {self.brain.getTotalNeuronCount()} neurons")
    
    def create_world(self) -> None:
        """Create and configure the simulation world"""
        print(f"Creating world ({self.config['world_width']}x{self.config['world_height']})...")
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            self.config["world_width"],
            self.config["world_height"],
            self.config["vision_width"],
            self.config["vision_height"]
        )
        
        # Set random seed for reproducible world behavior
        self.world.setRandomSeed(self.config["random_seed"])
        
        # Add some initial objects to the world
        self._populate_world()
        
        print("✓ World created with initial objects")
    
    def _populate_world(self) -> None:
        """Add initial objects to the world"""
        if not self.world:
            return
        
        # Add a resource (food)
        resource = pynlm.WorldObject()
        resource.x = self.config["world_width"] * 0.3
        resource.y = self.config["world_height"] * 0.3
        resource.type = pynlm.WorldObjectType.Resource
        resource.value = 10.0
        self.world.addObject(resource)
        
        # Add another resource
        resource2 = pynlm.WorldObject()
        resource2.x = self.config["world_width"] * 0.7
        resource2.y = self.config["world_height"] * 0.7
        resource2.type = pynlm.WorldObjectType.Resource
        resource2.value = 15.0
        self.world.addObject(resource2)
        
        # Add a hazard (wall)
        wall = pynlm.WorldObject()
        wall.x = self.config["world_width"] * 0.5
        wall.y = self.config["world_height"] * 0.5
        wall.type = pynlm.WorldObjectType.Wall
        wall.radius = 2.0
        self.world.addObject(wall)
    
    def create_agent(self) -> None:
        """Create and initialize the agent brain interface"""
        if not self.brain or not self.world:
            raise RuntimeError("Brain and world must be created first")
        
        print("Creating agent brain interface...")
        
        self.agent_brain = pynlm.createAgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
        
        # Enable systems based on configuration
        if self.config["enable_neuromodulation"]:
            self.agent_brain.enableRewardModulation(True)
        
        if self.config["enable_plasticity"]:
            self.agent_brain.enableStructuralPlasticity(True)
        
        if self.config["enable_development"]:
            self.agent_brain.enableDevelopment(True)
        
        if self.config["enable_curiosity"]:
            self.agent_brain.enableCuriosity(True)
        
        print("✓ Agent brain interface created and configured")
    
    def run_simulation(self, steps: int, timestep: float = 0.01) -> Dict[str, Any]:
        """Run a simulation with given parameters"""
        if not self.agent_brain or not self.world:
            raise RuntimeError("Agent and world must be created first")
        
        print(f"Running simulation for {steps} steps with {timestep}s timestep...")
        
        start_time = time.time()
        
        # Track metrics
        metrics = {
            "steps": steps,
            "start_time": start_time,
            "total_spikes": self.brain.getTotalSpikeCount(),
            "initial_spikes": self.brain.getTotalSpikeCount(),
            "final_spikes": 0,
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "spike_rate": 0.0,
            "max_spikes": 0,
            "reward": 0.0,
            "successful_actions": 0,
        }
        
        for step in range(steps):
            # 1. Get sensory input from world
            percept = self.world.getSensoryPercept()
            
            # 2. Process sensory input through agent brain
            self.agent_brain.processSensoryInput(percept)
            
            # 3. Run neural simulation (multiple steps per action)
            for neural_step in range(10):
                self.brain.step(neural_step, neural_step * self.config["simulation_timestep"])
                
                # Inject current to drive neural activity
                if neural_step % 5 == 0:  # Every 5th neural step
                    self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
            
            # 4. Decode motor command from brain activity
            action = self.brain.produceAction()
            if action:
                metrics["successful_actions"] += 1
                # Apply reward modulation (simplified)
                self.agent_brain.applyRewardModulation(1.0, 0.5)
            
            # 5. Update development
            self.agent_brain.updateDevelopment(timestep)
            
            # Track metrics every 10 steps
            if step % 10 == 0:
                current_spikes = self.brain.getTotalSpikeCount()
                metrics["max_spikes"] = max(metrics["max_spikes"], current_spikes)
            
            # Update world
            self.world.update(timestep)
            
            # Log progress every 25% of steps
            if (step + 1) % (steps // 4) == 0:
                elapsed = time.time() - start_time
                progress = (step + 1) / steps * 100
                print(f"  Progress: {progress:.0f}% ({step + 1}/{steps}) "
                      f"elapsed: {elapsed:.1f}s")
        
        end_time = time.time()
        metrics["end_time"] = end_time
        metrics["duration"] = end_time - start_time
        metrics["final_spikes"] = self.brain.getTotalSpikeCount()
        metrics["total_spikes"] = metrics["final_spikes"] - metrics["initial_spikes"]
        
        if metrics["duration"] > 0:
            metrics["spike_rate"] = metrics["total_spikes"] / metrics["duration"]
        
        print(f"✓ Simulation completed in {metrics['duration']:.2f} seconds")
        print(f"  Total spikes generated: {metrics['total_spikes']}")
        print(f"  Spike rate: {metrics['spike_rate']:.1f} spikes/second")
        print(f"  Successful actions: {metrics['successful_actions']}")
        
        return metrics
    
    def run_learning_demo(self, episodes: int, steps_per_episode: int) -> Dict[str, Any]:
        """Run a learning demonstration across multiple episodes"""
        if not self.agent_brain or not self.world:
            raise RuntimeError("Agent and world must be created first")
        
        print(f"Running learning demo for {episodes} episodes, {steps_per_episode} steps each...")
        
        episode_metrics = []
        
        for episode in range(episodes):
            print(f"\n--- Episode {episode + 1}/{episodes} ---")
            
            # Reset for new episode
            self.world.reset()
            self.agent_brain.reset()
            self.brain.reset()
            self.brain.initialize()
            
            # Re-populate world with objects
            self._populate_world()
            
            episode_start_time = time.time()
            episode_spikes = self.brain.getTotalSpikeCount()
            episode_reward = 0.0
            
            for step in range(steps_per_episode):
                # Get sensory percept
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent_brain.processSensoryInput(percept)
                
                # Run neural simulation
                for neural_step in range(5):
                    self.brain.step(neural_step, neural_step * self.config["simulation_timestep"])
                
                # Produce and apply action
                action = self.brain.produceAction()
                if action:
                    # Simple reward based on action type
                    episode_reward += 1.0
                
                # Update development
                self.agent_brain.updateDevelopment(0.01)
                
                # Update world
                self.world.update(0.01)
            
            episode_end_time = time.time()
            episode_duration = episode_end_time - episode_start_time
            
            # Collect episode metrics
            episode_metric = {
                "episode": episode + 1,
                "steps": steps_per_episode,
                "duration": episode_duration,
                "reward": episode_reward,
                "spikes": self.brain.getTotalSpikeCount() - episode_spikes,
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "average_firing_rate": self.brain.getAverageFiringRate(),
            }
            
            episode_metrics.append(episode_metric)
            
            print(f"  Episode completed in {episode_duration:.2f}s")
            print(f"  Reward: {episode_reward:.1f}")
            print(f"  Spikes: {episode_metric['spikes']}")
        
        # Calculate overall learning metrics
        total_reward = sum(m["reward"] for m in episode_metrics)
        total_spikes = sum(m["spikes"] for m in episode_metrics)
        avg_firing_rate = sum(m["average_firing_rate"] for m in episode_metrics) / len(episode_metrics)
        
        learning_metrics = {
            "episodes": episodes,
            "steps_per_episode": steps_per_episode,
            "total_reward": total_reward,
            "average_reward_per_episode": total_reward / episodes,
            "total_spikes": total_spikes,
            "average_spikes_per_episode": total_spikes / episodes,
            "average_firing_rate": avg_firing_rate,
            "episode_metrics": episode_metrics,
        }
        
        print(f"\nLearning demo completed!")
        print(f"Total reward: {total_reward:.1f}")
        print(f"Average reward per episode: {total_reward / episodes:.1f}")
        print(f"Average firing rate: {avg_firing_rate:.3f} Hz")
        
        return learning_metrics
    
    def run_basic_demo(self, steps: int) -> Dict[str, Any]:
        """Run basic demonstration showing core NLM functionality"""
        print("NLM Basic Demo - Core Neural Simulation")
        print("=" * 50)
        
        # Create all components
        self.create_brain()
        self.create_agent()
        
        # Run simulation
        metrics = self.run_simulation(steps)
        
        # Show final brain status
        print("\nFinal Brain Status:")
        self.brain.logStatus()
        
        return metrics
    
    def run_intermediate_demo(self, steps: int) -> Dict[str, Any]:
        """Run intermediate demonstration with world interaction"""
        print("NLM Intermediate Demo - World Interaction")
        print("=" * 50)
        
        # Create all components
        self.create_brain()
        self.create_world()
        self.create_agent()
        
        # Run simulation
        metrics = self.run_simulation(steps)
        
        # Show final status
        print("\nFinal Status:")
        self.brain.logStatus()
        
        if self.world:
            body = self.world.getAgentBody()
            print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
            print(f"Agent energy: {body.energy:.1f}")
            print(f"Agent age: {body.age}")
        
        return metrics
    
    def run_advanced_demo(self, steps: int) -> Dict[str, Any]:
        """Run advanced demonstration with learning"""
        print("NLM Advanced Demo - Learning and Development")
        print("=" * 50)
        
        # Create all components
        self.create_brain()
        self.create_world()
        self.create_agent()
        
        # Run learning demonstration
        metrics = self.run_learning_demo(5, steps // 5)
        
        # Show comprehensive analysis
        print("\nComprehensive Analysis:")
        self.brain.logStatus()
        
        # Analyze learning progress
        if metrics["episode_metrics"]:
            episode_rewards = [m["reward"] for m in metrics["episode_metrics"]]
            learning_progress = (episode_rewards[-1] - episode_rewards[0]) / max(episode_rewards[0], 1.0) * 100
            
            print(f"\nLearning Progress:")
            print(f"  Episode 1 reward: {episode_rewards[0]:.1f}")
            print(f"  Episode 5 reward: {episode_rewards[-1]:.1f}")
            print(f"  Improvement: {learning_progress:.1f}%")
        
        return metrics
    
    def run_world_interaction_demo(self, episodes: int, steps_per_episode: int) -> Dict[str, Any]:
        """Run world interaction demonstration"""
        print("NLM World Interaction Demo")
        print("=" * 50)
        
        # Create components
        self.create_brain()
        self.create_world()
        self.create_agent()
        
        print(f"Starting world interaction demo: {episodes} episodes")
        print(f"Each episode: {steps_per_episode} steps")
        
        episode_metrics = []
        
        for episode in range(episodes):
            print(f"\nWorld Episode {episode + 1}/{episodes}:")
            
            # Reset
            self.world.reset()
            self.agent_brain.reset()
            self.brain.reset()
            self.brain.initialize()
            self._populate_world()
            
            episode_start = time.time()
            
            for step in range(steps_per_episode):
                # Get percept
                percept = self.world.getSensoryPercept()
                
                # Process and act
                self.agent_brain.processSensoryInput(percept)
                
                for neural_step in range(5):
                    self.brain.step(neural_step, neural_step * self.config["simulation_timestep"])
                
                action = self.brain.produceAction()
                self.agent_brain.updateDevelopment(0.01)
                self.world.update(0.01)
                
                if (step + 1) % (steps_per_episode // 4) == 0:
                    body = self.world.getAgentBody()
                    print(f"    Step {step + 1}: pos=({body.x:.1f},{body.y:.1f}), energy={body.energy:.1f}")
            
            episode_end = time.time()
            duration = episode_end - episode_start
            
            body = self.world.getAgentBody()
            episode_metric = {
                "episode": episode + 1,
                "duration": duration,
                "final_position": (body.x, body.y),
                "final_energy": body.energy,
                "final_age": body.age,
                "spikes": self.brain.getTotalSpikeCount(),
            }
            
            episode_metrics.append(episode_metric)
            print(f"  Completed in {duration:.1f}s, final position: ({body.x:.1f}, {body.y:.1f})")
        
        # Calculate summary
        avg_duration = sum(m["duration"] for m in episode_metrics) / len(episode_metrics)
        final_positions = [(m["final_position"][0], m["final_position"][1]) for m in episode_metrics]
        
        world_metrics = {
            "episodes": episodes,
            "steps_per_episode": steps_per_episode,
            "average_duration": avg_duration,
            "final_positions": final_positions,
            "episode_metrics": episode_metrics,
        }
        
        print(f"\nWorld interaction demo completed!")
        print(f"Average episode duration: {avg_duration:.1f}s")
        print(f"Position movement: {final_positions[0]} -> {final_positions[-1]}")
        
        return world_metrics
    
    def cleanup(self) -> None:
        """Clean up resources"""
        print("Cleaning up resources...")
        
        self.brain = None
        self.world = None
        self.agent_brain = None
        
        print("✓ Cleanup completed")


def main_basic(args: argparse.Namespace) -> Dict[str, Any]:
    """Run basic demo"""
    demo = NLMDemo()
    
    try:
        metrics = demo.run_basic_demo(args.steps)
        return metrics
    finally:
        demo.cleanup()


def main_intermediate(args: argparse.Namespace) -> Dict[str, Any]:
    """Run intermediate demo"""
    demo = NLMDemo()
    
    try:
        metrics = demo.run_intermediate_demo(args.steps)
        return metrics
    finally:
        demo.cleanup()


def main_advanced(args: argparse.Namespace) -> Dict[str, Any]:
    """Run advanced demo"""
    demo = NLMDemo()
    
    try:
        metrics = demo.run_advanced_demo(args.steps)
        return metrics
    finally:
        demo.cleanup()


def main_world(args: argparse.Namespace) -> Dict[str, Any]:
    """Run world interaction demo"""
    demo = NLMDemo()
    
    try:
        metrics = demo.run_world_interaction_demo(args.episodes, args.steps)
        return metrics
    finally:
        demo.cleanup()


def main(args: argparse.Namespace) -> Dict[str, Any]:
    """Main entry point"""
    # Map demo modes to functions
    demo_functions = {
        DemoMode.BASIC: main_basic,
        DemoMode.INTERMEDIATE: main_intermediate,
        DemoMode.ADVANCED: main_advanced,
        DemoMode.WORLD_INTERACTION: main_world,
        DemoMode.LEARNING_DEMO: main_advanced,  # Use advanced for learning demo
    }
    
    # Get the requested demo function
    mode = DemoMode(args.mode)
    demo_function = demo_functions[mode]
    
    print(f"Starting NLM demo: {mode.value}")
    print(f"Python version: {sys.version}")
    
    try:
        if mode == DemoMode.BASIC:
            return demo_function(args)
        elif mode == DemoMode.INTERMEDIATE:
            return demo_function(args)
        elif mode == DemoMode.ADVANCED:
            return demo_function(args)
        elif mode == DemoMode.WORLD_INTERACTION:
            return demo_function(args)
        else:
            return demo_function(args)
            
    except Exception as e:
        print(f"✗ Demo failed with error: {e}")
        import traceback
        traceback.print_exc()
        return {"error": str(e), "mode": mode.value}


def create_argument_parser() -> argparse.ArgumentParser:
    """Create and configure argument parser"""
    parser = argparse.ArgumentParser(
        description="NLM Demo System - Simplified neural simulation demonstrations",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    
    # Common arguments
    parser.add_argument(
        "--mode",
        choices=[mode.value for mode in DemoMode],
        default=DemoMode.BASIC.value,
        help="Demo mode to run"
    )
    
    parser.add_argument(
        "--steps",
        type=int,
        default=100,
        help="Number of simulation steps (for basic/intermediate modes)"
    )
    
    parser.add_argument(
        "--episodes",
        type=int,
        default=5,
        help="Number of episodes (for world/intermediate modes)"
    )
    
    parser.add_argument(
        "--config",
        type=str,
        help="Path to custom configuration JSON file"
    )
    
    parser.add_argument(
        "--neurons",
        type=int,
        help="Number of neurons (overrides config)"
    )
    
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    # Mode-specific help
    parser.epilog = """
Demo Modes:
  basic          - Simple neural simulation with basic features
  intermediate   - Adds world interaction and agent interface
  advanced       - Includes learning, development, and curiosity
  world          - Focus on agent-world interaction
  learning       - Demonstrates learning across multiple episodes

Examples:
  python3 nlm_demo.py --mode basic --steps 200
  python3 nlm_demo.py --mode intermediate --episodes 10
  python3 nlm_demo.py --mode advanced --steps 500
  python3 nlm_demo.py --mode world --episodes 5 --steps 100

For more information, visit: https://github.com/nlm-project/nlm
"""
    
    return parser


def main_entry_point() -> None:
    """Main entry point with argument parsing"""
    parser = create_argument_parser()
    
    try:
        args = parser.parse_args()
        
        # Load custom configuration if specified
        if args.config:
            try:
                with open(args.config, 'r') as f:
                    config = json.load(f)
                print(f"Loaded configuration from {args.config}")
            except Exception as e:
                print(f"Failed to load config file: {e}")
                sys.exit(1)
        else:
            config = None
        
        # Override config with command line arguments
        if args.neurons:
            if config is None:
                config = {}
            config["neuron_count"] = args.neurons
        
        # Run the demo
        metrics = main(args)
        
        # Print summary
        print(f"\n{'='*60}")
        print("DEMO SUMMARY")
        print(f"{'='*60}")
        
        if "error" in metrics:
            print(f"✗ Demo failed: {metrics['error']}")
            sys.exit(1)
        
        print(f"✓ Demo completed successfully!")
        print(f"Mode: {args.mode}")
        
        if "total_steps" in metrics:
            print(f"Total steps: {metrics['total_steps']}")
        
        if "episodes" in metrics:
            print(f"Episodes: {metrics['episodes']}")
        
        print(f"Duration: {metrics.get('duration', 'N/A'):.2f} seconds")
        
        # Save metrics to file
        if args.verbose:
            try:
                with open("demo_metrics.json", "w") as f:
                    json.dump(metrics, f, indent=2, default=str)
                print(f"Metrics saved to: demo_metrics.json")
            except Exception as e:
                print(f"Failed to save metrics: {e}")
        
        print(f"\n{'='*60}")
        print("For more information, visit: https://github.com/nlm-project/nlm")
        print(f"{'='*60}")
        
    except KeyboardInterrupt:
        print(f"\n⚠ Demo interrupted by user")
        sys.exit(130)
    except Exception as e:
        print(f"\n✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main_entry_point()