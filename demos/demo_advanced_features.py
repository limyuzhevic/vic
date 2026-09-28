import pynlm
import time

"""
Demo 7: Advanced Features

This demo showcases advanced NLM features including learning demonstrations.
"""

def demo_advanced_features():
    print("=== NLM Advanced Features Demo ===")
    
    # Run learning demonstration
    print("\n1. Running learning demonstration...")
    try:
        brain, final_weight, synapse_count = pynlm.run_learning_demo()
        
        print(f"   Learning demo completed successfully:")
        print(f"     Final mean synaptic weight: {final_weight:.4f}")
        print(f"     Number of synapses: {synapse_count}")
        print(f"     Learning {'detected' if final_weight != 0.0 else 'not detected'}")
        print(f"     Total spikes: {brain.getTotalSpikeCount()}")
        print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
        
    except Exception as e:
        print(f"   Learning demo error: {e}")
    
    # Demonstrate monitoring
    print("\n2. Demonstrating brain monitoring...")
    
    # Create quick test setup
    config, brain = pynlm.quick_test_setup()
    
    # Monitor brain health
    print("   Monitoring brain health over 100 steps...")
    history = pynlm.monitor_brain_health(brain, duration_steps=100, update_interval=25.0)
    
    if history:
        print(f"   Collected {len(history)} monitoring snapshots")
        
        # Calculate statistics
        total_spikes = sum(h["total_spikes"] for h in history)
        avg_firing = sum(h["firing_neurons"] for h in history) / len(history)
        
        print(f"   Total spikes during monitoring: {total_spikes}")
        print(f"   Average firing neurons: {avg_firing:.1f}")
        
        # Analyze stability
        firing_variance = sum((h["firing_neurons"] - avg_firing) ** 2 for h in history) / len(history)
        firing_std = firing_variance ** 0.5
        
        print(f"   Firing stability (std dev): {firing_std:.1f}")
        stability = "Stable" if firing_std < 10 else "Variable"
        print(f"   Activity stability: {stability}")
    
    # Demonstrate error handling
    print("\n3. Demonstrating error handling...")
    
    # Validate components
    validation_results = pynlm.validate_brain(brain)
    if validation_results:
        print(f"   Brain validation warnings: {len(validation_results)}")
    
    # Run safe simulation
    print("   Running safe simulation with validation...")
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run safe agent simulation
        result = pynlm.safe_run_agent(brain, world, agent, 50, validate_inputs=True)
        successful_steps, failed_steps, total_spikes = result
        
        print(f"   Safe simulation results:")
        print(f"     Successful steps: {successful_steps}")
        print(f"     Failed steps: {failed_steps}")
        print(f"     Total spikes: {total_spikes}")
        
    except Exception as e:
        print(f"   Safe simulation error: {e}")
    
    # Demonstrate benchmarking
    print("\n4. Demonstrating performance benchmarking...")
    
    try:
        # Create another brain for benchmarking
        config2 = pynlm.create_advanced_config("default")
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        print("   Benchmarking brain performance...")
        benchmark_results = pynlm.benchmark_brain(brain2, 200, measure_memory=True)
        
        print("   Benchmark results:")
        for key, value in benchmark_results.items():
            if isinstance(value, float):
                print(f"     {key}: {value:.3f}")
            else:
                print(f"     {key}: {value}")
                
    except Exception as e:
        print(f"   Benchmarking error: {e}")
    
    print("\n=== Demo Complete ===")
    return True

if __name__ == "__main__":
    demo_advanced_features()
