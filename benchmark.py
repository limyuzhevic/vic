# NLM Performance Benchmark Suite

## Overview

This benchmark suite provides comprehensive performance testing for the NLM (Neural Learning Machine) framework. It measures execution time, memory usage, and computational efficiency across different components and configurations.

## Benchmark Categories

### 1. Core Neural Computation
- **Neuron Simulation**: LIF neuron dynamics
- **Synapse Propagation**: Spike transmission and delays
- **Plasticity Updates**: STDP and Hebbian learning
- **Brain Integration**: Complete brain simulation

### 2. Memory and Learning Systems
- **Working Memory**: Neural working memory operations
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations
- **Neuromodulation**: Learning signal processing

### 3. Cognitive Systems
- **Concept Formation**: Pattern abstraction
- **Predictive Planning**: Action sequence evaluation
- **Attention Mechanisms**: Competitive selection
- **Working Memory**: Active maintenance

### 4. Development and Adaptation
- **Structural Plasticity**: Synaptogenesis and pruning
- **Developmental Maturation**: Age-dependent changes
- **Checkpoint Processing**: Save/load operations
- **Replay Mechanisms**: Memory consolidation

### 5. Integration and Environment
- **World Simulation**: Physical and social environments
- **Sensory Processing**: Multi-modal input handling
- **Motor Output**: Action generation and execution
- **Agent Control**: Complete agent-brain interface

## Benchmark Configuration

### Standard Benchmark Configuration

```python
import pynlm
import time
import statistics
from typing import Dict, Any, List
import psutil
import os

class NLMBenchmarkSuite:
    """
    Comprehensive benchmark suite for NLM performance testing.
    Measures execution time, memory usage, and computational efficiency.
    """
    
    def __init__(self):
        self.process = psutil.Process(os.getpid())
        self.results = {}
        self.baseline_memory = self.process.memory_info().rss / 1024 / 1024  # MB
        
    def measure_memory_usage(self) -> float:
        """Measure current memory usage in MB."""
        return self.process.memory_info().rss / 1024 / 1024
        
    def measure_time(self, func, *args, **kwargs) -> tuple:
        """Measure execution time of a function."""
        start_time = time.perf_counter()
        result = func(*args, **kwargs)
        end_time = time.perf_counter()
        return result, end_time - start_time
        
    def benchmark_neuron_dynamics(self, num_neurons: int = 100, 
                                 num_steps: int = 1000) -> Dict[str, Any]:
        """
        Benchmark LIF neuron dynamics and spike generation.
        
        Args:
            num_neurons: Number of neurons to simulate
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with performance metrics
        """
        print(f"Benchmarking neuron dynamics: {num_neurons} neurons, {num_steps} steps")
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", num_neurons)
        config.set("random_seed", 42)
        config.set("simulation_timestep", 0.001)
        
        brain = pynlm.createBrain(config)
        initial_memory = self.measure_memory_usage()
        
        # Warm-up run
        for i in range(10):
            brain.step(i)
            
        # Benchmark run
        step_times = []
        spike_counts = []
        firing_rates = []
        
        for step in range(num_steps):
            step_start = time.perf_counter()
            brain.step(step)
            step_time = time.perf_counter() - step_start
            
            step_times.append(step_time)
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
            
            if step % 100 == 0:
                current_memory = self.measure_memory_usage()
                print(f"  Step {step}: {step_time*1000:.2f} ms, "
                      f"Memory: {current_memory-initial_memory:.1f} MB increase")
        
        final_memory = self.measure_memory_usage()
        
        results = {
            'neurons': num_neurons,
            'steps': num_steps,
            'total_time': statistics.mean(step_times) * num_steps,
            'avg_step_time': statistics.mean(step_times),
            'min_step_time': min(step_times),
            'max_step_time': max(step_times),
            'step_time_std': statistics.stdev(step_times) if len(step_times) > 1 else 0,
            'steps_per_second': num_steps / (statistics.mean(step_times) * num_steps),
            'total_spikes': spike_counts[-1],
            'avg_spikes_per_step': statistics.mean(spike_counts),
            'firing_rate_mean': statistics.mean(firing_rates),
            'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
            'memory_increase_mb': final_memory - initial_memory,
            'neurons_per_mb': num_neurons / (final_memory - initial_memory) if (final_memory - initial_memory) > 0 else 0,
            'spikes_per_second_per_neuron': spike_counts[-1] / (statistics.mean(step_times) * num_steps) / num_neurons
        }
        
        print(f"  Results: {results['steps_per_second']:.1f} steps/sec, "
              f"{results['total_spikes']} total spikes, "
              f"Memory: {results['memory_increase_mb']:.1f} MB increase\n")
        
        return results
        
    def benchmark_plasticity_systems(self, num_neurons: int = 200,
                                     num_steps: int = 500) -> Dict[str, Any]:
        """
        Benchmark plasticity learning systems (STDP, Hebbian, structural).
        
        Args:
            num_neurons: Number of neurons to simulate
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with plasticity performance metrics
        """
        print(f"Benchmarking plasticity systems: {num_neurons} neurons, {num_steps} steps")
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", num_neurons)
        config.set("random_seed", 123)
        config.set("simulation_timestep", 0.001)
        config.set("stdp_ltp_weight", 0.02)
        config.set("stdp_ltd_weight", 0.015)
        config.set("stdp_tau", 20.0)
        config.set("enable_structural_plasticity", True)
        config.set("enable_reward_modulation", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable plasticity on all synapses
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                for synapse in region->getSynapses():
                    synapse->enablePlasticity(True, True, True)
        
        step_times = []
        weight_changes = []
        plasticity_events = []
        
        for step in range(num_steps):
            step_start = time.perf_counter()
            
            # Generate correlated activity to drive plasticity
            if step % 10 == 0:  # Every 10 steps, inject correlated input
                # Create pre-post spike pairs to trigger STDP
                for i in range(5):
                    if i < num_neurons // 4:
                        brain->injectCurrentToNeurons(NeuronType::Excitatory, 30.0f)
            
            brain.step(step)
            
            step_time = time.perf_counter() - step_start
            step_times.append(step_time)
            
            # Measure weight changes
            total_weight_change = 0.0f
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    for synapse in region->getSynapses():
                        if step > 0:
                            weight_change = std::abs(synapse->getWeight() - synapse->getPreviousWeight())
                            total_weight_change += weight_change
                            synapse->setPreviousWeight(synapse->getWeight())
            
            weight_changes.append(total_weight_change)
            
            # Count plasticity events
            plasticity_events.append(brain->getTotalSpikeCount())
            
            if step % 100 == 0:
                print(f"  Step {step}: {step_time*1000:.2f} ms, "
                      f"Weight change: {total_weight_change:.6f}")
        
        results = {
            'neurons': num_neurons,
            'steps': num_steps,
            'total_time': statistics.mean(step_times) * num_steps,
            'avg_step_time': statistics.mean(step_times),
            'steps_per_second': num_steps / (statistics.mean(step_times) * num_steps),
            'total_weight_change': statistics.sum(weight_changes),
            'avg_weight_change_per_step': statistics.mean(weight_changes),
            'weight_change_std': statistics.stdev(weight_changes) if len(weight_changes) > 1 else 0,
            'plasticity_events': statistics.sum(plasticity_events),
            'avg_plasticity_events_per_step': statistics.mean(plasticity_events),
            'learning_efficiency': statistics.sum(weight_changes) / num_steps
        }
        
        print(f"  Results: {results['steps_per_second']:.1f} steps/sec, "
              f"Total weight change: {results['total_weight_change']:.6f}, "
              f"Learning efficiency: {results['learning_efficiency']:.9f}\n")
        
        return results
        
    def benchmark_memory_systems(self, num_neurons: int = 150,
                                num_steps: int = 800) -> Dict[str, Any]:
        """
        Benchmark memory encoding and retrieval systems.
        
        Args:
            num_neurons: Number of neurons to simulate
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with memory system performance metrics
        """
        print(f"Benchmarking memory systems: {num_neurons} neurons, {num_steps} steps")
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", num_neurons)
        config.set("random_seed", 456)
        config.set("simulation_timestep", 0.001)
        config.set("enable_reward_modulation", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        step_times = []
        memory_usage = []
        encoding_events = []
        retrieval_events = []
        
        for step in range(num_steps):
            step_start = time.perf_counter()
            
            # Generate input patterns to encode
            if step % 20 == 0:
                # Create memory patterns
                for pattern_idx in range(min(3, num_neurons // 50)):
                    # Inject current patterns representing memory items
                    activation_pattern = 20.0f * (1.0f - pattern_idx * 0.3f)
                    brain->injectCurrentToNeurons(NeuronType::Internal, activation_pattern)
            
            brain.step(step)
            
            step_time = time.perf_counter() - step_start
            step_times.append(step_time)
            
            # Measure memory system load
            current_memory = self.measure_memory_usage()
            memory_usage.append(current_memory)
            
            # Count encoding/retrieval events
            encoding_events.append(brain->getTotalSpikeCount())
            
            if step % 100 == 0:
                print(f"  Step {step}: {step_time*1000:.2f} ms, "
                      f"Memory: {current_memory:.1f} MB")
        
        memory_increase = memory_usage[-1] - memory_usage[0]
        
        results = {
            'neurons': num_neurons,
            'steps': num_steps,
            'total_time': statistics.mean(step_times) * num_steps,
            'avg_step_time': statistics.mean(step_times),
            'steps_per_second': num_steps / (statistics.mean(step_times) * num_steps),
            'memory_increase_mb': memory_increase,
            'memory_efficiency_mb_per_step': memory_increase / num_steps,
            'encoding_events': statistics.sum(encoding_events),
            'avg_encoding_events_per_step': statistics.mean(encoding_events),
            'memory_bandwidth_mbps': memory_increase * 8 / (statistics.mean(step_times) * num_steps)  # MB/s
        }
        
        print(f"  Results: {results['steps_per_second']:.1f} steps/sec, "
              f"Memory: {results['memory_increase_mb']:.1f} MB, "
              f"Bandwidth: {results['memory_bandwidth_mbps']:.1f} Mbps\n")
        
        return results
        
    def benchmark_agent_simulation(self, num_neurons: int = 300,
                                  world_size: tuple = (20, 20),
                                  num_steps: int = 1000) -> Dict[str, Any]:
        """
        Benchmark complete agent simulation with world interaction.
        
        Args:
            num_neurons: Number of neurons in brain
            world_size: Size of world (width, height)
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with agent simulation performance metrics
        """
        print(f"Benchmarking agent simulation: {num_neurons} neurons, "
              f"World {world_size}, {num_steps} steps")
        
        width, height = world_size
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", num_neurons)
        config.set("random_seed", 789)
        config.set("simulation_timestep", 0.1)  # Longer timestep for world simulation
        
        # Setup brain
        brain = pynlm.createBrain(config)
        
        # Setup world
        world = pynlm.createSimpleWorld()
        world.configure(width, height, visionWidth=8, visionHeight=8)
        world.setAgentStart(width//2, height//2)
        world.reset()
        
        # Setup agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        step_times = []
        world_updates = []
        agent_steps = []
        sensory_inputs = []
        motor_outputs = []
        
        for step in range(num_steps):
            step_start = time.perf_counter()
            
            # World update (physical simulation)
            world_update_start = time.perf_counter()
            world.update(0.1)
            world_update_time = time.perf_counter() - world_update_start
            world_updates.append(world_update_time)
            
            # Get sensory input
            sensory_start = time.perf_counter()
            percept = world.getSensoryPercept()
            sensory_time = time.perf_counter() - sensory_start
            sensory_inputs.append(sensory_time)
            
            # Agent processes sensory input
            agent_step_start = time.perf_counter()
            agent.processSensoryInput(percept)
            agent_step_time = time.perf_counter() - agent_step_start
            agent_steps.append(agent_step_time)
            
            # Brain computation
            brain_step_start = time.perf_counter()
            brain.step(step)
            brain_step_time = time.perf_counter() - brain_step_start
            
            # Decode motor command
            motor_start = time.perf_counter()
            action = agent.decodeMotorCommand()
            motor_time = time.perf_counter() - motor_start
            motor_outputs.append(motor_time)
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            total_step_time = time.perf_counter() - step_start
            step_times.append(total_step_time)
            
            if step % 200 == 0:
                print(f"  Step {step}: {total_step_time*1000:.2f} ms, "
                      f"Brain: {brain_step_time*1000:.2f} ms, "
                      f"Agent: {agent_step_time*1000:.2f} ms, "
                      f"World: {world_update_time*1000:.2f} ms")
        
        results = {
            'neurons': num_neurons,
            'world_size': world_size,
            'steps': num_steps,
            'total_time': statistics.mean(step_times) * num_steps,
            'avg_step_time': statistics.mean(step_times),
            'steps_per_second': num_steps / (statistics.mean(step_times) * num_steps),
            'world_update_time': statistics.mean(world_updates),
            'brain_step_time': statistics.mean([st - w - a - s for st, w, a, s in 
                                              zip(step_times, world_updates, agent_steps, sensory_inputs)]),
            'agent_step_time': statistics.mean(agent_steps),
            'sensory_processing_time': statistics.mean(sensory_inputs),
            'motor_output_time': statistics.mean(motor_outputs),
            'world_update_percentage': statistics.mean(world_updates) / statistics.mean(step_times) * 100,
            'brain_computation_percentage': statistics.mean([st - w - a - s for st, w, a, s in 
                                                           zip(step_times, world_updates, agent_steps, sensory_inputs)]) / statistics.mean(step_times) * 100,
            'agent_processing_percentage': statistics.mean(agent_steps) / statistics.mean(step_times) * 100,
            'sensory_processing_percentage': statistics.mean(sensory_inputs) / statistics.mean(step_times) * 100,
            'motor_output_percentage': statistics.mean(motor_outputs) / statistics.mean(step_times) * 100
        }
        
        print(f"  Results: {results['steps_per_second']:.1f} steps/sec, "
              f"World: {results['world_update_percentage']:.1f}%, "
              f"Brain: {results['brain_computation_percentage']:.1f}%, "
              f"Agent: {results['agent_processing_percentage']:.1f}%\n")
        
        return results
        
    def run_full_benchmark_suite(self) -> Dict[str, Dict[str, Any]]:
        """
        Run complete benchmark suite across all major NLM components.
        
        Returns:
            Dictionary with benchmark results for all categories
        """
        print("=== NLM Performance Benchmark Suite ===\n")
        
        results = {}
        
        # Core neural computation benchmarks
        results['neuron_dynamics'] = self.benchmark_neuron_dynamics()
        results['plasticity_systems'] = self.benchmark_plasticity_systems()
        results['memory_systems'] = self.benchmark_memory_systems()
        
        # Integration benchmarks
        results['agent_simulation'] = self.benchmark_agent_simulation()
        
        # Summary report
        self.print_benchmark_summary(results)
        
        return results
        
    def print_benchmark_summary(self, results: Dict[str, Dict[str, Any]]) -> None:
        """Print comprehensive benchmark summary."""
        print("=== NLM Benchmark Summary ===\n")
        
        print(f"{'Component':<20} {'Steps/sec':<12} {'Time/step':<12} {'Memory':<12} {'Key Metrics':<20}")
        print("-" * 80)
        
        # Neuron dynamics summary
        nd = results['neuron_dynamics']
        print(f"{'Neuron Dynamics':<20} {nd['steps_per_second']:<12.1f} "
              f"{nd['avg_step_time']*1000:<12.2f}ms "
              f"{nd['memory_increase_mb']:<12.1f}MB "
              f"{nd['spikes_per_second_per_neuron']:<20.3f} spikes/s/neuron")
        
        # Plasticity systems summary
        ps = results['plasticity_systems']
        print(f"{'Plasticity Systems':<20} {ps['steps_per_second']:<12.1f} "
              f"{ps['avg_step_time']*1000:<12.2f}ms "
              f"{'-':<12} "
              f"{ps['learning_efficiency']:<20.6f} weight_change/step")
        
        # Memory systems summary
        ms = results['memory_systems']
        print(f"{'Memory Systems':<20} {ms['steps_per_second']:<12.1f} "
              f"{ms['avg_step_time']*1000:<12.2f}ms "
              f"{ms['memory_increase_mb']:<12.1f}MB "
              f"{ms['memory_bandwidth_mbps']:<20.1f} Mbps")
        
        # Agent simulation summary
        asim = results['agent_simulation']
        print(f"{'Agent Simulation':<20} {asim['steps_per_second']:<12.1f} "
              f"{asim['avg_step_time']*1000:<12.2f}ms "
              f"{'-':<12} "
              f"Brain: {asim['brain_computation_percentage']:.0f}%, "
              f"Agent: {asim['agent_processing_percentage']:.0f}%")
        
        print("\n=== Performance Analysis ===")
        
        # Identify bottlenecks
        slowest_component = min([
            ('Neuron Dynamics', nd['avg_step_time']),
            ('Plasticity Systems', ps['avg_step_time']),
            ('Memory Systems', ms['avg_step_time']),
            ('Agent Simulation', asim['avg_step_time'])
        ], key=lambda x: x[1])
        
        print(f"Slowest component: {slowest_component[0]} ({slowest_component[1]*1000:.2f} ms per step)")
        
        # Memory efficiency
        most_memory_efficient = min([
            ('Neuron Dynamics', nd['neurons_per_mb']),
            ('Memory Systems', ms['memory_efficiency_mb_per_step'])
        ], key=lambda x: x[1], reverse=True)
        
        print(f"Most memory efficient: {most_memory_efficient[0]}")
        
        # Learning efficiency
        print(f"Learning efficiency: {ps['learning_efficiency']:.9f} weight changes per step")
        
        # System integration
        integration_efficiency = asim['brain_computation_percentage'] + asim['agent_processing_percentage']
        print(f"System integration efficiency: {integration_efficiency:.1f}% "
              f"(brain + agent processing)")
        
        print("\n=== Recommendations ===")
        
        if asim['brain_computation_percentage'] > 60:
            print("• Brain computation is the primary bottleneck. Consider:")
            print("  - Optimizing neural dynamics implementation")
            print("  - Using more efficient neuron models")
            print("  - Implementing vectorized operations")
        
        if ms['memory_efficiency_mb_per_step'] > 0.1:
            print("• Memory usage is high. Consider:")
            print("  - Using more compact data structures")
            print("  - Implementing memory pooling")
            print("  - Optimizing memory access patterns")
        
        if ps['learning_efficiency'] < 0.001:
            print("• Learning efficiency is low. Consider:")
            print("  - Adjusting plasticity parameters")
            print("  - Using more efficient STDP implementations")
            print("  - Optimizing synaptic weight updates")
        
        if integration_efficiency < 70:
            print("• System integration could be improved. Consider:")
            print("  - Reducing agent processing overhead")
            print("  - Optimizing world simulation")
            print("  - Streamlining data flow between components")

# Convenience functions for common benchmark scenarios

def quick_benchmark(config_name: str = "standard") -> Dict[str, Any]:
    """
    Run a quick benchmark with common configurations.
    
    Args:
        config_name: "standard", "high_performance", or "memory_constrained"
        
    Returns:
        Benchmark results
    """
    benchmark = NLMBenchmarkSuite()
    
    if config_name == "high_performance":
        return benchmark.benchmark_neuron_dynamics(num_neurons=200, num_steps=2000)
    elif config_name == "memory_constrained":
        return benchmark.benchmark_neuron_dynamics(num_neurons=50, num_steps=1000)
    else:  # standard
        return benchmark.benchmark_neuron_dynamics(num_neurons=100, num_steps=1000)

def comprehensive_benchmark() -> Dict[str, Dict[str, Any]]:
    """Run comprehensive benchmark across all major components."""
    benchmark = NLMBenchmarkSuite()
    return benchmark.run_full_benchmark_suite()

# Example usage
if __name__ == "__main__":
    print("NLM Performance Benchmark Suite")
    print("=" * 50)
    
    # Run comprehensive benchmark
    results = comprehensive_benchmark()
    
    print("\nBenchmark complete!")
    print("Results saved to benchmark_results.json")
    
    # Save results to file
    import json
    with open("benchmark_results.json", "w") as f:
        json.dump(results, f, indent=2, default=str)
    
    print("Results can be analyzed with:")
    print("  - Python's matplotlib for performance graphs")
    print("  - pandas for data analysis")
    print("  - scipy for statistical analysis")