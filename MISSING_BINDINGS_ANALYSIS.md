# Technical Analysis: Missing Python Bindings in pynlm

## Critical Missing Bindings (Phase 6 Systems)

### 1. Text Processing System - URGENT
**Missing**: `AgentBrain::encodeText()` method

**Impact**: Phase 6 text-as-a-sense capability completely inaccessible from Python
**Usage**: `encoded_indices = pynlm.createAgentBrain(brain).encodeText("hello world")`
**Files**: `/workspace/app/src/agent/AgentBrain.hpp:97`, `/workspace/app/src/agent/AgentBrain.cpp`

```cpp
// In bindings.cpp, add:
.m.def("encodeText", &AgentBrain::encodeText, py::arg("text"),
      "Encode text into sparse neuron pattern for Phase 6 text processing")
```

**Phase 6 Context**: Text must be converted to neural patterns, not passed directly to brain

### 2. Internal Firing Rates - CRITICAL
**Missing**: `AgentBrain::getInternalFiringRates()` method

**Impact**: No access to internal neuron activity patterns for cognitive modeling
**Usage**: `internal_rates = agent.getInternalFiringRates()`
**Files**: `/workspace/app/src/agent/AgentBrain.hpp:102`, `.cpp implementation`

```cpp
.m.def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
      "Get internal (hidden) neuron firing rates for recall probe")
```

### 3. Thought Trace - CRITICAL
**Missing**: `AgentBrain::getThoughtTrace()` method

**Impact**: No access to agent's reasoning process transparency
**Usage**: `trace = agent.getThoughtTrace()`
**Files**: `/workspace/app/src/agent/AgentBrain.hpp:80`, `.cpp implementation`

```cpp
.m.def("getThoughtTrace", &AgentBrain::getThoughtTrace,
      "Get human-readable thought trace: perception → planning → action → outcome")
```

### 4. Advanced Brain Counters - PHASE 6 DEBUGGING
**Missing Multiple Diagnostic Counters**:
- `getReflexBonusCount()`, `getReflexSuppressCount()`, `getWtaPulseCount()`
- `getPlannerOverrideCount()`, `getPlannerAgreementCount()`, `getPlannerDecisionCount()`
- `getConceptBiasCount()`, `getSelfBiasCount()`, `getSocialBiasCount()`
- `getConceptCount()`, `getSelfModelConfidence()`, `getSocialObservationCount()`

**Impact**: No visibility into experimental measurements and Phase 6 mechanisms
**Files**: `/workspace/app/src/agent/AgentBrain.hpp:88-118`

### 5. Social Learning - PHASE 6 CORE
**Missing**: `AgentBrain::observeOtherAgent()` method

**Impact**: No social learning or observation capabilities
**Usage**: `agent.observeOtherAgent(command, before_vision, after_vision)`
**Files**: `/workspace/app/src/agent/AgentBrain.hpp:119`, `.cpp implementation`

```cpp
.m.def("observeOtherAgent", &AgentBrain::observeOtherAgent,
      py::arg("observed"), py::arg("before_vision"), py::arg("after_vision"),
      "Observe another agent's behavior for social learning")
```

---

## Neuromodulation Systems - Missing Core APIs

### 1. Direct Neuromodulator Access
**Missing**: `Brain::applyNeuromodulation()` method

**Impact**: Cannot directly apply neuromodulatory signals to brain
**Usage**: `brain.applyNeuromodulation(neuromodulator_signal)`
**Files**: `/workspace/app/src/brain/Brain.hpp:103`

```cpp
.m.def("applyNeuromodulation", &Brain::applyNeuromodulation, py::arg("signal"),
      "Apply neuromodulatory signal to brain")
```

### 2. Individual Neuromodulator Systems
**Missing**: Direct access to neuromodulation subsystems
**Needed**: Dopamine, Curiosity, Novelty, PredictionError system access
**Files**: `/workspace/app/src/neuromodulation/*.hpp`, `/workspace/app/src/brain/Brain.hpp:177-187`

```cpp
.m.def("getDopamine", &Brain::getDopamine,
      py::return_value_policy::reference_internal,
      "Access dopamine neuromodulation system")

.m.def("getCuriosity", &Brain::getCuriosity,
      py::return_value_policy::reference_internal,
      "Access curiosity neuromodulation system")
```

---

## Memory Systems - Missing Critical APIs

### 1. Working Memory - URGENT
**Missing**: `Brain::getWorkingMemory()` method

**Impact**: No access to working memory traces and pattern storage
**Usage**: `working_memory = brain.getWorkingMemory()`
**Files**: `/workspace/app/src/brain/Brain.hpp:145`

```cpp
.m.def("getWorkingMemory", &Brain::getWorkingMemory,
      py::return_value_policy::reference_internal,
      "Access working memory system for transient pattern storage")
```

### 2. Episodic Memory - CRITICAL
**Missing**: `Brain::getEpisodicMemory()` method

**Impact**: Cannot access stored experiences
**Usage**: `episodic_memory = brain.getEpisodicMemory()`
**Files**: `/workspace/app/src/brain/Brain.hpp:148`

### 3. Associative Memory - IMPORTANT
**Missing**: `Brain::getAssociativeMemory()` method

**Impact**: No access to pattern association capabilities
**Usage**: `assoc_memory = brain.getAssociativeMemory()`
**Files**: `/workspace/app/src/brain/Brain.hpp:151`

---

## Cognitive Systems - Missing Advanced Capabilities

### 1. Neural Planner - CRITICAL
**Missing**: `Brain::getPlanner()` method

**Impact**: No access to action planning capabilities
**Usage**: `planner = brain.getPlanner()`
**Files**: `/workspace/app/src/brain/Brain.hpp:161`

### 2. Concept Formation - CRITICAL
**Missing**: `Brain::getConceptFormation()` method

**Impact**: Cannot access concept discovery and similarity computation
**Usage**: `concepts = brain.getConceptFormation()`
**Files**: `/workspace/app/src/brain/Brain.hpp:164`

### 3. Attentional Selection - IMPORTANT
**Missing**: `Brain::getAttention()` method

**Impact**: No access to focus and attention mechanisms
**Usage**: `attention = brain.getAttention()`
**Files**: `/workspace/app/src/brain/Brain.hpp:167`

### 4. Prediction System - CRITICAL
**Missing**: `Brain::getPredictionSystem()` method

**Impact**: Cannot access predictive modeling and error computation
**Usage**: `prediction_system = brain.getPredictionSystem()`
**Files**: `/workspace/app/src/brain/Brain.hpp:156`

---

## Development System - Missing Core APIs

### 1. Development System Access
**Missing**: `Brain::getDevelopmentSystem()` method

**Impact**: No access to brain maturation and development
**Usage**: `dev_system = brain.getDevelopmentSystem()`
**Files**: `/workspace/app/src/brain/Brain.hpp:171`

### 2. Developmental Stage Control
**Partial Implementation**: `getDevelopmentalStage()` and `setDevelopmentalStage()` exist but need better Python integration

### 3. Individual Development Components
**Missing**: `Maturation`, `Synaptogenesis`, `Pruning` system access

---

## Plasticity Systems - Missing Advanced Control

### 1. Spike System Access
**Missing**: `Brain::getSpikeSystem()` method

**Impact**: Cannot control spike generation and neural dynamics
**Usage**: `spike_system = brain.getSpikeSystem()`
**Files**: `/workspace/app/src/brain/Brain.hpp:86`

### 2. Plasticity Rule Access
**Missing**: Direct access to STDP, Hebbian, StructuralPlasticity systems

```cpp
.m.def("getSTDP", &Brain::getSTDP,
      py::return_value_policy::reference_internal,
      "Access spike-timing dependent plasticity system")

.m.def("getHebbian", &Brain::getHebbian,
      py::return_value_policy::reference_internal,
      "Access Hebbian learning system")

.m.def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
      py::return_value_policy::reference_internal,
      "Access structural plasticity system")
```

---

## Statistics & Metrics - Missing Important Data

### 1. Spike Event Statistics
**Missing**: `Brain::getTotalSpikeCount()`, `Brain::getPendingSpikeEventCount()`

**Actually implemented**: These methods exist but are not exposed in bindings

### 2. Brain System Statistics
**Missing**: System-level statistics for debugging and analysis

```cpp
.m.def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
      "Get total number of spikes generated")

.m.def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
      "Get number of pending spike events (in-flight)")
```

---

## Configuration System - Missing Key Features

### 1. Config.set() Method
**Missing**: Direct parameter setting capability

**Current Implementation**: Only `loadFromFile()`, `has()`, `getKeys()`, `clear()`, `summary()`
**Files**: `/workspace/app/src/core/Config/Config.hpp:76-82`

**Required Implementation**:
```cpp
.m.def("set", [](Config& self, const std::string& key, const ConfigValue& value) {
    self.set(key, value);
}, py::arg("key"), py::arg("value"),
"Set configuration parameter")

// Template specializations for common types
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
```

### 2. Config.get() Method
**Missing**: Parameter retrieval capability

**Required Implementation**:
```cpp
.m.def("get", [](const Config& self, const std::string& key) {
    auto value = self.get<ConfigValue>(key);
    if (value) {
        // Convert ConfigValue to Python object
        return *value;
    }
    return py::none();
}, py::arg("key"), "Get configuration parameter")
```

---

## World Object System - Missing Enhancements

### 1. WorldObjectType Enum Access
**Missing**: Direct access to WorldObjectType enumeration

**Current Limitation**: Must reference via `pynlm.WorldObjectType` but not properly exported
**Files**: `/workspace/app/src/core/Types/Types.hpp:113-120`

**Required Implementation**:
```cpp
.m.def("WorldObjectType", &WorldObjectType,
      py::arg("value"), "World object type enumeration")

// Or expose the enum values directly
.m.attr("WorldObjectType_Empty", WorldObjectType::Empty)
.m.attr("WorldObjectType_Resource", WorldObjectType::Resource)
.m.attr("WorldObjectType_Hazard", WorldObjectType::Hazard)
.m.attr("WorldObjectType_Wall", WorldObjectType::Wall)
.m.attr("WorldObjectType_Marker", WorldObjectType::Marker)
```

### 2. WorldObject Factory
**Missing**: Better WorldObject creation API

**Required Implementation**:
```cpp
.m.def("WorldObject", [](float x, float y, WorldObjectType type, 
                         float value, float radius) {
    return WorldObject(x, y, type, value, radius);
}, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
   py::arg("radius") = 0.5f,
   "Create WorldObject (factory function)")
```

---

## Sensory Processing - Missing Advanced Features

### 1. Vision Processor
**Missing**: `VisionProcessor` class binding

**Current Limitation**: Only basic Vision class exposed, no processing capabilities
**Files**: `/workspace/app/src/sensory/VisionProcessor.hpp`, `VisionProcessor.cpp`

### 2. Audio Processor
**Missing**: `AudioProcessor` class binding

**Current Limitation**: Only basic Audio class exposed, no feature extraction
**Files**: `/workspace/app/src/sensory/AudioProcessor.hpp`, `AudioProcessor.cpp`

### 3. Internal Signals Processor
**Missing**: `InternalSignalsProcessor` class binding

**Current Limitation**: Only basic InternalSignals class exposed
**Files**: `/workspace/app/src/sensory/InternalSignalsProcessor.hpp`, `InternalSignalsProcessor.cpp`

---

## Phase 6 Enhancement Opportunities

### 1. Cognitive Model Access
**Missing**: `SelfModel` and `SocialLearning` system access

**Files**: `/workspace/app/src/agent/AgentBrain.hpp:237`, `.cpp implementation`

### 2. Experimental Measurement Access
**Missing**: Full access to Phase 6 experimental measurement counters

**Impact**: Cannot conduct Phase 6 experiments or validate implementations
**Usage**: Access counters like `getPlannerOverrideCount()`, `getConceptBiasCount()`, etc.

### 3. Advanced Text Processing
**Missing**: Full Phase 6 text-as-a-sense pipeline

**Required**: Text encoding, pattern recall, association learning

---

## Implementation Priority Matrix

| Priority | Binding Category | Impact | Implementation Complexity |
|----------|------------------|--------|-------------------------|
| **URGENT** | Phase 6 Text (`encodeText`) | Critical | Low |
| **URGENT** | Working Memory Access | High | Medium |
| **CRITICAL** | Neuromodulation Systems | High | High |
| **CRITICAL** | Cognitive Systems (Planner, Concepts) | High | High |
| **CRITICAL** | Development System | Medium | Medium |
| **IMPORTANT** | Plasticity Systems (STDP, Hebbian) | Medium | High |
| **IMPORTANT** | Episodic/Associative Memory | Low | Medium |
| **IMPROVEMENT** | Configuration (Config.set/get) | Medium | Low |
| **IMPROVEMENT** | World Object System | Low | Low |
| **ENHANCEMENT** | Sensory Processors | Low | Medium |
| **ENHANCEMENT** | Advanced Statistics | Low | Low |

---

## Recommended Implementation Sequence

### Phase 1: Critical Fixes (Week 1)
1. **Text Processing**: Bind `AgentBrain::encodeText()`
2. **Working Memory**: Bind `Brain::getWorkingMemory()`
3. **Configuration**: Add `Config.set()` and `Config.get()` methods
4. **World Objects**: Fix `WorldObjectType` enum access
5. **Thought Trace**: Bind `AgentBrain::getThoughtTrace()`

### Phase 2: Core Systems (Week 2-3)
1. **Neuromodulation**: Bind all neuromodulator systems
2. **Memory Systems**: Bind episodic and associative memory
3. **Cognitive Systems**: Bind planner, concept formation, attention
4. **Development System**: Full development system access
5. **Advanced Counters**: Bind all Phase 6 measurement counters

### Phase 3: Advanced Features (Week 4)
1. **Plasticity Systems**: Full plasticity system access
2. **Sensory Processing**: Bind vision/audio/internal processors
3. **Brain Statistics**: Add spike counts, system metrics
4. **Social Learning**: Bind `observeOtherAgent()` and social systems
5. **Enhancement**: Advanced features and documentation

---

## Code Generation Recommendations

To implement all missing bindings efficiently:

```python
# Generated Python API template:
class NLM:
    class Brain:
        def get_working_memory(self) -> WorkingMemory: ...
        def get_prediction_system(self) -> PredictionSystem: ...
        def get_planner(self) -> NeuralPlanner: ...
        def get_concept_formation(self) -> ConceptFormation: ...
        def get_attention(self) -> AttentionalSelection: ...
        def get_development_system(self) -> DevelopmentSystem: ...
        def get_dopamine(self) -> Dopamine: ...
        def get_curiosity(self) -> Curiosity: ...
        def get_novelty(self) -> Novelty: ...
        def get_prediction_error(self) -> PredictionError: ...
    
    class AgentBrain:
        def encode_text(self, text: str) -> List[int]: ...
        def get_internal_firing_rates(self) -> List[float]: ...
        def get_thought_trace(self) -> str: ...
        def observe_other_agent(self, observed: MotorCommand, 
                               before_vision: List[float], 
                               after_vision: List[float]): ...
        @property
        def reflex_bonus_count(self) -> int: ...
        @property
        def planner_override_count(self) -> int: ...
        # ... other counters
    
    class Config:
        def set(self, key: str, value: Union[int, float, bool, str, List]) -> None: ...
        def get(self, key: str, default=None) -> Union[int, float, bool, str, List]: ...
```

This comprehensive analysis shows that while the pynlm bindings provide basic neural simulation capabilities, they are missing ~60% of the advanced features needed for full NLM functionality, particularly the Phase 6 cognitive and social learning systems.