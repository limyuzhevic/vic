"""NLM Performance Monitoring - Monitoring simulation performance"""
import pynlm
import time

def monitor_performance():
    """Monitor and track simulation performance."""
    print("=== Performance Monitoring Demo ===\n")
    
    # Create monitoring function
    def run_and_monitor(config_name, config_func, num_steps):
        print(f"Running {config_name}...")
        
        # Create and initialize
        start_time = time.time()
        config = config_func()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        initialization_time = time.time() - start_time
        
        # Run simulation with monitoring
        total_firing_rate = 0.0
        max_firing = 0
        step_times = []
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Core simulation loop
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.simulation_time)
            reward = world.computeReward(brain.getRegions()[0].get())
            agent.applyRewardModulation(reward, 0.0)
            
            # Collect metrics
            total_firing_rate += brain.getAverageFiringRate()
            max_firing = max(max_firing, brain.getFiringNeuronCount())
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            # Print progress every 100 steps
            if step % 100 == 0:
                avg_step_time = sum(step_times[-100:]) / len(step_times[-100:])
                print(f"  Step {step:3d}: "
                      f"Firing: {brain.getFiringNeuronCount():3d}, "
                      f"Rate: {brain.getAverageFiringRate():5.2f} Hz, "
                      f"Step time: {avg_step_time*1000:5.1f} ms")
        
        total_time = time.time() - start_time
        avg_step_time = total_time / num_steps
        
        # Calculate metrics
        results = {
            'config_name': config_name,
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'initialization_time': initialization_time,
            'total_firing_rate': total_firing_rate,
            'avg_firing_rate': total_firing_rate / num_steps,
            'max_firing': max_firing,
            'total_spikes': brain.total_spike_count,
            'steps_per_second': num_steps / total_time
        }
        
        print(f"  Summary:")
        print(f"    - Total time: {total_time:.1f}s")
        print(f"    - Average step time: {avg_step_time*1000:.1f} ms")
        print(f"    - Steps per second: {results['steps_per_second']:.0f}")
        print(f"    - Average firing rate: {results['avg_firing_rate']:.2f} Hz")
        print(f"    - Max firing neurons: {max_firing}")
        print(f"    - Total spikes: {results['total_spikes']}")
        
        return results
    
    # Run different configurations
    configurations = [
        ("Quick", pynlm.createQuickBrain),
        ("Default", lambda: pynlm.createBrain(pynlm.createDefaultConfig())),
        ("Research", lambda: pynlm.createBrain(pynlm.createResearchConfig()))
    ]
    
    all_results = []
    
    for config_name, config_func in configurations:
        # Create brain separately for monitoring
        config = config_func() if callable(config_func) else config_func
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"\n{config_name} Configuration:")
        print(f"  - Neurons: {brain.neuron_count:,}")
        print(f"  - Synapses: {brain.synapse_count:,}")
        
        # Run monitoring (shorter for demo)
        results = run_and_monitor(config_name, config_func, 100)
        all_results.append(results)
        print()
    
    # Performance comparison
    print("=== Performance Comparison ===")
    print(f"{'Config':<10} {'Total Time':<12} {'Avg Step':<12} {'Steps/sec':<12} {'Avg Firing':<12}")
    print("-" * 70)
    
    for results in all_results:
        print(f"{results['config_name']:<10} "
              f"{results['total_time']:<12.1f}s "
              f"{results['avg_step_time']*1000:<11.1f}ms "
              f"{results['steps_per_second']:<12.0f} "
              f"{results['avg_firing_rate']:<11.2f} Hz")
    
    print("\n=== Performance monitoring complete ===")

monitor_performance()
