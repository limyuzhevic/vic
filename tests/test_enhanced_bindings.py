#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings.

This script tests the new functionality added to the enhanced Python bindings,
verifying that the improved API works correctly.
"""

import sys
import pynlm
import traceback

def test_basic_functionality():
    """Test basic enhanced functionality."""
    print("Testing basic enhanced functionality...")
    
    try:
        # Test factory functions
        print("  1. Testing factory functions...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("     ✓ Basic brain creation works")
        
        world = pynlm.createSimpleWorld()
        print("     ✓ World creation works")
        
        agent = pynlm.createAgentBrain(brain)
        print("     ✓ Agent creation works")
        
        # Test enhanced factory functions
        research_brain = pynlm.createResearchBrain(100, 0.1)
        research_brain.initialize()
        print("     ✓ Research brain factory works")
        
        research_world = pynlm.createSimpleResearchWorld(10, 10)
        print("     ✓ Research world factory works")
        
        enhanced_agent = pynlm.createAgentWithDefaults(research_brain, research_world)
        print("     ✓ Enhanced agent factory works")
        
        # Test complete research setup
        brain, world, agent = pynlm.createCompleteResearchSetup()
        print("     ✓ Complete research setup works")
        
        return True
        
    except Exception as e:
        print(f"     ✗ Error: {e}")
        traceback.print_exc()
        return False

def test_neural_architecture():
    """Test neural architecture components."""
    print("\nTesting neural architecture components...")
    
    try:
        brain = pynlm.createResearchBrain(500, 0.05)
        brain.initialize()
        
        # Test getting all neurons
        print("  1. Testing neuron access...")
        neurons = pynlm.getAllNeuronsInBrain(brain)
        print(f"     ✓ Retrieved {len(neurons)} neurons")
        
        if neurons:
            neuron = neurons[0]
            print(f"     ✓ Neuron ID: {neuron.getId()}")
            print(f"     ✓ Neuron type: {neuron.getType()}")
            print(f"     ✓ Neuron state accessible: {neuron.getState() is not None}")
        
        # Test brain statistics
        print("\n  2. Testing brain statistics...")
        stats = pynlm.getBrainStats(brain)
        print(f"     ✓ Brain statistics retrieved")
        print(f"     ✓ Total neurons: {stats['total_neurons']}")
        print(f"     ✓ Total synapses: {stats['total_synapses']}")
        print(f"     ✓ Firing neurons: {stats['firing_neurons']}")
        
        # Test simulation
        print("\n  3. Testing simulation...")
        step_stats = pynlm.runBrainSimulation(brain, 10, verbose=False)
        print(f"     ✓ Simulation completed with {len(step_stats)} recorded steps")
        
        return True
        
    except Exception as e:
        print(f"     ✗ Error: {e}")
        traceback.print_exc()
        return False

def test_plasticity_and_neuromodulation():
    """Test plasticity and neuromodulation components."""
    print("\nTesting plasticity and neuromodulation...")
    
    try:
        brain, world, agent = pynlm.createCompleteResearchSetup()
        
        # Test enabling learning systems
        print("  1. Testing learning subsystem control...")
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        print(f"     ✓ Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
        print(f"     ✓ Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
        print(f"     ✓ Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
        print(f"     ✓ Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
        
        # Test neuromodulation metrics
        print("\n  2. Testing neuromodulation metrics...")
        print(f"     ✓ Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        print(f"     ✓ Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"     ✓ Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"     ✓ Prediction error: {agent.getPredictionError():.3f}")
        
        # Run simulation to see learning in action
        print("\n  3. Testing learning simulation...")
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            if step % 5 == 0:
                print(f"     Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
        
        print("     ✓ Learning simulation completed successfully")
        
        return True
        
    except Exception as e:
        print(f"     ✗ Error: {e}")
        traceback.print_exc()
        return False

def test_batch_operations():
    """Test batch operations for efficiency."""
    print("\nTesting batch operations...")
    
    try:
        print("  1. Testing multiple brain creation...")
        brain_configs = [
            {'neuron_count': 1000, 'connection_density': 0.1},
            {'neuron_count': 1500, 'connection_density': 0.05},
            {'neuron_count': 2000, 'connection_density': 0.08}
        ]
        
        brains = []
        for config in brain_configs:
            brain = pynlm.createResearchBrain(config['neuron_count'], config['connection_density'])
            brain.initialize()
            brains.append(brain)
        
        print(f"     ✓ Created {len(brains)} brains with different configurations")
        
        print("\n  2. Testing batch initialization...")
        pynlm.initializeAllBrains(brains, 50)
        print("     ✓ Initialized all brains simultaneously")
        
        print("\n  3. Testing batch statistics collection...")
        stats_list = [pynlm.getBrainStats(brain) for brain in brains]
        print(f"     ✓ Collected statistics for {len(stats_list)} brains")
        
        # Compare configurations
        print("\n  4. Comparing brain configurations...")
        for i, stats in enumerate(stats_list):
            print(f"     Brain {i+1}: {stats['total_neurons']:,} neurons, {stats['total_synapses']:,} synapses")
        
        return True
        
    except Exception as e:
        print(f"     ✗ Error: {e}")
        traceback.print_exc()
        return False

def test_analysis_utilities():
    """Test analysis utilities."""
    print("\nTesting analysis utilities...")
    
    try:
        brain = pynlm.createResearchBrain(1000, 0.1)
        brain.initialize()
        
        # Test brain statistics
        print("  1. Testing brain statistics analysis...")
        stats = pynlm.getBrainStats(brain)
        print(f"     ✓ Brain statistics: {stats['total_neurons']:,} neurons")
        
        # Test connectome analysis
        print("\n  2. Testing connectome analysis...")
        # Create a region for connectome analysis
        region_id = brain.addRegion("cortex")
        region = brain.getRegion(region_id)
        
        # Create some populations
        populations = pynlm.createNeuralPopulations(region, 100, pynlm.NeuronType.Excitatory)
        
        # Analyze connectome
        connectome_analysis = pynlm.analyzeConnectome(region)
        print(f"     ✓ Connectome analysis completed")
        print(f"     ✓ Neurons analyzed: {connectome_analysis['neuron_count']}")
        print(f"     ✓ Populations analyzed: {connectome_analysis['population_count']}")
        print(f"     ✓ Synaptic density: {connectome_analysis['synaptic_density']:.4f}")
        
        # Test neural activity visualization
        print("\n  3. Testing neural activity visualization...")
        activity_map = pynlm.visualizeNeuralActivity(region)
        print(f"     ✓ Activity map created: {len(activity_map)}x{len(activity_map[0])}")
        
        return True
        
    except Exception as e:
        print(f"     ✗ Error: {e}")
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test backward compatibility with original API."""
    print("\nTesting backward compatibility...")
    
    try:
        print("  1. Testing original API functions...")
        config = pynlm.createDefaultConfig()
        print("     ✓ createDefaultConfig() works")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("     ✓ createBrain() works")
        
        world = pynlm.createSimpleWorld()
        print("     ✓ createSimpleWorld() works")
        
        agent = pynlm.createAgentBrain(brain)
        print("     ✓ createAgentBrain() works")
        
        print("\n  2. Testing original API constants...")
        print(f"     ✓ INVALID_NEURON_ID available: {hasattr(pynlm, 'INVALID_NEURON_ID')}")
        print(f"     ✓ INVALID_SYNAPSE_ID available: {hasattr(pynlm, 'INVALID_SYNAPSE_ID')}")
        print(f"     ✓ INVALID_REGION_ID available: {hasattr(pynlm, 'INVALID_REGION_ID')}")
        print(f"     ✓ INVALID_POPULATION_ID available: {hasattr(pynlm, 'INVALID_POPULATION_ID')}")
        
        return True
        
    except Exception as e:
        print(f"     ✗ Error: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 80)
    print("NLM Enhanced Python Bindings - Test Suite")
    print("=" * 80)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Neural Architecture", test_neural_architecture),
        ("Plasticity and Neuromodulation", test_plasticity_and_neuromodulation),
        ("Batch Operations", test_batch_operations),
        ("Analysis Utilities", test_analysis_utilities),
        ("Backward Compatibility", test_backward_compatibility)
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*80}")
        print(f"Running: {test_name}")
        print(f"{'='*80}")
        
        try:
            success = test_func()
            results.append((test_name, success))
            
            if success:
                print(f"\n✓ {test_name} PASSED")
            else:
                print(f"\n✗ {test_name} FAILED")
                
        except Exception as e:
            print(f"\n✗ {test_name} FAILED with exception: {e}")
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'='*80}")
    print("TEST SUMMARY")
    print(f"{'='*80}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status} {test_name}")
    
    print(f"\nOverall: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        print("The enhanced NLM Python bindings are working correctly.")
    else:
        print(f"\n⚠️  {total - passed} tests failed.")
        print("Some issues need to be addressed.")
    
    return passed == total

if __name__ == "__main__":
    print("Enhanced NLM Python Bindings Test Suite")
    print("This script tests the new functionality added to the enhanced bindings.")
    
    success = run_all_tests()
    
    sys.exit(0 if success else 1)
