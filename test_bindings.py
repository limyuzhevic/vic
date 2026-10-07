#!/usr/bin/env python3
"""
Test script for NLM Python bindings.

This script demonstrates the improved Python API for the NLM (Neural Learning Machine)
framework, showing all the new functionality added in the modular bindings.
"""

import pynlm as nlm
import numpy as np

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("=== Testing Basic Functionality ===")
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    print(f"Created config: {config}")
    
    # Create brain
    brain = nlm.createBrain(config)
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(100, 100, 32, 32)
    print(f"Created world {world.getWidth()}x{world.getHeight()}")
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    print(f"Created agent brain")
    
    return config, brain, world, agent_brain

def test_memory_systems(config):
    """Test all memory system bindings."""
    print("\n=== Testing Memory Systems ===")
    
    # Create brain
    brain = nlm.Brain(config)
    
    # Test working memory
    working_memory = brain.getWorkingMemory()
    working_memory.setCapacity(50)
    working_memory.setDecayRate(0.01)
    
    # Simulate some working memory usage
    for i in range(5):
        pattern = np.random.randn(brain.getTotalNeuronCount()).tolist()
        working_memory.store(pattern, 1.0)
    
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Working memory decay rate: {working_memory.getDecayRate()}")
    print(f"Active memory traces: {working_memory.getActiveTraces()}")
    
    # Test episodic memory
    episodic_memory = brain.getEpisodicMemory()
    episodic_memory.storeExperience(
        state=np.random.randn(10).tolist(),
        action=nlm.ActionType.MoveForward,
        reward=1.0,
        next_state=np.random.randn(10).tolist()
    )
    
    print(f"Episodic memory experiences: {episodic_memory.getExperienceCount()}")
    
    # Test semantic memory
    semantic_memory = brain.getSemanticMemory()
    semantic_memory.storeConcept(
        pattern=np.random.randn(20).tolist(),
        concept="test_concept"
    )
    
    print(f"Semantic memory concepts: {len(semantic_memory.getConcepts())}")
    
    # Test procedural memory
    procedural_memory = brain.getProceduralMemory()
    procedural_memory.storeSkill(
        action_sequence=[nlm.ActionType.MoveForward, nlm.ActionType.TurnRight],
        quality=0.8
    )
    
    print(f"Procedural memory skills: {len(procedural_memory.getAllSkills())}")
    
    # Test associative memory
    associative_memory = brain.getAssociativeMemory()
    pattern1 = np.random.randn(15).tolist()
    pattern2 = np.random.randn(15).tolist()
    associative_memory.associate(pattern1, pattern2)
    
    retrieved = associative_memory.retrieve(pattern1)
    print(f"Associative memory associations: {len(associative_memory.getAssociations(pattern1))}")

def test_neuromodulation():
    """Test neuromodulation system bindings."""
    print("\n=== Testing Neuromodulation ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    
    # Test dopamine
    dopamine = brain.getDopamine()
    dopamine.setLevel(0.5)
    dopamine.signalReward(1.0)
    dopamine.signalRewardPredictionError(0.2)
    
    print(f"Dopamine level: {dopamine.getLevel()}")
    print(f"Dopamine plasticity factor: {dopamine.getPlasticityFactor()}")
    
    # Test acetylcholine
    acetylcholine = brain.getAcetylcholine()
    print(f"Acetylcholine name: {acetylcholine.getName()}")
    
    # Test norepinephrine
    norepinephrine = brain.getNorepinephrine()
    print(f"Norepinephrine name: {norepinephrine.getName()}")
    
    # Test serotonin
    serotonin = brain.getSerotonin()
    print(f"Serotonin name: {serotonin.getName()}")
    
    # Test reward
    reward = brain.getReward()
    reward.signalReward(0.5)
    print(f"Reward plasticity factor: {reward.getPlasticityFactor()}")
    
    # Test prediction error
    pred_error = brain.getPredictionErrorSignal()
    print(f"Prediction error plasticity factor: {pred_error.getPlasticityFactor()}")
    
    # Test novelty
    novelty = brain.getNovelty()
    print(f"Novelty plasticity factor: {novelty.getPlasticityFactor()}")
    
    # Test curiosity
    curiosity = brain.getCuriosity()
    print(f"Curiosity plasticity factor: {curiosity.getPlasticityFactor()}")

def test_plasticity():
    """Test plasticity system bindings."""
    print("\n=== Testing Plasticity Systems ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    
    # Test STDP
    stdp = brain.getSTDP()
    stdp.setLTPWeight(0.01)
    stdp.setLTDWeight(-0.01)
    stdp.setTimeConstant(20.0)
    
    print(f"STDP LTP weight: {stdp.getLTPWeight()}")
    print(f"STDP LTD weight: {stdp.getLTDWeight()}")
    print(f"STDP time constant: {stdp.getTimeConstant()}")
    
    # Test Hebbian
    hebbian = brain.getHebbian()
    hebbian.setLearningRate(0.001)
    hebbian.setDecayRate(0.0001)
    
    print(f"Hebbian learning rate: {hebbian.getLearningRate()}")
    print(f"Hebbian decay rate: {hebbian.getDecayRate()}")
    
    # Test structural plasticity
    struct_plastic = brain.getStructuralPlasticity()
    struct_plastic.enableGrowth(True)
    struct_plastic.enablePruning(True)
    
    print(f"Connection growth enabled: {struct_plastic.isGrowthEnabled()}")
    print(f"Connection pruning enabled: {struct_plastic.isPruningEnabled()}")
    print(f"Connection count: {struct_plastic.getConnectionCount()}")

def test_cognition():
    """Test cognitive system bindings."""
    print("\n=== Testing Cognitive Systems ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    
    # Test neural planner
    planner = brain.getPlanner()
    planner.setPlanningDepth(5)
    planner.setActionQuality(nlm.ActionType.MoveForward, 0.8)
    
    print(f"Planning depth: {planner.getPlanningDepth()}")
    print(f"Planning confidence: {planner.getPlanningConfidence()}")
    
    # Test self model
    self_model = brain.getSelfModel()
    self_model.setCurrentGoal(np.random.randn(8).tolist())
    
    print(f"Has self model: {self_model.hasSelfModel()}")
    print(f"Capability level: {self_model.getCapabilityLevel()}")
    
    # Test social learning
    social = brain.getSocialLearning()
    social.observeAgentAction(
        nlm.ActionType.MoveForward,
        np.random.randn(10).tolist(),
        np.random.randn(10).tolist()
    )
    
    print(f"Observation count: {social.getObservationCount()}")
    print(f"Can imitate: {social.canImitate(nlm.ActionType.MoveForward)}")
    
    # Test concept formation
    concept = brain.getConceptFormation()
    concept.discoverConcept(
        np.random.randn(12).tolist(),
        "test_category"
    )
    
    print(f"Concepts by category: {len(concept.getConceptsByCategory('test_category'))}")
    
    # Test attentional selection
    attention = brain.getAttention()
    attention.setInhibitionStrength(0.5)
    attention.setExcitationStrength(0.5)
    attention.setCompetitionThreshold(0.7)
    
    print(f"Inhibition strength: {attention.getInhibitionStrength()}")
    print(f"Excitation strength: {attention.getExcitationStrength()}")
    print(f"Competition threshold: {attention.getCompetitionThreshold()}")

def test_development():
    """Test development system bindings."""
    print("\n=== Testing Development Systems ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    
    development = brain.getDevelopmentSystem()
    print(f"Current stage: {development.getStageName()}")
    print(f"Plasticity modifier: {development.getPlasticityModifier()}")
    print(f"Critical period: {development.isCriticalPeriod()}")
    print(f"Critical period progress: {development.getCriticalPeriodProgress()}")
    
    # Test synaptogenesis
    synaptogenesis = brain.getSynaptogenesis()
    synaptogenesis.addConnection(1, 2, 1.0)
    synaptogenesis.addConnection(3, 4, 0.5)
    
    print(f"Synaptogenesis connections: {synaptogenesis.getConnectionCount()}")
    
    # Test maturation
    maturation = brain.getMaturation()
    maturation.setRefinementProgress(0.8)
    
    print(f"Maturation refinement progress: {maturation.getRefinementProgress()}")
    
    # Test pruning
    pruning = brain.getPruning()
    pruning.pruneConnections(0.1)
    pruning.consolidateMemory(0.9)
    
    print(f"Pruned connections: {pruning.getPrunedCount()}")
    print(f"Consolidated memories: {pruning.getConsolidatedCount()}")

def test_sensory_motor():
    """Test sensory and motor system bindings."""
    print("\n=== Testing Sensory and Motor Systems ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    world = nlm.SimpleWorld()
    world.configure(50, 50, 16, 16)
    
    # Test sensory input
    vision = nlm.Vision(16, 16)
    vision_data = [i * 0.01 for i in range(16 * 16)]
    vision.setData(vision_data)
    
    audio = nlm.Audio(44100, 1024)
    audio_data = [0.5 * np.sin(2 * np.pi * 440 * i / 44100) for i in range(1024)]
    audio.setData(audio_data)
    audio.setSampleRate(44100)
    
    internal = nlm.InternalSignals()
    for i in range(5):
        internal.addSignal(np.random.randn())
    
    print(f"Vision dimensions: {vision.getDimensions()}")
    print(f"Audio sample rate: {audio.getSampleRate()}")
    print(f"Internal signals count: {internal.getData().__len__()}")
    
    # Test action
    action = nlm.Action(nlm.ActionType.MoveForward, [1.0, 0.5, 0.2])
    print(f"Action type: {action.getType()}")
    print(f"Action name: {action.getName()}")
    print(f"Action parameters: {action.getParameters()}")
    
    # Test sensory percept
    percept = nlm.SensoryPercept()
    percept.setVision(vision)
    percept.setAudio(audio)
    percept.setInternal(internal)
    
    print(f"Vision width: {percept.getVisionWidth()}")
    print(f"Audio channels: {len(percept.getAudio())}")

def test_experiments():
    """Test experiment system bindings."""
    print("\n=== Testing Experiment Systems ===")
    
    # Test metrics
    metrics = nlm.Metrics()
    metrics.recordMetric("test_metric", 1.0, 0.0)
    metrics.recordMetric("learning_rate", 0.001, 0.0)
    
    print(f"Metrics count: {len(metrics.getMetrics())}")
    print(f"Average learning_rate: {metrics.getAverage('learning_rate')}")
    
    # Test visualization interface (would typically be used with GUI)
    visualization = nlm.VisualizationInterface()
    print(f"Visualization created (would show GUI in real application)")

def test_advanced_features():
    """Test advanced features for power users."""
    print("\n=== Testing Advanced Features ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    
    # Get direct access to internal systems (for advanced users)
    spike_system = brain.getSpikeSystem()
    print(f"Spike system count: {spike_system.getSpikeCount()}")
    
    dynamics = brain.getNeuralDynamics()
    print(f"Neural dynamics available: {dynamics is not None}")
    
    # Test factory functions
    brain2 = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain2)
    
    print(f"Factory-created brain neurons: {brain2.getTotalNeuronCount()}")
    print(f"Factory-created world size: {world.getWidth()}x{world.getHeight()}")
    
    # Test constants
    print(f"Invalid neuron ID: {nlm.INVALID_NEURON_ID}")
    print(f"Invalid synapse ID: {nlm.INVALID_SYNAPSE_ID}")
    print(f"Invalid region ID: {nlm.INVALID_REGION_ID}")

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    try:
        # Test basic functionality
        config, brain, world, agent_brain = test_basic_functionality()
        
        # Test all systems
        test_memory_systems(config)
        test_neuromodulation()
        test_plasticity()
        test_cognition()
        test_development()
        test_sensory_motor()
        test_experiments()
        test_advanced_features()
        
        print("\n" + "=" * 50)
        print("All tests completed successfully!")
        print("\nKey improvements in this version:")
        print("1. Modular binding structure for better organization")
        print("2. Comprehensive documentation for all classes and methods")
        print("3. All memory systems exposed (working, episodic, semantic, procedural, associative)")
        print("4. Complete neuromodulation system (dopamine, acetylcholine, norepinephrine, serotonin, reward, prediction error, novelty, curiosity)")
        print("5. Full plasticity system (STDP, Hebbian, structural plasticity)")
        print("6. Complete cognitive system (neural planner, self model, social learning, concept formation, attentional selection)")
        print("7. Full development system (synaptogenesis, maturation, pruning)")
        print("8. Advanced features for power users (direct system access, factory functions)")
        print("9. Comprehensive error handling and validation")
        print("10. Performance monitoring and metrics")
        
    except Exception as e:
        print(f"\nTest failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())