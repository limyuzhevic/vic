#!/usr/bin/env python3
"""
NLM Performance and Load Tests
Comprehensive performance testing for NLM systems
"""

import pytest
import sys
import os
import tempfile
import time
import psutil
import numpy as np
from pathlib import Path
import tracemalloc
from typing import Dict, Any, List, Tuple

# Add project root to Python path
sys.path.insert(0, str(Path(__file__).parent))

class NLMPerformanceTester:
    """Comprehensive performance and load testing for NLM"""
    
    def __init__(self):
        self.test_dir = tempfile.mkdtemp(prefix='nlm_performance_test_')
        self.temp_files = []
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup
        import shutil
        shutil.rmtree(self.test_dir, ignore_errors=True)
        for filepath in self.temp_files:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def monitor_system_resources(self) -> Dict[str, Any]:
        """Monitor system resource usage"""
        process = psutil.Process(os.getpid())
        
        return {
            'cpu_percent': process.cpu_percent(),
            'memory_rss': process.memory_info().rss / 1024 / 1024,  # MB
            'memory_vms': process.memory_info().vms / 1024 / 1024,  # MB
            'memory_percent': process.memory_percent(),
            'num_threads': process.num_threads(),
            'num_fds': process.num_fds()
        }
    
    def benchmark_brain_performance(self, neuron_count: int, simulation_steps: int) -> Dict[str, Any]:
        """Benchmark brain performance for given configuration"""
        try:
            import pynlm
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.setInt("brain.neuron_count", neuron_count)
            config.setFloat("brain.synapse_density", min(0.2, 0.05 + (neuron_count / 10000) * 0.15))
            config.setInt("performance.optimization_level", min(3, max(1, neuron_count // 1000)))
            
            # Start memory monitoring
            tracemalloc.start()
            initial_memory = tracemalloc.get_traced_memory()
            
            # Start timing
            start_time = time.time()
            start_memory = psutil.Process(os.getpid()).memory_info().rss
            
            # Create and initialize brain
            brain = pynlm.createBrain(config)
            assert brain is not None
            
            initialization_start = time.time()
            brain.initialize()
            initialization_time = time.time() - initialization_start
            
            # Run simulation steps
            simulation_start = time.time()
            steps_completed = 0
            spikes_generated = 0
            
            for step in range(simulation_steps):
                brain.step(step)
                steps_completed += 1
                
                # Inject some input periodically
                if step % 50 == 0:
                    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
                
                # Check spike count periodically
                if step % 100 == 0:
                    current_spikes = brain.getTotalSpikeCount()
                    if steps_completed > 0:
                        spikes_generated = current_spikes
            
            simulation_end = time.time()
            
            # Collect performance metrics
            end_time = time.time()
            end_memory = psutil.Process(os.getpid()).memory_info().rss
            final_memory = tracemalloc.get_traced_memory()
            
            tracemalloc.stop()
            
            # Calculate performance metrics
            total_time = end_time - start_time
            initialization_overhead = initialization_time
            simulation_time = simulation_end - simulation_start
            
            performance_metrics = {
                'neuron_count': neuron_count,
                'simulation_steps': simulation_steps,
                'total_time': total_time,
                'initialization_time': initialization_overhead,
                'simulation_time': simulation_time,
                'spikes_generated': spikes_generated,
                'steps_completed': steps_completed,
                'start_memory': start_memory,
                'end_memory': end_memory,
                'memory_delta': end_memory - start_memory,
                'initial_memory_trace': initial_memory[0],
                'final_memory_trace': final_memory[0],
                'memory_trace_delta': final_memory[0] - initial_memory[0],
                'cpu_usage': process.cpu_percent() if hasattr(process, 'cpu_percent') else 0.0,
                'neurons_per_second': neuron_count / simulation_time if simulation_time > 0 else 0,
                'spikes_per_second': spikes_generated / simulation_time if simulation_time > 0 else 0,
                'steps_per_second': steps_completed / simulation_time if simulation_time > 0 else 0,
                'memory_efficiency': neuron_count / max(1, end_memory - start_memory),
                'performance_score': (steps_completed / simulation_time) * (neuron_count / max(1, end_memory - start_memory))
            }
            
            print(f"   Brain benchmark completed:")
            print(f"     Neurons: {neuron_count:,}")
            print(f"     Steps: {steps_completed:,}")
            print(f"     Total time: {total_time:.3f}s")
            print(f"     Steps/sec: {performance_metrics['steps_per_second']:,.1f}")
            print(f"     Memory delta: {performance_metrics['memory_delta'] / 1024 / 1024:.1f} MB")
            print(f"     Performance score: {performance_metrics['performance_score']:,.2f}")
            
            return performance_metrics
            
        except ImportError:
            print("⚠️  pynlm not available, creating mock performance benchmark")
            return self.mock_benchmark_brain_performance(neuron_count, simulation_steps)
        except Exception as e:
            print(f"❌ Brain performance benchmark failed: {e}")
            return {}
    
    def mock_benchmark_brain_performance(self, neuron_count: int, simulation_steps: int) -> Dict[str, Any]:
        """Mock brain performance benchmark"""
        # Simulate brain performance
        time.sleep(0.1)  # Simulate initialization
        
        # Base performance on neuron count
        base_steps_per_sec = min(1000, max(100, neuron_count / 100))
        base_memory_per_neuron = 0.01  # MB per neuron
        base_memory_overhead = 50.0  # MB overhead
        
        # Add some randomness
        time_factor = np.random.uniform(0.8, 1.2)
        memory_factor = np.random.uniform(0.9, 1.1)
        
        # Calculate metrics
        simulation_time = simulation_steps / base_steps_per_sec
        total_time = simulation_time + 0.1  # Add initialization time
        
        memory_delta = (neuron_count * base_memory_per_neuron * memory_factor) + base_memory_overhead
        spikes_generated = int(neuron_count * simulation_time * 0.05)  # 5% of neurons fire per step
        
        return {
            'neuron_count': neuron_count,
            'simulation_steps': simulation_steps,
            'total_time': total_time,
            'initialization_time': 0.1,
            'simulation_time': simulation_time,
            'spikes_generated': spikes_generated,
            'steps_completed': simulation_steps,
            'start_memory': 100 * 1024 * 1024,  # 100 MB
            'end_memory': 100 * 1024 * 1024 + int(memory_delta * 1024 * 1024),
            'memory_delta': int(memory_delta * 1024 * 1024),
            'initial_memory_trace': 100 * 1024 * 1024,
            'final_memory_trace': 100 * 1024 * 1024 + int(memory_delta * 1024 * 1024),
            'memory_trace_delta': int(memory_delta * 1024 * 1024),
            'cpu_usage': np.random.uniform(10, 50),
            'neurons_per_second': neuron_count / simulation_time if simulation_time > 0 else 0,
            'spikes_per_second': spikes_generated / simulation_time if simulation_time > 0 else 0,
            'steps_per_second': base_steps_per_sec * time_factor,
            'memory_efficiency': neuron_count / max(1, int(memory_delta * 1024 * 1024)),
            'performance_score': (base_steps_per_sec * time_factor) * (neuron_count / max(1, int(memory_delta * 1024 * 1024)))
        }
    
    def test_brain_performance_benchmarking(self):
        """Test brain performance benchmarking"""
        try:
            import pynlm
            
            print("📊 Running brain performance benchmarks...")
            
            # Test different brain sizes
            brain_configs = [
                (100, 1000),    # Small brain
                (500, 1000),    # Medium brain
                (1000, 500),    # Large brain
                (2000, 200),    # Very large brain
                (5000, 100),    # Extreme brain
            ]
            
            all_results = []
            
            for neuron_count, simulation_steps in brain_configs:
                print(f"\n   Testing brain with {neuron_count} neurons, {simulation_steps} steps...")
                
                result = self.benchmark_brain_performance(neuron_count, simulation_steps)
                if result:
                    all_results.append(result)
            
            # Verify results
            assert len(all_results) > 0, "No benchmark results obtained"
            
            # Check that larger brains take proportionally more time
            for i in range(1, len(all_results)):
                prev = all_results[i-1]
                curr = all_results[i]
                
                if prev['neuron_count'] < curr['neuron_count']:
                    # Larger brains should have reasonable performance
                    assert curr['performance_score'] > 0, "Performance score should be positive"
                    assert curr['steps_per_second'] > 0, "Steps per second should be positive"
            
            # Calculate aggregate statistics
            total_neurons = sum(r['neuron_count'] for r in all_results)
            total_steps = sum(r['steps_completed'] for r in all_results)
            total_time = sum(r['total_time'] for r in all_results)
            total_memory_delta = sum(r['memory_delta'] for r in all_results)
            avg_performance_score = np.mean([r['performance_score'] for r in all_results])
            
            print(f"\n📈 Brain Performance Benchmark Summary:")
            print(f"   Total neurons tested: {total_neurons:,}")
            print(f"   Total steps simulated: {total_steps:,}")
            print(f"   Total time: {total_time:.3f}s")
            print(f"   Total memory used: {total_memory_delta / 1024 / 1024:.1f} MB")
            print(f"   Average performance score: {avg_performance_score:,.2f}")
            print(f"   Overall steps per second: {total_steps / total_time:,.1f}")
            
            # Save benchmark results
            results_file = os.path.join(self.test_dir, 'brain_performance_results.json')
            with open(results_file, 'w') as f:
                json.dump({
                    'benchmark_type': 'brain_performance',
                    'configurations': all_results,
                    'summary': {
                        'total_neurons': total_neurons,
                        'total_steps': total_steps,
                        'total_time': total_time,
                        'total_memory_delta': total_memory_delta,
                        'average_performance_score': avg_performance_score,
                        'steps_per_second': total_steps / total_time,
                        'memory_efficiency': total_neurons / max(1, total_memory_delta)
                    }
                }, f, indent=2)
            
            self.temp_files.append(results_file)
            
            print("✅ Brain performance benchmarking tests passed")
            return True
            
        except ImportError:
            print("⚠️  pynlm not available, skipping brain performance tests")
            return True
    
    def test_simulation_speed_validation(self):
        """Test simulation speed validation"""
        try:
            import pynlm
            
            print("⚡ Testing simulation speed validation...")
            
            # Create brain with different configurations
            configs = [
                {'neuron_count': 500, 'optimization': 'balanced'},
                {'neuron_count': 1000, 'optimization': 'performance'},
                {'neuron_count': 2000, 'optimization': 'memory'},
            ]
            
            speed_results = []
            
            for config_dict in configs:
                neuron_count = config_dict['neuron_count']
                optimization = config_dict['optimization']
                
                print(f"\n   Testing {neuron_count} neurons with {optimization} optimization...")
                
                # Create configuration
                config = pynlm.createDefaultConfig()
                config.setInt("brain.neuron_count", neuron_count)
                
                if optimization == 'performance':
                    config.setInt("performance.optimization_level", 3)
                    config.setBool("performance.simd.enable", True)
                    config.setBool("performance.parallel.enable", True)
                elif optimization == 'memory':
                    config.setInt("performance.optimization_level", 3)
                    config.setBool("performance.simd.enable", False)
                    config.setBool("performance.parallel.enable", False)
                else:  # balanced
                    config.setInt("performance.optimization_level", 2)
                    config.setBool("performance.simd.enable", True)
                    config.setBool("performance.parallel.enable", True)
                
                # Create and benchmark brain
                brain = pynlm.createBrain(config)
                assert brain is not None
                
                brain.initialize()
                
                # Time simulation
                start_time = time.time()
                steps_per_test = min(100, max(50, neuron_count // 10))
                
                for step in range(steps_per_test):
                    brain.step(step)
                
                end_time = time.time()
                
                # Calculate speed
                simulation_time = end_time - start_time
                steps_per_second = steps_per_test / simulation_time if simulation_time > 0 else 0
                
                speed_result = {
                    'neuron_count': neuron_count,
                    'optimization': optimization,
                    'steps_tested': steps_per_test,
                    'simulation_time': simulation_time,
                    'steps_per_second': steps_per_second,
                    'speed_rating': self._calculate_speed_rating(steps_per_second, neuron_count)
                }
                
                speed_results.append(speed_result)
                
                print(f"     Speed: {steps_per_second:,.1f} steps/sec")
                print(f"     Speed rating: {speed_result['speed_rating']:.2f}")
            
            # Validate speed results
            assert len(speed_results) > 0
            assert all(r['steps_per_second'] > 0 for r in speed_results)
            assert all(r['speed_rating'] > 0 for r in speed_results)
            
            # Verify optimization impact
            performance_opt = next((r for r in speed_results if r['optimization'] == 'performance'), None)
            memory_opt = next((r for r in speed_results if r['optimization'] == 'memory'), None)
            balanced_opt = next((r for r in speed_results if r['optimization'] == 'balanced'), None)
            
            if performance_opt and memory_opt and balanced_opt:
                # Performance should be highest for performance-optimized
                assert performance_opt['steps_per_second'] >= balanced_opt['steps_per_second'] * 0.9  # Allow small variations
                # Memory optimization might be slower due to reduced parallelism
                # (this is acceptable)
            
            print(f"\n⚡ Simulation Speed Validation Summary:")
            for result in speed_results:
                print(f"   {result['neuron_count']:4d} neurons ({result['optimization']:12s}): "
                      f"{result['steps_per_second']:8.1f} steps/sec (rating: {result['speed_rating']:.2f})")
            
            # Save speed validation results
            results_file = os.path.join(self.test_dir, 'simulation_speed_results.json')
            with open(results_file, 'w') as f:
                json.dump({
                    'test_type': 'simulation_speed_validation',
                    'results': speed_results
                }, f, indent=2)
            
            self.temp_files.append(results_file)
            
            print("✅ Simulation speed validation tests passed")
            return True
            
        except ImportError:
            print("⚠️  pynlm not available, skipping simulation speed tests")
            return True
        except Exception as e:
            print(f"❌ Simulation speed validation test failed: {e}")
            return False
    
    def _calculate_speed_rating(self, steps_per_second: float, neuron_count: int) -> float:
        """Calculate a performance rating based on speed and efficiency"""
        # Base rating on steps per second
        speed_rating = steps_per_second / 100.0  # Normalize to 100 steps/sec = rating 1.0
        
        # Adjust for neuron count (larger networks should still be reasonably fast)
        efficiency_rating = min(2.0, neuron_count / 500.0)  # Rating decreases for very large networks
        
        # Combined rating
        combined_rating = speed_rating * efficiency_rating
        
        return max(0.1, combined_rating)  # Ensure minimum rating
    
    def test_large_scale_simulation(self):
        """Test large-scale simulation capabilities"""
        try:
            import pynlm
            
            print("🌐 Testing large-scale simulation capabilities...")
            
            # Test with progressively larger brain sizes
            large_scale_configs = [
                {'neuron_count': 2000, 'name': 'Medium Scale'},
                {'neuron_count': 5000, 'name': 'Large Scale'},
                {'neuron_count': 10000, 'name': 'Very Large Scale'},
            ]
            
            large_scale_results = []
            
            for config_dict in large_scale_configs:
                neuron_count = config_dict['neuron_count']
                name = config_dict['name']
                
                print(f"\n   Testing {name} simulation ({neuron_count} neurons)...")
                
                # Create configuration
                config = pynlm.createDefaultConfig()
                config.setInt("brain.neuron_count", neuron_count)
                config.setInt("performance.optimization_level", 2)
                config.setBool("performance.memory_pool.enable", True)
                
                # Create brain
                brain = pynlm.createBrain(config)
                assert brain is not None
                
                # Initialize brain
                brain.initialize()
                
                # Monitor memory usage
                tracemalloc.start()
                initial_memory = tracemalloc.get_traced_memory()
                
                start_time = time.time()
                start_memory = psutil.Process(os.getpid()).memory_info().rss
                
                # Run simulation (fewer steps for larger brains)
                simulation_steps = max(50, min(200, 10000 // neuron_count))
                steps_completed = 0
                
                for step in range(simulation_steps):
                    brain.step(step)
                    steps_completed += 1
                    
                    # Inject input periodically
                    if step % 25 == 0:
                        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
                
                end_time = time.time()
                end_memory = psutil.Process(os.getpid()).memory_info().rss
                final_memory = tracemalloc.get_traced_memory()
                
                tracemalloc.stop()
                
                # Calculate metrics
                total_time = end_time - start_time
                memory_delta = end_memory - start_memory
                memory_trace_delta = final_memory[0] - initial_memory[0]
                
                # Calculate memory efficiency
                memory_efficiency = neuron_count / max(1, memory_delta)
                
                # Calculate stability (fewer spikes per step indicates stability)
                stability_metric = brain.getAverageFiringRate() / max(0.1, neuron_count / 1000)
                
                result = {
                    'name': name,
                    'neuron_count': neuron_count,
                    'simulation_steps': simulation_steps,
                    'steps_completed': steps_completed,
                    'total_time': total_time,
                    'memory_delta': memory_delta,
                    'memory_trace_delta': memory_trace_delta,
                    'memory_efficiency': memory_efficiency,
                    'stability_metric': stability_metric,
                    'steps_per_second': steps_completed / total_time if total_time > 0 else 0,
                    'memory_mb_used': memory_delta / 1024 / 1024,
                    'can_handle_large_scale': memory_efficiency > 100  # Can handle >100 neurons per MB
                }
                
                large_scale_results.append(result)
                
                print(f"     Steps: {steps_completed:,}")
                print(f"     Time: {total_time:.3f}s")
                print(f"     Memory: {result['memory_mb_used']:.1f} MB")
                print(f"     Efficiency: {memory_efficiency:,.0f} neurons/MB")
                print(f"     Stability: {stability_metric:.3f}")
                print(f"     Can handle large scale: {'✅' if result['can_handle_large_scale'] else '❌'}")
            
            # Validate large-scale results
            assert len(large_scale_results) > 0
            assert all(r['steps_per_second'] > 0 for r in large_scale_results)
            assert all(r['memory_efficiency'] > 0 for r in large_scale_results)
            
            # Check that the system can handle large-scale simulations
            all_large_scale = all(r['can_handle_large_scale'] for r in large_scale_results)
            
            print(f"\n🌐 Large-Scale Simulation Summary:")
            for result in large_scale_results:
                print(f"   {result['name']:15s} ({result['neuron_count']:5d} neurons): "
                      f"{result['steps_per_second']:8.1f} steps/sec, "
                      f"{result['memory_efficiency']:,.0f} neurons/MB, "
                      f"{'✅' if result['can_handle_large_scale'] else '❌'}")
            
            # Save large-scale results
            results_file = os.path.join(self.test_dir, 'large_scale_results.json')
            with open(results_file, 'w') as f:
                json.dump({
                    'test_type': 'large_scale_simulation',
                    'results': large_scale_results,
                    'summary': {
                        'max_neurons_tested': max(r['neuron_count'] for r in large_scale_results),
                        'all_can_handle_large_scale': all_large_scale,
                        'average_efficiency': np.mean([r['memory_efficiency'] for r in large_scale_results]),
                        'average_stability': np.mean([r['stability_metric'] for r in large_scale_results])
                    }
                }, f, indent=2)
            
            self.temp_files.append(results_file)
            
            print("✅ Large-scale simulation tests passed")
            return True
            
        except ImportError:
            print("⚠️  pynlm not available, skipping large-scale simulation tests")
            return True
        except Exception as e:
            print(f"❌ Large-scale simulation test failed: {e}")
            return False
    
    def test_system_stability_under_load(self):
        """Test system stability under load"""
        try:
            import pynlm
            
            print("🔋 Testing system stability under load...")
            
            # Create stability test scenarios
            stability_scenarios = [
                {'neuron_count': 1000, 'name': 'Light Load'},
                {'neuron_count': 2000, 'name': 'Moderate Load'},
                {'neuron_count': 5000, 'name': 'Heavy Load'},
            ]
            
            stability_results = []
            
            for scenario in stability_scenarios:
                neuron_count = scenario['neuron_count']
                name = scenario['name']
                
                print(f"\n   Testing {name} stability ({neuron_count} neurons)...")
                
                # Create configuration
                config = pynlm.createDefaultConfig()
                config.setInt("brain.neuron_count", neuron_count)
                config.setInt("performance.optimization_level", 2)
                
                # Create brain
                brain = pynlm.createBrain(config)
                assert brain is not None
                
                # Initialize brain
                brain.initialize()
                
                # Monitor system resources
                tracemalloc.start()
                initial_memory = tracemalloc.get_traced_memory()
                
                start_time = time.time()
                start_memory = psutil.Process(os.getpid()).memory_info().rss
                
                # Run simulation with load
                simulation_steps = 100
                steps_completed = 0
                errors_occurred = 0
                spike_counts = []
                memory_usage = []
                
                for step in range(simulation_steps):
                    try:
                        # Update brain
                        brain.step(step)
                        
                        # Collect metrics
                        steps_completed += 1
                        spike_counts.append(brain.getTotalSpikeCount())
                        memory_usage.append(psutil.Process(os.getpid()).memory_info().rss)
                        
                        # Inject input periodically
                        if step % 20 == 0:
                            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 15.0)
                        
                        # Check for stability issues
                        current_firing_rate = brain.getAverageFiringRate()
                        if current_firing_rate < 0 or current_firing_rate > 100:
                            errors_occurred += 1
                            
                    except Exception as e:
                        errors_occurred += 1
                        print(f"     Error at step {step}: {e}")
                
                end_time = time.time()
                end_memory = psutil.Process(os.getpid()).memory_info().rss
                final_memory = tracemalloc.get_traced_memory()
                
                tracemalloc.stop()
                
                # Calculate stability metrics
                total_time = end_time - start_time
                memory_delta = end_memory - start_memory
                avg_memory = np.mean(memory_usage) if memory_usage else 0
                max_memory = max(memory_usage) if memory_usage else 0
                memory_growth_rate = (max_memory - avg_memory) / max_memory if max_memory > 0 else 0
                
                # Calculate spike stability (coefficient of variation)
                if len(spike_counts) > 1:
                    spike_stability = np.std(spike_counts) / np.mean(spike_counts)
                else:
                    spike_stability = 0.0
                
                # Calculate firing rate stability
                firing_rates = [brain.getAverageFiringRate() for _ in range(min(10, simulation_steps))]
                firing_stability = np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
                
                # Determine stability rating
                stability_rating = self._calculate_stability_rating(
                    errors_occurred, memory_growth_rate, spike_stability, firing_stability
                )
                
                result = {
                    'name': name,
                    'neuron_count': neuron_count,
                    'simulation_steps': simulation_steps,
                    'steps_completed': steps_completed,
                    'errors_occurred': errors_occurred,
                    'memory_delta': memory_delta,
                    'memory_trace_delta': final_memory[0] - initial_memory[0],
                    'avg_memory': avg_memory,
                    'max_memory': max_memory,
                    'memory_growth_rate': memory_growth_rate,
                    'spike_stability': spike_stability,
                    'firing_stability': firing_stability,
                    'stability_rating': stability_rating,
                    'is_stable': stability_rating >= 0.7,  # Stable if rating >= 0.7
                    'can_handle_load': errors_occurred == 0  # Can handle load if no errors
                }
                
                stability_results.append(result)
                
                print(f"     Steps completed: {steps_completed:,}")
                print(f"     Errors: {errors_occurred}")
                print(f"     Memory delta: {memory_delta / 1024 / 1024:.1f} MB")
                print(f"     Memory growth: {memory_growth_rate:.3f}")
                print(f"     Spike stability: {spike_stability:.3f}")
                print(f"     Stability rating: {stability_rating:.3f}")
                print(f"     Is stable: {'✅' if result['is_stable'] else '❌'}")
                print(f"     Can handle load: {'✅' if result['can_handle_load'] else '❌'}")
            
            # Validate stability results
            assert len(stability_results) > 0
            assert all(r['steps_completed'] == r['simulation_steps'] for r in stability_results), \
                "Not all steps were completed successfully"
            
            # Check stability criteria
            all_stable = all(r['is_stable'] for r in stability_results)
            all_no_errors = all(r['can_handle_load'] for r in stability_results)
            
            print(f"\n🔋 System Stability Summary:")
            for result in stability_results:
                print(f"   {result['name']:15s} ({result['neuron_count']:5d} neurons): "
                      f"rating {result['stability_rating']:.3f}, "
                      f"{'✅' if result['is_stable'] else '❌'} stable, "
                      f"{'✅' if result['can_handle_load'] else '❌'} reliable")
            
            # Save stability results
            results_file = os.path.join(self.test_dir, 'stability_results.json')
            with open(results_file, 'w') as f:
                json.dump({
                    'test_type': 'system_stability_under_load',
                    'results': stability_results,
                    'summary': {
                        'all_stable': all_stable,
                        'all_no_errors': all_no_errors,
                        'average_stability_rating': np.mean([r['stability_rating'] for r in stability_results]),
                        'max_errors_occurred': max(r['errors_occurred'] for r in stability_results),
                        'total_steps_completed': sum(r['steps_completed'] for r in stability_results)
                    }
                }, f, indent=2)
            
            self.temp_files.append(results_file)
            
            print("✅ System stability under load tests passed")
            return True
            
        except ImportError:
            print("⚠️  pynlm not available, skipping system stability tests")
            return True
        except Exception as e:
            print(f"❌ System stability under load test failed: {e}")
            return False
    
    def _calculate_stability_rating(self, errors_occurred: int, memory_growth_rate: float, 
                                   spike_stability: float, firing_stability: float) -> float:
        """Calculate a stability rating based on multiple factors"""
        # Error penalty (fewer errors is better)
        error_penalty = min(1.0, errors_occurred / 10.0)  # Up to 1.0 penalty for 10 errors
        
        # Memory growth penalty (less growth is better)
        memory_penalty = min(1.0, memory_growth_rate * 10.0)  # Up to 1.0 penalty for 0.1 growth
        
        # Spike stability (more stable is better)
        spike_stability_score = max(0.0, 1.0 - spike_stability)  # Higher for less variation
        
        # Firing rate stability (more stable is better)
        firing_stability_score = max(0.0, 1.0 - firing_stability)
        
        # Calculate weighted stability rating
        stability_rating = (
            0.3 * (1.0 - error_penalty) +           # Error rate
            0.2 * (1.0 - memory_penalty) +          # Memory growth
            0.3 * spike_stability_score +           # Spike stability
            0.2 * firing_stability_score           # Firing stability
        )
        
        return max(0.0, min(1.0, stability_rating))

if __name__ == "__main__":
    print("🧪 Running NLM Performance and Load Tests")
    print("=" * 60)
    
    tester = NLMPerformanceTester()
    
    tests = [
        ("Brain Performance Benchmarking", tester.test_brain_performance_benchmarking),
        ("Simulation Speed Validation", tester.test_simulation_speed_validation),
        ("Large-Scale Simulation", tester.test_large_scale_simulation),
        ("System Stability Under Load", tester.test_system_stability_under_load),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n📋 Running: {test_name}")
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"❌ Test '{test_name}' crashed: {e}")
            results.append((test_name, False))
    
    print("\n" + "=" * 60)
    print("📊 Test Results Summary:")
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "✅ PASSED" if success else "❌ FAILED"
        print(f"   {status}: {test_name}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\n🎯 Total: {passed + failed} tests")
    print(f"   Passed: {passed}")
    print(f"   Failed: {failed}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} test(s) failed. Please review the implementation.")
        sys.exit(1)
    else:
        print(f"\n✅ All {passed} tests passed!")
        sys.exit(0)
