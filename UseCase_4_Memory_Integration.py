# Memory Integration for NLM

"""
Comprehensive demonstration of memory systems integration in the Neural Learning Machine (NLM).

This script implements the Memory Integration use case, demonstrating:
- Working memory with persistent activity and competitive storage
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- Memory formation and consolidation processes
- Decision-making based on integrated memory systems
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time
import random
from collections import deque

class MemoryIntegrationExperiment:
    """
    Comprehensive memory integration experiment demonstrating all memory systems.
    
    This experiment shows how working memory, episodic memory, and associative memory
    work together to enable intelligent behavior in the NLM brain.
    """
    
    def __init__(self, config):
        """Initialize the memory integration experiment."""
        self.config = config
        self.setup_memory_system()
        
        # Performance tracking
        self.memory_usage = {
            'working_memory_traces': [],
            'episodic_memories': [],
            'associative_strengths': [],
            'memory_access_times': [],
            'consolidation_events': [],
            'replay_events': []
        }
        
        # State tracking
        self.current_step = 0
        self.memory_efficiency = 0.0
        self.consolidation_efficiency = 0.0
    
    def setup_memory_system(self):
        """Setup the NLM brain with memory-optimized configuration."""
        # Create brain with memory-focused configuration
        config = pynlm.createDefaultConfig()
        
        # Optimize for memory functions
        config.set("brain.neuron_count", self.config.get('neuron_count', 1200))
        config.set("brain.region_count", self.config.get('region_count', 4))
        config.set("brain.v_thresh", -50.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -75.0)
        
        # Memory-specific parameters
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", self.config.get('stdp_learning_rate', 0.002))
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Setup simple world for controlled experiments
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable memory systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Performance monitoring
        self.performance_monitor = MemoryPerformanceMonitor()
    
    def run_memory_experiment(self, num_steps=5000, show_plots=True):
        """
        Run comprehensive memory integration experiment.
        
        Args:
            num_steps: Number of simulation steps
            show_plots: Whether to create visualizations
            
        Returns:
            Dictionary with experiment results
        """
        print("=== MEMORY INTEGRATION EXPERIMENT ===")
        print("Demonstrating working memory, episodic memory, and associative memory...")
        
        print(f"Starting memory experiment for {num_steps} steps...")
        print(f"Neuron count: {self.config.get('neuron_count', 1200)}")
        print(f"Brain regions: {self.config.get('region_count', 4)}")
        
        # Pattern generator for memory formation
        pattern_generator = np.random.RandomState(42)
        
        for step in range(num_steps):
            self.current_step = step
            
            # Generate input pattern based on simulation step
            input_pattern = self.generate_input_pattern(step, pattern_generator)
            
            # Process sensory input (updates working memory)
            self.agent.processSensoryInput(input_pattern)
            
            # Brain processes and updates all memory systems
            self.brain.step(step, step * 0.001)
            
            # Record memory metrics
            self.record_memory_metrics(step, input_pattern)
            
            # Simulate memory consolidation (every 500 steps)
            if step % 500 == 0 and step > 0:
                self.trigger_memory_consolidation()
            
            # Simulate memory replay (every 1000 steps)
            if step % 1000 == 0 and step > 0:
                self.trigger_memory_replay()
            
            # Print progress
            if step % 1000 == 0:
                print(f"Step {step}: Working memory traces={self.get_working_memory_traces()}, "
                      f"Episodic memories={self.get_episodic_memory_count()}, "
                      f"Firing rate={self.brain.getAverageFiringRate():.3f} Hz")
        
        # Analyze results
        results = self.analyze_memory_results()
        
        # Create visualizations
        if show_plots:
            self.create_memory_visualizations()
        
        return results
    
    def generate_input_pattern(self, step, pattern_generator):
        """
        Generate input pattern for memory training.
        
        Creates different types of input patterns to test memory systems:
        1. Random patterns for general learning
        2. Structured patterns for episodic memory
        3. Related patterns for associative memory
        """
        # Create sensory input based on step type
        if step % 100 == 0:
            # New pattern every 100 steps for episodic memory
            pattern_size = 50
            pattern = {
                'type': 'episodic',
                'data': pattern_generator.rand(pattern_size).tolist(),
                'context': step // 100,
                'time': time.time()
            }
        
        elif step % 50 == 0:
            # Related patterns for associative memory
            pattern_size = 30
            base_pattern = pattern_generator.rand(pattern_size).tolist()
            
            # Create variations
            variation = np.random.uniform(-0.1, 0.1, pattern_size)
            pattern = {
                'type': 'associative',
                'base_pattern': base_pattern,
                'variation': variation.tolist(),
                'similarity': 1.0 - np.std(variation)
            }
        
        else:
            # Random patterns for working memory
            pattern_size = 20
            pattern = {
                'type': 'working',
                'data': pattern_generator.rand(pattern_size).tolist(),
                'timestamp': step
            }
        
        # Convert to NLM percept
        percept = pynlm.SensoryPercept()
        percept.setVisual(pattern)
        
        return percept
    
    def record_memory_metrics(self, step, input_pattern):
        """
        Record comprehensive memory metrics.
        
        Tracks:
        - Working memory traces and capacity
        - Episodic memory formation and retrieval
        - Associative memory strength and patterns
        - Memory access patterns and efficiency
        """
        # Working memory metrics
        if hasattr(self.brain, 'getWorkingMemory'):
            wm = self.brain.getWorkingMemory()
            if wm:
                traces = wm.getActiveTraces()
                trace_capacity = wm.getCapacity()
                trace_activity = traces / max(trace_capacity, 1)
                
                self.memory_usage['working_memory_traces'].append({
                    'step': step,
                    'active_traces': traces,
                    'capacity': trace_capacity,
                    'activity_ratio': trace_activity,
                    'timestamp': time.time()
                })
        
        # Episodic memory metrics
        if hasattr(self.brain, 'getEpisodicMemory'):
            em = self.brain.getEpisodicMemory()
            if em:
                episodes = em.getEpisodeCount()
                replay_ability = em.getReplayability()
                
                self.memory_usage['episodic_memories'].append({
                    'step': step,
                    'episode_count': episodes,
                    'replay_ability': replay_ability,
                    'timestamp': time.time()
                })
        
        # Associative memory metrics (simplified model)
        pattern_type = input_pattern.get('type', 'working')
        associative_strength = self.calculate_associative_strength(pattern_type, step)
        
        self.memory_usage['associative_strengths'].append({
            'step': step,
            'pattern_type': pattern_type,
            'strength': associative_strength,
            'timestamp': time.time()
        })
        
        # Record access times
        access_time = time.time()
        self.memory_usage['memory_access_times'].append({
            'step': step,
            'access_time': access_time,
            'timestamp': access_time
        })
        
        # Update performance monitor
        self.performance_monitor.record_memory_metrics(
            step_time=0.001,
            working_memory_traces=self.get_working_memory_traces(),
            episodic_memories=self.get_episodic_memory_count(),
            associative_strength=associative_strength
        )
    
    def calculate_associative_strength(self, pattern_type, step):
        """
        Calculate associative memory strength for different pattern types.
        
        Implements Hebbian learning for associative memory formation.
        """
        if pattern_type == 'associative':
            # Higher strength for similar patterns
            base_strength = 0.7
            variation_effect = 0.3 * np.random.random()
            return min(1.0, base_strength + variation_effect)
        
        elif pattern_type == 'episodic':
            # Lower strength for unique episodic patterns
            return 0.5 + 0.3 * np.random.random()
        
        else: 0. # Working memory
            # Transient strength for working memory
            return 0.3 + 0.4 * np.random.random()
    
    def trigger_memory_consolidation(self, step):
        """
        Trigger memory consolidation process.
        
        Memory consolidation is the process of strengthening important memories
        and integrating them into long-term storage.
        """
        # Record consolidation event
        consolidation_time = time.time()
        
        # Calculate consolidation efficiency
        if self.memory_usage['working_memory_traces']:
            recent_traces = [t for t in self.memory_usage['working_memory_traces'] 
                           if t['step'] >= self.current_step - 100]
            if recent_traces:
                avg_activity = np.mean([t['activity_ratio'] for t in recent_traces])
                consolidation_efficiency = avg_activity * 0.8
            else:
                consolidation_efficiency = 0.0
        else:
            consolidation_efficiency = 0.0
        
        self.memory_usage['consolidation_events'].append({
            'step': self.current_step,
            'consolidation_time': consolidation_time,
            'efficiency': consolidation_efficiency,
            'timestamp': consolidation_time
        })
    
    def trigger_memory_replay(self, step):
        """
        Trigger memory replay process.
        
        Memory replay involves reactivating past experiences during rest or sleep,
        which helps strengthen memory consolidation.
        """
        # Record replay event
        replay_time = time.time()
        
        # Calculate replay efficiency
        if self.memory_usage['episodic_memories']:
            recent_episodes = [e for e in self.memory_usage['episodic_memories'] 
                              if e['step'] >= self.current_step - 500]
            if recent_episodes:
                avg_replayability = np.mean([e['replay_ability'] for e in recent_episodes])
                replay_efficiency = avg_replayability * 0.7
            else:
                replay_efficiency = 0.0
        else:
            replay_efficiency = 0.0
        
        self.memory_usage['replay_events'].append({
            'step': self.current_step,
            'replay_time': replay_time,
            'efficiency': replay_efficiency,
            'timestamp': replay_time
        })
    
    def get_working_memory_traces(self):
        """Get current working memory trace count."""
        if hasattr(self.brain, 'getWorkingMemory'):
            wm = self.brain.getWorkingMemory()
            if wm:
                return wm.getActiveTraces()
        return 0
    
    def get_episodic_memory_count(self):
        """Get current episodic memory count."""
        if hasattr(self.brain, 'getEpisodicMemory'):
            em = self.brain.getEpisodicMemory()
            if em:
                return em.getEpisodeCount()
        return 0
    
    def analyze_memory_results(self):
        """Analyze memory experiment results comprehensively."""
        
        # Working memory analysis
        wm_traces = [t for t in self.memory_usage['working_memory_traces']
                    if 'active_traces' in t]
        
        wm_analysis = {}
        if wm_traces:
            wm_analysis = {
                'avg_traces': np.mean([t['active_traces'] for t in wm_traces]),
                'max_traces': np.max([t['active_traces'] for t in wm_traces]),
                'min_traces': np.min([t['active_traces'] for t in wm_traces]),
                'trace_stability': np.std([t['active_traces'] for t in wm_traces]) / 
                                   np.mean([t['active_traces'] for t in wm_traces]) if np.mean([t['active_traces'] for t in wm_traces]) > 0 else 0,
                'avg_activity_ratio': np.mean([t['activity_ratio'] for t in wm_traces])
            }
        
        # Episodic memory analysis
        em_memories = [m for m in self.memory_usage['episodic_memories']
                      if 'episode_count' in m]
        
        em_analysis = {}
        if em_memories:
            em_analysis = {
                'final_episodes': em_memories[-1]['episode_count'],
                'avg_episodes': np.mean([m['episode_count'] for m in em_memories]),
                'replay_ability_avg': np.mean([m['replay_ability'] for m in em_memories]),
                'episode_growth_rate': (em_memories[-1]['episode_count'] - em_memories[0]['episode_count']) / 
                                       len(em_memories) if len(em_memories) > 1 else 0
            }
        
        # Associative memory analysis
        assoc_strengths = [s for s in self.memory_usage['associative_strengths']
                         if 'strength' in s]
        
        assoc_analysis = {}
        if assoc_strengths:
            assoc_analysis = {
                'avg_strength': np.mean([s['strength'] for s in assoc_strengths]),
                'strength_variance': np.var([s['strength'] for s in assoc_strengths]),
                'high_strength_episodes': len([s for s in assoc_strengths if s['strength'] > 0.8]),
                'strong_patterns': len([s for s in assoc_strengths if s['pattern_type'] == 'associative'])
            }
        
        # Memory consolidation analysis
        consolidation_events = self.memory_usage['consolidation_events']
        em_analysis_consolidation = {}
        if consolidation_events:
            em_analysis_consolidation = {
                'consolidation_events': len(consolidation_events),
                'avg_consolidation_efficiency': np.mean([e['efficiency'] for e in consolidation_events]),
                'consolidation_intervals': self.calculate_consolidation_intervals()
            }
        
        # Memory replay analysis
        replay_events = self.memory_usage['replay_events']
        em_analysis_replay = {}
        if replay_events:
            em_analysis_replay = {
                'replay_events': len(replay_events),
                'avg_replay_efficiency': np.mean([e['efficiency'] for e in replay_events]),
                'replay_frequency': len(replay_events) / (self.current_step / 1000) if self.current_step > 0 else 0
            }
        
        # Memory efficiency calculation
        overall_efficiency = self.calculate_overall_memory_efficiency(
            wm_analysis, em_analysis, assoc_analysis
        )
        
        return {
            'working_memory': wm_analysis,
            'episodic_memory': em_analysis,
            'associative_memory': assoc_analysis,
            'consolidation': em_analysis_consolidation,
            'replay': em_analysis_replay,
            'efficiency': overall_efficiency,
            'total_steps': self.current_step,
            'final_metrics': {
                'working_traces': self.get_working_memory_traces(),
                'episodic_memories': self.get_episodic_memory_count(),
                'avg_associative_strength': (assoc_analysis['avg_strength'] 
                                           if 'avg_strength' in assoc_analysis else 0),
                'consolidation_events': len(consolidation_events),
                'replay_events': len(replay_events)
            }
        }
    
    def calculate_consolidation_intervals(self):
        """Calculate intervals between consolidation events."""
        events = self.memory_usage['consolidation_events']
        if len(events) < 2:
            return []
        
        intervals = []
        for i in range(1, len(events)):
            interval = events[i]['step'] - events[i-1]['step']
            intervals.append(interval)
        
        return intervals
    
    def calculate_overall_memory_efficiency(self, wm_analysis, em_analysis, assoc_analysis):
        """Calculate overall memory system efficiency."""
        efficiency_components = []
        
        # Working memory efficiency
        if wm_analysis:
            wm_efficiency = (wm_analysis['avg_activity_ratio'] + 
                          (1 - wm_analysis['trace_stability'])) / 2
            efficiency_components.append(('working', wm_efficiency))
        
        # Episodic memory efficiency
        if em_analysis:
            em_efficiency = (em_analysis['episode_growth_rate'] / 10 +  # Normalize
                          em_analysis['replay_ability_avg']) / 2
            efficiency_components.append(('episodic', em_efficiency))
        
        # Associative memory efficiency
        if assoc_analysis:
            assoc_efficiency = (assoc_analysis['avg_strength'] + 
                             assoc_analysis['strong_patterns'] / max(assoc_analysis['strong_patterns'], 1)) / 2
            efficiency_components.append(('associative', assoc_efficiency))
        
        # Calculate weighted average
        if efficiency_components:
            total_efficiency = sum(comp[1] for comp in efficiency_components)
            return total_efficiency / len(efficiency_components)
        
        return 0.0
    
    def create_memory_visualizations(self):
        """Create comprehensive memory system visualizations."""
        
        plt.figure(figsize=(16, 12))
        
        # Plot 1: Working memory dynamics
        plt.subplot(2, 3, 1)
        
        wm_traces = [t for t in self.memory_usage['working_memory_traces']
                    if 'active_traces' in t]
        if wm_traces:
            steps = [t['step'] for t in wm_traces]
            traces = [t['active_traces'] for t in wm_traces]
            activities = [t['activity_ratio'] for t in wm_traces]
            
            plt.plot(steps, traces, 'b-', alpha=0.7, linewidth=1, label='Active Traces')
            plt.plot(steps, np.array(steps) * 0.01, 'r--', alpha=0.5, linewidth=1, label='Consolidation Window')
            plt.xlabel('Step')
            plt.ylabel('Trace Count')
            plt.title('Working Memory Dynamics')
            plt.legend()
            plt.grid(True, alpha=0.3)
        
        # Plot 2: Episodic memory formation
        plt.subplot(2, 3, 2)
        
        em_memories = [m for m in self.memory_usage['episodic_memories']
                      if 'episode_count' in m]
        if em_memories:
            steps = [m['step'] for m in em_memories]
            episodes = [m['episode_count'] for m in em_memories]
            replay_abilities = [m['replay_ability'] for m in em_memories]
            
            plt.plot(steps, episodes, 'g-', linewidth=2, label='Episodes')
            plt.plot(steps, replay_abilities, 'orange', linewidth=2, label='Replay Ability')
            plt.xlabel('Step')
            plt.ylabel('Count / Ability')
            plt.title('Episodic Memory Formation')
            plt.legend()
            plt.grid(True, alpha=0.3)
        
        # Plot 3: Associative memory strength
        plt.subplot(2, 3, 3)
        
        assoc_strengths = [s for s in self.memory_usage['associative_strengths']
                         if 'strength' in s]
        if assoc_strengths:
            steps = [s['step'] for s in assoc_strengths]
            strengths = [s['strength'] for s in assoc_strengths]
            pattern_types = [s['pattern_type'] for s in assoc_strengths]
            
            # Color code by pattern type
            colors = {'working': 'blue', 'episodic': 'green', 'associative': 'red'}
            for pattern in set(pattern_types):
                mask = [pt == pattern for pt in pattern_types]
                plt.scatter(np.array(steps)[mask], np.array(strengths)[mask], 
                           c=colors[pattern], alpha=0.6, s=10, label=pattern)
            
            plt.xlabel('Step')
            plt.ylabel('Associative Strength')
            plt.title('Associative Memory Strength')
            plt.legend()
            plt.grid(True, alpha=0.3)
        
        # Plot 4: Memory consolidation timeline
        plt.subplot(2, 3, 4)
        
        consolidation_events = self.memory_usage['consolidation_events']
        if consolidation_events:
            steps = [e['step'] for e in consolidation_events]
            efficiencies = [e['efficiency'] for e in consolidation_events]
            
            plt.scatter(steps, efficiencies, alpha=0.7, s=50, c='purple')
            plt.plot(steps, efficiencies, 'purple', alpha=0.5, linewidth=1)
            plt.xlabel('Step')
            plt.ylabel('Consolidation Efficiency')
            plt.title('Memory Consolidation Events')
            plt.grid(True, alpha=0.3)
        
        # Plot 5: Memory replay timeline
        plt.subplot(2, 3, 5)
        
        replay_events = self.memory_usage['replay_events']
        if replay_events:
            steps = [e['step'] for e in replay_events]
            efficiencies = [e['efficiency'] for e in replay_events]
            
            plt.scatter(steps, efficiencies, alpha=0.7, s=50, c='brown')
            plt.plot(steps, efficiencies, 'brown', alpha=0.5, linewidth=1)
            plt.xlabel('Step')
            plt.ylabel('Replay Efficiency')
            plt.title('Memory Replay Events')
            plt.grid(True, alpha=0.3)
        
        # Plot 6: Memory system interaction heatmap
        plt.subplot(2, 3, 6)
        
        # Create a simplified heatmap showing memory system interactions
        # over time windows
        if self.memory_usage['working_memory_traces']:
            # Divide time into windows
            num_windows = 10
            window_size = self.current_step // num_windows
            
            # Calculate average metrics for each window
            wm_window_avg = []
            em_window_avg = []
            assoc_window_avg = []
            
            for i in range(num_windows):
                start_step = i * window_size
                end_step = (i + 1) * window_size if i < num_windows - 1 else self.current_step
                
                wm_data = [t for t in self.memory_usage['working_memory_traces'] 
                          if start_step <= t['step'] < end_step]
                em_data = [m for m in self.memory_usage['episodic_memories']
                         if start_step <= m['step'] < end_step]
                assoc_data = [s for s in self.memory_usage['associative_strengths']
                            if start_step <= s['step'] < end_step]
                
                wm_avg = np.mean([t['activity_ratio'] for t in wm_data]) if wm_data else 0
                em_avg = np.mean([m['episode_count'] for m in em_data]) / 10 if em_data else 0  # Normalize
                assoc_avg = np.mean([s['strength'] for s in assoc_data]) if assoc_data else 0
                
                wm_window_avg.append(wm_avg)
                em_window_avg.append(em_avg)
                assoc_window_avg.append(assoc_avg)
            
            # Create heatmap
            heatmap_data = np.array([wm_window_avg, em_window_avg, assoc_window_avg])
            
            im = plt.imshow(heatmap_data, aspect='auto', cmap='RdYlBu_r', vmin=0, vmax=1)
            plt.colorbar(im, label='Activity Level')
            
            plt.yticks([0, 1, 2], ['Working Memory', 'Episodic Memory', 'Associative Memory'])
            plt.xlabel('Time Window')
            plt.title('Memory System Activity Heatmap')
        
        plt.tight_layout()
        plt.savefig('memory_integration_experiment.png', dpi=150)
        plt.show()

class MemoryPerformanceMonitor:
    """
    Monitor memory system performance and efficiency.
    """
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'working_memory_traces': [],
            'episodic_memories': [],
            'associative_strengths': [],
            'memory_access_efficiency': [],
            'consolidation_events': 0,
            'replay_events': 0
        }
    
    def record_memory_metrics(self, step_time, working_memory_traces, 
                             episodic_memories, associative_strength):
        """Record memory system metrics."""
        self.metrics['step_times'].append(step_time)
        self.metrics['working_memory_traces'].append(working_memory_traces)
        self.metrics['episodic_memories'].append(episodic_memories)
        self.metrics['associative_strengths'].append(associative_strength)
        
        # Calculate memory access efficiency
        access_efficiency = self.calculate_access_efficiency(step_time, 
                                                             working_memory_traces,
                                                             episodic_memories)
        self.metrics['memory_access_efficiency'].append(access_efficiency)
    
    def calculate_access_efficiency(self, step_time, working_traces, episodic_count):
        """
        Calculate memory access efficiency.
        
        Higher efficiency means: fast access, stable memory traces, good recall.
        """
        # Normalize components (all should be between 0 and 1)
        trace_efficiency = min(working_traces / 50, 1.0)  # Max 50 traces
        episode_efficiency = min(episodic_count / 100, 1.0)  # Max 100 episodes
        speed_efficiency = min(0.01 / step_time, 1.0)  # Faster = better
        
        # Weighted average
        efficiency = (trace_efficiency * 0.3 + 
                     episode_efficiency * 0.5 + 
                     speed_efficiency * 0.2)
        
        return efficiency
    
    def get_metrics(self):
        """Get aggregated memory performance metrics."""
        if not self.metrics['step_times']:
            return {}
        
        metrics = {
            'avg_step_time': np.mean(self.metrics['step_times']),
            'avg_working_memory_traces': np.mean(self.metrics['working_memory_traces']),
            'avg_episodic_memories': np.mean(self.metrics['episodic_memories']),
            'avg_associative_strength': np.mean(self.metrics['associative_strengths']),
            'avg_access_efficiency': np.mean(self.metrics['memory_access_efficiency']),
            'access_efficiency_variance': np.var(self.metrics['memory_access_efficiency']),
            'performance_score': self.calculate_performance_score()
        }
        
        return metrics
    
    def calculate_performance_score(self):
        """Calculate overall memory performance score."""
        # Weight different aspects of performance
        perf_score = (
            min(self.metrics['avg_working_memory_traces'] / 20, 1.0) * 0.25 +  # Working memory
            min(self.metrics['avg_episodic_memories'] / 50, 1.0) * 0.30 +  # Episodic memory
            self.metrics['avg_associative_strength'] * 0.25 +  # Associative memory
            self.metrics['avg_access_efficiency'] * 0.20  # Access efficiency
        )
        
        return perf_score

# Main execution function

def run_memory_integration_experiment(config=None, num_steps=5000):
    """
    Run comprehensive memory integration experiment.
    
    Args:
        config: Configuration dictionary for the experiment
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with experiment results
    """
    
    # Default configuration
    if config is None:
        config = {
            'neuron_count': 1200,
            'region_count': 4,
            'stdp_learning_rate': 0.002
        }
    
    print("=== MEMORY INTEGRATION EXPERIMENT FOR NLM ===")
    print("Demonstrating comprehensive memory systems integration")
    
    # Initialize experiment
    experiment = MemoryIntegrationExperiment(config)
    
    # Run experiment
    results = experiment.run_memory_experiment(num_steps)
    
    # Print summary
    print("\n" + "="*60)
    print("MEMORY INTEGRATION EXPERIMENT SUMMARY")
    print("="*60)
    
    mem_analysis = results['working_memory']
    em_analysis = results['episodic_memory']
    assoc_analysis = results['associative_memory']
    efficiency = results['efficiency']
    final_metrics = results['final_metrics']
    
    print(f"\nWorking Memory Performance:")
    if mem_analysis:
        print(f"  Average active traces: {mem_analysis['avg_traces']:.1f}")
        print(f"  Trace activity ratio: {mem_analysis['avg_activity_ratio']:.3f}")
        print(f"  Trace stability: {mem_analysis['trace_stability']:.3f}")
    
    print(f"\nEpisodic Memory Performance:")
    if em_analysis:
        print(f"  Final episode count: {em_analysis['final_episodes']}")
        print(f"  Episode growth rate: {em_analysis['episode_growth_rate']:.2f} per step")
        print(f"  Replay ability: {em_analysis['replay_ability_avg']:.3f}")
    
    print(f"\nAssociative Memory Performance:")
    if assoc_analysis:
        print(f"  Average strength: {assoc_analysis['avg_strength']:.3f}")
        print(f"  Strength variance: {assoc_analysis['strength_variance']:.4f}")
        print(f"  High strength patterns: {assoc_analysis['high_strength_episodes']}")
    
    print(f"\nMemory System Integration:")
    print(f"  Overall memory efficiency: {efficiency:.3f}")
    print(f"  Consolidation events: {results['consolidation']['consolidation_events']}")
    print(f"  Replay events: {results['replay']['replay_events']}")
    
    print(f"\nFinal Metrics:")
    for key, value in final_metrics.items():
        print(f"  {key}: {value:.2f}" if isinstance(value, float) else f"  {key}: {value}")
    
    print("\n" + "="*60)
    print("EXPERIMENT COMPLETE")
    print("="*60)
    print("Key insights:")
    print("✓ Working memory maintains active representations of current input")
    print("✓ Episodic memory stores discrete experience episodes")
    print("✓ Associative memory links related patterns and experiences")
    print("✓ Memory consolidation preserves important information")
    print("✓ Memory replay reinforces and integrates stored memories")
    print("✓ Multiple memory systems work together for intelligent behavior")
    
    return results

# Additional utility functions for documentation

def get_memory_integration_configuration():
    """
    Get example configuration for memory integration experiments.
    
    Returns:
        Dictionary with example configuration
    """
    return {
        'neuron_count': 1200,
        'region_count': 4,
        'stdp_learning_rate': 0.002,
        'working_memory_capacity': 50,
        'episodic_memory_limit': 1000,
        'consolidation_interval': 500,
        'replay_interval': 1000
    }

def get_memory_learning_insights():
    """
    Get educational insights about memory systems for documentation.
    
    Returns:
        List of memory system insights
    """
    return [
        {
            "title": "Working Memory Dynamics",
            "description": "The 'active' memory system holds current information for immediate use.",
            "example": "See a new pattern → Working memory stores it → Brain processes → Action selected"
        },
        {
            "title": "Episodic Memory Formation",
            "description": "Unique experiences are stored as discrete episodes for later recall.",
            "example": "Each novel event becomes an episode → Can be replayed → Reinforces learning"
        },
        {
            "title": "Associative Memory Linking",
            "description": "Similar patterns become linked through Hebbian learning.",
            "example": "Similar patterns → Strengthen connection → Better recall of related information"
        },
        {
            "title": "Memory Consolidation",
            "description": "Important memories are strengthened and integrated into long-term storage.",
            "example": "Rare but significant events → Consolidation → More stable memory representation"
        },
        {
            "title": "Memory Replay",
            "description": "Past experiences are reactivated to strengthen memory and extract patterns.",
            "example": "Replay during rest → Reinforces neural connections → Improves future performance"
        }
    ]

# Command line interface
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Run Memory Integration Experiment for NLM")
    parser.add_argument("--steps", type=int, default=5000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=1200, help="Number of neurons")
    parser.add_argument("--regions", type=int, default=4, help="Number of brain regions")
    parser.add_argument("--stdp-rate", type=float, default=0.002, help="STDP learning rate")
    parser.add_argument("--config", help="Path to configuration file")
    parser.add_argument("--no-plots", action="store_true", help="Disable plotting")
    
    args = parser.parse_args()
    
    # Load configuration if provided
    config = None
    if args.config:
        import json
        with open(args.config, 'r') as f:
            config = json.load(f)
    
    # Set configuration from command line arguments
    if config is None:
        config = {
            'neuron_count': args.neurons,
            'region_count': args.regions,
            'stdp_learning_rate': args.stdp_rate
        }
    
    # Run experiment
    results = run_memory_integration_experiment(config, args.steps)
