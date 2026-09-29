# Plasticity API Documentation

## Overview

This document provides comprehensive API documentation for the plasticity system in the NLM (Neural Learning Machine) project. The plasticity system implements various synaptic learning rules that enable the brain to adapt and learn from experience.

## Architecture

The plasticity system is organized into the following components:

### Core Plasticity Classes

#### 1. STDP (Spike-Timing-Dependent Plasticity)

**Purpose**: Implements classic STDP where synaptic strength changes based on the precise timing between pre- and post-synaptic spikes.

**Key Features**:
- Pre-before-post spikes potentiate the synapse (LTP)
- Post-before-pre spikes depress the synapse (LTD)  
- Exponential decay of influence over time
- Configurable time constant

**Parameters**:
- `ltp_weight` (float, range: 0.0-1.0): Potentiation amplitude
- `ltd_weight` (float, range: 0.0-1.0): Depression amplitude
- `time_constant` (float, range: 1.0-100.0): STDP time window (ms)

**Usage Example**:
```cpp
// Create STDP plasticity rule
auto stdp = PlasticityFactory::createPlasticityRule("STDP");

// Configure parameters
stdp->configure(0.02f, 0.015f, 20.0f);

// Or set individual parameters
stdp->setLTPWeight(0.02f);
stdp->setLTDWeight(0.015f);
stdp->setTimeConstant(20.0f);
```

#### 2. Hebbian (Hebbian Learning)

**Purpose**: Implements Hebbian learning "neurons that fire together, wire together" based on correlated activity.

**Key Features**:
- Strengthens connections between co-active neurons
- Uses covariance rule with threshold
- Configurable learning rate and weight bounds
- Activity-dependent learning

**Parameters**:
- `learning_rate` (float, range: 0.0-1.0): Learning speed
- `max_weight` (float, range: 0.0-10.0): Maximum synaptic weight
- `min_weight` (float, range: -10.0-0.0): Minimum synaptic weight  
- `covariance_threshold` (float): Minimum correlation for learning

**Usage Example**:
```cpp
// Create Hebbian plasticity rule
auto hebbian = PlasticityFactory::createPlasticityRule("Hebbian");

// Configure parameters
hebbian->configure(0.01f, 1.0f, -1.0f, 0.0f);

// Or set individual parameters
hebbian->setLearningRate(0.01f);
hebbian->setMaxWeight(1.0f);
hebbian->setMinWeight(-1.0f);
```

#### 3. RewardModulatedSTDP (R-STDP)

**Purpose**: Implements reward-modulated STDP where learning is modulated by dopamine signals representing prediction errors.

**Key Features**:
- Combines STDP timing with reward signals
- Uses dopamine prediction error modulation
- Enables reward-directed learning
- Eligibility traces for delayed reward effects

**Parameters**:
- `ltp_weight` (float, range: 0.0-1.0): Potentiation amplitude
- `ltd_weight` (float, range: 0.0-1.0): Depression amplitude  
- `time_constant` (float, range: 1.0-100.0): STDP time window (ms)
- `dopamine_scale` (float, range: 0.0-2.0): Dopamine modulation factor

**Usage Example**:
```cpp
// Create RewardModulatedSTDP plasticity rule
auto r_stdp = PlasticityFactory::createPlasticityRule("R-STDP");

// Configure parameters
r_stdp->configure(0.02f, 0.015f, 20.0f, 1.0f);
```

#### 4. TripletSTDP

**Purpose**: Implements advanced triplet STDP including short-term plasticity interactions and calcium dynamics.

**Key Features**:
- Triplet interactions (pre-post-unpaired spikes)
- Calcium dynamics simulation
- Short-term plasticity integration
- Homeostatic regulation
- Synaptic tagging for protein synthesis-dependent plasticity

**Parameters**:
- `a_plus` (float, range: 0.0-1.0): A+ for LTP (pre-post pairing)
- `a_minus` (float, range: 0.0-1.0): A- for LTD (post-pre pairing)
- `a_u` (float, range: 0.0-1.0): A_U for unpaired spikes
- `tau_plus` (float, range: 1.0-100.0): τ+ for pre-post influence
- `tau_minus` (float, range: 1.0-100.0): τ- for post-pre influence
- `tau_u` (float, range: 10.0-1000.0): τ_U for unpaired influence

**Usage Example**:
```cpp
// Create TripletSTDP plasticity rule
auto triplet = PlasticityFactory::createPlasticityRule("TripletSTDP");

// Configure parameters
triplet->configure(0.025f, 0.0275f, 0.005f, 20.0f, 20.0f, 200.0f);
```

### Factory Pattern

The PlasticityFactory provides a unified interface for creating and configuring plasticity rules:

**Key Methods**:
- `PlasticityFactory::registerPlasticityRule()`: Register custom plasticity rules
- `PlasticityFactory::createPlasticityRule()`: Create rule by name
- `PlasticityFactory::createFromConfig()`: Create rule from configuration
- `PlasticityFactory::createPlasticitySystem()`: Create multiple rules
- `PlasticityFactory::hasPlasticityRule()`: Check if rule exists

**Usage Example**:
```cpp
// Get available rule names
auto available = PlasticityFactory::getAvailableRuleNames();

// Create rule from configuration
auto rule = PlasticityFactory::createFromConfig(config);

// Create complete plasticity system
auto rules = PlasticityFactory::createPlasticitySystem(config);
```

### Configuration System

Plasticity rules can be configured via the existing NLM Config system:

**Configuration Parameters**:
```json
{
    "plasticity": {
        "enabled": true,
        "rule_name": "STDP",
        "params": {
            "ltp_weight": 0.01,
            "ltd_weight": 0.012,
            "time_constant": 20.0
        }
    },
    "plasticity.hebbian_enabled": true,
    "plasticity.stdp_enabled": true,
    "plasticity.reward_modulated_enabled": true,
    "plasticity.triplet_enabled": true
}
```

### Parameter System

The PlasticityParameters class provides type-safe parameter storage and access:

**Key Methods**:
- `set()`: Set parameter value
- `getFloat()`, `getInt()`, etc.: Get parameter value with defaults
- `hasParameter()`: Check if parameter exists
- `validate()`: Validate all parameters
- `toString()`: Get parameter summary

**Usage Example**:
```cpp
PlasticityParameters params;
params.set("learning_rate", 0.01f);
params.set("max_weight", 1.0f);
params.set("min_weight", -1.0f);

float rate = params.getFloat("learning_rate", 0.0f);
bool valid = params.validate();
```

## Integration with Brain System

The plasticity rules are integrated into the NLM Brain system:

**Brain Initialization**:
- The Brain creates instances of each plasticity rule
- Rules are configured based on global configuration
- Rules are updated during each simulation step
- Rules can be enabled/disabled via configuration

**Example Brain Configuration**:
```cpp
// In Brain::Impl constructor
spikeSystem = std::make_unique<SpikeSystem>();
stdp = std::make_unique<STDP>();
hebbian = std::make_unique<Hebbian>();
structuralPlasticity = std::make_unique<StructuralPlasticity>();

// Configure STDP from global config
float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
float tau = config->getOr<float>("stdp_tau", 20.0f);
stdp->configure(ltpWeight, ltdWeight, tau);
```

## Testing and Validation

Each plasticity rule has comprehensive test suites covering:

### STDP Tests
- Rule creation and default parameters
- Parameter configuration (individual and batch)
- Potentiation (pre-before-post spikes)
- Depression (post-before-pre spikes)
- Edge cases (empty spike vectors, simultaneous spikes)
- Weight bound enforcement

### Hebbian Tests  
- Rule creation and parameter validation
- Learning rate parameter testing
- Weight bounds validation
- Threshold-based learning
- Correlation-based learning

### RewardModulatedSTDP Tests
- Reward signal integration
- Dopamine modulation effects
- Prediction error handling
- Eligibility trace updates

### TripletSTDP Tests
- Triplet interaction validation
- Time constant parameter testing
- Homeostatic regulation
- Synaptic tagging

## Configuration Examples

### Minimal Configuration
```json
{
    "plasticity": {
        "enabled": true,
        "rule_name": "STDP"
    }
}
```

### Full STDP Configuration
```json
{
    "plasticity": {
        "enabled": true,
        "rule_name": "STDP",
        "params": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "time_constant": 20.0
        }
    },
    "plasticity.stdp_enabled": true
}
```

### Multi-Rule Configuration
```json
{
    "plasticity": {
        "enabled": true
    },
    "plasticity.hebbian_enabled": true,
    "plasticity.stdp_enabled": true,
    "plasticity.reward_modulated_enabled": true,
    "plasticity.triplet_enabled": true,
    "plasticity.hebbian_params": {
        "learning_rate": 0.01,
        "max_weight": 1.0,
        "min_weight": -1.0
    },
    "plasticity.stdp_params": {
        "ltp_weight": 0.01,
        "ltd_weight": 0.012,
        "time_constant": 20.0
    }
}
```

## Advanced Usage

### Custom Plasticity Rules
```cpp
// Register custom plasticity rule
PlasticityFactory::registerPlasticityRule("MyCustomSTDP", []() {
    return std::make_unique<MyCustomSTDP>();
});

// Use custom rule
auto customRule = PlasticityFactory::createPlasticityRule("MyCustomSTDP");
```

### Dynamic Rule Switching
```cpp
// Switch between different rule types based on task
auto rule = PlasticityFactory::createFromConfig(taskConfig);
```

### Parameter Optimization
```cpp
// Get default parameters and optimize
PlasticityParameters params = PlasticityFactory::getDefaultParameters("STDP");
params.set("time_constant", optimizeForTask(task));
```

## Performance Considerations

- **Rule Selection**: Factory pattern avoids runtime type checking
- **Parameter Validation**: Early validation prevents runtime errors
- **Memory Management**: Smart pointers prevent memory leaks
- **Configuration Caching**: Parameters cached for fast access
- **Thread Safety**: Factory operations are thread-safe

## Migration Guide

### From Legacy Phase 2 Code

If migrating from Phase 2 placeholder implementations:

**Old Code**:
```cpp
// Phase 2 placeholder classes
AntiHebbianRule antiHebbian;
BCMRule bcmRule;
```

**New Code**:
```cpp
// Use modern factory pattern
auto hebbian = PlasticityFactory::createPlasticityRule("Hebbian");
auto stdp = PlasticityFactory::createPlasticityRule("STDP");
```

### Adding New Plasticity Rules

1. **Create new class** inheriting from `PlasticityRule`
2. **Implement virtual methods**: `update()`, `applyWeightChange()`, `getName()`
3. **Add parameter getters/setters** for configuration
4. **Register with factory**: `PlasticityFactory::registerPlasticityRule()`
5. **Test thoroughly** using existing test framework

## Error Handling

The plasticity system includes comprehensive error handling:

- **Input Validation**: Checks for null pointers, empty vectors, invalid parameters
- **Graceful Degradation**: Continues operation even when individual rules fail
- **Error Logging**: Detailed error messages via NLM logging system
- **Exception Safety**: Catches and logs exceptions without crashing
- **Parameter Validation**: Validates parameter ranges and types

## Summary

The NLM plasticity system provides:

✅ **Complete Implementation**: All plasticity rules fully implemented
✅ **Factory Pattern**: Easy extensibility for new rules  
✅ **Configuration System**: JSON-based configuration with validation
✅ **Error Handling**: Comprehensive validation and error reporting
✅ **Documentation**: Complete API documentation with examples
✅ **Testing**: Comprehensive test coverage for all features
✅ **Performance**: Optimized for real-time neural simulation
✅ **Integration**: Seamless integration with Brain and Neuromodulation systems

The plasticity system is now production-ready and provides a robust foundation for adaptive neural learning in the NLM project.

## See Also

- [NLM Architecture Documentation](./ARCHITECTURE.md)
- [Brain System Integration](./docs/ARCHITECTURE.md#brain-system)
- [Neuromodulation System](./src/neuromodulation/)
- [Testing Framework](./tests/)
- [Configuration System](./src/core/Config/)
