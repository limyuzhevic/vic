#!/usr/bin/env python3
"""
NLM Advanced User Examples and Tutorials
======================================

Comprehensive examples and tutorials for advanced NLM users.
This provides in-depth examples for research scientists, developers, and advanced users.
"""

import numpy as np
import matplotlib.pyplot as plt
import json
import sys
import os
from pathlib import Path

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src', 'python'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install it first.")
    print("pip install pynlm")
    sys.exit(1)

class NLMAdvancedExamples:
    """Advanced examples for NLM users."""
    
    @staticmethod
    def example_research_workflow():
        """Example 1: Research Workflow - Developing and Testing Neural Architectures"""
        print("=== Example 1: Research Workflow ===")
        print("This example demonstrates a complete research workflow:")
        print("  1. Hypothesis formation")
        print("  2. Experimental design")
        print("  3. Parameter tuning")
        print("  4. Data collection and analysis")
        print("  5. Results interpretation")
        print()
        
        # Define research hypothesis
        hypothesis = {
            "title": "Effect of Network Connectivity on Learning Speed",
            "null_hypothesis": "Network connectivity has no effect on learning speed",
            "alternative_hypothesis": "Higher connectivity leads to faster learning",
            "variables": {
                "independent": "connection_probability",
                "dependent": "learning_rate",
                "controls": ["neuron_count", "simulation_timestep"]
            },
            "prediction": "Increasing connection probability from 0.1 to 0.3 should decrease learning time by 50%"
        }
        
        print(f"Research Hypothesis: {hypothesis['title']}")
        print(f"  Independent Variable: {hypothesis['variables']['independent']}")
        print(f"  Dependent Variable: {hypothesis['variables']['dependent']}")
        print(f"  Prediction: {hypothesis['prediction']}")
        print()
        
        # Experimental design
        experimental_conditions = [
            {"connection_probability": 0.1, "neuron_count": 100, "simulation_timestep": 0.001},
            {"connection_probability": 0.2, "neuron_count": 100, "simulation_timestep": 0.001},
            {"connection_probability": 0.3, "neuron_count": 100, "simulation_timestep": 0.001},
            {"connection_probability": 0.5, "neuron_count": 100, "simulation_timestep": 0.001}
        ]
        
        print("Experimental Design:")
        for i, condition in enumerate(experimental_conditions):
            print(f"  Condition {i+1}: {condition}")
        
        # Run simulations
        results = []
        for i, condition in enumerate(experimental_conditions):
            print(f"\nRunning simulation {i+1}...")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", condition["neuron_count"], pynlm.ConfigSource.Default)
            config.set("connection_probability", condition["connection_probability"], pynlm.ConfigSource.Default)
            config.set("random_seed", i + 42, pynlm.ConfigSource.Default)
            
            # Create brain and run simulation
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Collect baseline metrics
            baseline_neurons = brain->getTotalNeuronCount()
            baseline_synapses = brain->getTotalSynapseCount()
            
            # Run learning simulation
            learningStarts = brain->getTotalSpikeCount();
            for step in range(500):
                brain->step(step, step * condition["simulation_timestep"])
                
                # Apply learning input
                if step % 50 == 0:
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f)
            
            learningEnds = brain->getTotalSpikeCount();
            finalFiringRate = brain->getAverageFiringRate();
            
            result = {
                "condition": i + 1,
                "connection_probability": condition["connection_probability"],
                "baseline_neurons": baseline_neurons,
                "baseline_synapses": baseline_synapses,
                "learning_starts": learningStarts,
                "learning_ends": learningEnds,
                "final_firing_rate": finalFiringRate,
                "learning_effectiveness": (learningEnds - learningStarts) / std::max(1.0, learningStarts)
            }
            
            results.append(result)
            
            print(f"  Result: {result}")
        
        # Analyze results
        print("\n=== Results Analysis ===")
        
        # Find relationship between connectivity and learning
        connectivity_effects = []
        for result in results:
            connectivity_effects.append({
                "sparsity": 1.0 - result["connection_probability"],
                "learning_effectiveness": result["learning_effectiveness"],
                "final_firing_rate": result["final_firing_rate"]
            })
        
        # Calculate correlation
        sparsity_values = [effect["sparsity"] for effect in connectivity_effects]
        effectiveness_values = [effect["learning_effectiveness"] for effect in connectivity_effects]
        
        if len(sparsity_values) > 1:
            correlation = np.corrcoef(sparsity_values, effectiveness_values)[0, 1]
            print(f"Correlation between sparsity and learning effectiveness: {correlation:.3f}")
            
            if correlation < -0.5:
                print("  Strong negative correlation: More connectivity → Better learning (supports alternative hypothesis)")
            elif correlation > 0.5:
                print("  Strong positive correlation: More connectivity → Worse learning")
            else:
                print("  Weak correlation: No clear relationship")
        
        # Print summary
        print("\n=== Summary ===")
        print("Research experiment completed successfully!")
        print(f"Ran {len(experimental_conditions)} experimental conditions")
        print("Data collected and analyzed according to research hypothesis")
        print("Results ready for interpretation and publication")
        
        return results
        
    @staticmethod
    def example_performance_optimization():
        """Example 2: Performance Optimization - Finding Optimal Network Parameters"""
        print("=== Example 2: Performance Optimization ===")
        print("This example demonstrates performance optimization techniques:")
        print("  1. Benchmarking different configurations")
        print("  2. Finding optimal parameters")
        print("  3. Performance profiling")
        print("  4. Resource optimization")
        print()
        
        # Define parameter space to explore
        parameter_space = {
            "neuron_counts": [50, 100, 200, 500],
            "connection_probabilities": [0.05, 0.1, 0.15, 0.2],
            "simulation_timesteps": [0.0005, 0.001, 0.002],
            "plasticity_rates": [0.00001, 0.00005, 0.0001]
        }
        
        print("Parameter space to explore:")
        for param, values in parameter_space.items():
            print(f"  {param}: {values}")
        
        # Simulated parameter tuning
        best_config = None
        best_score = -1
        
        print("\nRunning parameter optimization...")
        
        config_index = 0
        for neuron_count in parameter_space["neuron_counts"]:
            for conn_prob in parameter_space["connection_probabilities"]:
                for timestep in parameter_space["simulation_timesteps"]:
                    for plasticity in parameter_space["plasticity_rates"]:
                        config_index += 1
                        
                        print(f"  Testing config {config_index}: {neuron_count} neurons, "
                              f"{conn_prob} connectivity, {timestep} timestep, {plasticity} plasticity")
                        
                        # Create configuration
                        config = pynlm.createDefaultConfig()
                        config.set("neuron_count", neuron_count, pynlm.ConfigSource.Default)
                        config.set("connection_probability", conn_prob, pynlm.ConfigSource.Default)
                        config.set("simulation_timestep", timestep, pynlm.ConfigSource.Default)
                        config.set("synaptogenesis_rate", plasticity, pynlm.ConfigSource.Default)
                        
                        # Create brain and run benchmark
                        brain = pynlm.createBrain(config)
                        brain.initialize()
                        
                        # Warm-up
                        for step in range(50):
                            brain->step(step, step * timestep)
                        
                        # Benchmark
                        import time
                        start_time = time.time()
                        
                        for step in range(500):
                            brain->step(step, step * timestep)
                            if step % 100 == 0:
                                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f)
                        
                        end_time = time.time()
                        duration = end_time - start_time
                        
                        # Calculate performance score
                        neurons = brain->getTotalNeuronCount()
                        synapses = brain->getTotalSynapseCount()
                        spikes = brain->getTotalSpikeCount()
                        firing_rate = brain->getAverageFiringRate()
                        
                        # Performance metrics
                        processing_speed = 500 / duration  # steps per second
                        memory_efficiency = neurons / max(1.0, spikes)
                        firing_stability = 1.0 / (1.0 + abs(firing_rate - 50.0) / 50.0)  # Optimal around 50 Hz
                        
                        # Combined score (higher is better)
                        score = processing_speed * 0.4 + memory_efficiency * 0.3 + firing_stability * 0.3
                        
                        print(f"    Performance: {processing_speed:.1f} steps/s, "
                              f"efficiency: {memory_efficiency:.1f}, stability: {firing_stability:.2f}")
                        print(f"    Score: {score:.2f}")
                        
                        # Update best configuration
                        if score > best_score:
                            best_score = score
                            best_config = {
                                "neuron_count": neuron_count,
                                "connection_probability": conn_prob,
                                "simulation_timestep": timestep,
                                "plasticity_rate": plasticity,
                                "processing_speed": processing_speed,
                                "memory_efficiency": memory_efficiency,
                                "firing_stability": firing_stability,
                                "score": score
                            }
        
        print("\n=== Optimization Results ===")
        if best_config:
            print("Optimal configuration found:")
            for key, value in best_config.items():
                if key != "score":
                    print(f"  {key}: {value}")
            
            print(f"\nOverall performance score: {best_score:.2f}")
            print("This configuration should provide the best balance of speed, efficiency, and stability")
        
        return best_config
        
    @staticmethod
    def example_advanced_analysis():
        """Example 3: Advanced Analysis - Complex Data Processing"""
        print("=== Example 3: Advanced Analysis ===")
        print("This example demonstrates advanced data analysis techniques:")
        print("  1. Statistical analysis of neural data")
        print("  2. Pattern recognition in neural activity")
        print("  3. Machine learning integration")
        print("  4. Visualization and interpretation")
        print()
        
        # Create a complex simulation for analysis
        print("Running complex simulation for analysis...")
        
        # Multi-stage learning simulation
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300, pynlm.ConfigSource.Default)
        config.set("region_count", 3, pynlm.ConfigSource.Default)
        config.set("connection_probability", 0.12, pynlm.ConfigSource.Default)
        config.set("random_seed", 12345, pynlm.ConfigSource.Default)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Collect time-series data
        time_series_data = []
        
        print("Collecting neural activity data...")
        for step in range(1000):
            brain->step(step, step * 0.001)
            
            # Record various metrics
            data_point = {
                "step": step,
                "time": step * 0.001,
                "neuron_count": brain->getTotalNeuronCount(),
                "synapse_count": brain->getTotalSynapseCount(),
                "active_neurons": brain->getActiveNeuronCount(),
                "firing_neurons": brain->getFiringNeuronCount(),
                "average_firing_rate": brain->getAverageFiringRate(),
                "exc_inh_ratio": brain->getExcitationInhibitionRatio(),
                "total_spikes": brain->getTotalSpikeCount()
            }
            
            time_series_data.append(data_point)
            
            if step % 200 == 0:
                print(f"  Step {step}/1000 collected")
        
        print("Data collection complete. Analyzing patterns...")
        
        # Statistical analysis
        print("\n=== Statistical Analysis ===")
        
        # Calculate statistics
        neurons = [d["neuron_count"] for d in time_series_data]
        synapses = [d["synapse_count"] for d in time_series_data]
        firing_rates = [d["average_firing_rate"] for d in time_series_data]
        activities = [d["active_neurons"] for d in time_series_data]
        
        print(f"Neuron count: mean={np.mean(neurons):.1f}, std={np.std(neurons):.1f}")
        print(f"Synapse count: mean={np.mean(synapses):.1f}, std={np.std(synapses):.1f}")
        print(f"Firing rate: mean={np.mean(firing_rates):.2f} Hz, std={np.std(firing_rates):.2f} Hz")
        print(f"Activity level: mean={np.mean(activities):.1f}, std={np.std(activities):.1f}")
        
        # Pattern detection
        print("\n=== Pattern Detection ===")
        
        # Detect activity bursts
        activity_changes = []
        for i in range(1, len(activities)):
            change = activities[i] - activities[i-1]
            activity_changes.append(change)
        
        positive_changes = sum(1 for change in activity_changes if change > 10)
        negative_changes = sum(1 for change in activity_changes if change < -10)
        
        print(f"Activity increases (>10 neurons): {positive_changes}")
        print(f"Activity decreases (<-10 neurons): {negative_changes}")
        
        # Detect oscillatory patterns
        if len(firing_rates) >= 10:
            # Simple frequency analysis
            mean_firing = np.mean(firing_rates)
            std_firing = np.std(firing_rates)
            cv = std_firing / max(1.0, mean_firing)  # Coefficient of variation
            
            print(f"Firing rate variability (CV): {cv:.3f}")
            if cv < 0.1:
                print("  Low variability: Stable, regular firing")
            elif cv < 0.3:
                print("  Moderate variability: Balanced dynamics")
            else:
                print("  High variability: Rich dynamic patterns")
        
        # Time-series visualization preparation
        print("\n=== Time-Series Visualization ===")
        print("Data ready for visualization:")
        print("  X-axis: Simulation time (s)")
        print("  Y-axis: Various neural metrics")
        print("  Multiple subplots for different metrics")
        print("  Color coding for different phases")
        
        # Save data for further analysis
        analysis_data = {
            "metadata": {
                "simulation_steps": len(time_series_data),
                "total_neurons": neurons[0],
                "final_synapses": synapses[-1],
                "analysis_timestamp": str(np.datetime64('now'))
            },
            "time_series": time_series_data,
            "statistics": {
                "neuron_stats": {"mean": np.mean(neurons), "std": np.std(neurons)},
                "synapse_stats": {"mean": np.mean(synapses), "std": np.std(synapses)},
                "firing_stats": {"mean": np.mean(firing_rates), "std": np.std(firing_rates)},
                "activity_stats": {"mean": np.mean(activities), "std": np.std(activities)}
            }
        }
        
        # Save to file
        with open("advanced_analysis_data.json", "w") as f:
            json.dump(analysis_data, f, indent=2, default=str)
        
        print(f"Analysis data saved to 'advanced_analysis_data.json'")
        print(f"Total data points: {len(time_series_data)}")
        
        return analysis_data
        
    @staticmethod
    def example_plugin_development():
        """Example 4: Plugin Development - Creating Custom NLM Extensions"""
        print("=== Example 4: Plugin Development ===")
        print("This example demonstrates how to develop custom NLM plugins:")
        print("  1. Plugin architecture overview")
        print("  2. Creating a custom plugin")
        print("  3. Plugin integration and testing")
        print("  4. Plugin distribution and sharing")
        print()
        
        print("Plugin Development Workflow:")
        print("  1. Define plugin interface and functionality")
        print("  2. Implement plugin class with required methods")
        print("  3. Test plugin integration with NLM")
        print("  4. Package plugin for distribution")
        print("  5. Document plugin API and usage")
        print()
        
        print("Example Plugin Structure:")
        print("  ```")
        print("  my_custom_plugin/")
        print("  ├── include/")
        print("  │   └── my_plugin_interface.hpp")
        print("  ├── src/")
        print("  │   ├── plugin.cpp")
        print("  │   └── plugin_impl.cpp")
        print("  ├── test/")
        print("  │   └── test_plugin.cpp")
        print("  ├── docs/")
        print("  │   └── plugin_documentation.md")
        print("  └── examples/")
        print("      └── usage_example.cpp")
        print("  ```")
        print()
        
        print("Plugin Integration Steps:")
        print("  1. Include NLM plugin headers")
        print("  2. Implement PluginInterface virtual methods")
        print("  3. Register plugin with PluginManager")
        print("  4. Test plugin functionality")
        print("  5. Build and distribute plugin")
        print()
        
        print("Plugin Categories:")
        print("  - Core plugins: Essential NLM functionality")
        print("  - Memory plugins: Memory system extensions")
        print("  - Visualization plugins: Custom visualization tools")
        print("  - Analysis plugins: Data analysis and processing")
        print("  - Research plugins: Experiment and hypothesis tools")
        print("  - Integration plugins: External system connections")
        print()
        
        print("Plugin Development Resources:")
        print("  - NLM Plugin Framework documentation")
        print("  - Plugin template repository")
        print("  - API reference and examples")
        print("  - Build and distribution scripts")
        print("  - Community forum and support")
        print()
        
        print("Next Steps for Plugin Development:")
        print("  1. Clone NLM plugin development repository")
        print("  2. Choose a plugin category and create your plugin")
        print("  3. Implement core functionality")
        print("  4. Test integration with NLM")
        print("  5. Create comprehensive documentation")
        print("  6. Package and share with the community")
        
        return True
        
    @staticmethod
    def example_research_paper_example():
        """Example 5: Research Paper - Writing Results for Publication"""
        print("=== Example 5: Research Paper Preparation ===")
        print("This example demonstrates how to structure research results for publication:")
        print("  1. Organizing experimental data")
        print("  2. Statistical analysis and validation")
        print("  3. Results presentation")
        print("  4. Discussion and interpretation")
        print("  5. Future research directions")
        print()
        
        print("Research Paper Structure:")
        print("  Abstract")
        print("    - Background and motivation")
        print("    - Key findings")
        print("    - Significance")
        print()
        print("  Introduction")
        print("    - Problem statement")
        print("    - Literature review")
        print("    - Research questions")
        print("    - Hypotheses")
        print()
        print("  Methods")
        print("    - Experimental design")
        print("    - Materials and procedures")
        print("    - Data analysis methods")
        print()
        print("  Results")
        print("    - Descriptive statistics")
        print("    - Inferential statistics")
        print("    - Visualization")
        print()
        print("  Discussion")
        print("    - Interpretation of findings")
        print("    - Limitations")
        print("    - Future research")
        print()
        print("  Conclusion")
        print("    - Summary of contributions")
        print("    - Implications")
        print("    - Future directions")
        print()
        
        print("Example Paper Section - Results:")
        print("  ```")
        print("  ## Results")
        print("  ")
        print("  ### Experimental Overview")
        print("  We investigated the effects of network connectivity on learning dynamics")
        print("  using NLM simulations across 4 experimental conditions (n=100 neurons each).")
        print("  ")
        print("  ### Neural Activity Patterns")
        print("  - Mean firing rate: 45.2 ± 12.3 Hz (mean ± SD)")
        print("  - Network stability: CV = 0.28 ± 0.05")
        print("  - Learning efficiency: 85.3% ± 4.2%")
        print("  ")
        print("  ### Statistical Analysis")
        print("  - Correlation between connectivity and learning: r = -0.72 (p < 0.01)")
        print("  - Analysis of variance: F(3, 12) = 8.45, p = 0.003")
        print("  - Post-hoc Tukey test: Significant differences between all conditions")
        print("  ```")
        print()
        
        print("Key Points for Research Writing:")
        print("  1. Clear and concise presentation of results")
        print("  2. Proper statistical analysis and interpretation")
        print("  3. Discussion of limitations and confounding factors")
        print("  4. Comparison with existing literature")
        print("  5. Future research directions and applications")
        print()
        
        return True

def main():
    """Run all example demonstrations."""
    print("NLM Advanced Examples and Tutorials")
    print("==================================")
    print()
    
    examples = NLMAdvancedExamples()
    
    # Run examples based on command line arguments
    if len(sys.argv) > 1:
        example_choice = sys.argv[1].lower()
        
        if example_choice == "1":
            examples.example_research_workflow()
        elif example_choice == "2":
            examples.example_performance_optimization()
        elif example_choice == "3":
            examples.example_advanced_analysis()
        elif example_choice == "4":
            examples.example_plugin_development()
        elif example_choice == "5":
            examples.example_research_paper_example()
        else:
            print(f"Unknown example: {example_choice}")
            print("Available examples: 1-5")
            return
    else:
        print("Available examples:")
        print("  1 - Research Workflow (experimental design and analysis)")
        print("  2 - Performance Optimization (parameter tuning)")
        print("  3 - Advanced Analysis (complex data processing)")
        print("  4 - Plugin Development (creating NLM plugins)")
        print("  5 - Research Paper (writing results for publication)")
        print()
        print("Run with: python nlm_advanced_examples.py <example_number>")
        
        # Run all examples for demonstration
        print("\n" + "="*50)
        print("RUNNING ALL EXAMPLES FOR DEMONSTRATION")
        print("="*50)
        
        examples.example_research_workflow()
        examples.example_performance_optimization()
        examples.example_advanced_analysis()
        examples.example_plugin_development()
        examples.example_research_paper_example()
    
    print("\n" + "="*50)
    print("ALL EXAMPLES COMPLETED SUCCESSFULLY")
    print("="*50)

if __name__ == "__main__":
    main()
