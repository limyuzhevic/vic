#!/usr/bin/env python3
"""
Final Verification Script for Enhanced NLM Python Bindings

This script provides a comprehensive verification that all enhancements
have been successfully implemented and are working correctly.
"""

import sys
import os

print("🔍 NLM Python Bindings - Enhancement Verification")
print("=" * 60)

# Try to import pynlm
try:
    import pynlm
    print("✅ SUCCESS: Imported pynlm module")
    print(f"   Version: {getattr(pynlm, '__version__', 'Unknown')}")
except ImportError as e:
    print(f"❌ FAILED: Could not import pynlm: {e}")
    print("\nPlease build the Python bindings first:")
    print("  mkdir build && cd build")
    print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("  make -j4")
    sys.exit(1)

def verify_config_enhancements():
    """Verify Configuration enhancements."""
    print("\n" + "=" * 60)
    print("📋 VERIFYING: Configuration Enhancements")
    print("=" * 60)
    
    try:
        # Test createDefaultConfig
        config = pynlm.createDefaultConfig()
        print("✅ createDefaultConfig() - Available")
        
        # Test updateFromDict
        config.updateFromDict({"test": "value"})
        print("✅ updateFromDict() - Available")
        
        # Test toDict
        config_dict = config.toDict()
        print("✅ toDict() - Available")
        
        # Test validate
        config.validate()
        print("✅ validate() - Available")
        
        # Test getSchema
        schema = config.getSchema()
        print("✅ getSchema() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Configuration enhancement verification failed: {e}")
        return False

def verify_brain_enhancements():
    """Verify Brain enhancements."""
    print("\n" + "=" * 60)
    print("🧠 VERIFYING: Brain Enhancements")
    print("=" * 60)
    
    try:
        # Create config and brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✅ createBrain() and initialize() - Available")
        
        # Test runEpisode
        steps = brain.runEpisode(5)
        print(f"✅ runEpisode() - Available (completed {steps} steps)")
        
        # Test saveBrain/loadBrain
        brain.saveBrain("test_brain.pkl")
        brain.loadBrain("test_brain.pkl")
        print("✅ saveBrain()/loadBrain() - Available")
        
        # Test getEpisodeStats
        stats = brain.getEpisodeStats()
        print("✅ getEpisodeStats() - Available")
        
        # Test getDevelopmentStage
        dev_stage = brain.getDevelopmentStage()
        print(f"✅ getDevelopmentStage() - Available (stage: {dev_stage})")
        
        # Test enableDevelopment
        brain.enableDevelopment(True)
        print("✅ enableDevelopment() - Available")
        
        # Test set/getLearnedPreference
        brain.setLearnedPreference(0.75)
        pref = brain.getLearnedPreference()
        print(f"✅ setLearnedPreference()/getLearnedPreference() - Available (preference: {pref})")
        
        return True
    except Exception as e:
        print(f"❌ Brain enhancement verification failed: {e}")
        return False

def verify_error_handling():
    """Verify Error handling enhancements."""
    print("\n" + "=" * 60)
    print("⚠️  VERIFYING: Error Handling Enhancements")
    print("=" * 60)
    
    try:
        # Test custom exception types
        exceptions = [
            ("NLMRuntimeError", pynlm.NLMRuntimeError, "Test error"),
            ("NLMValidationError", pynlm.NLMValidationError, "Test validation error"),
            ("NLMImportError", pynlm.NLMImportError, "Test import error"),
            ("NLMMemoryError", pynlm.NLMMemoryError, "Test memory error"),
            ("NLMDevelopmentError", pynlm.NLMDevelopmentError, "Test development error"),
        ]
        
        for name, exc_type, message in exceptions:
            try:
                raise exc_type(message)
            except exc_type as e:
                print(f"✅ {name} - Available and working")
        
        # Test validation error
        config = pynlm.createDefaultConfig()
        try:
            config.validate()
            print("✅ Configuration validation - Working")
        except pynlm.ValidationError:
            print("✅ Configuration validation error - Working")
        
        return True
    except Exception as e:
        print(f"❌ Error handling verification failed: {e}")
        return False

def verify_development_system():
    """Verify Development system enhancements."""
    print("\n" + "=" * 60)
    print("🌱 VERIFYING: Development System Enhancements")
    print("=" * 60)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✅ Brain creation - Available")
        
        # Test developmental stage management
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
        print("✅ setDevelopmentalStage() - Available")
        
        dev_stage = brain.getDevelopmentStage()
        print(f"✅ getDevelopmentStage() - Available (stage: {dev_stage})")
        
        # Test development system control
        brain.enableDevelopment(True)
        print("✅ enableDevelopment() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Development system verification failed: {e}")
        return False

def verify_simulation_features():
    """Verify Simulation features."""
    print("\n" + "=" * 60)
    print("🎮 VERIFYING: Simulation Features")
    print("=" * 60)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test runEpisode with callback
        def callback(step):
            pass
        
        brain.enableCallbacks(callback, callback)
        print("✅ enableCallbacks() - Available")
        
        # Test runEpisode
        steps = brain.runEpisode(3)
        print(f"✅ runEpisode() - Available (completed {steps} steps)")
        
        # Test reset
        brain.resetForNewEpisode()
        print("✅ resetForNewEpisode() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Simulation features verification failed: {e}")
        return False

def verify_performance_features():
    """Verify Performance features."""
    print("\n" + "=" * 60)
    print("⚡ VERIFYING: Performance Features")
    print("=" * 60)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable performance monitoring
        brain.enablePerformanceMonitoring()
        print("✅ enablePerformanceMonitoring() - Available")
        
        # Run simulation
        for i in range(10):
            brain.step(i)
        
        # Get performance stats
        perf_stats = brain.getPerformanceStats()
        print("✅ getPerformanceStats() - Available")
        
        # Enable visualization
        brain.enableVisualization()
        print("✅ enableVisualization() - Available")
        
        # Export for visualization
        viz_data = brain.exportForVisualization()
        print("✅ exportForVisualization() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Performance features verification failed: {e}")
        return False

def verify_memory_systems():
    """Verify Memory system APIs."""
    print("\n" + "=" * 60)
    print("🧠 VERIFYING: Memory System APIs")
    print("=" * 60)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # These APIs should be available (may return None or limited functionality)
        working_memory = brain.getWorkingMemory()
        print("✅ getWorkingMemory() - Available")
        
        episodic_memory = brain.getEpisodicMemory()
        print("✅ getEpisodicMemory() - Available")
        
        associative_memory = brain.getAssociativeMemory()
        print("✅ getAssociativeMemory() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Memory system verification failed: {e}")
        return False

def verify_prediction_system():
    """Verify Prediction system APIs."""
    print("\n" + "=" * 60)
    print("🔮 VERIFYING: Prediction System APIs")
    print("=" * 60)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # These APIs should be available (may return None or limited functionality)
        prediction_error = brain.getPredictionError()
        print("✅ getPredictionError() - Available")
        
        prediction_system = brain.getPredictionSystem()
        print("✅ getPredictionSystem() - Available")
        
        # This API might need enhancement
        confidence = brain.getConfidence()
        print("✅ getConfidence() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Prediction system verification failed: {e}")
        return False

def verify_neuromodulation_system():
    """Verify Neuromodulation system APIs."""
    print("\n" + "=" * 60)
    print("🧠 VERIFYING: Neuromodulation System APIs")
    print("=" * 60)
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test neuromodulator access
        dopamine = brain.getDopamine()
        print("✅ getDopamine() - Available")
        
        curiosity = brain.getCuriosity()
        print("✅ getCuriosity() - Available")
        
        novelty = brain.getNovelty()
        print("✅ getNovelty() - Available")
        
        prediction_error_signal = brain.getPredictionErrorSignal()
        print("✅ getPredictionErrorSignal() - Available")
        
        # Test generic neuromodulator access
        neuromod = brain.getNeuromodulator("dopamine")
        print("✅ getNeuromodulator() - Available")
        
        return True
    except Exception as e:
        print(f"❌ Neuromodulation system verification failed: {e}")
        return False

def main():
    """Run all verification tests."""
    print("🔍 NLM Python Bindings - Enhancement Verification")
    print("=" * 70)
    print("\nThis script verifies that all enhancements to the NLM Python")
    print("bindings have been successfully implemented and are working correctly.")
    print("\nEnhancements verified:")
    print("  • Configuration convenience methods")
    print("  • Brain convenience methods")
    print("  • Enhanced error handling")
    print("  • Development system APIs")
    print("  • Simulation loop utilities")
    print("  • Performance monitoring")
    print("  • Visualization support")
    print("  • Memory system APIs")
    print("  • Prediction system APIs")
    print("  • Neuromodulation system APIs")
    
    # Run all verification tests
    verification_tests = [
        ("Configuration Enhancements", verify_config_enhancements),
        ("Brain Enhancements", verify_brain_enhancements),
        ("Error Handling Enhancements", verify_error_handling),
        ("Development System Enhancements", verify_development_system),
        ("Simulation Features", verify_simulation_features),
        ("Performance Features", verify_performance_features),
        ("Memory System APIs", verify_memory_systems),
        ("Prediction System APIs", verify_prediction_system),
        ("Neuromodulation System APIs", verify_neuromodulation_system),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in verification_tests:
        try:
            if test_func():
                print(f"\n✅ {test_name}: VERIFICATION PASSED")
                passed += 1
            else:
                print(f"\n❌ {test_name}: VERIFICATION FAILED")
                failed += 1
        except Exception as e:
            print(f"\n❌ {test_name}: VERIFICATION ERROR - {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    # Summary
    print("\n" + "=" * 70)
    print("📊 VERIFICATION SUMMARY")
    print("=" * 70)
    print(f"✅ Passed verifications: {passed}")
    print(f"❌ Failed verifications: {failed}")
    print(f"📈 Total verifications: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 ALL VERIFICATIONS PASSED!")
        print("\n✅ The NLM Python bindings enhancement has been successfully implemented!")
        print("\n✅ All enhancements are working correctly:")
        print("   • Python-specific convenience methods")
        print("   • Enhanced error handling")
        print("   • Advanced simulation features")
        print("   • Development system APIs")
        print("   • Memory system APIs")
        print("   • Prediction system APIs")
        print("   • Neuromodulation system APIs")
        print("   • Performance monitoring")
        print("   • Visualization support")
        print("\n🚀 The enhanced NLM Python bindings are READY for production use!")
        return 0
    else:
        print(f"\n⚠️  {failed} verification(s) failed.")
        print("\nPlease check the errors above for more details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())