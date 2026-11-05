# Implementation Plan: Fix Missing Python Bindings in pynlm

## Overview

This document provides a comprehensive implementation plan to address the critical gaps in the pynlm Python bindings. The current bindings expose only ~15% of the available NLM functionality, missing key Phase 6 cognitive and social learning systems.

## Phase 1: Critical Missing Bindings (Week 1)

### 1.1 Text Processing System - URGENT

**Problem**: Phase 6 text-as-a-sense capability completely inaccessible from Python
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Add to nlm namespace in bindings.cpp (around line 400)
.m.def("encodeText", &AgentBrain::encodeText, py::arg("text"),
      "Encode text into sparse neuron pattern for Phase 6 text processing")
.m.def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
      "Get internal (hidden) neuron firing rates for recall probe")
.m.def("getThoughtTrace", &AgentBrain::getThoughtTrace,
      "Get human-readable thought trace: perception → planning → action → outcome")

// Phase 6 diagnostic counters
.m.def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
      "Get vision-gated reflex bonus count")
.m.def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
      "Get vision-gated hazard suppression count")
.m.def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
      "Get winner-take-all pulse count")
.m.def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
      "Get planner override count")
.m.def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
      "Get planner agreement count")
.m.def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
      "Get planner decision count")
.m.def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
      "Get concept bias count")
.m.def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
      "Get self-bias count")
.m.def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
      "Get social bias count")
.m.def("getConceptCount", &AgentBrain::getConceptCount,
      "Get concept count")
.m.def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
      "Get self-model confidence")
.m.def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
      "Get social observation count")
```

**Test Code**:
```python
import pynlm

# Test Phase 6 text processing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Encode text (Phase 6 critical feature)
text_pattern = agent.encodeText("hello world")
print(f"Encoded pattern: {text_pattern}")

# Verify encoding is deterministic and unique
pattern1 = agent.encodeText("apple")
pattern2 = agent.encodeText("Apple")  # Should be same (case-insensitive)
pattern3 = agent.encodeText("zebra")  # Should be different

assert pattern1 == pattern2, "Text encoding must be case-insensitive and deterministic"
assert pattern1 != pattern3, "Different words must encode differently"

# Get internal firing rates
internal_rates = agent.getInternalFiringRates()
print(f"Internal firing rates: {len(internal_rates)} values")

# Get thought trace
trace = agent.getThoughtTrace()
print(f"Thought trace: {trace}")

# Get Phase 6 counters
print(f"Reflex bonus count: {agent.getReflexBonusCount()}")
print(f"Planner override count: {agent.getPlannerOverrideCount()}")
print(f"Concept count: {agent.getConceptCount()}")
```

### 1.2 Social Learning - URGENT

**Problem**: No social learning or observation capabilities
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
.m.def("observeOtherAgent", &AgentBrain::observeOtherAgent,
      py::arg("observed"), py::arg("before_vision"), py::arg("after_vision"),
      "Observe another agent's behavior for social learning")
```

**Test Code**:
```python
import pynlm

# Setup two agents for social learning
config = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config)
brain1.initialize()
agent1 = pynlm.createAgentBrain(brain1)

brain2 = pynlm.createBrain(config)
brain2.initialize()
agent2 = pynlm.createAgentBrain(brain2)

# Simulate agent2's action
observed_action = pynlm.MotorCommand.TurnLeft
before_vision = [0.1] * 24  # Example vision data
after_vision = [0.2] * 24   # Example vision data

# Agent1 observes agent2's behavior
agent1.observeOtherAgent(observed_action, before_vision, after_vision)
print("Social observation completed")
```

### 1.3 Working Memory Access - CRITICAL

**Problem**: No access to working memory traces
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
.m.def("getWorkingMemory", &Brain::getWorkingMemory,
      py::return_value_policy::reference_internal,
      "Access working memory system for transient pattern storage")
```

**Test Code**:
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory
working_memory = brain.getWorkingMemory()
print(f"Working memory accessible: {working_memory is not None}")

# Note: Working memory API would need to be exposed separately
# For now, demonstrate the capability exists
```

### 1.4 Configuration System Enhancement - IMPORTANT

**Problem**: Missing Config.set() and Config.get() methods
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Config.set() overloads for different types
.m.def("set", [](Config& self, const std::string& key, const ConfigValue& value) {
    self.set(key, value);
}, py::arg("key"), py::arg("value"),
"Set configuration parameter")

.m.def("set", [](Config& self, const std::string& key, int value) {
    self.set(key, value);
}, py::arg("key"), py::arg("value"))

.m.def("set", [](Config& self, const std::string& key, double value) {
    self.set(key, value);
}, py::arg("key"), py::arg("value"))

.m.def("set", [](Config& self, const std::string& key, bool value) {
    self.set(key, value);
}, py::arg("key"), py::arg("value"))

.m.def("set", [](Config& self, const std::string& key, const std::string& value) {
    self.set(key, value);
}, py::arg("key"), py::arg("value"))

// Config.get() method
.m.def("get", [](const Config& self, const std::string& key) {
    auto value = self.get<ConfigValue>(key);
    if (value) {
        return *value;
    }
    return py::none();
}, py::arg("key"), "Get configuration parameter")
```

**Test Code**:
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()

# Set configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)

# Get configuration parameters
neuron_count = config.get("brain.neuron_count", 1000)
print(f"Neuron count: {neuron_count}")

# Check if parameter exists
if config.has("brain.connection_probability"):
    connection_prob = config.get("brain.connection_probability")
    print(f"Connection probability: {connection_prob}")
```

---

## Phase 2: Core Systems Implementation (Weeks 2-3)

### 2.1 Neuromodulation Systems

**Problem**: Missing neuromodulation system access
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Brain neuromodulation access
.m.def("getDopamine", &Brain::getDopamine,
      py::return_value_policy::reference_internal,
      "Access dopamine neuromodulation system")

.m.def("getCuriosity", &Brain::getCuriosity,
      py::return_value_policy::reference_internal,
      "Access curiosity neuromodulation system")

.m.def("getNovelty", &Brain::getNovelty,
      py::return_value_policy::reference_internal,
      "Access novelty detection system")

.m.def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
      py::return_value_policy::reference_internal,
      "Access prediction error signal system")

// Direct neuromodulation application
.m.def("applyNeuromodulation", &Brain::applyNeuromodulation,
      py::arg("signal"),
      "Apply neuromodulatory signal to brain")
```

### 2.2 Memory Systems

**Problem**: Missing memory system access
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Brain memory system access
.m.def("getWorkingMemory", &Brain::getWorkingMemory,
      py::return_value_policy::reference_internal,
      "Access working memory system")

.m.def("getEpisodicMemory", &Brain::getEpisodicMemory,
      py::return_value_policy::reference_internal,
      "Access episodic memory system")

.m.def("getAssociativeMemory", &Brain::getAssociativeMemory,
      py::return_value_policy::reference_internal,
      "Access associative memory system")
```

### 2.3 Cognitive Systems

**Problem**: Missing cognitive system access
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Brain cognitive system access
.m.def("getPredictionSystem", &Brain::getPredictionSystem,
      py::return_value_policy::reference_internal,
      "Access prediction system")

.m.def("getPlanner", &Brain::getPlanner,
      py::return_value_policy::reference_internal,
      "Access neural planner system")

.m.def("getConceptFormation", &Brain::getConceptFormation,
      py::return_value_policy::reference_internal,
      "Access concept formation system")

.m.def("getAttention", &Brain::getAttention,
      py::return_value_policy::reference_internal,
      "Access attentional selection system")

.m.def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
      py::return_value_policy::reference_internal,
      "Access development system")
```

### 2.4 Plasticity Systems

**Problem**: Missing plasticity system access
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Brain plasticity system access
.m.def("getSpikeSystem", &Brain::getSpikeSystem,
      py::return_value_policy::reference_internal,
      "Access spike system")

.m.def("getSTDP", &Brain::getSTDP,
      py::return_value_policy::reference_internal,
      "Access STDP plasticity system")

.m.def("getHebbian", &Brain::getHebbian,
      py::return_value_policy::reference_internal,
      "Access Hebbian learning system")

.m.def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
      py::return_value_policy::reference_internal,
      "Access structural plasticity system")
```

### 2.5 Advanced Brain Statistics

**Problem**: Missing important brain statistics
**Files to modify**: `/workspace/app/python/bindings.cpp`

**Implementation**:

```cpp
// Additional brain statistics
.m.def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
      "Get total number of spikes generated")

.m.def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
      "Get number of pending spike events (in-flight)")

.m.def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
      "Get excitation/inhibition balance ratio")

.m.def("getAverageFiringRate", &Brain::getAverageFiringRate,
      "Get average firing rate across all neurons")
```

---

## Phase 3: Enhancement & Documentation (Week 4)

### 3.1 Enhanced Error Handling

**Problem**: Limited Python exception handling
**Implementation**:

```cpp
// Enhanced error handling for common operations
.m.def("safe_step", [](Brain& self, SimulationStep step) {
    try {
        self.step(step);
        return true;
    } catch (const std::exception& e) {
        throw py::runtime_error(std::string("Brain step failed: ") + e.what());
    }
}, py::arg("step"), "Safely perform brain step with error handling")

.m.def("safe_initialize", [](Brain& self) {
    try {
        if (!self.initialize()) {
            throw py::runtime_error("Brain initialization failed");
        }
        return true;
    } catch (const std::exception& e) {
        throw py::runtime_error(std::string("Brain initialization failed: ") + e.what());
    }
}, "Safely initialize brain with error handling")
```

### 3.2 Pythonic API Enhancements

**Problem**: Some APIs not Pythonic
**Implementation**:

```cpp
// Pythonic property access
.m.def("getTotalNeurons", [](const Brain& self) {
    return self.getTotalNeuronCount();
}, "Get total neuron count (Pythonic alias)")

.m.def("getTotalSynapses", [](const Brain& self) {
    return self.getTotalSynapseCount();
}, "Get total synapse count (Pythonic alias)")

// Context manager for brain reset
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    std::shared_ptr<Brain> get() { return brain_; }
private:
    std::shared_ptr<Brain> brain_;
};

.m.def("createBrainContext", [](std::shared_ptr<Brain> brain) {
    return std::make_shared<BrainContext>(brain);
}, py::arg("brain"), "Create brain context manager for safe operations")
```

### 3.3 Comprehensive Documentation

**Files to update**:
1. `docs/PYTHON_BINDINGS_GUIDE.md` - Comprehensive Python API guide
2. `docs/PHASE6_PYTHON_EXAMPLES.md` - Phase 6 specific examples
3. Update `easy_usage.md` and `HOW_TO_USE.md` with new features

---

## Build & Installation Updates

### 4.1 CMake Configuration Updates

**File**: `/workspace/app/python/CMakeLists.txt`

```cmake
# Add Python bindings version and features
set(PYNLM_VERSION "0.1.0")
set(PYNLM_FEATURES 
    "basic_simulation"
    "brain_control" 
    "world_simulation"
    "agent_interface"
    "phase6_text_processing"
    "neuromodulation"
    "memory_systems"
    "cognitive_systems"
    "plasticity_control"
)

# Export configuration for build
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pynlm_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pynlm_config.h"
    @ONLY
)
```

### 4.2 pyproject.toml Updates

**File**: `/workspace/app/pyproject.toml`

```toml
[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

[project.optional-dependencies]
extended = [
    "numpy>=1.20",
    "scipy>=1.7.0",
    "matplotlib>=3.4.0",
]

[project.scripts]
pynlm-demo = "pynlm.demo:main"
pynlm-explore = "pynlm.explore:main"
```

---

## Testing Strategy

### 5.1 Unit Tests for New Bindings

**File**: `/workspace/app/tests/test_python_bindings.cpp`

```cpp
namespace test_python_bindings {

void testTextEncoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test deterministic encoding
    auto pattern1 = nlm::AgentBrain::encodeText("apple");
    auto pattern2 = nlm::AgentBrain::encodeText("Apple");
    auto pattern3 = nlm::AgentBrain::encodeText("zebra");
    
    assert(pattern1 == pattern2); // Case insensitive
    assert(pattern1 != pattern3); // Different words
    assert(pattern1.size() == 4); // 4 neurons for text encoding
    
    std::cout << "    testTextEncoding passed" << std::endl;
}

void testNeuromodulationAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation system access
    auto dopamine = brain.getDopamine();
    auto curiosity = brain.getCuriosity();
    auto novelty = brain.getNovelty();
    auto prediction_error = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(prediction_error != nullptr);
    
    std::cout << "    testNeuromodulationAccess passed" << std::endl;
}

// Add tests for all new bindings...

} // namespace test_python_bindings
```

### 5.2 Integration Tests

**File**: `/workspace/app/tests/integration_test.py`

```python
#!/usr/bin/env python3
"""
Integration tests for enhanced pynlm Python bindings
"""
import pynlm
import sys

def test_complete_agent_with_phase6():
    """Test complete agent with Phase 6 capabilities"""
    print("Testing complete agent with Phase 6...")
    
    # Setup brain with extended configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with Phase 6 features
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Phase 6: Access thought trace
        trace = agent.getThoughtTrace()
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Phase 6: Access cognitive metrics
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        # Access Phase 6 counters
        if step % 20 == 0:
            print(f"Step {step}: Curiosity={curiosity_level:.3f}, "
                  f"Novelty={novelty_level:.3f}, "
                  f"Prediction Error={prediction_error:.3f}")
            print(f"  Thought trace: {trace}")
            print(f"  Counters: Planner overrides={agent.getPlannerOverrideCount()}, "
                  f"Concepts={agent.getConceptCount()}")
    
    print("Complete agent test passed!")

def test_text_processing_pipeline():
    """Test complete text processing pipeline"""
    print("Testing text processing pipeline...")
    
    # Setup brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Test various text encodings
    test_texts = ["hello", "world", "test", "python", "nlm"]
    patterns = {}
    
    for text in test_texts:
        pattern = agent.encodeText(text)
        patterns[text] = pattern
        print(f"Encoded '{text}' -> {pattern}")
    
    # Verify encoding properties
    assert len(patterns["hello"]) == len(patterns["world"])
    assert patterns["hello"] != patterns["world"]
    assert patterns["hello"] == patterns["hello"]  # Same text
    assert patterns["hello"].lower() == agent.encodeText("HELLO")  # Case insensitive
    
    # Get internal firing rates
    internal_rates = agent.getInternalFiringRates()
    print(f"Internal firing rates length: {len(internal_rates)}")
    
    print("Text processing pipeline test passed!")

if __name__ == "__main__":
    try:
        test_text_processing_pipeline()
        test_complete_agent_with_phase6()
        print("\nAll integration tests passed!")
        sys.exit(0)
    except Exception as e:
        print(f"\nIntegration test failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
```

---

## Performance Considerations

### 6.1 Memory Management

**Recommendations**:
1. Use smart pointers consistently
2. Implement copy-on-write where appropriate
3. Provide Python-style memory management
4. Add garbage collection hints

### 6.2 Data Transfer Optimizations

**Optimizations**:
1. Use pybind11::array_t for large numpy-like arrays
2. Implement zero-copy data transfers
3. Use vector-to-vector converters
4. Optimize for batch operations

### 6.3 Thread Safety

**Considerations**:
1. Brain systems may not be thread-safe
2. Implement locking mechanisms where needed
3. Document thread safety guarantees
4. Provide thread-safe wrapper functions

---

## Migration Guide

### 7.1 Existing Code Compatibility

**Backward Compatibility**: All existing code will continue to work
**New Features**: Can be adopted incrementally

**Migration Path**:
```python
# Old code (still works)
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# New code (extended capabilities)
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Extended configuration
brain.config.set("brain.neuron_count", 2000)
brain.config.set("neuromod.curiosity.enable", True)

# Access extended systems
working_memory = brain.getWorkingMemory()
neuron_count = brain.getTotalSpikeCount()
curiosity_level = brain.getCuriosity().getLevel()

# Phase 6 features
agent = pynlm.createAgentBrain(brain)
text_pattern = agent.encodeText("hello world")
thought_trace = agent.getThoughtTrace()
```

### 7.2 Documentation Updates

**Files to update**:
1. `docs/EXISTING_API.md` - Document currently working APIs
2. `docs/ENHANCED_API.md` - Document new enhanced APIs
3. `docs/MIGRATION_GUIDE.md` - Migration instructions
4. Update README with new examples

---

## Summary

This implementation plan addresses the critical gaps in the pynlm Python bindings, focusing on:

1. **Phase 6 Critical Features**: Text processing, social learning, cognitive access
2. **Core Systems**: Neuromodulation, memory, cognitive, plasticity systems
3. **Enhanced APIs**: Pythonic properties, error handling, context managers
4. **Comprehensive Testing**: Unit tests, integration tests, performance validation
5. **Backward Compatibility**: All existing code continues to work

**Total Missing Bindings to Implement**: ~60 methods across ~15 classes
**Priority Focus**: Phase 6 systems (text, cognition, social learning)
**Implementation Timeline**: 4 weeks (Phase 1-3)

This plan transforms pynlm from a basic skeleton into a comprehensive Python interface for the full NLM neural simulation framework.