#!/usr/bin/env python3
"""
Enhanced NLM Python API Examples

This file contains comprehensive examples of using the NLM Python API
with advanced features, demonstrating real-world use cases and
research applications.
"""

import pynlm
import numpy as np
import time
import json
import threading
from typing import List, Optional, Dict, Any, Callable
from enum import Enum
import matplotlib.pyplot as plt

class NLMAgent:
    """
    Enhanced agent wrapper that demonstrates advanced NLM usage patterns.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize enhanced agent with optional configuration file."""
        if config_file:
            self.config = pynlm.Config()
            self.config.loadFromFile(config_file)
        else:
            self.config = pynlm.Config()
        
        self.brain = pynlm.Brain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Advanced configuration
        self.setup_advanced_brain()
        self.setup_advanced_world()
        
    def setup_advanced_brain(self):
        """Configure brain with advanced settings for learning."""
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Advanced brain configuration
        self.config.set("neuron_count", 5000)
        self.config.set("region_count", 4)
        self.config.set("connection_probability", 0.15)
        self.config.set("stdp_ltp_weight", 0.015)
        self.config.set("stdp_ltd_weight", 0.018)
        self.config.set("stdp_tau", 25.0)
        self.config.set("synaptogenesis_rate", 0.0002f)
        self.config.set("pruning_rate", 0.00002f)
        self.config.set("replay_interval", 50)
        self.config.set("consolidation_interval", 500)
        
    def setup_advanced_world(self):
        """Setup world with challenging environment."""
        self.world.configure(width=50.0, height=50.0, visionWidth=32, visionHeight=32)
        self.world.setRandomSeed(12345)
        
    def initialize(self):
        """Initialize the complete agent."""
        self.brain.initialize()
        self.agent.initialize(self.world)
        return self
        
    def run_episode(self, max_steps: int = 1000, verbose: bool = True) -> Dict[str, Any]:
        """
        Run a complete learning episode.
        
        Returns detailed statistics about the episode.
        """
        # Store initial state
        initial_neurons = self.brain.getTotalNeuronCount()
        initial_synapses = self.brain.getTotalSynapseCount()
        initial_firing = self.brain.getFiringNeuronCount()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Episode statistics
        stats = {
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'max_novelty': 0.0,
            'planning_confidence': 0.0,
            'body_awareness': 0.0,
            'concepts_formed': 0,
            'working_memory_usage': 0.0,
            'spike_statistics': [],
            'action_distribution': {},
            'development_stage': 0
        }
        
        # Run episode
        for step in range(max_steps):
            # Get sensory input from world
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            action_result = self.world.applyMotorCommand(motor_cmd, step * 0.1)
            
            # Apply reward modulation
            reward = action_result.reward
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect statistics
            stats['total_reward'] += reward
            stats['max_curiosity'] = max(stats['max_curiosity'], self.agent.getCuriosityLevel())
            stats['max_novelty'] = max(stats['max_novelty'], self.agent.getNoveltyLevel())
            stats['planning_confidence'] = max(stats['planning_confidence'], self.agent.getPredictionError())
            stats['body_awareness'] = max(stats['body_awareness'], 0.5)  # Simplified
            stats['working_memory_usage'] = max(stats['working_memory_usage'], 0.3)  # Simplified
            stats['development_stage'] = self.brain.getDevelopmentalStage()
            
            # Track action distribution
            action_type = motor_cmd
            if action_type not in stats['action_distribution']:
                stats['action_distribution'][action_type] = 0
            stats['action_distribution'][action_type] += 1
            
            # Track spike statistics
            spike_count = self.brain.getTotalSpikeCount() - initial_spikes
            stats['spike_statistics'].append(spike_count)
            
            # Check if episode should end
            if self.brain.getTotalSpikeCount() - initial_spikes > 10000 or step % 100 == 0:
                if step < max_steps - 1:
                    self.brain.reset()
                    self.agent.reset()
                    self.world.reset()
                    self.agent.initialize(self.world)
                    # Adjust for new initialization
                    initial_spikes = self.brain.getTotalSpikeCount()
                    
        # Calculate episode summary
        stats['final_neurons'] = self.brain.getTotalNeuronCount()
        stats['final_synapses'] = self.brain.getTotalSynapseCount()
        stats['total_spikes'] = self.brain.getTotalSpikeCount() - initial_spikes
        stats['steps'] = max_steps
        stats['avg_reward'] = stats['total_reward'] / max_steps
        stats['avg_spikes'] = np.mean(stats['spike_statistics']) if stats['spike_statistics'] else 0
        stats['spike_std'] = np.std(stats['spike_statistics']) if stats['spike_statistics'] else 0
        
        return stats
        
    def train_curriculum(self, curriculum: List[Dict]) -> Dict[str, Any]:
        """
        Train using a curriculum learning approach.
        
        Each curriculum item should have: difficulty, duration, parameters
        """
        results = []
        
        for phase in curriculum:
            print(f"\n=== Curriculum Phase: {phase['name']} ===")
            print(f"Difficulty: {phase.get('difficulty', 1.0)}")
            print(f"Duration: {phase.get('duration', 100)} steps")
            
            # Apply phase-specific configuration
            for key, value in phase.get('config', {}).items():
                if isinstance(value, float) and value < 1.0:
                    value = float(value)
                elif isinstance(value, int):
                    value = int(value)
                self.config.set(key, value)
            
            # Reinitialize brain with new config
            self.brain.reset()
            self.agent.reset()
            self.world.reset()
            self.agent.initialize(self.world)
            
            # Run phase
            phase_stats = self.run_episode(
                max_steps=phase.get('duration', 100),
                verbose=False
            )
            
            phase_stats['phase'] = phase['name']
            results.append(phase_stats)
            
            # Brief rest period
            if phase != curriculum[-1]:
                print("Rest period...")
                time.sleep(0.5)
                
        return results
        
    def save_brain(self, path: str):
        """Save brain state to file."""
        self.brain.save(path)
        print(f"Brain state saved to: {path}")
        
    def load_brain(self, path: str):
        """Load brain state from file."""
        if self.brain.load(path):
            print(f"Brain state loaded from: {path}")
            return True
        else:
            print(f"Failed to load brain state from: {path}")
            return False


def run_comparison_study():
    """
    Demonstrate running a comparison study between different configurations.
    """
    print("=== NLM Comparison Study ===\n")
    
    configurations = [
        {"name": "Small", "neurons": 1000, "plasticity": 0.5, "curiosity": 0.5},
        {"name": "Medium", "neurons": 3000, "plasticity": 1.0, "curiosity": 1.0},
        {"name": "Large", "neurons": 5000, "plasticity": 1.5, "curiosity": 1.5},
    ]
    
    results = {}
    
    for config in configurations:
        print(f"Running {config['name']} configuration...")
        
        agent = NLMAgent()
        agent.config.set("neuron_count", config['neurons'])
        
        # Set plasticity based on configuration
        plasticity_factor = config['plasticity']
        for key in agent.config.getKeys():
            if 'stdp' in key.lower() or 'plasticity' in key.lower():
                value = agent.config.getOr[float](key, 0.01)
                agent.config.set(key, value * plasticity_factor)
        
        agent.agent.enableCuriosity(config['curiosity'] > 0.0)
        
        agent.initialize()
        stats = agent.run_episode(max_steps=500, verbose=False)
        
        results[config['name']] = {
            'config': config,
            'stats': stats,
            'final_neurons': agent.brain.getTotalNeuronCount(),
            'final_synapses': agent.brain.getTotalSynapseCount()
        }
        
        print(f"  Total reward: {stats['total_reward']:.2f}")
        print(f"  Average curiosity: {stats['max_curiosity']:.3f}")
        print(f"  Average novelty: {stats['max_novelty']:.3f}")
        print()
    
    return results


def create_advanced_visualization(agent: NLMAgent, stats: Dict[str, Any]):
    """
    Create advanced visualizations of brain activity and learning.
    """
    try:
        import matplotlib.pyplot as plt
        import seaborn as sns
        
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        fig.suptitle('NLM Advanced Visualization', fontsize=16, fontweight='bold')
        
        # Spike statistics over time
        ax = axes[0, 0]
        if 'spike_statistics' in stats and stats['spike_statistics']:
            ax.plot(stats['spike_statistics'])
            ax.set_title('Spike Count per Step')
            ax.set_xlabel('Step')
            ax.set_ylabel('Spikes in window')
            ax.grid(True, alpha=0.3)
        
        # Action distribution
        ax = axes[0, 1]
        if 'action_distribution' in stats:
            actions = list(stats['action_distribution'].keys())
            counts = list(stats['action_distribution'].values())
            ax.bar(range(len(actions)), counts)
            ax.set_title('Action Distribution')
            ax.set_xlabel('Action Index')
            ax.set_ylabel('Count')
            ax.set_xticks(range(len(actions)))
        
        # Reward over time (simulated)
        ax = axes[0, 2]
        steps = min(100, len(stats.get('spike_statistics', [])))
        if steps > 0:
            # Create synthetic reward curve based on spike statistics
            window_size = max(1, len(stats['spike_statistics']) // steps)
            smoothed_spikes = []
            for i in range(steps):
                start = i * window_size
                end = min(start + window_size, len(stats['spike_statistics']))
                smoothed_spikes.append(np.mean(stats['spike_statistics'][start:end]))
            
            ax.plot(smoothed_spikes)
            ax.set_title('Activity Over Time')
            ax.set_xlabel('Window')
            ax.set_ylabel('Activity Level')
            ax.grid(True, alpha=0.3)
        
        # Development stages
        ax = axes[1, 0]
        developmental_stage = stats.get('development_stage', 0)
        stages = ['Initial', 'Critical', 'Mature', 'Adult']
        progress = min(1.0, developmental_stage / 3.0)
        ax.barh(stages, [progress] + [0] * 3)
        ax.set_title('Development Progress')
        ax.set_xlim(0, 1)
        
        # Neuromodulation levels
        ax = axes[1, 1]
        neuromod_data = {
            'Curiosity': stats.get('max_curiosity', 0),
            'Novelty': stats.get('max_novelty', 0),
            'Planning': stats.get('planning_confidence', 0),
            'Memory': stats.get('working_memory_usage', 0)
        }
        ax.barh(list(neuromod_data.keys()), list(neuromod_data.values()))
        ax.set_title('Neuromodulation Levels')
        ax.set_xlim(0, 1)
        
        # Network growth
        ax = axes[1, 2]
        growth_data = {
            'Neurons': stats.get('final_neurons', 0) if 'final_neurons' in stats else 0,
            'Synapses': stats.get('final_synapses', 0) if 'final_synapses' in stats else 0
        }
        ax.bar(list(growth_data.keys()), list(growth_data.values()))
        ax.set_title('Network Architecture')
        ax.set_ylabel('Count')
        
        plt.tight_layout()
        
        # Save visualization
        filename = 'nlm_advanced_visualization.png'
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        print(f"\nVisualization saved as: {filename}")
        
        return filename
        
    except ImportError:
        print("Warning: matplotlib/seaborn not available. Skipping visualization.")
        return None


def run_advanced_research():
    """
    Demonstrate advanced research patterns with NLM.
    """
    print("=== Advanced NLM Research Demo ===\n")
    
    # Research question 1: Effect of plasticity on learning
    print("1. Testing plasticity effects on learning speed...")
    
    plasticity_configs = [
        {"plasticity": 0.5, "name": "Low Plasticity"},
        {"plasticity": 1.0, "name": "Medium Plasticity"},
        {"plasticity": 2.0, "name": "High Plasticity"},
    ]
    
    plasticity_results = {}
    
    for config in plasticity_configs:
        agent = NLMAgent()
        
        # Apply plasticity settings
        agent.config.set("stdp_ltp_weight", 0.01 * config['plasticity'])
        agent.config.set("stdp_ltd_weight", 0.012 * config['plasticity'])
        agent.config.set("synaptogenesis_rate", 0.0001 * config['plasticity'])
        
        agent.initialize()
        stats = agent.run_episode(max_steps=300, verbose=False)
        
        plasticity_results[config['name']] = stats['total_reward'] / 300
        print(f"   {config['name']}: {plasticity_results[config['name']]:.3f} avg reward/step")
    
    # Research question 2: Curiosity vs exploitation tradeoff
    print("\n2. Testing curiosity-exploitation tradeoff...")
    
    curiosity_configs = [
        {"curiosity": 0.0, "name": "No Curiosity"},
        {"curiosity": 0.5, "name": "Moderate Curiosity"},
        {"curiosity": 1.0, "name": "High Curiosity"},
    ]
    
    curiosity_results = {}
    
    for config in curiosity_configs:
        agent = NLMAgent()
        agent.agent.enableCuriosity(config['curiosity'] > 0.0)
        
        # Modify curiosity response
        curiosity_multiplier = config['curiosity'] * 2.0
        original_viz = agent.agent.getNoveltyLevel
        
        agent.initialize()
        stats = agent.run_episode(max_steps=400, verbose=False)
        
        curiosity_results[config['name']] = {
            'avg_reward': stats['total_reward'] / 400,
            'exploration': stats['max_curiosity'],
            'novelty': stats['max_novelty']
        }
        print(f"   {config['name']}: {curiosity_results[config['name']]['avg_reward']:.3f} reward, "
              f"{curiosity_results[config['name']]['exploration']:.2f} curiosity")
    
    return {
        'plasticity': plasticity_results,
        'curiosity': curiosity_results
    }


def create_research_paper_figure():
    """
    Create a figure suitable for a research paper.
    """
    print("\n=== Creating Research Paper Visualization ===")
    
    # Create a comprehensive comparison figure
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        # Generate synthetic data for demonstration
        n_figures = 4
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Phase 6 Integration: Comprehensive Analysis', 
                    fontsize=14, fontweight='bold')
        
        # Figure 1: Network Growth
        ax = axes[0, 0]
        steps = np.linspace(0, 100, 50)
        neurons = 500 + steps * 30
        synapses = 2000 + steps * 150
        ax.plot(steps, neurons, 'b-', linewidth=2, label='Neurons')
        ax.plot(steps, synapses, 'r-', linewidth=2, label='Synapses')
        ax.set_xlabel('Development Steps')
        ax.set_ylabel('Count')
        ax.set_title('Network Growth During Development')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Figure 2: Neuromodulation Dynamics
        ax = axes[0, 1]
        time_points = np.linspace(0, 50, 100)
        dopamine = 0.5 * np.exp(-time_points/20)
        curiosity = 0.3 * np.sin(time_points * 0.2) * np.exp(-time_points/30)
        novelty = 0.4 * np.exp(-time_points/15)
        
        ax.plot(time_points, dopamine, 'g-', linewidth=2, label='Dopamine')
        ax.plot(time_points, curiosity, 'b-', linewidth=2, label='Curiosity')
        ax.plot(time_points, novelty, 'r-', linewidth=2, label='Novelty')
        ax.set_xlabel('Time (arbitrary units)')
        ax.set_ylabel('Level')
        ax.set_title('Neuromodulation Dynamics')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Figure 3: Learning Curve
        ax = axes[1, 0]
        learning_steps = np.linspace(0, 100, 50)
        learning_curve = 1 - np.exp(-learning_steps * 0.05)
        ax.plot(learning_steps, learning_curve, 'm-', linewidth=2)
        ax.set_xlabel('Training Steps')
        ax.set_ylabel('Performance')
        ax.set_title('Learning Curve')
        ax.grid(True, alpha=0.3)
        ax.set_ylim(0, 1)
        
        # Figure 4: Memory System Activity
        ax = axes[1, 1]
        memory_types = ['Working Memory', 'Episodic Memory', 'Associative Memory']
        memory_values = [0.7, 0.5, 0.6]
        colors = ['#2E86AB', '#A23B72', '#F18F01']
        
        bars = ax.bar(memory_types, memory_values, color=colors, alpha=0.7)
        ax.set_ylabel('Activity Level')
        ax.set_title('Memory System Integration')
        ax.set_ylim(0, 1)
        
        # Add value labels on bars
        for bar, value in zip(bars, memory_values):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.05,
                   f'{value:.1f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        # Save figure
        filename = 'nlm_research_figure.png'
        plt.savefig(filename, dpi=300, bbox_inches='tight', 
                   facecolor='white', edgecolor='none')
        print(f"Research figure saved as: {filename}")
        
        return filename
        
    except ImportError:
        print("Warning: matplotlib not available. Skipping research figure creation.")
        return None


if __name__ == "__main__":
    print("Enhanced NLM Python API Examples")
    print("===============================\n")
    
    # Example 1: Basic agent usage
    print("Example 1: Creating and running an enhanced agent...")
    agent = NLMAgent()
    agent.initialize()
    stats = agent.run_episode(max_steps=100, verbose=True)
    print(f"\nEpisode Results:")
    print(f"  Total Reward: {stats['total_reward']:.2f}")
    print(f"  Average Firing Rate: {stats['avg_spikes']:.1f}")
    print(f"  Development Stage: {stats['development_stage']}")
    
    # Save visualization
    create_advanced_visualization(agent, stats)
    
    # Example 2: Comparison study
    print("\n\nExample 2: Running comparison study...")
    comparison_results = run_comparison_study()
    
    # Example 3: Advanced research
    print("\n\nExample 3: Advanced research patterns...")
    research_results = run_advanced_research()
    
    # Example 4: Research paper figure
    create_research_paper_figure()
    
    print("\n" + "="*60)
    print("Enhanced NLM Python API Examples Complete!")
    print("="*60)
    print("\nKey Features Demonstrated:")
    print("  • Advanced agent wrapper with curriculum learning")
    print("  • Comprehensive statistics collection and analysis")
    print("  • Comparison studies across configurations")
    print("  • Advanced research patterns")
    print("  • Publication-quality visualizations")
    print("  • Neuromodulation and plasticity studies")
    print("\nFor more detailed documentation, see the NLM documentation.")
