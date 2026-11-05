#!/usr/bin/env python3
"""
Analysis example: Brain performance analysis and statistics

This example demonstrates advanced analysis of brain performance,
including statistics collection, pattern analysis, and benchmarking.
"""

import time
import statistics
import random
from nlm import (
    Brain, Config, SimpleWorld, Vision, Audio, InternalSignals,
    SensoryPercept, Action, ActionType, Reward,
    create_default_config, create_brain, create_simple_world,
    create_agent_brain, get_brain_statistics, SimulationLoop,
    ActionSequence, format_duration, benchmark_function
)

def create_synthetic_sensory(percept: SensoryPercept, pattern: str = "normal"):
    """Create synthetic sensory data with different patterns for testing"""
    
    if pattern == "high_activity":
        # High activity pattern - lots of signals
        vision_data = [i * 0.1 for i in range(320 * 240 * 3)]
        audio_data = [j * 0.05 for j in range(1000)]
        touch_signals = [random.uniform(0.7, 1.0) for _ in range(10)]
        internal_signals = [random.uniform(0.5, 1.0) for _ in range(5)]
        proprioception_signals = [random.uniform(0.3, 0.8) for _ in range(3)]
    
    elif pattern == "low_activity":
        # Low activity pattern - minimal signals
        vision_data = [random.uniform(0, 0.1) for _ in range(320 * 240 * 3)]
        audio_data = [random.uniform(-0.1, 0.1) for _ in range(1000)]
        touch_signals = [random.uniform(0, 0.3) for _ in range(10)]
        internal_signals = [random.uniform(-0.1, 0.1) for _ in range(5)]
        proprioception_signals = [random.uniform(-0.2, 0.2) for _ in range(3)]
    
    else:  # normal pattern
        vision_data = [random.uniform(0, 1) for _ in range(320 * 240 * 3)]
        audio_data = [random.uniform(-1, 1) for _ in range(1000)]
        touch_signals = [random.uniform(0, 0.5) for _ in range(10)]
        internal_signals = [random.uniform(-0.3, 0.3) for _ in range(5)]
        proprioception_signals = [random.uniform(-0.5, 0.5) for _ in range(3)]
    
    # Set up vision
    vision = Vision()
    vision.setData(vision_data)
    
    # Set up audio
    audio = Audio(sample_rate=44100)
    audio.setData(audio_data)
    
    # Set up touch
    touch = InternalSignals()
    for signal in touch_signals:
        touch.addSignal(signal)
    
    # Set up internal
    internal = InternalSignals()
    for signal in internal_signals:
        internal.addSignal(signal)
    
    # Set up proprioception
    proprioception = InternalSignals()
    for signal in proprioception_signals:
        proprioception.addSignal(signal)
    
    # Combine into percept
    percept.setVision(vision)
    percept.setTouch(touch)
    percept.setInternal(internal)
    percept.setProprioception(proprioception)
    percept.setAudio(audio)

class AnalysisAgent:
    """Agent that records data for analysis"""
    
    def __init__(self, agent_brain, world):
        self.agent_brain = agent_brain
        self.world = world
        self.action_sequence = ActionSequence()
        self.brain_stats_history = []
        self.performance_metrics = []
    
    def run_step(self) -> Action:
        """Run one step and record data"""
        # Get current brain stats
        stats = get_brain_statistics(self.agent_brain.getBrain())
        self.brain_stats_history.append({
            'step': len(self.action_sequence.actions),
            'timestamp': time.time(),
            'statistics': stats
        })
        
        # Create synthetic sensory data
        percept = SensoryPercept()
        pattern = "high_activity" if len(self.action_sequence.actions) % 10 == 0 else "normal"
        create_synthetic_sensory(percept, pattern)
        
        # Process sensory input
        self.agent_brain.processSensoryInput(percept)
        
        # Get action
        action = self.agent_brain.decode_motor_command()
        self.action_sequence.add_action(action)
        
        # Apply action
        self.world.applyMotorCommand(action, len(self.action_sequence.actions))
        
        return action
    
    def get_action_patterns(self) -> dict:
        """Get analysis of action patterns"""
        return self.action_sequence.analyze_patterns()
    
    def get_brain_evolution(self) -> dict:
        """Get how brain statistics evolved over time"""
        if not self.brain_stats_history:
            return {}
        
        evolution = {
            'step_range': (self.brain_stats_history[0]['step'], self.brain_stats_history[-1]['step']),
            'neuron_count': {
                'initial': self.brain_stats_history[0]['statistics']['neuron_count']['total'],
                'final': self.brain_stats_history[-1]['statistics']['neuron_count']['total'],
                'change': self.brain_stats_history[-1]['statistics']['neuron_count']['total'] - 
                         self.brain_stats_history[0]['statistics']['neuron_count']['total']
            },
            'synapse_count': {
                'initial': self.brain_stats_history[0]['statistics']['synapse_count']['total'],
                'final': self.brain_stats_history[-1]['statistics']['synapse_count']['total'],
                'change': self.brain_stats_history[-1]['statistics']['synapse_count']['total'] - 
                         self.brain_stats_history[0]['statistics']['synapse_count']['total']
            },
            'firing_rate': {
                'initial': self.brain_stats_history[0]['statistics']['activity']['average_firing_rate'],
                'final': self.brain_stats_history[-1]['statistics']['activity']['average_firing_rate'],
                'change': self.brain_stats_history[-1]['statistics']['activity']['average_firing_rate'] - 
                         self.brain_stats_history[0]['statistics']['activity']['average_firing_rate']
            }
        }
        
        return evolution

def demonstrate_basic_analysis():
    """Demonstrate basic analysis of brain simulation"""
    print("=== Basic Analysis Example ===\n")
    
    # Create simulation
    config = create_default_config()
    brain = create_brain(config)
    world = create_simple_world()
    agent_brain = create_agent_brain(brain, world)
    
    # Create analysis agent
    analyzer = AnalysisAgent(agent_brain, world)
    
    # Configure and initialize
    world.configure(640, 480, 320, 240)
    brain.initialize()
    agent_brain.initialize(world)
    
    # Run simulation
    print("Running simulation for 15 steps...")
    for step in range(15):
        action = analyzer.run_step()
        print(f"  Step {step}: {action.getName()}")
    
    # Analyze results
    print("\n=== Analysis Results ===\n")
    
    # Action patterns
    print("1. Action Pattern Analysis:")
    patterns = analyzer.get_action_patterns()
    print(f"   Total actions: {patterns['total_actions']}")
    print(f"   Unique actions: {patterns['unique_actions']}")
    print(f"   Action variance: {patterns['action_variance']}")
    if patterns['most_common_action']:
        print(f"   Most common action: {patterns['most_common_action']} ({patterns['most_common_count']} times)")
    
    # Brain evolution
    print("\n2. Brain Evolution:")
    evolution = analyzer.get_brain_evolution()
    if evolution:
        print(f"   Neuron count: {evolution['neuron_count']['initial']} → {evolution['neuron_count']['final']} "
              f"({evolution['neuron_count']['change']:+d})")
        print(f"   Synapse count: {evolution['synapse_count']['initial']} → {evolution['synapse_count']['final']} "
              f"({evolution['synapse_count']['change']:+d})")
        print(f"   Firing rate: {evolution['firing_rate']['initial']:.3f} → {evolution['firing_rate']['final']:.3f} "
              f"({evolution['firing_rate']['change']:+.3f})")
    
    # Performance metrics
    print("\n3. Performance Metrics:")
    simulation_loop = SimulationLoop(brain, world, agent_brain)
    simulation_loop.run_episode(15)
    
    stats = simulation_loop.get_performance_stats()
    print(f"   Steps per second: {stats['steps_per_second']:.1f}")
    print(f"   Total time: {format_duration(stats['total_time'])}")
    print(f"   Unique actions: {stats['unique_actions']}")
    
    print("\n=== Basic Analysis Complete ===")

def demonstrate_performance_benchmarking():
    """Demonstrate performance benchmarking"""
    print("\n\n=== Performance Benchmarking Example ===\n")
    
    # Create simulation
    config = create_default_config()
    brain = create_brain(config)
    world = create_simple_world()
    agent_brain = create_agent_brain(brain, world)
    
    world.configure(640, 480, 320, 240)
    brain.initialize()
    agent_brain.initialize(world)
    
    # Benchmark simulation function
    def run_simulation(brain, world, steps=20):
        """Function to benchmark"""
        loop = SimulationLoop(brain, world, agent_brain)
        return loop.run_episode(steps)
    
    print("1. Benchmarking simulation performance...")
    
    # Run benchmark
    benchmark_result = benchmark_function(run_simulation, brain, world, 20)
    
    print("2. Benchmark Results:")
    print(f"   Duration: {format_duration(benchmark_result['duration'])}")
    print(f"   Actions per second: {benchmark_result['duration'] / 20:.1f}")
    
    # Compare with different step counts
    print("\n3. Performance Comparison:")
    step_counts = [5, 10, 15, 20]
    
    for steps in step_counts:
        result = benchmark_function(run_simulation, brain, world, steps)
        print(f"   {steps:2d} steps: {format_duration(result['duration'])} "
              f"({steps/result['duration']:.1f} steps/sec)")
    
    print("\n=== Performance Benchmarking Complete ===")

def demonstrate_comprehensive_analysis():
    """Demonstrate comprehensive analysis with multiple scenarios"""
    print("\n\n=== Comprehensive Analysis Example ===\n")
    
    # Run multiple scenarios with different configurations
    scenarios = [
        {'name': 'Normal Activity', 'pattern': 'normal', 'steps': 10},
        {'name': 'High Activity', 'pattern': 'high_activity', 'steps': 8},
        {'name': 'Low Activity', 'pattern': 'low_activity', 'steps': 12},
    ]
    
    results = []
    
    for scenario in scenarios:
        print(f"1. Running scenario: {scenario['name']}...")
        
        # Create fresh simulation
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        world.configure(640, 480, 320, 240)
        brain.initialize()
        agent_brain.initialize(world)
        
        # Create analyzer
        analyzer = AnalysisAgent(agent_brain, world)
        
        # Run simulation
        loop = SimulationLoop(brain, world, agent_brain)
        actions = loop.run_episode(scenario['steps'])
        
        # Collect results
        scenario_result = {
            'name': scenario['name'],
            'steps': scenario['steps'],
            'actions': len(actions),
            'action_patterns': analyzer.get_action_patterns(),
            'brain_evolution': analyzer.get_brain_evolution(),
            'performance': loop.get_performance_stats(),
            'brain_stats': analyzer.brain_stats_history[-1]['statistics'] if analyzer.brain_stats_history else {}
        }
        
        results.append(scenario_result)
        
        print(f"   Completed: {scenario['steps']} steps")
        print(f"   Performance: {scenario_result['performance']['steps_per_second']:.1f} steps/sec")
    
    # Compare results
    print("\n=== Comparative Analysis ===")
    
    print("\n2. Scenario Comparison:")
    print(f"{'Scenario':<20} {'Steps':<6} {'Actions':<8} {'Perf (steps/sec)':<15} {'Neurons':<10} {'Synapses':<10}")
    print("-" * 70)
    
    for result in results:
        brain_stats = result['brain_stats']
        neuron_count = brain_stats.get('neuron_count', {}).get('total', 0) if brain_stats else 0
        synapse_count = brain_stats.get('synapse_count', {}).get('total', 0) if brain_stats else 0
        print(f"{result['name']:<20} {result['steps']:<6} {result['actions']:<8} "
              f"{result['performance']['steps_per_second']:<15.1f} {neuron_count:<10} {synapse_count:<10}")
    
    # Identify best performing scenario
    best_scenario = max(results, key=lambda x: x['performance']['steps_per_second'])
    print(f"\n3. Best Performance: {best_scenario['name']} "
          f"({best_scenario['performance']['steps_per_second']:.1f} steps/sec)")
    
    print("\n=== Comprehensive Analysis Complete ===")

if __name__ == "__main__":
    import random
    
    try:
        demonstrate_basic_analysis()
        demonstrate_performance_benchmarking()
        demonstrate_comprehensive_analysis()
        
        print("\n\n" + "="*60)
        print("ALL ANALYSIS EXAMPLES COMPLETED SUCCESSFULLY!")
        print("="*60)
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
