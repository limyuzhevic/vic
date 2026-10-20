#!/usr/bin/env python3
"""
Comprehensive test script to verify all new Python bindings for cognition and neuromodulation systems.
This script tests all the newly added classes and helper functions.
"""

import pynlm
import sys

def test_all_bindings():
    """Run comprehensive tests for all new bindings"""
    print("=" * 80)
    print("COMPREHENSIVE NLM PYTHON BINDINGS TEST")
    print("Testing all cognition and neuromodulation systems")
    print("=" * 80)
    
    all_passed = True
    
    # Test 1: Create brain and basic functionality
    print("\n1. Testing Basic Brain Creation and Initialization...")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print("   ✓ Brain created and initialized successfully")
        print(f"   ✓ Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   ✓ Total synapses: {brain.getTotalSynapseCount()}")
    except Exception as e:
        print(f"   ✗ FAILED: {e}")
        all_passed = False
        brain = None
    
    # Test 2: NeuralWorkingMemory (memory system)
    if brain:
        print("\n2. Testing NeuralWorkingMemory...")
        try:
            working_memory = pynlm.getWorkingMemoryFromBrain(brain)
            if working_memory:
                print("   ✓ NeuralWorkingMemory retrieved successfully")
                print(f"   ✓ Capacity: {working_memory.getCapacity()}")
                print(f"   ✓ Active traces: {working_memory.getActiveTraces()}")
                
                # Test basic operations
                working_memory.store([0.1, 0.2, 0.3], 0.5)
                print("   ✓ Pattern stored successfully")
                
                retrieved = working_memory.retrieve()
                print(f"   ✓ Pattern retrieved: {retrieved}")
                
                working_memory.update(0.1)
                print("   ✓ Working memory updated")
                
                working_memory.clear()
                print("   ✓ Working memory cleared")
                
                print("   NeuralWorkingMemory test PASSED")
            else:
                print("   ✗ NeuralWorkingMemory not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 3: NeuralEpisodicMemory (memory system)
    if brain:
        print("\n3. Testing NeuralEpisodicMemory...")
        try:
            episodic_memory = pynlm.getEpisodicMemoryFromBrain(brain)
            if episodic_memory:
                print("   ✓ NeuralEpisodicMemory retrieved successfully")
                print(f"   ✓ Episode count: {episodic_memory.getEpisodeCount()}")
                
                # Test episodic memory operations
                # Create a simple episodic memory item
                from pynlm import ActionType
                episode = pynlm.EpisodicMemoryItem()
                episode.timestamp = 100
                episode.action = ActionType.MoveForward
                episode.reward = 0.8
                
                episodic_memory.storeEpisode(episode)
                print("   ✓ Episode stored successfully")
                
                recent = episodic_memory.getRecentEpisodes(5)
                print(f"   ✓ Recent episodes retrieved: {len(recent)}")
                
                episodic_memory.clear()
                print("   ✓ Episodic memory cleared")
                
                print("   NeuralEpisodicMemory test PASSED")
            else:
                print("   ✗ NeuralEpisodicMemory not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 4: NeuralAssociativeMemory (memory system)
    if brain:
        print("\n4. Testing NeuralAssociativeMemory...")
        try:
            associative_memory = pynlm.getAssociativeMemoryFromBrain(brain)
            if associative_memory:
                print("   ✓ NeuralAssociativeMemory retrieved successfully")
                print(f"   ✓ Association count: {associative_memory.getAssociationCount()}")
                
                # Test associative memory operations
                pattern_a = [0.1, 0.2, 0.3]
                pattern_b = [0.4, 0.5, 0.6]
                
                associative_memory.associate(pattern_a, pattern_b, 0.7)
                print("   ✓ Association created successfully")
                
                strength = associative_memory.getAssociationStrength(pattern_a, pattern_b)
                print(f"   ✓ Association strength: {strength}")
                
                retrieved = associative_memory.retrieve(pattern_a, 3)
                print(f"   ✓ Patterns retrieved: {len(retrieved)}")
                
                associative_memory.clear()
                print("   ✓ Associative memory cleared")
                
                print("   NeuralAssociativeMemory test PASSED")
            else:
                print("   ✗ NeuralAssociativeMemory not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 5: PredictionSystem
    if brain:
        print("\n5. Testing PredictionSystem...")
        try:
            prediction_system = pynlm.getPredictionSystemFromBrain(brain)
            if prediction_system:
                print("   ✓ PredictionSystem retrieved successfully")
                
                # Test prediction system operations
                prediction = prediction_system.predictNextState([])
                print(f"   ✓ Prediction made: {prediction}")
                
                print(f"   ✓ Prediction error: {prediction_system.getPredictionError()}")
                print(f"   ✓ Prediction confidence: {prediction_system.getConfidence()}")
                
                error_history = prediction_system.getErrorHistory()
                print(f"   ✓ Error history length: {len(error_history)}")
                
                prediction_system.clearHistory()
                print("   ✓ Prediction history cleared")
                
                print("   PredictionSystem test PASSED")
            else:
                print("   ✗ PredictionSystem not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 6: NeuralPlanner (cognition system)
    if brain:
        print("\n6. Testing NeuralPlanner...")
        try:
            neural_planner = pynlm.getNeuralPlannerFromBrain(brain)
            if neural_planner:
                print("   ✓ NeuralPlanner retrieved successfully")
                print(f"   ✓ Planning depth: {neural_planner.getPlanningDepth()}")
                print(f"   ✓ Planning confidence: {neural_planner.getPlanningConfidence()}")
                
                # Test neural planner operations
                current_goal = neural_planner.getCurrentGoal()
                print(f"   ✓ Current goal: {current_goal}")
                
                neural_planner.setCurrentGoal([0.1, 0.2, 0.3, 0.4, 0.5])
                print("   ✓ Current goal set successfully")
                
                action = neural_planner.planAction([0.1, 0.2, 0.3])
                print(f"   ✓ Planned action: {action}")
                
                neural_planner.clearCache()
                print("   ✓ Neural planner cache cleared")
                
                print("   NeuralPlanner test PASSED")
            else:
                print("   ✗ NeuralPlanner not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 7: ConceptFormation (cognition system)
    if brain:
        print("\n7. Testing ConceptFormation...")
        try:
            concept_formation = pynlm.getConceptFormationFromBrain(brain)
            if concept_formation:
                print("   ✓ ConceptFormation retrieved successfully")
                print(f"   ✓ Concept count: {concept_formation.getConceptCount()}")
                
                # Test concept formation operations
                pattern = [0.5, 0.6, 0.7, 0.8, 0.9]
                concept_id = concept_formation.classify(pattern)
                print(f"   ✓ Concept classified with ID: {concept_id}")
                
                # Test learning concept
                features = [1.0, 1.2, 1.5]
                learned_id = concept_formation.learnConcept(pattern, features, 0.8, 100)
                print(f"   ✓ Concept learned with ID: {learned_id}")
                
                concept_formation.clearConcepts()
                print("   ✓ Concepts cleared")
                
                print("   ConceptFormation test PASSED")
            else:
                print("   ✗ ConceptFormation not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 8: AttentionalSelection (cognition system)
    if brain:
        print("\n8. Testing AttentionalSelection...")
        try:
            attentional_selection = pynlm.getAttentionalSelectionFromBrain(brain)
            if attentional_selection:
                print("   ✓ AttentionalSelection retrieved successfully")
                print(f"   ✓ Inhibition strength: {attentional_selection.getInhibitionStrength()}")
                print(f"   ✓ Excitation strength: {attentional_selection.getExcitationStrength()}")
                
                # Test attentional selection operations
                competitors = [0.1, 0.5, 0.3, 0.8, 0.2]
                attention_value = attentional_selection.process_competition(competitors, 0.2)
                print(f"   ✓ Competition result: {attention_value}")
                
                attentional_selection.focusOnRegion("cortex")
                print("   ✓ Attention focused on region")
                
                attentional_selection.releaseAttention()
                print("   ✓ Attention released")
                
                attentional_selection.reset()
                print("   ✓ Attentional selection reset")
                
                print("   AttentionalSelection test PASSED")
            else:
                print("   ✗ AttentionalSelection not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 9: DevelopmentSystem
    if brain:
        print("\n9. Testing DevelopmentSystem...")
        try:
            development_system = pynlm.getDevelopmentSystemFromBrain(brain)
            if development_system:
                print("   ✓ DevelopmentSystem retrieved successfully")
                print(f"   ✓ Developmental stage: {development_system.getDevelopmentalStage()}")
                print(f"   ✓ Developmental age: {development_system.getDevelopmentalAge()}")
                print(f"   ✓ Plasticity modifier: {development_system.getPlasticityModifier()}")
                
                # Test development system operations
                development_system.setDevelopmentalAge(10.5)
                print("   ✓ Developmental age set successfully")
                
                development_system.advanceStage()
                print("   ✓ Developmental stage advanced")
                
                print("   DevelopmentSystem test PASSED")
            else:
                print("   ✗ DevelopmentSystem not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 10: Dopamine
    if brain:
        print("\n10. Testing Dopamine...")
        try:
            dopamine = pynlm.getDopamineFromBrain(brain)
            if dopamine:
                print("   ✓ Dopamine retrieved successfully")
                print(f"   ✓ Dopamine level: {dopamine.getLevel()}")
                print(f"   ✓ Plasticity factor: {dopamine.getPlasticityFactor()}")
                
                # Test dopamine operations
                dopamine.setLevel(0.5)
                print("   ✓ Dopamine level set to 0.5")
                
                dopamine.signalReward(0.8)
                print("   ✓ Reward signal sent")
                
                dopamine.signalRewardPredictionError(-0.3)
                print("   ✓ Reward prediction error signal sent")
                
                print("   Dopamine test PASSED")
            else:
                print("   ✗ Dopamine not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 11: Novelty
    if brain:
        print("\n11. Testing Novelty...")
        try:
            novelty = pynlm.getNoveltyFromBrain(brain)
            if novelty:
                print("   ✓ Novelty retrieved successfully")
                print(f"   ✓ Novelty level: {novelty.getLevel()}")
                
                # Test novelty operations
                novelty.setLevel(0.3)
                print("   ✓ Novelty level set to 0.3")
                
                print("   Novelty test PASSED")
            else:
                print("   ✗ Novelty not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 12: PredictionError
    if brain:
        print("\n12. Testing PredictionError...")
        try:
            prediction_error = pynlm.getPredictionErrorFromBrain(brain)
            if prediction_error:
                print("   ✓ PredictionError retrieved successfully")
                print(f"   ✓ Prediction error level: {prediction_error.getLevel()}")
                print(f"   ✓ Prediction error sign: {prediction_error.getSign()}")
                
                # Test prediction error operations
                prediction_error.setLevel(-0.5)
                print("   ✓ Prediction error level set to -0.5")
                print(f"   ✓ Prediction error sign now: {prediction_error.getSign()}")
                
                prediction_error.update(0.1, 1.0, 1.2)
                print("   ✓ Prediction error updated")
                
                print("   PredictionError test PASSED")
            else:
                print("   ✗ PredictionError not available")
                all_passed = False
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 13: Helper Functions
    print("\n13. Testing Helper Functions...")
    try:
        # Test configuration creation helpers
        config1 = pynlm.createWorkingMemoryConfig()
        print("   ✓ createWorkingMemoryConfig()")
        
        config2 = pynlm.createPredictionConfig()
        print("   ✓ createPredictionConfig()")
        
        config3 = pynlm.createCognitionConfig()
        print("   ✓ createCognitionConfig()")
        
        config4 = pynlm.createNeuromodulationConfig()
        print("   ✓ createNeuromodulationConfig()")
        
        config5 = pynlm.createDevelopmentConfig()
        print("   ✓ createDevelopmentConfig()")
        
        # Test system factory functions
        wm = pynlm.createWorkingMemory()
        print("   ✓ createWorkingMemory()")
        
        epm = pynlm.createEpisodicMemory()
        print("   ✓ createEpisodicMemory()")
        
        am = pynlm.createAssociativeMemory()
        print("   ✓ createAssociativeMemory()")
        
        pred = pynlm.createPredictionSystem()
        print("   ✓ createPredictionSystem()")
        
        # Test type conversion helpers
        neuron_id = pynlm.neuron_id_from_index(42)
        print("   ✓ neuron_id_from_index(42)")
        
        neuron_index = pynlm.neuron_id_to_index(neuron_id)
        print(f"   ✓ neuron_id_to_index() returned {neuron_index}")
        
        action_type = pynlm.action_type_from_string("MoveForward")
        print(f"   ✓ action_type_from_string('MoveForward') returned {action_type}")
        
        print("   Helper functions test PASSED")
    except Exception as e:
        print(f"   ✗ FAILED: {e}")
        all_passed = False
    
    # Test 14: Brain Convenience Methods
    if brain:
        print("\n14. Testing Brain Convenience Methods...")
        try:
            # Test bulk system access
            memory_systems = brain.getAllMemorySystems()
            print(f"   ✓ getAllMemorySystems() returned {len(memory_systems)} systems")
            
            prediction_systems = brain.getAllPredictionSystems()
            print(f"   ✓ getAllPredictionSystems() returned {len(prediction_systems)} systems")
            
            cognition_systems = brain.getAllCognitionSystems()
            print(f"   ✓ getAllCognitionSystems() returned {len(cognition_systems)} systems")
            
            neuromodulation_systems = brain.getAllNeuromodulationSystems()
            print(f"   ✓ getAllNeuromodulationSystems() returned {len(neuromodulation_systems)} systems")
            
            # Test system initialization
            brain.initializeAllSystems()
            print("   ✓ initializeAllSystems()")
            
            # Test system updates
            brain.updateAllSystems(0.1)
            print("   ✓ updateAllSystems(0.1)")
            
            # Test system stats
            stats = brain.getSystemStats()
            print(f"   ✓ getSystemStats() returned {len(stats)} metrics")
            
            brain.resetAllSystems()
            print("   ✓ resetAllSystems()")
            
            print("   Brain convenience methods test PASSED")
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Test 15: Agent Convenience Methods
    if brain:
        print("\n15. Testing Agent Convenience Methods...")
        try:
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(pynlm.createSimpleWorld())
            
            neuromodulators = agent.getAllNeuromodulators()
            print(f"   ✓ getAllNeuromodulators() returned {len(neuromodulators)} neuromodulators")
            
            agent.enableAllNeuromodulators(True)
            print("   ✓ enableAllNeuromodulators(True)")
            
            status = agent.getSystemStatus()
            print(f"   ✓ getSystemStatus() returned {len(status)} status entries")
            
            agent.resetAllSystems()
            print("   ✓ resetAllSystems()")
            
            print("   Agent convenience methods test PASSED")
        except Exception as e:
            print(f"   ✗ FAILED: {e}")
            all_passed = False
    
    # Final summary
    print("\n" + "=" * 80)
    if all_passed:
        print("🎉 ALL TESTS PASSED! 🎉")
        print("All Python bindings are working correctly!")
    else:
        print("❌ SOME TESTS FAILED ❌")
        print("There are issues with some of the bindings.")
    print("=" * 80)
    
    return all_passed

if __name__ == "__main__":
    success = test_all_bindings()
    sys.exit(0 if success else 1)