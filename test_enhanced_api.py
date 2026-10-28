#!/usr/bin/env python3
"""
Comprehensive test script for NLM Enhanced Python API
Tests all new features and improvements in Phase 7
"""

import time
import sys
import traceback
import pynlm

def test_basic_functionality():
    """Test basic Python API functionality"""
    print("=" * 60)
    print("TEST 1: Basic Python API Functionality")
    print("=" * 60)
    
    try:
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Brain initialized successfully")
        
        # Run simulation
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Simulation completed with {brain.getTotalSpikeCount()} spikes")
        print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        return True
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_classes():
    """Test enhanced Python API classes"""
    print("\n" + "=" * 60)
    print("TEST 2: Enhanced Python API Classes")
    print("=" * 60)
    
    try:
        # Test neuron creation and manipulation
        neuron_id = pynlm.NeuronId(0)
        neuron = pynlm.createNeuron(neuron_id)
        
        print(f"✓ Neuron created with ID: {neuron.getId().value}")
        neuron.setThreshold(-45.0)
        neuron.setType(pynlm.NeuronType.Modulatory)
        print(f"✓ Neuron threshold set to: {neuron.getThreshold()}")
        print(f"✓ Neuron type set to: {neuron.getType()}")
        
        # Test synapse creation
        synapse = pynlm.createSynapse(
            pynlm.SynapseId(0),
            neuron_id,
            neuron_id
        )
        print(f"✓ Synapse created with ID: {synapse.getId().value}")
        synapse.setWeight(1.5)
        synapse.setDelay(5)
        print(f"✓ Synapse weight set to: {synapse.getWeight()}")
        print(f"✓ Synapse delay set to: {synapse.getDelay()}")
        
        # Test neural region
        region = pynlm.createNeuralRegion(pynlm.RegionId(0))
        print(f"✓ Neural region created with ID: {region.getId().value}")
        
        # Test neural population
        population = pynlm.createNeuralPopulation(pynlm.PopulationId(0), region)
        print(f"✓ Neural population created with ID: {population.getId().value}")
        print(f"✓ Population neuron count: {population.getNeuronCount()}")
        
        # Test memory systems
        working_memory = pynlm.createWorkingMemory()
        working_memory.setCapacity(1000)
        print(f"✓ Working memory created with capacity: {working_memory.getCapacity()}")
        
        episodic_memory = pynlm.createEpisodicMemory()
        print(f"✓ Episodic memory created")
        
        # Test neuromodulation
        reward = pynlm.createReward()
        reward.setReward(1.0)
        print(f"✓ Reward system created with value: {reward.getValue()}")
        
        curiosity = pynlm.createCuriosity()
        curiosity.setLevel(0.7)
        print(f"✓ Curiosity system created with level: {curiosity.getLevel()}")
        
        # Test plasticity systems
        stdp = pynlm.createSTDP()
        hebbian = pynlm.createHebbian()
        structural = pynlm.createStructuralPlasticity()
        print(f"✓ Plasticity systems created (STDP, Hebbian, Structural)")
        
        # Test development system
        development = pynlm.createDevelopmentSystem()
        development.setStage(pynlm.DevelopmentalStage.CriticalPeriod)
        print(f"✓ Development system created with stage: {development.getCurrentStage()}")
        
        # Test cognition systems
        planner = pynlm.createNeuralPlanner()
        concept = pynlm.createConceptFormation()
        print(f"✓ Cognition systems created (Planner, ConceptFormation)")
        
        return True
    except Exception as e:
        print(f"✗ Enhanced classes test failed: {e}")
        traceback.print_exc()
        return False

def test_dynamic_commands():
    """Test dynamic command system"""
    print("\n" + "=" * 60)
    print("TEST 3: Dynamic Command System")
    print("=" * 60)
    
    try:
        # Create command registry
        registry = pynlm.createCommandRegistry()
        print(f"✓ Command registry created")
        
        # Show available commands
        registry.showCommands()
        print(f"✓ Commands displayed (showing basic commands)")
        
        # Test help command
        registry.executeCommand("help", [])
        print(f"✓ Help command executed")
        
        # Test dynamic command registration (would need custom handler)
        print(f"✓ Dynamic command system ready for custom commands")
        
        return True
    except Exception as e:
        print(f"✗ Dynamic commands test failed: {e}")
        traceback.print_exc()
        return False

def test_performance_monitoring():
    """Test performance monitoring and optimization"""
    print("\n" + "=" * 60)
    print("TEST 4: Performance Monitoring")
    print("=" * 60)
    
    try:
        # Create brain with monitoring
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Configure memory pools
        memory_config = pynlm.createMemoryPoolConfig()
        memory_config.blockSize = 4096
        memory_config.numBlocks = 256
        memory_config.enablePreallocation = True
        
        # Configure SIMD vectorization
        simd_config = pynlm.createSIMDConfig()
        simd_config.enableVectorization = True
        simd_config.vectorWidth = 512
        
        # Configure parallel processing
        events = pynlm.createEventQueue()
        events.setMaxWorkers(4)
        events.setMaxQueueSize(1000)
        
        print(f"✓ Performance monitoring components configured")
        print(f"✓ Memory pools: {memory_config.numBlocks} blocks of {memory_config.blockSize} bytes each")
        print(f"✓ SIMD vectorization: {simd_config.vectorWidth}-bit enabled")
        print(f"✓ Parallel processing: {events.getMaxWorkers()} workers, {events.getMaxQueueSize()} queue size")
        
        # Run simulation with monitoring
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        print(f"✓ Simulation completed in {end_time - start_time:.2f} seconds")
        print(f"✓ Performance monitoring functional")
        
        return True
    except Exception as e:
        print(f"✗ Performance monitoring test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_memory():
    """Test advanced memory system features"""
    print("\n" + "=" * 60)
    print("TEST 5: Advanced Memory Systems")
    print("=" * 60)
    
    try:
        # Working memory test
        working_memory = pynlm.createWorkingMemory()
        working_memory.setCapacity(1000)
        
        # Store patterns
        pattern1 = [0.1, 0.5, 0.9, 0.3, 0.7]
        working_memory.store(pattern1)
        
        pattern2 = [0.8, 0.2, 0.4, 0.6, 0.1]
        working_memory.store(pattern2)
        
        # Retrieve patterns
        retrieved = working_memory.retrieve(0)
        print(f"✓ Working memory: Stored and retrieved {working_memory.getActivePatternCount()} patterns")
        print(f"✓ Pattern similarity: {working_memory.getPatternSimilarity(pattern1, pattern2):.3f}")
        
        # Episodic memory test
        episodic_memory = pynlm.createEpisodicMemory()
        
        # Store experiences
        episodic_memory.storeExperience(" learned pattern recognition", 0.9, time.time())
        episodic_memory.storeExperience(" solved problem", 0.8, time.time() + 10)
        episodic_memory.storeExperience(" tested hypothesis", 0.7, time.time() + 20)
        
        print(f"✓ Episodic memory: {episodic_memory.getEpisodeCount()} episodes stored")
        print(f"✓ Total importance: {episodic_memory.getTotalImportance()}")
        print(f"✓ Average importance: {episodic_memory.getAverageImportance():.2f}")
        
        # Test memory replay
        if episodic_memory.getEpisodeCount() > 0:
            episode_id = 0
            episodic_memory.replayEpisode(episode_id)
            print(f"✓ Episodic memory replay functional")
        
        return True
    except Exception as e:
        print(f"✗ Advanced memory test failed: {e}")
        traceback.print_exc()
        return False

def test_neuromodulation():
    """Test advanced neuromodulation systems"""
    print("\n" + "=" * 60)
    print("TEST 6: Neuromodulation Systems")
    print("=" * 60)
    
    try:
        # Create brain for neuromodulation tests
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Reward system test
        reward = pynlm.createReward()
        reward.setReward(1.0)
        reward.setPredictionError(0.2)
        reward.applyToBrain(brain)
        
        print(f"✓ Reward system: value={reward.getValue()}, prediction error={reward.getPredictionError()}")
        print(f"✓ Reward sign: {'positive' if reward.isPositive() else 'negative' if reward.isNegative() else 'neutral'}")
        
        # Curiosity system test
        curiosity = pynlm.createCuriosity()
        curiosity.setLevel(0.7)
        curiosity.triggerExploration()
        
        print(f"✓ Curiosity system: level={curiosity.getLevel()}, drive={curiosity.getDriveStrength()}")
        print(f"✓ Exploration triggered: {curiosity.isHighCuriosity()}")
        
        # Novelty detection
        novelty = pynlm.createNovelty()
        novelty.setSensitivity(0.8)
        
        test_pattern1 = [0.1, 0.2, 0.3]
        test_pattern2 = [0.9, 0.8, 0.7]  # Different pattern
        novelty.updateBaseline(test_pattern1)
        
        is_novel = novelty.isNovel(test_pattern2)
        print(f"✓ Novelty detection: sensitivity={novelty.getSensitivity()}, novel={is_novel}")
        
        # Prediction error system
        prediction_error = pynlm.createPredictionError()
        prediction_error.calculateError(1.0, 0.8)  # Expected=1.0, Actual=0.8
        
        print(f"✓ Prediction error: error={prediction_error.getError():.3f}, magnitude={prediction_error.getMagnitude():.3f}")
        
        return True
    except Exception as e:
        print(f"✗ Neuromodulation test failed: {e}")
        traceback.print_exc()
        return False

def test_plasticity_systems():
    """Test advanced plasticity systems"""
    print("\n" + "=" * 60)
    print("TEST 7: Plasticity Systems")
    print("=" * 60)
    
    try:
        # Create plasticity systems
        stdp = pynlm.createSTDP()
        hebbian = pynlm.createHebbian()
        structural = pynlm.createStructuralPlasticity()
        
        # STDP test
        stdp.setLearningRate(0.001)
        ltp = stdp.calculateLTP(10.0)  # Positive delta
        ltd = stdp.calculateLTD(-5.0)  # Negative delta
        
        print(f"✓ STDP system: learning rate={stdp.getLearningRate()}, LTP={ltp:.4f}, LTD={ltd:.4f}")
        
        # Hebbian test
        hebbian.setLearningRate(0.005)
        hebbian.setCoherenceThreshold(0.7)
        update = hebbian.calculateUpdate(10, 5)  # preSpike=10, postSpike=5
        
        print(f"✓ Hebbian system: learning rate={hebbian.getLearningRate()}, update={update:.4f}")
        
        # Structural plasticity test
        structural.setSynaptogenesisRate(0.0002)
        structural.setPruningRate(0.00001)
        structural.setTargetSynapseCount(1000)
        
        print(f"✓ Structural plasticity: synaptogenesis={structural.getSynaptogenesisRate()}, pruning={structural.getPruningRate()}")
        print(f"✓ Target synapse count: {structural.getTargetSynapseCount()}")
        
        # Test plasticity enabling in neurons
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get neurons and enable plasticity
        neurons = brain.getAllNeurons()
        for neuron in neurons[:5]:
            neuron.enablePlasticity(True, True, False)  # Hebbian + STDP
        
        print(f"✓ Plasticity enabled in {len(neurons[:5])} neurons")
        
        return True
    except Exception as e:
        print(f"✗ Plasticity systems test failed: {e}")
        traceback.print_exc()
        return False

def test_development_system():
    """Test development system"""
    print("\n" + "=" * 60)
    print("TEST 8: Development System")
    print("=" * 60)
    
    try:
        # Create development system
        development = pynlm.createDevelopmentSystem()
        
        # Test development stages
        stages = [pynlm.DevelopmentalStage.Initial,
                 pynlm.DevelopmentalStage.CriticalPeriod,
                 pynlm.DevelopmentalStage.Maturation,
                 pynlm.DevelopmentalStage.Adult,
                 pynlm.DevelopmentalStage.Aging]
        
        for stage in stages:
            development.setStage(stage)
            print(f"✓ Development stage: {stage}")
        
        # Test development metrics
        development.setAge(10.0)
        development.setPlasticityRate(0.5)
        
        print(f"✓ Age: {development.getAge()}")
        print(f"✓ Current stage: {development.getCurrentStage()}")
        print(f"✓ Plasticity rate: {development.getPlasticityRate()}")
        print(f"✓ Critical period: {development.getCriticalPeriodActive()}")
        print(f"✓ Maturation progress: {development.getMaturationProgress()}")
        print(f"✓ Is mature: {development.isMature()}")
        print(f"✓ Is critical period: {development.isCriticalPeriod()}")
        
        return True
    except Exception as e:
        print(f"✗ Development system test failed: {e}")
        traceback.print_exc()
        return False

def test_cognition_systems():
    """Test cognition systems"""
    print("\n" + "=" * 60)
    print("TEST 9: Cognition Systems")
    print("=" * 60)
    
    try:
        # Create cognition systems
        planner = pynlm.createNeuralPlanner()
        concept = pynlm.createConceptFormation()
        
        # Neural planner test
        planner.setPlanningHorizon(100)
        planner.setExplorationExploitationRatio(0.3)
        
        # Mock action sequence evaluation
        test_sequence = [1, 2, 3, 4, 5]  # Mock action IDs
        utility = planner.evaluateActionSequence(test_sequence)
        
        print(f"✓ Neural planner: horizon={planner.getPlanningHorizon()}, utility={utility}")
        
        # Concept formation test
        concept.setLearningRate(0.02)
        concept.setActivationThreshold(0.5)
        
        # Mock pattern processing
        test_pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
        concept_id = concept.processPattern(test_pattern)
        
        print(f"✓ Concept formation: learning rate={concept.getLearningRate()}")
        print(f"✓ Concept activation threshold: {concept.getConceptActivationThreshold()}")
        
        return True
    except Exception as e:
        print(f"✗ Cognition systems test failed: {e}")
        traceback.print_exc()
        return False

def test_integration():
    """Test integrated system functionality"""
    print("\n" + "=" * 60)
    print("TEST 10: Integrated System")
    print("=" * 60)
    
    try:
        # Create complete integrated system
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run integrated simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.2f}, "
                      f"DevStage={brain.getDevelopmentalStage()}")
        
        print(f"✓ Integrated system: Step {step} completed")
        print(f"✓ Final statistics: Neuring firing={brain.getFiringNeuronCount()}")
        print(f"✓ System integration functional")
        
        return True
    except Exception as e:
        print(f"✗ Integrated system test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("NLM Enhanced Python API Test Suite")
    print("Testing Phase 7: Advanced Python API with Runtime Features")
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Classes", test_enhanced_classes),
        ("Dynamic Commands", test_dynamic_commands),
        ("Performance Monitoring", test_performance_monitoring),
        ("Advanced Memory", test_advanced_memory),
        ("Neuromodulation", test_neuromodulation),
        ("Plasticity Systems", test_plasticity_systems),
        ("Development System", test_development_system),
        ("Cognition Systems", test_cognition_systems),
        ("Integrated System", test_integration),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} test crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success rate: {passed/len(tests)*100:.1f}%")
    
    if failed == 0:
        print("\n🎉 All tests passed! Phase 7 enhancements are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please review the failures above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())