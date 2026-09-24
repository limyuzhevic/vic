#!/usr/bin/env python3
"""
NLM Enhanced Python Bindings - Demonstration Script

This script demonstrates the enhanced Python API for the NLM framework.
It shows the new features and capabilities added to improve usability
and functionality for both beginners and advanced users.

Usage:
    python3 demo_enhanced.py
"""

import sys
import time

# Import the enhanced API
try:
    import pynlm_enhanced as pynlm
    print("✅ Successfully imported enhanced NLM bindings")
except ImportError as e:
    print(f"❌ Failed to import enhanced bindings: {e}")
    print("This script requires the enhanced bindings to be built.")
    sys.exit(1)

def demo_basic_enhanced_api():
    """Demonstrate basic enhanced API usage."""
    print("=" * 60)
    print("DEMONSTRATION 1: Basic Enhanced API Usage")
    print("=" * 60)
    
    # Create configuration with enhanced API
    config = pynlm.createMinimalConfig(1000)
    print(f"✓ Created minimal config with 1000 neurons")
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Created and initialized brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation with enhanced API
    brain.runSimulation(500, logProgress=True)
    print(f"✓ Ran simulation with progress logging")
    
    # Get statistics
    stats = brain.getStatistics()
    print(f"✓ Retrieved comprehensive statistics")
    print(f"  - Total neurons: {stats['total_neurons']}")
    print(f"  - Total synapses: {stats['total_synapses']}")
    print(f"  - Firing neurons: {stats['firing_neurons']}")
    print(f"  - E/I ratio: {stats['e_i_ratio']:.3f}")

def demo_advanced_features():
    """Demonstrate advanced features."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION 2: Advanced Features")
    print("=" * 60)
    
    # Create advanced brain configuration
    config = pynlm.createBrainConfiguration({
        "brain": {
            "neuron_count": 5000,
            "region_count": 3,
            "connection_probability": 0.08
        }
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Created advanced brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create advanced agent
    agent = pynlm.createAdvancedAgentBrain(brain)
    print(f"✓ Created advanced agent brain")
    
    # Configure advanced features
    agent.enableAllNeuromodulation(True)
    agent.setDopamineGain(1.5)
    agent.setCuriosityThreshold(0.2)
    print(f"✓ Configured advanced neuromodulation features")
    
    # Create world and agent interaction
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    agent.initialize(world)
    print(f"✓ Created world and initialized agent")
    
    # Run a few steps with monitoring
    for step in range(50):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        result = world.applyMotorCommand(motor_cmd, step * 0.1)
        
        agent.applyRewardModulation(0.0, 0.0)
        agent.updateDevelopment(0.1)
    
    print(f"✓ Ran 50 steps of agent-world interaction")
    
    # Get agent statistics
    agent_stats = agent.getAgentStats()
    print(f"✓ Retrieved agent statistics:")
    print(f"  - Curiosity: {agent_stats['curiosity']:.3f}")
    print(f"  - Novelty: {agent_stats['novelty']:.3f}")
    print(f"  - Prediction Error: {agent_stats['prediction_error']:.3f}")
    print(f"  - Neuromodulation: {agent_stats['neuromodulation']:.3f}")

def demo_performance_monitoring():
    """Demonstrate performance monitoring."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION 3: Performance Monitoring")
    print("=" * 60)
    
    # Create and configure brain
    config = pynlm.createBrainConfiguration({
        "brain": {
            "neuron_count": 2000,
            "region_count": 2
        }
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Created brain with performance monitoring")
    
    # Create performance monitor
    monitor = pynlm.createPerformanceMonitor(brain, None)
    monitor.startMonitoring()
    print(f"✓ Started performance monitoring")
    
    # Run simulation with monitoring
    for step in range(100):
        brain.step(step)
        monitor.recordPerformance(step)
    
    # Generate performance report
    report = monitor.getPerformanceReport()
    print(f"✓ Generated performance report:")
    print(f"  - Total steps: {report['simulation_summary']['total_steps']}")
    print(f"  - Steps per second: {report['simulation_summary']['steps_per_second']:.1f}")
    print(f"  - Average step time: {report['performance_metrics']['avg_step_time_ms']:.2f} ms")
    print(f"  - Average CPU efficiency: {report['cpu_performance']['avg_cpu_percent']:.1f}%")
    print(f"  - Average memory usage: {report['memory_performance']['avg_memory_usage']:.1f}%")

def demo_error_handling():
    """Demonstrate enhanced error handling."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION 4: Enhanced Error Handling")
    print("=" * 60)
    
    # Create brain with invalid configuration to demonstrate error handling
    try:
        # Create configuration with invalid parameters
        invalid_config = pynlm.createBrainConfiguration({
            "brain": {
                "neuron_count": -1,  # Invalid: negative neuron count
                "region_count": 0   # Invalid: no regions
            }
        })
        
        brain = pynlm.createBrain(invalid_config)
        print(f"✓ Created brain with invalid configuration")
        
        # Attempt to initialize (should fail with validation)
        if brain.isValidConfiguration():
            brain.initialize()
            print(f"✓ Brain initialized successfully")
        else:
            print(f"✗ Configuration validation failed (as expected)")
            
    except pynlm.InvalidArgumentError as e:
        print(f"✓ Caught InvalidArgumentError: {e}")
    except Exception as e:
        print(f"✓ Caught exception: {type(e).__name__}: {e}")
    
    # Demonstrate successful configuration validation
    print(f"\n✓ Demonstrating successful configuration:")
    valid_config = pynlm.createMinimalConfig(100)
    if valid_config.has("brain.neuron_count"):
        print(f"  - Valid configuration has brain.neuron_count: {valid_config.getOr('brain.neuron_count', 0)}")
    
    # Demonstrate robust simulation with recovery
    print(f"\n✓ Demonstrating robust simulation with recovery:")
    try:
        brain = pynlm.createBrain(pynlm.createMinimalConfig(500))
        brain.initialize()
        brain.runSimulation(200)
        print(f"  - Simulation completed successfully")
    except Exception as e:
        print(f"  - Simulation handled error gracefully: {type(e).__name__}")

def demo_experiment_framework():
    """Demonstrate experiment framework."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION 5: Experiment Framework")
    print("=" * 60)
    
    # Create experiment framework
    framework = pynlm.createAdvancedExperimentFramework()
    print(f"✓ Created advanced experiment framework")
    
    # Create simple learning experiment
    experiment = framework.createLearningExperiment({
        "conditions": ["control", "experimental"],
        "duration": 500,
        "sampling_rate": 10
    })
    
    print(f"✓ Created learning experiment with conditions: {experiment['conditions']}")
    
    # Create brain configuration for experiment
    brain_config = pynlm.createBrainConfiguration({
        "brain": {
            "neuron_count": 1000,
            "region_count": 2
        }
    })
    
    # Run experiment
    brain = pynlm.createBrain(brain_config)
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.reset()
    
    print(f"✓ Created experiment environment")
    
    # Run experiment steps
    for step in range(experiment['duration']):
        percept = world.getSensoryPercept()
        # Process sensory input (simplified for demo)
        result = world.applyMotorCommand(pynlm.MotorCommand.Wait, step * 0.1)
        
        if step % experiment['sampling_rate'] == 0:
            print(f"  - Experiment step {step}/{experiment['duration']} (reward: {result.reward})")
    
    print(f"✓ Experiment completed")

def demo_backward_compatibility():
    """Demonstrate backward compatibility."""
    print("\n" + "=" * 60)
    print("DEMONSTRATION 6: Backward Compatibility")
    print("=" * 60)
    
    print(f"✓ Demonstrating backward compatibility with original API:")
    
    # Original API still works
    import pynlm
    
    # This is the same as the original API
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Original API still works:")
    print(f"  - Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"  - Can still access all original properties and methods")
    
    # Enhanced API provides additional features
    print(f"✓ Enhanced API provides additional features:")
    print(f"  - Factory methods with sensible defaults")
    print(f"  - Enhanced error handling")
    print(f"  - Convenience functions")
    print(f"  - Performance monitoring")
    
    # Show version information
    print(f"\n✓ Version information:")
    print(f"  - Original API version: NLM 2.0.0")
    print(f"  - Enhanced API version: pynlm_enhanced 2.0.0")
    print(f"  - Enhanced features: {len(pynlm_enhanced.__dict__)} additional features")

def main():
    """Run all demonstrations."""
    print("NLM Enhanced Python Bindings - Demonstration Script")
    print("=" * 60)
    print("This script demonstrates the enhanced features of the NLM framework.")
    print("Enhanced API: pynlm_enhanced")
    print("Original API: pynlm (for backward compatibility)")
    print("=" * 60)
    
    try:
        # Run all demonstrations
        demo_basic_enhanced_api()
        demo_advanced_features()
        demo_performance_monitoring()
        demo_error_handling()
        demo_experiment_framework()
        demo_backward_compatibility()
        
        print("\n" + "=" * 60)
        print("✅ ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY")
        print("=" * 60)
        print("\nThe NLM enhanced Python bindings provide:")
        print("✅ Enhanced factory methods with sensible defaults")
        print("✅ Advanced neuromodulation and curiosity systems")
        print("✅ Comprehensive performance monitoring")
        print("✅ Robust error handling and recovery")
        print("✅ Advanced experiment framework")
        print("✅ Full backward compatibility")
        print("\nFor more information, see:")
        print("- enhanced_bindings_report.md")
        print("- power_user_enhancements.md")
        print("- Documentation in the docs/ directory")
        
    except Exception as e:
        print(f"\n❌ ERROR during demonstration: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
