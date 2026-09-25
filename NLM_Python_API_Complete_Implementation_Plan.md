# NLM Python API - Complete Implementation Priority List

## Executive Summary

**Mission**: Transform NLM from a disconnected collection of components (35.8% exposed) into a coherent artificial brain (>80% integrated).

**Phase 6 Mandate**: "Make existing systems work together as a coherent whole, not add more disconnected components."

**Current State**: 43 critical systems completely disconnected from Python API and brain loop.

## IMPLEMENTATION ROADMAP

### WEEK 1: CRITICAL SYSTEM INTEGRATION (Phase 6 Immediate Goals)

#### 1.1 Memory Systems Integration (4/20 systems completely disconnected)

**Files to Modify**:
- `src/brain/Brain.cpp` - Implement memory system accessors
- `src/brain/Brain.hpp` - Add memory system method declarations
- `python/bindings.cpp` - Expose memory systems through Python

**Specific Changes**:

**A. WorkingMemory Integration**:
```cpp
// Brain.cpp: Implement getWorkingMemory()
NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl->workingMemory) {
        pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    }
    return pImpl->workingMemory.get();
}

// python/bindings.cpp: Expose through Python
py::class_<NeuralWorkingMemory>(m, "WorkingMemory")
    .def("store", &NeuralWorkingMemory::store)
    .def("retrieve", &NeuralWorkingMemory::retrieve)
    .def("contains", &NeuralWorkingMemory::contains)
    .def("clear", &NeuralWorkingMemory::clear)
    .def("decay", &NeuralWorkingMemory::decay)
    .def("getCapacity", &NeuralWorkingMemory::getCapacity)
    .def("getCurrentSize", &NeuralWorkingMemory::getCurrentSize);
```

**Integration Points**:
1. `Brain::step()` - Update working memory with sensory input
2. `AgentBrain::processSensoryInput()` - Store percepts in working memory
3. `Development::update()` - Manage working memory decay

**B. EpisodicMemory Integration**:
```cpp
// Brain.cpp: Implement getEpisodicMemory()
NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (!pImpl->episodicMemory) {
        pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    }
    return pImpl->episodicMemory.get();
}

// python/bindings.cpp: Expose through Python
py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory")
    .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode)
    .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode)
    .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes)
    .def("consolidate", &NeuralEpisodicMemory::consolidate);
```

**Integration Points**:
1. `Brain::step()` - Replay episodic memory during working phases
2. `AgentBrain::applyRewardModulation()` - Update episodic memory strength
3. `Development::update()` - Consolidate important episodes

**C. SemanticMemory Integration**:
```cpp
// Brain.cpp: Implement getSemanticMemory()
SemanticMemory* Brain::getSemanticMemory() {
    if (!pImpl->semanticMemory) {
        pImpl->semanticMemory = std::make_unique<SemanticMemory>();
    }
    return pImpl->semanticMemory.get();
}
```

**Integration Points**:
1. `Brain::step()` - Update semantic knowledge from episodic memory
2. `AgentBrain::processSensoryInput()` - Extract semantic patterns
3. `ConceptFormation::update()` - Update semantic representations

**D. ProceduralMemory Integration**:
```cpp
// Brain.cpp: Implement getProceduralMemory()
ProceduralMemory* Brain::getProceduralMemory() {
    if (!pImpl->proceduralMemory) {
        pImpl->proceduralMemory = std::make_unique<ProceduralMemory>();
    }
    return pImpl->proceduralMemory.get();
}
```

**Integration Points**:
1. `Brain::step()` - Practice procedural skills
2. `AgentBrain::decodeMotorCommand()` - Select procedural actions
3. `Development::update()` - Refine procedural memory

#### 1.2 Prediction System Integration (0/10 systems connected)

**Files to Modify**:
- `src/prediction/PredictionSystem.hpp` - Ensure complete implementation
- `src/brain/Brain.cpp` - Implement getPredictionSystem()
- `python/bindings.cpp` - Expose prediction system

**Specific Changes**:

**A. PredictionSystem Integration**:
```cpp
// Brain.cpp: Implement getPredictionSystem()
PredictionSystem* Brain::getPredictionSystem() {
    if (!pImpl->predictionSystem) {
        pImpl->predictionSystem = std::make_unique<PredictionSystem>(this);
    }
    return pImpl->predictionSystem.get();
}

// python/bindings.cpp: Expose through Python
py::class_<PredictionSystem>(m, "PredictionSystem")
    .def("train", &PredictionSystem::train)
    .def("setTarget", &PredictionSystem::setTarget)
    .def("update", &PredictionSystem::update)
    .def("getError", &PredictionSystem::getError)
    .def("getConfidence", &PredictionSystem::getConfidence)
    .def("getNextStatePrediction", &PredictionSystem::getNextStatePrediction)
    .def("getHistory", &PredictionSystem::getHistory)
    .def("getModelWeights", &PredictionSystem::getModelWeights);
```

**Integration Points**:
1. `Brain::step()` - Update prediction system with current state
2. `AgentBrain::applyRewardModulation()` - Learn from prediction error
3. `Neuromodulation::apply()` - Use prediction error for learning

#### 1.3 Cognitive System Integration (0/20 systems disconnected)

**Files to Modify**:
- `src/cognition/` - Ensure all cognitive systems are complete
- `src/brain/Brain.cpp` - Implement all cognitive system accessors
- `python/bindings.cpp` - Expose all cognitive systems

**Specific Changes**:

**A. NeuralPlanner Integration**:
```cpp
// Brain.cpp: Implement getPlanner()
NeuralPlanner* Brain::getPlanner() {
    if (!pImpl->neuralPlanner) {
        pImpl->neuralPlanner = std::make_unique<NeuralPlanner>(this);
    }
    return pImpl->neuralPlanner.get();
}

// python/bindings.cpp: Expose through Python
py::class_<NeuralPlanner>(m, "NeuralPlanner")
    .def("setGoal", &NeuralPlanner::setGoal)
    .def("setConstraint", &NeuralPlanner::setConstraint)
    .def("plan", &NeuralPlanner::plan)
    .def("getActionSequence", &NeuralPlanner::getActionSequence)
    .def("getExpectedOutcome", &NeuralPlanner::getExpectedOutcome);
```

**B. ConceptFormation Integration**:
```cpp
// Brain.cpp: Implement getConceptFormation()
ConceptFormation* Brain::getConceptFormation() {
    if (!pImpl->conceptFormation) {
        pImpl->conceptFormation = std::make_unique<ConceptFormation>(this);
    }
    return pImpl->conceptFormation.get();
}
```

**C. SelfModel Integration**:
```cpp
// Brain.cpp: Implement getSelfModel()
SelfModel* Brain::getSelfModel() {
    if (!pImpl->selfModel) {
        pImpl->selfModel = std::make_unique<SelfModel>(this);
    }
    return pImpl->selfModel.get();
}
```

**Integration Points**:
1. `Brain::step()` - Update cognitive systems with current state
2. `AgentBrain::processSensoryInput()` - Feed percepts to concept formation
3. `Development::update()` - Update self model and social learning
4. `Prediction::update()` - Use spatial/temporal representations

#### 1.4 Neuromodulation System Integration (5/15 partial)

**Files to Modify**:
- `src/neuromodulation/` - Implement missing neuromodulators
- `python/bindings.cpp` - Expose complete neuromodulation system

**Specific Changes**:

**A. Complete Dopamine Implementation**:
```cpp
class Dopamine : public Neuromodulator {
public:
    void setLevel(float level) override { currentLevel_ = level; }
    void applyToPlasticity() override {
        // Expand beyond current STDP scaling
        modulateHebbianLearning(currentLevel_ > 0.5f);
        modulateSTDP(currentLevel_ > 0.3f);
        modulateStructuralPlasticity(currentLevel_ > 0.7f);
    }
    void applyToMemoryConsolidation() override {
        // Strengthen important memories
        strengthenEpisodicMemory(currentLevel_);
    }
    void applyToAttention() override {
        // Allocate attention based on reward relevance
        allocateAttention(currentLevel_);
    }
    void applyToMotivation() override {
        // Drive action selection
        increaseMotivation(currentLevel_);
    }
};
```

**B. Implement Missing Neuromodulators**:
```cpp
// Acetylcholine
class Acetylcholine : public Neuromodulator {
public:
    void setLevel(float level) override { currentLevel_ = level; }
    void applyToAttention(float strength) override {
        attentionalGain_ = strength;
    }
    void applyToSensoryProcessing(float gain) override {
        sensoryGain_ = gain;
    }
    void applyToMemoryEncoding(float rate) override {
        encodingRate_ = rate;
    }
};

// Norepinephrine
class Norepinephrine : public Neuromodulator {
public:
    void setLevel(float level) override { currentLevel_ = level; }
    void applyToArousal(float effect) override {
        arousalLevel_ = effect;
    }
    void applyToAlertness(bool enable) override {
        alertness_ = enable;
    }
    void applyToWorkingMemoryCapacity(int capacity_increase) override {
        workingMemoryCapacity_ += capacity_increase;
    }
};

// Serotonin
class Serotonin : public Neuromodulator {
public:
    void setLevel(float level) override { currentLevel_ = level; }
    void applyToMood(float influence) override {
        mood_ = level_ * influence;
    }
    void applyToSleepCycle(bool enable) override {
        sleepCycle_ = enable;
    }
    void applyToMemoryReconsolidation(bool enable) override {
        reconsolidation_ = enable;
    }
};
```

#### 1.5 Development System Integration (4/10 minimal)

**Files to Modify**:
- `src/development/DevelopmentSystem.hpp` - Expand development processes
- `src/brain/Brain.cpp` - Implement getDevelopmentSystem()
- `python/bindings.cpp` - Expose complete development system

**Specific Changes**:

```cpp
class DevelopmentSystem {
public:
    void update(double timestep) {
        // Update all developmental processes
        updateSynaptogenesis();
        updatePruning();
        updateMaturation();
        updateSpecialization();
        updateDegradation();
    }
    
    void setStage(DevelopmentalStage stage) {
        currentStage_ = stage;
        
        // Stage-specific effects
        switch (stage) {
            case DevelopmentalStage::CriticalPeriod:
                enableEnhancedSensoryLearning(true);
                break;
            case DevelopmentalStage::Maturation:
                enableNeuralSpecialization(true);
                break;
            case DevelopmentalStage::Adult:
                enableStablePerformance(true);
                break;
            case DevelopmentalStage::Aging:
                enableDegenerativeProcesses(true);
                break;
        }
    }
    
    void updateSynaptogenesis() {
        if (synaptogenesisEnabled_) {
            float rate = getSynaptogenesisRate();
            createNewConnections(rate);
        }
    }
    
    // Similar implementations for other processes...
};
```

#### 1.6 Persistence Integration (1/10 stubs)

**Files to Modify**:
- `src/performance/CheckpointSystem.hpp` - Complete implementation
- `src/brain/Brain.cpp` - Implement save() and load()
- `python/bindings.cpp` - Expose checkpoint system

**Specific Changes**:

```cpp
// Brain.cpp: Implement save() and load()
bool Brain::save(const std::string& filepath) const {
    if (pImpl->checkpointSystem) {
        return pImpl->checkpointSystem->saveBrain(*this, filepath);
    }
    return false;
}

bool Brain::load(const std::string& filepath) {
    if (pImpl->checkpointSystem) {
        return pImpl->checkpointSystem->loadBrain(*this, filepath);
    }
    return false;
}
```

### WEEK 2: PYTHON API ENHANCEMENT (Phase 6 User Experience)

#### 2.1 Direct Neural Access API

**Files to Modify**:
- `python/bindings.cpp` - Add neuron direct access APIs

**Specific Changes**:

```cpp
// Expose Neuron for direct access
py::class_<Neuron>(m, "Neuron")
    .def("getId", &Neuron::getId)
    .def("getType", &Neuron::getType)
    .def("getPotential", &Neuron::getMembranePotential)
    .def("getRegion", &Neuron::getRegion)
    .def("getPopulation", &Neuron::getPopulation)
    .def("getConnections", &Neuron::getConnections)
    .def("getWeight", &Neuron::getWeight)
    .def("setWeight", &Neuron::setWeight)
    .def("isActive", &Neuron::isActive)
    .def("getLastSpikeTime", &Neuron::getLastSpikeTime);

// Neuron lookup utilities
m.def("getNeuron", &Brain::getNeuron, py::arg("id"),
      "Get a neuron by ID. Returns nullptr if not found.");

m.def("findNeurons", &Brain::findNeurons, py::arg("type"),
      "Find all neurons of a specific type.");
```

#### 2.2 Enhanced Statistics API

**Files to Modify**:
- `python/bindings.cpp` - Add network analysis functions

**Specific Changes**:

```cpp
// Network topology metrics
m.def("computeNetworkTopology", &Brain::computeNetworkTopology,
      "Compute network topology metrics (degree, clustering, path length).");

m.def("computeClusteringCoefficient", &Brain::computeClusteringCoefficient,
      "Compute clustering coefficient of the neural network.");

m.def("computePathLength", &Brain::computePathLength,
      "Compute average path length of the neural network.");

m.def("detectCommunities", &Brain::detectCommunities,
      "Detect communities in the neural network using clustering algorithms.");

m.def("computeInformationFlow", &Brain::computeInformationFlow,
      "Compute information flow between neural regions.");

// Neuron statistics
m.def("getNeuronStats", &Brain::getNeuronStats,
      "Get statistics about neuron population (firing rates, activity patterns).");

// Synapse statistics
m.def("getSynapseStats", &Brain::getSynapseStats,
      "Get statistics about synapse population (weight distribution, connectivity).");
```

#### 2.3 Advanced Simulation Control API

**Files to Modify**:
- `python/bindings.cpp` - Add simulation control utilities

**Specific Changes**:

```cpp
// Simulation control
m.def("setTimestep", &Brain::setTimestep, py::arg("timestep"),
      "Set simulation timestep (seconds).");

m.def("enableMultithreading", &Brain::enableMultithreading, py::arg("enable") = true,
      "Enable/disable multithreaded simulation for performance.");

m.def("setEventDrivenMode", &Brain::setEventDrivenMode, py::arg("enable") = true,
      "Enable/disable event-driven simulation mode.");

m.def("setMaxSteps", &Brain::setMaxSteps, py::arg("steps"),
      "Set maximum simulation steps before automatic stop.");

// Time control
m.def("getSimulationTime", &SimpleWorld::getSimulationTime,
      "Get current simulation time in seconds.");

m.def("setSimulationTime", &SimpleWorld::setSimulationTime, py::arg("time"),
      "Set current simulation time (useful for time travel/debugging).");

// Performance control
m.def("getPerformanceStats", &Brain::getPerformanceStats,
      "Get performance statistics (FPS, memory usage, etc.).");

m.def("optimizeHotPaths", &Brain::optimizeHotPaths,
      "Optimize hot computational paths for better performance.");
```

### WEEK 3: PERFORMANCE AND SCALE (Phase 6 Research Infrastructure)

#### 3.1 Performance Infrastructure Integration

**Files to Modify**:
- `python/bindings.cpp` - Add performance utilities

**Specific Changes**:

```cpp
// Memory pool management
m.def("getMemoryPoolStats", &Performance::getMemoryPoolStats,
      "Get memory pool statistics for debugging.");

m.def("optimizeMemoryPools", &Performance::optimizeMemoryPools,
      "Optimize memory pool usage for better performance.");

// Event queue utilities
m.def("getEventQueueStats", &Performance::getEventQueueStats,
      "Get event queue statistics.");

m.def("flushEventQueue", &Performance::flushEventQueue,
      "Flush pending events (useful for debugging).");

// Sparse connectivity tools
m.def("analyzeConnectivity", &Performance::analyzeConnectivity,
      "Analyze neural connectivity patterns for optimization.");

m.def("optimizeSparseConnectivity", &Performance::optimizeSparseConnectivity,
      "Optimize sparse connectivity for memory/performance trade-off.");

// Parallel processing controls
m.def("getParallelStats", &Performance::getParallelStats,
      "Get parallel processing statistics.");

m.def("setParallelWorkers", &Performance::setParallelWorkers, py::arg("workers"),
      "Set number of parallel workers for computation.");

// SIMD vectorization
m.def("checkSIMDSupport", &Performance::checkSIMDSupport,
      "Check if CPU supports SIMD instructions for optimization.");

m.def("getVectorizationStats", &Performance::getVectorizationStats,
      "Get SIMD vectorization statistics.");
```

#### 3.2 Advanced Configuration API

**Files to Modify**:
- `python/bindings.cpp` - Add configuration utilities

**Specific Changes**:

```cpp
// Research mode configuration
m.def("createResearchConfig", []() {
    auto config = std::make_shared<Config>();
    
    // Performance optimization settings
    config->set("performance.enable_multithreading", true);
    config->set("performance.enable_simd", true);
    config->set("performance.memory_pool_size", "large");
    
    // Research-specific settings
    config->set("research.trial_name", "phase6_integration");
    config->set("research.record_detailed_metrics", true);
    config->set("research.enable_checkpoints", true);
    config->set("research.checkpoint_interval", 1000);
    
    // Large-scale simulation settings
    config->set("simulation.max_neurons", 100000);
    config->set("simulation.max_synapses", 1000000);
    config->set("simulation.time_step", 0.01);
    
    return config;
}, "Create configuration optimized for research simulation.");

// Configuration presets
m.def("createLargeScaleConfig", []() {
    auto config = pynlm::createDefaultConfig();
    config->set("brain.neuron_count", 10000);
    config->set("brain.synapse_density", 0.05);
    config->set("performance.enable_multithreading", true);
    config->set("performance.memory_pool_size", "xlarge");
    return config;
}, "Create configuration for large-scale simulation.");

m.def("createHighPrecisionConfig", []() {
    auto config = pynlm::createDefaultConfig();
    config->set("performance.enable_simd", true);
    config->set("performance.precision_mode", "double");
    config->set("brain.v_thresh", -55.0);
    config->set("brain.tau_mem", 30.0);
    return config;
}, "Create configuration for high-precision simulation.");
```

### WEEK 4: VALIDATION AND TESTING (Phase 6 Research Infrastructure)

#### 4.1 Experiment Infrastructure API

**Files to Modify**:
- `python/bindings.cpp` - Add experiment utilities

**Specific Changes**:

```cpp
// Experiment runner
m.def("runExperiment", &ExperimentRunner::run, py::arg("experiment_name"),
      "Run a predefined experiment.");

m.def("runExperimentWithConfig", &ExperimentRunner::runWithConfig,
      py::arg("experiment_name"), py::arg("config"),
      "Run experiment with custom configuration.");

// Metrics collection
m.def("collectMetrics", &ExperimentRunner::collectMetrics, py::arg("duration"),
      "Collect simulation metrics over specified duration.");

m.def("getMetrics", &ExperimentRunner::getMetrics, py::arg("metric_name"),
      "Get specific metric values (firing_rate, memory_usage, etc.).");

// Statistical analysis
m.def("computeStatistics", &ExperimentRunner::computeStatistics,
      py::arg("data"), py::arg("stat_type"),
      "Compute statistical analysis of experimental data.");

m.def("runAblationStudy", &ExperimentRunner::runAblationStudy,
      py::arg("component"), py::arg("duration"),
      "Run ablation study by disabling specific component.");

// Multi-agent experiments
m.def("createMultiAgentExperiment", &ExperimentRunner::createMultiAgentExperiment,
      py::arg("num_agents"), py::arg("agents_config"),
      "Create multi-agent experiment configuration.");

m.def("runMultiAgentExperiment", &ExperimentRunner::runMultiAgentExperiment,
      py::arg("experiment_name"), py::arg("num_steps"),
      "Run multi-agent experiment with interaction.");
```

#### 4.2 Documentation and Examples

**Files to Create**:
- `examples/advanced_usage.py` - Advanced usage examples
- `examples/research_workflows.py` - Research-specific workflows
- `examples/performance_tuning.py` - Performance optimization examples
- `examples/memory_analysis.py` - Memory system analysis examples
- `examples/predictive_modeling.py` - Prediction system usage
- `examples/development_studies.py` - Developmental trajectory studies

## IMPLEMENTATION CHECKLIST

### Week 1: Core Systems Integration

#### ✓ Files to Modify
1. `src/brain/Brain.cpp` - 7 critical system accessors
2. `src/brain/Brain.hpp` - 7 new method declarations  
3. `python/bindings.cpp` - 20+ new Python exposes

#### ✓ Specific Implementation Tasks
- [ ] WorkingMemory: store/retrieve/clear/decay
- [ ] EpisodicMemory: store/retrieve/consolidate
- [ ] SemanticMemory: store/retrieve
- [ ] ProceduralMemory: learn/retrieve/update
- [ ] PredictionSystem: train/update/getError/getConfidence
- [ ] NeuralPlanner: setGoal/plan/getSequence
- [ ] ConceptFormation: extractFeatures/getConcepts
- [ ] SelfModel: update/getBeliefs/getGoals
- [ ] SocialLearning: observe/learn/getKnowledge
- [ ] SpatialRepresentation: update/getDistance/getDirection
- [ ] TemporalRelation: update/getOrder/getDuration
- [ ] Dopamine: applyToPlasticity/Memory/Attention/Motivation
- [ ] Acetylcholine: applyToAttention/Sensory/Memory
- [ ] Norepinephrine: applyToArousal/Alertness/Memory
- [ ] Serotonin: applyToMood/Sleep/Memory
- [ ] DevelopmentSystem: update/setStage/process control
- [ ] CheckpointSystem: save/load/create/restore/list/delete

### Week 2: Python API Enhancement

#### ✓ Files to Modify
1. `python/bindings.cpp` - 15+ new Python APIs

#### ✓ Specific Implementation Tasks
- [ ] Neuron direct access (getId/getType/getWeight/setWeight)
- [ ] Network analysis (topology/clustering/pathLength/communities)
- [ ] Simulation control (setTimestep/enableMultithreading/setEventDrivenMode)
- [ ] Performance stats (getPerformanceStats/optimizeHotPaths)

### Week 3: Performance and Scale

#### ✓ Files to Modify
1. `python/bindings.cpp` - 10+ performance APIs

#### ✓ Specific Implementation Tasks
- [ ] Memory pool management (getMemoryPoolStats/optimizeMemoryPools)
- [ ] Event queue utilities (getEventQueueStats/flushEventQueue)
- [ ] Connectivity analysis (analyzeConnectivity/optimizeSparseConnectivity)
- [ ] Parallel processing (getParallelStats/setParallelWorkers)
- [ ] SIMD vectorization (checkSIMDSupport/getVectorizationStats)

### Week 4: Validation and Testing

#### ✓ Files to Create
1. `examples/advanced_usage.py` - 5 advanced examples
2. `examples/research_workflows.py` - 3 research workflows
3. `examples/performance_tuning.py` - 2 performance examples
4. `examples/memory_analysis.py` - 2 memory analysis examples
5. `examples/predictive_modeling.py` - 2 prediction examples
6. `examples/development_studies.py` - 2 development examples

## EXPECTED RESULTS

### After Implementation:
- **Integration Score**: 35.8% → 80%+
- **Critical Systems**: All memory, prediction, cognition systems connected
- **Python API**: Complete exposure of all C++ capabilities
- **Research Ready**: Full experimental infrastructure available

### Quality Metrics:
- **Code Coverage**: >90% for new integration code
- **Documentation**: Complete API documentation with examples
- **Testing**: Comprehensive test suite for all new functionality
- **Performance**: No performance regression for existing code

### Research Impact:
- **Memory Studies**: Full episodic memory integration for experience tracking
- **Learning Research**: Complete prediction system for forward modeling
- **Cognitive Research**: All cognitive systems (planner, concepts, self) integrated
- **Developmental Research**: Full lifespan development with stage-specific processes
- **Neuromodulation Research**: Complete neuromodulation system with all transmitters
- **Persistence Research**: Checkpoint system with memory replay capabilities

## RISK ASSESSMENT

### Low Risk (Exposing existing C++ functionality):
- All changes expose already-implemented C++ systems
- No new algorithms or complex implementations required
- Existing test infrastructure can validate integration

### Dependencies:
- None external required
- All changes are internal to NLM project
- Backward compatible with existing Python API

## TIME ESTIMATE

### Implementation Timeline:
- **Week 1**: Core system integration (7 critical files modified)
- **Week 2**: Python API enhancement (15 new APIs)
- **Week 3**: Performance and scale infrastructure (10 performance APIs)
- **Week 4**: Documentation and examples (15 example files)

**Total**: 4 weeks to complete Phase 6 integration
**Effort**: ~40 person-days of development

This implementation plan ensures that NLM achieves Phase 6's goal of becoming a coherent artificial brain rather than a disconnected collection of components.