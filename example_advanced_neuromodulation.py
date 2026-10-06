#!/usr/bin/env python3
"""
Advanced NLM Example: Neuromodulation and Curiosity-Driven Learning

This example demonstrates advanced NLM capabilities including:
- Neuromodulation (dopamine, curiosity, novelty)
- Reward prediction error and learning
- Developmental stages and plasticity
- Complex environment interaction
- Performance monitoring and analysis

This is Part 1 of a comprehensive tutorial series on advanced NLM usage.
"""

import pynlm
import json
import time
from pathlib import Path

class AdvancedAgentSimulator:
    """
    Advanced agent simulation demonstrating NLM capabilities.
    
    This class implements a complete simulation with:
    - Neuromodulation-driven learning
    - Curiosity-based exploration
    - Developmental progression
    - Performance monitoring
    - Checkpoint and recovery
    """
    
    def __init__(self, config_file=None):
        """Initialize the advanced simulator."""
        print("=== Advanced NLM Agent Simulator ===")
        
        # Create configuration with advanced settings
        self.config = pynlm.create_default_config()
        
        # Apply advanced configuration parameters
        self.config.set("neuron_count", 2000, pynlm.ConfigSource.Runtime)
        self.config.set("region_count", 3, pynlm.ConfigSource.Runtime)
        self.config.set("connection_probability", 0.15, pynlm.ConfigSource.Runtime)
        
        # Neuromodulation settings
        self.config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.stdp.learning_rate", 0.01, pynlm.ConfigSource.Runtime)
        self.config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.Runtime)
        self.config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.Runtime)
        self.config.set("neuromod.dopamine.scale", 1.0, pynlm.ConfigSource.Runtime)
        
        # Development settings
        self.config.set("development.enable", True, pynlm.ConfigSource.Runtime)
        self.config.set("development.initial_plasticity", 1.0, pynlm.ConfigSource.Runtime)
        self.config.set("development.adult_plasticity", 0.2, pynlm.ConfigSource.Runtime)
        
        # Environment settings
        self.config.set("environment.width", 50, pynlm.ConfigSource.Runtime)
        self.config.set("environment.height", 50, pynlm.ConfigSource.Runtime)
        self.config.set("environment.vision.width", 16, pynlm.ConfigSource.Runtime)
        self.config.set("environment.vision.height", 16, pynlm.ConfigSource.Runtime)
        
        # Performance settings
        self.config.set("simulation.timestep", 0.001, pynlm.ConfigSource.Runtime)
        self.config.set("simulation.max_steps", 5000, pynlm.ConfigSource.Runtime)
        
        print(f"Configuration loaded with {self.config.get('neuron_count')} neurons")
        print(f"Environment: {self.config.get('environment.width')}x{self.config.get('environment.height')}m")
        print(f"Neuromodulation enabled: curiosity, novelty, dopamine")
        print(f"Development enabled: {self.config.get('development.enable')}")
        
        # Initialize brain with error handling
        try:
            self.brain = pynlm.create_brain(self.config)
            self.brain.initialize()
            print("✓ Brain initialized successfully")
        except Exception as e:
            print(f"✗ Brain initialization failed: {e}")
            raise
        
        # Create agent interface
        self.agent = pynlm.create_agent_brain(self.brain)
        print("✓ Agent interface created")
        
        # Performance monitoring
        self.performance_data = {
            'start_time': time.time(),
            'steps_completed': 0,
            'total_reward': 0.0,
            'learning_events': 0,
            'max_curiosity': 0.0,
            'max_novelty': 0.0,
            'average_firing_rate': 0.0,
            'checkpoint_times': []
        }
        
        # Advanced features tracking
        self.advanced_features = {
            'curiosity_enabled': self.config.get('neuromod.curiosity.enable'),
            'novelty_enabled': self.config.get('neuromod.novelty.enable'),
            'development_enabled': self.config.get('development.enable'),
            'structural_plasticity_enabled': self.config.get('plasticity.structural.enable', False)
        }
        
        # Metrics tracking
        self.metrics_history = {
            'curiosity': [],
            'novelty': [],
            'dopamine': [],
            'reward': [],
            'firing_rate': []
        }
        
    def run_simulation(self, save_checkpoint=False):
        """Run the advanced agent simulation."""
        print("\n=== Starting Advanced Simulation ===")
        
        # Create world with advanced configuration
        self.world = pynlm.create_simple_world()
        self.world.configure(
            width=self.config.get('environment.width'),
            height=self.config.get('environment.height'),
            vision_width=self.config.get('environment.vision.width'),
            vision_height=self.config.get('environment.vision.height')
        )
        self.world.reset()
        self.world.set_agent_start(25.0, 25.0)  # Center of world
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable advanced features
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        self.agent.enable_development(True)
        self.agent.enable_structural_plasticity(True)
        
        print(f"World created: {self.config.get('environment.width')}x{self.config.get('environment.height')}")
        print(f"Agent starting position: (25.0, 25.0)")
        print(f"Vision: {self.config.get('environment.vision.width')}x{self.config.get('environment.vision.height')}")
        
        # Run simulation with performance monitoring
        max_steps = self.config.get('simulation.max_steps')
        
        for step in range(max_steps):
            step_start_time = time.time()
            
            # World update
            self.world.update(0.1)  # 100ms per step
            
            # Get sensory percept
            percept = self.world.get_sensory_percept()
            
            # Process sensory input
            self.agent.process_sensory_input(percept)
            
            # Brain step with developmental updates
            self.brain.step(step, step * 0.1)  # 100ms timestep
            
            # Get motor command
            motor_cmd = self.agent.decode_motor_command()
            
            # Apply action to world
            action_result = self.world.apply_motor_command(
                motor_cmd, self.world.get_simulation_time())
            
            # Apply neuromodulation based on reward
            reward = action_result.reward
            predicted_reward = 0.0  # Simplified for this example
            self.agent.apply_reward_modulation(reward, predicted_reward)
            
            # Update development
            self.agent.update_development(0.1)
            
            # Record metrics
            self._record_step_metrics(step, percept, motor_cmd, action_result)
            
            # Checkpoint saving
            if save_checkpoint and step % 1000 == 0:
                self._save_checkpoint(step)
            
            # Performance logging
            if step % 100 == 0:
                self._log_performance(step)
                
            step_time = time.time() - step_start_time
            self.performance_data['steps_completed'] += 1
            
            # Early termination if brain is overstimulated
            if self.brain.get_average_firing_rate() > 50.0:
                print(f"⚠️  High firing rate detected: {self.brain.get_average_firing_rate():.2f} Hz")
                if step > 100:
                    print("Simulation terminated due to high firing rate")
                    break
        
        print(f"\n=== Simulation Complete ===")
        print(f"Steps completed: {self.performance_data['steps_completed']}")
        print(f"Total reward: {self.performance_data['total_reward']:.2f}")
        print(f"Average curiosity: {sum(self.metrics_history['curiosity'])/len(self.metrics_history['curiosity']):.3f}")
        print(f"Average novelty: {sum(self.metrics_history['novelty'])/len(self.metrics_history['novelty']):.3f}")
        print(f"Final developmental stage: {self.agent.get_developmental_stage().name}")
        
        return self._generate_summary()
        
    def _record_step_metrics(self, step, percept, motor_cmd, action_result):
        """Record metrics for each simulation step."""
        # Record neuromodulation levels
        self.metrics_history['curiosity'].append(self.agent.get_curiosity_level())
        self.metrics_history['novelty'].append(self.agent.get_novelty_level())
        self.metrics_history['dopamine'].append(self.agent.get_neuromodulation_level())
        self.metrics_history['reward'].append(action_result.reward)
        self.metrics_history['firing_rate'].append(self.brain.get_average_firing_rate())
        
        # Update performance data
        self.performance_data['total_reward'] += action_result.reward
        self.performance_data['max_curiosity'] = max(
            self.performance_data['max_curiosity'],
            self.agent.get_curiosity_level())
        self.performance_data['max_novelty'] = max(
            self.performance_data['max_novelty'],
            self.agent.get_novelty_level())
        
        # Detect learning events (significant changes)
        if step > 0 and abs(self.agent.get_prediction_error()) > 0.1:
            self.performance_data['learning_events'] += 1
            
    def _log_performance(self, step):
        """Log performance metrics for monitoring."""
        avg_firing_rate = sum(self.metrics_history['firing_rate'][-100:]) / min(100, len(self.metrics_history['firing_rate']))
        self.performance_data['average_firing_rate'] = avg_firing_rate
        
        print(f"Step {step:4d}: "
              f"Curiosity: {self.agent.get_curiosity_level():.3f}, "
              f"Novelty: {self.agent.get_novelty_level():.3f}, "
              f"Dopamine: {self.agent.get_neuromodulation_level():.3f}, "
              f"Reward: {self.agent.get_prediction_error():+.3f}, "
              f"Firing: {avg_firing_rate:.1f} Hz, "
              f"Dev: {self.agent.get_developmental_stage().name}")
              
    def _save_checkpoint(self, step):
        """Save simulation checkpoint."""
        checkpoint_file = f"checkpoint_step_{step}.bin"
        if self.brain.save(checkpoint_file):
            checkpoint_time = time.time() - self.performance_data['start_time']
            self.performance_data['checkpoint_times'].append(checkpoint_time)
            print(f"✓ Checkpoint saved: {checkpoint_file}")
        else:
            print(f"✗ Checkpoint save failed: {checkpoint_file}")
            
    def _generate_summary(self):
        """Generate simulation summary."""
        summary = {
            'final_metrics': {
                'total_steps': self.performance_data['steps_completed'],
                'total_reward': self.performance_data['total_reward'],
                'max_curiosity': self.performance_data['max_curiosity'],
                'max_novelty': self.performance_data['max_novelty'],
                'learning_events': self.performance_data['learning_events'],
                'average_firing_rate': self.performance_data['average_firing_rate'],
                'developmental_stage': self.agent.get_developmental_stage().name,
                'neuromodulation_enabled': self.advanced_features['curiosity_enabled'],
                'development_enabled': self.advanced_features['development_enabled']
            },
            'performance': {
                'total_time': time.time() - self.performance_data['start_time'],
                'checkpoint_count': len(self.performance_data['checkpoint_times']),
                'average_step_time': (time.time() - self.performance_data['start_time']) / max(1, self.performance_data['steps_completed'])
            }
        }
        
        # Save summary to JSON file
        with open('simulation_summary.json', 'w') as f:
            json.dump(summary, f, indent=2)
            
        return summary
        
    def print_detailed_report(self):
        """Print detailed simulation report."""
        print("\n" + "="*60)
        print("DETAILED SIMULATION REPORT")
        print("="*60)
        
        summary = self.performance_data
        
        print(f"Total Simulation Time: {summary['total_time']:.2f} seconds")
        print(f"Steps Completed: {summary['steps_completed']}")
        print(f"Total Reward Accumulated: {summary['total_reward']:.2f}")
        print(f"Learning Events Detected: {summary['learning_events']}")
        print(f"Maximum Curiosity Level: {summary['max_curiosity']:.3f}")
        print(f"Maximum Novelty Level: {summary['max_novelty']:.3f}")
        print(f"Average Firing Rate: {summary['average_firing_rate']:.2f} Hz")
        print(f"Checkpoints Created: {len(summary['checkpoint_times'])}")
        
        if summary['checkpoint_times']:
            print(f"Checkpoint Interval: {sum(summary['checkpoint_times'])/len(summary['checkpoint_times']):.2f}s average")
        
        print(f"\nFinal Brain State:")
        print(f"  Neurons: {self.brain.get_total_neuron_count()}")
        print(f"  Synapses: {self.brain.get_total_synapse_count()}")
        print(f"  Firing Neurons: {self.brain.get_firing_neuron_count()}")
        print(f"  Developmental Stage: {self.agent.get_developmental_stage().name}")
        
        print(f"\nAdvanced Features Status:")
        for feature, enabled in self.advanced_features.items():
            print(f"  {feature.replace('_', ' ').title()}: {'Enabled' if enabled else 'Disabled'}")
            
        print(f"\nNeuromodulation Levels:")
        print(f"  Curiosity: {self.agent.get_curiosity_level():.3f} (exploration motivation)")
        print(f"  Novelty: {self.agent.get_novelty_level():.3f} (change detection)")
        print(f"  Dopamine: {self.agent.get_neuromodulation_level():.3f} (reward prediction error)")
        print(f"  Prediction Error: {self.agent.get_prediction_error():.3f}")


def run_advanced_example_1():
    """Example 1: Basic advanced neuromodulation simulation."""
    print("\n" + "="*80)
    print("ADVANCED EXAMPLE 1: Neuromodulation and Curiosity-Driven Learning")
    print("="*80)
    
    simulator = AdvancedAgentSimulator()
    summary = simulator.run_simulation(save_checkpoint=True)
    simulator.print_detailed_report()
    
    return simulator


def run_advanced_example_2():
    """Example 2: Developmental learning simulation."""
    print("\n" + "="*80)
    print("ADVANCED EXAMPLE 2: Developmental Learning and Maturation")
    print("="*80)
    
    simulator = AdvancedAgentSimulator()
    summary = simulator.run_simulation(save_checkpoint=True)
    simulator.print_detailed_report()
    
    return simulator


if __name__ == "__main__":
    print("🚀 Advanced NLM Examples - Part 1")
    print("This script demonstrates advanced NLM capabilities including:")
    print("  • Neuromodulation (dopamine, curiosity, novelty)")
    print("  • Reward prediction error and learning")
    print("  • Developmental stages and plasticity")
    print("  • Complex environment interaction")
    print("  • Performance monitoring and analysis")
    print("\nChoose an example:")
    print("  1 - Neuromodulation and Curiosity-Driven Learning")
    print("  2 - Developmental Learning and Maturation")
    print("  (Any other number - will run Example 1)")
    
    try:
        choice = input("\nEnter your choice (1-2): ").strip()
        if choice == "2":
            run_advanced_example_2()
        else:
            run_advanced_example_1()
    except KeyboardInterrupt:
        print("\n⚠️  Simulation interrupted by user")
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()

print("\nAdvanced NLM examples completed!")