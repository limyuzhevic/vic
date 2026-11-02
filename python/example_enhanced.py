#!/usr/bin/env python3
"""
Example demonstrating the enhanced NLM Python bindings.

This script shows the key features of the enhanced bindings:
- Context managers for resource management
- Training utilities with metrics
- Visualization integration
- Model export capabilities
- Error handling and validation
"""

import sys
import os
import json
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm_enhanced as nlm
    print("✓ Successfully imported pynlm_enhanced")
except ImportError as e:
    print(f"✗ Failed to import pynlm_enhanced: {e}")
    print("Please build the enhanced bindings first:")
    print("  mkdir build_python && cd build_python && cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON")
    print("  make -j$(nproc)")
    sys.exit(1)

class TrainingExample:
    """Example class demonstrating training with enhanced bindings."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        self.metrics = []
        
    def setup(self):
        """Set up the training environment."""
        print("Setting up training environment...")
        
        # Create enhanced configuration
        self.config = nlm.config.create_training_config(
            learning_rate=0.001,
            hidden_neurons=1000,
            population_size=200
        )
        
        # Validate configuration
        nlm.validate_config(self.config)
        print("✓ Configuration validated")
        
        # Create brain with validation
        self.brain = nlm.create_brain(
            self.config,
            initialize_now=True,
            name="TrainingExample_Brain"
        )
        print("✓ Brain created and initialized")
        
        # Create world and agent
        self.world = nlm.create_simple_world()
        self.world.configure(
            width=100,
            height=100,
            vision_width=16,
            vision_height=16
        )
        
        self.agent = nlm.create_agent_brain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        print("✓ Training environment setup complete")
    
    def training_callback(self, episode_num, result):
        """Callback function for episode results."""
        print(f"  Episode {episode_num}: "
              f"Reward={result.total_reward:6.2f}, "
              f"Avg={result.average_reward:6.3f}, "
              f"Success={'✓' if result.success else '✗'}")
        
        # Store metrics for later analysis
        self.metrics.append(result)
    
    def run_training(self, num_episodes=100, steps_per_episode=50):
        """Run training with enhanced APIs."""
        print(f"\nStarting training ({num_episodes} episodes, {steps_per_episode} steps each)...")
        
        # Use the enhanced training API
        self.metrics = nlm.train(
            brain=self.brain,
            agent=self.agent,
            world=self.world,
            num_episodes=num_episodes,
            steps_per_episode=steps_per_episode,
            render=False,
            callback=self.training_callback,
            stop_condition=lambda: False  # Run full duration
        )
        
        print("✓ Training completed")
    
    def evaluate(self, num_episodes=20):
        """Evaluate trained model."""
        print(f"\nEvaluating model ({num_episodes} episodes)...")
        
        avg_reward = nlm.evaluate(
            brain=self.brain,
            agent=self.agent,
            world=self.world,
            num_episodes=num_episodes,
            steps_per_episode=100
        )
        
        print(f"✓ Evaluation completed. Average reward: {avg_reward:.3f}")
        return avg_reward
    
    def demonstrate_context_manager(self):
        """Demonstrate context manager usage."""
        print("\nDemonstrating context manager...")
        
        # Create a fresh session for demonstration
        fresh_brain = nlm.create_brain(self.config, initialize_now=False)
        fresh_agent = nlm.create_agent_brain(fresh_brain)
        fresh_world = nlm.create_simple_world()
        fresh_world.configure(50, 50, 8, 8)
        fresh_agent.initialize(fresh_world)
        
        # Use context manager for safe resource management
        with nlm.create_brain_session(fresh_brain, fresh_agent, fresh_world) as session:
            print("  Context manager entered")
            
            # Run several steps
            for step in range(5):
                success = session.step(step, float(step) * 0.1)
                if success:
                    print(f"  Step {step} completed successfully")
                else:
                    print(f"  Step {step} failed")
                    break
            
            # Get current state
            percept = fresh_world.getSensoryPercept()
            agent_action = fresh_agent.decodeMotorCommand()
            print(f"  Current action: {agent_action}")
            
            # Session will be automatically cleaned up on exit
            print("  Context manager exited (automatic cleanup)")
        
        print("✓ Context manager demonstration completed")
    
    def save_and_visualize(self):
        """Save metrics and create visualizations."""
        print("\nSaving metrics and creating visualizations...")
        
        # Save training metrics
        output_dir = Path("training_results")
        output_dir.mkdir(exist_ok=True)
        
        # Save JSON metrics
        metrics_file = output_dir / "training_metrics.json"
        nlm.save_metrics(self.metrics, str(metrics_file), "json")
        print(f"✓ Metrics saved to {metrics_file}")
        
        # Generate training plot
        plot_file = output_dir / "training_progress.png"
        nlm.visualization.plot_training_metrics(
            self.metrics,
            output_file=str(plot_file),
            show_plots=False  # Set to True if you have matplotlib backend
        )
        print(f"✓ Training plot saved to {plot_file}")
        
        # Create web dashboard
        dashboard_dir = output_dir / "dashboard"
        nlm.visualization.create_training_dashboard(
            self.metrics,
            output_dir=str(dashboard_dir)
        )
        print(f"✓ Training dashboard created at {dashboard_dir}")
        
        # Save brain state
        brain_file = output_dir / "final_brain.pkl"
        nlm.export.export_to_pickle(
            self.brain,
            str(brain_file),
            include_weights=True,
            include_config=True
        )
        print(f"✓ Brain model saved to {brain_file}")
        
        # Create summary report
        summary = self._create_summary_report()
        summary_file = output_dir / "summary.json"
        with open(summary_file, 'w') as f:
            json.dump(summary, f, indent=2)
        print(f"✓ Summary report saved to {summary_file}")
        
        print("✓ All outputs created successfully")
    
    def _create_summary_report(self):
        """Create a summary report of training results."""
        if not self.metrics:
            return {}
        
        # Calculate statistics
        total_rewards = [m.total_reward for m in self.metrics if m.success]
        if not total_rewards:
            return {"error": "No successful episodes"}
        
        return {
            "total_episodes": len(self.metrics),
            "successful_episodes": sum(1 for m in self.metrics if m.success),
            "total_reward": sum(total_rewards),
            "average_reward": sum(total_rewards) / len(total_rewards),
            "max_reward": max(total_rewards),
            "min_reward": min(total_rewards),
            "std_reward": (sum((r - sum(total_rewards)/len(total_rewards))**2 for r in total_rewards) / len(total_rewards))**0.5,
            "final_brain_neuron_count": self.brain.getTotalNeuronCount() if self.brain else 0,
            "final_brain_synapse_count": self.brain.getTotalSynapseCount() if self.brain else 0,
            "configuration": {
                "learning_rate": 0.001,
                "hidden_neurons": 1000,
                "population_size": 200
            }
        }
    
    def cleanup(self):
        """Clean up resources."""
        print("\nCleaning up resources...")
        # The enhanced bindings automatically handle cleanup through context managers
        # and proper RAII, but we can explicitly clean up if needed
        print("✓ Cleanup completed")
    
    def run_complete_example(self):
        """Run the complete example workflow."""
        try:
            print("=" * 60)
            print("Enhanced NLM Python Bindings - Complete Example")
            print("=" * 60)
            
            # Step 1: Setup
            self.setup()
            
            # Step 2: Run training
            self.run_training(num_episodes=50, steps_per_episode=30)
            
            # Step 3: Evaluate
            avg_reward = self.evaluate()
            
            # Step 4: Demonstrate context manager
            self.demonstrate_context_manager()
            
            # Step 5: Save and visualize
            self.save_and_visualize()
            
            # Step 6: Show summary
            summary = self._create_summary_report()
            print("\n" + "=" * 60)
            print("TRAINING SUMMARY")
            print("=" * 60)
            print(f"Total Episodes: {summary['total_episodes']}")
            print(f"Successful Episodes: {summary['successful_episodes']}")
            print(f"Average Reward: {summary['average_reward']:.3f}")
            print(f"Max Reward: {summary['max_reward']:.3f}")
            print(f"Brain Neurons: {summary['final_brain_neuron_count']}")
            print(f"Brain Synapses: {summary['final_brain_synapse_count']}")
            print("=" * 60)
            
            print("\n✓ Complete example executed successfully!")
            return True
            
        except Exception as e:
            print(f"\n✗ Example failed: {e}")
            import traceback
            traceback.print_exc()
            return False

class QuickStartExample:
    """Quick start example showing basic usage."""
    
    def run(self):
        """Run quick start example."""
        print("\n" + "=" * 60)
        print("QUICK START - Basic Usage")
        print("=" * 60)
        
        # Minimal setup
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        world = nlm.create_simple_world()
        agent = nlm.create_agent_brain(brain)
        
        print("✓ Created default config, brain, world, and agent")
        
        # Configure world
        world.configure(50, 50, 8, 8)
        agent.initialize(world)
        print("✓ Configured and initialized world")
        
        # Use context manager
        with nlm.create_brain_session(brain, agent, world) as session:
            print("✓ Context manager active")
            
            # Run one step
            session.step(0, 0.0)
            print("✓ Completed simulation step")
            
            # Get observations
            percept = world.getSensoryPercept()
            print(f"✓ Got sensory percept (vision size: {percept.getVisionWidth()}x{percept.getVisionHeight()})")
        
        print("✓ Quick start example completed")

def main():
    """Main function."""
    print("Enhanced NLM Python Bindings Examples")
    print("====================================")
    
    # Run quick start first
    quick_start = QuickStartExample()
    quick_start.run()
    
    # Run complete example
    complete_example = TrainingExample()
    success = complete_example.run_complete_example()
    
    if success:
        print("\n🎉 All examples completed successfully!")
        print("\nNext steps:")
        print("1. Explore the enhanced API in the documentation (ENHANCED_BINDINGS.md)")
        print("2. Build the module with: cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON")
        print("3. Run tests with: ./test_bindings_enhanced")
        print("4. Visit https://nlm.readthedocs.io for more documentation")
    else:
        print("\n❌ Examples completed with errors")
        return 1
    
    return 0
if __name__ == "__main__":
    sys.exit(main())
