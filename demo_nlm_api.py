#!/usr/bin/env python3
"""
NLM Python API Demonstration Script

This script demonstrates the key features of the enhanced NLM Python API.
It shows how to use the new convenience methods and error handling.

Run this script to see the improved NLM Python API in action:

    python demo_nlm_api.py
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Check if we can import the enhanced NLM API
try:
    import nlm
    from nlm import NLMBrain, NLMWorld, NLMEnvironment, BrainStats
    API_AVAILABLE = True
    print("✓ NLM Python API is available")
    print(f"  Version: {nlm.get_version()}")
    print(f"  Author: {nlm.__author__}")
    print(f"  Description: {nlm.__description__}")
    print()
except ImportError as e:
    print(f"✗ NLM Python API not available: {e}")
    print("  Please install with: pip install -e .")
    sys.exit(1)

def demo_basic_brain():
    """Demonstrate basic brain operations."""
    print("=== Demo 1: Basic Brain Operations ===")
    
    try:
        # Create a brain with default configuration
        brain = nlm.create_brain()
        print(f"Created brain: {brain}")
        
        # Initialize the brain
        brain.initialize()
        print("Brain initialized successfully")
        
        # Get brain statistics
        stats = brain.get_stats()
        print(f"Brain stats: {stats}")
        print(f"Neurons: {stats.get_neuron_count()}")
        print(f"Current firing rate: {stats.get_average_firing_rate():.2f} Hz")
        
        # Run a simple simulation
        print("\nRunning 10-step simulation...")
        simulation_stats = brain.run_simulation(10)
        print(f"Simulation completed: {simulation_stats.get('success', 'Unknown')}")
        print(f"Total spikes generated: {simulation_stats.get('spikes_generated', 0)}")
        
        # Demonstrate convenience methods
        print("\nDemonstrating convenience methods:")
        print("  - Adding sensory stimulus...")
        brain.add_sensory_stimulus(0.5, 5)  # 50% stimulus for 5 steps
        
        print("  - Running burst simulation...")
        burst_stats = brain.run_burst_simulation(2, 3)  # 2 bursts with 3 steps each
        print(f"    Burst simulation completed with {burst_stats.get('spikes_generated', 0)} spikes")
        
        return True
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def demo_world_interaction():
    """Demonstrate agent-world interaction."""
    print("\n=== Demo 2: Agent-World Interaction ===")
    
    try:
        # Create brain, world, and agent
        brain = nlm.create_brain()
        world = nlm.create_world(width=10, height=10, vision_width=6, vision_height=6)
        agent = nlm.create_agent_brain(brain)
        
        print(f"Created components:")
        print(f"  Brain: {brain}")
        print(f"  World: {world}")
        print(f"  Agent: Available")
        
        # Initialize agent with world
        agent.initialize(world.world)
        print("\nAgent initialized with world")
        
        # Configure learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        print("Learning subsystems enabled")
        
        # Set agent starting position
        world.set_agent_start(5.0, 5.0)
        print("Agent starting position set to (5.0, 5.0)")
        
        # Create environment
        environment = nlm.create_environment(brain, world)
        print("\nEnvironment created")
        
        # Run a short episode
        print("Running 5-step episode...")
        episode_stats = environment.run_episode(max_steps=5, enable_learning=True)
        
        print(f"Episode completed: {episode_stats.get('status', 'Unknown')}")
        print(f"Total reward: {episode_stats.get('total_reward', 0):.2f}")
        print(f"Steps completed: {episode_stats.get('steps', 0)}")
        print(f"Average reward per step: {episode_stats.get('average_reward', 0):.3f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def demo_error_handling():
    """Demonstrate improved error handling."""
    print("\n=== Demo 3: Error Handling and Validation ===")
    
    try:
        print("Testing input validation...")
        
        # Create a brain
        brain = nlm.create_brain()
        
        # Test invalid input validation
        try:
            # This should fail - invalid number of steps
            brain.run_simulation(-5)
            print("✗ Validation should have caught negative steps")
            return False
        except nlm.ValidationError as e:
            print(f"✓ Validation correctly caught error: {e}")
        
        # Test invalid stimulus strength
        try:
            brain.add_sensory_stimulus(1.5)  # Should be 0.0-1.0
            print("✗ Validation should have caught invalid stimulus strength")
            return False
        except nlm.ValidationError as e:
            print(f"✓ Validation correctly caught error: {e}")
        
        # Test invalid burst parameters
        try:
            brain.run_burst_simulation(-1, 5)
            print("✗ Validation should have caught negative bursts")
            return False
        except nlm.ValidationError as e:
            print(f"✓ Validation correctly caught error: {e}")
        
        print("\n✓ All error handling tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def demo_advanced_features():
    """Demonstrate advanced features."""
    print("\n=== Demo 4: Advanced Features ===")
    
    try:
        # Create a more complex setup
        brain = nlm.create_brain()
        world = nlm.create_world(width=15, height=15)
        environment = nlm.create_environment(brain, world)
        
        print("Demonstrating advanced agent training...")
        
        # Enable all learning subsystems
        environment._agent_brain.enableRewardModulation(True)
        environment._agent_brain.enableCuriosity(True)
        environment._agent_brain.enableDevelopment(True)
        
        # Run multiple episodes
        for episode in range(3):
            print(f"\nEpisode {episode + 1}/3...")
            
            # Reset for new episode
            world.reset()
            brain.reset()
            environment._agent_brain.reset()
            environment._agent_brain.initialize(world.world)
            world.set_agent_start(7.5, 7.5)
            
            # Run episode
            episode_stats = environment.run_episode(max_steps=20, enable_learning=True)
            
            print(f"  Status: {episode_stats.get('status', 'Unknown')}")
            print(f"  Total reward: {episode_stats.get('total_reward', 0):.2f}")
            print(f"  Steps: {episode_stats.get('steps', 0)}")
            print(f"  Final curiosity: {episode_stats.get('final_curiosity', 0):.3f}")
            print(f"  Final novelty: {episode_stats.get('final_novelty', 0):.3f}")
        
        print("\n✓ Advanced features demonstration completed")
        return True
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def demo_package_info():
    """Display package information."""
    print("\n=== Demo 5: Package Information ===")
    
    try:
        # Get package info
        info = nlm.get_info()
        
        print(f"Package Information:")
        print(f"  Name: {info['name']}")
        print(f"  Version: {info['version']}")
        print(f"  Author: {info['author']}")
        print(f"  Description: {info['description']}")
        print(f"  License: {info['license']}")
        
        print(f"\nFeatures:")
        for feature, available in info['features'].items():
            status = "✓" if available else "✗"
            print(f"  {status} {feature.replace('_', ' ').title()}")
        
        print(f"\nDefault Configuration:")
        for key, value in info['default_config'].items():
            print(f"  {key}: {value}")
        
        # Check dependencies
        print(f"\nChecking dependencies...")
        deps = nlm.check_dependencies()
        for dep, info in deps.items():
            if info['available']:
                print(f"  ✓ {dep}: {info['version']}")
            else:
                print(f"  ✗ {dep}: Not available - {info['error']}")
        
        return True
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main demonstration function."""
    print("=" * 60)
    print("NLM Python API Demonstration")
    print("=" * 60)
    
    # Show welcome message
    nlm.print_welcome_message()
    
    # Run demonstrations
    demos = [
        ("Basic Brain Operations", demo_basic_brain),
        ("Agent-World Interaction", demo_world_interaction),
        ("Error Handling and Validation", demo_error_handling),
        ("Advanced Features", demo_advanced_features),
        ("Package Information", demo_package_info),
    ]
    
    results = []
    for name, demo_func in demos:
        print(f"\n{'=' * 60}")
        print(f"Running: {name}")
        print(f"{'=' * 60}")
        
        success = demo_func()
        results.append((name, success))
    
    # Summary
    print(f"\n{'=' * 60}")
    print("DEMONSTRATION SUMMARY")
    print(f"{'=' * 60}")
    
    passed = 0
    for name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{name:40} {status}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{len(results)} demonstrations passed")
    
    if passed == len(results):
        print("\n🎉 All demonstrations completed successfully!")
        print("\nThe enhanced NLM Python API provides:")
        print("  • User-friendly convenience methods")
        print("  • Comprehensive error handling and validation")
        print("  • Advanced agent training capabilities")
        print("  • Performance monitoring and statistics")
        print("  • Easy-to-use examples and tutorials")
        return True
    else:
        print(f"\n⚠ {len(results) - passed} demonstrations failed")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)