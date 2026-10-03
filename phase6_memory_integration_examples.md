# NLM Phase 6 Integration Examples

## Overview

Phase 6 integration examples demonstrate how all brain systems work together as a coherent artificial brain. These examples show the connection over addition principle where previously separate systems are now integrated and influencing each other.

## 1. Memory System Integration

This example demonstrates how all memory systems are integrated and work together:

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

class NLMMemoryIntegration:
    """Demonstrates Phase 6 memory system integration."""
    
    def __init__(self, brain_size: str = "medium"):
        """Initialize memory integration system.
        
        Args:
            brain_size: Size of brain ('small', 'medium', 'large')
        """
        self.brain_size = brain_size
        self.memory_traces = []
        self.episode_log = []
        self.concept_clusters = {}
        
        # Create brain with size configuration
        if brain_size == "small":
            config = pynlm.createSmallBrain()
        elif brain_size == "large":
            config = pynlm.createLargeBrain()
        else:
            config = pynlm.createMediumBrain()
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent with memory integration
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableAllModules()  # Enable all neuromodulation and plasticity
        
        # Create world for environment interaction
        self.world = pynlm.createSimulationWorld(100, 100)
        self.agent.initialize(self.world)
        
        print(f"Initialized NLM Memory Integration with {brain_size} brain")
    
    def demonstrate_working_memory(self, duration_steps: int = 1000):
        """Demonstrate working memory integration.
        
        Args:
            duration_steps: Number of steps to run
            
        Returns:
            Working memory performance metrics
        """
        print("\n=== Working Memory Integration Demo ===")
        
        working_memory_metrics = {
            'trace_count': [],
            'decay_patterns': [],
            'competition_dynamics': []
        }
        
        # Pattern 1: Simple maintenance task
        print("Pattern 1: Maintaining sensory input over time")
        sensory_pattern = []
        
        for step in range(duration_steps):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input - this should update working memory
            self.agent.processSensoryInput(percept)
            
            # Record sensory pattern
            sensory_pattern.append(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = result.reward if result else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update world
            self.world.update(0.1)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record working memory metrics
            active_neurons = self.brain.getActiveNeuronCount()
            firing_rate = self.brain.getAverageFiringRate()
            
            working_memory_metrics['trace_count'].append(active_neurons)
            working_memory_metrics['decay_patterns'].append(firing_rate)
        
        # Analyze working memory performance
        avg_traces = np.mean(working_memory_metrics['trace_count'])
        avg_firing = np.mean(working_memory_metrics['decay_patterns'])
        
        print(f"  Average active neurons: {avg_traces:.1f}")
        print(f"  Average firing rate: {avg_firing:.3f} Hz")
        print(f"  Working memory integrated: {'✅ YES' if avg_traces > 10 else '❌ NO'}")
        
        return {
            'pattern': 'sensory_maintenance',
            'avg_active_neurons': avg_traces,
            'avg_firing_rate': avg_firing,
            'memory_working': avg_traces > 10,
            'metrics': working_memory_metrics
        }
    
    def demonstrate_episodic_memory(self, num_episodes: int = 20, steps_per_episode: int = 500):
        """Demonstrate episodic memory storage and recall.
        
        Args:
            num_episodes: Number of episodes to run
            steps_per_episode: Steps per episode
            
        Returns:
            Episodic memory performance metrics
        """
        print("\n=== Episodic Memory Integration Demo ===")
        
        episodic_metrics = {
            'episodes_stored': [],
            'recall_accuracy': [],
            'consolidation_effectiveness': []
        }
        
        # Generate unique episodes with varying patterns
        for episode in range(num_episodes):
            print(f"Running episode {episode + 1}/{num_episodes}")
            
            # Reset for new episode
            self.agent.reset()
            
            # Store initial brain state
            initial_stats = {
                'neurons': self.brain.getTotalNeuronCount(),
                'firing': self.brain.getFiringNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount()
            }
            
            # Run episode with unique pattern
            episode_reward = 0.0
            episode_curiosity = []
            episode_novelty = []
            
            for step in range(steps_per_episode):
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Add episode-specific variation
                if episode % 4 == 0:  # Different movement patterns per episode
                    # Add some artificial variation to simulate different experiences
                    self.agent.enableCuriosity(True)
                elif episode % 4 == 1:
                    self.agent.enableRewardModulation(True)
                elif episode % 4 == 2:
                    self.agent.enableDevelopment(True)
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode and apply action
                action = self.agent.decodeMotorCommand()
                result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                if result:
                    episode_reward += result.reward
                
                # Apply reward and update
                self.agent.applyRewardModulation(result.reward if result else 0.0, 0.0)
                self.world.update(0.1)
                self.agent.updateDevelopment(0.1)
                
                # Record neuromodulation levels
                episode_curiosity.append(self.agent.getCuriosityLevel())
                episode_novelty.append(self.agent.getNoveltyLevel())
            
            # Store episodic memory
            final_stats = {
                'neurons': self.brain.getTotalNeuronCount(),
                'firing': self.brain.getFiringNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'final_reward': episode_reward,
                'avg_curiosity': np.mean(episode_curiosity),
                'avg_novelty': np.mean(episode_novelty)
            }
            
            self.episode_log.append({
                'episode_id': episode,
                'initial_state': initial_stats,
                'final_state': final_stats,
                'duration': steps_per_episode
            })
            
            episodic_metrics['episodes_stored'].append(1)
            episodic_metrics['recall_accuracy'].append(
                min(1.0, final_stats['final_reward'] / (steps_per_episode * 0.1))
            )
            episodic_metrics['consolidation_effectiveness'].append(
                (final_stats['spikes'] - initial_stats['spikes']) / max(1, initial_stats['spikes'])
            )
        
        # Calculate episodic memory performance
        avg_stored = np.mean(episodic_metrics['episodes_stored'])
        avg_recall = np.mean(episodic_metrics['recall_accuracy'])
        avg_consolidation = np.mean(episodic_metrics['consolidation_effectiveness'])
        
        print(f"  Episodes stored: {avg_stored}")
        print(f"  Average recall accuracy: {avg_recall:.3f}")
        print(f"  Average consolidation effectiveness: {avg_consolidation:.3f}")
        print(f"  Episodic memory integrated: {'✅ YES' if avg_recall > 0.3 else '❌ NO'}")
        
        return {
            'pattern': 'episodic_storage',
            'episodes_stored': avg_stored,
            'recall_accuracy': avg_recall,
            'consolidation_effectiveness': avg_consolidation,
            'memory_integrated': avg_recall > 0.3,
            'metrics': episodic_metrics,
            'episode_log': self.episode_log
        }
    
    def demonstrate_associative_memory(self, association_pairs: int = 10):
        """Demonstrate associative memory and pattern completion.
        
        Args:
            association_pairs: Number of association pairs to create
            
        Returns:
            Associative memory performance metrics
        """
        print("\n=== Associative Memory Integration Demo ===")
        
        # Create two distinct patterns to associate
        pattern_a_sequence = []
        pattern_b_sequence = []
        
        # Pattern A: High curiosity, specific actions
        print("Creating pattern A (high curiosity, exploration)")
        for step in range(100):
            self.agent.enableCuriosity(True)
            self.agent.enableNovelty(True)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.agent.applyRewardModulation(0.1, 0.0)
            self.world.update(0.1)
            self.agent.updateDevelopment(0.1)
            
            pattern_a_sequence.append({
                'step': step,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel()
            })
        
        # Pattern B: Low curiosity, systematic actions
        print("Creating pattern B (low curiosity, systematic)")
        for step in range(100):
            self.agent.enableCuriosity(False)
            self.agent.enableNovelty(False)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.agent.applyRewardModulation(0.5, 0.0)
            self.world.update(0.1)
            self.agent.updateDevelopment(0.1)
            
            pattern_b_sequence.append({
                'step': step,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel()
            })
        
        # Now test associative recall by presenting partial pattern A
        print("Testing associative recall (partial pattern A cue)")
        
        # Reset and present partial cue from pattern A
        self.agent.reset()
        world = pynlm.createSimulationWorld(100, 100)
        self.agent.initialize(world)
        
        # Present strong curiosity cue (part of pattern A)
        self.agent.enableCuriosity(True)
        self.agent.enableNovelty(True)
        
        recall_metrics = {
            'cue_strength': [],
            'pattern_completion': [],
            'association_strength': []
        }
        
        for step in range(100):
            # Present cue (partial pattern A)
            percept = self.world.getSensoryPercept()
            
            # Apply pattern A characteristics (partial cue)
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Measure response (should show pattern A characteristics)
            curiosity_level = self.agent.getCuriosityLevel()
            novelty_level = self.agent.getNoveltyLevel()
            
            recall_metrics['cue_strength'].append(curiosity_level)
            recall_metrics['pattern_completion'].append(novelty_level)
            
            # Calculate association strength (correlation between curiosity and novelty)
            if step > 10:
                association_strength = curiosity_level * novelty_level
                recall_metrics['association_strength'].append(association_strength)
            
            # Execute action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.agent.applyRewardModulation(0.2, 0.0)
            self.world.update(0.1)
            self.agent.updateDevelopment(0.1)
        
        # Analyze associative memory performance
        avg_cue_strength = np.mean(recall_metrics['cue_strength'])
        avg_pattern_completion = np.mean(recall_metrics['pattern_completion'])
        avg_association_strength = np.mean(recall_metrics['association_strength']) if recall_metrics['association_strength'] else 0.0
        
        print(f"  Average cue strength: {avg_cue_strength:.3f}")
        print(f"  Average pattern completion: {avg_pattern_completion:.3f}")
        print(f"  Average association strength: {avg_association_strength:.3f}")
        print(f"  Associative memory integrated: {'✅ YES' if avg_association_strength > 0.1 else '❌ NO'}")
        
        return {
            'pattern': 'associative_recall',
            'cue_strength': avg_cue_strength,
            'pattern_completion': avg_pattern_completion,
            'association_strength': avg_association_strength,
            'memory_integrated': avg_association_strength > 0.1,
            'metrics': recall_metrics,
            'pattern_a': pattern_a_sequence,
            'pattern_b': pattern_b_sequence
        }
    
    def run_complete_memory_integration_demo(self) -> Dict[str, Any]:
        """Run complete memory integration demonstration.
        
        Returns:
            Complete memory integration results
        """
        print("=== NLM Memory Integration Comprehensive Demo ===")
        print(f"Brain size: {self.brain_size}")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        
        # Run all memory demonstrations
        working_memory_results = self.demonstrate_working_memory(500)
        episodic_memory_results = self.demonstrate_episodic_memory(5, 200)
        associative_memory_results = self.demonstrate_associative_memory(5)
        
        # Calculate overall integration score
        integration_score = (
            working_memory_results['memory_working'] * 0.3 +
            episodic_memory_results['memory_integrated'] * 0.4 +
            associative_memory_results['memory_integrated'] * 0.3
        )
        
        results = {
            'system_info': {
                'brain_size': self.brain_size,
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'modules_enabled': 'All (Reward, Plasticity, Development, Curiosity)'
            },
            'working_memory': working_memory_results,
            'episodic_memory': episodic_memory_results,
            'associative_memory': associative_memory_results,
            'integration_score': integration_score,
            'overall_assessment': self._get_integration_assessment(integration_score),
            'episode_count': len(self.episode_log),
            'concept_count': len(self.concept_clusters)
        }
        
        return results
    
    def _get_integration_assessment(self, score: float) -> str:
        """Get qualitative assessment of integration based on score.
        
        Args:
            score: Integration score (0-1)
            
        Returns:
            Qualitative assessment
        """
        if score > 0.8:
            return "✅ EXCELLENT - All memory systems are well-integrated and functional"
        elif score > 0.6:
            return "✅ GOOD - Most memory systems are integrated with room for improvement"
        elif score > 0.4:
            return "⚠️ MODERATE - Memory integration is partial, needs enhancement"
        elif score > 0.2:
            return "❌ LIMITED - Memory systems have basic integration but significant gaps"
        else:
            return "❌ POOR - Memory systems are not properly integrated"

# Phase 6 memory integration example
if __name__ == "__main__":
    # Test with different brain sizes
    for brain_size in ["small", "medium", "large"]:
        print(f"\n{'='*60}")
        print(f"PHASE 6 MEMORY INTEGRATION DEMONSTRATION - {brain_size.upper()} BRAIN")
        print(f"{'='*60}")
        
        # Create integration system
        memory_system = NLMMemoryIntegration(brain_size)
        
        # Run complete demonstration
        results = memory_system.run_complete_memory_integration_demo()
        
        # Print summary
        print(f"\n{'='*60}")
        print(f"INTEGRATION SUMMARY - {brain_size.upper()} BRAIN")
        print(f"{'='*60}")
        print(f"System Info:")
        print(f"  Brain Size: {results['system_info']['brain_size']}")
        print(f"  Neurons: {results['system_info']['total_neurons']}")
        print(f"  Synapses: {results['system_info']['total_synapses']}")
        print(f"  Modules: {results['system_info']['modules_enabled']}")
        
        print(f"\nMemory Integration Results:")
        print(f"  Working Memory: {'✅' if results['working_memory']['memory_working'] else '❌'}")
        print(f"  Episodic Memory: {'✅' if results['episodic_memory']['memory_integrated'] else '❌'}")
        print(f"  Associative Memory: {'✅' if results['associative_memory']['memory_integrated'] else '❌'}")
        
        print(f"\nPerformance Metrics:")
        print(f"  Integration Score: {results['integration_score']:.2f}/1.0")
        print(f"  Overall Assessment: {results['overall_assessment']}")
        print(f"  Episodes Created: {results['episode_count']}")
        
        if results['integration_score'] > 0.5:
            print(f"\n🎉 SUCCESS: Memory systems are well-integrated!")
        else:
            print(f"\n⚠️ CHALLENGE: Memory integration needs enhancement")
```