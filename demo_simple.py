#!/usr/bin/env python3
"""
Simple NLM Python Interface - Demonstrating Enhanced Features

This script provides a simple interface to the enhanced NLM Phase 6 system,
showcasing the improvements made to neuromodulation, memory systems, 
activity monitoring, and configuration options.
"""

import sys
import time
from pathlib import Path

# Add the NLM source directory to Python path
sys.path.insert(0, str(Path(__file__).parent))

# Check if required packages are available
try:
    import numpy as np
    print("✅ NumPy available")
except ImportError:
    print("❌ NumPy required but not available")
    print("Please install with: pip install numpy>=1.20 scipy>=1.8.0")
    sys.exit(1)

try:
    # Try to import NLM bindings
    from pynlm import NLM
    print("✅ NLM bindings available")
except ImportError:
    print("❌ NLM Python bindings not available")
    print("Please build the native library first:")
    print("  mkdir build && cd build && cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON")
    print("  make -j4")
    print("  pip install -e .")
    sys.exit(1)

def print_welcome():
    """Print welcome message"""
    print("=" * 70)
    print("NLM Phase 6 Enhanced Python Interface")
    print("=" * 70)
    print("\nWelcome to the enhanced NLM system!")
    print("\nKey improvements:")
    print("  • Real neuromodulation and reward systems")
    print("  • Neural activity monitoring and control")
    print("  • Enhanced memory systems integration")
    print("  • Advanced configuration options")
    print("  • Performance monitoring tools")
    print("=" * 70)

def demonstrate_basic_usage():
    """Demonstrate basic NLM usage"""
    print("\n[BASIC USAGE DEMONSTRATION]")
    print("-" * 40)
    
    # Create NLM instance with enhanced configuration
    print("Creating NLM neural network...")
    
    # Initialize with advanced parameters
    nlm_config = {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "enable_development": True,
        "target_firing_rate": 12.0,
        "max_firing_rate": 80.0,
        "activity_monitoring": True
    }
    
    print("Configuration:")
    for key, value in nlm_config.items():
        print(f"  {key:25}: {value}")
    
    # Create NLM instance
    nlm = NLM(**nlm_config)
    
    print(f"\n✅ NLM instance created: {nlm.get_neuron_count()} neurons")
    
    # Demonstrate simulation
    print("\nRunning simulation...")
    start_time = time.time()
    
    # Run for a few steps
    for step in range(10):
        nlm.step(step, step * 0.001)
        if step % 5 == 0:
            firing_rate = nlm.get_firing_rate()
            print(f"  Step {step:2}: Firing rate = {firing_rate:6.2f} Hz")
    
    end_time = time.time()
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    
    return nlm

def demonstrate_activity_monitoring(nlm):
    """Demonstrate activity monitoring features"""
    print("\n[ACTIVITY MONITORING DEMONSTRATION]")
    print("-" * 40)
    
    # Get activity summary
    activity_summary = nlm.get_activity_summary()
    print("Activity Summary:")
    print(activity_summary)
    
    # Demonstrate activity control
    print("\nDemonstrating activity control...")
    
    # Set activity thresholds
    nlm.set_activity_thresholds(0.5, 1.2)
    print("✅ Activity thresholds set")
    
    # Get activity statistics
    stats = nlm.get_activity_statistics()
    print("\nActivity Statistics:")
    print(stats)
    
    # Demonstrate anomaly detection
    anomalies = nlm.check_neural_anomalies()
    print(f"\nNeural anomalies detected: {anomalies}")
    
    # Show current control settings
    control_settings = nlm.get_control_settings()
    print("\nCurrent Control Settings:")
    print(control_settings)

def demonstrate_neuromodulation(nlm):
    """Demonstrate neuromodulation features"""
    print("\n[NEUROMODULATION DEMONSTRATION]")
    print("-" * 40)
    
    # Get neuromodulation status
    neuromodulation_status = nlm.get_neuromodulation_status()
    print("Neuromodulation Status:")
    print(neuromodulation_status)
    
    # Demonstrate reward system
    print("\nDemonstrating reward system...")
    
    # Get reward information
    reward_info = nlm.get_reward_info()
    print("Reward Information:")
    for key, value in reward_info.items():
        print(f"  {key:20}: {value}")
    
    # Demonstrate curiosity system
    curiosity_info = nlm.get_curiosity_info()
    print("\nCuriosity Information:")
    for key, value in curiosity_info.items():
        print(f"  {key:20}: {value}")

def demonstrate_memory_systems(nlm):
    """Demonstrate memory systems features"""
    print("\n[MEMORY SYSTEMS DEMONSTRATION]")
    print("-" * 40)
    
    # Get memory information
    memory_info = nlm.get_memory_info()
    print("Memory Information:")
    for key, value in memory_info.items():
        print(f"  {key:20}: {value}")
    
    # Demonstrate episodic memory
    print("\nDemonstrating episodic memory...")
    
    # Get recent episodes
    episodes = nlm.get_recent_episodes(5)
    print(f"Recent episodes ({len(episodes)}):")
    for i, episode in enumerate(episodes):
        print(f"  Episode {i+1}:")
        print(f"    Timestamp: {episode.get('timestamp', 'N/A')}")
        print(f"    Action: {episode.get('action', 'N/A')}")
        print(f"    Reward: {episode.get('reward', 'N/A')}")
        print(f"    Duration: {episode.get('duration', 'N/A')} steps")
    
    # Demonstrate working memory
    working_memory = nlm.get_working_memory()
    print(f"\nWorking Memory:")
    print(f"  Active traces: {working_memory.get('active_traces', 0)}")
    print(f"  Total neurons: {working_memory.get('neuron_count', 0)}")
    print(f"  Current activity: {working_memory.get('current_activity', 0.0):.3f}")

def demonstrate_configuration(nlm):
    """Demonstrate configuration features"""
    print("\n[CONFIGURATION DEMONSTRATION]")
    print("-" * 40)
    
    # Get current configuration
    config = nlm.get_configuration()
    print("Current Configuration:")
    for key, value in config.items():
        print(f"  {key:25}: {value}")
    
    # Demonstrate configuration updates
    print("\nUpdating configuration...")
    
    # Update some parameters
    updates = {
        "target_firing_rate": 15.0,
        "max_firing_rate": 60.0,
        "curiosity_enabled": True,
        "activity_monitoring": True
    }
    
    nlm.update_configuration(updates)
    print("✅ Configuration updated")
    
    # Get updated configuration
    new_config = nlm.get_configuration()
    print("\nUpdated Configuration:")
    for key in ["target_firing_rate", "max_firing_rate", "curiosity_enabled", "activity_monitoring"]:
        print(f"  {key:25}: {new_config.get(key, 'N/A')}")

def demonstrate_performance(nlm):
    """Demonstrate performance monitoring"""
    print("\n[PERFORMANCE MONITORING DEMONSTRATION]")
    print("-" * 40)
    
    # Get performance metrics
    performance = nlm.get_performance_metrics()
    print("Performance Metrics:")
    
    for category, metrics in performance.items():
        print(f"\n{category.upper()}:")
        if isinstance(metrics, dict):
            for key, value in metrics.items():
                print(f"  {key:20}: {value}")
        else:
            print(f"  {metrics}")
    
    # Demonstrate system health
    print("\nSystem Health:")
    health = nlm.get_system_health()
    for key, value in health.items():
        print(f"  {key:20}: {value}")

def main():
    """Main function"""
    print_welcome()
    
    try:
        # Demonstrate basic usage
        nlm = demonstrate_basic_usage()
        
        # Demonstrate activity monitoring
        demonstrate_activity_monitoring(nlm)
        
        # Demonstrate neuromodulation
        demonstrate_neuromodulation(nlm)
        
        # Demonstrate memory systems
        demonstrate_memory_systems(nlm)
        
        # Demonstrate configuration
        demonstrate_configuration(nlm)
        
        # Demonstrate performance
        demonstrate_performance(nlm)
        
        print("\n" + "=" * 70)
        print("✅ ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY")
        print("=" * 70)
        print("\nThe enhanced NLM system is working correctly!")
        print("\nNext steps:")
        print("1. Experiment with different configurations")
        print("2. Use the monitoring tools for optimization")
        print("3. Integrate with your own applications")
        print("4. Explore advanced features in the documentation")
        
    except Exception as e:
        print(f"\n❌ Error during demonstration: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()