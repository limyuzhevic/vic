#!/usr/bin/env python3
"""
NLM Visualization Module

This module provides visualization capabilities for NLM (Neural Learning Machine) simulations.
"""

import sys
import os
from typing import Optional, Tuple, List
import numpy as np

# Try to import matplotlib for visualization
matplotlib_available = True
try:
    import matplotlib
    import matplotlib.pyplot as plt
    from matplotlib.animation import FuncAnimation
    import matplotlib.gridspec as gridspec
    from matplotlib.patches import Circle, Rectangle
    from matplotlib.colors import LinearSegmentedColormap
    import warnings
    warnings.filterwarnings('ignore', category=UserWarning)
except ImportError:
    matplotlib_available = False
    print("Warning: matplotlib not available. Visualization features will be limited.")
    print("Install with: pip install matplotlib")

import pynlm


class NLMVisualizer:
    """
    Main visualization class for NLM simulations.
    
    Provides comprehensive visualization capabilities including:
    - Brain activity visualization
    - Neural population dynamics
    - Agent movement in world space
    - Development stage progression
    - Performance metrics
    """
    
    def __init__(self):
        if not matplotlib_available:
            raise ImportError("matplotlib is required for visualization. Install with: pip install matplotlib")
        
        # Set up matplotlib
        plt.style.use('seaborn-v0_8')
        self.fig = None
        self.gs = None
        self.axes = None
        self.animation = None
        self.data_history = []
        self.current_step = 0
    
    def create_dashboard(self, figsize: Tuple[int, int] = (16, 10)):
        """
        Create a comprehensive visualization dashboard.
        
        Args:
            figsize: Figure size as (width, height)
        """
        self.fig = plt.figure(figsize=figsize)
        self.gs = gridspec.GridSpec(3, 4, figure=self.fig, hspace=0.3, wspace=0.2)
        
        # Initialize subplots
        self.axes = {
            'brain_activity': self.fig.add_subplot(self.gs[0, 0]),
            'neural_regions': self.fig.add_subplot(self.gs[0, 1]),
            'world_map': self.fig.add_subplot(self.gs[0, 2]),
            'sensory_input': self.fig.add_subplot(self.gs[0, 3]),
            'metrics': self.fig.add_subplot(self.gs[1, :2]),
            'development': self.fig.add_subplot(self.gs[1, 2]),
            'neuromodulation': self.fig.add_subplot(self.gs[1, 3]),
            'agent_stats': self.fig.add_subplot(self.gs[2, :]),
        }
        
        # Customize subplots
        self._setup_subplots()
        
        return self.fig
    
    def _setup_subplots(self):
        """Set up individual subplots with appropriate labels and formatting."""
        # Brain activity plot
        self.axes['brain_activity'].set_title('Brain Activity', fontsize=12, fontweight='bold')
        self.axes['brain_activity'].set_xlabel('Time')
        self.axes['brain_activity'].set_ylabel('Firing Rate (Hz)')
        self.axes['brain_activity'].grid(True, alpha=0.3)
        
        # Neural regions plot
        self.axes['neural_regions'].set_title('Neural Regions', fontsize=12, fontweight='bold')
        self.axes['neural_regions'].set_xlabel('Region')
        self.axes['neural_regions'].set_ylabel('Neuron Count')
        self.axes['neural_regions'].grid(True, alpha=0.3)
        
        # World map plot
        self.axes['world_map'].set_title('World Environment', fontsize=12, fontweight='bold')
        self.axes['world_map'].set_xlabel('X Position')
        self.axes['world_map'].set_ylabel('Y Position')
        self.axes['world_map'].set_xlim(0, 20)
        self.axes['world_map'].set_ylim(0, 20)
        self.axes['world_map'].grid(True, alpha=0.3)
        
        # Sensory input plot
        self.axes['sensory_input'].set_title('Sensory Input', fontsize=12, fontweight='bold')
        self.axes['sensory_input'].set_xlabel('Neuron Index')
        self.axes['sensory_input'].set_ylabel('Input Strength')
        self.axes['sensory_input'].grid(True, alpha=0.3)
        
        # Metrics plot
        self.axes['metrics'].set_title('Simulation Metrics', fontsize=12, fontweight='bold')
        self.axes['metrics'].set_xlabel('Time')
        self.axes['metrics'].set_ylabel('Value')
        self.axes['metrics'].grid(True, alpha=0.3)
        
        # Development plot
        self.axes['development'].set_title('Development Stage', fontsize=12, fontweight='bold')
        self.axes['development'].set_xlabel('Time')
        self.axes['development'].set_ylabel('Stage')
        self.axes['development'].set_ylim(-0.1, 1.1)
        self.axes['development'].grid(True, alpha=0.3)
        
        # Neuromodulation plot
        self.axes['neuromodulation'].set_title('Neuromodulation Levels', fontsize=12, fontweight='bold')
        self.axes['neuromodulation'].set_xlabel('Time')
        self.axes['neuromodulation'].set_ylabel('Level')
        self.axes['neuromodulation'].set_ylim(-0.1, 1.1)
        self.axes['neuromodulation'].grid(True, alpha=0.3)
        
        # Agent stats plot
        self.axes['agent_stats'].set_title('Agent Statistics', fontsize=12, fontweight='bold')
        self.axes['agent_stats'].set_xlabel('Time')
        self.axes['agent_stats'].set_ylabel('Value')
        self.axes['agent_stats'].grid(True, alpha=0.3)
    
    def update_data(self, step: int, brain: pynlm.Brain, 
                    world: Optional[pynlm.SimpleWorld] = None,
                    agent: Optional[pynlm.AgentBrain] = None,
                    sensory: Optional[pynlm.SensoryPercept] = None):
        """
        Update visualization data with new simulation step.
        
        Args:
            step: Current simulation step
            brain: Brain instance
            world: World instance
            agent: Agent instance
            sensory: Sensory input
        """
        self.current_step = step
        
        # Collect metrics
        metrics = pynlm.SimulationMetrics.collect(brain, world)
        self.data_history.append({
            'step': step,
            'firing_rate': metrics['average_firing_rate'],
            'firing_neurons': metrics['firing_neuron_count'],
            'total_spikes': metrics['total_spike_count'],
            'development_stage': metrics['developmental_stage'],
            'e_i_ratio': metrics['excitation_inhibition_ratio'],
        })
        
        if world and agent:
            # Collect world and agent data
            body = world.getAgentBody()
            self.data_history[-1].update({
                'agent_x': body.x,
                'agent_y': body.y,
                'agent_energy': body.energy,
                'agent_health': body.health,
                'agent_age': body.age,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError(),
            })
    
    def animate_step(self):
        """Animate to the next simulation step."""
        if self.animation:
            self.animation.event_source.stop()
        
        # Update all plots
        self._update_brain_activity()
        self._update_neural_regions()
        self._update_world_map()
        self._update_sensory_input()
        self._update_metrics()
        self._update_development()
        self._update_neuromodulation()
        self._update_agent_stats()
        
        plt.draw()
    
    def _update_brain_activity(self):
        """Update brain activity plot."""
        if len(self.data_history) < 2:
            return
        
        ax = self.axes['brain_activity']
        ax.clear()
        
        # Get firing rate data
        steps = [d['step'] for d in self.data_history]
        firing_rates = [d['firing_rate'] for d in self.data_history]
        
        ax.plot(steps, firing_rates, 'b-', linewidth=2, alpha=0.8)
        ax.fill_between(steps, firing_rates, alpha=0.3, color='blue')
        
        ax.set_title('Brain Activity', fontsize=12, fontweight='bold')
        ax.set_xlabel('Time')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.grid(True, alpha=0.3)
    
    def _update_neural_regions(self):
        """Update neural regions plot."""
        ax = self.axes['neural_regions']
        ax.clear()
        
        if len(self.data_history) == 0:
            return
        
        # For now, show a simplified representation
        # In a real implementation, you would access brain regions
        regions = ["Sensory", "Motor", "Association", "Memory"]
        neuron_counts = [1000, 800, 1200, 900]  # Placeholder data
        
        bars = ax.bar(regions, neuron_counts, color='skyblue', alpha=0.8)
        ax.set_title('Neural Regions', fontsize=12, fontweight='bold')
        ax.set_xlabel('Region')
        ax.set_ylabel('Neuron Count')
        ax.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{height}', ha='center', va='bottom')
    
    def _update_world_map(self):
        """Update world map plot."""
        ax = self.axes['world_map']
        ax.clear()
        
        if len(self.data_history) == 0:
            return
        
        # Draw environment boundaries
        ax.plot([0, 20, 20, 0, 0], [0, 0, 20, 20, 0], 'k-', linewidth=2, alpha=0.5)
        
        # Draw agent
        agent_data = self.data_history[-1]
        agent_x = agent_data.get('agent_x', 10)
        agent_y = agent_data.get('agent_y', 10)
        
        agent_circle = Circle((agent_x, agent_y), radius=0.5, 
                            color='red', alpha=0.8, label='Agent')
        ax.add_patch(agent_circle)
        
        # Draw some obstacles or resources (placeholder)
        for i in range(5):
            x = np.random.uniform(2, 18)
            y = np.random.uniform(2, 18)
            obj_type = np.random.choice(['R', 'H'], p=[0.7, 0.3])  # Resource or Hazard
            color = 'green' if obj_type == 'R' else 'red'
            label = 'Resource' if obj_type == 'R' else 'Hazard'
            
            obj_circle = Circle((x, y), radius=0.3, color=color, alpha=0.6)
            ax.add_patch(obj_circle)
        
        ax.set_title('World Environment', fontsize=12, fontweight='bold')
        ax.set_xlabel('X Position')
        ax.set_ylabel('Y Position')
        ax.set_xlim(0, 20)
        ax.set_ylim(0, 20)
        ax.grid(True, alpha=0.3)
        ax.legend()
    
    def _update_sensory_input(self):
        """Update sensory input plot."""
        ax = self.axes['sensory_input']
        ax.clear()
        
        if len(self.data_history) == 0:
            return
        
        # Create sample sensory input data
        np.random.seed(self.current_step)
        sensory_data = np.random.randn(20) * 0.5
        sensory_data = np.clip(sensory_data, -1, 1)
        
        ax.plot(range(len(sensory_data)), sensory_data, 'g-', linewidth=1.5)
        ax.fill_between(range(len(sensory_data)), sensory_data, alpha=0.3, color='green')
        
        ax.set_title('Sensory Input', fontsize=12, fontweight='bold')
        ax.set_xlabel('Neuron Index')
        ax.set_ylabel('Input Strength')
        ax.grid(True, alpha=0.3)
    
    def _update_metrics(self):
        """Update metrics plot."""
        ax = self.axes['metrics']
        ax.clear()
        
        if len(self.data_history) < 2:
            return
        
        steps = [d['step'] for d in self.data_history]
        
        # Plot multiple metrics in separate subplots within this main metrics plot
        # For space efficiency, we'll show the most important ones
        
        # Create a grid within this axis for multiple metrics
        gs_metrics = gridspec.GridSpecFromSubplotSpec(self.gs[1, :2], 
                                                      ax=ax, nrows_ncols=(2, 2), 
                                                      hspace=0.3, wspace=0.3)
        
        # Firing rate
        ax_firing = self.fig.add_subplot(gs_metrics[0, 0])
        firing_rates = [d['firing_rate'] for d in self.data_history]
        ax_firing.plot(steps, firing_rates, 'r-', linewidth=2)
        ax_firing.set_title('Firing Rate')
        ax_firing.set_ylabel('Hz')
        ax_firing.grid(True, alpha=0.3)
        
        # E/I ratio
        ax_ei = self.fig.add_subplot(gs_metrics[0, 1])
        ei_ratios = [d['e_i_ratio'] for d in self.data_history]
        ax_ei.plot(steps, ei_ratios, 'b-', linewidth=2)
        ax_ei.set_title('E/I Ratio')
        ax_ei.set_ylabel('Ratio')
        ax_ei.grid(True, alpha=0.3)
        
        # Total spikes
        ax_spikes = self.fig.add_subplot(gs_metrics[1, 0])
        total_spikes = [d['total_spikes'] for d in self.data_history]
        ax_spikes.plot(steps, total_spikes, 'g-', linewidth=2)
        ax_spikes.set_title('Total Spikes')
        ax_spikes.set_ylabel('Count')
        ax_spikes.set_xlabel('Time')
        ax_spikes.grid(True, alpha=0.3)
        
        # Remove the main metrics axis since we created subplots
        ax.remove()
    
    def _update_development(self):
        """Update development plot."""
        ax = self.axes['development']
        ax.clear()
        
        if len(self.data_history) < 2:
            return
        
        steps = [d['step'] for d in self.data_history]
        stages = [d['development_stage'] for d in self.data_history]
        
        # Convert development stages to numeric values for plotting
        stage_values = []
        for stage in stages:
            if stage == pynlm.DevelopmentalStage.Initial:
                stage_values.append(0.0)
            elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
                stage_values.append(0.3)
            elif stage == pynlm.DevelopmentalStage.Maturation:
                stage_values.append(0.6)
            elif stage == pynlm.DevelopmentalStage.Adult:
                stage_values.append(0.8)
            elif stage == pynlm.DevelopmentalStage.Aging:
                stage_values.append(1.0)
            else:
                stage_values.append(0.5)
        
        ax.plot(steps, stage_values, 'orange', linewidth=2, marker='o', markersize=3)
        ax.set_title('Development Stage', fontsize=12, fontweight='bold')
        ax.set_xlabel('Time')
        ax.set_ylabel('Stage')
        ax.set_ylim(-0.1, 1.1)
        ax.grid(True, alpha=0.3)
        
        # Add stage markers
        ax.axhline(y=0.3, color='gray', linestyle='--', alpha=0.5)
        ax.axhline(y=0.6, color='gray', linestyle='--', alpha=0.5)
        ax.axhline(y=0.8, color='gray', linestyle='--', alpha=0.5)
    
    def _update_neuromodulation(self):
        """Update neuromodulation plot."""
        ax = self.axes['neuromodulation']
        ax.clear()
        
        if len(self.data_history) < 2:
            return
        
        steps = [d['step'] for d in self.data_history]
        neuromodulation = [d.get('neuromodulation', 0.5) for d in self.data_history]
        
        ax.plot(steps, neuromodulation, 'purple', linewidth=2)
        ax.set_title('Neuromodulation Levels', fontsize=12, fontweight='bold')
        ax.set_xlabel('Time')
        ax.set_ylabel('Level')
        ax.set_ylim(-0.1, 1.1)
        ax.grid(True, alpha=0.3)
    
    def _update_agent_stats(self):
        """Update agent statistics plot."""
        ax = self.axes['agent_stats']
        ax.clear()
        
        if len(self.data_history) < 2:
            return
        
        steps = [d['step'] for d in self.data_history]
        
        # Create subplots for different agent stats
        gs_agent = gridspec.GridSpecFromSubplotSpec(self.gs[2, :],
                                                   ax=ax, nrows_ncols=(2, 2),
                                                   hspace=0.3, wspace=0.3)
        
        # Energy
        ax_energy = self.fig.add_subplot(gs_agent[0, 0])
        energies = [d.get('agent_energy', 100) for d in self.data_history]
        ax_energy.plot(steps, energies, 'cyan', linewidth=2)
        ax_energy.set_title('Energy')
        ax_energy.set_ylabel('Energy')
        ax_energy.set_xlabel('Time')
        ax_energy.grid(True, alpha=0.3)
        
        # Health
        ax_health = self.fig.add_subplot(gs_agent[0, 1])
        healths = [d.get('agent_health', 1.0) for d in self.data_history]
        ax_health.plot(steps, healths, 'pink', linewidth=2)
        ax_health.set_title('Health')
        ax_health.set_ylabel('Health')
        ax_health.set_xlabel('Time')
        ax_health.grid(True, alpha=0.3)
        
        # Age
        ax_age = self.fig.add_subplot(gs_agent[1, 0])
        ages = [d.get('agent_age', 0) for d in self.data_history]
        ax_age.plot(steps, ages, 'gold', linewidth=2)
        ax_age.set_title('Age')
        ax_age.set_ylabel('Age')
        ax_age.set_xlabel('Time')
        ax_age.grid(True, alpha=0.3)
        
        # Curiosity
        ax_curiosity = self.fig.add_subplot(gs_agent[1, 1])
        curiosities = [d.get('curiosity', 0.0) for d in self.data_history]
        ax_curiosity.plot(steps, curiosities, 'lime', linewidth=2)
        ax_curiosity.set_title('Curiosity')
        ax_curiosity.set_ylabel('Level')
        ax_curiosity.set_xlabel('Time')
        ax_curiosity.grid(True, alpha=0.3)
        
        # Remove the main agent stats axis
        ax.remove()
    
    def save_animation(self, filename: str, fps: int = 10):
        """
        Save the current animation to a file.
        
        Args:
            filename: Output filename
            fps: Frames per second
        """
        if self.animation:
            self.animation.save(filename, writer='pillow', fps=fps)
            print(f"Animation saved to {filename}")
    
    def save_figure(self, filename: str, dpi: int = 150):
        """
        Save the current figure to a file.
        
        Args:
            filename: Output filename
            dpi: Dots per inch
        """
        self.fig.savefig(filename, dpi=dpi, bbox_inches='tight')
        print(f"Figure saved to {filename}")
    
    def close(self):
        """Close the visualization window."""
        plt.close(self.fig)


def create_visualization(brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                        agent: Optional[pynlm.AgentBrain] = None,
                        sensory: Optional[pynlm.SensoryPercept] = None) -> NLMVisualizer:
    """
    Create a new visualizer for NLM simulations.
    
    Args:
        brain: Brain instance to visualize
        world: World instance (optional)
        agent: Agent instance (optional)
        sensory: Sensory input (optional)
        
    Returns:
        NLMVisualizer instance
    """
    return NLMVisualizer()


def animate_simulation(brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                      agent: Optional[pynlm.AgentBrain] = None,
                      sensory: Optional[pynlm.SensoryPercept] = None,
                      num_steps: int = 100, interval: int = 100):
    """
    Create an animation of the simulation.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: Agent instance
        sensory: Sensory input
        num_steps: Number of simulation steps
        interval: Animation interval in milliseconds
    """
    visualizer = NLMVisualizer()
    visualizer.create_dashboard()
    
    # Create animation
    def update(frame):
        visualizer.update_data(frame, brain, world, agent, sensory)
        visualizer.animate_step()
    
    frames = range(num_steps)
    visualizer.animation = FuncAnimation(
        visualizer.fig, update, frames=frames, 
        interval=interval, repeat=False
    )
    
    return visualizer


def main():
    """Main entry point for visualization tools."""
    parser = argparse.ArgumentParser(
        prog="pynlm-viz",
        description="NLM Visualization Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm-viz create --steps 50                # Create static visualization
  pynlm-viz animate --steps 100               # Create animation
  pynlm-viz save --output viz.png             # Save current visualization
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create command
    parser_create = subparsers.add_parser(
        "create",
        help="Create a new visualization",
        description="Create a new visualization dashboard"
    )
    parser_create.add_argument("--steps", type=int, default=10, help="Number of steps to simulate")
    parser_create.add_argument("--output", help="Output file for static visualization")
    parser_create.add_argument("--show", action="store_true", help="Show plot interactively")
    
    # Animate command
    parser_animate = subparsers.add_parser(
        "animate",
        help="Create an animation",
        description="Create an animated visualization"
    )
    parser_animate.add_argument("--steps", type=int, default=50, help="Number of simulation steps")
    parser_animate.add_argument("--interval", type=int, default=100, help="Animation interval (ms)")
    parser_animate.add_argument("--output", help="Output file for animation")
    parser_animate.add_argument("--show", action="store_true", help="Show animation interactively")
    
    # Save command
    parser_save = subparsers.add_parser(
        "save",
        help="Save current visualization",
        description="Save current visualization to file"
    )
    parser_save.add_argument("--output", required=True, help="Output file path")
    parser_save.add_argument("--dpi", type=int, default=150, help="DPI for output")
    
    # Show help
    parser_add = subparsers.add_parser(
        "add",
        help="Add visualization elements",
        description="Add visualization elements to existing plot"
    )
    
    args = parser.parse_args()
    
    try:
        if args.command == "create":
            # Create a sample visualization
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Create visualizer
            visualizer = NLMVisualizer()
            visualizer.create_dashboard()
            
            # Simulate and update
            for step in range(args.steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                sensory = world.getSensoryPercept()
                visualizer.update_data(step, brain, world, agent, sensory)
                visualizer.animate_step()
            
            if args.output:
                visualizer.save_figure(args.output)
            
            if args.show:
                plt.show()
            
            visualizer.close()
            
        elif args.command == "animate":
            # Create animation
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            visualizer = animate_simulation(brain, world, agent, None, args.steps, args.interval)
            
            if args.output:
                visualizer.save_animation(args.output, fps=1000 // args.interval)
            
            if args.show:
                plt.show()
            
            visualizer.close()
            
        elif args.command == "save":
            # Create a sample plot and save it
            plt.figure(figsize=(10, 6))
            
            # Create some sample data
            x = np.linspace(0, 10, 100)
            y = np.sin(x) * np.exp(-x/5)
            
            plt.plot(x, y, 'b-', linewidth=2, label='Sample Signal')
            plt.title('NLM Sample Visualization')
            plt.xlabel('Time')
            plt.ylabel('Amplitude')
            plt.grid(True, alpha=0.3)
            plt.legend()
            
            plt.savefig(args.output, dpi=args.dpi, bbox_inches='tight')
            plt.close()
            
            print(f"Visualization saved to {args.output}")
        
        else:
            parser.print_help()
            return 1
            
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0


if __name__ == "__main__":
    sys.exit(main())