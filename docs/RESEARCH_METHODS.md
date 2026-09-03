# NLM Research Methods Documentation

## Overview

This document establishes scientific methodology for conducting research with NLM. Following these methods ensures reproducible, statistically valid results.

## Research Workflow

```
┌─────────────────────────────────────────────────────────────────┐
│                     Research Workflow                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. HYPOTHESIS                                                   │
│     └─> What mechanism or behavior are you investigating?        │
│                                                                  │
│  2. EXPERIMENT DESIGN                                            │
│     └─> What is the independent variable?                        │
│     └─> What is the dependent variable?                          │
│     └─> What are the controls?                                   │
│                                                                  │
│  3. BASELINE                                                     │
│     └─> Run full system to establish baseline                    │
│     └─> Multiple seeds for statistics                            │
│                                                                  │
│  4. INTERVENTION                                                  │
│     └─> Apply ablation/modification                              │
│     └─> Match baseline conditions                                │
│                                                                  │
│  5. MEASUREMENT                                                  │
│     └─> Collect metrics automatically                            │
│     └─> Record timestamps                                        │
│     └─> Save checkpoints                                         │
│                                                                  │
│  6. ANALYSIS                                                     │
│     └─> Statistical tests                                        │
│     └─> Effect size                                              │
│     └─> Significance                                             │
│                                                                  │
│  7. REPLICATION                                                  │
│     └─> Run with different seeds                                 │
│     └─> Verify results hold                                       │
│                                                                  │
│  8. DOCUMENTATION                                                 │
│     └─> Record findings (positive AND negative)                  │
│     └─> Update FAILURES.md                                       │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Hypothesis Testing

### Good Hypotheses

1. **Specific**: "STDP is necessary for sequence learning" not " plasticity matters"

2. **Falsifiable**: Can be disproven by experiment

3. **Measurable**: Has quantitative metrics

### Hypothesis Examples

**H1**: "Dopamine modulation increases reward learning rate"
- Independent variable: dopamine modulation (on/off)
- Dependent variable: reward acquisition rate
- Control: identical environments, seeds

**H2**: "Larger neural populations have better generalization"
- Independent variable: neuron count (10k, 100k, 1M)
- Dependent variable: generalization accuracy
- Control: identical architecture, connectivity

**H3**: "Inhibitory circuits prevent runaway excitation"
- Independent variable: inhibition strength (0.5x, 1x, 2x)
- Dependent variable: firing rate stability
- Control: matched total synaptic weight

## Experiment Design

### Control Conditions

Every experiment should include:

1. **Baseline**: Full system, no modifications
2. **Ablation**: Specific mechanism disabled
3. **Multiple seeds**: Minimum 3 seeds for statistics

### Experiment Configuration

```cpp
struct ExperimentConfig {
    std::string name;
    std::string hypothesis;
    
    // Independent variables
    size_t neuronCount;
    float modificationParameter;
    
    // Dependent variables (what we measure)
    std::vector<std::string> metrics;
    
    // Controls
    uint64_t randomSeed;
    uint64_t steps;
    
    // Replication
    size_t numSeeds;
    std::vector<uint64_t> seeds;
};
```

### Statistical Design

**Minimum sample size**: 3 seeds

**Recommended sample size**: 5-10 seeds

**For publication**: Consider power analysis

## Ablation Framework

### Ablatable Mechanisms

| Target | Effect when disabled |
|--------|---------------------|
| STDP | No spike-timing learning |
| Hebbian | No correlation-based learning |
| Reward modulation | No reward-guided learning |
| Structural plasticity | No synapse formation/removal |
| Working memory | No persistent activity |
| Prediction | No temporal sequence learning |

### Ablation Procedure

```cpp
AblationSystem ablation;
ablation.setBaseline("full_system");

// Disable STDP
ablation.applyAblation({{AblationTarget::STDP}});

// Run experiment
auto result = runExperiment(brain, config);

// Re-enable
ablation.resetToFull();
```

### Ablation Metrics

Record for both baseline and ablation:
- Total reward
- Learning curve
- Final performance
- Firing rates
- Weight distributions

### Statistical Comparison

```cpp
struct AblationComparison {
    float baselineMean;
    float ablationMean;
    float effectSize;  // Cohen's d
    float pValue;       // t-test
    bool significant;
};
```

## Metrics Collection

### Required Metrics

Every experiment should collect:

1. **Behavioral metrics**
   - Total reward
   - Actions per step
   - Exploration rate
   - Goal success rate

2. **Neural metrics**
   - Average firing rate
   - Spike count
   - Active neuron fraction
   - Weight statistics

3. **Learning metrics**
   - Prediction error
   - Memory retention
   - Learning curve

### Optional Metrics

For specific experiments:

1. **Oscillation analysis**
   - Power spectral density
   - Frequency bands
   - Phase synchrony

2. **Connectivity analysis**
   - Clustering coefficient
   - Path length
   - Hub neurons

3. **Information theory**
   - Entropy
   - Mutual information
   - Information transfer

### Metric Collection Interface

```cpp
class MetricCollector {
public:
    void record(const std::string& name, float value);
    void record(const std::string& name, size_t value);
    
    void startEpisode();
    void endEpisode();
    
    Metrics getMetrics() const;
    std::string toCSV() const;
};
```

## Checkpointing

### Checkpoint Types

1. **Initial state**: Before any learning
2. **Periodic**: Every N steps
3. **Episode end**: After each experiment
4. **Final**: Complete state

### Checkpoint Contents

```cpp
struct Checkpoint {
    // Version
    uint32_t version;
    
    // Brain state
    std::vector<NeuronState> neurons;
    std::vector<SynapseState> synapses;
    
    // Simulation state
    uint64_t step;
    double time;
    
    // Random state
    uint64_t rngState;
    
    // Configuration
    std::string configHash;
};
```

### Checkpoint Usage

1. **Resume**: Continue interrupted experiment
2. **Comparison**: Compare states at different times
3. **Analysis**: Offline analysis of saved states
4. **Replication**: Share initial conditions

## Result Documentation

### Experiment Log

For each experiment, document:

```markdown
## Experiment: [Name]
**Date**: YYYY-MM-DD
**Hypothesis**: [Text]
**Seed**: [Number]

### Configuration
- Neurons: [Count]
- Steps: [Count]
- Modification: [Description]

### Results
- Reward: [Mean ± Std]
- Performance: [Value]
- Statistical significance: [p-value]

### Observations
- [What happened]
- [Any unexpected behavior]

### Conclusions
- [Supported/Not supported]
- [Next steps]
```

### FAILURES.md

Document ALL failed experiments:

```markdown
## Failed: [Name]
**Date**: YYYY-MM-DD
**Hypothesis**: [What we expected]
**Observation**: [What happened]
**Analysis**: [Why it failed]
**Lessons**: [What we learned]
```

## Reproducibility

### Required Information

Every experiment should record:

1. **Code version**: Git commit hash
2. **Configuration**: Full config file
3. **Random seed**: Exact seed used
4. **Hardware**: CPU/RAM/GPU info
5. **Build flags**: Compiler optimizations

### Reproducibility Checklist

- [ ] Git commit recorded
- [ ] Config saved
- [ ] Seeds documented
- [ ] Random state checkpointed
- [ ] Results saved with metadata
- [ ] Analysis scripted (not manual)

### Command Line Example

```bash
# Run reproducible experiment
./nlm_experiment \
    --config=experiment_config.json \
    --seed=42 \
    --output=results/exp_001/ \
    --checkpoint_freq=1000

# Verify reproducibility
./nlm_experiment \
    --config=experiment_config.json \
    --seed=42 \
    --verify_only
```

## Visualization

### When to Visualize

1. **Exploratory analysis**: Understand behavior
2. **Debugging**: Find problems
3. **Presentation**: Show results

### When NOT to Visualize

1. **Final results**: Use quantitative metrics
2. **Scaling studies**: Use benchmarks
3. **Statistical tests**: Use p-values

### Visualization Types

1. **Spike raster**: Show spike times
2. **Population activity**: Firing rate over time
3. **Weight distribution**: Histogram of weights
4. **Learning curves**: Performance over time
5. **State space**: PCA/t-SNE of neural activity

## Ethical Considerations

### Appropriate Claims

DO say:
- "NLM shows behavior consistent with..."
- "Emergent pattern formation observed..."
- "Learning rate increased by X%..."
- "Self-organization toward X observed..."

DON'T say:
- "NLM understands..."
- "NLM is conscious..."
- "NLM has memories like..."
- "NLM simulated brain..."

### System Limitations

Never claim NLM:
- Is a biological brain
- Has human-like intelligence
- Experiences emotions
- Understands in human sense
- Has consciousness

## Publication Checklist

Before publishing results:

- [ ] Hypothesis clearly stated
- [ ] Methods fully described
- [ ] Configuration documented
- [ ] Multiple seeds run
- [ ] Statistical tests performed
- [ ] Effect sizes reported
- [ ] Limitations acknowledged
- [ ] Code available (if applicable)
- [ ] Data available (if applicable)
- [ ] FAILURES documented