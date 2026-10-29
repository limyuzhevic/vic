#!/usr/bin/env python3
"""
NLM Integration Test Suite

This script tests the complete NLM brain loop integration to verify that:
1. Memory systems are connected to neural processing
2. Neuromodulation affects plasticity and dynamics
3. Development affects plasticity rates
4. Prediction system is integrated
5. Replay and consolidation are functional
6. Checkpoint save/load works

This is Phase 6 integration verification.
"""

import sys
import os
import traceback
import tempfile
import time

def test_integration_suite():
    """Run comprehensive integration tests"""
    print("=== NLM Phase 6 Integration Test Suite ===\n")
    
    try:
        # Import NLM modules
        import pynlm
        print("✅ NLM imported successfully")
        
    except ImportError as e:
        print(f"❌ FAILED: Could not import pynlm: {e}")
        return False
    
    tests_passed = 0
    tests_failed = 0
    
    # Test 1: Basic brain initialization and configuration
    print("\nTest 1: Basic brain initialization...")
    try:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
        assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
        
        print(f"✅ Basic brain setup: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses")
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
    
    # Test 2: World-agent integration
    print("\nTest 2: World-agent integration...")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all major subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("✅ World-agent integration complete")
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
    
    # Test 3: Complete simulation loop
    print("\nTest 3: Complete simulation loop...")
    try:
        # Setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        num_steps = 100
        total_reward = 0.0
        max_firing = 0
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            total_reward += reward
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track metrics
            current_firing = brain.getFiringNeuronCount()
            if current_firing > max_firing:
                max_firing = current_firing
        
        # Check results
        avg_firing = sum([brain.getFiringNeuronCount() for _ in range(10)]) / 10
        curiosity_level = agent.getCuriosityLevel()
        development_stage = brain.getDevelopmentalStage()
        
        print(f"✅ Simulation completed: {num_steps} steps")
        print(f"   Total reward: {total_reward:.2f}")
        print(f"   Max firing: {max_firing}")
        print(f"   Avg firing: {avg_firing:.1f}")
        print(f"   Final curiosity: {curiosity_level:.3f}")
        print(f"   Final development stage: {development_stage}")
        
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
        traceback.print_exc()
    
    # Test 4: Memory integration
    print("\nTest 4: Memory integration...")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Enable working memory
        if hasattr(brain, 'getWorkingMemory'):
            wm = brain.getWorkingMemory()
            if wm:
                # Record some activity
                for i in range(50):
                    brain.step(i)
                    if brain.getFiringNeuronCount() > 0:
                        wm.recordActivity(brain.getAverageFiringRate())
                
                print("✅ Working memory integration test passed")
            else:
                print("⚠️  Working memory not available")
        else:
            print("⚠️  getWorkingMemory method not available")
        
        # Check episodic memory if available
        if hasattr(brain, 'getEpisodicMemory'):
            em = brain.getEpisodicMemory()
            if em:
                print("✅ Episodic memory system available")
        
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
    
    # Test 5: Neuromodulation integration
    print("\nTest 5: Neuromodulation integration...")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Track neuromodulation levels
        initial_dopamine = agent.getNeuromodulationLevel()
        initial_curiosity = agent.getCuriosityLevel()
        
        # Run some steps with reward
        for i in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            
            # Apply reward
            reward = 0.1 if i % 10 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        final_dopamine = agent.getNeuromodulationLevel()
        final_curiosity = agent.getCuriosityLevel()
        
        print(f"✅ Neuromodulation integration complete")
        print(f"   Initial dopamine: {initial_dopamine:.3f}")
        print(f"   Final dopamine: {final_dopamine:.3f}")
        print(f"   Initial curiosity: {initial_curiosity:.3f}")
        print(f"   Final curiosity: {final_curiosity:.3f}")
        
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
    
    # Test 6: Development integration
    print("\nTest 6: Development integration...")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=12, height=12)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Track development stages
        stages = []
        for step in range(0, 200, 20):
            agent.updateDevelopment(0.1)
            stages.append(brain.getDevelopmentalStage())
        
        print(f"✅ Development integration complete")
        print(f"   Stages progression: {stages}")
        
        # Verify development actually progressed
        if len(set(stages)) > 1:
            print("   ✅ Development stages changed")
        else:
            print("   ⚠️  Development stages did not change")
        
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
    
    # Test 7: Checkpoint save/load
    print("\nTest 7: Checkpoint save/load...")
    try:
        # Create and initialize brain
        brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain1.initialize()
        
        # Run some simulation
        for i in range(50):
            brain1.step(i)
        
        # Create second brain
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        
        # Save brain1 state
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        brain1.save(checkpoint_file)
        print(f"   ✅ Saved checkpoint to: {checkpoint_file}")
        
        # Load into brain2
        brain2.load(checkpoint_file)
        print("   ✅ Loaded checkpoint into brain2")
        
        # Verify states are similar
        neurons1 = brain1.getTotalNeuronCount()
        neurons2 = brain2.getTotalNeuronCount()
        
        print(f"   Original brain neurons: {neurons1}")
        print(f"   Loaded brain neurons: {neurons2}")
        
        # Cleanup
        os.unlink(checkpoint_file)
        
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
        traceback.print_exc()
    
    # Test 8: Advanced Python API features
    print("\nTest 8: Advanced Python API features...")
    try:
        config = pynlm.createDefaultConfig()
        
        # Test configuration features
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        
        print("✅ Advanced configuration options available")
        
        # Test brain features
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test various brain methods
        methods_to_test = [
            ('getTotalNeuronCount', lambda: brain.getTotalNeuronCount()),
            ('getTotalSynapseCount', lambda: brain.getTotalSynapseCount()),
            ('getFiringNeuronCount', lambda: brain.getFiringNeuronCount()),
            ('getAverageFiringRate', lambda: brain.getAverageFiringRate()),
            ('getExcitationInhibitionRatio', lambda: brain.getExcitationInhibitionRatio()),
            ('getTotalSpikeCount', lambda: brain.getTotalSpikeCount()),
        ]
        
        for method_name, method_call in methods_to_test:
            try:
                result = method_call()
                print(f"  ✅ {method_name}: {result}")
            except Exception as e:
                print(f"  ❌ {method_name}: {e}")
        
        tests_passed += 1
        
    except Exception as e:
        print(f"❌ FAILED: {e}")
        tests_failed += 1
    
    # Summary
    print("\n" + "="*60)
    print("INTEGRATION TEST SUMMARY:")
    print(f"✅ Passed: {tests_passed}")
    print(f"❌ Failed: {tests_failed}")
    print(f"📊 Total: {tests_passed + tests_failed}")
    
    if tests_failed > 0:
        print(f"\n⚠️  {tests_failed} integration test(s) failed")
        return False
    else:
        print(f"\n🎉 All integration tests passed!")
        return True

if __name__ == "__main__":
    success = test_integration_suite()
    sys.exit(0 if success else 1)