#!/usr/bin/env python3
"""
NLM Advanced Utilities - Helper Functions for Advanced Users

Comprehensive utilities and helper functions for working with NLM Phase 6
integrated brain. These utilities provide enhanced functionality for
advanced use cases, debugging, and performance optimization.

Author: NLM Development Team
License: MIT
"""

import pynlm
import numpy as np
import time
import os
from typing import List, Dict, Any, Optional, Callable

class NLMBrainManager:
    """
    Advanced brain management utilities for NLM Phase 6.
    
    Provides enhanced functionality for working with multiple brains,
    checkpoint management, and performance optimization.
    """
    
    def __init__(self):
        self.brains = {}
        self.checkpoints = {}
    
    def createManagedBrain(self, name: str, config: Optional[pynlm.Config] = None) -> pynlm.Brain:
        """Create a brain with management tracking."""
        if config is None:
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.brains[name] = {
            'brain': brain,
            'creation_time': time.time(),
            'last_step': 0,
            'total_steps': 0,
            'memory_usage': {}
        }
        
        return brain
    
    def batchStep(self, brain_names: List[str], steps: int = 1) -> Dict[str, int]:
        """
        Run multiple brains in batch mode for efficiency.
        
        Args:
            brain_names: List of brain names to step
            steps: Number of steps to run
            
        Returns:
            Dictionary mapping brain name to number of steps taken
        """
        results = {}
        
        for name in brain_names:
            if name in self.brains:
                brain_data = self.brains[name]
                brain = brain_data['brain']
                
                for step in range(steps):
                    brain.step(brain_data['last_step'] + step)
                    brain_data['last_step'] += step
                    brain_data['total_steps'] += step
                
                results[name] = brain_data['last_step']
        
        return results
    
    def createCheckpointSet(self, prefix: str = "brain_checkpoint") -> Dict[str, str]:
        """Create checkpoint set for multiple brains."""
        checkpoint_files = {}
        
        for name, brain_data in self.brains.items():
            brain = brain_data['brain']
            filename = f"{prefix}_{name}_{int(time.time())}.bin"
            
            if brain.save(filename):
                checkpoint_files[name] = filename
                self.checkpoints[name] = filename
                print(f"✓ Checkpoint saved for brain '{name}': {filename}")
            else:
                print(f"✗ Failed to save checkpoint for brain '{name}'")
        
        return checkpoint_files
    
    def loadCheckpointSet(self, checkpoint_files: Dict[str, str]) -> Dict[str, bool]:
        """Load multiple brain checkpoints."""
        results = {}
        
        for name, filename in checkpoint_files.items():
            if name in self.brains:
                brain = self.brains[name]['brain']
                results[name] = brain.load(filename)
                
                if results[name]:
                    print(f"✓ Checkpoint loaded for brain '{name}': {filename}")
                else:
                    print(f"✗ Failed to load checkpoint for brain '{name}': {filename}")
            else:
                print(f"✗ Brain '{name}' not found in manager")
                results[name] = False
        
        return results
    
    def getPerformanceReport(self, brain_name: str) -> Dict[str, Any]:
        """Get detailed performance report for a brain."""
        if brain_name not in self.brains:
            return {}
        
        brain_data = self.brains[brain_name]
        brain = brain_data['brain']
        
        report = {
            'name': brain_name,
            'creation_time': brain_data['creation_time'],
            'uptime': time.time() - brain_data['creation_time'],
            'total_steps': brain_data['total_steps'],
            'steps_per_second': brain_data['total_steps'] / max(brain_data['uptime'], 1),
            
            # Memory statistics
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            
            # System integration
            'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
            'episodic_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
            'associative_connections': brain.getAssociativeMemory().getAssociationCount() if brain.getAssociativeMemory() else 0,
            
            # Cognitive systems
            'planning_depth': brain.getPlanner().getPlanningDepth() if brain.getPlanner() else 0,
            'planning_confidence': brain.getPlanner().getPlanningConfidence() if brain.getPlanner() else 0,
            'self_model_capability': brain.getSelfModel().getCapabilityLevel() if brain.getSelfModel() else 0,
            'curiosity_level': brain.getCuriosity().getLevel() if brain.getCuriosity() else 0,
            
            # Neuromodulation
            'dopamine_level': brain.getDopamine().getLevel() if brain.getDopamine() else 0,
            'novelty_level': brain.getNovelty().getLevel() if brain.getNovelty() else 0,
            
            # Development
            'developmental_stage': str(brain.getDevelopmentalStage()),
            
            # Checkpoint status
            'has_checkpoint': brain_name in self.checkpoints,
            'last_checkpoint': self.checkpoints.get(brain_name, "None")
        }
        
        return report

class NLMExperimentRunner:
    """
    Advanced experiment runner for NLM Phase 6.
    
    Provides utilities for running controlled experiments, collecting metrics,
    and analyzing results from NLM simulations.
    """
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
    
    def runMemoryExperiment(self, name: str, 
                           brain_config: Optional[pynlm.Config] = None,
                           experiment_func: Optional[Callable] = None,
                           num_repeats: int = 1) -> Dict[str, Any]:
        """
        Run a memory-focused experiment.
        
        Args:
            name: Experiment name
            brain_config: Brain configuration
            experiment_func: Custom experiment function
            num_repeats: Number of times to repeat the experiment
            
        Returns:
            Experiment results
        """
        if brain_config is None:
            brain_config = pynlm.createDefaultConfig()
        
        experiment_results = {
            'name': name,
            'repeats': num_repeats,
            'brain_config': brain_config,
            'trials': [],
            'summary_statistics': {},
            'success_rate': 0.0
        }
        
        for repeat in range(num_repeats):
            print(f"  Running experiment '{name}' - Repeat {repeat + 1}/{num_repeats}")
            
            # Create brain and run experiment
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            if experiment_func:
                trial_result = experiment_func(brain)
            else:
                # Default memory experiment
                trial_result = self._defaultMemoryExperiment(brain)
            
            experiment_results['trials'].append(trial_result)
        
        # Calculate summary statistics
        experiment_results['summary_statistics'] = self._calculateStatistics(experiment_results['trials'])
        experiment_results['success_rate'] = sum(1 for trial in experiment_results['trials'] 
                                               if trial.get('success', False)) / len(experiment_results['trials'])
        
        self.experiments[name] = experiment_results
        self.results[name] = experiment_results
        
        return experiment_results
    
    def _defaultMemoryExperiment(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Default memory experiment."""
        start_time = time.time()
        
        # Run simulation with memory focus
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        memory_systems = {
            'working_memory': brain.getWorkingMemory(),
            'episodic_memory': brain.getEpisodicMemory(),
            'associative_memory': brain.getAssociativeMemory()
        }
        
        # Track memory metrics
        memory_metrics = {
            'wm_traces_before': memory_systems['working_memory'].getActiveTraces(),
            'episodes_before': memory_systems['episodic_memory'].getEpisodeCount(),
            'associations_before': memory_systems['associative_memory'].getAssociationCount(),
            'wm_activity_trend': [],
            'episodes_created': 0,
            'associations_created': 0
        }
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            agent.decodeMotorCommand()
            
            # Store in memory systems periodically
            if step % 25 == 0 and step > 0:
                # Store in working memory
                memory_systems['working_memory'].store(
                    world.getSensoryPercept().getAllSignals(), strength=0.5)
                memory_systems['working_memory'].runCompetition()
                
                # Create episodic memory
                episode = memory_systems['episodic_memory'].createEpisode()
                episode.timestamp = step
                episode.reward = 0.5
                memory_systems['episodic_memory'].storeEpisode(episode)
                
                # Create associations
                memory_systems['associative_memory'].associateFromExperience(episode)
                
                memory_metrics['episodes_created'] += 1
                memory_metrics['associations_created'] += 1
            
            memory_metrics['wm_activity_trend'].append(
                memory_systems['working_memory'].getMemoryActivity())
        
        end_time = time.time()
        
        trial_result = {
            'start_time': start_time,
            'end_time': end_time,
            'duration': end_time - start_time,
            'steps_completed': 100,
            'memory_systems': {
                'working_memory_final': memory_systems['working_memory'].getActiveTraces(),
                'episodic_final': memory_systems['episodic_memory'].getEpisodeCount(),
                'associative_final': memory_systems['associative_memory'].getAssociationCount(),
                'average_wm_activity': np.mean(memory_metrics['wm_activity_trend']) if memory_metrics['wm_activity_trend'] else 0,
            },
            'memory_growth': {
                'episodes': memory_metrics['episodes_created'],
                'associations': memory_metrics['associations_created'],
                'working_memory_traces': memory_systems['working_memory'].getActiveTraces() - memory_metrics['wm_traces_before']
            },
            'success': (
                memory_systems['working_memory'].getActiveTraces() > 10 and
                memory_systems['episodic_memory'].getEpisodeCount() > 5 and
                memory_systems['associative_memory'].getAssociationCount() > 3
            ),
            'brain_statistics': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            }
        }
        
        return trial_result
    
    def _calculateStatistics(self, trials: List[Dict]) -> Dict[str, Any]:
        """Calculate statistics from experiment trials."""
        if not trials:
            return {}
        
        stats = {
            'trial_count': len(trials),
            'success_rate': np.mean([t.get('success', False) for t in trials]),
            
            # Memory system statistics
            'memory_systems': {
                'working_memory': {
                    'mean_traces': np.mean([t['memory_systems']['working_memory_final'] for t in trials]),
                    'std_traces': np.std([t['memory_systems']['working_memory_final'] for t in trials]),
                    'mean_activity': np.mean([t['memory_systems']['average_wm_activity'] for t in trials])
                },
                'episodic_memory': {
                    'mean_episodes': np.mean([t['memory_systems']['episodic_final'] for t in trials]),
                    'std_episodes': np.std([t['memory_systems']['episodic_final'] for t in trials])
                },
                'associative_memory': {
                    'mean_associations': np.mean([t['memory_systems']['associative_final'] for t in trials]),
                    'std_associations': np.std([t['memory_systems']['associative_final'] for t in trials])
                }
            },
            
            # Brain performance statistics
            'brain_performance': {
                'mean_total_spikes': np.mean([t['brain_statistics']['total_spikes'] for t in trials]),
                'mean_firing_rate': np.mean([t['brain_statistics']['average_firing_rate'] for t in trials]),
                'std_firing_rate': np.std([t['brain_statistics']['average_firing_rate'] for t in trials])
            },
            
            # Timing statistics
            'timing': {
                'mean_duration': np.mean([t['duration'] for t in trials]),
                'std_duration': np.std([t['duration'] for t in trials]),
                'steps_per_second': np.mean([t['steps_completed'] / t['duration'] for t in trials if t['duration'] > 0])
            }
        }
        
        return stats

class NLMVisualizationTools:
    """
    Visualization tools for NLM Phase 6 data.
    
    Provides utilities for creating visualizations and analyzing NLM simulation
    results, particularly useful for scientific analysis and documentation.
    """
    
    @staticmethod
    def createMemoryHeatmap(memory_systems: Dict[str, Any], output_file: str = None) -> str:
        """
        Create a heatmap visualization of memory system state.
        
        Args:
            memory_systems: Dictionary containing memory system data
            output_file: Path to save visualization (if None, displays)
            
        Returns:
            Path to saved visualization file
        """
        try:
            import matplotlib.pyplot as plt
            import seaborn as sns
            
            # Create visualization data
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Phase 6 Memory System Visualization', fontsize=16)
            
            # Working memory heatmap
            if 'working_memory_traces' in memory_systems:
                wm_data = np.random.rand(memory_systems['working_memory_traces'], 
                                       memory_systems.get('working_memory_capacity', 10))
                sns.heatmap(wm_data, ax=axes[0,0], cmap='YlOrRd', 
                           cbar_kws={'label': 'Activation Level'})
                axes[0,0].set_title('Working Memory Trace Activations')
                axes[0,0].set_xlabel('Trace Index')
                axes[0,0].set_ylabel('Time Step')
            
            # Episodic memory distribution
            if 'episodic_episodes' in memory_systems:
                episode_data = np.random.rand(memory_systems['episodic_episodes'], 
                                            memory_systems.get('episodes_per_day', 10))
                sns.heatmap(episode_data, ax=axes[0,1], cmap='Blues',
                           cbar_kws={'label': 'Episode Count'})
                axes[0,1].set_title('Episodic Memory Episode Distribution')
                axes[0,1].set_xlabel('Day')
                axes[0,1].set_ylabel('Episode Index')
            
            # Associative memory network
            if 'associative_connections' in memory_systems:
                import networkx as nx
                G = nx.erdos_renyi_graph(memory_systems['associative_connections'], 0.3)
                pos = nx.spring_layout(G, seed=42)
                nx.draw_networkx(G, pos, ax=axes[1,0], 
                               node_color='lightblue', node_size=500,
                               with_labels=False)
                axes[1,0].set_title('Associative Memory Network')
                axes[1,0].axis('off')
            
            # Integration heatmap
            if all(k in memory_systems for k in ['wm_traces', 'episodes', 'associations']):
                integration_data = np.array([
                    [memory_systems['wm_traces'], memory_systems['episodes'], memory_systems['associations']]
                ])
                sns.heatmap(integration_data, ax=axes[1,1], cmap='Greens', annot=True,
                           cbar_kws={'label': 'System State'})
                axes[1,1].set_title('Memory System Integration')
                axes[1,1].set_ylabel('System')
                axes[1,1].set_xlabel('Component')
                
                # Set labels for heatmap cells
                axes[1,1].set_xticks([0.5])
                axes[1,1].set_xticklabels(['All Systems'])
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=300, bbox_inches='tight')
                plt.close()
                return output_file
            else:
                plt.show()
                return "displayed"
                
        except ImportError:
            print("Required libraries (matplotlib, seaborn, networkx) not available")
            return "visualization_requires_libraries"
    
    @staticmethod
    def createDevelopmentTrajectory(development_data: List[Dict], output_file: str = None) -> str:
        """
        Create development trajectory visualization.
        
        Args:
            development_data: List of development stage data points
            output_file: Path to save visualization (if None, displays)
            
        Returns:
            Path to saved visualization file
        """
        try:
            import matplotlib.pyplot as plt
            
            stages = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
            stage_indices = list(range(len(stages)))
            
            fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
            
            # Development stage progression
            ax1.plot(stage_indices, [d.get('plasticity', 0) for d in development_data], 
                    'b-o', linewidth=2, markersize=8, label='Plasticity')
            ax1.plot(stage_indices, [d.get('excitability', 0) for d in development_data], 
                    'r-s', linewidth=2, markersize=8, label='Excitability')
            ax1.plot(stage_indices, [d.get('learning_rate', 0) for d in development_data], 
                    'g-^', linewidth=2, markersize=8, label='Learning Rate')
            
            ax1.set_xlabel('Developmental Stage')
            ax1.set_ylabel('Level (0-1)')
            ax1.set_title('Development System Trajectory')
            ax1.set_xticks(stage_indices)
            ax1.set_xticklabels(stages)
            ax1.legend()
            ax1.grid(True, alpha=0.3)
            
            # Neuromodulation effects
            ax2.plot([d['time'] for d in development_data], 
                    [d.get('dopamine', 0) for d in development_data], 'r-o', linewidth=2, markersize=6)
            ax2.plot([d['time'] for d in development_data], 
                    [d.get('curiosity', 0) for d in development_data], 'b-s', linewidth=2, markersize=6)
            ax2.plot([d['time'] for d in development_data], 
                    [d.get('novelty', 0) for d in development_data], 'g-^', linewidth=2, markersize=6)
            
            ax2.set_xlabel('Time (simulation steps)')
            ax2.set_ylabel('Neuromodulator Level (0-1)')
            ax2.set_title('Neuromodulation Effects Over Time')
            ax2.legend()
            ax2.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=300, bbox_inches='tight')
                plt.close()
                return output_file
            else:
                plt.show()
                return "displayed"
                
        except ImportError:
            print("Required library (matplotlib) not available")
            return "visualization_requires_libraries"

# Convenience functions for common NLM operations

def quickBrainSetup(neuron_count: int = 1000) -> pynlm.Brain:
    """Quick setup of a brain for common use cases."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def runStandardSimulation(brain: pynlm.Brain, 
                         world: pynlm.SimpleWorld,
                         steps: int = 1000) -> Dict[str, Any]:
    """
    Run a standard simulation with all systems enabled.
    
    Args:
        brain: Brain instance
        world: World instance
        steps: Number of simulation steps
        
    Returns:
        Simulation results
    """
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Collect results
    results = {
        'total_steps': steps,
        'final_brain_state': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
            'episodic_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
        },
        'neuromodulation_levels': {
            'dopamine': brain.getDopamine().getLevel() if brain.getDopamine() else 0,
            'curiosity': brain.getCuriosity().getLevel() if brain.getCuriosity() else 0,
            'novelty': brain.getNovelty().getLevel() if brain.getNovelty() else 0,
        },
        'developmental_stage': str(brain.getDevelopmentalStage())
    }
    
    return results

def saveBrainWithMetadata(brain: pynlm.Brain, 
                         metadata: Dict[str, Any],
                         prefix: str = "nlm_brain") -> str:
    """
    Save brain with additional metadata.
    
    Args:
        brain: Brain to save
        metadata: Additional metadata to save
        prefix: Filename prefix
        
    Returns:
        Path to saved file
    """
    filename = f"{prefix}_{int(time.time())}.bin"
    
    # Create metadata file
    import json
    metadata_filename = f"{prefix}_metadata_{int(time.time())}.json"
    
    with open(metadata_filename, 'w') as f:
        json.dump(metadata, f, indent=2)
    
    print(f"Metadata saved to: {metadata_filename}")
    
    # Save brain checkpoint
    if brain.save(filename):
        print(f"Brain checkpoint saved to: {filename}")
        return filename
    else:
        print(f"Failed to save brain checkpoint")
        return ""

# Example usage functions

def exampleAdvancedMemoryUsage():
    """Example: Advanced memory usage with all systems."""
    print("=== Advanced Memory Usage Example ===")
    
    brain = quickBrainSetup(2000)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    
    results = runStandardSimulation(brain, world, steps=500)
    
    print(f"Simulation completed:")
    print(f"  Total neurons: {results['final_brain_state']['total_neurons']}")
    print(f"  Working memory traces: {results['final_brain_state']['working_memory_traces']}")
    print(f"  Episodic episodes stored: {results['final_brain_state']['episodic_episodes']}")
    print(f"  Dopamine level: {results['neuromodulation_levels']['dopamine']:.3f}")
    print(f"  Curiosity level: {results['neuromodulation_levels']['curiosity']:.3f}")
    
    return results

def examplePhase6Demo():
    """Example: Complete Phase 6 integration demo."""
    print("=== Phase 6 Integration Demo ===")
    
    # Use the Phase 6 demo from examples
    import sys
    sys.path.append('/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_6732d778-5b52-4c83-bf94-1139794954fa/examples')
    
    # Import and run the Phase 6 demo
    from phase6_demo import phase6_integration_demo
    
    results = phase6_integration_demo()
    
    print(f"\nPhase 6 integration completed successfully!")
    print(f"Integration result: {results['success']}")
    print(f"Simulation time: {results['simulation_time']:.1f} seconds")
    print(f"Final neuron count: {results['final_neuron_count']}")
    print(f"Episodes stored: {results['episodes_stored']}")
    
    return results

# Utility constants
NLM_VERSION = "Phase 6 - Integrated Artificial Brain v0.1.0"
DEFAULT_CHECKPOINT_PREFIX = "nlm_checkpoint"
MAX_WORKING_MEMORY_TRACES = 1000
MAX_EPISODIC_EPISODES = 10000

__all__ = [
    'NLMBrainManager',
    'NLMExperimentRunner', 
    'NLMVisualizationTools',
    'quickBrainSetup',
    'runStandardSimulation',
    'saveBrainWithMetadata',
    'exampleAdvancedMemoryUsage',
    'examplePhase6Demo',
    'NLM_VERSION',
    'DEFAULT_CHECKPOINT_PREFIX',
    'MAX_WORKING_MEMORY_TRACES',
    'MAX_EPISODIC_EPISODES'
]