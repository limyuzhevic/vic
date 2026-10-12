# Phase 2 Tutorial: LIF Neurons, Spikes, Plasticity

## Learning Objectives

After completing this tutorial, you will:

- Understand the Leaky Integrate-and-Fire (LIF) neuron model
- Implement basic neural dynamics and spike generation
- Configure and use spike-timing-dependent plasticity (STDP)
- Set up basic Hebbian learning
- Create and simulate a simple neural network
- Interpret neural activity patterns

## Prerequisites

- Basic understanding of neural computation concepts
- Familiarity with C++ programming
- Understanding of basic physics (membrane potential, current)

## Setup

### Required Files

- `src/brain/Neuron.hpp` - LIF neuron implementation
- `src/brain/Brain.hpp` - Brain coordinator
- `src/plasticity/STDP.hpp` - STDP implementation
- `src/plasticity/Hebbian.hpp` - Hebbian learning implementation

### Dependencies

```cpp
#include "Brain.hpp"
#include "SensoryInput.hpp"
#include "Action.hpp"
```

## Core Concepts

### Leaky Integrate-and-Fire (LIF) Model

The LIF neuron model is the simplest realistic neuron model, consisting of:

```
dV/dt = (-V + R·I - V_reset)/τ_m
```

Where:
- V = membrane potential
- I = input current
- R = membrane resistance
- V_reset = reset potential after spike
- τ_m = membrane time constant

**Firing Condition:** When V ≥ V_threshold, neuron emits spike and V resets to V_reset.

**Refractory Period:** After firing, neuron cannot fire again for refractory period.

### Neural Dynamics

1. **Integration:** Membrane potential integrates synaptic inputs
2. **Leak:** Potential decays towards resting level
3. **Threshold:** When potential exceeds threshold, spike occurs
4. **Reset:** After spike, potential is reset
5. **Refractory:** Neuron becomes refractory for brief period

### Plasticity Rules

#### STDP (Spike-Timing-Dependent Plasticity)

- **LTP (Long-Term Potentiation):** Pre-before-post spikes strengthen synapse
- **LTD (Long-Term Depression):** Post-before-pre spikes weaken synapse
- **Timing-dependent:** Strength depends on spike timing difference

#### Hebbian Learning

- **Co-activation:** Neurons firing together strengthen connections
- **Global:** Depends on overall activity levels
- **Symmetric:** Same rule for all synapses

## Implementation

### Step 1: Create Configuration

```cpp
// Create brain configuration
auto config = std::make_shared<nlm::Config>();

// Configure for Phase 2 experiment
config->set("neuron_count", 100);           // Small network for learning
config->set("region_count", 1);             // Single region
config->set("connection_probability", 0.2); // Moderate connectivity
config->set("simulation_timestep", 0.001);   // Fine time step
config->set("stdp_ltp_weight", 0.01f);      // LTP strength
config->set("stdp_ltd_weight", 0.012f);     // LTD strength
config->set("stdp_tau", 20.0f);             // STDP time constant
```

### Step 2: Initialize Brain

```cpp
// Create and initialize brain
nlm::Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    return 1;
}

std::cout << "Brain initialized successfully:" << std::endl;
std::cout << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
std::cout << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
```

### Step 3: Configure Plasticity

```cpp
// Configure STDP plasticity
auto* stdp = brain.getSTDP();
if (stdp) {
    stdp->configure(0.01f, 0.012f, 20.0f);
    std::cout << "STDP configured with LTP=0.01, LTD=0.012, τ=20ms" << std::endl;
}

// Configure Hebbian learning
auto* hebbian = brain.getHebbian();
if (hebbian) {
    hebbian->configure(0.001f);  // Learning rate
    std::cout << "Hebbian learning configured with rate=0.001" << std::endl;
}
```

### Step 4: Run Learning Experiment

```cpp
// Learning experiment: pre-post spike timing
const size_t learning_steps = 5000;
nlm::Timestamp time = 0.0;

std::cout << "Running STDP learning experiment..." << std::endl;

for (nlm::SimulationStep step = 0; step < learning_steps; ++step) {
    time = step * 0.001;
    
    // Step 1: Brain processes spikes and updates neurons
    brain.step(step, time);
    
    // Step 2: Inject pre-synaptic spikes (at neuron 0)
    if (step % 100 == 0) {
        brain.injectCurrent(nlm::NeuronId(0), 2.0f); // Strong pre-synaptic input
    }
    
    // Step 3: Inject post-synaptic spikes (at neuron 1) 
    if (step % 150 == 0) {
        brain.injectCurrent(nlm::NeuronId(1), 2.0f); // Post-synaptic input
    }
    
    // Step 4: Log statistics every 500 steps
    if (step % 500 == 0) {
        std::cout << "Step " << step << ": "
                  << brain.getTotalSpikeCount() << " total spikes, "
                  << brain.getPendingSpikeEventCount() << " pending events" << std::endl;
    }
}
```

### Step 5: Analyze Plasticity Effects

```cpp
// Analyze synapse weight changes
void analyze_plasticity(nlm::Brain& brain) {
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    std::cout << "\n=== Plasticity Analysis ===" << std::endl;
    
    if (stdp) {
        // Get synapse weight statistics
        float avg_weight = 0.0f;
        size_t synapse_count = brain.getTotalSynapseCount();
        if (synapse_count > 0) {
            avg_weight = brain.getAverageFiringRate(); // Simplified
            std::cout << "Average synapse weight: " << avg_weight << std::endl;
        }
    }
    
    // Analyze spike statistics
    std::cout << "Total spikes generated: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "Pending spike events: " << brain.getPendingSpikeEventCount() << std::endl;
    
    // Calculate E/I ratio (excitatory/inhibitory balance)
    float ei_ratio = brain.getExcitationInhibitionRatio();
    std::cout << "E/I ratio: " << ei_ratio << std::endl;
    if (ei_ratio < 10.0f) {
        std::cout << "Warning: High inhibition may suppress activity" << std::endl;
    }
}
```

## Advanced Concepts

### STDP Timing Variations

```cpp
// Experiment with different spike timing scenarios
void explore_stdp_timing(nlm::Brain& brain) {
    brain.reset();
    
    // Scenario 1: Pre before post (should strengthen)
    std::cout << "Scenario 1: Pre-before-post (should strengthen)" << std::endl;
    for (int pre_delay = 0; pre_delay <= 50; pre_delay += 10) {
        // Create scenario with specific timing
        // Record resulting synapse weight changes
    }
    
    // Scenario 2: Post before pre (should weaken)
    std::cout << "Scenario 2: Post-before-pre (should weaken)" << std::endl;
    for (int post_delay = 0; post_delay <= 50; post_delay += 10) {
        // Create scenario with specific timing
        // Record resulting synapse weight changes
    }
}
```

### Hebbian Learning Analysis

```cpp
// Measure Hebbian learning rate
void measure_hebbian_learning(nlm::Brain& brain) {
    brain.reset();
    
    // Measure correlation-based learning
    std::cout << "Measuring Hebbian learning..." << std::endl;
    
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject correlated pre-post activity
        if (step % 20 == 0) {
            brain.injectCurrent(nlm::NeuronId(0), 1.0f); // Pre-synaptic
        }
        if (step % 25 == 0) {
            brain.injectCurrent(nlm::NeuronId(1), 1.0f); // Post-synaptic
        }
        
        // Record weight changes
        if (step % 500 == 0) {
            auto* hebbian = brain.getHebbian();
            if (hebbian) {
                // Measure learning effect
            }
        }
    }
}
```

## Extensions and Custom Experiments

### Custom STDP Parameters

```cpp
// Experiment with different STDP parameters
void custom_stdp_experiment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("stdp_ltp_weight", 0.05f);   // Strong LTP
    config->set("stdp_ltd_weight", 0.04f);   // Strong LTD
    config->set("stdp_tau", 10.0f);          // Fast decay
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run experiment with custom parameters
    // Analyze results
}
```

### Neural Population Experiments

```cpp
// Experiment with neural populations
void population_experiment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 3);  // Three regions
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Add populations to regions
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        auto pop_id = region->addPopulation(100, nlm::NeuronType::Internal);
        auto* population = region->getPopulation(pop_id);
        
        if (population) {
            population->initializeRandom(*brain.getRandomGenerator());
        }
    }
    
    // Run population dynamics experiment
}
```

## Results and Interpretation

### Expected Learning Curves

1. **STDP Learning:** Synapse weights should show Hebbian-like patterns
2. **Activity Patterns:** Network should develop preferred firing patterns
3. **E/I Balance:** Network should maintain stable excitation/inhibition ratio

### Measurement Metrics

- **Total Spikes:** Number of spikes generated over time
- **Average Firing Rate:** Mean firing rate across all neurons
- **E/I Ratio:** Balance between excitation and inhibition
- **Pending Events:** Number of spikes waiting to be delivered

## Troubleshooting

### Common Issues

**Problem:** No spikes generated
**Solution:** Check neuron thresholds, input current, and membrane parameters

**Problem:** Network unstable
**Solution:** Reduce time step, check synaptic weights, verify LIF parameters

**Problem:** STDP not working
**Solution:** Verify STDP configuration, check spike timing, validate synapse structure

### Debugging Commands

```cpp
// Debug neural state
void debug_neural_state(nlm::Brain& brain) {
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        auto* population = region->getPopulation(pop_id);
        if (population) {
            for (size_t i = 0; i < std::min(size_t(10), population->getSize()); ++i) {
                auto* neuron = population->getNeuron(i);
                if (neuron) {
                    auto state = neuron->getState();
                    std::cout << "Neuron " << neuron->getId() << ": "
                              << "V=" << state.membranePotential << "mV, "
                              << "firing=" << static_cast<int>(state.firingState) << std::endl;
                }
            }
        }
    }
}
```

## Conclusion

Phase 2 provides the foundation for understanding neural dynamics and plasticity. You've learned:

- LIF neuron operation and spike generation
- STDP and Hebbian learning principles
- Basic network configuration and simulation
- How to interpret neural activity patterns

These concepts form the basis for all subsequent phases of the NLM system.

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Phase 3 Tutorial](PHASE3/README.md)
- [Phase 4 Tutorial](PHASE4/README.md)
- [Phase 6 Tutorial](PHASE6/README.md)
