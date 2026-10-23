"""
NLM Examples and Demos
=======================

Collection of example simulations and demonstrations for the NLM framework.
These examples showcase the capabilities of the neural simulation system
and provide templates for creating custom simulations.
"""

import pynlm as nlm
import numpy as np
from typing import List, Dict, Any
import time
import matplotlib.pyplot as plt

__all__ = [
    'run_silent_brain_example',
    'run_agent_world_example', 
    'run_learning_demonstration',
    'run_curiosity_example',
    'run_plasticity_demo',
    'create_visualization_data',
    'plot_simulation_results'
]


def run_silent_brain_example() -> Dict[str, Any]:
    """
    Example 1: Silent Brain (Just Neurons)
    
    Demonstrates the basic brain functionality without world interaction.
    Shows how neurons can fire and learn without external input.
    """
    print("Running Silent Brain Example...")
    
    # Create and initialize brain
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Simulate for 100 steps
    for i in range(100):
        brain.step(i)
    
    # Collect results
    result = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'simulation_steps': 100
    }
    
    print(f"  Brain created with {result['total_neurons']} neurons")
    print(f"  Generated {result['total_spikes']} spikes over {result['simulation_steps']} steps")
    print(f"  Average firing rate: {result['average_firing_rate']:.2f} Hz")
    
    return result


def run_agent_world_example() -> Dict[str, Any]:
    """
    Example 2: Brain Watching a World
    
    Demonstrates agent-world integration with sensory processing and action.
    Shows how a brain perceives and interacts with an environment.
    """
    print("Running Agent-World Example...")
    
    # Setup components using builder pattern
    config = nlm.create_default_config()
    brain = nlm.BrainBuilder().with_neuron_count(200).build().get()
    brain.initialize()
    
    world = nlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Run for 30 steps watching the world
    firing_rates = []
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        firing_rates.append(brain.getFiringNeuronCount())
    
    # Collect results
    result = {
        'brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate()
        },
        'world_stats': {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'agent_position': (world.getAgentBody().x, world.getAgentBody().y)
        },
        'agent_stats': {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        },
        'firing_rate_history': firing_rates,
        'steps_completed': 30
    }
    
    print(f"  Watched world for {result['steps_completed']} steps")
    print(f"  Average firing rate: {np.mean(firing_rates):.2f} Hz")
    print(f"  Agent position: ({result['world_stats']['agent_position'][0]:.1f}, {result['world_stats']['agent_position'][1]:.1f})")
    
    return result


def run_learning_demonstration() -> Dict[str, Any]:
    """
    Example 3: Complete Agent with Learning
    
    Demonstrates a complete agent simulation with all learning subsystems enabled.
    Shows reward modulation, curiosity, and structural plasticity working together.
    """
    print("Running Learning Demonstration...")
    
    # Create agent with all learning systems
    agent_builder = (nlm.AgentBuilder()
                    .configure_world(width=15, height=15, visionWidth=8, visionHeight=8)
                    .with_start_position(7.5, 7.5)
                    .enable_learning(reward_modulation=True, curiosity=True,
                                    structural_plasticity=True, development=True))
    
    agent = agent_builder.get()
    brain = agent.getBrain()
    
    # Get world if accessible
    world = getattr(agent_builder, 'world', None)
    if world is None:
        # Create a minimal world for the demo
        world = nlm.create_simple_world()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        agent.initialize(world)
    
    # Run agent simulation
    runner = nlm.SimulationRunner(agent, world)
    stats = runner.run_episode(500, timestep=0.1, log_interval=100)
    
    # Collect detailed results
    result = {
        'agent_stats': {
            'final_curiosity': stats.get('curiosity', 0.0),
            'final_novelty': stats.get('novelty', 0.0),
            'final_prediction_error': stats.get('prediction_error', 0.0),
            'neuromodulation_level': stats.get('neuromodulation', 0.0),
            'development_stage': brain.getDevelopmentalStage()
        },
        'brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'final_spike_count': brain.getTotalSpikeCount(),
            'final_firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate()
        },
        'world_stats': {
            'final_position': (world.getAgentBody().x, world.getAgentBody().y),
            'simulation_time': world.getSimulationTime()
        },
        'simulation_stats': stats,
        'simulation_steps': 500
    }
    
    print(f"  Completed {result['simulation_steps']} simulation steps")
    print(f"  Final curiosity level: {result['agent_stats']['final_curiosity']:.3f}")
    print(f"  Final novelty level: {result['agent_stats']['final_novelty']:.3f}")
    print(f"  Brain generated {result['brain_stats']['final_spike_count']} total spikes")
    
    return result


def run_curiosity_example() -> Dict[str, Any]:
    """
    Example 4: Curiosity-Driven Exploration
    
    Demonstrates how curiosity drives exploration behavior.
    Shows the agent exploring its environment based on novelty and prediction error.
    """
    print("Running Curiosity Example...")
    
    # Create agent with high curiosity
    agent_builder = (nlm.AgentBuilder()
                    .configure_world(width=10, height=10, visionWidth=4, visionHeight=4)
                    .with_start_position(5.0, 5.0)
                    .enable_learning(curiosity=True, reward_modulation=False))
    
    agent = agent_builder.get()
    brain = agent.getBrain()
    world = getattr(agent_builder, 'world', None)
    
    if world is None:
        world = nlm.create_simple_world()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        agent.initialize(world)
    
    # Track exploration behavior
    exploration_actions = []
    curiosity_levels = []
    novelty_levels = []
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        exploration_actions.append(action)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Analyze exploration
    action_counts = {}
    for action in exploration_actions:
        action_counts[str(action)] = action_counts.get(str(action), 0) + 1
    
    result = {
        'exploration_stats': {
            'total_steps': 200,
            'unique_actions': len(action_counts),
            'action_distribution': action_counts,
            'curiosity_curve': curiosity_levels,
            'novelty_curve': novelty_levels,
            'avg_curiosity': np.mean(curiosity_levels),
            'avg_novelty': np.mean(novelty_levels)
        },
        'brain_stats': {
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate()
        },
        'final_agent_state': {
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        }
    }
    
    print(f"  Exploration completed {result['exploration_stats']['total_steps']} steps")
    print(f"  Used {result['exploration_stats']['unique_actions']} different actions")
    print(f"  Average curiosity: {result['exploration_stats']['avg_curiosity']:.3f}")
    print(f"  Average novelty: {result['exploration_stats']['avg_novelty']:.3f}")
    
    return result


def run_plasticity_demo() -> Dict[str, Any]:
    """
    Example 5: Plasticity Learning Demonstration
    
    Demonstrates synaptic plasticity including STDP and Hebbian learning.
    Shows how connections strengthen and weaken over time.
    """
    print("Running Plasticity Demonstration...")
    
    # Create brain with all plasticity enabled
    brain = (nlm.BrainBuilder()
             .with_neuron_count(300)
             .with_plasticity(stdp_enabled=True, hebbian_enabled=True, structural_enabled=True)
             .build().get())
    
    brain.initialize()
    
    # Apply specific input patterns to demonstrate learning
    learning_phases = {
        'initial': 0,
        'correlation': 0,
        'delay': 0
    }
    
    # Phase 1: Initial activity
    NLM_LOG_INFO("Phase 1: Initial correlated activity (LTP)")
    for step in range(100):
        for i in range(20):
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0f)
        brain.step(step)
        if step % 25 == 0:
            NLM_LOG_INFO(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Store post-phase statistics
    learning_phases['correlation'] = brain.getTotalSpikeCount()
    
    # Phase 2: Rest period
    NLM_LOG_INFO("Phase 2: Rest period")
    for step in range(100, 200):
        brain.step(step)
    
    learning_phases['delay'] = brain.getTotalSpikeCount()
    
    # Collect plasticity demonstration results
    result = {
        'brain_stats': {
            'initial_neurons': 300,
            'final_synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'learning_phases': learning_phases
        },
        'plasticity_enabled': {
            'stdp': True,
            'hebbian': True,
            'structural': True
        },
        'demonstration': {
            'description': 'Demonstrates activity-dependent synaptic modification',
            'plasticity_mechanisms': [
                'Spike-Timing-Dependent Plasticity (STDP)',
                'Hebbian Learning',
                'Structural Plasticity (synaptogenesis/pruning)'
            ]
        }
    }
    
    print(f"  Plasticity demonstration completed")
    print(f"  Brain connected {result['brain_stats']['final_synapses']} synapses")
    print(f"  Generated {result['brain_stats']['total_spikes']} total spikes")
    
    return result


def create_visualization_data(results: Dict[str, Any]) -> Dict[str, Any]:
    """
    Create data structures for visualization of simulation results.
    
    Args:
        results: Simulation results from any example function
        
    Returns:
        Dictionary with visualization-ready data
    """
    viz_data = {
        'plot_data': {},
        'statistics': {},
        'insights': []
    }
    
    # Extract different types of data for visualization
    if 'firing_rate_history' in results:
        viz_data['plot_data']['firing_rate'] = {
            'data': results['firing_rate_history'],
            'type': 'time_series',
            'ylabel': 'Firing Rate (Hz)',
            'title': 'Neural Activity Over Time'
        }
    
    if 'curiosity_curve' in results:
        viz_data['plot_data']['curiosity'] = {
            'data': results['curiosity_curve'],
            'type': 'time_series',
            'ylabel': 'Curiosity Level',
            'title': 'Curiosity-Driven Exploration'
        }
    
    if 'novelty_curve' in results:
        viz_data['plot_data']['novelty'] = {
            'data': results['novelty_curve'],
            'type': 'time_series',
            'ylabel': 'Novelty Level',
            'title': 'Novelty Detection Over Time'
        }
    
    # Generate insights from results
    if 'average_firing_rate' in results:
        firing_rate = results['average_firing_rate']
        if firing_rate < 1.0:
            viz_data['insights'].append("Low average firing rate suggests sparse neural activity")
        elif firing_rate < 10.0:
            viz_data['insights'].append("Moderate firing rate indicates balanced neural activity")
        else:
            viz_data['insights'].append("High firing rate suggests high neural excitation")
    
    if 'avg_curiosity' in results:
        avg_curiosity = results['avg_curiosity']
        if avg_curiosity > 0.7:
            viz_data['insights'].append("High average curiosity indicates strong exploration drive")
        elif avg_curiosity > 0.3:
            viz_data['insights'].append("Moderate curiosity indicates balanced exploration")
        else:
            viz_data['insights'].append("Low curiosity suggests conservative behavior")
    
    return viz_data


def plot_simulation_results(results: Dict[str, Any], output_file: str = None) -> str:
    """
    Create plots from simulation results.
    
    Args:
        results: Simulation results dictionary
        output_file: Optional path to save the plot
        
    Returns:
        Path to the saved plot file, or None if plotting failed
    """
    try:
        # Create visualization data
        viz_data = create_visualization_data(results)
        
        # Create plots
        plt.figure(figsize=(15, 10))
        
        # Plot 1: Firing rate if available
        if 'firing_rate' in viz_data['plot_data']:
            plt.subplot(2, 2, 1)
            plt.plot(viz_data['plot_data']['firing_rate']['data'])
            plt.xlabel('Time Step')
            plt.ylabel(viz_data['plot_data']['firing_rate']['ylabel'])
            plt.title(viz_data['plot_data']['firing_rate']['title'])
            plt.grid(True, alpha=0.3)
        
        # Plot 2: Curiosity curve if available
        if 'curiosity' in viz_data['plot_data']:
            plt.subplot(2, 2, 2)
            plt.plot(viz_data['plot_data']['curiosity']['data'])
            plt.xlabel('Time Step')
            plt.ylabel(viz_data['plot_data']['curiosity']['ylabel'])
            plt.title(viz_data['plot_data']['curiosity']['title'])
            plt.grid(True, alpha=0.3)
        
        # Plot 3: Novelty curve if available
        if 'novelty' in viz_data['plot_data']:
            plt.subplot(2, 2, 3)
            plt.plot(viz_data['plot_data']['novelty']['data'])
            plt.xlabel('Time Step')
            plt.ylabel(viz_data['plot_data']['novelty']['ylabel'])
            plt.title(viz_data['plot_data']['novelty']['title'])
            plt.grid(True, alpha=0.3)
        
        # Plot 4: Statistics summary
        plt.subplot(2, 2, 4)
        if 'brain_stats' in results:
            stats = results['brain_stats']
            categories = ['Neurons', 'Synapses', 'Spikes']
            values = [stats['total_neurons'], stats['total_synapses'], stats['total_spikes']]
            
            bars = plt.bar(categories, values, color=['skyblue', 'lightgreen', 'salmon'])
            plt.ylabel('Count')
            plt.title('Brain Statistics')
            
            # Add value labels on bars
            for bar, value in zip(bars, values):
                plt.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                        f'{value:,}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Plot saved to: {output_file}")
        
        plt.close()
        return output_file if output_file else "plot_generated"
        
    except Exception as e:
        print(f"Warning: Could not create visualization: {e}")
        return None


# Global logging function
NLM_LOG_INFO = lambda msg: print(f"[INFO] {msg}")
NLM_LOG_ERROR = lambda msg: print(f"[ERROR] {msg}")
NLM_LOG_WARNING = lambda msg: print(f"[WARNING] {msg}")


def print_example_summary(example_name: str, result: Dict[str, Any]):
    """
    Print a formatted summary of example results.
    
    Args:
        example_name: Name of the example
        result: Results dictionary from example function
    """
    print(f"\n{'='*60}")
    print(f"Example Results: {example_name}")
    print(f"{'='*60}")
    
    if 'brain_stats' in result:
        brain = result['brain_stats']
        print(f"\nBrain Statistics:")
        print(f"  Total Neurons: {brain.get('total_neurons', 'N/A')}")
        print(f"  Total Synapses: {brain.get('total_synapses', 'N/A')}")
        print(f"  Total Spikes: {brain.get('total_spikes', 'N/A')}")
        print(f"  Average Firing Rate: {brain.get('average_firing_rate', 'N/A')}")
    
    if 'agent_stats' in result:
        agent = result['agent_stats']
        print(f"\nAgent Statistics:")
        print(f"  Curiosity Level: {agent.get('curiosity_level', 'N/A')}")
        print(f"  Novelty Level: {agent.get('novelty_level', 'N/A')}")
        print(f"  Prediction Error: {agent.get('prediction_error', 'N/A')}")
    
    if 'exploration_stats' in result:
        expl = result['exploration_stats']
        print(f"\nExploration Statistics:")
        print(f"  Steps Completed: {expl.get('total_steps', 'N/A')}")
        print(f"  Unique Actions: {expl.get('unique_actions', 'N/A')}")
        print(f"  Average Curiosity: {expl.get('avg_curiosity', 'N/A')}")
    
    print(f"\n{'='*60}")


# Convenience function for running all examples
def run_all_examples() -> Dict[str, Any]:
    """
    Run all examples and collect results.
    
    Returns:
        Dictionary with results from all examples
    """
    print("Running all NLM examples...")
    print("=" * 60)
    
    results = {}
    
    try:
        results['silent_brain'] = run_silent_brain_example()
        print_example_summary("Silent Brain", results['silent_brain'])
    except Exception as e:
        print(f"Error in silent brain example: {e}")
        results['silent_brain'] = {'error': str(e)}
    
    try:
        results['agent_world'] = run_agent_world_example()
        print_example_summary("Agent-World", results['agent_world'])
    except Exception as e:
        print(f"Error in agent-world example: {e}")
        results['agent_world'] = {'error': str(e)}
    
    try:
        results['learning_demo'] = run_learning_demonstration()
        print_example_summary("Learning Demo", results['learning_demo'])
    except Exception as e:
        print(f"Error in learning demonstration: {e}")
        results['learning_demo'] = {'error': str(e)}
    
    try:
        results['curiosity_demo'] = run_curiosity_example()
        print_example_summary("Curiosity Demo", results['curiosity_demo'])
    except Exception as e:
        print(f"Error in curiosity demonstration: {e}")
        results['curiosity_demo'] = {'error': str(e)}
    
    try:
        results['plasticity_demo'] = run_plasticity_demo()
        print_example_summary("Plasticity Demo", results['plasticity_demo'])
    except Exception as e:
        print(f"Error in plasticity demonstration: {e}")
        results['plasticity_demo'] = {'error': str(e)}
    
    return results