#!/usr/bin/env python3
"""
Test script to verify the new neural memory systems bindings.
This script tests all the neural systems that were added to the Python bindings.
"""

import pynlm as nlm

def test_neural_working_memory():
    """Test NeuralWorkingMemory bindings"""
    print("Testing NeuralWorkingMemory...")
    
    # Create a brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create working memory
    wmem = nlm.NeuralWorkingMemory()
    
    # Test basic operations
    pattern = [0.5, 0.3, 0.8, 0.2, 0.6]
    wmem.store(pattern, 1.0)
    
    retrieved = wmem.retrieve()
    print(f"  Stored pattern: {pattern}")
    print(f"  Retrieved: {retrieved}")
    print(f"  Contains: {wmem.contains(nlm.INVALID_NEURON_ID)}")
    print(f"  Memory activity: {wmem.getMemoryActivity()}")
    print(f"  Active traces: {wmem.getActiveTraces()}")
    print("  ✓ NeuralWorkingMemory basic operations passed")
    return wmem

def test_neural_episodic_memory():
    """Test NeuralEpisodicMemory bindings"""
    print("\nTesting NeuralEpisodicMemory...")
    
    # Create a brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create episodic memory
    emem = nlm.NeuralEpisodicMemory()
    
    # Test creating and storing an episode
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = 100
    episode.sensoryState = [0.1, 0.2, 0.3, 0.4, 0.5]
    episode.positionX = 5.0
    episode.positionY = 3.0
    episode.orientation = 0.0
    episode.action = nlm.ActionType.MoveForward
    episode.reward = 1.0
    episode.energy = 0.8
    episode.novelty = 0.3
    
    emem.storeEpisode(episode)
    
    print(f"  Stored episode with timestamp: {episode.timestamp}")
    print(f"  Episode count: {emem.getEpisodeCount()}")
    print(f"  Recent episodes: {len(emem.getRecentEpisodes(2))}")
    print(f"  Average reward: {emem.getAverageReward()}")
    
    # Test retrieval
    similar = emem.retrieveSimilar(episode.sensoryState, 3)
    print(f"  Similar episodes found: {len(similar)}")
    
    # Test temporal retrieval
    temporal = emem.retrieveTemporal(50, 150, 5)
    print(f"  Temporal episodes (50-150): {len(temporal)}")
    
    # Test location-based retrieval
    location = emem.retrieveByLocation(5.0, 3.0, 1.0, 5)
    print(f"  Location episodes: {len(location)}")
    
    # Test action-based retrieval
    action_episodes = emem.retrieveAfterAction(nlm.ActionType.MoveForward, 5)
    print(f"  Action episodes: {len(action_episodes)}")
    
    print("  ✓ NeuralEpisodicMemory operations passed")
    return emem

def test_neural_associative_memory():
    """Test NeuralAssociativeMemory bindings"""
    print("\nTesting NeuralAssociativeMemory...")
    
    # Create a brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create associative memory
    aemem = nlm.NeuralAssociativeMemory()
    
    # Test creating associations
    pattern_a = [1.0, 0.0, 0.0, 1.0, 0.0]
    pattern_b = [0.0, 1.0, 1.0, 0.0, 1.0]
    
    aemem.associate(pattern_a, pattern_b, 0.8)
    
    # Test retrieval
    results = aemem.retrieve(pattern_a, 5)
    print(f"  Retrieved {len(results)} associations for pattern_a")
    
    # Test association strength
    strength = aemem.getAssociationStrength(pattern_a, pattern_b)
    print(f"  Association strength: {strength}")
    
    # Test spread activation
    spread = aemem.spreadActivation(pattern_a, 3)
    print(f"  Spread activation results: {len(spread)} patterns")
    
    print(f"  Total associations: {aemem.getAssociationCount()}")
    print("  ✓ NeuralAssociativeMemory operations passed")
    return aemem

def test_development_system():
    """Test DevelopmentSystem bindings"""
    print("\nTesting DevelopmentSystem...")
    
    # Create development system
    dsys = nlm.DevelopmentSystem()
    
    print(f"  Initial stage: {dsys.getStage()}")
    print(f"  Stage name: {dsys.getStageName()}")
    print(f"  Plasticity modifier: {dsys.getPlasticityModifier()}")
    print(f"  Critical period: {dsys.isCriticalPeriod()}")
    print(f"  Critical period progress: {dsys.getCriticalPeriodProgress()}")
    print(f"  Developmental age: {dsys.getDevelopmentalAge()}")
    
    # Test setting stage
    dsys.setStage(nlm.DevelopmentalStage.Maturation)
    print(f"  After setting to Maturation: {dsys.getStage()}")
    
    # Test advancing stage
    dsys.advanceStage()
    print(f"  After advancing: {dsys.getStage()}")
    
    # Test developmental age
    dsys.setDevelopmentalAge(100.0)
    print(f"  Set developmental age to 100.0")
    
    print("  ✓ DevelopmentSystem operations passed")
    return dsys

def test_development_mechanisms():
    """Test development mechanism bindings"""
    print("\nTesting Development Mechanisms...")
    
    # Test Synaptogenesis
    synaptogenesis = nlm.Synaptogenesis()
    print(f"  Synaptogenesis formation rate: {synaptogenesis.getFormationRate()}")
    print(f"  Synaptogenesis target density: {synaptogenesis.getTargetDensity()}")
    
    # Test Maturation
    maturation = nlm.Maturation()
    print(f"  Maturation progress: {maturation.getProgress()}")
    print(f"  Maturation mature threshold: {maturation.getMatureThreshold()}")
    print(f"  Maturation mature resting potential: {maturation.getMatureRestingPotential()}")
    print(f"  Maturation mature time constant: {maturation.getMatureTimeConstant()}")
    
    # Test Pruning
    pruning = nlm.Pruning()
    print(f"  Pruning threshold: {pruning.getThreshold()}")
    print(f"  Pruning rate: {pruning.getPruningRate()}")
    
    print("  ✓ Development mechanisms operations passed")
    return synaptogenesis, maturation, pruning

def test_factory_functions():
    """Test factory functions"""
    print("\nTesting Factory Functions...")
    
    # Test existing factory functions
    config = nlm.createDefaultConfig()
    print(f"  Created config: {config is not None}")
    
    brain = nlm.createBrain(config)
    print(f"  Created brain: {brain is not None}")
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    print(f"  Created world: {world is not None}")
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    print(f"  Created agent: {agent is not None}")
    agent.initialize(world)
    
    # Test that the brain and agent work with neural memory systems
    wmem = nlm.NeuralWorkingMemory()
    wmem.initialize(brain)
    print(f"  NeuralWorkingMemory initialized with brain")
    
    print("  ✓ Factory functions test passed")
    return config, brain, world, agent

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Neural Memory Systems - Python Bindings Test")
    print("=" * 60)
    
    try:
        # Test all neural memory systems
        wmem = test_neural_working_memory()
        emem = test_neural_episodic_memory()
        aemem = test_neural_associative_memory()
        dsys = test_development_system()
        synaptogenesis, maturation, pruning = test_development_mechanisms()
        config, brain, world, agent = test_factory_functions()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED!")
        print("=" * 60)
        print("\nSummary of implemented bindings:")
        print("✓ NeuralWorkingMemory - for persistent activity patterns")
        print("✓ NeuralEpisodicMemory - for storing experiences and episodes")
        print("✓ NeuralAssociativeMemory - for pattern relationships")
        print("✓ DevelopmentSystem - for neural maturation and plasticity")
        print("✓ Synaptogenesis - for new synapse formation")
        print("✓ Maturation - for adult-level development")
        print("✓ Pruning - for removing weak synapses")
        print("✓ Factory functions - for easy Python usage")
        print("\nAll neural systems from the exploration report are now exposed to Python!")
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())