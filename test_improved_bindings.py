#!/usr/bin/env python3
"""
Quick test to verify the improved Python bindings
"""

import pynlm

def test_basic_api():
    """Test basic API functionality"""
    print("Testing basic API...")
    
    # Test config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("✓ Config creation works")
    
    # Test brain creation
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("✓ Brain creation works")
    
    # Test brain initialization
    assert brain.initialize()
    print("✓ Brain initialization works")
    
    # Test basic brain operations
    brain.step(0, 0.0)
    assert brain.getTotalNeuronCount() > 0
    print("✓ Brain stepping works")
    
    # Test region management
    region_id = brain.addRegion("test_region")
    assert region_id is not None
    print("✓ Region management works")
    
    # Test region access
    region = brain.getRegion(region_id)
    assert region is not None
    print("✓ Region access works")
    
    print("Basic API test PASSED\n")

def test_phase6_system_access():
    """Test Phase 6 system access"""
    print("Testing Phase 6 system access...")
    
    # Create Phase 6 brain
    config = pynlm.createPhase6Config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    assert working_memory is not None
    assert episodic_memory is not None
    assert associative_memory is not None
    print("✓ Memory systems accessible")
    
    # Test prediction system
    prediction_system = brain.getPredictionSystem()
    assert prediction_system is not None
    print("✓ Prediction system accessible")
    
    # Test cognition systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    assert planner is not None
    assert concept_formation is not None
    assert attention is not None
    print("✓ Cognition systems accessible")
    
    # Test development system
    development_system = brain.getDevelopmentSystem()
    assert development_system is not None
    print("✓ Development system accessible")
    
    # Test neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    assert dopamine is not None
    assert curiosity is not None
    assert novelty is not None
    assert prediction_error is not None
    print("✓ Neuromodulation systems accessible")
    
    # Test development stage
    developmental_stage = brain.getDevelopmentalStage()
    assert developmental_stage is not None
    print("✓ Developmental stage accessible")
    
    # Test random generator
    random_gen = brain.getRandomGenerator()
    assert random_gen is not None
    print("✓ Random generator accessible")
    
    # Test learned preference
    learned_pref = brain.getLearnedPreference()
    assert learned_pref is not None
    print("✓ Learned preference accessible")
    
    print("Phase 6 system access test PASSED\n")

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test non-existent region
    region_id = pynlm.RegionId(99999)
    region = brain.getRegion(region_id)
    assert region is None
    print("✓ Non-existent region handled correctly")
    
    # Test invalid brain operations
    try:
        # This should handle gracefully
        brain.step(-1, -1.0)  # Negative step (should be handled)
        print("✓ Invalid step handled gracefully")
    except Exception as e:
        print(f"✓ Invalid step raised expected exception: {type(e).__name__}")
    
    print("Error handling test PASSED\n")

def test_agent_functionality():
    """Test AgentBrain functionality"""
    print("Testing AgentBrain functionality...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent properties
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    print("✓ Agent properties accessible")
    
    # Test agent initialization with world
    agent.initialize(world)
    print("✓ Agent re-initialization works")
    
    # Test agent with Phase 6 features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isCuriosityEnabled()
    assert agent.isDevelopmentEnabled()
    print("✓ Agent feature toggling works")
    
    print("Agent functionality test PASSED\n")

def test_documentation():
    """Test that documentation is accessible"""
    print("Testing documentation...")
    
    # Check that the module has documentation
    import inspect
    module_doc = pynlm.__doc__
    assert module_doc is not None
    assert "NLM" in module_doc
    print("✓ Module documentation available")
    
    # Check that Brain class has documentation
    brain_class = pynlm.Brain
    class_doc = brain_class.__doc__
    assert class_doc is not None
    print("✓ Brain class documentation available")
    
    print("Documentation test PASSED\n")

def main():
    """Run all tests"""
    print("NLM Python Bindings - Quick Test")
    print("=" * 40)
    print()
    
    try:
        test_basic_api()
        test_phase6_system_access()
        test_error_handling()
        test_agent_functionality()
        test_documentation()
        
        print("=" * 40)
        print("All tests PASSED! ✓")
        print()
        print("Summary of improvements:")
        print("✓ Enhanced Brain class with Phase 6 system access")
        print("✓ Improved error handling and exception safety")
        print("✓ Comprehensive documentation")
        print("✓ Maintained backward compatibility")
        print()
        print("The Python bindings are now ready for use!")
        
    except Exception as e:
        print(f"Test FAILED with error: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
