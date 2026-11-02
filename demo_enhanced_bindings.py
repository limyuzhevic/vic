#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the new Python-specific convenience methods
and enhanced features of the NLM Python bindings.
"""

import sys
import os

# Try to import pynlm
try:
    import pynlm
    print("✅ SUCCESS: Imported pynlm module")
except ImportError as e:
    print(f"❌ FAILED: Could not import pynlm: {e}")
    print("\nPlease ensure the Python bindings are built:")
    print("  mkdir build && cd build")
    print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("  make -j4")
    sys.exit(1)

def demo_config_enhancements():
    """Demonstrate enhanced configuration features."""
    print("\n" + "="*60)
    print("📋 DEMO: Enhanced Configuration Features")
    print("="*60)
    
    # Create default configuration
    print("\n1. Creating default configuration...")
    config = pynlm.createDefaultConfig()
    print("   ✅ Created default config")
    
    # Update from Python dictionary
    print("\n2. Updating configuration from Python dictionary...")
    config_data = {
        "int_value": 42,
        "double_value": 3.14159,
        "string_value": "Hello, NLM!",
        "bool_value": True,
        "list_int": [1, 2, 3, 4, 5],
        "list_float": [1.1, 2.2, 3.3],
        "list_string": ["neuron", "brain", "simulation"]
    }
    
    config.updateFromDict(config_data)
    print(f"   ✅ Updated config with {len(config_data)} key-value pairs")
    
    # Test validation
    print("\n3. Testing configuration validation...")
    try:
        config.validate()
        print("   ✅ Configuration validation passed")
    except pynlm.ValidationError as e:
        print(f"   ❌ Validation error: {e}")
        return False
    
    # Get configuration schema
    print("\n4. Getting configuration schema...")
    schema = config.getSchema()
    print("   ✅ Configuration schema:")
    for key, description in schema.items():
        print(f"      • {key}: {description}")
    
    # Convert to Python dictionary
    print("\n5. Converting configuration to Python dictionary...")
    config_dict = config.toDict()
    print(f"   ✅ Converted to dictionary with {len(config_dict)} keys")
    print("   Sample keys:", list(config_dict.keys())[:3])
    
    return True

def demo_brain_convenience():
    """Demonstrate enhanced brain convenience methods."""
    print("\n" + "="*60)
    print("🧠 DEMO: Enhanced Brain Convenience Methods")
    print("="*60)
    
    # Create configuration and brain
    print("\n1. Creating configuration and brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("   ✅ Created and initialized brain")
    
    # Get developmental stage
    print("\n2. Getting developmental stage...")
    dev_stage = brain.getDevelopmentStage()
    print(f"   ✅ Current developmental stage: {dev_stage}")
    
    # Enable development system
    print("\n3. Enabling development system...")
    brain.enableDevelopment(True)
    print("   ✅ Development system enabled")
    
    # Set developmental stage
    print("\n4. Setting developmental stage...")
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    print("   ✅ Set developmental stage to Maturation")
    
    # Get learned preference
    print("\n5. Getting learned preference...")
    preference = brain.getLearnedPreference()
    print(f"   ✅ Learned preference: {preference}")
    
    # Set learned preference
    print("\n6. Setting learned preference...")
    brain.setLearnedPreference(0.75)
    print("   ✅ Set learned preference to 0.75")
    
    # Run simulation episode
    print("\n7. Running simulation episode...")
    steps_completed = brain.runEpisode(5)
    print(f"   ✅ Completed {steps_completed} simulation steps")
    
    # Get brain statistics
    print("\n8. Getting brain statistics...")
    stats = brain.getEpisodeStats()
    print("   ✅ Brain statistics:")
    for key, value in stats.items():
        print(f"      • {key}: {value}")
    
    # Reset for new episode
    print("\n9. Resetting brain for new episode...")
    brain.resetForNewEpisode()
    print("   ✅ Brain reset for new episode")
    
    return True

def demo_error_handling():
    """Demonstrate enhanced error handling."""
    print("\n" + "="*60)
    print("⚠️  DEMO: Enhanced Error Handling")
    print("="*60)
    
    # Test validation error
    print("\n1. Testing validation error handling...")
    config = pynlm.createDefaultConfig()
    
    try:
        config.validate()
        print("   ✅ Configuration validation passed (as expected)")
    except pynlm.ValidationError as e:
        print(f"   ✅ Caught validation error: {type(e).__name__}: {e}")
    
    # Test custom exception types
    print("\n2. Testing custom exception types...")
    
    exceptions_to_test = [
        ("NLMRuntimeError", pynlm.NLMRuntimeError, "Test runtime error"),
        ("NLMValidationError", pynlm.NLMValidationError, "Test validation error"),
        ("NLMImportError", pynlm.NLMImportError, "Test import error"),
        ("NLMMemoryError", pynlm.NLMMemoryError, "Test memory error"),
        ("NLMDevelopmentError", pynlm.NLMDevelopmentError, "Test development error"),
    ]
    
    for name, exc_type, message in exceptions_to_test:
        try:
            raise exc_type(message)
        except exc_type as e:
            print(f"   ✅ Caught {name}: {e}")
    
    return True

def demo_simulation_callbacks():
    """Demonstrate simulation callbacks."""
    print("\n" + "="*60)
    print("🔄 DEMO: Simulation Callbacks")
    print("="*60)
    
    # Create brain
    print("\n1. Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("   ✅ Created and initialized brain")
    
    # Set up callback tracking
    print("\n2. Setting up callback tracking...")
    callback_count = {"pre": 0, "post": 0}
    
    def pre_step_callback(step):
        callback_count["pre"] += 1
        print(f"      🔄 Pre-step {step} called (total: {callback_count['pre']})")
    
    def post_step_callback(step, stats):
        callback_count["post"] += 1
        print(f"      📊 Post-step {step} called - Active neurons: {stats['active_neurons']} (total: {callback_count['post']})")
    
    # Enable callbacks
    print("\n3. Enabling simulation callbacks...")
    brain.enableCallbacks(pre_step_callback, post_step_callback)
    print("   ✅ Callbacks enabled")
    
    # Run simulation with callbacks
    print("\n4. Running simulation with callbacks...")
    brain.runEpisode(3)
    print(f"   ✅ Simulation completed")
    print(f"   📈 Callback summary:")
    print(f"      • Pre-steps: {callback_count['pre']}")
    print(f"      • Post-steps: {callback_count['post']}")
    
    return True

def demo_performance_features():
    """Demonstrate performance monitoring features."""
    print("\n" + "="*60)
    print("⚡ DEMO: Performance Monitoring Features")
    print("="*60)
    
    # Create brain and enable performance monitoring
    print("\n1. Creating brain and enabling performance monitoring...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.enablePerformanceMonitoring()
    print("   ✅ Performance monitoring enabled")
    
    # Run simulation
    print("\n2. Running simulation...")
    for i in range(10):
        brain.step(i)
    
    print("   ✅ Simulation completed")
    
    # Get performance statistics
    print("\n3. Getting performance statistics...")
    perf_stats = brain.getPerformanceStats()
    print("   ✅ Performance statistics:")
    for key, value in perf_stats.items():
        print(f"      • {key}: {value}")
    
    # Enable visualization
    print("\n4. Enabling visualization...")
    brain.enableVisualization()
    print("   ✅ Visualization enabled")
    
    # Export for visualization
    print("\n5. Exporting data for visualization...")
    viz_data = brain.exportForVisualization()
    print("   ✅ Exported visualization data")
    print(f"   📊 Data contains:")
    print(f"      • brain_state: {'brain_state' in viz_data}")
    print(f"      • timestamp: {viz_data.get('timestamp', 'N/A')}")
    
    return True

def main():
    """Run all demonstrations."""
    print("🚀 NLM Python Bindings - Enhanced Features Demonstration")
    print("="*70)
    print("\nThis demonstration showcases the new Python-specific convenience")
    print("methods and enhanced features added to the NLM Python bindings.")
    print("\nThe enhancements make it easier to:")
    print("  • Set up and configure NLM simulations")
    print("  • Run simulations with callbacks and monitoring")
    print("  • Handle errors with specific exception types")
    print("  • Access advanced development system features")
    print("  • Export data for analysis and visualization")
    
    # Run demonstrations
    demos = [
        ("Configuration Enhancements", demo_config_enhancements),
        ("Brain Convenience Methods", demo_brain_convenience),
        ("Error Handling", demo_error_handling),
        ("Simulation Callbacks", demo_simulation_callbacks),
        ("Performance Features", demo_performance_features),
    ]
    
    passed = 0
    failed = 0
    
    for demo_name, demo_func in demos:
        try:
            if demo_func():
                print(f"\n✅ {demo_name}: SUCCESS")
                passed += 1
            else:
                print(f"\n❌ {demo_name}: FAILED")
                failed += 1
        except Exception as e:
            print(f"\n❌ {demo_name}: ERROR - {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    # Summary
    print("\n" + "="*70)
    print("📊 DEMONSTRATION SUMMARY")
    print("="*70)
    print(f"✅ Successful demonstrations: {passed}")
    print(f"❌ Failed demonstrations: {failed}")
    print(f"📈 Total demonstrations: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY!")
        print("\nThe enhanced NLM Python bindings are working perfectly!")
        print("\nKey features demonstrated:")
        print("  • Python-specific configuration convenience methods")
        print("  • Enhanced brain convenience methods for simulation")
        print("  • Custom exception types for error handling")
        print("  • Simulation callbacks for monitoring and control")
        print("  • Performance monitoring and visualization support")
        return 0
    else:
        print(f"\n⚠️  {failed} demonstration(s) failed.")
        print("\nPlease check the errors above for more details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())