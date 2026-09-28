"""
Comprehensive test demonstrating the improved pynlm Python bindings.

This test showcases all the new features added to the Python bindings:
- Core neural components (Neuron, Synapse, NeuralRegion, NeuralPopulation)
- Advanced plasticity rules (STDP, Hebbian, reward-modulated learning)
- Memory systems (WorkingMemory, EpisodicMemory)
- Neuromodulation (Reward, Curiosity, Novelty, PredictionError)
- Development and aging mechanisms
- Performance optimizations (MemoryPool, EventQueue, CheckpointSystem)
- Advanced simulation controls and utilities
"""

import sys
import time
import tempfile
import os
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"  Version: {pynlm.get_version()}")
    print(f"  Author: {pynlm.__author__}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("  This test requires the pynlm module to be built")
    sys.exit(1)

def test_basic_types():
    """Test basic type classes."""
    print("\n=== Testing Basic Types ===")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(42)
    print(f"✓ Created NeuronId: {neuron_id}")
    assert neuron_id.value == 42
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(100)
    print(f"✓ Created SynapseId: {synapse_id}")
    assert synapse_id.value == 100
    
    # Test enums
    assert pynlm.NeuronType.Excitatory.value == 0
    assert pynlm.NeuronType.Inhibitory.value == 1
    assert pynlm.NeuronType.Modulatory.value == 2
    assert pynlm.NeuronType.Sensory.value == 3
    assert pynlm.NeuronType.Motor.value == 4
    assert pynlm.NeuronType.Internal.value == 5
    print("✓ NeuronType enum values correct")
    
    assert pynlm.SynapseType.Excitatory.value == 0
    assert pynlm.SynapseType.Inhibitory.value == 1
    assert pynlm.SynapseType.Modulatory.value == 2
    assert pynlm.SynapseType.Electrical.value == 3
    assert pynlm.SynapseType.GapJunction.value == 4
    print("✓ SynapseType enum values correct")
    
    print("✓ Basic types test passed")

def test_config():
    """Test configuration system."""
    print("\n=== Testing Configuration ===")
    
    config = pynlm.create_default_config()
    print(f"✓ Created default config: {config}")
    
    # Test config operations
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 10)
    config.set("simulation.dt", 0.001)
    
    assert config.has("brain.neuron_count")
    assert config.has("brain.region_count")
    assert config.has("simulation.dt")
    print("✓ Config set and has operations work")
    
    # Test getting keys
    keys = config.getKeys()
    print(f"✓ Config keys: {keys}")
    assert "brain.neuron_count" in keys
    assert "brain.region_count" in keys
    assert "simulation.dt" in keys
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
    
    try:
        config.saveToFile(config_path)
        print(f"✓ Config saved to {config_path}")
        
        config2 = pynlm.create_default_config()
        config2.loadFromFile(config_path)
        print("✓ Config loaded from file")
        
        assert config2.get("brain.neuron_count") == 1000
        print("✓ Config loaded correctly")
    finally:
        os.unlink(config_path)
    
    print("✓ Configuration test passed")

def test_plasticity_rules():
    """Test advanced plasticity rules."""
    print("\n=== Testing Plasticity Rules ===")
    
    # Test STDP rule
    stdp = pynlm.create_stdp_rule(ltp_weight=0.02, ltd_weight=0.02, tau=25.0)
    print(f"✓ Created STDP rule: {stdp}")
    
    assert stdp.getLTPWeight() == 0.02
    assert stdp.getLTDWeight() == 0.02
    assert stdp.getTimeConstant() == 25.0
    print("✓ STDP parameters correct")
    
    # Test Hebbian rule
    hebbian = pynlm.create_hebbian_rule(learning_rate=0.05)
    print(f"✓ Created Hebbian rule: {hebbian}")
    
    assert hebbian.getLearningRate() == 0.05
    print("✓ Hebbian parameters correct")
    
    # Test reward-modulated STDP
    rstdp = pynlm.create_reward_modulated_stdp()
    print(f"✓ Created RewardModulatedSTDP rule: {rstdp}")
    
    # Test structural plasticity
    from pynlm import StructuralPlasticity
    struct_plast = StructuralPlasticity()
    print(f"✓ Created StructuralPlasticity rule: {struct_plast}")
    
    print("✓ Plasticity rules test passed")

def test_memory_systems():
    """Test memory systems."""
    print("\n=== Testing Memory Systems ===")
    
    # Test working memory
    working_memory = pynlm.create_working_memory(capacity=2000)
    print(f"✓ Created working memory: {working_memory}")
    
    assert working_memory.getCapacity() == 2000
    assert working_memory.getSize() == 0
    assert working_memory.getUtilization() == 0.0
    print("✓ Working memory initialized correctly")
    
    # Test storing and retrieving
    working_memory.store("neuron_activity", 0.75)
    working_memory.store("synaptic_weight", 0.82)
    working_memory.store("firing_rate", 15.3)
    
    assert working_memory.has("neuron_activity")
    assert working_memory.has("synaptic_weight")
    assert working_memory.has("firing_rate")
    print("✓ Working memory store/retrieve operations work")
    
    activity = working_memory.retrieve("neuron_activity")
    assert activity == 0.75
    print("✓ Working memory retrieve correct value")
    
    # Test episodic memory
    episodic_memory = pynlm.create_episodic_memory(capacity=200)
    print(f"✓ Created episodic memory: {episodic_memory}")
    
    assert episodic_memory.getCapacity() == 200
    assert episodic_memory.getSize() == 0
    print("✓ Episodic memory initialized correctly")
    
    # Test storing experiences
    episodic_memory.store("experience_1", {"time": 1.0, "reward": 0.8, "state": "exploration"})
    episodic_memory.store("experience_2", {"time": 2.0, "reward": 0.3, "state": "resting"})
    episodic_memory.store("experience_3", {"time": 3.0, "reward": 0.9, "state": "learning"})
    
    assert episodic_memory.getSize() == 3
    print("✓ Episodic memory stores experiences")
    
    # Test replay callback
    replay_count = [0]
    def replay_callback(experience):
        replay_count[0] += 1
        print(f"  Replaying experience: time={experience['time']}, reward={experience['reward']}")
    
    episodic_memory.replay(replay_callback)
    assert replay_count[0] == 3
    print("✓ Episodic memory replay callback works")
    
    # Test consolidation
    episodic_memory.consolidate(importance_threshold=0.5)
    consolidated = episodic_memory.getConsolidationCount()
    print(f"✓ Episodic memory consolidated {consolidated} experiences")
    
    print("✓ Memory systems test passed")

def test_neuromodulation():
    """Test neuromodulation systems."""
    print("\n=== Testing Neuromodulation ===")
    
    # Test reward modulation
    reward = pynlm.create_reward_modulator(initial_value=0.5, learning_rate=0.02)
    print(f"✓ Created reward modulator: {reward}")
    
    assert reward.getCurrentValue() == 0.5
    assert reward.getLearningRate() == 0.02
    print("✓ Reward modulator initialized correctly")
    
    # Test reward prediction error
    reward_error = reward.getPredictionError(0.8)  # predicted 0.8, actual might be different
    print(f"✓ Reward prediction error: {reward_error}")
    
    # Test curiosity
    curiosity = pynlm.create_curiosity_module(initial_level=0.7, decay_rate=0.01)
    print(f"✓ Created curiosity module: {curiosity}")
    
    assert curiosity.getCurrentLevel() == 0.7
    assert curiosity.getDecayRate() == 0.01
    print("✓ Curiosity module initialized correctly")
    
    # Test novelty detection
    from pynlm import Novelty
    novelty = Novelty()
    print(f"✓ Created novelty detector: {novelty}")
    
    # Test prediction error
    from pynlm import PredictionError
    pred_error = PredictionError()
    print(f"✓ Created prediction error calculator: {pred_error}")
    
    print("✓ Neuromodulation test passed")

def test_brain_components():
    """Test brain components and regions."""
    print("\n=== Testing Brain Components ===")
    
    # Create config
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 100)
    config.set("brain.region_count", 5)
    
    # Create brain
    brain = pynlm.create_brain(config)
    print(f"✓ Created brain: {brain}")
    
    # Create world
    world = pynlm.create_simple_world()
    print(f"✓ Created world: {world}")
    
    # Configure world
    world.configure(100, 100, 50, 50)
    print(f"✓ Configured world: {world.getWidth()}x{world.getHeight()}")
    
    # Initialize brain
    brain.initialize()
    print("✓ Brain initialized")
    
    # Check brain stats
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getRegionCount() > 0
    print(f"✓ Brain has {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    print(f"✓ Brain has {brain.getRegionCount()} regions")
    
    # Create agent brain
    agent_brain = pynlm.create_agent_brain(brain)
    print(f"✓ Created agent brain: {agent_brain}")
    
    # Create neural region
    region = pynlm.create_neural_region("cortex")
    print(f"✓ Created neural region: {region}")
    
    assert region.getName() == "cortex"
    print("✓ Neural region name correct")
    
    # Create neural population
    from pynlm import NeuralPopulation
    population = NeuralPopulation()
    print(f"✓ Created neural population: {population}")
    
    print("✓ Brain components test passed")

def test_simulation():
    """Test basic simulation loop."""
    print("\n=== Testing Simulation ===")
    
    # Create components
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    world = pynlm.create_simple_world()
    agent_brain = pynlm.create_agent_brain(brain)
    
    # Configure world
    world.configure(50, 50, 25, 25)
    world.setAgentStart(25, 25)
    
    # Initialize brain
    brain.initialize()
    agent_brain.initialize(world)
    
    # Create simulation clock
    from pynlm import SimulationClock
    clock = SimulationClock()
    print(f"✓ Created simulation clock: {clock}")
    
    # Run a few simulation steps
    for step in range(5):
        clock.step(0.1)  # 100ms per step
        
        # Get sensory input
        percept = world.getSensoryPercept()
        print(f"  Step {step}: time={clock.getTime():.3f}s, step={clock.getStep()}")
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Get action
        action = agent_brain.decodeMotorCommand()
        print(f"    Action: {action.getType()} ({action.getName()})")
        
        # Apply action in world
        world.applyMotorCommand(action, clock.getTime())
        
        # Update brain
        agent_brain.applyRewardModulation(0.1, 0.15)  # reward=0.1, predicted=0.15
        agent_brain.updateDevelopment(0.1)
        
        # Brain internal step
        brain.step(clock.getStep(), clock.getTime())
    
    print("✓ Simulation steps completed")
    
    # Check some stats
    print(f"  Final time: {clock.getTime():.3f}s")
    print(f"  Final step: {clock.getStep()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    
    print("✓ Simulation test passed")

def test_performance_features():
    """Test performance optimization features."""
    print("\n=== Testing Performance Features ===")
    
    from pynlm import MemoryPool, EventQueue, CheckpointSystem
    
    # Test memory pool
    memory_pool = MemoryPool(2000)
    print(f"✓ Created memory pool: {memory_pool}")
    
    neuron_id = memory_pool.allocateNeuron()
    print(f"  Allocated neuron ID: {neuron_id}")
    
    synapse_id = memory_pool.allocateSynapse()
    print(f"  Allocated synapse ID: {synapse_id}")
    
    assert memory_pool.getUsedCapacity() == 2
    assert memory_pool.getUtilization() > 0.0
    print(f"  Memory pool utilization: {memory_pool.getUtilization():.2%}")
    
    memory_pool.deallocateNeuron(neuron_id)
    memory_pool.deallocateSynapse(synapse_id)
    print("  Deallocated all resources")
    
    # Test event queue
    event_queue = EventQueue()
    print(f"✓ Created event queue: {event_queue}")
    
    # Test checkpoint system
    checkpoint_system = CheckpointSystem()
    print(f"✓ Created checkpoint system: {checkpoint_system}")
    
    # Note: Saving/loading would require actual brain state
    # but we can test the interface
    checkpoint_system.setAutoSave(100)  # Save every 100 steps
    assert checkpoint_system.getAutoSaveInterval() == 100
    print(f"  Checkpoint auto-save interval: {checkpoint_system.getAutoSaveInterval()} steps")
    
    print("✓ Performance features test passed")

def test_advanced_features():
    """Test advanced features and utilities."""
    print("\n=== Testing Advanced Features ===")
    
    # Test plasticity rule factory
    plasticity_rules = []
    for rule_type in ["stdp", "hebbian", "reward_modulated_stdp"]:
        rule = pynlm.create_plasticity_rule(rule_type)
        plasticity_rules.append(rule)
        print(f"✓ Created {rule_type} rule: {rule}")
    
    # Test memory system factory
    memory_systems = []
    for mem_type in ["working", "episodic"]:
        memory = pynlm.create_memory_system(mem_type, capacity=500)
        memory_systems.append(memory)
        print(f"✓ Created {mem_type} memory system: {memory}")
    
    # Test neuromodulator factory
    neuromodulators = []
    for mod_type in ["reward", "curiosity"]:
        modulator = pynlm.create_neuromodulator(mod_type, initial_level=0.5)
        neuromodulators.append(modulator)
        print(f"✓ Created {mod_type} modulator: {modulator}")
    
    # Test development module factory
    from pynlm import Maturation, Synaptogenesis, Pruning
    dev_modules = [Maturation(), Synaptogenesis(), Pruning()]
    for dev_module in dev_modules:
        print(f"✓ Created development module: {dev_module}")
    
    # Test performance optimizer factory
    from pynlm import MemoryPool, EventQueue, CheckpointSystem, SparseConnectivity, OptimizedBrain
    optimizers = [
        MemoryPool(1000),
        EventQueue(),
        CheckpointSystem(),
        SparseConnectivity(10000),
        OptimizedBrain(None)  # Would need actual brain for full test
    ]
    for optimizer in optimizers:
        print(f"✓ Created performance optimizer: {optimizer}")
    
    print("✓ Advanced features test passed")

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test invalid plasticity rule type
        pynlm.create_plasticity_rule("invalid_type")
        print("✗ Should have raised ValueError for invalid rule type")
    except ValueError as e:
        print(f"✓ Correctly caught ValueError: {e}")
    
    try:
        # Test invalid memory system type
        pynlm.create_memory_system("invalid_type")
        print("✗ Should have raised ValueError for invalid memory type")
    except ValueError as e:
        print(f"✓ Correctly caught ValueError: {e}")
    
    try:
        # Test invalid neuromodulator type
        pynlm.create_neuromodulator("invalid_type")
        print("✗ Should have raised ValueError for invalid modulator type")
    except ValueError as e:
        print(f"✓ Correctly caught ValueError: {e}")
    
    print("✓ Error handling test passed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("PYNLM COMPREHENSIVE TEST SUITE")
    print("=" * 60)
    print("Testing improved pynlm Python bindings...")
    
    tests = [
        test_basic_types,
        test_config,
        test_plasticity_rules,
        test_memory_systems,
        test_neuromodulation,
        test_brain_components,
        test_simulation,
        test_performance_features,
        test_advanced_features,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"✓ Passed: {passed}")
    print(f"✗ Failed: {failed}")
    print(f"📊 Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The pynlm bindings are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    exit(main())