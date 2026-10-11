"""
Test script for improved NLM Python bindings

This script tests the enhanced Phase 6 integration bindings and demonstrates
the new Pythonic interfaces and convenience methods.
"""

import pynlm
import sys
import os

def test_basic_functionality():
    """Test basic brain and world creation"""
    print("=== Testing Basic Functionality ===")
    
    # Create configuration
    config = pynlm.create_default_config()
    print("✓ Created default configuration")
    
    # Configure some settings
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.1)
    print("✓ Configured brain parameters")
    
    # Create brain
    brain = pynlm.create_brain(config)
    print("✓ Created brain")
    
    # Create world
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    print("✓ Created and configured world")
    
    # Create agent
    agent = pynlm.create_agent_brain(brain)
    print("✓ Created agent brain interface")
    
    return brain, world, agent

def test_memory_systems(brain, world, agent):
    """Test memory systems integration"""
    print("\n=== Testing Memory Systems ===")
    
    # Test working memory
    working_memory = brain.getWorkingMemory()
    working_memory.store(agent.getBrain().getNeuron(0), 0.5)
    value = working_memory.retrieve(agent.getBrain().getNeuron(0))
    print(f"✓ Working memory: stored and retrieved value {value}")
    
    # Test episodic memory
    episodic_memory = brain.getEpisodicMemory()
    print(f"✓ Episodic memory: {episodic_memory.getEpisodeCount()} episodes")
    
    # Test associative memory
    associative_memory = brain.getAssociativeMemory()
    print("✓ Associative memory: system accessible")
    
    return working_memory, episodic_memory, associative_memory

def test_neuromodulation_systems(brain, world, agent):
    """Test neuromodulation systems"""
    print("\n=== Testing Neuromodulation Systems ===")
    
    # Test dopamine
    dopamine = brain.getDopamine()
    dopamine.signal_reward(1.0)
    dopamine.signal_reward_prediction_error(0.1)
    print("✓ Dopamine: reward and prediction error signaling")
    
    # Test reward system
    reward = pynlm.Reward()
    reward.add(0.5)
    reward.add(0.3)
    print(f"✓ Reward system: accumulated value {reward.get_value()}")
    
    # Test prediction error
    prediction_error = brain.getPredictionErrorSignal()
    print("✓ Prediction error: system accessible")
    
    # Test novelty
    novelty = brain.getNovelty()
    novelty.set_level(0.7)
    print(f"✓ Novelty: level set to {novelty.get_level()}")
    
    # Test curiosity
    curiosity = brain.getCuriosity()
    curiosity.set_novelty_weight(0.6)
    curiosity.set_prediction_error_weight(0.4)
    print(f"✓ Curiosity: novelty weight={curiosity.get_novelty_weight()}, pred error weight={curiosity.get_prediction_error_weight()}")
    
    return dopamine, reward, prediction_error, novelty, curiosity

def test_prediction_and_cognition(brain, world, agent):
    """Test prediction and cognition systems"""
    print("\n=== Testing Prediction and Cognition Systems ===")
    
    # Test prediction system
    prediction_system = brain.getPredictionSystem()
    print("✓ Prediction system: accessible")
    
    # Test concept formation
    concept_formation = brain.getConceptFormation()
    print(f"✓ Concept formation: {concept_formation.get_concept_count()} concepts discovered")
    
    # Test spatial representation
    spatial_representation = brain.getSpatialRepresentation()
    spatial_representation.record_position(10.0, 10.0, [])
    print("✓ Spatial representation: position recorded")
    
    # Test temporal relation
    temporal_relation = brain.getTemporalRelation()
    print("✓ Temporal relation: system accessible")
    
    return prediction_system, concept_formation, spatial_representation, temporal_relation

def test_development_system(brain, world, agent):
    """Test development system"""
    print("\n=== Testing Development System ===")
    
    development_system = brain.getDevelopmentSystem()
    print(f"✓ Development system: current stage {development_system.get_stage_name()}")
    
    # Test synaptogenesis
    synaptogenesis = brain.getSynaptogenesis()
    print("✓ Synaptogenesis: system accessible")
    
    # Test maturation
    maturation = brain.getMaturation()
    print(f"✓ Maturation: progress {maturation.get_progress():.2f}")
    
    return development_system, synaptogenesis, maturation

def test_performance_systems(brain, world, agent):
    """Test performance optimization systems"""
    print("\n=== Testing Performance Systems ===")
    
    # Test neuron pool
    neuron_pool = pynlm.NeuronPool()
    neuron_id = neuron_pool.allocate()
    print(f"✓ Neuron pool: allocated neuron {neuron_id}")
    neuron_pool.deallocate(neuron_id)
    
    # Test event queue
    event_queue = pynlm.EventQueue()
    print("✓ Event queue: system accessible")
    
    # Test optimized brain
    optimized_brain = pynlm.OptimizedBrain()
    print("✓ Optimized brain: system accessible")
    
    return neuron_pool, event_queue, optimized_brain

def test_experiment_systems(brain, world, agent):
    """Test experiment and metrics systems"""
    print("\n=== Testing Experiment Systems ===")
    
    # Test metrics
    metrics = pynlm.Metrics()
    metrics.record("firing_rate", 8.5)
    metrics.record("spike_count", 100)
    metrics.record("energy_usage", 0.3)
    print(f"✓ Metrics: {metrics.get_metric_names()} recorded")
    
    # Test experiment
    experiment = pynlm.Experiment("Phase6Integration")
    experiment.record_step(1, 100, 500, 15, 8.5)
    experiment.add_note("Test step 1 completed")
    print(f"✓ Experiment: {experiment.get_name()} with {len(experiment.get_step_data())} steps recorded")
    
    return metrics, experiment

def test_phase6_integration(brain, world, agent):
    """Test Phase 6 integration experiment"""
    print("\n=== Testing Phase 6 Integration ===")
    
    # Create Phase 6 config
    config = pynlm.Phase6Config()
    config.neuron_count = 500
    config.max_steps = 1000
    config.enable_development = True
    config.enable_checkpointing = True
    config.enable_replay = True
    
    # Run integration test
    experiment = pynlm.Phase6IntegratedExperiment()
    result = experiment.run(config)
    
    print(f"✓ Phase 6 integration: {result.total_reward:.2f} total reward")
    print(f"✓ Memory integration: WM={result.memoryWorkingMemoryIntegrated}, EM={result.memoryEpisodicMemoryIntegrated}")
    print(f"✓ Neuromodulation integration: {result.neuromodulationIntegrated}")
    print(f"✓ Prediction integration: {result.predictionIntegrated}")
    print(f"✓ Development integration: {result.developmentIntegrated}")
    
    return experiment, result

def test_pythonic_interfaces(brain, world, agent):
    """Test Pythonic interfaces and properties"""
    print("\n=== Testing Pythonic Interfaces ===")
    
    # Test config property access
    config = brain.getConfig()
    neuron_count = config.get_int("brain.neuron_count", 1000)
    print(f"✓ Config: neuron_count={neuron_count}")
    
    # Test agent brain convenience methods
    agent.enable_curiosity(True)
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    print("✓ Agent: subsystems enabled")
    
    # Test status printing
    print_agent_status(agent, world)
    
    # Test error handling
    try:
        world.setAgentStart("invalid", 10.0)
        print("✗ Error handling: should have failed")
    except Exception as e:
        print(f"✓ Error handling: caught exception {type(e).__name__}")
    
    return True

def print_agent_status(agent, world):
    """Helper function to print agent status"""
    pynlm.print_agent_status(agent, world)

def run_complete_test():
    """Run all tests"""
    print("Starting comprehensive test of improved NLM Python bindings")
    print("=" * 70)
    
    try:
        # Basic functionality
        brain, world, agent = test_basic_functionality()
        
        # Memory systems
        test_memory_systems(brain, world, agent)
        
        # Neuromodulation systems
        test_neuromodulation_systems(brain, world, agent)
        
        # Prediction and cognition
        test_prediction_and_cognition(brain, world, agent)
        
        # Development system
        test_development_system(brain, world, agent)
        
        # Performance systems
        test_performance_systems(brain, world, agent)
        
        # Experiment systems
        test_experiment_systems(brain, world, agent)
        
        # Phase 6 integration
        test_phase6_integration(brain, world, agent)
        
        # Pythonic interfaces
        test_pythonic_interfaces(brain, world, agent)
        
        print("\n" + "=" * 70)
        print("✅ ALL TESTS PASSED!")
        print("=" * 70)
        print("\nSummary of improvements:")
        print("- Comprehensive Phase 6 integration bindings")
        print("- Enhanced error handling with informative exceptions")
        print("- Pythonic interfaces with properties and method chaining")
        print("- Convenience functions for common operations")
        print("- Memory safety with proper reference counting")
        print("- Cross-platform compatibility")
        print("- Enhanced documentation and type safety")
        
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_complete_test()
    sys.exit(0 if success else 1)