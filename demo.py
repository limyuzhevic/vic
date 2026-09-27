#!/usr/bin/env python3
"""
Enhanced NLM Python API Demo

This demo script showcases all the improvements made to the NLM Python bindings:
1. Comprehensive error handling with custom exceptions
2. Type safety and parameter validation
3. Consistent API design with detailed documentation
4. Developer tools (DebugMonitor)
5. Convenience functions for common tasks
6. Performance monitoring and analysis

The demo is designed to be educational and demonstrate best practices
for using the enhanced NLM Python API.
"""

import pynlm
import tempfile
import os
import sys

def print_header(title):
    """Print a formatted header."""
    print("\n" + "="*60)
    print(f"  {title}")
    print("="*60)

def demo_1_basic_usage():
    """Demo 1: Basic usage with enhanced error handling."""
    print_header("Demo 1: Basic Usage with Enhanced Error Handling")
    
    try:
        print("Creating brain with default configuration...")
        config = pynlm.createDefaultConfig()
        print(f"✓ Configuration created: {config}")
        
        print("\nCreating brain...")
        brain = pynlm.createBrain(config)
        print(f"✓ Brain created successfully")
        
        print("\nInitializing brain...")
        if brain.initialize():
            print("✓ Brain initialized successfully")
        else:
            print("⚠ Warning: Brain initialization may have issues")
        
        print("\nRunning simulation...")
        initial_spikes = brain.getTotalSpikeCount()
        
        for step in range(5):
            brain.step(step)
            current_spikes = brain.getTotalSpikeCount()
            print(f"  Step {step}: {current_spikes - initial_spikes} new spikes "
                  f"(Total: {current_spikes})")
        
        print("\nBrain Statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
    except pynlm.NLMException as e:
        print(f"✗ NLM Error: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    return True

def demo_2_error_handling():
    """Demo 2: Enhanced error handling with custom exceptions."""
    print_header("Demo 2: Enhanced Error Handling")
    
    try:
        print("Testing parameter validation...")
        config = pynlm.createDefaultConfig()
        
        # Test setting various parameter types
        print("  Setting various configuration values...")
        config.set("string_param", "test_value")
        config.set("int_param", 42)
        config.set("float_param", 3.14)
        config.set("bool_param", True)
        
        print("  ✓ Configuration values set successfully")
        
        # Test getting values with defaults
        print("\n  Testing value retrieval...")
        print(f"    String: {config.getOr('string_param', 'default')}")
        print(f"    Integer: {config.getOr('int_param', 0)}")
        print(f"    Float: {config.getOr('float_param', 0.0)}")
        print(f"    Boolean: {config.getOr('bool_param', False)}")
        print(f"    Non-existent: {config.getOr('nonexistent', 'default_value')}")
        
        print("\n✓ Parameter validation completed")
        
    except pynlm.InvalidParameterError as e:
        print(f"✗ Invalid parameter error: {e}")
        return False
    except Exception as e:
        print(f"✗ Error in error handling demo: {e}")
        return False
    
    return True

def demo_3_agent_world_simulation():
    """Demo 3: Complete agent-world simulation."""
    print_header("Demo 3: Agent-World Simulation")
    
    try:
        print("Creating simulation components...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain created and initialized")
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        print(f"✓ World configured ({world.getWidth()}x{world.getHeight()} cells)")
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("✓ Agent created and initialized")
        
        # Enable advanced features
        print("\nEnabling advanced features...")
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        print("  ✓ Reward modulation enabled")
        print("  ✓ Curiosity enabled")
        print("  ✓ Development enabled")
        
        # Run simulation
        print("\nRunning simulation...")
        for step in range(15):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 5 == 0:
                print(f"  Step {step}:")
                print(f"    Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
                print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"    Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
                print(f"    Development: {agent.getDevelopmentalStage()}")
        
        print(f"\n✓ Simulation completed successfully")
        print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
        
    except Exception as e:
        print(f"✗ Error in agent-world simulation: {e}")
        return False
    
    return True

def demo_4_type_safety():
    """Demo 4: Type safety and validation."""
    print_header("Demo 4: Type Safety and Validation")
    
    try:
        print("Testing type safety...")
        
        # Test NeuronId
        from pynlm import NeuronId
        neuron_id = NeuronId(1)
        print(f"✓ Created NeuronId: {neuron_id}")
        print(f"  Value: {neuron_id.value}")
        print(f"  Index: {neuron_id.index()}")
        
        # Test equality and hash
        neuron_id2 = NeuronId(1)
        print(f"✓ Equality test: {neuron_id == neuron_id2}")
        print(f"✓ Hash test: {hash(neuron_id) == hash(neuron_id2)}")
        
        # Test sensory inputs
        print("\nTesting sensory input types...")
        vision = pynlm.Vision(10, 10, 3)
        print(f"✓ Vision created: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
        
        audio = pynlm.Audio(44100, 1024)
        print(f"✓ Audio created: {audio.getSampleRate()}Hz, {audio.getNumSamples()} samples")
        
        # Test action types
        print("\nTesting action types...")
        move_action = pynlm.Action(pynlm.ActionType.MoveForward)
        print(f"✓ Move action: {move_action.getName()}")
        
        custom_action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        print(f"✓ Custom action: {custom_action.getType()}, params: {custom_action.getParameters()}")
        
        # Test world objects
        print("\nTesting world object types...")
        resource = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource)
        print(f"✓ Resource object: type={resource.type}, position=({resource.x}, {resource.y})")
        
        print("\n✓ Type safety tests completed")
        
    except Exception as e:
        print(f"✗ Error in type safety demo: {e}")
        return False
    
    return True

def demo_5_debug_monitoring():
    """Demo 5: Debug monitoring and analysis."""
    print_header("Demo 5: Debug Monitoring and Analysis")
    
    try:
        print("Creating debug monitor...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        monitor = pynlm.DebugMonitor(brain, world, agent)
        print("✓ Debug monitor created")
        
        # Capture system state
        print("\nCapturing system state...")
        monitor.capture_state("Initial state")
        print("✓ Initial state captured")
        
        # Run simulation with monitoring
        for step in range(10):
            world.update(0.1)
            monitor.capture_state(f"Step {step}")
            
            # Process input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step == 4:  # Middle of simulation
                print(f"✓ Captured state at step {step}")
        
        # Get stability report
        print("\nAnalyzing system stability...")
        report = monitor.get_stability_report()
        print(f"✓ Stability analysis completed")
        print(f"  Stable: {report['stable']}")
        print(f"  Total captures: {report['total_captures']}")
        if report['issues']:
            print(f"  Issues: {report['issues']}")
        if report['warnings']:
            print(f"  Warnings: {report['warnings']}")
        
        # Export history (optional - requires write permissions)
        print("\nExporting history...")
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            history_file = f.name
        
        monitor.export_history(history_file)
        print(f"✓ History exported to: {history_file}")
        
        # Clean up
        os.unlink(history_file)
        
    except Exception as e:
        print(f"✗ Error in debug monitoring demo: {e}")
        return False
    
    return True

def demo_6_file_operations():
    """Demo 6: File operations with error handling."""
    print_header("Demo 6: File Operations with Error Handling")
    
    try:
        print("Testing configuration file operations...")
        
        # Create and save configuration
        config = pynlm.createDefaultConfig()
        config.set("test_param_1", "test_value")
        config.set("test_param_2", 123)
        config.set("test_param_3", True)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        print(f"✓ Configuration file: {config_file}")
        
        # Save configuration
        if config.saveToFile(config_file):
            print("✓ Configuration saved successfully")
        else:
            print("✗ Failed to save configuration")
            return False
        
        # Load configuration
        config2 = pynlm.createDefaultConfig()
        if config2.loadFromFile(config_file):
            print("✓ Configuration loaded successfully")
            
            # Verify loaded values
            assert config2.getOr("test_param_1", "") == "test_value"
            assert config2.getOr("test_param_2", 0) == 123
            assert config2.getOr("test_param_3", False) is True
            print("✓ Loaded values verified")
        else:
            print("✗ Failed to load configuration")
            return False
        
        # Test brain checkpoint
        print("\nTesting brain checkpoint operations...")
        
        brain = pynlm.createBrain(config2)
        brain.initialize()
        
        with tempfile.NamedTemporaryFile(suffix='.checkpoint', delete=False) as f:
            brain_file = f.name
        
        print(f"✓ Brain checkpoint file: {brain_file}")
        
        # Save brain
        if brain.save(brain_file):
            print("✓ Brain saved successfully")
        else:
            print("✗ Failed to save brain")
            return False
        
        # Load brain
        config3 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config3)
        
        if brain2.load(brain_file):
            print("✓ Brain loaded successfully")
            
            # Verify brain state
            assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert brain2.getTotalSynapseCount() == brain.getTotalSynapseCount()
            print("✓ Brain state verified")
        else:
            print("✗ Failed to load brain")
            return False
        
        # Clean up
        os.unlink(config_file)
        os.unlink(brain_file)
        
        print("\n✓ All file operations completed successfully")
        
    except Exception as e:
        print(f"✗ Error in file operations demo: {e}")
        return False
    
    return True

def demo_7_performance_monitoring():
    """Demo 7: Performance monitoring and analysis."""
    print_header("Demo 7: Performance Monitoring and Analysis")
    
    try:
        print("Running performance benchmark...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        steps = 100
        print(f"Running {steps} simulation steps...")
        
        for step in range(steps):
            brain.step(step)
            
            if step % 25 == 0:
                print(f"  Step {step}: {brain.getTotalSpikeCount()} total spikes")
        
        # Performance analysis
        print("\nPerformance Analysis:")
        print(f"  Steps completed: {steps}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average spikes per step: {brain.getTotalSpikeCount() / steps:.2f}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        
        # Check for potential issues
        if brain.getAverageFiringRate() > 10.0:
            print(f"  ⚠ Warning: High firing rate ({brain.getAverageFiringRate():.3f})")
        
        if brain.getExcitationInhibitionRatio() > 5.0:
            print(f"  ⚠ Warning: High E/I ratio ({brain.getExcitationInhibitionRatio():.3f})")
        
        print("\n✓ Performance monitoring completed")
        
    except Exception as e:
        print(f"✗ Error in performance monitoring demo: {e}")
        return False
    
    return True

def demo_8_advanced_features():
    """Demo 8: Advanced features and functionality."""
    print_header("Demo 8: Advanced Features")
    
    try:
        print("Demonstrating advanced features...")
        
        # Create brain with development
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable all advanced features
        print("Enabling advanced brain features...")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        print("✓ All advanced features enabled")
        
        # Run advanced simulation
        print("\nRunning advanced simulation...")
        for step in range(20):
            world.update(0.1)
            
            # Process with full features
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Advanced decision making
            action = agent.decodeMotorCommand()
            
            # Apply with neuromodulation
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 5 == 0:
                print(f"  Step {step}:")
                print(f"    Stage: {brain.getDevelopmentalStage()}")
                print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"    Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
                print(f"    Prediction error: {agent.getPredictionError():.3f}")
                print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
        
        print("\n✓ Advanced features demonstration completed")
        
    except Exception as e:
        print(f"✗ Error in advanced features demo: {e}")
        return False
    
    return True

def demo_9_convenience_functions():
    """Demo 9: Convenience functions and utilities."""
    print_header("Demo 9: Convenience Functions and Utilities")
    
    try:
        print("Testing convenience functions...")
        
        # Test create_brain_with_validation
        print("Testing create_brain_with_validation...")
        brain = pynlm.create_brain_with_validation(
            random_seed=42,
            simulation_timestep=0.01
        )
        print(f"✓ Brain created with validation: {brain.getTotalNeuronCount()} neurons")
        
        # Test run_simulation
        print("\nTesting run_simulation...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation with convenience function
        results = pynlm.run_simulation(
            brain=brain,
            world=world,
            agent=agent,
            num_steps=10
        )
        
        print("✓ Simulation results:")
        for key, value in results.items():
            print(f"    {key}: {value}")
        
        print("\n✓ Convenience functions tested successfully")
        
    except Exception as e:
        print(f"✗ Error in convenience functions demo: {e}")
        return False
    
    return True

def main():
    """Main demo function."""
    print("Enhanced NLM Python API Demo")
    print("=" * 60)
    print("This demo showcases all improvements made to the NLM Python bindings:")
    print("• Comprehensive error handling with custom exceptions")
    print("• Type safety and parameter validation")
    print("• Consistent API design with documentation")
    print("• Developer tools (DebugMonitor)")
    print("• Convenience functions")
    print("• Performance monitoring")
    print("=" * 60)
    
    results = []
    
    # Run all demos
    demos = [
        ("Basic Usage", demo_1_basic_usage),
        ("Error Handling", demo_2_error_handling),
        ("Agent-World Simulation", demo_3_agent_world_simulation),
        ("Type Safety", demo_4_type_safety),
        ("Debug Monitoring", demo_5_debug_monitoring),
        ("File Operations", demo_6_file_operations),
        ("Performance Monitoring", demo_7_performance_monitoring),
        ("Advanced Features", demo_8_advanced_features),
        ("Convenience Functions", demo_9_convenience_functions),
    ]
    
    for demo_name, demo_func in demos:
        print(f"\n{'='*60}")
        print(f"Running: {demo_name}")
        print('='*60)
        
        if demo_func():
            results.append((demo_name, True))
            print(f"✓ {demo_name}: PASSED")
        else:
            results.append((demo_name, False))
            print(f"✗ {demo_name}: FAILED")
    
    # Summary
    print("\n" + "="*60)
    print("DEMO SUMMARY")
    print("="*60)
    
    passed = 0
    failed = 0
    
    for demo_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{demo_name}: {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print("="*60)
    print(f"Total: {passed + failed} demos")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All demos completed successfully!")
        print("\nThe enhanced NLM Python API provides:")
        print("  • Robust error handling with custom exceptions")
        print("  • Type safety and parameter validation")
        print("  • Comprehensive documentation and examples")
        print("  • Developer tools for debugging and analysis")
        print("  • Convenience functions for common tasks")
        print("  • Performance monitoring and optimization")
        return 0
    else:
        print(f"\n⚠️  {failed} demo(s) failed. Check output for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
