# NLM Configuration Validation Rules

## Overview
This document defines the specific validation rules that should be implemented for the NLM configuration system to ensure parameter integrity, prevent runtime errors, and maintain biological plausibility of the neural network simulation.

## Core Required Parameters

### 1. Basic Network Parameters
- **neuron_count**: Range [1, 1,000,000]
  - Must be positive integer
  - Upper bound prevents excessive memory usage
  - Lower bound ensures meaningful simulation

- **region_count**: Range [1, 100]
  - Must be positive integer
  - Reasonable upper bound for computational efficiency

- **population_per_region**: Range [1, 10,000]
  - Must be positive integer
  - Balances resolution vs. computational cost

## Section-Specific Validation Rules

### 2. Basic Configuration Section

**Neuron Size Constraints**
- neuron_size_min ∈ [10, 100] (μm)
- neuron_size_max ∈ [neuron_size_min, 1000]
- Ensures biologically plausible neuron dimensions

**Network Connectivity**
- connection_probability ∈ [0.0, 1.0]
- connection_delay_min ∈ [1, 10] (steps)
- connection_delay_max ∈ [connection_delay_min, 100]

**Simulation Parameters**
- simulation_timestep ∈ [0.0001, 1.0] (seconds)
- simulation_steps ∈ [1, 1,000,000]
- simulation_time = simulation_timestep × simulation_steps ∈ [0.001, 10000] (seconds)

### 3. Synaptic Dynamics Section

**Electrical Properties**
- excitatory_reversal_potential ∈ [-100, 50] (mV)
- inhibitory_reversal_potential ∈ [-100, 10] (mV)
- membrane_capacitance ∈ [0.1, 10] (nF)
- time_constant ∈ [1, 1000] (ms)

**Spike Dynamics**
- spike_threshold ∈ [-60, -20] (mV) - more negative than resting potential
- spike_reset_potential ∈ [-80, -60] (mV) - near resting
- refractory_period ∈ [1, 100] (ms)

**Adaptation**
- adaptation_time_constant ∈ [10, 1000] (ms)
- adaptation_strength ∈ [0, 10]

### 4. Plasticity Section

**STDP Parameters**
- stdp_ltp_weight ∈ [0.0, 0.5] ( Hebbian strength )
- stdp_ltd_weight ∈ [0.0, 0.5] (Anti-Hebbian strength)
- stdp_tau ∈ [1, 1000] (ms) - time constant
- stdp_max_weight_change ∈ [0.0, 0.5]

**Hebbian Learning**
- hebbian_learning_rate ∈ [0.0, 0.1]
- hebbian_threshold ∈ [0.0, 1.0]

**Structural Plasticity**
- synaptogenesis_rate ∈ [0.0, 0.001] (connections/step)
- pruning_rate ∈ [0.0, 0.001] (connections/step)
- max_synaptic_weight ∈ [0.0, 10.0]
- min_synaptic_weight ∈ [-10.0, 0.0]

**Cross-parameter Rules**
- max_synaptic_weight > min_synaptic_weight
- stdp_ltp_weight typically < stdp_ltd_weight for Bienenstock-Cooper-Munroe learning

### 5. Neuromodulation Section

**Dopamine**
- dopamine_baseline ∈ [0.0, 1.0] (baseline modulation level)
- dopamine_peak ∈ [0.0, 10.0] (peak modulation level)
- Cross-parameter: dopamine_peak > dopamine_baseline
- dopamine_decay_rate ∈ [0.0, 1.0] (decay per timestep)

**Other Neuromodulators**
- acetylcholine_baseline ∈ [0.0, 1.0]
- norepinephrine_sharpening ∈ [0.0, 1.0]
- serotonin_inhibition ∈ [0.0, 1.0]
- histamine_wake ∈ [0.0, 1.0]

### 6. Development Section

**Critical Period**
- critical_period_start ∈ [0.0, 1.0] (fraction of total development)
- critical_period_end ∈ [0.0, 1.0] (fraction of total development)
- Cross-parameter: critical_period_start < critical_period_end

**Development Rates**
- maturation_rate ∈ [0.0, 0.1] (fraction/step)
- pruning_rate ∈ [0.0, 0.1] (fraction/step)
- myelination_rate ∈ [0.0, 0.1]
- refinement_rate ∈ [0.0, 0.1]

### 7. Memory Section

**Capacity Limits**
- working_memory_capacity ∈ [10, 1,000,000]
- max_episodes ∈ [10, 100,000]
- semantic_memory_capacity ∈ [100, 1,000,000]
- procedural_memory_capacity ∈ [10, 100,000]

**Decay Rates**
- working_memory_decay_rate ∈ [0.0, 1.0]
- episode_decay_rate ∈ [0.0, 1.0]

### 8. Integration Section

**Sensory Processing**
- sensory_weight ∈ [0.0, 10.0]
- sensory_threshold ∈ [0.0, 1.0]

**Motor Output**
- motor_gain ∈ [0.0, 10.0]
- motor_threshold ∈ [0.0, 1.0]

**Attention**
- attentional_focus_width ∈ [0.0, 1.0] (coverage fraction)
- attentional_gain ∈ [0.1, 10.0]

**Prediction Error**
- prediction_error_threshold ∈ [0.0, 1.0]
- prediction_error_decay ∈ [0.0, 1.0]

## Cross-Parameter Validation Rules

### 9. Consistency Checks

**Neuron Size Consistency**
- neuron_size_max ≥ neuron_size_min

**Plasticity Consistency**
- plasticity weight bounds properly ordered
- LTP weight < LTD weight (for BCM-like learning)

**Development Consistency**
- critical_period_start < critical_period_end

**Neuromodulation Consistency**
- peak levels > baseline levels

### 10. Biological Plausibility Rules

**Electrical Consistency**
- excitatory_reversal_potential ≥ 0 (depolarizing)
- inhibitory_reversal_potential ≤ resting_potential (hyperpolarizing)
- spike_threshold > resting_potential
- spike_reset_potential ≈ resting_potential

**Temporal Consistency**
- time_constant ≥ simulation_timestep
- refractory_period ≥ time_constant
- adaptation_time_constant ≥ simulation_timestep

**Rate Consistency**
- learning_rates ≤ 1.0
- decay_rates ≤ 1.0
- modulation_levels ≤ 10.0

## Configuration Versioning

### 11. Version Support
- CONFIG_VERSION = 1
- Backward compatibility for existing configurations
- Forward compatibility with validation for new parameters

### 12. Migration Support
- Graceful handling of missing optional parameters
- Default values for new parameters in old configurations
- Clear error messages for deprecated parameters

## Error Handling and Reporting

### 13. Validation Error Categories
1. **Missing Required Parameters**: Clearly identify what's needed
2. **Type Mismatches**: Parameter type validation
3. **Range Violations**: Specific valid ranges
4. **Cross-Parameter Violations**: Dependencies between parameters
5. **Biological Plausibility Violations**: Reasonable values for neural parameters

### 14. Error Message Format
```
Configuration validation failed:
  - Missing required parameter: neuron_count
  - Invalid neuron_size_max: must be between minimum and 1000
  - Invalid stdp_ltp_weight: must be between 0.0 and 0.5
  - Cross-parameter: max_synaptic_weight must be greater than min_synaptic_weight
```

## Implementation Notes

### 15. Validation Function Design
- Separate validation functions for each configuration section
- Return pairs of (valid, error_messages)
- Collect all errors before failing validation
- Provide specific, actionable error messages

### 16. Performance Considerations
- Validation performed at load time
- Cached validation results for repeated loads
- Lightweight validation for runtime modifications

### 17. Integration with NLM Systems
- Validation occurs in loadFromFile()
- Schema-based parsing improves maintainability
- Flexible schema allows for future extensions
- Validation errors provide clear feedback for configuration debugging

## Testing and Validation

### 18. Test Cases
- Valid configuration files should load successfully
- Invalid configurations should fail with descriptive errors
- Edge cases (minimum/maximum values) should work correctly
- Cross-parameter validation should detect inconsistencies
- Missing required parameters should be clearly reported

### 19. Quality Assurance
- Unit tests for individual validation functions
- Integration tests for complete configuration loading
- Property-based tests for edge cases
- Performance tests for large configurations

## Summary

This comprehensive validation system ensures that NLM configurations are:
1. **Complete**: All required parameters are present
2. **Correct**: Parameters are within valid ranges
3. **Consistent**: Cross-parameter relationships are maintained
4. **Plausible**: Values are biologically reasonable
5. **Maintainable**: Validation is modular and extensible

The implementation replaces the simple key=value parser with robust JSON-based configuration with comprehensive validation, addressing all the issues mentioned in the requirements:
- ✅ Proper JSON parsing
- ✅ JSON schema validation
- ✅ Type validation for configuration values
- ✅ Bounds checking for numeric parameters
- ✅ Better error handling and reporting
- ✅ Configuration versioning support
- ✅ Validation for NLM-specific parameters (neuron counts, synaptic parameters, etc.)