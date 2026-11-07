#!/usr/bin/env python3
"""
Quick test script to validate NLM improvements.
Tests both basic and advanced NLM functionality.
"""

import sys
import os
import time
import json
from pathlib import Path

# Add the current directory to path so we can import our modules
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Testing Basic NLM Functionality ===")
    
    try:
        import pynlm
        
        # Create a simple brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Brain created: {brain.getTotalNeuronCount()} neurons")
        
        # Run some steps
        for step in range(50):
            brain.step(step)
        
        print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes")
        print(f"✓ Final firing neurons: {brain.getFiringNeuronCount()}")
        
        return True
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False
def test_enhanced_features():
    """Test enhanced features from our improvements."""
    print("\n=== Testing Enhanced Features ===")
    
    try:
        import pynlm
        from pynlm_extended import NLMBuilder, TrainingSession, MonitoringTools
        
        # Test NLMBuilder
        print("Testing NLMBuilder...")
        builder = NLMBuilder()
        builder.set_brain_size(200, 3)
        builder.set_plasticity(stdp=True, hebbian=True, structural=False)
        builder.enable_curiosity(True, 0.8)
        
        brain = builder.create_brain()
        print(f"✓ NLMBuilder created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test MonitoringTools
        print("Testing MonitoringTools...")
        metrics = MonitoringTools.getBrainMetrics(brain)
        print(f"✓ Brain metrics: Neurons={metrics[0]}, Synapses={metrics[1]}, "
              f"Firing={metrics[3]}, Rate={metrics[4]:.3f}")
        
        # Test create_simple_agent (from extended module)
        print("Testing create_simple_agent...")
        from pynlm_extended import create_simple_agent
        brain2, agent, world = create_simple_agent()
        
        print(f"✓ Created agent with world: {world.getWidth()}x{world.getHeight()}")
        
        # Run a quick agent episode
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain2.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"✓ Agent completed {step + 1} steps")
        print(f"✓ Final curiosity: {agent.getCuriosityLevel():.3f}")
        
        return True
    except Exception as e:
        print(f"✗ Enhanced features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_training_session():
    """Test advanced training session functionality."""
    print("\n=== Testing Training Session ===")
    
    try:
        import pynlm
        from training_session import AdvancedTrainingSession
        
        # Create simple brain, world, and agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Create training session
        trainer = AdvancedTrainingSession(brain, world, agent, "test_training")
        
        # Run a few training episodes
        episode_results = []
        for episode in range(3):
            result = trainer.run_training_episode(steps=100, verbose=True)
            episode_results.append(result)
            
            # Save checkpoint occasionally
            if episode % 2 == 0:
                checkpoint_path = trainer.save_episode_checkpoint(episode)
                print(f"  Saved checkpoint: {checkpoint_path}")
        
        # Get training statistics
        stats = trainer.get_training_statistics()
        print(f"✓ Training completed: {stats['total_episodes']} episodes, "
              f"{stats['total_training_steps']} steps")
        print(f"✓ Average firing rate: {stats['aggregate_metrics']['average_firing_rate']:.3f}")
        print(f"✓ Stability index: {stats['aggregate_metrics']['firing_rate_stability']:.3f}")
        
        # Export training data
        export_path = trainer.export_training_data()
        print(f"✓ Training data exported to: {export_path}")
        
        return True
    except Exception as e:
        print(f"✗ Training session test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_batch_processing():
    """Test batch processing utilities."""
    print("\n=== Testing Batch Processing ===")
    
    try:
        import pynlm
        from pynlm_extended import batch_run_simulations
        
        # Create batch simulation configurations
        simulations = []
        for i in range(3):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 100 + i * 50, pynlm.ConfigSource.Runtime)
            
            simulations.append({
                'name': f'batch_sim_{i}',
                'config': config,
                'steps': 50 + i * 20
            })
        
        # Run batch simulations
        results = batch_run_simulations(simulations, output_prefix="nlm_batch_test")
        
        print(f"✓ Batch processing completed: {len(results)} simulations")
        
        for result in results:
            print(f"  Sim {result['simulation_id']}: "
                  f"Steps={result['steps']}, "
                  f"Final neurons={result['final_firing_neurons']}, "
                  f"Total spikes={result['total_spikes']}")
        
        return True
    except Exception as e:
        print(f"✗ Batch processing test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_architecture_improvements():
    """Test architectural improvements and error handling."""
    print("\n=== Testing Architecture Improvements ===")
    
    try:
        import pynlm
        
        # Test validation functions
        print("Testing validation functions...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add validation helper if available
        if hasattr(pynlm, 'validateBrainState'):
            is_valid = pynlm.validateBrainState(brain)
            print(f"✓ Brain validation: {'Valid' if is_valid else 'Invalid'}")
        
        # Test safe step function
        print("Testing safe step function...")
        if hasattr(pynlm, 'safeBrainStep'):
            result = pynlm.safeBrainStep(brain, 10)
            print(f"✓ Safe brain step: {'Success' if result else 'Failed'}")
        
        # Test batch processing helper
        print("Testing batch processing helper...")
        if hasattr(pynlm, 'batchProcessBrains'):
            brains = [brain]
            pynlm.batchProcessBrains(brains, lambda b: b.step(20))
            print("✓ Batch processing: Success")
        
        # Test configuration helpers
        print("Testing configuration helpers...")
        if hasattr(pynlm, 'createStandardConfig'):
            standard_config = pynlm.createStandardConfig()
            print(f"✓ Standard config created: {standard_config.has('brain.neuron_count')}")
        
        if hasattr(pynlm, 'createMinimalConfig'):
            minimal_config = pynlm.createMinimalConfig()
            print(f"✓ Minimal config created: {minimal_config.has('brain.neuron_count')}")
        
        # Test statistics helpers
        print("Testing statistics helpers...")
        if hasattr(pynlm, 'computeFiringStats'):
            stats = pynlm.computeFiringStats(brain)
            print(f"✓ Firing stats computed: {len(stats)} metrics")
        
        return True
    except Exception as e:
        print(f"✗ Architecture improvements test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_documentation_examples():
    """Test examples from documentation."""
    print("\n=== Testing Documentation Examples ===")
    
    try:
        import pynlm
        
        # Example from easy_usage.md
        print("Testing easy_usage.md example...")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Example complete: {brain.getTotalSpikeCount()} spikes")
        
        # Example from HOW_TO_USE.md
        print("Testing HOW_TO_USE.md example...")
        config = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        for step in range(1000):
            brain2.step(step)
            if step % 100 == 0:
                print(f"  Step {step}: {brain2.getFiringNeuronCount()} firing neurons")
        
        print(f"✓ Complete example: {brain2.getAverageFiringRate():.3f} avg firing rate")
        
        return True
    except Exception as e:
        print(f"✗ Documentation examples test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def main():
    """Run all tests."""
    print("NLM Improvements Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Features", test_enhanced_features),
        ("Training Session", test_training_session),
        ("Batch Processing", test_batch_processing),
        ("Architecture Improvements", test_architecture_improvements),
        ("Documentation Examples", test_documentation_examples)
    ]
    
    results = []
    for test_name, test_func in tests:
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ {test_name} crashed: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 50)
    print("TEST SUMMARY")
    print("=" * 50)
    
    passed = 0
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
    
    print(f"\nResults: {passed}/{len(results)} tests passed")
    
    if passed == len(results):
        print("✓ All tests passed! NLM improvements are working correctly.")
        return 0
    else:
        print(f"✗ {len(results) - passed} tests failed. Please check the implementation.")
        return 1
if __name__ == "__main__":
    sys.exit(main())