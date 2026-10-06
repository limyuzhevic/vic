#!/usr/bin/env python3
"""
NLM Performance Benchmark

Benchmarks key performance metrics for the NLM neural simulation framework
to establish baseline performance and track improvements over time.
"""

import pynlm
import time
import statistics
import math

def benchmark_brain_creation(num_trials=20):
    """Benchmark brain creation and initialization"""
    print(f"=== Brain Creation Benchmark ({num_trials} trials) ===")
    
    times = []
    neuron_counts = []
    synapse_counts = []
    
    for i in range(num_trials):
        # Time config creation
        start = time.time()
        config = pynlm.createDefaultConfig()
        config.set("random_seed", i)  # Different seed each trial
        
        # Time brain creation
        brain = pynlm.createBrain(config)
        brain_start = time.time()
        brain.initialize()
        
        end = time.time()
        
        elapsed = end - start
        brain_only_time = end - brain_start
        
        times.append(elapsed)
        neuron_counts.append(brain.getTotalNeuronCount())
        synapse_counts.append(brain.getTotalSynapseCount())
        
        if i % 5 == 0:
            print(f"  Trial {i+1}: {elapsed:.3f}s total, {brain_only_time:.3f}s brain init")
    
    avg_time = statistics.mean(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0
    avg_neurons = statistics.mean(neuron_counts)
    avg_synapses = statistics.mean(synapse_counts)
    
    print(f"✓ Brain Creation Benchmark Results:")
    print(f"  Average time: {avg_time:.3f}s ± {std_time:.3f}s")
    print(f"  Average neurons: {avg_neurons:.0f}")
    print(f"  Average synapses: {avg_synapses:.0f}")
    print(f"  Brains per second: {1.0 / avg_time:.1f}")
    
    return {
        'avg_time': avg_time,
        'std_time': std_time,
        'avg_neurons': avg_neurons,
        'avg_synapses': avg_synapses,
        'trials': num_trials
    }

def benchmark_simulation_performance(brain, num_steps=500, timestep=0.001):
    """Benchmark simulation performance"""
    print(f"\n=== Simulation Performance Benchmark ({num_steps} steps) ===")
    
    # Warm-up run
    for warm_step in range(50):
        brain.step(warm_step, warm_step * timestep)
    
    # Benchmark
    start_time = time.time()
    start_spikes = brain.getTotalSpikeCount()
    start_firing = brain.getAverageFiringRate()
    
    for step in range(num_steps):
        brain.step(step, step * timestep)
        
        # Inject some input to maintain activity
        if step % 10 == 0:
            from pynlm import NeuronType
            # Inject current to multiple neurons
            for i in range(min(10, brain.getTotalNeuronCount())):
                brain.injectCurrent(i, 30.0)
    
    end_time = time.time()
    
    total_time = end_time - start_time
    end_spikes = brain.getTotalSpikeCount()
    end_firing = brain.getAverageFiringRate()
    
    spikes_generated = end_spikes - start_spikes
    avg_firing_rate = (end_firing + start_firing) / 2
    
    # Calculate performance metrics
    steps_per_second = num_steps / total_time
    seconds_per_step = total_time / num_steps
    neurons_per_second = brain.getTotalNeuronCount() * steps_per_second
    spikes_per_neuron = spikes_generated / brain.getTotalNeuronCount()
    
    print(f"✓ Simulation Benchmark Results:")
    print(f"  Total time: {total_time:.3f}s")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Seconds per step: {seconds_per_step:.4f}")
    print(f"  Neurons per second: {neurons_per_second:.0f}")
    print(f"  Spikes generated: {spikes_generated}")
    print(f"  Average firing rate: {avg_firing_rate:.2f}Hz")
    print(f"  Spikes per neuron: {spikes_per_neuron:.2f}")
    
    return {
        'total_time': total_time,
        'steps_per_second': steps_per_second,
        'seconds_per_step': seconds_per_step,
        'spikes_generated': spikes_generated,
        'avg_firing_rate': avg_firing_rate,
        'spikes_per_neuron': spikes_per_neuron
    }

def benchmark_memory_usage(brain, test_steps=100):
    """Benchmark memory-related operations"""
    print(f"\n=== Memory Usage Benchmark ({test_steps} steps) ===")
    
    # Get initial memory metrics
    initial_neural_count = brain.getTotalNeuronCount()
    
    # Benchmark step operations (which involves memory updates)
    start_time = time.time()
    
    for step in range(test_steps):
        # Step simulation
        brain.step(step, step * 0.001)
        
        # Do some memory-related operations
        if step % 20 == 0:
            # Access various memory systems
            active_count = brain.getActiveNeuronCount()
            firing_count = brain.getFiringNeuronCount()
            avg_firing = brain.getAverageFiringRate()
            
            # These operations involve memory system access
            _ = active_count + firing_count + avg_firing
    
    end_time = time.time()
    
    total_time = end_time - start_time
    steps_per_second = test_steps / total_time
    
    print(f"✓ Memory Usage Benchmark Results:")
    print(f"  Total time: {total_time:.3f}s")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Time per step: {total_time / test_steps * 1000:.2f}ms")
    
    # Check memory growth
    final_neural_count = brain.getTotalNeuronCount()
    memory_growth = final_neural_count - initial_neural_count
    
    print(f"  Memory growth: {memory_growth} neurons")
    
    return {
        'total_time': total_time,
        'steps_per_second': steps_per_second,
        'memory_growth': memory_growth
    }

def benchmark_plasticity_operations(brain, steps=200):
    """Benchmark plasticity operations"""
    print(f"\n=== Plasticity Operations Benchmark ({steps} steps) ===")
    
    # Warm-up
    for warm_step in range(20):
        brain.step(warm_step, warm_step * 0.001)
    
    start_time = time.time()
    
    # Benchmark plasticity operations
    for step in range(steps):
        # Inject patterns to trigger plasticity
        from pynlm import NeuronType
        
        # Create correlated activity for STDP
        if step % 5 == 0:
            # Pre-synaptic neuron activation
            pre_neuron = min(5, brain.getTotalNeuronCount() - 1)
            brain.injectCurrent(pre_neuron, 50.0)
            brain.step(step * 2, step * 2 * 0.001)
            
            # Post-synaptic neuron activation (short delay for STDP)
            if step % 10 == 0:
                post_neuron = min(10, brain.getTotalNeuronCount() - 1)
                brain.injectCurrent(post_neuron, 50.0)
                brain.step(step * 2 + 1, (step * 2 + 1) * 0.001)
        
        # Inject Hebbian-like correlated activity
        if step % 7 == 0:
            # Simultaneous activation of related neurons
            neurons = []
            for i in range(min(3, brain.getTotalNeuronCount())):
                neurons.append(i)
                brain.injectCurrent(i, 40.0)
            
            # Step to allow correlation
            brain.step(step, step * 0.001)
    
    end_time = time.time()
    
    total_time = end_time - start_time
    steps_per_second = steps / total_time
    
    print(f"✓ Plasticity Benchmark Results:")
    print(f"  Total time: {total_time:.3f}s")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Time per step: {total_time / steps * 1000:.2f}ms")
    
    return {
        'total_time': total_time,
        'steps_per_second': steps_per_second
    }

def benchmark_configuration_operations(num_trials=50):
    """Benchmark configuration management operations"""
    print(f"\n=== Configuration Operations Benchmark ({num_trials} trials) ===")
    
    times = []
    
    for i in range(num_trials):
        start = time.time()
        
        # Create and configure brain
        config = pynlm.createDefaultConfig()
        config.set("random_seed", i)
        config.set("neuron_count", 100 + (i % 50))
        config.set("simulation_timestep", 0.001)
        config.set("enable_plasticity", True)
        config.set("plasticity_hebbian", True)
        config.set("plasticity_stdp", True)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Do some configuration operations
        keys = brain.getConfig().getKeys()
        config_val = brain.getConfig().getOr("neuron_count", 0)
        
        end = time.time()
        
        times.append(end - start)
    
    avg_time = statistics.mean(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0
    
    print(f"✓ Configuration Benchmark Results:")
    print(f"  Average time: {avg_time:.3f}s ± {std_time:.3f}s")
    print(f"  Configurations per second: {1.0 / avg_time:.1f}")
    
    return {
        'avg_time': avg_time,
        'std_time': std_time,
        'trials': num_trials
    }

def benchmark_python_api_access(brain, world, agent_brain, num_accesses=100):
    """Benchmark Python API access patterns"""
    print(f"\n=== Python API Access Benchmark ({num_accesses} accesses) ===")
    
    from pynlm import Vision, ActionType
    
    # Warm-up
    for warm_step in range(20):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        agent_brain.updateDevelopment(warm_step * 0.001)
        motor_command = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(motor_command, warm_step * 0.001)
    
    start_time = time.time()
    
    # Benchmark various API accesses
    for i in range(num_accesses):
        # Sensory processing
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Development updates
        agent_brain.updateDevelopment(i * 0.001)
        
        # Motor commands
        motor_command = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(motor_command, i * 0.001)
        
        # Configuration access
        config = brain.getConfig()
        config_summary = config.summary()
        
        # Brain status
        brain.logStatus()
        
        # Every 10th access, do some complex operations
        if i % 10 == 0:
            # These are more expensive operations
            body = world.getAgentBody()
            vision = world.getSensoryPercept().getVision()
            neurons = brain.getActiveNeuronCount()
            firing = brain.getFiringNeuronCount()
    
    end_time = time.time()
    
    total_time = end_time - start_time
    accesses_per_second = num_accesses / total_time
    
    print(f"✓ Python API Access Benchmark Results:")
    print(f"  Total time: {total_time:.3f}s")
    print(f"  API accesses per second: {accesses_per_second:.1f}")
    print(f"  Average time per access: {total_time / num_accesses * 1000:.2f}ms")
    
    return {
        'total_time': total_time,
        'accesses_per_second': accesses_per_second,
        'avg_time_per_access_ms': total_time / num_accesses * 1000
    }

def run_all_benchmarks():
    """Run all comprehensive benchmarks"""
    print("=" * 60)
    print("NLM Performance Benchmark Suite")
    print("=" * 60)
    
    # Setup components
    print("Setting up benchmark components...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.setAgentStart(5, 5)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    benchmark_results = {}
    
    try:
        # Run benchmarks
        benchmark_results['brain_creation'] = benchmark_brain_creation()
        benchmark_results['simulation'] = benchmark_simulation_performance(brain)
        benchmark_results['memory'] = benchmark_memory_usage(brain)
        benchmark_results['plasticity'] = benchmark_plasticity_operations(brain)
        benchmark_results['configuration'] = benchmark_configuration_operations()
        benchmark_results['python_api'] = benchmark_python_api_access(brain, world, agent_brain)
        
        # Summary
        print("\n" + "=" * 60)
        print("BENCHMARK SUMMARY")
        print("=" * 60)
        
        for benchmark_name, results in benchmark_results.items():
            print(f"\n{benchmark_name.upper().replace('_', ' ')}:")
            for key, value in results.items():
                if isinstance(value, float):
                    if key.endswith('_time'):
                        print(f"  {key.replace('_', ' '):.<30} {value:.3f}s")
                    elif key.endswith('_per_second') or key.endswith('_rate'):
                        print(f"  {key.replace('_', ' '):.<30} {value:.1f}")
                    else:
                        print(f"  {key.replace('_', ' '):.<30} {value:.3f}")
                else:
                    print(f"  {key.replace('_', ' '):.<30} {value}")
        
        print(f"\n✓ BENCHMARK SUITE COMPLETED SUCCESSFULLY")
        return benchmark_results
        
    except Exception as e:
        print(f"\n✗ BENCHMARK SUITE FAILED: {e}")
        import traceback
        traceback.print_exc()
        return benchmark_results

if __name__ == "__main__":
    results = run_all_benchmarks()
    print(f"\nBenchmark completed with {len(results)} benchmark sections.")