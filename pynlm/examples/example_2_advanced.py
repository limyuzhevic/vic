#!/usr/bin/env python3
"""
NLM - Example 2: Advanced Usage

This example demonstrates advanced NLM usage with comprehensive features
including learning, monitoring, and optimization.
"""

import time
import json
import psutil
import os
import matplotlib.pyplot as plt
from nlm import (
    AdvancedNLM, 
    Simulation, 
    EventType, 
    create_config,
    get_default_config,
    setup_logging,
    logging
)

# Setup logging
setup_logging(logging.INFO)

class AdvancedNLMExample:
    """
    Advanced NLM example class demonstrating comprehensive features.
    """
    
    def __init__(self, config_name="default"):
        """
        Initialize advanced NLM example.
        
        Args:
            config_name: Configuration type ("default", "minimal", "research")
        """
        self.config_name = config_name
        self.brain = None
        self.simulation = None
        self.results = {}
        self.event_data = []
        
        # Create brain with appropriate configuration
        self._create_brain()
        
        # Setup event monitoring
        self._setup_event_monitoring()
        
        print(f"Advanced NLM example initialized with {config_name} configuration")
    
    def _create_brain(self):
        """Create brain with specified configuration."""
        if self.config_name == "minimal":
            from nlm import get_minimal_config
            self.brain = AdvancedNLM(get_minimal_config())
        elif self.config_name == "research":
            from nlm import get_research_config
            self.brain = AdvancedNLM(get_research_config())
        else:
            self.brain = AdvancedNLM(get_default_config())
    
    def _setup_event_monitoring(self):
        """Setup comprehensive event monitoring."""
        
        def on_step_completed(event):
            self._handle_step_completed(event)
        
        def on_simulation_started(event):
            self._handle_simulation_started(event)
        
        def on_simulation_stopped(event):
            self._handle_simulation_stopped(event)
        
        def on_error(event):
            self._handle_error(event)
        
        # Add event listeners
        self.simulation = Simulation(self.brain)
        self.simulation.add_event_listener(EventType.STEP_COMPLETED, on_step_completed)
        self.simulation.add_event_listener(EventType.SIMULATION_STARTED, on_simulation_started)
        self.simulation.add_event_listener(EventType.SIMULATION_STOPPED, on_simulation_stopped)
        self.simulation.add_event_listener(EventType.ERROR_OCCURRED, on_error)
    
    def _handle_step_completed(self, event):
        """Handle step completion events."""
        self.event_data.append({
            'step': event.step,
            'timestamp': event.timestamp,
            'data': event.data
        })
        
        # Log step information
        if event.step % 50 == 0:
            print(f"   Step {event.step}: "
                  f"Firing rate={event.data.get('firing_rate', 0):.2f}Hz, "
                  f"Spikes={event.data.get('spike_count', 0)}, "
                  f"Firing neurons={event.data.get('firing_neurons', 0)}")
    
    def _handle_simulation_started(self, event):
        """Handle simulation started events."""
        print(f"   Simulation started: {event.data['num_steps']} steps planned")
    
    def _handle_simulation_stopped(self, event):
        """Handle simulation stopped events."""
        elapsed_time = time.time() - self.results.get('start_time', time.time())
        
        self.results.update({
            'end_time': time.time(),
            'elapsed_time': elapsed_time,
            'total_steps': event.step,
            'events_collected': len(self.event_data),
        })
        
        print(f"   Simulation stopped: {event.data['total_steps']} steps completed")
        print(f"   Duration: {elapsed_time:.2f} seconds")
        print(f"   Speed: {event.data['steps_per_second']:.1f} steps/second")
    
    def _handle_error(self, event):
        """Handle error events."""
        print(f"   ERROR at step {event.step}: {event.data}")
    
    def configure_learning_systems(self, reward=True, structural=True, development=True, curiosity=True):
        """
        Configure learning systems.
        
        Args:
            reward: Enable reward modulation
            structural: Enable structural plasticity
            development: Enable development
            curiosity: Enable curiosity
        """
        if reward:
            self.brain.brain.enableRewardModulation(True)
            print("   - Reward modulation enabled")
        
        if structural:
            self.brain.brain.enableStructuralPlasticity(True)
            print("   - Structural plasticity enabled")
        
        if development:
            self.brain.brain.enableDevelopment(True)
            print("   - Development enabled")
        
        if curiosity:
            self.brain.brain.enableCuriosity(True)
            print("   - Curiosity enabled")
    
    def run_basic_simulation(self, steps=100):
        """
        Run basic simulation.
        
        Args:
            steps: Number of steps to simulate
        """
        print(f"\n   Running basic simulation: {steps} steps")
        
        # Clear previous data
        self.event_data.clear()
        
        # Start timing
        self.results['start_time'] = time.time()
        
        # Run simulation
        self.simulation.run(steps)
        
        # Collect basic statistics
        self.results['basic_stats'] = {
            'steps_completed': steps,
            'total_spikes': self.brain.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.brain.getExcitationInhibitionRatio(),
        }
        
        print(f"   Basic simulation completed")
        print(f"   - Total spikes: {self.results['basic_stats']['total_spikes']}")
        print(f"   - Firing neurons: {self.results['basic_stats']['firing_neurons']}")
        print(f"   - Average firing rate: {self.results['basic_stats']['average_firing_rate']:.2f} Hz")
    
    def run_learning_simulation(self, steps=300):
        """
        Run learning simulation.
        
        Args:
            steps: Number of steps to simulate
        """
        print(f"\n   Running learning simulation: {steps} steps")
        
        # Configure learning systems
        self.configure_learning_systems()
        
        # Clear previous data
        self.event_data.clear()
        self.results['start_time'] = time.time()
        
        # Run simulation
        self.simulation.run(steps)
        
        # Collect learning statistics
        self.results['learning_stats'] = {
            'steps_completed': steps,
            'total_spikes': self.brain.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.brain.getExcitationInhibitionRatio(),
            'plasticity_summary': self.brain.enhanced_brain.get_plasticity_summary(),
        }
        
        print(f"   Learning simulation completed")
        print(f"   - Learning rate: {self.results['learning_stats']['plasticity_summary']['stdp_learning_rate']:.4f}")
    
    def run_stability_test(self, steps=200):
        """
        Run stability test.
        
        Args:
            steps: Number of steps to simulate
        """
        print(f"\n   Running stability test: {steps} steps")
        
        # Clear previous data
        self.event_data.clear()
        self.results['start_time'] = time.time()
        
        # Run simulation without learning
        self.configure_learning_systems(reward=False, structural=False)
        self.simulation.run(steps)
        
        # Analyze stability
        stability_analysis = self.brain.enhanced_brain.analyze_stability(steps)
        
        self.results['stability_test'] = {
            'steps_completed': steps,
            'stability': stability_analysis['stability'],
            'recommendations': stability_analysis['recommendations'],
            'final_spike_count': self.brain.brain.getTotalSpikeCount(),
            'final_firing_rate': self.brain.brain.getAverageFiringRate(),
        }
        
        print(f"   Stability test completed")
        print(f"   - Stability: {self.results['stability_test']['stability']}")
        print(f"   - Recommendations: {self.results['stability_test']['recommendations']}")
    
    def run_performance_test(self, steps=100):
        """
        Run performance test with monitoring.
        
        Args:
            steps: Number of steps to simulate
        """
        print(f"\n   Running performance test: {steps} steps")
        
        # Get initial memory usage
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024
        initial_cpu = process.cpu_percent()
        
        # Clear previous data
        self.event_data.clear()
        self.results['start_time'] = time.time()
        
        # Define performance monitoring callback
        def monitor_performance(step, brain):
            current_memory = process.memory_info().rss / 1024 / 1024
            current_cpu = process.cpu_percent()
            
            if step % 25 == 0:
                print(f"   Step {step}: Memory={current_memory:.1f}MB, CPU={current_cpu:.1f}%")
        
        # Run simulation with performance monitoring
        self.simulation.run(steps, callback=monitor_performance)
        
        # Get final memory usage
        final_memory = process.memory_info().rss / 1024 / 1024
        
        self.results['performance_test'] = {
            'steps_completed': steps,
            'initial_memory_mb': initial_memory,
            'final_memory_mb': final_memory,
            'memory_delta': final_memory - initial_memory,
            'initial_cpu_percent': initial_cpu,
            'final_cpu_percent': process.cpu_percent(),
            'performance_stats': self.brain.enhanced_brain.get_performance_stats(),
        }
        
        print(f"   Performance test completed")
        print(f"   - Memory delta: {self.results['performance_test']['memory_delta']:.1f} MB")
    
    def analyze_activities(self, steps=100):
        """
        Analyze brain activities.
        
        Args:
            steps: Number of steps to analyze
        """
        print(f"\n   Analyzing brain activities: {steps} steps")
        
        # Get activity patterns
        patterns = self.brain.enhanced_brain.get_activity_patterns(steps)
        
        self.results['activity_analysis'] = {
            'patterns_collected': len(patterns),
            'pattern_names': list(patterns.keys()),
            'pattern_dimensions': {name: data.shape for name, data in patterns.items()},
        }
        
        # Find active regions
        active_regions = self.brain.enhanced_brain.find_active_regions(threshold=0.1)
        
        self.results['active_regions'] = {
            'count': len(active_regions),
            'regions': active_regions,
        }
        
        print(f"   Activity analysis completed")
        print(f"   - Patterns collected: {len(patterns)}")
        print(f"   - Active regions: {len(active_regions)}")
    
    def export_results(self):
        """Export all results to files."""
        print(f"\n   Exporting results...")
        
        # Export events
        self.simulation.export_events(f"{self.config_name}_simulation_events.json")
        
        # Export metrics
        self.simulation.export_metrics(f"{self.config_name}_simulation_metrics.json")
        
        # Export brain model
        self.brain.save_brain(f"{self.config_name}_brain_model.bin")
        
        # Export results summary
        with open(f"{self.config_name}_results_summary.json", "w") as f:
            json.dump(self.results, f, indent=2)
        
        print(f"   - Events exported: {self.config_name}_simulation_events.json")
        print(f"   - Metrics exported: {self.config_name}_simulation_metrics.json")
        print(f"   - Brain model saved: {self.config_name}_brain_model.bin")
        print(f"   - Results summary: {self.config_name}_results_summary.json")
    
    def visualize_results(self, output_path="advanced_example_results.png"):
        """
        Visualize all results.
        
        Args:
            output_path: Output file path
        """
        if not self.event_data:
            print("   No event data available for visualization")
            return
        
        # Extract data from event data
        steps = [event['step'] for event in self.event_data]
        spike_counts = [event['data'].get('spike_count', 0) for event in self.event_data]
        firing_rates = [event['data'].get('firing_rate', 0) for event in self.event_data]
        firing_neurons = [event['data'].get('firing_neurons', 0) for event in self.event_data]
        
        # Create visualization
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle(f"Advanced NLM Example: {self.config_name.title()} Configuration", fontsize=16)
        
        # Plot 1: Spike count over time
        axes[0, 0].plot(steps, spike_counts, 'r-', linewidth=2, label='Spike Count')
        axes[0, 0].set_xlabel('Simulation Steps')
        axes[0, 0].set_ylabel('Spike Count')
        axes[0, 0].set_title('Spike Count Over Time')
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Firing rate over time
        axes[0, 1].plot(steps, firing_rates, 'b-', linewidth=2, label='Firing Rate')
        axes[0, 1].set_xlabel('Simulation Steps')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        axes[0, 1].set_title('Firing Rate Over Time')
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Firing neurons over time
        axes[1, 0].plot(steps, firing_neurons, 'g-', linewidth=2, label='Firing Neurons')
        axes[1, 0].set_xlabel('Simulation Steps')
        axes[1, 0].set_ylabel('Number of Neurons')
        axes[1, 0].set_title('Active Neurons Over Time')
        axes[1, 0].legend()
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Results summary
        axes[1, 1].axis('off')
        
        summary_text = f"Simulation Results:\n"
        summary_text += f"  Steps completed: {len(steps)}\n"
        summary_text += f"  Total spikes: {sum(spike_counts)}\n"
        summary_text += f"  Final firing rate: {firing_rates[-1]:.2f} Hz\n"
        summary_text += f"  Active regions: {self.results.get('active_regions', {}).get('count', 'N/A')}\n"
        summary_text += f"  Stability: {self.results.get('stability_test', {}).get('stability', 'N/A')}"
        
        axes[1, 1].text(0.1, 0.5, summary_text, fontsize=12, 
                       verticalalignment='center', fontfamily='monospace')
        axes[1, 1].set_title('Results Summary')
        
        # Adjust layout
        plt.tight_layout()
        
        # Save visualization
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"   Visualization saved to: {output_path}")
    
    def run_complete_workflow(self):
        """Run complete workflow with all features."""
        print(f"\n{'=' * 60}")
        print(f"Running Complete Advanced NLM Workflow")
        print(f"{'=' * 60}")
        
        # Step 1: Run basic simulation
        self.run_basic_simulation(100)
        
        # Step 2: Run learning simulation
        self.run_learning_simulation(300)
        
        # Step 3: Run stability test
        self.run_stability_test(200)
        
        # Step 4: Run performance test
        self.run_performance_test(100)
        
        # Step 5: Analyze activities
        self.analyze_activities(150)
        
        # Step 6: Export results
        self.export_results()
        
        # Step 7: Visualize results
        self.visualize_results()
        
        print(f"\n{'=' * 60}")
        print(f"Advanced NLM workflow completed successfully!")
        print(f"{'=' * 60}")
        
        return self.results


def main():
    """
    Main function demonstrating advanced NLM usage.
    """
    print("NLM Example 2: Advanced Usage")
    print("This example demonstrates advanced NLM features including learning,")
    print("monitoring, optimization, and comprehensive analysis.")
    
    # Create advanced NLM examples
    examples = {
        "default": AdvancedNLMExample("default"),
        "minimal": AdvancedNLMExample("minimal"),
        "research": AdvancedNLMExample("research"),
    }
    
    try:
        # Run each example
        for config_name, example in examples.items():
            print(f"\n{'=' * 60}")
            print(f"Running Advanced NLM Example: {config_name.title()} Configuration")
            print(f"{'=' * 60}")
            
            # Run complete workflow
            results = example.run_complete_workflow()
            
            # Print summary
            print(f"\n{'-' * 60}")
            print(f"Example '{config_name}' completed successfully!")
            print(f"{'-' * 60}")
        
        print(f"\n{'=' * 60}")
        print("ALL ADVANCED EXAMPLES COMPLETED SUCCESSFULLY!")
        print(f"{'=' * 60}")
        
        print("\nGenerated files for each configuration:")
        for config_name in examples.keys():
            print(f"  - {config_name}/simulation_events.json")
            print(f"  - {config_name}/simulation_metrics.json")
            print(f"  - {config_name}/brain_model.bin")
            print(f"  - {config_name}/results_summary.json")
            print(f"  - {config_name}_results.png (visualization)")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()