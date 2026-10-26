"""
NLM Enhanced Python Bindings - Example Usage

This file demonstrates the improved Python bindings for the NLM neural simulation framework.
It showcases the new features, APIs, and best practices.

The enhanced bindings provide:
- Better error handling and user-friendly APIs
- Comprehensive metrics collection and analysis
- Environment simulation integration
- Configuration management
- Command-line tools
- Advanced features for power users
"""

import sys
import os

# Add this directory to the path so we can import from the python subdirectory
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def example_basic_simulation():
    """Example 1: Basic brain simulation with enhanced bindings."""
    print("Example 1: Basic Brain Simulation")
    print("=" * 50)
    
    try:
        from pynlm_enhanced import BrainSimulator
        
        # Create and initialize brain simulator
        brain_sim = BrainSimulator()
        brain_sim.initialize()
        
        # Run a simulation
        print("Running simulation...")
        metrics = brain_sim.run(200)
        
        # Get simulation results
        summary = brain_sim.get_metrics_summary()
        
        print(f"✓ Simulation completed successfully")
        print(f"  - Total steps: {summary['total_steps']}")
        print(f"  - Maximum firing neurons: {summary['max_firing_neurons']}")
        print(f"  - Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
        print(f"  - Total spikes: {summary['total_spikes']}")
        
        return True
        
    except Exception as e:
        print(f"✗ Example failed: {e}")
        return False


def example_environment_interaction():
    """Example 2: Environment simulation with agent interaction."""
    print("\nExample 2: Environment Simulation with Agent")
    print("=" * 50)
    
    try:
        from pynlm_enhanced import BrainSimulator, NLMEnvironment
        
        # Create brain
        brain_sim = BrainSimulator()
        brain_sim.initialize()
        
        # Create and configure environment
        env = NLMEnvironment()
        env.configure(width=20, height=20, vision_width=8, vision_height=8)
        env.reset()
        
        # Set agent start position
        env.set_agent_start(10.0, 10.0)
        
        # Run simulation loop
        print("Running agent-environment simulation...")
        for step in range(100):
            # Update environment
            env_result = env.update(timestep=0.1)
            
            # Run brain step
            metrics = brain_sim.step(step)
            
            # You would typically decode motor commands here
            # action = brain_sim.decode_action(metrics)
            
            if step % 20 == 0:
                print(f"  Step {step}: {metrics.active_neurons} firing neurons")
        
        print("✓ Environment simulation completed")
        return True
        
    except Exception as e:
        print(f"✗ Example failed: {e}")
        return False


def example_configuration_management():
    """Example 3: Configuration management with enhanced bindings."""
    print("\nExample 3: Configuration Management")
    print("=" * 50)
    
    try:
        from pynlm_enhanced import NLMConfigurator
        
        # Create configurator
        config = NLMConfigurator()
        
        # Set configuration values
        print("Setting configuration values...")
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.12)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("neuromod.curiosity.enable", True)
        
        # Get configuration values
        neuron_count = config.get("brain.neuron_count")
        learning_rate = config.get("plasticity.stdp.learning_rate")
        
        print(f"✓ Configuration set successfully")
        print(f"  - Neuron count: {neuron_count}")
        print(f"  - Learning rate: {learning_rate}")
        
        # Show all configuration keys
        keys = config.config.getKeys()
        print(f"  - Configuration keys: {len(keys)}")
        
        return True
        
    except Exception as e:
        print(f"✗ Example failed: {e}")
        return False


def example_metrics_analysis():
    """Example 4: Metrics collection and analysis."""
    print("\nExample 4: Metrics Collection and Analysis")
    print("=" * 50)
    
    try:
        from pynlm_enhanced import BrainSimulator
        
        # Create brain with custom configuration
        brain_sim = BrainSimulator()
        brain_sim.initialize()
        
        # Run simulation with data collection
        print("Running simulation with metrics collection...")
        metrics = brain_sim.run(300)
        
        # Analyze metrics
        summary = brain_sim.get_metrics_summary()
        
        print("✓ Metrics analysis completed")
        print(f"  - Time range: {summary['step_range'][0]} to {summary['step_range'][1]} steps")
        print(f"  - Neuron activity range: {summary['min_firing_neurons']} to {summary['max_firing_neurons']} neurons")
        print(f"  - Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
        
        # Export metrics
        brain_sim.export_metrics("simulation_metrics.json", "json")
        print("✓ Metrics exported to simulation_metrics.json")
        
        # Clean up
        if os.path.exists("simulation_metrics.json"):
            os.remove("simulation_metrics.json")
        
        return True
        
    except Exception as e:
        print(f"✗ Example failed: {e}")
        return False


def example_error_handling():
    """Example 5: Robust error handling."""
    print("\nExample 5: Error Handling and Recovery")
    print("=" * 50)
    
    try:
        from pynlm_enhanced import BrainSimulator, NLMEnvironment, SimulationError
        
        # Test 1: Proper initialization
        print("Test 1: Proper initialization...")
        brain_sim = BrainSimulator()
        brain_sim.initialize()
        print("✓ Initialization successful")
        
        # Test 2: Simulation without error
        print("Test 2: Error-free simulation...")
        brain_sim.run(50)
        print("✓ Simulation completed without errors")
        
        # Test 3: Environment setup
        print("Test 3: Environment setup...")
        env = NLMEnvironment()
        env.configure(width=10, height=10)
        env.reset()
        print("✓ Environment setup successful")
        
        # Test 4: Metrics collection
        print("Test 4: Metrics collection...")
        for i in range(20):
            metrics = brain_sim.step(i)
            if metrics.active_neurons > 0:
                print(f"  - Step {i}: {metrics.active_neurons} neurons firing")
        print("✓ Metrics collection successful")
        
        print("✓ All error handling tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Example failed: {e}")
        return False


def example_convenience_functions():
    """Example 6: Using convenience functions."""
    print("\nExample 6: Convenience Functions")
    print("=" * 50)
    
    try:
        from pynlm_enhanced import create_brain, create_world, run_experiment
        
        # Use convenience functions
        print("Creating brain using convenience function...")
        brain = create_brain()
        print("✓ Brain created")
        
        print("Creating world using convenience function...")
        world = create_world(width=15, height=15)
        print("✓ World created")
        
        # Run complete experiment
        print("Running complete experiment...")
        results = run_experiment(
            brain_config=None,
            world_config={'width': 15, 'height': 15, 'vision_width': 6, 'vision_height': 6},
            num_steps=100
        )
        
        print("✓ Complete experiment completed")
        print(f"  - Steps recorded: {len(results['metrics'])}")
        print(f"  - Final firing neurons: {results['metrics'][-1].active_neurons}")
        
        return True
        
    except Exception as e:
        print(f"✗ Example failed: {e}")
        return False


def main():
    """Run all examples."""
    print("NLM Enhanced Python Bindings - Usage Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_basic_simulation,
        example_environment_interaction,
        example_configuration_management,
        example_metrics_analysis,
        example_error_handling,
        example_convenience_functions
    ]
    
    passed = 0
    failed = 0
    
    for example in examples:
        try:
            if example():
                passed += 1
            else:
                failed += 1
                print(f"  ✗ {example.__name__} failed")
        except Exception as e:
            failed += 1
            print(f"  ✗ {example.__name__} failed with exception: {e}")
        
        print()
    
    print("=" * 60)
    print(f"Example Summary: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n🎉 All examples completed successfully!")
        print("\nThe enhanced NLM Python bindings provide:")
        print("  • Better error handling and user-friendly APIs")
        print("  • Comprehensive metrics collection and analysis")
        print("  • Environment simulation integration")
        print("  • Configuration management tools")
        print("  • Command-line interface support")
        print("  • Advanced features for power users")
        return 0
    else:
        print(f"\n❌ {failed} examples failed")
        return 1


if __name__ == '__main__':
    sys.exit(main())