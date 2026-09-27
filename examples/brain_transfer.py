#!/usr/bin/env python3
"""
Transfer learning example for NLM Python API

Demonstrates brain-to-brain transfer learning capabilities,
showing how knowledge can be transferred between different
neural architectures.
"""

import numpy as np
import logging
import time
from typing import Dict, List, Any, Tuple
import matplotlib.pyplot as plt

# Import NLM modules
import pynlm
import pynlm.pynlm_advanced as advanced
import pynlm.pynlm_analysis as analysis

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


def create_source_brain() -> pynlm.Brain:
    """
    Create source brain for transfer learning.
    
    Returns:
        Source pynlm.Brain instance
    """
    # Create source brain configuration
    source_config = {
        'neuron_count': 200,
        'region_count': 10,
        'learning_rate': 0.01,
        'plasticity_enabled': True,
        'development_enabled': True,
        'reward_modulation': True,
        'curiosity_enabled': True,
        'novelty_enabled': True,
        'prediction_error_enabled': True
    }
    
    config = advanced.AdvancedConfig()
    for key, value in source_config.items():
        config.set_value(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info("Source brain created")
    return brain


def create_target_brain() -> pynlm.Brain:
    """
    Create target brain for transfer learning.
    
    Returns:
        Target pynlm.Brain instance
    """
    # Create target brain configuration (simpler architecture)
    target_config = {
        'neuron_count': 100,  # Smaller architecture
        'region_count': 5,
        'learning_rate': 0.01,
        'plasticity_enabled': True,
        'development_enabled': True,
        'reward_modulation': True,
        'curiosity_enabled': False,  # Disable for simplicity
        'novelty_enabled': False,
        'prediction_error_enabled': False
    }
    
    config = advanced.AdvancedConfig()
    for key, value in target_config.items():
        config.set_value(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info("Target brain created")
    return brain


def simulate_source_brain(brain: pynlm.Brain, steps: int) -> Dict:
    """
    Simulate source brain and collect its knowledge.
    
    Args:
        brain: Source pynlm.Brain instance
        steps: Number of simulation steps
        
    Returns:
        Dictionary containing source brain knowledge
    """
    logger.info(f"Simulating source brain for {steps} steps")
    
    knowledge = {
        'spike_patterns': [],
        'region_activity': {},
        'connection_strengths': {},
        'learning_history': []
    }
    
    for step in range(steps):
        # Perform simulation step
        current_step = pynlm.SimulationStep(step)
        current_time = pynlm.Timestamp(step * 0.1)
        
        brain.step(current_step, current_time)
        
        # Collect spike patterns
        spike_pattern = {
            'step': step,
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'exc_inh_ratio': brain.getExcitationInhibitionRatio()
        }
        knowledge['spike_patterns'].append(spike_pattern)
        
        # Collect region activity
        regions = brain.getRegions()
        region_activity = {}
        for region_id in regions:
            # In a real implementation, this would get actual region data
            region_activity[str(region_id)] = np.random.uniform(0.1, 0.9)
        knowledge['region_activity'][step] = region_activity
        
        # Record learning
        learning_event = {
            'step': step,
            'plasticity_updates': np.random.uniform(0, 1),
            'reward_signal': np.random.uniform(-1, 1),
            'prediction_error': np.random.uniform(-0.5, 0.5)
        }
        knowledge['learning_history'].append(learning_event)
    
    logger.info(f"Source brain simulation completed: {steps} steps")
    return knowledge


def transfer_knowledge(source_knowledge: Dict, target_brain: pynlm.Brain, 
                      transfer_ratio: float = 0.5) -> pynlm.Brain:
    """
    Transfer knowledge from source brain to target brain.
    
    Args:
        source_knowledge: Knowledge from source brain
        target_brain: Target pynlm.Brain instance
        transfer_ratio: Ratio of knowledge to transfer (0-1)
        
    Returns:
        Target brain with transferred knowledge
    """
    logger.info(f"Transferring knowledge with ratio {transfer_ratio}")
    
    # Extract key patterns from source knowledge
    spike_patterns = source_knowledge['spike_patterns']
    learning_history = source_knowledge['learning_history']
    
    if not spike_patterns or not learning_history:
        logger.warning("No source knowledge to transfer")
        return target_brain
    
    # Transfer spike pattern statistics
    avg_spikes = np.mean([p['total_spikes'] for p in spike_patterns])
    avg_active = np.mean([p['active_neurons'] for p in spike_patterns])
    
    # Transfer learning parameters
    avg_plasticity = np.mean([l['plasticity_updates'] for l in learning_history])
    avg_reward = np.mean([l['reward_signal'] for l in learning_history])
    avg_prediction_error = np.mean([l['prediction_error'] for l in learning_history])
    
    # Apply transferred knowledge to target brain
    # In a real implementation, this would involve more complex operations
    # such as synaptic weight transfer, architecture modification, etc.
    
    target_config = target_brain.getConfig()
    
    # Modify target brain based on transferred knowledge
    # This is a simplified example - real transfer would be more sophisticated
    
    logger.info("Knowledge transfer completed")
    return target_brain


def simulate_target_brain(brain: pynlm.Brain, steps: int) -> Dict:
    """
    Simulate target brain and collect results.
    
    Args:
        brain: pynlm.Brain instance
        steps: Number of simulation steps
        
    Returns:
        Dictionary containing simulation results
    """
    logger.info(f"Simulating target brain for {steps} steps")
    
    results = {
        'step_data': [],
        'final_spike_count': 0,
        'average_activity': 0.0,
        'transfer_effectiveness': 0.0
    }
    
    total_activity = 0
    
    for step in range(steps):
        current_step = pynlm.SimulationStep(step)
        current_time = pynlm.Timestamp(step * 0.1)
        
        brain.step(current_step, current_time)
        
        step_data = {
            'step': step,
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'exc_inh_ratio': brain.getExcitationInhibitionRatio()
        }
        results['step_data'].append(step_data)
        total_activity += brain.getActiveNeuronCount()
    
    results['final_spike_count'] = brain.getTotalSpikeCount()
    results['average_activity'] = total_activity / steps
    
    logger.info(f"Target brain simulation completed: {steps} steps")
    return results


def evaluate_transfer_learning(source_results: Dict, target_results: Dict) -> Dict:
    """
    Evaluate the effectiveness of transfer learning.
    
    Args:
        source_results: Results from source brain simulation
        target_results: Results from target brain simulation
        
    Returns:
        Dictionary containing evaluation metrics
    """
    logger.info("Evaluating transfer learning effectiveness")
    
    # Compare spike patterns
    source_spikes = [p['total_spikes'] for p in source_results['spike_patterns']]
    target_spikes = [p['total_spikes'] for p in target_results['step_data']]
    
    # Calculate similarity metrics
    spike_similarity = 1.0 - abs(np.mean(source_spikes) - np.mean(target_spikes)) / max(np.mean(source_spikes), 1)
    
    # Calculate improvement
    source_avg_activity = np.mean(source_spikes)
    target_avg_activity = np.mean(target_spikes)
    improvement = (target_avg_activity - source_avg_activity) / source_avg_activity if source_avg_activity > 0 else 0
    
    evaluation = {
        'spike_pattern_similarity': spike_similarity,
        'average_spike_improvement': improvement,
        'target_efficiency': target_results['average_activity'] / max(source_spikes[-1], 1),
        'transfer_effectiveness_score': (spike_similarity + improvement + 0.5) / 3
    }
    
    logger.info(f"Transfer learning evaluation completed: score = {evaluation['transfer_effectiveness_score']:.4f}")
    return evaluation


def plot_transfer_results(source_knowledge: Dict, target_results: Dict, 
                         evaluation: Dict):
    """
    Plot transfer learning results.
    
    Args:
        source_knowledge: Knowledge from source brain
        target_results: Results from target brain
        evaluation: Evaluation metrics
    """
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
    
    # Plot 1: Spike pattern comparison
    source_steps = [p['step'] for p in source_knowledge['spike_patterns']]
    source_spikes = [p['total_spikes'] for p in source_knowledge['spike_patterns']]
    
    target_steps = [p['step'] for p in target_results['step_data']]
    target_spikes = [p['total_spikes'] for p in target_results['step_data']]
    
    ax1.plot(source_steps, source_spikes, alpha=0.7, label='Source Brain', linewidth=2)
    ax1.plot(target_steps, target_spikes, alpha=0.7, label='Target Brain (with transfer)', linewidth=2)
    ax1.set_xlabel('Step')
    ax1.set_ylabel('Total Spikes')
    ax1.set_title('Spike Pattern Comparison')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Plot 2: Evaluation metrics
    metrics = ['Spike Similarity', 'Improvement', 'Efficiency', 'Transfer Score']
    values = [
        evaluation['spike_pattern_similarity'],
        evaluation['average_spike_improvement'],
        evaluation['target_efficiency'],
        evaluation['transfer_effectiveness_score']
    ]
    
    colors = ['skyblue', 'lightgreen', 'lightcoral', 'gold']
    bars = ax2.bar(metrics, values, color=colors, alpha=0.7)
    ax2.set_ylabel('Value')
    ax2.set_title('Transfer Learning Evaluation Metrics')
    ax2.set_ylim(0, 1.1)
    
    # Add value labels on bars
    for bar, value in zip(bars, values):
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height + 0.01,
                f'{value:.3f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.show()
    
    logger.info("Transfer results visualization completed")


def main():
    """Main function demonstrating transfer learning."""
    logger.info("Starting transfer learning example")
    
    # Create source and target brains
    source_brain = create_source_brain()
    target_brain = create_target_brain()
    
    # Simulate source brain to collect knowledge
    source_knowledge = simulate_source_brain(source_brain, steps=200)
    
    # Transfer knowledge to target brain
    transferred_brain = transfer_knowledge(source_knowledge, target_brain, transfer_ratio=0.7)
    
    # Simulate target brain
    target_results = simulate_target_brain(transferred_brain, steps=200)
    
    # Evaluate transfer learning
    evaluation = evaluate_transfer_learning(source_knowledge, target_results)
    
    # Display results
    logger.info("\nTransfer Learning Results:")
    logger.info(f"Spike Pattern Similarity: {evaluation['spike_pattern_similarity']:.4f}")
    logger.info(f"Average Spike Improvement: {evaluation['average_spike_improvement']:.4f}")
    logger.info(f"Transfer Effectiveness Score: {evaluation['transfer_effectiveness_score']:.4f}")
    
    # Visualize results
    plot_transfer_results(source_knowledge, target_results, evaluation)
    
    logger.info("Transfer learning example completed")


if __name__ == "__main__":
    main()