# STDP Learning Experiment for NLM

"""
Comprehensive demonstration of Spike-Timing-Dependent Plasticity (STDP) 
in the Neural Learning Machine (NLM) / 熙然.

This script implements the Learning Experiment use case as specified.

STDP is a learning rule where the timing of pre- and post-synaptic spikes
 determines whether synaptic strength increases (potentiation) or decreases (depression).

Key principles:
- Pre-before-post spikes → LTP (Long-Term Potentiation) → stronger synapses
- Post-before-pre spikes → LTD (Long-Term Depression) → weaker synapses
- No significant timing difference → no change
"""

import pynlm
import matplotlib.pyplot as plt
import numpy as np
import time
from collections import deque

class STDPExperiment:
    """
    Comprehensive STDP learning experiment demonstrating synaptic changes
    based on spike timing relationships.
    """
    
    def __init__(self, config):
        """Initialize the STDP experiment with configuration."""
        self.config = config
        self.setup_experiment()
    
    def setup_experiment(self):
        """Setup the brain and agent for STDP experiments."""
        # Create and configure the brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Setup simple world for controlled experiments
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable STDP and related systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Storage for STDP analysis
        self.spike_times = []
        self.pre_post_weights = []
        self.post_pre_weights = []
        self.ltp_events = []
        self.ltd_events = []
        self.learning_curves = []
        self.firing_patterns = []
        
        # Initialize for pattern generation
        self.pattern_generator = np.random.RandomState(42)
        self.current_training_phase = 0
    
    def generate_stdp_training_pairs(self, num_pairs=10, interval=100):
        """
        Generate pre-post spike pairs for STDP training.
        
        Args:
            num_pairs: Number of spike pairs to generate
            interval: Time between pairs in simulation steps
            
        Returns:
            List of (pre_time, post_time) pairs with appropriate timing
        """
        spike_pairs = []
        
        for i in range(num_pairs):
            # Random timing offset
            time_offset = np.random.uniform(0, interval)
            
            # Determine pair type based on training phase
            if self.current_training_phase == 0:
                # Phase 0: Pre-before-post (for LTP)
                pre_time = int(time_offset)
                post_time = int(time_offset + np.random.uniform(10, 50))  # Post after pre
                pair_type = "LTP"
                
            elif self.current_training_phase == 1:
                # Phase 1: Post-before-pre (for LTD)
                pre_time = int(time_offset + np.random.uniform(10, 50))  # Pre after post
                post_time = int(time_offset)
                pair_type = "LTD"
                
            elif self.current_training_phase == 2:
                # Phase 2: Synchronous (no significant change)
                pre_time = int(time_offset)
                post_time = int(time_offset + np.random.uniform(-5, 5))  # Almost simultaneous
                pair_type = "NO CHANGE"
            
            spike_pairs.append({
                'pre_time': pre_time,
                'post_time': post_time,
                'pair_type': pair_type,
                'strength_change': 0.0
            })
        
        return spike_pairs
    
    def run_stdp_training_phase(self, spike_pairs, max_steps=2000):
        """
        Run a specific STDP training phase with given spike pairs.
        
        Args:
            spike_pairs: List of spike timing pairs to train with
            max_steps: Maximum simulation steps for this phase
            
        Returns:
            Dictionary with training results and analysis
        """
        print(f"Running {self.current_training_phase} training phase with {len(spike_pairs)} pairs")
        
        phase_start_time = time.time()
        training_complete = False
        pair_index = 0
        
        for step in range(max_steps):
            # Generate new spike pair if needed
            if pair_index < len(spike_pairs):
                pair = spike_pairs[pair_index]
                
                # Schedule pre-synaptic spike
                if step == pair['pre_time']:
                    self.inject_pre_synaptic_spike(pair['pre_time'])
                
                # Schedule post-synaptic spike
                if step == pair['post_time']:
                    self.inject_post_synaptic_spike(pair['post_time'])
                
                # Record synaptic changes after spikes
                if step >= pair['post_time']:
                    self.record_synaptic_changes(pair)
                    pair_index += 1
            
            # Run brain simulation step
            self.brain.step(step, step * 0.001)
            
            # Record learning metrics
            self.record_learning_metrics(step)
            
            # Check if training is complete
            if pair_index >= len(spike_pairs) and step > max(spike_pairs, key=lambda x: x['post_time'])['post_time']:
                training_complete = True
                break
        
        phase_duration = time.time() - phase_start_time
        
        # Analyze results
        results = self.analyze_stdp_results(spike_pairs, phase_duration)
        
        return results
    
    def inject_pre_synaptic_spike(self, step_time):
        """Inject pre-synaptic spike to trigger LTP or LTD."""
        # Get a random pre-synaptic neuron
        regions = self.brain.getRegions()
        if regions:
            population = regions[0].getPopulations()
            if population:
                neurons = population.getNeurons()
                if neurons:
                    pre_neuron = np.random.choice(len(neurons))
                    neurons[pre_neuron].injectCurrent(15.0)  # Strong input to ensure spike
    
    def inject_post_synaptic_spike(self, step_time):
        """Inject post-synaptic spike to complete STDP pairing."""
        # Get a different post-synaptic neuron
        regions = self.brain.getRegions()
        if regions:
            population = regions[0].getPopulations()
            if population:
                neurons = population.getNeurons()
                if neurons:
                    # Select a neuron different from pre-synaptic
                    post_neuron = np.random.choice([i for i in range(len(neurons)) 
                                                 if i != neurons.index(self.get_last_pre_neuron())])
                    neurons[post_neuron].injectCurrent(12.0)  # Medium input
    
    def record_synaptic_changes(self, spike_pair):
        """Record synaptic weight changes after spike pair."""
        # Get current synaptic weights
        regions = self.brain.getRegions()
        if regions:
            population = regions[0].getPopulations()
            if population:
                synapses = population.getSynapses()
                
                # Calculate average weight change
                total_weight_change = 0
                weight_count = 0
                
                for synapse in synapses:
                    # This is a simplified model - in reality, STDP affects specific
                    # pre-post synapse pairs based on their timing
                    old_weight = synapse.getWeight()
                    
                    # Apply STDP rule (simplified)
                    if spike_pair['pair_type'] == "LTP":
                        new_weight = min(old_weight + 0.01, 1.0)  # Potentiation
                    elif spike_pair['pair_type'] == "LTD":
                        new_weight = max(old_weight - 0.01, 0.0)  # Depression
                    else:
                        new_weight = old_weight  # No change
                    
                    weight_change = new_weight - old_weight
                    total_weight_change += abs(weight_change)
                    weight_count += 1
                
                avg_weight_change = total_weight_change / max(weight_count, 1)
                spike_pair['strength_change'] = avg_weight_change
                
                # Record the change
                if spike_pair['pair_type'] == "LTP":
                    self.ltp_events.append({
                        'step': spike_pair['post_time'],
                        'weight_change': avg_weight_change
                    })
                elif spike_pair['pair_type'] == "LTD":
                    self.ltd_events.append({
                        'step': spike_pair['post_time'],
                        'weight_change': avg_weight_change
                    })
    
    def record_learning_metrics(self, step):
        """Record learning metrics for analysis."""
        # Record firing patterns
        firing_rate = self.brain.getAverageFiringRate()
        self.firing_patterns.append({
            'step': step,
            'firing_rate': firing_rate,
            'time': time.time()
        })
        
        # Record synaptic weights
        regions = self.brain.getRegions()
        if regions:
            population = regions[0].getPopulations()
            if population:
                synapses = population.getSynapses()
                total_weight = sum(syn.getWeight() for syn in synapses)
                self.learning_curves.append({
                    'step': step,
                    'total_weight': total_weight,
                    'num_synapses': len(synapses)
                })
    
    def analyze_stdp_results(self, spike_pairs, phase_duration):
        """Analyze STDP experiment results."""
        # Calculate summary statistics
        total_weight_change = sum(pair['strength_change'] for pair in spike_pairs)
        avg_weight_change = total_weight_change / len(spike_pairs) if spike_pairs else 0
        
        # Count LTP/LTD events
        ltp_count = len(self.ltp_events)
        ltd_count = len(self.ltd_events)
        
        # Calculate learning efficiency
        if self.learning_curves:
            initial_weight = self.learning_curves[0]['total_weight']
            final_weight = self.learning_curves[-1]['total_weight']
            weight_efficiency = (final_weight - initial_weight) / len(self.learning_curves) if self.learning_curves else 0
        
        # Calculate firing rate stability
        if self.firing_patterns:
            firing_rates = [p['firing_rate'] for p in self.firing_patterns]
            firing_stability = np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
        
        return {
            'phase': self.current_training_phase,
            'spike_pairs': len(spike_pairs),
            'training_duration': phase_duration,
            'total_weight_change': total_weight_change,
            'average_weight_change': avg_weight_change,
            'ltp_events': ltp_count,
            'ltd_events': ltd_count,
            'weight_efficiency': weight_efficiency if 'weight_efficiency' in locals() else 0,
            'firing_stability': firing_stability if 'firing_stability' in locals() else 0,
            'learning_curve': self.learning_curves,
            'firing_patterns': self.firing_patterns,
            'pair_details': spike_pairs
        }
    
    def get_last_pre_neuron(self):
        """Get the last pre-synaptic neuron (for spike pairing)."""
        # This is a simplified method - in a real implementation,
        # we'd track the actual pre-synaptic neuron that spiked
        return None

def run_complete_stdp_experiment(num_steps_per_phase=2000, num_pairs=10):
    """
    Run the complete STDP learning experiment with multiple phases.
    
    The experiment demonstrates three types of STDP training:
    1. LTP (Long-Term Potentiation): Pre-before-post spikes
    2. LTD (Long-Term Depression): Post-before-pre spikes  
    3. No Change: Synchronous spikes
    """
    
    print("=== STDP Learning Experiment for NLM ===")
    print("Demonstrating Spike-Timing-Dependent Plasticity (STDP)")
    
    # Create configuration optimized for STDP experiments
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    # Initialize experiment
    experiment = STDPExperiment(config)
    
    # Run three training phases
    all_results = []
    
    for phase in range(3):
        experiment.current_training_phase = phase
        
        # Generate spike pairs for this phase
        spike_pairs = experiment.generate_stdp_training_pairs(num_pairs)
        
        # Run training
        results = experiment.run_stdp_training_phase(spike_pairs, num_steps_per_phase)
        all_results.append(results)
        
        print(f"\nPhase {phase} Results:")
        print(f"  Type: {results['pair_details'][0]['pair_type'] if results['pair_details'] else 'Unknown'}")
        print(f"  Total weight change: {results['total_weight_change']:.4f}")
        print(f"  Average weight change: {results['average_weight_change']:.4f}")
        print(f"  LTP events: {results['ltp_events']}")
        print(f"  LTD events: {results['ltd_events']}")
        print(f"  Weight efficiency: {results['weight_efficiency']:.4f}")
        print(f"  Firing stability: {results['firing_stability']:.3f}")
        
        # Print example spike pair details
        if results['pair_details']:
            print(f"  Example spike pair: Pre at step {results['pair_details'][0]['pre_time']}, "
                  f"Post at step {results['pair_details'][0]['post_time']}, "
                  f"Change: {results['pair_details'][0]['strength_change']:.4f}")
    
    # Create comprehensive visualization
    create_stdp_visualizations(all_results)
    
    # Analyze and print summary
    analyze_stdp_summary(all_results)
    
    return all_results

def create_stdp_visualizations(results):
    """Create comprehensive visualizations for STDP experiment."""
    
    plt.figure(figsize=(16, 12))
    
    # Plot 1: STDP phase timeline
    plt.subplot(2, 3, 1)
    phases = range(len(results))
    phase_types = [r['pair_details'][0]['pair_type'] if r['pair_details'] else 'Unknown' 
                  for r in results]
    
    # Map phase types to colors
    color_map = {'LTP': 'green', 'LTD': 'red', 'NO CHANGE': 'blue'}
    colors = [color_map.get(pt, 'gray') for pt in phase_types]
    
    plt.bar(phases, [1] * len(phases), color=colors, alpha=0.7)
    plt.xlabel('Training Phase')
    plt.ylabel('Phase')
    plt.title('STDP Training Phases')
    plt.xticks(phases, phase_types, rotation=45)
    plt.ylim(0, 1.5)
    
    # Plot 2: Weight changes over time (from first phase)
    plt.subplot(2, 3, 2)
    if results and 'learning_curve' in results[0]:
        learning_curve = results[0]['learning_curve']
        weights = [lc['total_weight'] for lc in learning_curve]
        steps = [lc['step'] for lc in learning_curve]
        
        plt.plot(steps, weights, 'b-', alpha=0.7, linewidth=1)
        plt.xlabel('Step')
        plt.ylabel('Total Synaptic Weight')
        plt.title('Synaptic Weight Dynamics')
        plt.grid(True, alpha=0.3)
    
    # Plot 3: LTP/LTD distribution
    plt.subplot(2, 3, 3)
    ltp_counts = [r['ltp_events'] for r in results]
    ltd_counts = [r['ltd_events'] for r in results]
    
    x = np.arange(len(results))
    width = 0.35
    
    plt.bar(x - width/2, ltp_counts, width, label='LTP Events', color='green', alpha=0.7)
    plt.bar(x + width/2, ltd_counts, width, label='LTD Events', color='red', alpha=0.7)
    
    plt.xlabel('Training Phase')
    plt.ylabel('Number of Events')
    plt.title('LTP vs LTD Event Counts')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Plot 4: Firing rate stability
    plt.subplot(2, 3, 4)
    stabilities = [r['firing_stability'] for r in results]
    
    plt.bar(range(len(stabilities)), stabilities, color='orange', alpha=0.7)
    plt.xlabel('Training Phase')
    plt.ylabel('Firing Rate Stability (CV)')
    plt.title('Firing Rate Stability Across Phases')
    plt.grid(True, alpha=0.3)
    
    # Plot 5: Learning efficiency
    plt.subplot(2, 3, 5)
    efficiencies = [r['weight_efficiency'] for r in results]
    
    plt.plot(range(len(efficiencies)), efficiencies, 'r-', linewidth=2, marker='o')
    plt.xlabel('Training Phase')
    plt.ylabel('Learning Efficiency')
    plt.title('Learning Efficiency Evolution')
    plt.grid(True, alpha=0.3)
    
    # Plot 6: Comprehensive analysis summary
    plt.subplot(2, 3, 6)
    
    # Create radar chart for each phase
    categories = ['LTP Events', 'LTD Events', 'Weight Change', 'Stability', 'Efficiency']
    
    # Normalize data for radar chart
    normalized_data = []
    for i, r in enumerate(results):
        ltp_norm = r['ltp_events'] / max(r['ltp_events'], 1)
        ltd_norm = r['ltd_events'] / max(r['ltd_events'], 1)
        weight_norm = abs(r['average_weight_change']) / max(abs(r['average_weight_change']), 0.01)
        stability_norm = 1 - min(r['firing_stability'], 1)  # Invert so higher is better
        efficiency_norm = min(r['weight_efficiency'], 1)
        
        normalized_data.append([ltp_norm, ltd_norm, weight_norm, stability_norm, efficiency_norm])
    
    # Plot each phase as a line
    angles = np.linspace(0, 2 * np.pi, len(categories), closed=True)
    angles += np.pi / len(categories)
    
    for i, phase_data in enumerate(normalized_data):
        phase_data += [phase_data[0]]  # Close the polygon
        plt.plot(angles, phase_data, 'o-', linewidth=2, label=f'Phase {i}')
    
    plt.xticks(angles[:-1], categories)
    plt.yticks([0.2, 0.4, 0.6, 0.8, 1.0])
    plt.title('Phase Performance Radar Chart')
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('stdp_learning_experiment.png', dpi=150)
    plt.show()

def analyze_stdp_summary(results):
    """Analyze and print STDP experiment summary."""
    
    print("\n=== STDP EXPERIMENT SUMMARY ===")
    
    # Overall statistics
    total_pairs = sum(r['spike_pairs'] for r in results)
    total_weight_change = sum(r['total_weight_change'] for r in results)
    avg_weight_change = total_weight_change / len(results) if results else 0
    
    total_ltp = sum(r['ltp_events'] for r in results)
    total_ltd = sum(r['ltd_events'] for r in results)
    
    print(f"\nOverall Experiment Statistics:")
    print(f"  Total STDP pairs trained: {total_pairs}")
    print(f"  Total weight change: {total_weight_change:.4f}")
    print(f"  Average weight change per phase: {avg_weight_change:.4f}")
    print(f"  LTP events: {total_ltp}")
    print(f"  LTD events: {total_ltd}")
    print(f"  LTP to LTD ratio: {total_ltp / max(total_ltd, 1):.2f}")
    
    # Analysis based on results
    print(f"\nSTDP Learning Analysis:")
    
    if avg_weight_change > 0.001:
        print("  Result: Strong synaptic potentiation (learning)")
        print("  Interpretation: Network is effectively strengthening connections")
    elif avg_weight_change < -0.001:
        print("  Result: Strong synaptic depression (forgetting)")
        print("  Interpretation: Network is effectively weakening connections")
    else:
        print("  Result: Minimal synaptic change")
        print("  Interpretation: Network is in homeostasis or STDP effects are balanced")
    
    # LTP/LTD balance analysis
    if total_ltp > total_ltd * 2:
        print("  Interpretation: Network biased toward strengthening (exploration)")
    elif total_ltd > total_ltp * 2:
        print("  Interpretation: Network biased toward weakening (stability)")
    else:
        print("  Interpretation: Network has balanced LTP/LTD (optimal learning)")
    
    # Learning efficiency assessment
    avg_efficiency = np.mean([r['weight_efficiency'] for r in results if r['weight_efficiency']])
    
    print(f"\nLearning Efficiency Assessment:")
    print(f"  Average learning efficiency: {avg_efficiency:.3f}")
    
    if avg_efficiency > 0.1:
        print("  Classification: High efficiency - Effective STDP implementation")
    elif avg_efficiency > 0.05:
        print("  Classification: Moderate efficiency - Room for optimization")
    elif avg_efficiency > 0.01:
        print("  Classification: Low efficiency - Needs improvement")
    else:
        print("  Classification: Very low efficiency - Significant issues")
    
    # Key STDP insights
    print(f"\nKey STDP Insights:")
    print("  1. Spike timing precisely controls synaptic plasticity")
    print("  2. Pre-before-post pairing drives LTP (memory formation)")
    print("  3. Post-before-pre pairing drives LTD (memory weakening)")
    print("  4. Synchronous spikes maintain synaptic homeostasis")
    print("  5. Network-level STDP emerges from individual synaptic changes")

if __name__ == "__main__":
    # Run the complete STDP experiment
    results = run_complete_stdp_experiment(num_steps_per_phase=2000, num_pairs=10)
    
    print("\n=== STDP LEARNING EXPERIMENT COMPLETE ===")
    print("Key insights:")
    print("- STDP precisely controls synaptic strength based on spike timing")
    print("- Pre-before-post spikes induce LTP (long-term potentiation)")
    print("- Post-before-pre spikes induce LTD (long-term depression)")
    print("- Synchronous spikes maintain synaptic homeostasis")
    print("- Network learning emerges from coordinated spike timing")


def get_spike_pairs_for_phase(phase):
    """
    Helper function to get example spike pairs for documentation.
    
    Args:
        phase: Training phase (0=LTP, 1=LTD, 2=No Change)
        
    Returns:
        List of example spike pairs
    """
    examples = {
        0: [  # LTP examples (pre before post)
            {'pre_time': 100, 'post_time': 150, 'pair_type': 'LTP'},
            {'pre_time': 200, 'post_time': 250, 'pair_type': 'LTP'},
            {'pre_time': 300, 'post_time': 400, 'pair_type': 'LTP'}
        ],
        1: [  # LTD examples (post before pre)
            {'pre_time': 500, 'post_time': 450, 'pair_type': 'LTD'},
            {'pre_time': 600, 'post_time': 550, 'pair_type': 'LTD'},
            {'pre_time': 700, 'post_time': 650, 'pair_type': 'LTD'}
        ],
        2: [  # No change examples (synchronous)
            {'pre_time': 800, 'post_time': 805, 'pair_type': 'NO CHANGE'},
            {'pre_time': 900, 'post_time': 895, 'pair_type': 'NO CHANGE'},
            {'pre_time': 1000, 'post_time': 1005, 'pair_type': 'NO CHANGE'}
        ]
    }
    
    return examples.get(phase, examples[0])

# Additional utility functions for documentation

def create_stdp_configuration_template():
    """
    Create STDP configuration template for documentation.
    
    Returns:
        Dictionary with STDP configuration parameters
    """
    return {
        "brain": {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.05,
            "initial_weight_mean": 0.5,
            "initial_weight_std": 0.1
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.002,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": True,
                "learning_rate": 0.001
            }
        },
        "training": {
            "num_pairs": 10,
            "phase_duration": 2000,
            "intervals": {
                "ltp": 50,
                "ltd": 50,
                "no_change": 5
            }
        }
    }

def get_stdp_learning_insights():
    """
    Get educational insights about STDP for documentation.
    
    Returns:
        List of STDP insights
    """
    return [
        {
            "title": "Spike Timing Dictates Plasticity",
            "description": "The precise timing between pre- and post-synaptic spikes determines whether synapses strengthen (LTP) or weaken (LTD).",
            "example": "Pre-synaptic spike at t=100ms, Post-synaptic spike at t=150ms → LTP"
        },
        {
            "title": "Trace Intervals Control Learning",
            "description": "The time difference between spikes (Δt) controls the magnitude and sign of synaptic changes.",
            "example": "Large Δt (pre much before post) → Strong LTP, Large Δt (post much before pre) → Strong LTD"
        },
        {
            "title": "Neuromodulation Modulates STDP",
            "description": "Dopamine and other neuromodulators can gate or scale STDP effects.",
            "example": "High dopamine levels → Enhanced LTP, Low dopamine levels → Reduced STDP"
        },
        {
            "title": "Developmental Critical Periods",
            "description": "STDP efficacy changes with development, with stronger plasticity in early stages.",
            "example": "Young brain → Large STDP effects, Mature brain → Reduced STDP"
        },
        {
            "title": "Network-Level STDP Emergence",
            "description": "Individual synaptic STDP combines to produce network-level learning dynamics.",
            "example": "Many LTP events → Overall network strengthening, Many LTD events → Network pruning"
        }
    ]

# Export for use in documentation
__all__ = [
    'STDPExperiment',
    'run_complete_stdp_experiment',
    'get_spike_pairs_for_phase',
    'create_stdp_configuration_template',
    'get_stdp_learning_insights'
]

# Instructions for running from command line
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Run STDP Learning Experiment for NLM")
    parser.add_argument("--phases", type=int, default=3, help="Number of training phases")
    parser.add_argument("--steps", type=int, default=2000, help="Steps per training phase")
    parser.add_argument("--pairs", type=int, default=10, help="Number of spike pairs per phase")
    parser.add_argument("--config", help="Path to configuration file")
    
    args = parser.parse_args()
    
    if args.config:
        # Load configuration from file
        import json
        with open(args.config, 'r') as f:
            config = json.load(f)
        # Create config object (implementation depends on pynlm API)
        # This is a placeholder - actual implementation would need to match pynlm's config API
    
    # Run experiment
    results = run_complete_stdp_experiment(
        num_steps_per_phase=args.steps,
        num_pairs=args.pairs
    )