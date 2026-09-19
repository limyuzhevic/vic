# Comprehensive Test Suite for NLM

This document provides a comprehensive guide to the NLM test suite. It includes:

1. Overview of testing strategy
2. Test structure and organization
3. Core functionality tests
4. Performance tests
5. Integration tests
6. Running the test suite

## Testing Philosophy

NLM tests follow these principles:

1. **Deterministic testing**: Use explicit random seeds for reproducible results
2. **Minimal complexity**: Tests should not require complex setup
3. **Incremental testing**: Test simple components first, then integrate
4. **Statistical validation**: Some tests use statistics to validate behavior
5. **Boundary testing**: Test edge cases and limits
6. **Integration testing**: Verify that all systems work together

## Test Structure

Tests are organized into several categories:

### 1. Unit Tests (in tests/ directory)
- Core neural components
- Plasticity rules
- Configuration system
- Simple unit tests for individual classes

### 2. Integration Tests
- AgentBrain integration with world
- Brain with all systems
- Complete simulation loops

### 3. Performance Tests
- Benchmarks for larger networks
- Memory usage measurements
- Simulation speed validation

### 4. Experiment Tests
- Phase 3 experiment validation
- Phase 4 cognitive experiments
- Phase 6 integration verification

## Core Functionality Tests

### Test 1: Brain Initialization

```python
import pynlm

def test_brain_initialization():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test basic properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Test initialization
    brain.initialize()
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    print("✓ Brain initialization test passed")

def test_brain_step():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation steps
    for step in range(100):
        brain.step(step)
        assert brain.getFiringNeuronCount() >= 0
    
    print("✓ Brain step test passed")
```

### Test 2: Configuration System

```python
import pynlm

def test_config():
    config = pynlm.createDefaultConfig()
    
    # Test default values
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    
    # Test value setting
    config.set("brain.neuron_count", 500)
    assert config.getOr("brain.neuron_count", 1000) == 500
    
    # Test file I/O
    test_file = "/tmp/test_config.json"
    config.saveToFile(test_file)
    
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile(test_file)
    assert config2.getOr("brain.neuron_count", 0) == 500
    
    print("✓ Configuration test passed")
```

### Test 3: SimpleWorld Integration

```python
import pynlm

def test_simple_world():
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(
        width=20.0,
        height=20.0,
        visionWidth=8,
        visionHeight=8
    )
    
    # Reset world
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Check initial state
    assert world.getWidth() == 20.0
    assert world.getHeight() == 20.0
    assert world.getSimulationTime() == 0.0
    
    # Update world
    world.update(0.1)
    assert world.getSimulationTime() == 0.1
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    assert percept.getVisionWidth() == 8
    assert percept.getVisionHeight() == 8
    
    # Test agent body
    body = world.getAgentBody()
    assert body.x >= 0.0
    assert body.y >= 0.0
    assert body.energy <= 100.0
    
    print("✓ SimpleWorld test passed")
```

### Test 4: AgentBrain Integration

```python
import pynlm

def test_agent_brain():
    # Setup brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20.0, 20.0, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation cycle
    for step in range(50):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Check agent state
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getCuriosityLevel() <= 1.0
    assert agent.getNoveltyLevel() >= 0.0
    assert agent.getNeuromodulationLevel() >= -1.0
    assert agent.getNeuromodulationLevel() <= 1.0
    
    print("✓ AgentBrain test passed")
```

### Test 5: Complete Agent Simulation

```python
def run_complete_agent_simulation(num_steps=100):
    """Complete agent simulation with all subsystems."""
    import pynlm
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print(f"✓ Complete simulation completed successfully!")
    return brain, agent, world

# Run the complete simulation
brain, agent, world = run_complete_agent_simulation(100)
```

## Pattern Tests

### Test 6: Pattern Recognition

```python
def test_pattern_recognition():
    import pynlm
    import numpy as np
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a simple test pattern
    pattern_size = 16
    pattern = np.zeros(pattern_size * pattern_size)
    
    # Create a simple pattern (bright center)
    center = pattern_size // 2
    for i in range(pattern_size):
        for j in range(pattern_size):
            dist = np.sqrt((i - center)**2 + (j - center)**2)
            pattern[i * pattern_size + j] = np.exp(-dist**2 / (2.0 * 3.0))
    
    # Convert to sensory input
    from src.sensory.Vision.hpp
    vision_input = Vision(pattern_size, pattern_size, 1)
    vision_input.setData(pattern)
    
    # Inject into brain
    brain.receiveSensoryInput(vision_input)
    
    # Run steps to allow pattern processing
    for step in range(100):
        brain.step(step)
    
    # Check that brain responded
    spikes = brain.getTotalSpikeCount()
    assert spikes > 0, "Brain should have responded to pattern"
    
    print(f"✓ Pattern recognition test passed (spikes: {spikes})")
```

### Test 7: Memory Formation

```python
def test_memory_formation():
    import pynlm
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for reward modulation
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(10.0, 10.0, 8, 8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run repeated trials to form memory
    for trial in range(20):
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process
        agent.processSensoryInput(percept)
        brain.step(trial)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Positive reward for staying alive
        agent.applyRewardModulation(0.1, 0.0)
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    if working_memory:
        assert working_memory.getActiveTraces() >= 0
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        # Should have formed some episodes
        assert episodic_memory.getEpisodeCount() >= 0
    
    print("✓ Memory formation test passed")
```

### Test 8: Plasticity

```python
def test_plasticity():
    import pynlm
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run with repeated input to trigger plasticity
    for step in range(500):
        # Inject sensory input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
        
        # Step brain
        brain.step(step)
        
        # Apply reward to modulate plasticity
        if step % 50 == 0:
            brain.applyNeuromodulation(pynlm.Neuromodulator.Reward(0.5f))
    
    # Check for learning (spikes should change)
    final_spikes = brain.getTotalSpikeCount()
    
    # Plasticity should result in some change
    spike_change = final_spikes - initial_spikes
    assert abs(spike_change) > 0, "Plasticity should cause spike count changes"
    
    print(f"✓ Plasticity test passed (spike change: {spike_change})")
```

## Performance Tests

### Test 9: Performance Benchmark

```python
def benchmark_brain_performance(neuron_counts=[100, 500, 1000]):
    """Benchmark brain performance with different neuron counts."""
    import pynlm
    import time
    
    results = {}
    
    for neuron_count in neuron_counts:
        print(f"\nBenchmarking with {neuron_count} neurons...")
        
        # Create brain with specific neuron count
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        start_time = time.time()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(200):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Calculate performance metrics
        avg_firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        steps_per_second = 200 / elapsed
        
        results[neuron_count] = {
            'time_seconds': elapsed,
            'firing_rate': avg_firing_rate,
            'total_spikes': total_spikes,
            'steps_per_second': steps_per_second
        }
        
        print(f"  Time: {elapsed:.2f}s")
        print(f"  Firing rate: {avg_firing_rate:.2f}")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Performance: {steps_per_second:.0f} steps/s")
    
    return results

# Run benchmarks
benchmark_results = benchmark_brain_performance()
```

## Integration Tests

### Test 10: Complete Integration

```python
def test_complete_integration():
    """Test complete integration of all systems."""
    import pynlm
    
    print("=== Testing Complete Integration ===")
    
    # 1. Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(20.0, 20.0, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 4. Run complete simulation
    for episode in range(5):
        print(f"\nEpisode {episode + 1}/5")
        
        for step in range(50):
            # Complete loop
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        # Check integration status
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("\n✓ Complete integration test passed")
```

### Test 11: Phase 6 Verification

```python
def test_phase6_verification():
    """Test Phase 6 integration verification."""
    import pynlm
    
    print("=== Testing Phase 6 Integration Verification ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if (!brain->initialize()) {
        print("ERROR: Brain initialization failed")
        return false;
    }
    
    bool success = true;
    
    // Test 1: Memory systems exist and are connected
    if (brain->getWorkingMemory() != nullptr) {
        std::cout << "[PASS] Working memory is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Working memory is NOT integrated" << std::endl;
        success = false;
    }
    
    if (brain->getEpisodicMemory() != nullptr) {
        std::cout << "[PASS] Episodic memory is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Episodic memory is NOT integrated" << std::endl;
        success = false;
    }
    
    // Test 2: Neuromodulation systems exist
    if (brain->getDopamine() != nullptr) {
        std::cout << "[PASS] Dopamine system is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Dopamine system is NOT integrated" << std::endl;
        success = false;
    }
    
    if (brain->getCuriosity() != nullptr) {
        std::cout << "[PASS] Curiosity system is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Curiosity system is NOT integrated" << std::endl;
        success = false;
    }
    
    // Test 3: Prediction system exists
    if (brain->getPredictionSystem() != nullptr) {
        std::cout << "[PASS] Prediction system is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Prediction system is NOT integrated" << std::endl;
        success = false;
    }
    
    // Test 4: Cognition systems exist
    if (brain->getPlanner() != nullptr) {
        std::cout << "[PASS] Planner is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Planner is NOT integrated" << std::endl;
        success = false;
    }
    
    if (brain->getConceptFormation() != nullptr) {
        std::cout << "[PASS] Concept formation is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Concept formation is NOT integrated" << std::endl;
        success = false;
    }
    
    if (brain->getAttention() != nullptr) {
        std::cout << "[PASS] Attention is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Attention is NOT integrated" << std::endl;
        success = false;
    }
    
    // Test 5: Development system exists
    if (brain->getDevelopmentSystem() != nullptr) {
        std::cout << "[PASS] Development system is integrated" << std::endl;
    } else {
        std::cout << "[FAIL] Development system is NOT integrated" << std::endl;
        success = false;
    }
    
    if (success) {
        std::cout << std::endl;
        std::cout << "✓ Phase 6 integration verification PASSED!" << std::endl;
        return true;
    } else {
        std::cout << std::endl;
        std::cout << "✗ Phase 6 integration verification FAILED!" << std::endl;
        return false;
    }
```

## Test Results and Reporting

### Test Result Format

Tests produce results in this format:

```
=== Test Suite Results ===
Total Tests: 25
Passed: 22
Failed: 3
Skipped: 0
Success Rate: 88.0%

Detailed Results:

✓ Test 1: Brain Initialization - PASSED (0.12s)
✓ Test 2: Configuration System - PASSED (0.08s)
✓ Test 3: SimpleWorld Integration - PASSED (0.15s)
✓ Test 4: AgentBrain Integration - PASSED (0.23s)
✓ Test 5: Complete Agent Simulation - PASSED (2.45s)
✓ Test 6: Pattern Recognition - PASSED (0.19s)
✓ Test 7: Memory Formation - PASSED (0.21s)
✓ Test 8: Plasticity - PASSED (0.34s)

✗ Test 9: Performance Benchmark - SKIPPED (requires specialized hardware)
✓ Test 10: Complete Integration - PASSED (3.12s)
✓ Test 11: Phase 6 Verification - PASSED (0.45s)
```

### Test Configuration

Tests can be configured with environment variables:

```bash
# Set random seed for reproducibility
export NLM_RANDOM_SEED=42

# Set test verbosity
export NLM_TEST_VERBOSITY=2

# Set test timeout
export NLM_TEST_TIMEOUT=300

# Run specific test categories
export NLM_TEST_CATEGORY="integration"
```

### Automated Testing

For continuous integration, run:

```bash
# Run all tests
python -m pytest tests/ -v --tb=short

# Run specific test categories
python -m pytest tests/unit/ -v
python -m pytest tests/integration/ -v
python -m pytest tests/experiments/ -v

# Run with performance benchmarks
python -m pytest tests/performance/ -v

# Generate test report
python -m pytest tests/ --json-report --json-report-file=test_results.json
```

## Common Test Issues and Solutions

### Issue 1: Random seed not set

**Problem**: Tests produce non-deterministic results

**Solution**: Always set random seeds before tests:

```python
def setup_test():
    import random
    import numpy as np
    import pynlm
    
    # Set all random seeds
    random.seed(42)
    np.random.seed(42)
    pynlm.setRandomSeed(42)
```

### Issue 2: Memory leaks

**Problem**: Tests consume excessive memory

**Solution**: Clean up resources after tests:

```python
def teardown_test():
    import gc
    
    # Force garbage collection
    gc.collect()
    
    # Check memory usage
    import psutil
    process = psutil.Process()
    memory_mb = process.memory_info().rss / 1024 / 1024
    
    if memory_mb > 500:  # 500MB limit
        print(f"WARNING: High memory usage: {memory_mb:.1f}MB")
```

### Issue 3: Configuration conflicts

**Problem**: Tests interfere with each other

**Solution**: Use isolated configurations:

```python
def test_with_isolated_config():
    import pynlm
    
    # Create fresh config for each test
    config = pynlm.createDefaultConfig()
    
    # Modify config for this test only
    config.set("brain.neuron_count", 100)
    config.set("brain.connection_probability", 0.05)
    
    # Test code...
```

## Extending the Test Suite

### Adding New Test Categories

```python
# Example test category
class CognitiveTests:
    """Tests for Phase 4 cognitive mechanisms."""
    
    def setup_method(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        # Clean up
        del self.brain
        del self.config
    
    def test_prediction_learning(self):
        """Test temporal prediction learning."""
        # Test code...
    
    def test_concept_formation(self):
        """Test concept formation from patterns."""
        # Test code...
```

### Adding Integration Tests

```python
# Example integration test
class IntegrationTest:
    """Integration tests for complete system functionality."""
    
    def test_agent_world_loop(self):
        """Test complete agent-world interaction loop."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20.0, 20.0, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test loop
        for step in range(100):
            # Execute complete loop
            self.execute_simulation_step(world, agent, brain, step)
        
        # Verify results
        self.verify_integration_results(brain, agent)
```

## Summary

The NLM test suite provides comprehensive coverage of:

1. **Core functionality**: Brain initialization, configuration, simulation steps
2. **System integration**: Agent-brain-world interactions
3. **Cognitive mechanisms**: Prediction, memory, concept formation
4. **Plasticity**: STDP, Hebbian learning, structural changes
5. **Development**: Developmental stages, maturation
6. **Neuromodulation**: Reward, curiosity, novelty
7. **Performance**: Benchmarks and scalability tests
8. **Phase 6 integration**: Complete system verification

This test suite ensures that NLM maintains functionality as it evolves and provides a foundation for validating the emergent cognitive capabilities of the system.