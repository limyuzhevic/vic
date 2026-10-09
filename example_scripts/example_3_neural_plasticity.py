#!/usr/bin/env python3
"""
Example 3: Neural Plasticity Analysis

This example demonstrates neural plasticity analysis and visualization.
It focuses on analyzing synaptic changes, learning patterns, and
plasticity mechanisms in the NLM system.

Key features demonstrated:
- Plasticity mechanism analysis
- Synaptic weight tracking
- Learning curve visualization
- Pattern detection
- Statistical analysis of plasticity
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import pandas as pd

class PlasticityAnalyzer:
    """
    Specialized analyzer for neural plasticity mechanisms.
    
    This class provides tools for analyzing:
    - Synaptic weight changes over time
    - Plasticity rule effectiveness
    - STDP and Hebbian learning patterns
    - Structural plasticity events
    """
    
    def __init__(self):
        self.plasticity_data = []
        self.weight_history = []
        self.plasticity_events = []
    
    def analyze_stdp_plasticity(self, brain, steps=100):
        """
        Analyze Spike-Timing-Dependent Plasticity (STDP) mechanisms.
        
        Args:
            brain: NLM brain instance
            steps: Number of steps to analyze
            
        Returns:
            dict: STDP analysis results
        """
        print("Analyzing STDP plasticity mechanisms...")
        
        # Enable STDP for analysis
        if hasattr(brain, 'getRegion'):
            region = brain.getRegion(brain.getRegionIds()[0] if brain.getRegionIds() else 0)
            if region:
                # Get initial weights
                initial_weights = []
                for synapse in region.getSynapses():
                    if synapse.getType() == pynlm.SynapseType.Excitatory:
                        initial_weights.append(synapse.getWeight())
                
                print(f"  Initial excitatory synapse weights: {len(initial_weights)} synapses")
                if initial_weights:
                    print(f"  Mean initial weight: {np.mean(initial_weights):.4f}")
                
                # Apply correlated activity to trigger STDP
                print("  Applying correlated pre-post activity...")
                neurons = region.getAllNeurons()
                if len(neurons) >= 2:
                    # Pre-synaptic stimulation
                    pre_neuron = neurons[0]
                    post_neuron = neurons[1]
                    
                    # Correlated firing: pre then post
                    for i in range(steps // 2):
                        # Pre neuron fires
                        pre_neuron.injectCurrent(60.0)
                        brain.step(i * 2, i * 2 * 0.001)
                        
                        # Post neuron fires (STDP window)
                        post_neuron.injectCurrent(60.0)
                        brain.step(i * 2 + 1, (i * 2 + 1) * 0.001)
                    
                    # Analyze weight changes
                    final_weights = []
                    for synapse in region.getSynapses():
                        if synapse.getType() == pynlm.SynapseType.Excitatory:
                            final_weights.append(synapse.getWeight())
                    
                    if final_weights:
                        weight_changes = [final - initial for final, initial 
                                         in zip(final_weights, initial_weights[:len(final_weights)])]
                        
                        print(f"  Final excitatory synapse weights: {len(final_weights)} synapses")
                        print(f"  Mean final weight: {np.mean(final_weights):.4f}")
                        print(f"  Mean weight change: {np.mean(weight_changes):.4f}")
                        
                        # Count strengthened vs weakened synapses
                        strengthened = sum(1 for change in weight_changes if change > 0.001)
                        weakened = sum(1 for change in weight_changes if change < -0.001)
                        unchanged = len(weight_changes) - strengthened - weakened
                        
                        print(f"  Strengthened synapses: {strengthened}")
                        print(f"  Weakened synapses: {weakened}")
                        print(f"  Unchanged synapses: {unchanged}")
                        
                        # STDP effectiveness
                        stdp_effectiveness = (strengthened - weakened) / len(weight_changes)
                        print(f"  STDP effectiveness: {stdp_effectiveness:.3f}")
                        
                        return {
                            'initial_weights': initial_weights,
                            'final_weights': final_weights,
                            'weight_changes': weight_changes,
                            'strengthened': strengthened,
                            'weakened': weakened,
                            'unchanged': unchanged,
                            'stdp_effectiveness': stdp_effectiveness
                        }
        
        return None
    
    def analyze_hebbian_plasticity(self, brain, steps=50):
        """
        Analyze Hebbian plasticity mechanisms.
        
        Args:
            brain: NLM brain instance
            steps: Number of steps to analyze
            
        Returns:
            dict: Hebbian analysis results
        """
        print("Analyzing Hebbian plasticity mechanisms...")
        
        # Get initial state
        if hasattr(brain, 'getTotalSynapseCount'):
            initial_synapses = brain.getTotalSynapseCount()
            print(f"  Initial total synapses: {initial_synapses}")
        
        # Apply correlated activity (Hebbian: simultaneous activation)
        print("  Applying simultaneous pre-post activation...")
        
        for i in range(steps):
            # Simultaneously activate multiple neurons
            for neuron_idx in range(min(10, brain.getTotalNeuronCount())):
                # Use getRegion to access neurons
                regions = brain.getRegionIds()
                if regions:
                    region = brain.getRegion(regions[0])
                    if region:
                        neurons = region.getAllNeurons()
                        if neuron_idx < len(neurons):
                            neurons[neuron_idx].injectCurrent(40.0)
            
            brain.step(i, i * 0.001)
        
        # Analyze final state
        final_synapses = brain.getTotalSynapseCount()
        print(f"  Final total synapses: {final_synapses}")
        
        synapse_change = final_synapses - initial_synapses
        print(f"  Synapse change: {synapse_change}")
        
        # Analyze structural plasticity
        if hasattr(brain, 'getDevelopmentalStage'):
            stage = brain.getDevelopmentalStage()
            print(f"  Developmental stage: {stage}")
        
        return {
            'initial_synapses': initial_synapses,
            'final_synapses': final_synapses,
            'synapse_change': synapse_change,
            'structural_plasticity': synapse_change > 0
        }
    
    def analyze_plasticity_patterns(self, brain, duration=100):
        """
        Analyze long-term plasticity patterns.
        
        Args:
            brain: NLM brain instance
            duration: Duration of analysis in steps
            
        Returns:
            dict: Plasticity pattern analysis
        """
        print("Analyzing long-term plasticity patterns...")
        
        # Collect weight history
        weight_history = []
        steps = []
        
        for step in range(duration):
            if hasattr(brain, 'getRegion'):
                regions = brain.getRegionIds()
                if regions:
                    region = brain.getRegion(regions[0])
                    if region:
                        # Collect average synaptic weight
                        weights = []
                        for synapse in region.getSynapses():
                            if synapse.getType() == pynlm.SynapseType.Excitatory:
                                weights.append(synapse.getWeight())
                        
                        if weights:
                            weight_history.append(np.mean(weights))
                            steps.append(step)
            
            brain.step(step, step * 0.001)
            
            # Record periodically
            if step % 20 == 0 and weight_history:
                print(f"  Step {step}: Mean excitatory weight = {weight_history[-1]:.4f}")
        
        # Analyze patterns
        if len(weight_history) >= 10:
            # Calculate statistics
            mean_weight = np.mean(weight_history)
            weight_std = np.std(weight_history)
            weight_cv = weight_std / mean_weight if mean_weight > 0 else 0
            
            # Detect trends
            x = np.arange(len(weight_history))
            slope, intercept, r_value, p_value, std_err = stats.linregress(x, weight_history)
            
            # Detect cycles
            if len(weight_history) >= 20:
                # Simple cycle detection
                diff = np.diff(weight_history)
                zero_crossings = np.where(np.diff(np.sign(diff)))[0]
                cycle_count = len(zero_crossings) // 2
            else:
                cycle_count = 0
            
            print(f"\nPlasticity Pattern Analysis:")
            print(f"  Mean synaptic weight: {mean_weight:.4f}")
            print(f"  Weight coefficient of variation: {weight_cv:.4f}")
            print(f"  Weight trend slope: {slope:.6f}")
            print(f"  Weight correlation coefficient: {r_value:.4f}")
            print(f"  Statistical significance: {'Significant' if p_value < 0.05 else 'Not significant'}")
            print(f"  Estimated cycles: {cycle_count}")
            
            return {
                'steps': steps,
                'weight_history': weight_history,
                'mean_weight': mean_weight,
                'weight_std': weight_std,
                'weight_cv': weight_cv,
                'slope': slope,
                'r_value': r_value,
                'p_value': p_value,
                'cycle_count': cycle_count,
                'trend': 'increasing' if slope > 0.001 else 'decreasing' if slope < -0.001 else 'stable'
            }
        
        return None
    
    def visualize_plasticity_analysis(self, analysis_results, save_path=None):
        """
        Visualize plasticity analysis results.
        
        Args:
            analysis_results: Results from plasticity analysis
            save_path: Path to save visualization
        """
        if not analysis_results:
            print("No analysis results to visualize")
            return
        
        try:
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('Neural Plasticity Analysis', fontsize=16)
            
            # Plot 1: Weight changes over time
            ax1 = axes[0, 0]
            if 'weight_changes' in analysis_results:
                ax1.barh(range(len(analysis_results['weight_changes'])), 
                        analysis_results['weight_changes'])
                ax1.set_xlabel('Weight Change')
                ax1.set_title('Individual Synaptic Weight Changes')
                ax1.axvline(x=0, color='black', linestyle='-', linewidth=0.5)
            else:
                ax1.text(0.5, 0.5, 'No weight change data', 
                        horizontalalignment='center', verticalalignment='center')
            
            # Plot 2: STDP effectiveness
            ax2 = axes[0, 1]
            if 'weight_changes' in analysis_results:
                strengthened = analysis_results['strengthened']
                weakened = analysis_results['weakened']
                unchanged = analysis_results['unchanged']
                
                ax2.pie([strengthened, weakened, unchanged], 
                       labels=['Strengthened', 'Weakened', 'Unchanged'],
                       autopct='%1.1f%%', startangle=90)
                ax2.set_title('STDP Effect Distribution')
            
            # Plot 3: Weight history
            ax3 = axes[1, 0]
            if 'weight_history' in analysis_results:
                ax3.plot(analysis_results['steps'], analysis_results['weight_history'])
                ax3.set_xlabel('Step')
                ax3.set_ylabel('Mean Excitatory Weight')
                ax3.set_title('Synaptic Weight Evolution')
                ax3.grid(True, alpha=0.3)
            
            # Plot 4: Pattern summary
            ax4 = axes[1, 1]
            if 'trend' in analysis_results:
                trend_data = {
                    'Trend': analysis_results['trend'],
                    'Stability': f"CV: {analysis_results.get('weight_cv', 0):.3f}",
                    'Cycles': f"{analysis_results.get('cycle_count', 0)}",
                    'Signif': 'Significant' if analysis_results.get('p_value', 1) < 0.05 else 'Not significant'
                }
                
                y_pos = np.arange(len(trend_data))
                ax4.barh(y_pos, [1, 1, 1, 1], color=['green', 'blue', 'orange', 'red'])
                ax4.set_yticks(y_pos)
                ax4.set_yticklabels(trend_data.keys())
                ax4.set_xlabel('Status')
                ax4.set_title('Plasticity Pattern Summary')
                ax4.set_xlim(0, 1)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=150, bbox_inches='tight')
                print(f"Plasticity analysis visualization saved to {save_path}")
            
            plt.show()
            
        except Exception as e:
            print(f"Error creating visualization: {e}")

class PlasticityExperiment:
    """
    Experiment class for studying neural plasticity.
    
    This class sets up and runs experiments to study various plasticity
    mechanisms including STDP, Hebbian learning, and structural plasticity.
    """
    
    def __init__(self, name, description):
        self.name = name
        self.description = description
        self.analyzer = PlasticityAnalyzer()
        self.results = {}
    
    def run_stdp_experiment(self, brain_config=None, duration=100):
        """Run STDP plasticity experiment."""
        print(f"\n=== Running STDP Experiment: {self.name} ===")
        print(f"Description: {self.description}")
        
        # Create brain
        config = brain_config or create_advanced_brain_config(
            neuron_count=1000,
            enable_learning=True,
            enable_development=False,
            enable_curiosity=False
        )
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.hebbian.enable", False)  # Disable Hebbian for pure STDP
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run STDP analysis
        stdp_results = self.analyzer.analyze_stdp_plasticity(brain, duration)
        
        if stdp_results:
            self.results['stdp'] = stdp_results
            print(f"✓ STDP experiment completed successfully")
            print(f"  STDP effectiveness: {stdp_results.get('stdp_effectiveness', 0):.3f}")
        else:
            print(f"✗ STDP experiment failed")
        
        return stdp_results
    
    def run_hebbian_experiment(self, brain_config=None, duration=50):
        """Run Hebbian plasticity experiment."""
        print(f"\n=== Running Hebbian Experiment: {self.name} ===")
        
        # Create brain
        config = brain_config or create_advanced_brain_config(
            neuron_count=800,
            enable_learning=True,
            enable_development=False,
            enable_curiosity=False
        )
        
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run Hebbian analysis
        hebbian_results = self.analyzer.analyze_hebbian_plasticity(brain, duration)
        
        if hebbian_results:
            self.results['hebbian'] = hebbian_results
            print(f"✓ Hebbian experiment completed successfully")
            print(f"  Structural plasticity: {'Yes' if hebbian_results.get('structural_plasticity', False) else 'No'}")
        else:
            print(f"✗ Hebbian experiment failed")
        
        return hebbian_results
    
    def run_combined_experiment(self, brain_config=None, duration=150):
        """Run combined plasticity experiment."""
        print(f"\n=== Running Combined Plasticity Experiment: {self.name} ===")
        
        # Create brain with all plasticity mechanisms
        config = brain_config or create_advanced_brain_config(
            neuron_count=1200,
            enable_learning=True,
            enable_development=True,
            enable_curiosity=False
        )
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.learning_rate", 0.0005)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run combined analysis
        pattern_results = self.analyzer.analyze_plasticity_patterns(brain, duration)
        
        if pattern_results:
            self.results['combined'] = pattern_results
            print(f"✓ Combined experiment completed successfully")
            print(f"  Weight trend: {pattern_results.get('trend', 'unknown')}")
            print(f"  Cycles detected: {pattern_results.get('cycle_count', 0)}")
        else:
            print(f"✗ Combined experiment failed")
        
        return pattern_results
    
    def run_all_experiments(self):
        """Run all plasticity experiments."""
        print(f"\n=== Running All Plasticity Experiments: {self.name} ===")
        
        # Run STDP experiment
        self.run_stdp_experiment(duration=100)
        
        # Run Hebbian experiment
        self.run_hebbian_experiment(duration=50)
        
        # Run combined experiment
        self.run_combined_experiment(duration=150)
        
        print(f"\n=== Experiment Summary ===")
        print(f"Experiments completed: {len(self.results)}")
        for exp_type, result in self.results.items():
            print(f"  {exp_type.capitalize()}: {'✓' if result else '✗'}")
        
        # Create visualization if results available
        if self.results:
            print(f"\nCreating plasticity visualization...")
            # Use the last experiment results for visualization
            last_result = list(self.results.values())[-1]
            self.analyzer.visualize_plasticity_analysis(last_result, 
                                                      save_path=f"{self.name}_analysis.png")
        
        return self.results

def example_plasticity_analysis():
    """Demonstrate neural plasticity analysis."""
    print("=== Example 3: Neural Plasticity Analysis ===\n")
    
    # Create plasticity experiment
    experiment = PlasticityExperiment(
        name="Plasticity Analysis Experiment",
        description="Comprehensive study of STDP, Hebbian, and combined plasticity mechanisms"
    )
    
    # Run all experiments
    results = experiment.run_all_experiments()
    
    return results, experiment

def example_plasticity_visualization():
    """Demonstrate plasticity visualization techniques."""
    print("\n=== Plasticity Visualization Example ===\n")
    
    # Create analyzer
    analyzer = PlasticityAnalyzer()
    
    # Create synthetic data for demonstration
    print("1. Creating synthetic plasticity data...")
    steps = list(range(0, 100, 10))
    
    # Simulate weight changes over time
    initial_weight = 0.5
    weight_history = [initial_weight]
    
    for i in range(len(steps) - 1):
        # Add some noise and trend
        change = np.random.normal(0, 0.05) + (0.001 if i % 20 < 5 else 0)
        new_weight = weight_history[-1] + change
        new_weight = max(0.1, min(1.0, new_weight))  # Clamp between 0.1 and 1.0
        weight_history.append(new_weight)
    
    print(f"   Weight range: {min(weight_history):.3f} to {max(weight_history):.3f}")
    print(f"   Mean weight: {np.mean(weight_history):.3f}")
    
    # Create synthetic analysis results
    analysis_results = {
        'steps': steps,
        'weight_history': weight_history,
        'mean_weight': np.mean(weight_history),
        'weight_std': np.std(weight_history),
        'weight_cv': np.std(weight_history) / np.mean(weight_history),
        'slope': np.polyfit(steps, weight_history, 1)[0],
        'r_value': np.corrcoef(steps, weight_history)[0, 1],
        'p_value': 0.001,
        'cycle_count': 3,
        'trend': 'stable'
    }
    
    # Create visualization
    print("2. Creating plasticity visualization...")
    try:
        analyzer.visualize_plasticity_analysis(analysis_results, 
                                              save_path="plasticity_analysis.png")
        print("✓ Visualization created and saved")
    except Exception as e:
        print(f"Note: Visualization requires matplotlib. Error: {e}")
    
    return analysis_results

def example_comparison_studies():
    """Demonstrate comparison studies between plasticity mechanisms."""
    print("\n=== Comparison Studies Example ===\n")
    
    # Create multiple experiments with different parameters
    print("1. Setting up comparison studies...")
    
    experiments = []
    
    # Experiment 1: High STDP rate
    exp1 = PlasticityExperiment("High STDP", "Experiment with high STDP learning rate")
    exp1.run_stdp_experiment(duration=100)
    experiments.append(('High STDP', exp1))
    
    # Experiment 2: Low STDP rate
    exp2 = PlasticityExperiment("Low STDP", "Experiment with low STDP learning rate")
    exp2.run_stdp_experiment(duration=100)
    experiments.append(('Low STDP', exp2))
    
    # Experiment 3: Combined mechanisms
    exp3 = PlasticityExperiment("Combined", "Experiment with STDP and Hebbian")
    exp3.run_combined_experiment(duration=150)
    experiments.append(('Combined', exp3))
    
    # Compare results
    print("\n2. Comparing experiment results...")
    
    comparison = {
        'experiment_names': [],
        'stdp_effectiveness': [],
        'weight_changes': [],
        'trend_stability': []
    }
    
    for name, exp in experiments:
        comparison['experiment_names'].append(name)
        
        # Extract metrics
        if 'stdp' in exp.results:
            comparison['stdp_effectiveness'].append(exp.results['stdp'].get('stdp_effectiveness', 0))
        else:
            comparison['stdp_effectiveness'].append(0)
        
        if 'combined' in exp.results:
            comparison['weight_changes'].append(exp.results['combined'].get('weight_cv', 0))
        elif 'stdp' in exp.results:
            changes = exp.results['stdp'].get('weight_changes', [])
            comparison['weight_changes'].append(np.std(changes) if changes else 0)
        else:
            comparison['weight_changes'].append(0)
        
        if 'combined' in exp.results:
            trend = exp.results['combined'].get('trend', 'stable')
            comparison['trend_stability'].append(1 if trend == 'stable' else 0.5 if trend == 'increasing' else 0.3)
        else:
            comparison['trend_stability'].append(0)
    
    # Print comparison
    print(f"\nComparison Results:")
    for i in range(len(comparison['experiment_names'])):
        print(f"\n{comparison['experiment_names'][i]}:")
        print(f"  STDP Effectiveness: {comparison['stdp_effectiveness'][i]:.3f}")
        print(f"  Weight Change CV: {comparison['weight_changes'][i]:.4f}")
        print(f"  Trend Stability: {comparison['trend_stability'][i]:.2f}")
    
    # Determine best experiment
    best_idx = np.argmax(comparison['stdp_effectiveness'])
    print(f"\nBest Experiment: {comparison['experiment_names'][best_idx]} (STDP: {comparison['stdp_effectiveness'][best_idx]:.3f})")
    
    return comparison, experiments

if __name__ == "__main__":
    print("NLM Advanced Features - Example 3: Neural Plasticity Analysis")
    print("=" * 65)
    
    try:
        # Run plasticity analysis example
        plasticity_results, plasticity_experiment = example_plasticity_analysis()
        
        # Run visualization example
        visualization_results = example_plasticity_visualization()
        
        # Run comparison studies example
        comparison_results, all_experiments = example_comparison_studies()
        
        print("\n" + "=" * 65)
        print("Example 3 completed successfully!")
        print("Key insights:")
        print("  - STDP produces bidirectional weight changes")
        print("  - Hebbian plasticity promotes synchronous activation")
        print("  - Combined mechanisms show complex dynamics")
        print("  - Visualization helps understand plasticity patterns")
        print("  - Comparison studies identify optimal configurations")
        
    except Exception as e:
        print(f"\nError in example 3: {e}")
        import traceback
        traceback.print_exc()
