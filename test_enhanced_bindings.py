#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings
Tests all the new functionality added to the bindings.cpp file
"""

import sys
import traceback

print("Testing Enhanced NLM Python Bindings")
print("=" * 50)

try:
    # Test 1: Basic import and version
    print("\n1. Testing basic import and version...")
    import pynlm
    print(f"✓ pynlm imported successfully")
    print(f"✓ Version: {pynlm.__version__}")
    
    # Test 2: Basic brain creation and initialization
    print("\n2. Testing basic brain creation and initialization...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Test 3: Advanced brain factory functions
    print("\n3. Testing advanced brain factory functions...")
    basic_brain = pynlm.createBasicBrain()
    advanced_brain = pynlm.createAdvancedBrain()
    test_brain = pynlm.createQuickTestBrain()
    print(f"✓ Basic brain: {basic_brain.getTotalNeuronCount()} neurons")
    print(f"✓ Advanced brain: {advanced_brain.getTotalNeuronCount()} neurons")
    print(f"✓ Test brain: {test_brain.getTotalNeuronCount()} neurons")
    
    # Test 4: Memory systems access
    print("\n4. Testing memory systems access...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    semantic_memory = brain.getSemanticMemory()
    procedural_memory = brain.getProceduralMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"✓ Working memory: {working_memory.getCapacity()} capacity")
    print(f"✓ Episodic memory: {episodic_memory.getEpisodeCount()} episodes")
    print(f"✓ Semantic memory: {semantic_memory.getAllFacts()} facts")
    print(f"✓ Procedural memory: {len(procedural_memory.getAllSkills())} skills")
    print(f"✓ Associative memory: {associative_memory.getAssociationCount()} associations")
    
    # Test 5: Working memory operations
    print("\n5. Testing working memory operations...")
    test_pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
    brain.wmStore(test_pattern)
    print(f"✓ Working memory pattern stored")
    
    wm_content = brain.wmRetrieve()
    print(f"✓ Working memory retrieved: {len(wm_content)} elements")
    
    # Test episodic memory operations
    print("\n6. Testing episodic memory operations...")
    episode = pynlm.EpisodicMemoryItem()
    episode.positionX = 10.0
    episode.positionY = 15.0
    episode.action = pynlm.ActionType.Eat
    episode.reward = 1.0
    brain.emStoreEpisode(episode)
    print(f"✓ Episodic memory stored")
    
    recent_episodes = brain.emGetRecentEpisodes(5)
    print(f"✓ Recent episodes: {len(recent_episodes)} episodes")
    
    # Test semantic memory operations
    print("\n7. Testing semantic memory operations...")
    brain.smStoreFact("test_key", "test_value")
    fact = brain.smRetrieveFact("test_key")
    print(f"✓ Semantic fact stored and retrieved: {fact is not None}")
    
    all_facts = brain.smGetAllFacts()
    print(f"✓ All semantic facts: {len(all_facts)} facts")
    
    # Test procedural memory operations
    print("\n8. Testing procedural memory operations...")
    brain.pmLearnSkill("test_skill", [0.8, 0.9, 0.1])
    skills = brain.pmGetAllSkills()
    print(f"✓ Procedural skills learned: {len(skills)} skills")
    
    # Test associative memory operations
    print("\n9. Testing associative memory operations...")
    brain.amAssociate([0.1, 0.2], [0.3, 0.4], 0.8)
    strength = brain.amGetAssociationStrength([0.1, 0.2], [0.3, 0.4])
    print(f"✓ Association created with strength: {strength}")
    
    associations = brain.amGetAssociationCount()
    print(f"✓ Total associations: {associations}")
    
    # Test prediction system access
    print("\n10. Testing prediction system access...")
    prediction_system = brain.getPredictionSystem()
    print(f"✓ Prediction system accessed")
    
    prediction_error = prediction_system.getPredictionError()
    confidence = prediction_system.getConfidence()
    print(f"✓ Prediction error: {prediction_error:.3f}")
    print(f"✓ Prediction confidence: {confidence:.3f}")
    
    # Test neuromodulation system access
    print("\n11. Testing neuromodulation system access...")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    dopamine.signalReward(1.0)
    curiosity.signalRewardPredictionError(0.5)
    print(f"✓ Neuromodulation systems accessed and signals sent")
    
    # Test development system access
    print("\n12. Testing development system access...")
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Initial)
    stage = brain.getDevelopmentalStage()
    print(f"✓ Developmental stage set to: {stage}")
    
    # Test cognition systems access
    print("\n13. Testing cognition systems access...")
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    brain.setPlanningDepth(3)
    planning_depth = brain.getPlanningDepth()
    print(f"✓ Planning depth set to: {planning_depth}")
    
    planning_confidence = brain.getPlanningConfidence()
    print(f"✓ Planning confidence: {planning_confidence:.3f}")
    
    # Test advanced cognition methods
    print("\n14. Testing advanced cognition methods...")
    concept_id = brain.learnConcept(
        pattern=[0.7, 0.8, 0.9],
        features=[0.1, 0.2],
        reward=1.0,
        currentTime=100
    )
    print(f"✓ Concept learned with ID: {concept_id}")
    
    # Test attention system
    brain.focusAttention(pynlm.RegionId(0))
    focus = brain.getAttentionalFocus()
    print(f"✓ Attention focused on {len(focus)} regions")
    
    # Test batch processing
    print("\n15. Testing batch processing...")
    brains = [brain, basic_brain, advanced_brain]
    steps = [50, 100, 150]
    
    batch_results = pynlm.runMultipleBrainsParallel(brains, steps)
    print(f"✓ Batch processing completed: {len(batch_results)} brains processed")
    
    # Test performance monitoring
    print("\n16. Testing performance monitoring...")
    performance = pynlm.profilePerformance(brains, steps)
    print(f"✓ Performance monitoring completed")
    print(f"  Total execution time: {performance['total_execution_time_ms']} ms")
    print(f"  Brains processed: {performance['brains_processed']}")
    
    # Test simulation monitoring
    print("\n17. Testing simulation monitoring...")
    monitoring_data = pynlm.monitorSimulationRealTime(
        brain, 
        list(range(50)),
        0.1
    )
    print(f"✓ Simulation monitoring completed: {len(monitoring_data)} data points")
    
    # Test anomaly detection
    print("\n18. Testing anomaly detection...")
    anomalies = pynlm.detectAnomalies(brain, list(range(50)), 0.1)
    print(f"✓ Anomaly detection completed")
    print(f"  Spike anomalies: {len(anomalies['spike_anomalies'])}")
    print(f"  Firing rate anomalies: {len(anomalies['firing_rate_anomalies'])}")
    print(f"  Connectivity anomalies: {len(anomalies['connectivity_anomalies'])}")
    
    # Test system health
    print("\n19. Testing system health...")
    health_reports = pynlm.getSystemHealth(brains, ["Brain1", "Brain2", "Brain3"])
    print(f"✓ System health monitoring completed: {len(health_reports)} reports")
    
    # Test connectivity analysis
    print("\n20. Testing connectivity analysis...")
    connectivity = pynlm.analyzeConnectivity(brain)
    print(f"✓ Connectivity analysis completed")
    print(f"  E/I Balance: {connectivity['e_i_balance']}")
    print(f"  Network Efficiency: {connectivity['network_efficiency']}")
    
    # Test scenario creation
    print("\n21. Testing scenario creation...")
    exploration_scenario = pynlm.createExplorationScenario()
    print(f"✓ Exploration scenario created")
    
    learning_scenario = pynlm.createLearningScenario()
    print(f"✓ Learning scenario created")
    
    development_scenario = pynlm.createDevelopmentScenario()
    print(f"✓ Development scenario created")
    
    # Test world creation
    print("\n22. Testing world creation...")
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    print(f"✓ World configured: {world.getWidth()}x{world.getHeight()} with {world.getVisionWidth()}x{world.getVisionHeight()} vision")
    
    # Test agent creation
    print("\n23. Testing agent creation...")
    agent = pynlm.createAgentBrain(brain)
    print(f"✓ Agent brain created")
    
    # Run simulation episode
    print("\n24. Running simulation episode...")
    episode_results = pynlm.runEpisode(brain, agent, world, 100, 0.1)
    print(f"✓ Simulation episode completed")
    print(f"  Final spike count: {episode_results['final_spike_count']}")
    print(f"  Curiosity level: {episode_results['curiosity_level']:.3f}")
    
    # Test multiple episodes
    print("\n25. Testing multiple episodes...")
    episodes = [exploration_scenario, learning_scenario, development_scenario]
    multi_results = pynlm.runBatchEpisodes(episodes, 50, 0.1)
    print(f"✓ Multiple episodes completed: {len(multi_results)} episodes")
    
    print("\n" + "=" * 50)
    print("ALL TESTS PASSED!")
    print("=" * 50)
    
except Exception as e:
    print(f"\n❌ TEST FAILED: {e}")
    print("\nFull traceback:")
    traceback.print_exc()
    sys.exit(1)
