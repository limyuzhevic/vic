# NLM Python API - Critical Improvements Priority List

## Executive Summary

**Current Status**: Python bindings expose only 43/120 features (35.8%) with critical systems (memory, prediction, cognition) completely disconnected.

**Phase 6 Mandate**: "The primary goal of Phase 6 is to make existing systems work together as a coherent whole, not to add more disconnected components."

**Action Required**: Immediately expose missing C++ systems through Python bindings.

## PRIORITY 1: CONNECT EXISTING C++ SYSTEMS (Immediate - Phase 6 Mandate)

### 1.1 Memory Systems Integration (Critical - 4/20 disconnected)

**Files to modify**: python/bindings.cpp

**Required changes**:

```cpp
// Expose WorkingMemory
py::class_<NeuralWorkingMemory>(m, "WorkingMemory")
    .def("store", &Brain::getWorkingMemory)  // Add to Brain
    .def("retrieve", ...)
    .def("contains", ...)
    .def("clear", ...)

// Expose EpisodicMemory  
py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory")
    .def("storeEpisode", &Brain::getEpisodicMemory)  // Add to Brain
    .def("retrieveEpisode", ...)
    .def("getRecentEpisodes", ...)
    .def("consolidate", ...)

// Expose SemanticMemory
py::class_<SemanticMemory>(m, "SemanticMemory")
    .def("storeFact", &Brain::getSemanticMemory)  // Add to Brain
    .def("retrieveFact", ...)

// Expose ProceduralMemory
py::class_<ProceduralMemory>(m, "ProceduralMemory")
    .def("learnSkill", &Brain::getProceduralMemory)  // Add to Brain
    .def("getSkill", ...)
    .def("updateProficiency", ...)
```

**C++ Changes Required**:
1. Brain.cpp: Implement `getWorkingMemory()`, `getEpisodicMemory()`, `getSemanticMemory()`, `getProceduralMemory()` (currently return nullptr)
2. Add memory system access methods to Brain class in Brain.hpp

### 1.2 Prediction System Integration (Critical - 0/10 disconnected)

**Files to modify**: python/bindings.cpp, src/prediction/PredictionSystem.hpp

**Required changes**:

```cpp
// Expose PredictionSystem
py::class_<PredictionSystem>(m, "PredictionSystem")
    .def("train", &Brain::getPredictionSystem)  // Add to Brain
    .def("getError", ...)
    .def("getConfidence", ...)
    .def("getNextStatePrediction", ...)
    .def("getHistory", ...)
```

**C++ Changes Required**:
1. Brain.cpp: Implement `getPredictionSystem()` (currently returns nullptr)
2. Connect PredictionSystem to sensory processing in Brain::step()

### 1.3 Cognitive System Integration (Critical - 0/20 disconnected)

**Files to modify**: python/bindings.cpp, src/cognition/

**Required changes**:

```cpp
// Expose NeuralPlanner
py::class_<NeuralPlanner>(m, "NeuralPlanner")
    .def("setGoal", &Brain::getPlanner)  // Add to Brain
    .def("plan", ...)
    .def("getActionSequence", ...)

// Expose ConceptFormation
py::class_<ConceptFormation>(m, "ConceptFormation")
    .def("extractFeatures", &Brain::getConceptFormation)  // Add to Brain
    .def("getConcepts", ...)

// Expose SelfModel
py::class_<SelfModel>(m, "SelfModel")
    .def("update", &Brain::getSelfModel)  // Add to Brain
    .def("getBeliefs", ...)
    .def("getGoals", ...)
```

**C++ Changes Required**:
1. Brain.cpp: Implement all `get*()` methods (currently return nullptr)
2. Connect cognitive systems to brain loop in Brain::step()

### 1.4 Complete Neuromodulation Integration (High - 5/15 partial)

**Files to modify**: python/bindings.cpp, src/neuromodulation/

**Required changes**:

```cpp
// Expose complete neuromodulation system
py::class_<Dopamine>(m, "Dopamine")
    .def("setLevel", &Brain::getDopamine)  // Add to Brain
    .def("applyToPlasticity", ...)
    .def("getLevel", ...)

// Expose other neuromodulators (currently stubs)
py::class_<Acetylcholine>(m, "Acetylcholine")
    .def("setLevel", &Brain::getAcetylcholine)  // Add to Brain
    .def("applyToAttention", ...)
```

**C++ Changes Required**:
1. Implement stub neuromodulators (ACh, NE, 5-HT)
2. Expand dopamine effects beyond STDP scaling
3. Connect neuromodulation to memory and attention systems

### 1.5 Development System Integration (Medium - 4/10 minimal)

**Files to modify**: python/bindings.cpp, src/development/

**Required changes**:

```cpp
// Expose DevelopmentSystem
py::class_<DevelopmentSystem>(m, "DevelopmentSystem")
    .def("update", &Brain::getDevelopmentSystem)  // Add to Brain
    .def("setStage", &Brain::setDevelopmentalStage)
    .def("getStage", &Brain::getDevelopmentalStage)
    .def("getAge", ...)
    .def("getPlasticityRate", ...)
```

**C++ Changes Required**:
1. Expand development effects beyond structural plasticity
2. Connect development to neuromodulation, memory, attention

### 1.6 Persistence Integration (Critical - 1/10 stubs)

**Files to modify**: python/bindings.cpp, src/performance/

**Required changes**:

```cpp
// Expose CheckpointSystem
py::class_<CheckpointSystem>(m, "CheckpointSystem")
    .def("saveBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    })
    .def("loadBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    })
```

**C++ Changes Required**:
1. Implement Brain::save() and Brain::load() (currently stubs)
2. Connect CheckpointSystem to Brain

## PRIORITY 2: ENHANCE PYTHON API (Medium-term)

### 2.1 Direct Neural Access (Medium)

**Files to modify**: python/bindings.cpp

**Required changes**:

```cpp
// Expose Neuron direct access
py::class_<Neuron>(m, "Neuron")
    .def("getId", ...)
    .def("getType", ...)
    .def("getPotential", ...)
    .def("getRegion", ...)
    .def("getPopulation", ...)

// Neuron lookup utilities
m.def("getNeuron", &Brain::getNeuron, py::arg("id"))
```

### 2.2 Enhanced Statistics (Medium)

**Files to modify**: python/bindings.cpp

**Required changes**:

```cpp
// Network analysis functions
m.def("computeNetworkTopology", &Brain::computeNetworkTopology)
m.def("computeClusteringCoefficient", ...)
m.def("computePathLength", ...)
m.def("detectCommunities", ...)
```

### 2.3 Advanced Simulation Control (Low)

**Files to modify**: python/bindings.cpp

**Required changes**:

```cpp
// Simulation control utilities
m.def("setTimestep", &Brain::setTimestep)
m.def("enableMultithreading", &Brain::enableMultithreading)
m.def("setEventDrivenMode", &Brain::setEventDrivenMode)
```

## PRIORITY 3: PERFORMANCE AND SCALE (Low-term)

### 3.1 Performance Infrastructure Integration (Low)

**Files to modify**: python/bindings.cpp, src/performance/

**Required changes**:

```cpp
// Performance utilities
m.def("getMemoryPoolStats", &Performance::getMemoryPoolStats)
m.def("getEventQueueStats", &Performance::getEventQueueStats)
m.def("optimizeConnectivity", &Performance::optimizeConnectivity)
```

### 3.2 Advanced Configuration (Low)

**Files to modify**: python/bindings.cpp, src/core/Config/Config.hpp

**Required changes**:

```cpp
// Research mode configuration
m.def("createResearchConfig", []() {
    auto config = std::make_shared<Config>();
    // Set research-specific parameters
    return config;
})
```

## PRIORITY 4: VALIDATION AND TESTING (Long-term)

### 4.1 Experiment Infrastructure (Low)

**Files to modify**: python/bindings.cpp, src/experiments/

**Required changes**:

```cpp
// Experiment utilities
m.def("runExperiment", &ExperimentRunner::run)
m.def("collectMetrics", &ExperimentRunner::collectMetrics)
m.def("compareExperiments", &ExperimentRunner::compare)
```

### 4.2 Documentation and Examples (Low)

**Files to create**:
- `examples/advanced_usage.py`
- `examples/research_workflows.py`
- `docs/API_REFERENCE.md`
- `docs/RESEARCH_GUIDES.md`

## IMMEDIATE ACTION PLAN (Next 2 weeks)

### Week 1: System Integration
1. **Brain.cpp**: Implement memory system accessors (WorkingMemory, EpisodicMemory, etc.)
2. **Brain.cpp**: Implement prediction system accessor
3. **Brain.cpp**: Implement cognitive system accessors (NeuralPlanner, ConceptFormation, etc.)
4. **Brain.cpp**: Implement neuromodulation accessors
5. **Brain.cpp**: Implement development system accessor
6. **Brain.cpp**: Implement persistence accessors (save/load)

### Week 2: Python API Enhancement
1. **python/bindings.cpp**: Add all missing C++ system exposes
2. **python/bindings.cpp**: Add direct neural access APIs
3. **python/bindings.cpp**: Add enhanced statistics functions
4. **Create**: Documentation and examples

## Files to Modify (Immediate)

### High Priority (Week 1):
- `src/brain/Brain.cpp` - Implement system accessors
- `src/brain/Brain.hpp` - Add missing method declarations
- `python/bindings.cpp` - Expose all C++ systems

### Medium Priority (Week 2):
- `src/prediction/PredictionSystem.hpp` - Ensure complete implementation
- `src/cognition/` - Ensure complete implementation  
- `src/neuromodulation/` - Implement missing neuromodulators

### Low Priority (Follow-up):
- `src/performance/` - Integration with Python
- `src/experiments/` - Experiment framework integration
- `python/CMakeLists.txt` - Additional dependencies

## Expected Results

After implementing these changes:
- **Integration Score**: Increase from 35.8% to >80%
- **Critical Systems**: All memory, prediction, cognition systems connected
- **Python API**: Complete exposure of all C++ capabilities
- **Research Ready**: Full experimental infrastructure available

**Time to Complete**: 2-3 weeks for Phase 6 integration
**Risk Level**: LOW (all changes are exposing existing functionality)

## Dependencies

No external dependencies required. All changes are exposing existing C++ systems that are already implemented but disconnected.