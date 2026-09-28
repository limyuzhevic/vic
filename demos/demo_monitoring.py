import pynlm
import time

"""
Demo 9: Real-Time Monitoring and Analysis

This demo showcases real-time monitoring and analysis features from the NLM Python API.
"""

def demo_monitoring():
    print("=== NLM Real-Time Monitoring and Analysis Demo ===")
    
    # Setup monitoring
    print("\n1. Setting up real-time monitoring...")
    
    config, brain = pynlm.quick_test_setup()
    
    print("   Monitoring setup complete")
    print(f"   - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"   - Monitoring interval: 25 steps")
    
    # Start monitoring
    print("\n2. Starting real-time monitoring (150 steps)...")
    
    start_time = time.time()
    
    # Monitor brain health
    history = pynlm.monitor_brain_health(brain, duration_steps=150, update_interval=25.0)
    
    end_time = time.time()
    monitoring_time = end_time - start_time
    
    print(f"   Monitoring completed in {monitoring_time:.3f} seconds")
    print(f"   Collected {len(history)} monitoring snapshots")
    
    # Analyze monitoring data
    print("\n3. Analyzing monitoring data...")
    
    if history:
        # Basic statistics
        steps = [h["step"] for h in history]
        total_spikes = [h["total_spikes"] for h in history]
        firing_neurons = [h["firing_neurons"] for h in history]
        firing_rates = [h["avg_firing_rate"] for h in history]
        e_i_ratios = [h["e_i_ratio"] for h in history]
        
        # Calculate statistics
        initial_spikes = total_spikes[0]
        final_spikes = total_spikes[-1]
        spike_growth = final_spikes - initial_spikes
        
        avg_firing = sum(firing_neurons) / len(firing_neurons)
        avg_rate = sum(firing_rates) / len(firing_rates)
        avg_ei_ratio = sum(e_i_ratios) / len(e_i_ratios)
        
        print("   Monitoring Analysis:")
        print(f"     Initial total spikes: {initial_spikes}")
        print(f"     Final total spikes: {final_spikes}")
        print(f"     Spike growth: {spike_growth}")
        print(f"     Average firing neurons: {avg_firing:.1f}")
        print(f"     Average firing rate: {avg_rate:.2f} Hz")
        print(f"     Average E/I ratio: {avg_ei_ratio:.2f}")
        
        # Calculate stability metrics
        spike_variance = sum((s - initial_spikes) ** 2 for s in total_spikes) / len(total_spikes)
        spike_std = spike_variance ** 0.5
        
        firing_variance = sum((f - avg_firing) ** 2 for f in firing_neurons) / len(firing_neurons)
        firing_std = firing_variance ** 0.5
        
        print(f"     Spike stability (std dev): {spike_std:.1f}")
        print(f"     Firing stability (std dev): {firing_std:.1f}")
        
        # Determine stability
        spike_stable = spike_std < 100
        firing_stable = firing_std < 10
        
        print(f"     Spike activity: {'Stable' if spike_stable else 'Variable'}")
        print(f"     Firing pattern: {'Stable' if firing_stable else 'Variable'}")
        
        # Analyze trends
        spike_trend = "Increasing" if total_spikes[-1] > total_spikes[0] else "Decreasing" if total_spikes[-1] < total_spikes[0] else "Stable"
        rate_trend = "Increasing" if firing_rates[-1] > firing_rates[0] else "Decreasing" if firing_rates[-1] < firing_rates[0] else "Stable"
        
        print(f"     Spike trend: {spike_trend}")
        print(f"     Rate trend: {rate_trend}")
    
    # Demonstrate performance analysis
    print("\n4. Demonstrating performance analysis...")
    
    try:
        # Create another brain for performance benchmarking
        config2 = pynlm.create_advanced_config("default")
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        print("   Running performance benchmark...")
        benchmark_results = pynlm.benchmark_brain(brain2, 100, measure_memory=True)
        
        print("   Performance Analysis:")
        for key, value in benchmark_results.items():
            if isinstance(value, float):
                if key in ["total_time_seconds", "steps_per_second", "spikes_per_step", "spikes_per_second"]:
                    print(f"     {key}: {value:.3f}")
                else:
                    print(f"     {key}: {value}")
            else:
                print(f"     {key}: {value}")
        
        # Performance assessment
        steps_per_sec = benchmark_results["steps_per_second"]
        if steps_per_sec > 1000:
            performance = "Excellent"
        elif steps_per_sec > 500:
            performance = "Good"
        elif steps_per_sec > 100:
            performance = "Fair"
        else:
            performance = "Slow"
        
        print(f"     Performance assessment: {performance}")
        
    except Exception as e:
        print(f"   Performance analysis error: {e}")
    
    # Demonstrate data export
    print("\n5. Demonstrating data export...")
    
    try:
        # Export brain statistics to file
        filename = "monitoring_results.json"
        result = pynlm.export_brain_stats(brain, filename, include_detailed_analysis=True)
        
        if result:
            print(f"   Brain statistics exported to '{filename}'")
            print("   Export includes detailed analysis and metadata")
            
            # Show file info
            import os
            if os.path.exists(filename):
                file_size = os.path.getsize(filename)
                print(f"   File size: {file_size} bytes")
        
    except Exception as e:
        print(f"   Data export error: {e}")
    
    # Demonstrate comprehensive monitoring report
    print("\n6. Generating comprehensive monitoring report...")
    
    if history:
        # Calculate monitoring summary
        summary = {
            "duration_steps": len(history),
            "initial_conditions": {
                "neurons": history[0]["firing_neurons"],
                "spikes": history[0]["total_spikes"],
                "rate": history[0]["avg_firing_rate"]
            },
            "final_conditions": {
                "neurons": history[-1]["firing_neurons"],
                "spikes": history[-1]["total_spikes"],
                "rate": history[-1]["avg_firing_rate"]
            },
            "stability": {
                "spike_stable": spike_stable if 'spike_stable' in locals() else False,
                "firing_stable": firing_stable if 'firing_stable' in locals() else False
            },
            "trends": {
                "spike_trend": spike_trend if 'spike_trend' in locals() else "Unknown",
                "rate_trend": rate_trend if 'rate_trend' in locals() else "Unknown"
            }
        }
        
        print("   Monitoring Summary:")
        print(f"     Total monitoring steps: {summary['duration_steps']}")
        print(f"     Initial firing neurons: {summary['initial_conditions']['neurons']}")
        print(f"     Final firing neurons: {summary['final_conditions']['neurons']}")
        print(f"     Spike activity stable: {summary['stability']['spike_stable']}")
        print(f"     Firing pattern stable: {summary['stability']['firing_stable']}")
        print(f"     Spike trend: {summary['trends']['spike_trend']}")
        print(f"     Rate trend: {summary['trends']['rate_trend']}")
    
    print("\n=== Demo Complete ===")
    return True

if __name__ == "__main__":
    demo_monitoring()
