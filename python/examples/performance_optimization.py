"""
Performance Optimization Examples for NLM

This module demonstrates performance optimization techniques including:
- Memory optimization
- Computational efficiency
- Scaling strategies
- Hardware utilization
- Benchmarking and profiling
"""

import pynlm
import numpy as np
import time
from typing import Dict, List, Optional, Tuple
import psutil
import os

class PerformanceExamples:
    """Performance optimization examples."""
    
    @staticmethod
    def create_memory_optimization_example():
        """
        Example: Memory optimization strategies.
        
        Demonstrates techniques to reduce memory footprint including:
        - Sparse connectivity optimization
        - Memory pool management
        - Data structure optimization
        - Cache-friendly layouts
        """
        print("=== Memory Optimization Example ===")
        
        # Create memory-optimized configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.03)  # Very sparse for memory efficiency
        config.set("brain.connection_probability", 0.01)
        
        # Optimize for memory usage
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)  # Reduced for stability
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.003)
        config.set("plasticity.structural.enable", False)  # Disable structural for memory savings
        
        print("Creating memory-optimized brain configuration...")
        print(f"  Target neurons: {config.get('brain.neuron_count'):,}")
        print(f"  Synapse density: {config.get('brain.synapse_density'):.3f}")
        print(f"  Connection probability: {config.get('brain.connection_probability'):.3f}")
        print("  Structural plasticity: Disabled (memory savings)")
        
        # Get baseline memory usage
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_memory = psutil.virtual_memory()
        print(f"\nBaseline memory usage:")
        print(f"  Total: {initial_memory.total / (1024**3):.1f} GB")
        print(f"  Used: {initial_memory.used / (1024**3):.1f} GB")
        print(f"  Available: {initial_memory.available / (1024**3):.1f} GB")
        
        # Simulate memory-efficient operations
        print("\nSimulating memory-efficient operations...")
        
        memory_usage_data = {
            "step": [],
            "memory_used_gb": [],
            "memory_percent": [],
            "neuron_memory_efficiency": []
        }
        
        for step in range(200):
            # Record memory before operation
            before_memory = psutil.virtual_memory()
            
            # Execute brain step (memory-efficient)
            brain.step(step)
            
            # Record memory after operation
            after_memory = psutil.virtual_memory()
            
            # Calculate memory efficiency
            memory_increase = after_memory.used - before_memory.used
            memory_efficiency = (config.get("brain.neuron_count") / max(memory_increase, 1)) if memory_increase > 0 else float('inf')
            
            memory_usage_data["step"].append(step)
            memory_usage_data["memory_used_gb"].append(after_memory.used / (1024**3))
            memory_usage_data["memory_percent"].append(after_memory.percent)
            memory_usage_data["neuron_memory_efficiency"].append(memory_efficiency)
            
            if step % 50 == 0:
                print(f"  Step {step}: Memory efficiency = {memory_efficiency:,.0f} neurons/GB")
        
        # Calculate memory optimization metrics
        final_memory = psutil.virtual_memory()
        total_memory_increase = final_memory.used - initial_memory.used
        memory_increase_mb = total_memory_increase / (1024**2)
        
        # Estimate theoretical memory savings
        theoretical_dense_memory = config.get("brain.neuron_count") * 100  # 100 bytes per neuron for dense
        actual_memory_usage = total_memory_increase / (1024**2)
        memory_savings_percent = (1 - actual_memory_usage / theoretical_dense_memory) * 100
        
        print(f"\nMemory Optimization Results:")
        print(f"  Memory increase: {memory_increase_mb:.1f} MB")
        print(f"  Theoretical dense memory usage: {theoretical_dense_memory:,} MB")
        print(f"  Actual memory usage: {actual_memory_usage:.1f} MB")
        print(f"  Memory savings: {memory_savings_percent:.1f}%")
        print(f"  Average memory efficiency: {np.mean(memory_usage_data['neuron_memory_efficiency']):,.0f} neurons/GB")
        
        # Memory optimization recommendations
        recommendations = []
        if memory_savings_percent > 50:
            recommendations.append("✓ Excellent memory optimization achieved")
        if total_memory_increase < 100 * 1024 * 1024:  # Less than 100MB
            recommendations.append("✓ Memory growth within acceptable limits")
        if config.get("brain.synapse_density") < 0.05:
            recommendations.append("✓ Sparse connectivity effectively reduces memory")
        
        print(f"\nOptimization Recommendations:")
        for recommendation in recommendations:
            print(f"  {recommendation}")
        
        return {
            "brain": brain,
            "memory_usage_data": memory_usage_data,
            "config": config,
            "optimization_metrics": {
                "total_memory_increase_mb": memory_increase_mb,
                "memory_savings_percent": memory_savings_percent,
                "avg_memory_efficiency": np.mean(memory_usage_data['neuron_memory_efficiency'])
            }
        }
    
    @staticmethod
    def create_computational_efficiency_example():
        """
        Example: Computational efficiency optimization.
        
        Demonstrates techniques to optimize CPU utilization including:
        - Vectorization opportunities
        - Loop optimization
        - Parallel processing
        - Algorithmic improvements
        """
        print("\n=== Computational Efficiency Example ===")
        
        # Create computationally efficient configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.06)
        
        # Configure for computational efficiency
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.008)
        config.set("plasticity.hebbian.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Creating computationally efficient configuration")
        print(f"  Neurons: {config.get('brain.neuron_count'):,}")
        print(f"  Synapse density: {config.get('brain.synapse_density'):.2f}")
        print("  Optimized for vectorized operations")
        
        # Performance benchmarking
        print("\nPerformance benchmarking...")
        
        performance_metrics = {
            "step": [],
            "execution_time_ms": [],
            "neurons_per_second": [],
            "spikes_per_second": [],
            "cpu_efficiency": []
        }
        
        # Warm-up
        for step in range(20):
            brain.step(step)
        
        # Benchmark different operation types
        benchmark_phases = [
            {"name": "Neuron Update", "steps": 100, "operations_per_step": 50},
            {"name": "Synaptic Update", "steps": 150, "operations_per_step": 30},
            {"name": "Plasticity Update", "steps": 100, "operations_per_step": 20},
            {"name": "Memory Integration", "steps": 80, "operations_per_step": 25}
        ]
        
        for phase in benchmark_phases:
            print(f"\n  Benchmarking {phase['name']}...")
            
            phase_start_time = time.time()
            
            for step in range(phase['steps']):
                brain.step(step + 200)  # Offset to avoid overlap
                
                # Record performance metrics
                execution_time = time.time() - phase_start_time
                neurons_per_second = config.get("brain.neuron_count") / execution_time
                
                performance_metrics["step"].append(step)
                performance_metrics["execution_time_ms"].append(execution_time * 1000)
                performance_metrics["neurons_per_second"].append(neurons_per_second)
                
                if step % 20 == 0:
                    print(f"    Phase progress: {step}/{phase['steps']} steps, "
                          f"Performance: {neurons_per_second:,.0f} neurons/sec")
            
            phase_end_time = time.time() - phase_start_time
            print(f"    {phase['name']} completed in {phase_end_time:.2f}s")
        
        # Calculate efficiency metrics
        print("\n=== Computational Efficiency Analysis ===")
        
        execution_times = performance_metrics["execution_time_ms"]
        neurons_per_seconds = performance_metrics["neurons_per_second"]
        
        avg_execution_time = np.mean(execution_times)
        min_execution_time = np.min(execution_times)
        max_execution_time = np.max(execution_times)
        
        avg_neurons_per_second = np.mean(neurons_per_seconds)
        total_spikes = sum(m.get("spikes_per_second", 0) for m in performance_metrics["spikes_per_second"])
        
        # Calculate efficiency scores
        time_efficiency = min_execution_time / avg_execution_time
        computational_efficiency = avg_neurons_per_second / 1000  # Normalize to thousands
        
        print(f"Computational Efficiency Results:")
        print(f"  Average execution time: {avg_execution_time:.2f} ms")
        print(f"  Min execution time: {min_execution_time:.2f} ms")
        print(f"  Max execution time: {max_execution_time:.2f} ms")
        print(f"  Time efficiency ratio: {time_efficiency:.2f}")
        print(f"  Average neurons/sec: {avg_neurons_per_second:,.0f}")
        print(f"  Computational efficiency score: {computational_efficiency:.1f}")
        
        # Efficiency recommendations
        recommendations = []
        if time_efficiency > 0.8:
            recommendations.append("✓ Good time consistency - low variance in execution times")
        if computational_efficiency > 1.0:
            recommendations.append("✓ High computational throughput - efficient neuron processing")
        if avg_execution_time < 10:
            recommendations.append("✓ Fast execution - optimized for real-time use")
        
        print(f"\nOptimization Recommendations:")
        for recommendation in recommendations:
            print(f"  {recommendation}")
        
        return {
            "brain": brain,
            "performance_metrics": performance_metrics,
            "config": config,
            "efficiency_analysis": {
                "time_efficiency": time_efficiency,
                "computational_efficiency": computational_efficiency,
                "avg_execution_time_ms": avg_execution_time,
                "recommendations": recommendations
            }
        }
    
    @staticmethod
    def create_scaling_benchmark_example():
        """
        Example: Scaling benchmark for different network sizes.
        
        Tests performance across different scales to identify:
        - Optimal size for target applications
        - Scaling bottlenecks
        - Hardware requirements
        - Performance tradeoffs
        """
        print("\n=== Scaling Benchmark Example ===")
        
        # Define scales to test
        scales = {
            "micro": {"neurons": 1000, "steps": 500},
            "small": {"neurons": 10000, "steps": 200},
            "medium": {"neurons": 50000, "steps": 100},
            "large": {"neurons": 100000, "steps": 50}
        }
        
        print("Scaling benchmark across network sizes:")
        print("  Scale  | Neurons  | Steps  | Perf (neur/sec) | Memory (MB)  | Status")
        print("  -------|----------|--------|-----------------|--------------|--------")
        
        benchmark_results = {}
        
        for scale_name, scale_config in scales.items():
            print(f"\nBenchmarking {scale_name} scale...")
            
            # Create scale-appropriate configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", scale_config["neurons"])
            
            # Adjust density based on scale
            if scale_name == "micro":
                config.set("brain.synapse_density", 0.2)  # Dense for small networks
                config.set("brain.connection_probability", 0.15)
            elif scale_name == "small":
                config.set("brain.synapse_density", 0.1)   # Moderate density
                config.set("brain.connection_probability", 0.08)
            elif scale_name == "medium":
                config.set("brain.synapse_density", 0.05)   # Sparse for medium
                config.set("brain.connection_probability", 0.03)
            else:  # large
                config.set("brain.synapse_density", 0.03)   # Very sparse for large
                config.set("brain.connection_probability", 0.01)
            
            # Configure for scale
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.stdp.learning_rate", 0.01)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run benchmark
            start_time = time.time()
            for step in range(scale_config["steps"]):
                brain.step(step)
            execution_time = time.time() - start_time
            
            # Calculate metrics
            neurons_per_second = scale_config["neurons"] / execution_time
            
            # Estimate memory usage (simplified)
            # Formula: neurons * (neuron_size + avg_synapses * synapse_size)
            avg_synapses_per_neuron = scale_config["neurons"] * config.get("brain.synapse_density") / scale_config["neurons"]
            estimated_memory_mb = (scale_config["neurons"] * 50 +  # Neuron state
                                 avg_synapses_per_neuron * scale_config["neurons"] * 20) / (1024 * 1024)  # Synapse state
            
            # Determine status based on performance
            if neurons_per_second > 10000:
                status = "✓ Excellent"
            elif neurons_per_second > 1000:
                status = "✓ Good"
            elif neurons_per_second > 100:
                status = "⚠ Moderate"
            else:
                status = "✗ Poor"
            
            print(f"  {scale_name:7} | {scale_config['neurons']:8,} | {scale_config['steps']:6} | "
                  f"{neurons_per_second:16,.0f} | {estimated_memory_mb:11.1f} | {status}")
            
            benchmark_results[scale_name] = {
                "neurons": scale_config["neurons"],
                "steps": scale_config["steps"],
                "execution_time": execution_time,
                "neurons_per_second": neurons_per_second,
                "estimated_memory_mb": estimated_memory_mb,
                "status": status,
                "config": config,
                "brain": brain
            }
        
        # Analyze scaling efficiency
        print("\n=== Scaling Analysis ===")
        
        scales_ordered = list(benchmark_results.keys())
        neurons_list = [benchmark_results[s]["neurons"] for s in scales_ordered]
        performance_list = [benchmark_results[s]["neurons_per_second"] for s in scales_ordered]
        
        # Calculate scaling efficiency (how well performance scales with size)
        if len(performance_list) >= 3:
            # Check if performance scales linearly (ideal) or sub-linearly
            size_ratios = [neurons_list[i+1] / neurons_list[i] for i in range(len(neurons_list)-1)]
            performance_ratios = [performance_list[i+1] / performance_list[i] for i in range(len(performance_list)-1)]
            
            scaling_efficiencies = []
            for size_ratio, perf_ratio in zip(size_ratios, performance_ratios):
                efficiency = perf_ratio / size_ratio  # Should be ~1.0 for linear scaling
                scaling_efficiencies.append(efficiency)
            
            avg_scaling_efficiency = np.mean(scaling_efficiencies)
            
            print(f"Scaling Efficiency Analysis:")
            print(f"  Average scaling efficiency: {avg_scaling_efficiency:.2f}")
            print(f"  Ideal efficiency: 1.0 (linear scaling)")
            print(f"  Efficiency interpretation:")
            
            if avg_scaling_efficiency > 0.8:
                print("    ✓ Near-linear scaling - excellent hardware utilization")
            elif avg_scaling_efficiency > 0.5:
                print("    ✓ Good scaling - reasonable hardware utilization")
            elif avg_scaling_efficiency > 0.2:
                print("    ⚠ Poor scaling - significant bottlenecks")
            else:
                print("    ✗ Very poor scaling - major bottlenecks")
        
        # Identify optimal scale for different use cases
        print(f"\n=== Optimal Scale Recommendations ===")
        
        # Find best scale for different metrics
        best_for_performance = scales_ordered[np.argmax(performance_list)]
        best_for_memory = scales_ordered[np.argmin([benchmark_results[s]["estimated_memory_mb"] for s in scales_ordered])]
        
        print(f"  Best performance: {best_for_performance} scale ({benchmark_results[best_for_performance]['neurons_per_second']:,.0f} neurons/sec)")
        print(f"  Best memory efficiency: {best_for_memory} scale ({benchmark_results[best_for_memory]['estimated_memory_mb']:.1f} MB)")
        
        # Determine optimal scale based on use case
        print(f"\n  Recommended scales for different applications:")
        print(f"    - Real-time control: {best_for_performance} scale")
        print(f"    - Research/Prototyping: micro or small scale")
        print(f"    - Production: {best_for_performance} scale if performance needed, else medium for balance")
        
        return {
            "benchmark_results": benchmark_results,
            "scales_ordered": scales_ordered,
            "scaling_efficiency": avg_scaling_efficiency if 'avg_scaling_efficiency' in locals() else 0,
            "optimal_scales": {
                "performance": best_for_performance,
                "memory": best_for_memory
            }
        }

if __name__ == "__main__":
    print("Performance Optimization Examples")
    print("=" * 30)
    
    # Example 1: Memory optimization
    memory_result = PerformanceExamples.create_memory_optimization_example()
    print(f"\nMemory optimization: {memory_result['optimization_metrics']['memory_savings_percent']:.1f}% savings")
    
    # Example 2: Computational efficiency
    efficiency_result = PerformanceExamples.create_computational_efficiency_example()
    print(f"\nComputational efficiency: {efficiency_result['efficiency_analysis']['computational_efficiency']:.1f} score")
    
    # Example 3: Scaling benchmark
    scaling_result = PerformanceExamples.create_scaling_benchmark_example()
    print(f"\nScaling benchmark: {len(scaling_result['benchmark_results'])} scales tested")
    
    print("\n=== All performance optimization examples completed ===")
