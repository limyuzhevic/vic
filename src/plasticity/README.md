# Plasticity Module - NLM Learning and Adaptation

**NLM Plasticity Module Documentation**

## Overview

The Plasticity Module implements the comprehensive learning and adaptation mechanisms of the NLM brain, providing spike-timing dependent plasticity (STDP), Hebbian learning, reward-modulated learning, and structural plasticity for synaptic strength modulation and connectivity changes.

### Key Features

- **STDP Learning**: Spike-timing dependent plasticity for temporal learning
- **Hebbian Learning**: Correlation-based synaptic strengthening
- **Reward-Modulated Learning**: Temporal difference learning integration
- **Structural Plasticity**: Synaptic formation and elimination
- **Eligibility Traces**: Delayed reward signaling for learning
- **Plasticity Regulation**: Developmental and neuromodulatory control
- **Learning Integration**: Seamless integration with neural processing

## Core Classes

### STDP
Spike-timing dependent plasticity for temporal learning and prediction error

### Hebbian
Correlation-based learning for pattern associations

### StructuralPlasticity
Synaptic formation, elimination, and structural reorganization

## Plasticity Architecture

### Plasticity System Integration
```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING
  ↓
NEUROMODULATION
  ├─ Dopamine (Reward)
  ├─ Curiosity (Exploration)
  ├─ Novelty (Novelty Detection)
  └─ Prediction Error (Temporal Difference)
  ↓
PLASTICITY SYSTEMS
  ├─ STDP (Temporal Learning)
  ├─ Hebbian (Correlation Learning)
  ├─ Reward-Modulated (TD Learning)
  └─ Structural (Connectivity Changes)
  ↓
SYNAPTIC MODULATION
  ├─ Weight Adjustment
  ├─ Connection Formation
  └─ Strength Optimization
  ↓
LEARNING INTEGRATION
  ├─ Memory Updates
  ├─ Behavior Modification
  └─ Future Prediction
```

### Plasticity Processing Pipeline

1. **Sensory Input**: Neural activity and spike timing
2. **Neuromodulation**: Reward prediction error signals
3. **Temporal Processing**: Spike timing relationships
4. **Learning Rules**: Apply appropriate plasticity mechanism
5. **Synaptic Update**: Modify connection weights and structure
6. **Memory Integration**: Update stored patterns and associations
7. **Behavior Change**: Modified response patterns

## Core Classes

### STDP
Spike-timing dependent plasticity for temporal learning

**Purpose**: Implements spike-timing dependent plasticity (STDP) for temporal learning and prediction error signaling. STDP is crucial for:
- **Temporal Credit Assignment**: Assigning credit based on spike timing
- **Prediction Error Learning**: Learning from prediction errors
- **Temporal Sequencing**: Learning temporal order of events
- **Forward Models**: Developing internal models of the world

**Key Features**:
- **Pre-Post Spike Pairing**: Asymmetric weight changes based on spike order
- **Time Window Learning**: Learning within specific time windows
- **Reward Integration**: Modulation by reward signals
- **Learning Rate Control**: Adjustable learning dynamics
- **Eligibility Traces**: Delayed reward signaling

**Usage Examples**:
```cpp
// Configure STDP learning
auto* stdp = brain->getSTDP();
stdp->setLearningRate(0.01f);
stdp->setTimeWindow(20);
stdp->enableRewardModulation(true);

// Record pre-synaptic spikes
stdp->recordPreSpike(timestamp);

// Record post-synaptic spikes
stdp->recordPostSpike(timestamp);

// Compute STDP updates
stdp->computeUpdates();
```

### Hebbian
Correlation-based learning for pattern associations

**Purpose**: Implements Hebbian learning for correlation-based synaptic strengthening, supporting:
- **Pattern Association**: Learning correlated patterns
- **Concept Formation**: Discovering abstract concepts
- **Semantic Memory**: Building semantic relationships
- **Procedural Learning**: Sequence and skill acquisition
- **Feature Extraction**: Identifying meaningful features

**Key Features**:
- **Positive Correlation**: Strengthen connections for correlated activity
- **Pattern Discovery**: Identify meaningful pattern relationships
- **Concept Learning**: Form abstract representations
- **Association Formation**: Build semantic networks
- **Incremental Learning**: Continuous pattern refinement

**Usage Examples**:
```cpp
// Configure Hebbian learning
auto* hebbian = brain->getHebbian();
hebbian->setLearningRate(0.005f);
hebbian->enableAssociationFormation(true);

// Record pattern activity
hebbian->recordPattern(patternA);
hebbian->recordPattern(patternB);

// Compute Hebbian updates
hebbian->computeUpdates();
```

### StructuralPlasticity
Synaptic formation, elimination, and structural reorganization

**Purpose**: Implements structural plasticity for synaptic formation, elimination, and reorganization, supporting:
- **Synaptogenesis**: Formation of new synaptic connections
- **Synaptic Pruning**: Elimination of weak or unused connections
- **Rewiring**: Reconfiguration of neural networks
- **Optimization**: Network optimization for efficiency
- **Development**: Structural changes during development

**Key Features**:
- **Connection Formation**: Create new synaptic connections
- **Connection Elimination**: Remove weak or unused connections
- **Network Rewiring**: Reconfigure network topology
- **Resource Optimization**: Optimize synaptic resource usage
- **Developmental Changes**: Age-dependent structural changes

**Usage Examples**:
```cpp
// Configure structural plasticity
auto* structural = brain->getStructuralPlasticity();
structural->setConnectionProbability(0.1f);
structural->setPruningRate(0.05f);
structural->enableRewiring(true);

// Form new connections
structural->formNewConnections();

// Prune weak connections
structural->pruneWeakConnections();

// Reconfigure network
structural->rewireNetwork();
```

## Plasticity Integration

### STDP Integration
```cpp
void integrateSTDP(Brain* brain, float predictionError) {
    auto* stdp = brain->getSTDP();
    if (!stdp) return;
    
    // Configure STDP based on neuromodulation
    stdp->setLearningRate(0.01f * brain->getDopamine()->getSensitivity());
    stdp->enableRewardModulation(brain->getPredictionErrorSignal() > 0);
    
    // Apply STDP learning to active synapses
    stdp->applyToActiveSynapses(predictionError);
    
    // Update eligibility traces
    stdp->updateEligibilityTraces(brain->getCuriosity()->getIntensity());
}
```

### Hebbian Integration
```cpp
void integrateHebbian(Brain* brain) {
    auto* hebbian = brain->getHebbian();
    if (!hebbian) return;
    
    // Extract patterns from neural activity
    std::vector<std::vector<float>> patterns = extractPatterns(brain);
    
    // Form associations between correlated patterns
    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (areCorrelated(patterns[i], patterns[j])) {
                hebbian->formAssociation(patterns[i], patterns[j]);
            }
        }
    }
    
    // Compute Hebbian updates
    hebbian->computeUpdates();
}
```

### Structural Plasticity Integration
```cpp
void integrateStructuralPlasticity(Brain* brain) {
    auto* structural = brain->getStructuralPlasticity();
    if (!structural) return;
    
    // Configure based on developmental stage
    structural->setConnectionProbability(
        calculateOptimalConnectionProbability(brain)
    );
    
    // Apply pruning based on activity levels
    applyPruningBasedOnActivity(brain);
    
    // Form new connections based on need
    formNewConnectionsBasedOnNeed(brain);
    
    // Optimize network connectivity
    optimizeNetworkConnectivity(brain);
}
```

## Plasticity Management

### Plasticity Lifecycle
```
Sensory Input
    ↓
Spike Generation
    ↓
Neuromodulation
    ↓
Plasticity Rules
    ├─ STDP (Temporal Learning)
    ├─ Hebbian (Correlation Learning)
    ├─ Reward-Modulated (TD Learning)
    └─ Structural (Connectivity Changes)
    ↓
Synaptic Updates
    ↓
Memory Integration
    ↓
Behavior Modification
    ↓
Future Prediction
    ↓
Memory Replay
    ↓
Learning Consolidation
```

### Plasticity Allocation and Deallocation
```cpp
class PlasticityManager {
public:
    // Initialize plasticity systems
    void initializePlasticitySystems(Brain* brain) {
        // Initialize STDP
        stdpSystem = std::make_unique<STDP>();
        stdpSystem->configureFromConfig(brain->getConfig());
        
        // Initialize Hebbian
        hebbianSystem = std::make_unique<Hebbian>();
        hebbianSystem->configureFromConfig(brain->getConfig());
        
        // Initialize structural plasticity
        structuralSystem = std::make_unique<StructuralPlasticity>();
        structuralSystem->configureFromConfig(brain->getConfig());
    }
    
    // Deallocate plasticity systems
    void cleanupPlasticitySystems() {
        stdpSystem.reset();
        hebbianSystem.reset();
        structuralSystem.reset();
    }
    
    // Get plasticity statistics
    PlasticityStats getPlasticityStats() const {
        PlasticityStats stats;
        stats.stdpUpdates = stdpSystem->getUpdateCount();
        stats.hebbianUpdates = hebbianSystem->getUpdateCount();
        stats.structuralUpdates = structuralSystem->getUpdateCount();
        stats.totalUpdates = stats.stdpUpdates + stats.hebbianUpdates + stats.structuralUpdates;
        return stats;
    }
};
```

### Plasticity Persistence
```cpp
void savePlasticityState(Brain* brain, const std::string& filepath) {
    auto* stdp = brain->getSTDP();
    if (stdp) {
        stdp->saveToFile(filepath + ".stdp");
    }
    
    auto* hebbian = brain->getHebbian();
    if (hebbian) {
        hebbian->saveToFile(filepath + ".hebbian");
    }
    
    auto* structural = brain->getStructuralPlasticity();
    if (structural) {
        structural->saveToFile(filepath + ".structural");
    }
    
    savePlasticityMetadata(brain, filepath + ".meta");
}

void loadPlasticityState(Brain* brain, const std::string& filepath) {
    auto* stdp = brain->getSTDP();
    if (stdp && fileExists(filepath + ".stdp")) {
        stdp->loadFromFile(filepath + ".stdp");
    }
    
    auto* hebbian = brain->getHebbian();
    if (hebbian && fileExists(filepath + ".hebbian")) {
        hebbian->loadFromFile(filepath + ".hebbian");
    }
    
    auto* structural = brain->getStructuralPlasticity();
    if (structural && fileExists(filepath + ".structural")) {
        structural->loadFromFile(filepath + ".structural");
    }
    
    loadPlasticityMetadata(brain, filepath + ".meta");
}
```

## Plasticity Performance

### Time Complexity
- **STDP Updates**: O(n) where n is number of spiking events
- **Hebbian Updates**: O(m) where m is number of pattern pairs
- **Structural Plasticity**: O(k) where k is number of connections
- **Plasticity Integration**: O(1) per integration step

### Memory Usage
- **STDP Traces**: O(t) where t is time window size
- **Hebbian Weights**: O(p) where p is number of patterns
- **Structural Connections**: O(c) where c is number of synapses
- **Plasticity Overhead**: ~5-10% additional memory

### Optimization Features
- **Event-Driven Plasticity**: Only update on relevant events
- **Sparse Plasticity**: Only update active synapses
- **Parallel Plasticity**: Concurrent learning rule application
- **Adaptive Plasticity**: Dynamic adjustment of learning rates
- **Plasticity Pruning**: Eliminate unnecessary plasticity updates

## Plasticity Behavior

### STDP Dynamics
```cpp
class STDPBehavior {
public:
    // Pre-post spike pairing
    void prePostPairing(float weight, float timeDifference) {
        if (timeDifference > 0) {
            // Post before pre: long-term depression
            weight *= (1 - learningRate);
        } else {
            // Pre before post: long-term potentiation
            weight *= (1 + learningRate);
        }
    }
    
    // Time window learning
    void timeWindowLearning() {
        // Apply learning only within time window
        if (abs(currentTime - lastSpikeTime) < timeWindow) {
            applySTDP();
        }
    }
};
```

### Hebbian Dynamics
```cpp
class HebbianBehavior {
public:
    // Positive correlation learning
    void positiveCorrelationLearning(const std::vector<float>& patternA, 
                                    const std::vector<float>& patternB) {
        // Compute correlation
        float correlation = computeCorrelation(patternA, patternB);
        
        // Strengthen weights for positive correlation
        if (correlation > threshold) {
            strengthenWeights(patternA, patternB, correlation);
        }
    }
    
    // Pattern discovery
    void patternDiscovery(const std::vector<float>& pattern) {
        // Identify meaningful features
        identifyFeatures(pattern);
        
        // Form concepts
        formConcepts(pattern);
        
        // Learn associations
        learnAssociations(pattern);
    }
};
```

### Structural Plasticity Dynamics
```cpp
class StructuralPlasticityBehavior {
public:
    // Synaptogenesis
    void synaptogenesis(Brain* brain) {
        // Calculate connection probability
        float connectionProb = calculateConnectionProbability(brain);
        
        // Form new connections randomly
        for (each neuron in brain) {
            if (shouldFormConnection(brain, neuron)) {
                formNewConnection(brain, neuron);
            }
        }
    }
    
    // Synaptic pruning
    void synapticPruning(Brain* brain) {
        // Identify weak connections
        identifyWeakConnections(brain);
        
        // Remove weak connections
        removeWeakConnections(brain);
        
        // Reallocate resources
        reallocateResources(brain);
    }
};
```

## Testing and Validation

### Plasticity System Testing
```bash
# Test STDP learning
./nlm_plasticity_test --type=stdp --learningRate=0.01 --timeWindow=20

# Test Hebbian learning
./nlm_plasticity_test --type=hebbian --learningRate=0.005 --threshold=0.7

# Test structural plasticity
./nlm_plasticity_test --type=structural --connectionProb=0.1 --pruning=0.05

# Test plasticity integration
./nlm_plasticity_test --integration=true --benchmark=true
```

### Plasticity Performance Testing
```cpp
void benchmarkPlasticityPerformance(Brain* brain) {
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structural = brain->getStructuralPlasticity();
    
    // Benchmark STDP performance
    benchmarkSTDP(stdp, 10000);
    
    // Benchmark Hebbian performance
    benchmarkHebbian(hebbian, 5000);
    
    // Benchmark structural plasticity performance
    benchmarkStructuralPlasticity(structural, 1000);
    
    // Benchmark plasticity integration
    benchmarkPlasticityIntegration(brain, 1000);
}
```

## Troubleshooting

### Common Plasticity Issues and Solutions

**Issue**: STDP learning rate too high
**Solution**: Reduce learning rate or narrow time window

**Issue**: Hebbian learning converging too slowly
**Solution**: Increase learning rate or reduce threshold

**Issue**: Structural plasticity creating too many connections
**Solution**: Reduce connection probability or increase pruning rate

**Issue**: Plasticity updates too frequent
**Solution**: Implement activity-dependent plasticity control

**Issue**: Plasticity persistence corrupted
**Solution**: Check file permissions and backup strategy

## Configuration Options

### STDP Configuration
- `plasticity.stdp.learningRate`: STDP learning rate (default: 0.01)
- `plasticity.stdp.timeWindow`: Time window (default: 20)
- `plasticity.stdp.maxWeightChange`: Maximum weight change (default: 0.1)
- `plasticity.stdp.rewardModulation`: Enable reward modulation (default: true)

### Hebbian Configuration
- `plasticity.hebbian.learningRate`: Hebbian learning rate (default: 0.005)
- `plasticity.hebbian.threshold`: Learning threshold (default: 0.7)
- `plasticity.hebbian.associationStrength`: Association strength (default: 0.1)
- `plasticity.hebbian.patternDiscovery`: Enable pattern discovery (default: true)

### Structural Plasticity Configuration
- `plasticity.structural.connectionProbability`: Connection probability (default: 0.1)
- `plasticity.structural.pruningRate`: Pruning rate (default: 0.05)
- `plasticity.structural.rewiring`: Enable rewiring (default: true)
- `plasticity.structural.adaptationRate`: Adaptation rate (default: 0.01)

## Future Enhancements

### Planned Plasticity Features
1. **Meta-Learning**: Learning how to learn
2. **Knowledge Transfer**: Transfer of learned skills between tasks
3. **Online Plasticity**: Continuous learning during deployment
4. **Plasticity Regulation**: Dynamic regulation of plasticity
5. **Multi-Objective Plasticity**: Balancing multiple learning objectives

## License

MIT

## References

1. **Brain Module** (`src/brain/README.md`)
2. **Config Module** (`src/core/Config/README.md`)
3. **Memory Systems Module** (`src/memory/README.md`)
4. **Neuromodulation Module** (`src/neuromodulation/README.md`)

---

*This documentation is continuously updated as the plasticity systems evolve. For the latest information, refer to inline source code documentation and recent commits.*

**Version**: Phase 6 Plasticity Systems
**Last Updated**: Current
**Status**: Production Ready