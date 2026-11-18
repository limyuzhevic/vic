# Advanced Features for Expert Users

This document details advanced features available in the NLM brain simulation system for expert users.

## 1. Custom Plasticity Configuration

### Understanding Plasticity Parameters

NLM supports advanced configuration of plasticity rules for expert users:

#### STDP Parameters
- `stdp_ltp_weight`: Learning weight for Long-Term Potentiation (LTP)
- `stdp_ltd_weight`: Learning weight for Long-Term Depression (LTD)
- `stdp_tau`: Time constant for spike timing dependence
- `stdp_window`: Width of STDP temporal window (in simulation steps)

#### Hebbian Parameters
- `hebbian_learning_rate`: Base learning rate for Hebbian connections
- `hebbian_decay`: Rate of Hebbian weight decay
- `hebbian_threshold`: Threshold for Hebbian activation

#### Reward-Modulated Plasticity
- `reward_modulation_factor`: Scaling factor for reward signals
- `eligibility_trace_decay`: Decay rate of eligibility traces

## 2. Advanced Configuration Examples

### Example 1: High Plasticity Regime

```bash
nlm_advanced \
  --param stdp_ltp_weight 0.05 \
  --param stdp_ltd_weight 0.03 \
  --param hebbian_learning_rate 0.1 \
  --param reward_modulation_factor 2.0 \
  --steps 5000 \
  --neurons 5000
```

### Example 2: Stable Learning Regime

```bash
nlm_advanced \
  --param stdp_ltp_weight 0.01 \
  --param stdp_ltd_weight 0.012 \
  --param hebbian_learning_rate 0.02 \
  --param reward_modulation_factor 0.5 \
  --simulation detailed \
  --steps 2000
```

### Example 3: Reproducible Experiments

```bash
nlm_advanced \
  --param random_seed 12345 \
  --param connection_probability 0.05 \
  --param stdp_window 50 \
  --save-checkpoint reproducible_state.chkpt \
  --steps 3000
```

## 3. Advanced CLI Features

### Feature Toggles

#### `--disable-feature plasticity`
Disable all plasticity rules for stability testing:
- STDP learning disabled
- Hebbian learning disabled
- Reward-modulated plasticity disabled

#### `--disable-feature neuromodulation`
Disable neuromodulatory systems:
- Dopamine signaling disabled
- Curiosity signals disabled
- Novelty detection disabled

#### `--disable-feature memory`
Disable memory systems for testing neural dynamics alone:
- Working memory disabled
- Episodic memory disabled
- Associative memory disabled

### Expert Mode

#### `--expert-mode`
Enables additional advanced options:
- Direct neuron access by ID
- Custom connectivity patterns
- Advanced visualization options
- Performance profiling tools

## 4. Configuration File Format

### JSON Configuration

```json
{
    "simulation": {
        "steps": 1000,
        "neurons": 1000,
        "region_count": 1,
        "simulation_type": "standard"
    },
    "plasticity": {
        "stdp": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.012,
            "tau": 20.0
        },
        "hebbian": {
            "learning_rate": 0.05,
            "decay": 0.01
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.1,
            "peak": 1.0
        }
    },
    "output": {
        "format": "json",
        "path": "results.json"
    }
}
```

### Config File Usage

```bash
# Save configuration
nlm_advanced --save-config my_config.json

# Load configuration
nlm_advanced --config my_config.json --steps 1000
```

## 5. Performance Optimization Techniques

### 1. Event-Driven Processing

For large-scale simulations (10k+ neurons), enable event-driven processing:

```bash
nlm_advanced \
  --simulation lightweight \
  --disable-feature neuromodulation \
  --disable-feature memory
```

### 2. Sparse Connectivity

Reduce memory usage for large networks:

```bash
nlm_advanced \
  --simulation lightweight \
  --param connection_probability 0.01
```

### 3. Multithreading

Enable parallel processing for faster simulations:

```bash
nlm_advanced \
  --simulation detailed \
  --benchmark
```

## 6. Debugging and Analysis Tools

### Debug Mode

```bash
nlm_advanced \
  --simulation debug \
  --verbose \
  --steps 100
```

### Profiling

```bash
nlm_advanced \
  --profiling \
  --benchmark \
  --steps 1000
```

### Validation Mode

```bash
nlm_advanced \
  --validate-config my_config.json
```

## 7. Batch Processing

### Run Multiple Configurations

```bash
cat > configs/batch.json << EOF
[
    {"neurons": 1000, "steps": 1000},
    {"neurons": 2000, "steps": 2000},
    {"neurons": 3000, "steps": 3000}
]
EOF

# Process all configurations
for config in $(cat configs/batch.json); do
    nlm_advanced --config "$(echo $config | jq -r '.neurons')" "$config"
    mv results.json "results_$(echo $config | jq -r '.neurons')_neurons.json"
done
```

## 8. Advanced API Access

### Direct Brain Control

```cpp
// Access to brain components for expert users
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

// Custom analysis
void analyzeBrainState(Brain& brain) {
    // Direct neuron access
    auto& neurons = brain.getAllNeurons();
    
    // Custom plasticity modification
    for (auto& synapse : brain.getAllSynapses()) {
        if (synapse->isExcitatory()) {
            // Modify weight based on custom criteria
            float currentWeight = synapse->getWeight();
            if (currentWeight > 0.5f) {
                synapse->addToWeight(-0.01f);  // Reduce strong connections
            }
        }
    }
    
    // Custom spike injection
    brain.injectCurrent(NeuronId(42), 100.0f);
}
```

## 9. Export and Import

### Export Brain State

```bash
nlm_advanced \
  --export-brain-state brain_data.xml \
  --steps 1000
```

### Import Brain State

```bash
nlm_advanced \
  --import-brain-state custom_brain.chkpt \
  --simulation detailed
```

## 10. Script Integration

### Python Integration

```python
import subprocess
import json
import time

# Run simulation with custom parameters
params = {
    "steps": 1000,
    "neurons": 2000,
    "plasticity": {
        "stdp_ltp_weight": 0.03,
        "hebbian_learning_rate": 0.05
    }
}

# Execute simulation
result = subprocess.run([
    "nlm_advanced",
    "--param", "stdp_ltp_weight", str(params["plasticity"]["stdp_ltp_weight"]),
    "--neurons", str(params["neurons"]),
    "--steps", str(params["steps"])
])

# Process results
if result.returncode == 0:
    with open("results.json", "r") as f:
        data = json.load(f)
    print(f"Simulation completed: {data['summary']}")
```

## 11. Advanced Troubleshooting

### Common Issues and Solutions

#### Issue: "No spikes detected"

**Solution:**
```bash
# Check neuron parameters
nlm_advanced \
  --simulation debug \
  --verbose \
  --steps 100

# Verify connectivity
nlm_advanced \
  --validate-config my_config.json \
  --simulation debug
```

#### Issue: "Memory usage too high"

**Solution:**
```bash
# Enable sparse connectivity
nlm_advanced \
  --simulation lightweight \
  --param connection_probability 0.01

# Disable memory systems
nlm_advanced \
  --disable-feature memory \
  --simulation lightweight
```

#### Issue: "Performance bottlenecks"

**Solution:**
```bash
# Enable profiling
nlm_advanced \
  --profiling \
  --benchmark \
  --steps 500

# Optimize simulation
nlm_advanced \
  --simulation optimized \
  --steps 1000
```

## 12. Advanced Use Cases

### 1. Neuromodulation Research

Study how different neuromodulators affect learning:

```bash
# Experiment with dopamine
nlm_advanced \
  --param dopamine_baseline 0.2 \
  --param reward_modulation_factor 1.5 \
  --steps 2000

# Experiment with curiosity
nlm_advanced \
  --param curiosity_threshold 0.8 \
  --steps 2000
```

### 2. Developmental Studies

Investigate developmental changes in plasticity:

```bash
# Simulate development stages
nlm_advanced \
  --enable-feature development \
  --simulation detailed \
  --steps 5000
```

### 3. Memory Consolidation Studies

Study memory consolidation and replay:

```bash
# Enable memory systems
nlm_advanced \
  --enable-feature memory \
  --simulation detailed \
  --steps 3000
```

## 13. Integration with External Tools

### MATLAB Integration

```matlab
% Run NLM simulation from MATLAB
function [results] = runNLMExperiment(params)
    % Create command line
    cmd = ['nlm_advanced ', ...
           '--neurons ', num2str(params.neurons), ' ', ...
           '--steps ', num2str(params.steps), ' ', ...
           '--param stdp_ltp_weight ', num2str(params.stdp_ltp_weight), ' ' ...
           '--output results.mat'];
    
    % Execute simulation
    [status, output] = system(cmd);
    
    if status == 0
        % Load results
        results = load('results.mat');
    else
        error('NLM simulation failed: %s', output);
    end
end
```

### R Integration

```r
# Run NLM simulation from R
library(subshell)

run_nlm_simulation <- function(params) {
    cmd <- paste0(
        "nlm_advanced ",
        "--neurons ", params$neurons, " ",
        "--steps ", params$steps, " ",
        "--param stdp_ltp_weight ", params$stdp_ltp_weight, " ",
        "--output results.json"
    )
    
    result <- subshell(cmd)
    if (result$status != 0) {
        stop("NLM simulation failed")
    }
    
    # Load and analyze results
    data <- jsonlite::fromJSON("results.json")
    return(data)
}
```

## 14. Performance Monitoring

### Real-time Monitoring

```bash
# Enable real-time monitoring
nlm_advanced \
  --monitoring \
  --verbose \
  --steps 1000
```

### Resource Usage Tracking

```bash
# Track CPU and memory usage
nlm_advanced \
  --performance-monitoring \
  --steps 1000
```

## 15. Advanced Configuration Management

### Configuration Templates

```bash
# Save common configurations as templates
nlm_advanced --save-template baseline "--steps 1000 --neurons 1000"
nlm_advanced --save-template advanced "--steps 5000 --neurons 5000 --simulation detailed"

# Load templates
nlm_advanced --load-template baseline
nlm_advanced --load-template advanced
```

### Configuration Comparison

```bash
# Compare two configurations
nlm_advanced \
  --compare-config base_config.json \
  --compare-config modified_config.json \
  --analysis detailed
```

## Summary

These advanced features provide expert users with comprehensive control over NLM simulations, from basic configuration to sophisticated research experiments. The system supports:

- Custom plasticity rules
- Advanced configuration management
- Batch processing capabilities
- Integration with external tools
- Performance optimization techniques
- Comprehensive debugging and analysis tools
- Expert mode with additional features

All features are designed to be backward compatible while providing powerful new capabilities for advanced users and research applications.