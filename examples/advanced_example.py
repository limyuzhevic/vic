#!/usr/bin/env python3
"""
NLM Advanced Example: Complex Multi-Agent Learning Experiment

This example demonstrates advanced NLM usage including:
1. Multi-agent coordination and social learning
2. Complex environment interactions
3. Advanced memory management
4. Performance optimization and benchmarking
5. Custom experiment design

This example is suitable for:
- Researchers designing experiments
- Performance testing
- Advanced users wanting to push NLM capabilities
- Educational demonstrations of complex brain behaviors

Prerequisites:
- Python 3.8 or higher
- NLM Python bindings installed
- Advanced understanding of NLM concepts
- Familiarity with the intermediate tutorial
"""

import sys
import os
import time
import numpy as np
import threading
import multiprocessing
import json
from typing import Dict, List, Optional, Tuple

# Add the src directory to the path for direct imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from sensory.SensoryInput import SensoryInput
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
    from core.Logger.Logger import NLM_LOG_INFO, NLM_LOG_ERROR
except ImportError as e:
    print(f"Error importing NLM modules: {e}")
    print("Make sure NLM is properly built and the Python bindings are installed.")
    sys.exit(1)

class MultiAgentExperiment:
    """
    Complex multi-agent learning experiment with social interaction,
    coordination, and emergent behaviors.
    """
    
    def __init__(self, config_path: Optional[str] = None):
        self.config_path = config_path
        self.agents: List[AgentBrain] = []
        self.worlds: List[SimpleWorld] = []
        self.brains: List[Brain] = []
        self.experiment_config: Dict = {}
        self.results: Dict = {}
        self.lock = threading.Lock()
        
        print("Multi-Agent Experiment initialized")
    
    def create_experiment_config(self, 
                               neuron_count: int = 1000,
                               agent_count: int = 3,
                               world_size: int = 64,
                               duration: int = 100,
                               complexity: str = "high") -> Dict:
        """
        Create complex experiment configuration for multi-agent learning.
        """
        print(f"\nCreating experiment configuration for {agent_count} agents...")
        
        config = {
            "name": f"multi_agent_social_learning_{agent_count}_agents",
            "description": "Complex social learning experiment with multiple agents",
            "parameters": {
                "neuron_count": neuron_count,
                "agent_count": agent_count,
                "world_size": world_size,
                "duration": duration,
                "complexity": complexity,
                "seed": 123456789,
                "communication_enabled": True,
                "social_learning_enabled": True,
                "competition_enabled": True,
                "cooperation_enabled": True,
                "shared_memory": True,
                "environment_objects": 10,
                "resource_allocation": "balanced"
            },
            "performance": {
                "enable_vectorization": True,
                "enable_sparse_connectivity": True,
                "thread_count": min(agent_count, 8),
                "memory_pool_size_multiplier": 2.0,
                "checkpoint_interval": 50
            },
            "metrics": {
                "collect_neural_activity": True,
                "collect_behavioral_data": True,
                "collect_social_metrics": True,
                "collect_coordination_metrics": True,
                "collect_learning_metrics": True
            }
        }
        
        self.experiment_config = config
        
        print("  ✓ Experiment configuration created")
        print(f"  ✓ Configuration name: {config['name']}")
        print(f"  ✓ Number of agents: {config['parameters']['agent_count']}")
        print(f"  ✓ Duration: {config['parameters']['duration']} episodes")
        print(f"  ✓ Complexity: {config['parameters']['complexity']}")
        
        return config
    
    def create_agents(self):
        """
        Create multiple agents with varied configurations.
        """
        print(f"\nCreating {len(self.agents) + 1} agents...")
        
        params = self.experiment_config["parameters"]
        agent_count = params["agent_count"]
        
        for agent_id in range(agent_count):
            print(f"\nCreating Agent {agent_id + 1}/{agent_count}...")
            
            # Create different configurations for each agent
            config = Config()
            
            # Base configuration
            config.set("neuron_count", params["neuron_count"] // agent_count)
            config.set("plasticity_rate", 0.1f)
            config.set("working_memory_capacity", 100)
            config.set("prediction_horizon", 5)
            
            # Agent-specific variations
            if agent_id % 3 == 0:
                # "Fast" learner: high plasticity, short memory
                config.set("plasticity_rate", 0.2f)
                config.set("working_memory_capacity", 50)
                config.set("curiosity_enabled", True)
                print("    Configuration: Fast learner (high plasticity)")
            
            elif agent_id % 3 == 1:
                # "Careful" learner: low plasticity, long memory
                config.set("plasticity_rate", 0.05f)
                config.set("working_memory_capacity", 200)
                config.set("curiosity_enabled", False)
                print("    Configuration: Careful learner (low plasticity)")
            
            else:
                # "Balanced" learner: medium settings
                config.set("plasticity_rate", 0.1f)
                config.set("working_memory_capacity", 150)
                print("    Configuration: Balanced learner")
            
            # Add social learning features
            config.set("social_learning_enabled", True)
            config.set("novelty_threshold", 0.3f)
            
            # Create brain and agent
            brain = Brain(config)
            brain.initialize()
            self.brains.append(brain)
            
            # Create world for this agent
            world = SimpleWorld(params["world_size"], params["world_size"])
            world.initialize()
            self.worlds.append(world)
            
            # Create agent brain
            agent = AgentBrain(brain)
            agent.initialize(world)
            self.agents.append(agent)
            
            print(f"    ✓ Agent {agent_id + 1} created")
            print(f"      - Neurons: {brain.getTotalNeuronCount()}")
            print(f"      - Memory capacity: {config.get('working_memory_capacity')}")
            print(f"      - Plasticity rate: {config.get('plasticity_rate')}")
    
    def run_experiment_episode(self, episode_id: int) -> Dict:
        """
        Run a single episode of the multi-agent experiment.
        """
        print(f"\n--- Episode {episode_id + 1} ---")
        
        episode_results = {
            "episode": episode_id + 1,
            "start_time": time.time(),
            "agent_actions": [],
            "rewards": [],
            "neuromodulation_levels": [],
            "memory_usage": [],
            "prediction_accuracies": [],
            "social_interactions": 0,
            "coordination_events": 0
        }
        
        params = self.experiment_config["parameters"]
        complexity = params["complexity"]
        
        # Reset all agents and worlds
        for agent, world in zip(self.agents, self.worlds):
            world.reset()
            agent.reset()
        
        # Main episode loop
        for step in range(params["duration"] // len(self.agents)):  # Divide steps per agent
            print(f"  Step {step + 1}/{params['duration'] // len(self.agents)}")
            
            # Each agent takes turns (round-robin)
            for agent_idx, (agent, world, brain) in enumerate(zip(self.agents, self.worlds, self.brains)):
                
                # Create sensory input based on world state
                input_ = SensoryInput(10, 10)
                
                if complexity == "high":
                    # Create complex sensory patterns
                    for x in range(10):
                        for y in range(10):
                            # Pattern based on world state and agent position
                            world_x = (agent_idx * 7 + x) % params["world_size"]
                            world_y = (agent_idx * 11 + y) % params["world_size"]
                            
                            # Create meaningful sensory patterns
                            pixel_value = 0.0f
                            
                            # Agent sees its own position (center)
                            if x == 5 and y == 5:
                                pixel_value = 1.0f
                            
                            # See nearby objects
                            for i in range(3):
                                if world_x + i < params["world_size"] and world_y + i < params["world_size"]:
                                    # Simplified object detection
                                    pixel_value += 0.2f
                            
                            # Add agent-specific patterns
                            pixel_value += (agent_idx + 1) * 0.1f
                            pixel_value = min(pixel_value, 1.0f)
                            
                            input_.setPixel(x, y, pixel_value)
                
                else:
                    # Simpler patterns for medium complexity
                    for x in range(10):
                        for y in range(10):
                            pixel_value = float((x + y) % 2)  # Checkerboard pattern
                            pixel_value += 0.3f  # Base brightness
                            input_.setPixel(x, y, pixel_value)
                
                # Agent processes sensory input
                agent.processSensoryInput(input_)
                
                # Simulate neural processing
                for neural_step in range(5):  # 5 internal neural steps
                    brain.step(neural_step)
                
                # Agent produces action
                action = brain.produceAction()
                if action:
                    # Apply action to world
                    world.applyAction(*action)
                    
                    # Get rewards and other feedback
                    reward = world.getReward()
                    
                    # Track agent-specific metrics
                    neuromodulation = agent.getNeuromodulationLevel()
                    curiosity = agent.getCuriosityLevel()
                    novelty = agent.getNoveltyLevel()
                    prediction_error = agent.getPredictionError()
                    
                    # Store action and reward
                    episode_results["agent_actions"].append({
                        "agent_id": agent_idx,
                        "action": action.getType(),
                        "step": step
                    })
                    episode_results["rewards"].append(reward)
                    episode_results["neuromodulation_levels"].append({
                        "agent_id": agent_idx,
                        "dopamine": neuromodulation,
                        "curiosity": curiosity,
                        "novelty": novelty,
                        "prediction_error": prediction_error
                    })
                    
                    # Apply neuromodulation
                    predicted_reward = prediction_error * 0.5f
                    agent.applyRewardModulation(reward, predicted_reward)
                    
                    # Update development
                    agent.updateDevelopment(0.1f)
                    
                    print(f"    Agent {agent_idx + 1}: Action={action.getType()}, Reward={reward:.2f}")
                
                # Social interactions (every other step)
                if step % 2 == 0 and agent_idx < len(self.agents) - 1:
                    # Simple social interaction: observe nearby agent
                    nearby_agent = self.agents[agent_idx + 1]
                    other_neuromod = nearby_agent.getNeuromodulationLevel()
                    
                    # Social learning signal
                    social_signal = other_neuromod * 0.3f
                    episode_results["social_interactions"] += 1
                    print(f"    Social interaction: Agent {agent_idx + 1} observed Agent {agent_idx + 2}")
                
                # Coordination events (agent observes world state)
                if step % 10 == 0:
                    episode_results["coordination_events"] += 1
                    print(f"    Coordination event: Agent {agent_idx + 1} scanning environment")
        
        episode_results["end_time"] = time.time()
        episode_results["duration"] = episode_results["end_time"] - episode_results["start_time"]
        
        return episode_results
    
    def run_experiment(self, num_episodes: int = 10) -> Dict:
        """
        Run the complete multi-agent experiment.
        """
        print_header("Multi-Agent Learning Experiment")
        print(f"\nStarting experiment with {num_episodes} episodes...")
        print(f"Total duration: ~{num_episodes * 10} seconds")
        
        total_start_time = time.time()
        
        # Create all agents
        self.create_agents()
        
        # Run episodes
        all_episode_results = []
        for episode in range(num_episodes):
            episode_results = self.run_experiment_episode(episode)
            all_episode_results.append(episode_results)
            
            # Save intermediate results
            if (episode + 1) % 5 == 0:
                self.save_intermediate_results(all_episode_results)
        
        total_end_time = time.time()
        
        # Analyze and summarize results
        summary = self.analyze_results(all_episode_results, total_start_time, total_end_time)
        
        # Save final results
        self.save_final_results(summary, all_episode_results)
        
        return summary
    
    def analyze_results(self, episode_results: List[Dict], total_start: float, total_end: float) -> Dict:
        """
        Analyze experiment results and generate summary.
        """
        print_header("Experiment Results Analysis")
        
        summary = {
            "total_duration": total_end - total_start,
            "episodes_completed": len(episode_results),
            "agent_count": len(self.agents),
            "total_actions": 0,
            "total_rewards": 0.0,
            "average_reward_per_episode": 0.0,
            "social_interactions": 0,
            "coordination_events": 0,
            "performance_metrics": {},
            "agent_performance": {}
        }
        
        print("\n1. Analyzing agent performance...")
        
        # Calculate performance metrics for each agent
        for agent_idx in range(len(self.agents)):
            agent_actions = 0
            agent_rewards = 0.0
            agent_neuromod_levels = []
            
            for episode in episode_results:
                for action in episode["agent_actions"]:
                    if action["agent_id"] == agent_idx:
                        agent_actions += 1
                
                for reward in episode["rewards"]:
                    # Simplified reward attribution
                    agent_rewards += reward * 0.3  # Equal share per agent
                
                for neuromod in episode["neuromodulation_levels"]:
                    if neuromod["agent_id"] == agent_idx:
                        agent_neuromod_levels.append(neuromod["dopamine"])
            
            avg_neuromod = sum(agent_neuromod_levels) / len(agent_neuromod_levels) if agent_neuromod_levels else 0
            
            summary["agent_performance"][f"agent_{agent_idx + 1}"] = {
                "actions_taken": agent_actions,
                "total_rewards": agent_rewards,
                "average_reward": agent_rewards / agent_actions if agent_actions > 0 else 0,
                "average_neuromodulation": avg_neuromod,
                "steps_per_action": 10  # Approximate
            }
            
            summary["total_actions"] += agent_actions
            summary["total_rewards"] += agent_rewards
        
        summary["average_reward_per_episode"] = summary["total_rewards"] / len(episode_results) if episode_results else 0
        
        print(f"   ✓ Completed {len(episode_results)} episodes")
        print(f"   ✓ Total actions: {summary['total_actions']}")
        print(f"   ✓ Total rewards: {summary['total_rewards']:.2f}")
        print(f"   ✓ Average reward per episode: {summary['average_reward_per_episode']:.2f}")
        
        print("\n2. Analyzing social dynamics...")
        
        # Calculate social interaction metrics
        total_social = sum(episode.get("social_interactions", 0) for episode in episode_results)
        total_coordination = sum(episode.get("coordination_events", 0) for episode in episode_results)
        
        summary["social_interactions"] = total_social
        summary["coordination_events"] = total_coordination
        
        print(f"   ✓ Total social interactions: {total_social}")
        print(f"   ✓ Total coordination events: {total_coordination}")
        print(f"   ✓ Social interaction rate: {total_social / (len(episode_results) * 5):.2f} per step")
        
        print("\n3. Analyzing performance metrics...")
        
        # Performance metrics
        summary["performance_metrics"] = {
            "neural_activity": {
                "total_spikes": sum(brain.getTotalSpikeCount() for brain in self.brains),
                "average_firing_rate": sum(brain.getAverageFiringRate() for brain in self.brains) / len(self.brains) if self.brains else 0
            },
            "memory_usage": {
                "working_memory_entries": sum(agent.getWorkingMemory().getEntryCount() if agent.getWorkingMemory() else 0 for agent in self.agents),
                "episodic_memory_episodes": sum(agent.getEpisodicMemory().getEpisodeCount() if agent.getEpisodicMemory() else 0 for agent in self.agents)
            },
            "learning_metrics": {
                "total_plasticity_events": len(episode_results) * 5,  # Approximate
                "adaptation_rate": 0.8  # Placeholder
            }
        }
        
        print(f"   ✓ Total neural activity: {summary['performance_metrics']['neural_activity']['total_spikes']} spikes")
        print(f"   ✓ Average firing rate: {summary['performance_metrics']['neural_activity']['average_firing_rate']:.2f} Hz")
        
        return summary
    
    def save_intermediate_results(self, episode_results: List[Dict]):
        """Save intermediate results during experiment."""
        print("\nSaving intermediate results...")
        
        timestamp = int(time.time())
        filename = f"intermediate_results_{timestamp}.json"
        
        # Convert results to JSON-serializable format
        serializable_results = []
        for episode in episode_results:
            serializable_results.append({
                "episode": episode["episode"],
                "actions_count": len(episode["agent_actions"]),
                "rewards": episode["rewards"],
                "social_interactions": episode["social_interactions"],
                "coordination_events": episode["coordination_events"]
            })
        
        with open(filename, 'w') as f:
            json.dump(serializable_results, f, indent=2)
        
        print(f"   ✓ Saved intermediate results to {filename}")
    
    def save_final_results(self, summary: Dict, all_episode_results: List[Dict]):
        """Save final experiment results."""
        print_header("Saving Final Results")
        
        timestamp = int(time.time())
        filename = f"experiment_results_{timestamp}.json"
        
        # Save comprehensive results
        final_data = {
            "experiment_info": self.experiment_config,
            "summary": summary,
            "episode_results": all_episode_results
        }
        
        with open(filename, 'w') as f:
            json.dump(final_data, f, indent=2, default=str)
        
        print(f"   ✓ Saved comprehensive results to {filename}")
        
        # Generate human-readable summary
        self.generate_human_readable_summary(summary)
    
    def generate_human_readable_summary(self, summary: Dict):
        """Generate human-readable summary of experiment results."""
        print("\nExperiment Summary:")
        print("-" * 60)
        print(f"Experiment: {self.experiment_config['name']}")
        print(f"Description: {self.experiment_config['description']}")
        print(f"\nDuration: {summary['total_duration']:.1f} seconds")
        print(f"Episodes Completed: {summary['episodes_completed']}")
        print(f"Agents: {summary['agent_count']}")
        
        print(f"\nPerformance:")
        print(f"  Total Actions: {summary['total_actions']}")
        print(f"  Total Rewards: {summary['total_rewards']:.2f}")
        print(f"  Average Reward per Episode: {summary['average_reward_per_episode']:.2f}")
        
        print(f"\nSocial Dynamics:")
        print(f"  Social Interactions: {summary['social_interactions']}")
        print(f"  Coordination Events: {summary['coordination_events']}")
        
        print(f"\nAgent Performance:")
        for agent_id, perf in summary["agent_performance"].items():
            print(f"  {agent_id}:")
            print(f"    Actions: {perf['actions_taken']}")
            print(f"    Total Rewards: {perf['total_rewards']:.2f}")
            print(f"    Average Reward: {perf['average_reward']:.2f}")
            print(f"    Neuromodulation Level: {perf['average_neuromodulation']:.2f}")
        
        print(f"\nNeural Activity:")
        neural = summary["performance_metrics"]["neural_activity"]
        print(f"  Total Spikes: {neural['total_spikes']}")
        print(f"  Average Firing Rate: {neural['average_firing_rate']:.2f} Hz")
        
        print(f"\nMemory Usage:")
        memory = summary["performance_metrics"]["memory_usage"]
        print(f"  Working Memory Entries: {memory['working_memory_entries']}")
        print(f"  Episodic Memory Episodes: {memory['episodic_memory_episodes']}")
        
        print("-" * 60)
        print("\nKey Insights:")
        
        # Generate insights
        if summary['average_reward_per_episode'] > 5.0:
            print("  • High learning efficiency observed")
        elif summary['average_reward_per_episode'] > 2.0:
            print("  • Moderate learning performance")
        else:
            print("  • Low learning performance - consider parameter tuning")
        
        if summary['social_interactions'] > 20:
            print("  • Strong social interaction and learning")
        else:
            print("  • Limited social interaction - may need more complex environment")
        
        if neural['average_firing_rate'] > 10.0:
            print("  • High neural activity levels")
        else:
            print("  • Moderate neural activity levels")
        
        print("\nExperiment completed successfully!")

def run_advanced_example():
    """Run the advanced multi-agent learning experiment."""
    print("\n" + "=" * 80)
    print("NLM Advanced Example: Multi-Agent Learning Experiment")
    print("=" * 80)
    
    print("\nThis example demonstrates advanced NLM capabilities including:")
    print("  • Complex multi-agent coordination and social learning")
    print("  • Advanced memory management and prediction systems")
    print("  • Neuromodulation and developmental processes")
    print("  • Performance optimization and benchmarking")
    print("  • Experimental design and result analysis")
    
    print("\nThis experiment is designed to run for approximately 30 seconds.")
    print("It will create 3 agents with different learning characteristics")
    print("and run a 10-episode learning experiment with social interaction.")
    
    # Create experiment
    experiment = MultiAgentExperiment()
    
    # Configure experiment
    config = experiment.create_experiment_config(
        neuron_count=900,
        agent_count=3,
        world_size=64,
        duration=30,
        complexity="high"
    )
    
    # Run experiment
    results = experiment.run_experiment(num_episodes=5)
    
    return results

def main():
    """Main function for the advanced example."""
    print_header("NLM Advanced Example")
    print("\nThis advanced example demonstrates complex multi-agent learning")
    print("with social interaction, coordination, and emergent behaviors.")
    
    print("\nWhat this example covers:")
    print("  1. Complex experiment configuration")
    print("  2. Multi-agent creation with varied characteristics")
    print("  3. Social learning and interaction")
    print("  4. Coordination and collaboration")
    print("  5. Performance optimization")
    print("  6. Comprehensive result analysis")
    
    print("\nThis example is computationally intensive and may take 30+ seconds.")
    print("It will demonstrate advanced NLM capabilities in action.")
    
    # Check if user wants to run the example
    try:
        response = input("\nRun advanced multi-agent learning experiment? (y/n): ").strip().lower()
        if response not in ['y', 'yes', '']:
            print("\nAdvanced experiment cancelled by user.")
            return
    except EOFError:
        response = 'y'  # Auto-run for automated environments
    
    # Run the advanced example
    try:
        results = run_advanced_example()
        
        print("\n" + "=" * 80)
        print("ADVANCED EXAMPLE COMPLETED SUCCESSFULLY!")
        print("=" * 80)
        
        print(f"\nFinal Results:")
        print(f"  • Episodes Completed: {results['episodes_completed']}")
        print(f"  • Total Duration: {results['total_duration']:.1f} seconds")
        print(f"  • Total Actions: {results['total_actions']}")
        print(f"  • Average Reward per Episode: {results['average_reward_per_episode']:.2f}")
        
        print(f"\nThe experiment has been saved to JSON files in the current directory.")
        print(f"Check for files matching 'experiment_results_*.json'.")
        
    except KeyboardInterrupt:
        print("\n\nExperiment interrupted by user.")
        print("Results may have been partially saved.")
    except Exception as e:
        print(f"\n\nExperiment error: {e}")
        print("Please check your NLM installation and try again.")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n\nFatal error: {e}")
        print("Please check your NLM installation.")
        sys.exit(1)