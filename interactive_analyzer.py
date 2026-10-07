#!/usr/bin/env python3
"""
Advanced NLM Interactive Analyzer

Interactive command-line interface for analyzing NLM simulations
and experimenting with advanced features.
"""

import advanced_nlm
import sys
from typing import Dict, Any

def print_menu():
    """Print the main menu."""
    print("\n" + "=" * 60)
    print("Advanced NLM Interactive Analyzer")
    print("=" * 60)
    print("1. Create Advanced Brain")
    print("2. Run Dynamics")
    print("3. Analyze Activity Patterns")
    print("4. Set Up Experiment Manager")
    print("5. Data Collection and Analysis")
    print("6. Machine Learning Integration")
    print("7. Visualization Tools")
    print("8. Save/Load Brain State")
    print("9. View Statistics")
    print("0. Exit")
    print("=" * 60)

def create_advanced_brain():
    """Create an advanced brain with configuration."""
    try:
        brain = advanced_nlm.createAdvancedBrain()
        brain.enable_advanced_features(
            reward_modulation=True,
            structural_plasticity=True,
            development=True,
            curiosity=True,
            prediction=True
        )
        return brain
    except Exception as e:
        print(f"Error creating brain: {e}")
        return None

def run_dynamics(brain):
    """Run dynamics with configurable parameters."""
    print("\nEnter number of steps (default: 100):")
    try:
        steps = int(input().strip()) if input().strip() else 100
        print(f"Running {steps} dynamics steps...")
        metrics = brain.run_dynamics(steps, collect_metrics=True)
        print(f"   Completed {steps} steps")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    except Exception as e:
        print(f"Error running dynamics: {e}")

def analyze_activity_patterns(brain):
    """Analyze activity patterns."""
    print("\nEnter window size for pattern analysis (default: 100):")
    try:
        window = int(input().strip()) if input().strip() else 100
        patterns = brain.get_activity_pattern(window)
        print(f"\nActivity Pattern Analysis (window={window}):")
        print(f"   Stability Index: {patterns.get('stability_index', 0):.3f}")
        print(f"   Exploration Index: {patterns.get('exploration_index', 0):.3f}")
        print(f"   Spike Trends: {patterns.get('spike_trends', {})}")
        print(f"   Learning Curve Length: {len(patterns.get('learning_curve', []))}")
    except Exception as e:
        print(f"Error analyzing patterns: {e}")

def setup_experiment_manager():
    """Set up experiment manager."""
    print("\nExperiment Manager Setup:")
    print("1. Create Parameter Sweep")
    print("2. Create Experiment Configuration")
    
    choice = input("Choose option (1-2): ").strip()
    
    if choice == '1':
        print("Enter parameter name (e.g., 'brain.neuron_count'):")
        param_name = input().strip()
        
        print("Enter parameter values (comma-separated):")
        values_input = input().strip()
        try:
            param_values = [float(v.strip()) for v in values_input.split(',')]
            print(f"Creating parameter sweep for '{param_name}' with values: {param_values}")
            
            experiment_manager = advanced_nlm.ExperimentManager()
            sweep_config = experiment_manager.create_parameter_sweep(param_name, param_values, steps=100)
            print("Parameter sweep configured successfully!")
            
        except ValueError as e:
            print(f"Error parsing values: {e}")
            
    elif choice == '2':
        print("Enter experiment type:")
        exp_type = input().strip()
        print(f"Experiment configuration created for type: {exp_type}")
    
    else:
        print("Invalid choice")

def data_collection_analysis(brain):
    """Data collection and analysis demo."""
    print("\nData Collection and Analysis:")
    
    # Create data collector
    data_collector = advanced_nlm.DataCollector()
    
    # Collect data
    data_collector.collect_data(brain, "interactive_session", {
        'session_type': 'interactive_analysis',
        'timestamp': '2026-10-07'
    })
    
    # Apply basic filter
    data_collector.apply_filter(lambda x: x['brain_stats']['total_spikes'])
    
    # Process data
    data_collector.process_data()
    
    # Get summary
    summary = data_collector.get_summary_statistics()
    print(f"\nData Collection Summary:")
    print(f"   Total experiments: {summary.get('total_experiments', 0)}")
    print(f"   Average spikes: {summary.get('avg_spikes_per_experiment', 0):.1f}")
    print(f"   Spike range: {summary.get('spike_range', (0, 0))}")
    
    print("\nData processing complete!")

def ml_integration_demo(brain):
    """Demonstrate machine learning integration."""
    print("\nMachine Learning Integration Demo:")
    
    ml_integration = advanced_nlm.MLIntegration()
    
    # Create sample data for ML demo
    print("Generating sample data for ML demonstration...")
    np.random.seed(42)
    X = np.random.randn(50, 8)  # 50 samples, 8 features
    y = np.random.randint(0, 2, 50)  # Binary labels
    
    # Train model
    print("Training random forest model...")
    model = ml_integration.train_supervised_model(X, y, 'random_forest')
    print(f"Model trained: {model}")
    
    # Make predictions
    print("Making predictions...")
    predictions = ml_integration.predict_with_model('random_forest', X[:5])
    print(f"Sample predictions: {predictions}")
    
    print("\nML integration demo complete!")

def visualization_demo(brain):
    """Demonstrate visualization tools."""
    print("\nVisualization Tools Demo:")
    
    try:
        # Create sample data for visualization
        print("Creating sample visualizations...")
        
        # Spike histogram
        spike_data = np.random.poisson(50, 1000)
        histogram = advanced_nlm.VisualProfiler.create_spike_histogram(spike_data, bins=20)
        print(f"Spike histogram created with {len(histogram['bins'])} bins")
        
        # Activity pattern analysis
        if hasattr(brain, 'metrics_history') and brain.metrics_history:
            activity_analysis = advanced_nlm.VisualProfiler.analyze_activity_patterns(brain.metrics_history)
            print(f"Activity analysis completed:")
            print(f"   Mean firing rate: {activity_analysis.get('mean_firing_rate', 0):.2f} Hz")
            print(f"   Coefficient of variation: {activity_analysis.get('coefficient_of_variation', 0):.3f}")
            print(f"   Trend: {activity_analysis.get('trend', 'unknown')}")
        
        print("\nVisualization demo complete!")
        
    except Exception as e:
        print(f"Visualization error: {e}")

def save_load_brain(brain):
    """Handle brain state save/load operations."""
    print("\nBrain State Operations:")
    print("1. Save Brain State")
    print("2. Load Brain State")
    
    choice = input("Choose option (1-2): ").strip()
    
    if choice == '1':
        filename = input("Enter filename (default: brain_state.bin): ").strip()
        if not filename:
            filename = "brain_state.bin"
        
        try:
            brain.save_brain_state(filename)
            print(f"Brain state saved to: {filename}")
        except Exception as e:
            print(f"Error saving brain state: {e}")
            
    elif choice == '2':
        filename = input("Enter filename to load: ").strip()
        
        try:
            brain.load_brain_state(filename)
            print(f"Brain state loaded from: {filename}")
        except Exception as e:
            print(f"Error loading brain state: {e}")

def view_statistics(brain):
    """View comprehensive brain statistics."""
    print("\nComprehensive Brain Statistics:")
    stats = brain.get_stats()
    
    for key, value in stats.items():
        print(f"   {key.replace('_', ' ').title()}: {value}")
    
    print(f"\nMetrics history length: {len(brain.metrics_history) if hasattr(brain, 'metrics_history') else 0}")

def main_interactive():
    """Main interactive analyzer function."""
    brain = None
    
    print("Advanced NLM Interactive Analyzer")
    print("This tool provides comprehensive analysis and experimentation capabilities for NLM simulations.")
    
    while True:
        print_menu()
        
        try:
            choice = input("Enter your choice (0-9): ").strip()
            
            if choice == '0':
                print("\nThank you for using Advanced NLM Interactive Analyzer!")
                break
                
            elif choice == '1':
                brain = create_advanced_brain()
                if brain:
                    print("Advanced brain created successfully!")
                
            elif choice == '2':
                if brain:
                    run_dynamics(brain)
                else:
                    print("Please create an advanced brain first (option 1).")
                    
            elif choice == '3':
                if brain:
                    analyze_activity_patterns(brain)
                else:
                    print("Please create an advanced brain first (option 1).")
                    
            elif choice == '4':
                setup_experiment_manager()
                
            elif choice == '5':
                if brain:
                    data_collection_analysis(brain)
                else:
                    print("Please create an advanced brain first (option 1).")
                    
            elif choice == '6':
                ml_integration_demo(brain if brain else None)
                
            elif choice == '7':
                if brain:
                    visualization_demo(brain)
                else:
                    print("Please create an advanced brain first (option 1).")
                    
            elif choice == '8':
                if brain:
                    save_load_brain(brain)
                else:
                    print("Please create an advanced brain first (option 1).")
                    
            elif choice == '9':
                if brain:
                    view_statistics(brain)
                else:
                    print("Please create an advanced brain first (option 1).")
                    
            else:
                print("Invalid choice. Please enter a number between 0 and 9.")
                
        except KeyboardInterrupt:
            print("\n\nInterrupted by user. Exiting...")
            break
        except Exception as e:
            print(f"\nError: {e}")
            print("Please try again.")

if __name__ == "__main__":
    try:
        main_interactive()
    except ImportError as e:
        print(f"Import error: {e}")
        print("Please ensure all dependencies are installed:")
        print("pip install advanced-nlm[dev]")
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)
