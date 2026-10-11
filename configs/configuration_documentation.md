# NLM Configuration Parameter Documentation

This document provides comprehensive documentation of all configuration parameters used in the NLM (熙然) project. Configuration parameters control the behavior, performance, and characteristics of the neural system.

## Configuration Overview

The NLM configuration system is organized into logical groups:

1. **Core Simulation Parameters** - Basic simulation settings
2. **Network Architecture** - Brain structure and connectivity
3. **Plasticity Control** - Learning rule parameters
4. **Development Control** - Age-dependent regulation
5. **Neuromodulation** - Biological signal systems
6. **Environment Integration** - World interaction settings
7. **Logging and Monitoring** - System output and debugging
8. **Performance Optimization** - Computational settings

Configuration parameters can be set via:
- Default configuration file (default.cfg)
- Command-line arguments (e.g., `--random_seed=123`)
- Runtime API calls (Config::set())

## Core Simulation Parameters

### random_seed

**Description:** Random seed for deterministic simulation execution
**Type:** Integer (`uint64_t`)
**Default Value:** 42
**Valid Range:** 0 to 2^64-1

**Purpose:**
- Ensures reproducible experiments
- Allows consistent debugging and testing
- Enables scientific validation of results

**Usage Examples:**
```cpp
// Set random seed for reproducible experiments
config.set("random_seed", 12345ULL);

// Use command-line argument
--random_seed=999
```

**Impact:**
- Affects all stochastic processes in the simulation
- Determines random neuron spike timing
- Influences weight initialization
- Controls random connection formation

**Performance Considerations:**
- Larger seeds provide better randomness distribution
- Small seeds may cause patterns to repeat quickly
- Seed selection affects experimental statistical power

### simulation_timestep

**Description:** Simulation timestep in seconds
**Type:** Floating-point (`double`)
**Default Value:** 0.001 (1ms)
**Valid Range:** 1e-6 to 0.1 (1μs to 100ms)

**Purpose:**
- Controls temporal resolution of simulation
- Determines accuracy of differential equation integration
- Affects stability of neural dynamics

**Usage Examples:**
```cpp
// Fine-grained simulation for biological accuracy
config.set("simulation_timestep", 0.0001);

// Coarse simulation for performance optimization
config.set("simulation_timestep", 0.01);
```

**Impact:**
- Smaller timesteps increase accuracy but decrease performance
- Larger timesteps may cause numerical instability
- Affects spike timing precision
- Influences computational load

**Performance Considerations:**
- Typical value: 1ms balances accuracy and performance
- 0.1ms suitable for real-time applications
- 0.001ms required for millisecond-precise spike timing
- Adaptive timestep algorithms can optimize both accuracy and speed

## Network Architecture Parameters

### neuron_count

**Description:** Total number of neurons in the brain
**Type:** Integer (`uint64_t`)
**Default Value:** 1000
**Valid Range:** 100 to 1,000,000

**Purpose:**
- Defines simulation scale and complexity
- Affects memory requirements
- Influences computational performance
- Determines network capacity for information storage

**Usage Examples:**
```cpp
// Small-scale experimental system
config.set("neuron_count", 100);

// Large-scale realistic brain model
config.set("neuron_count", 1000000);
```

**Impact:**
- Directly proportional to computational load
- Affects memory usage for neuron states
- Influences connectivity pattern generation
- Determines maximum information storage capacity

**Performance Considerations:**
- 100 neurons: Fast, simple debugging
- 10,000 neurons: Balanced performance
- 1,000,000 neurons: Requires optimization, parallel processing
- Each neuron typically requires ~100 bytes of memory

### region_count

**Description:** Number of brain regions
**Type:** Integer (`uint64_t`)
**Default Value:** 2
**Valid Range:** 1 to 100

**Purpose:**
- Defines hierarchical organization of neural populations
- Enables modular architecture design
- Supports regional specialization

**Usage Examples:**
```cpp
// Simple two-region model (sensory + motor)
config.set("region_count", 2);

// Multi-regional model with specialized areas
config.set("region_count", 10);
```

**Impact:**
- Affects connectivity matrix complexity
- Enables distributed processing
- Supports functional segregation
- Influences developmental progression

**Performance Considerations:**
- Region-to-region connectivity scales with O(n²)
- Inter-region connections typically sparser than intra-region
- Regional boundaries influence information flow
- More regions enable greater functional specialization

### connection_probability

**Description:** Connection probability for random connectivity
**Type:** Floating-point (`double`)
**Default Value:** 0.1 (10%)
**Valid Range:** 0.0 to 1.0

**Purpose:**
- Controls network connectivity density
- Balances integration vs. segregation
- Affects information flow and processing capacity

**Usage Examples:**
```cpp
// Sparse connectivity for energy efficiency
config.set("connection_probability", 0.01);

// Dense connectivity for high integration
config.set("connection_probability", 0.5);
```

**Impact:**
- Determines number of synaptic connections
- Affects network synchronization properties
- Influences robustness and fault tolerance
- Controls information transmission capacity

**Performance Considerations:**
- Sparse networks (0.01): Faster simulation, less integration
- Moderate connectivity (0.1): Balanced performance
- Dense networks (0.5): High integration, computationally expensive
- Connection probability affects spike propagation efficiency

## Plasticity Control Parameters

### plasticity_learning_rate

**Description:** Base learning rate for plasticity mechanisms
**Type:** Floating-point (`double`)
**Default Value:** 0.01
**Valid Range:** 0.0 to 1.0

**Purpose:**
- Scales overall plasticity strength
- Controls speed of synaptic weight changes
- Balances stability and adaptability

**Usage Examples:**
```cpp
// Fast learning for rapid adaptation
config.set("plasticity_learning_rate", 0.1);

// Slow learning for stable memory formation
config.set("plasticity_learning_rate", 0.001);
```

**Impact:**
- Higher rates accelerate learning but reduce stability
- Lower rates increase stability but slow adaptation
- Affects convergence of learning algorithms
- Determines memory formation and retention

**Performance Considerations:**
- 0.1: Fast initial learning, potential instability
- 0.01: Balanced learning and stability (typical)
- 0.001: Very slow learning, high stability
- Must be scaled appropriately for different plasticity rules

### stdp_ltp_weight

**Description:** STDP long-term potentiation weight (A⁺)
**Type:** Floating-point (`double`)
**Default Value:** 0.01
**Valid Range:** 0.0 to 1.0

**Purpose:**
- Controls synaptic strengthening from STDP
- Determines magnitude of LTP effects
- Influences balance between potentiation and depression

**Usage Examples:**
```cpp
// Strong LTP for robust memory formation
config.set("stdp_ltp_weight", 0.1);

// Weak LTP for fine-tuning
config.set("stdp_ltp_weight", 0.001);
```

**Impact:**
- Directly affects memory trace strength
- Influences pattern completion capabilities
- Determines stability of learned associations
- Affects competition between synapses

**Performance Considerations:**
- Typically slightly less than LTD weight for stability
- Must be balanced with stdp_ltd_weight
- Affects signal-to-noise ratio in memory retrieval
- Influences energy efficiency of synaptic changes

### stdp_ltd_weight

**Description:** STDP long-term depression weight (A⁻)
**Type:** Floating-point (`double`)
**Default Value:** 0.012
**Valid Range:** 0.0 to 1.0

**Purpose:**
- Controls synaptic weakening from STDP
- Provides stability to prevent runaway excitation
- Balances LTP effects for homeostatic regulation

**Usage Examples:**
```cpp
// Strong LTD for stability
config.set("stdp_ltd_weight", 0.02);

// Weak LTD for fine-tuning
config.set("stdp_ltd_weight", 0.005);
```

**Impact:**
- Prevents unbounded synaptic growth
- Enables competitive learning
- Maintains network stability
- Regulates overall network activity

**Performance Considerations:**
- Often slightly larger than LTP weight for stability
- Must be balanced with stdp_ltp_weight
- Affects synaptic turnover rate
- Influences energy cost of learning

### stdp_time_constant

**Description:** STDP time constant for exponential decay (τ)
**Type:** Floating-point (`double`)
**Default Value:** 20.0 (milliseconds)
**Valid Range:** 1.0 to 100.0 (ms)

**Purpose:**
- Controls temporal window for STDP learning
- Determines sensitivity to spike timing differences
- Affects temporal credit assignment

**Usage Examples:**
```cpp
// Wide STDP window for temporal integration
config.set("stdp_time_constant", 50.0);

// Narrow STDP window for precise timing
config.set("stdp_time_constant", 5.0);
```

**Impact:**
- Larger τ = longer learning window = more temporal integration
- Smaller τ = narrower window = more precise timing
- Affects temporal sequence learning capability
- Influences synchronization requirements

**Performance Considerations:**
- 10-50ms typical for biological systems
- Larger values increase computational complexity
- Narrow windows require precise spike timing
- Affects energy efficiency of learning

## Development Control Parameters

### development_synaptogenesis_rate

**Description:** Rate of new synapse formation during development
**Type:** Floating-point (`double`)
**Default Value:** 0.001
**Valid Range:** 0.0 to 0.01

**Purpose:**
- Controls construction of neural circuitry
- Implements developmental wiring program
- Determines initial network complexity

**Usage Examples:**
```cpp
// Aggressive synaptogenesis for complex networks
config.set("development_synaptogenesis_rate", 0.01);

// Conservative synaptogenesis for efficient networks
config.set("development_synaptogenesis_rate", 0.0001);
```

**Impact:**
- Affects initial network connectivity
- Determines developmental time course
- Influences information storage capacity
- Affects energy cost of development

**Performance Considerations:**
- Higher rates lead to more connections
- Slower rates allow for more selective wiring
- Must be balanced with pruning rate
- Affects developmental stage duration

### development_pruning_rate

**Description:** Rate of synapse pruning during development
**Type:** Floating-point (`double`)
**Default Value:** 0.0001
**Valid Range:** 0.0 to 0.001

**Purpose:**
- Eliminates weak or unused connections
- Refines network topology
- Improves efficiency and functionality

**Usage Examples:**
```cpp
// Aggressive pruning for efficiency
config.set("development_pruning_rate", 0.001);

// Conservative pruning for preservation
config.set("development_pruning_rate", 0.00001);
```

**Impact:**
- Reduces network connectivity
- Improves information transmission efficiency
- Enhances functional specialization
- Affects robustness and fault tolerance

**Performance Considerations:**
- Pruning-to-synaptogenesis ratio determines final density
- Activity-dependent pruning requires neural activity
- Too much pruning reduces network capacity
- Too little pruning leaves inefficient connections

## Neuromodulation Parameters

### dopamine_baseline

**Description:** Baseline dopamine level
**Type:** Floating-point (`double`)
**Default Value:** 0.1
**Valid Range:** 0.0 to 1.0

**Purpose:**
- Sets tonic dopamine activity
- Influences baseline neural excitability
- Affects overall neuromodulation state

**Usage Examples:**
```cpp
// High baseline for increased alertness
config.set("dopamine_baseline", 0.5);

// Low baseline for calm state
config.set("dopamine_baseline", 0.01);
```

**Impact:**
- Affects overall learning rate
- Influences attention and arousal
- Modulates reward sensitivity
- Determines behavioral drive

**Performance Considerations:**
- Baseline affects response magnitude to rewards
- Higher baseline increases overall plasticity
- Lower baseline reduces noise in signaling
- Must be balanced with phasic responses

## Reward Control Parameters

### reward_discount_factor

**Description:** Discount factor for reward prediction
**Type:** Floating-point (`double`)
**Default Value:** 0.99
**Valid Range:** 0.9 to 1.0

**Purpose:**
- Controls how future rewards are valued
- Balances immediate vs. delayed gratification
- Determines time horizon for decision making

**Usage Examples:**
```cpp
// Short time horizon (impatient)
config.set("reward_discount_factor", 0.9);

// Long time horizon (patient)
config.set("reward_discount_factor", 0.999);
```

**Impact:**
- Affects planning and decision making
- Determines exploration vs. exploitation tradeoff
- Influences behavioral strategy
- Affects value learning stability

**Performance Considerations:**
- Higher values = more patient behavior
- Lower values = more immediate-focused behavior
- Affects convergence of value functions
- Must match environmental temporal structure

## Environment Integration Parameters

### environment_name

**Description:** Name of the simulation environment
**Type:** String
**Default Value:** "GridWorld"
**Valid Values:** "GridWorld", "SimpleWorld", "ComplexWorld"

**Purpose:**
- Selects the interaction environment
- Determines sensory input characteristics
- Controls behavioral challenges

**Usage Examples:**
```cpp
// Simple navigation task
config.set("environment_name", "GridWorld");

// Complex multi-agent environment
config.set("environment_name", "ComplexWorld");
```

**Impact:**
- Determines available actions and rewards
- Affects sensory modality and complexity
- Influences learning difficulty
- Shapes behavioral objectives

**Performance Considerations:**
- GridWorld: Simple, fast, controlled
- SimpleWorld: More complex, realistic
- ComplexWorld: Highest challenge, computationally expensive
- Environment choice affects evaluation metrics

### environment_width, environment_height

**Description:** Dimensions of the simulation environment
**Type:** Integer (`uint64_t`)
**Default Value:** 10, 10
**Valid Range:** 5 to 100

**Purpose:**
- Defines spatial constraints for behavior
- Affects action space complexity
- Determines exploration requirements

**Usage Examples:**
```cpp
// Small compact environment
config.set("environment_width", 5);
config.set("environment_height", 5);

// Large expansive environment
config.set("environment_width", 50);
config.set("environment_height", 50);
```

**Impact:**n**Description:**
- **Purpose:**
- **Valid Range:**

### environment_width, environment_height

**Description:** Dimensions of the simulation environment
**Type:** Integer (`uint64_t`)
**Default Value:** 10, 10
**Valid Range:** 5 to 100

**Purpose:**
- Defines spatial constraints for behavior
- Affects action space complexity
- Determines exploration requirements

**Usage Examples:**
```cpp
// Small compact environment
config.set("environment_width", 5);
config.set("environment_height", 5);

// Large expansive environment
config.set("environment_width", 50);
config.set("environment_height", 50);
```

**Impact:**
- Directly proportional to action space size (width × height × actions)
- Affects exploration complexity and efficiency
- Determines computational cost of world simulation
- Influences movement planning and navigation strategies

**Performance Considerations:**
- 5×5: Fast simulation, simple behaviors
- 10×10: Balanced performance (default)
- 50×50: Complex behaviors, requires optimization
- Large environments require efficient pathfinding algorithms
- Environment size affects memory requirements for world representation

## Logging and Monitoring Parameters

### log_level

**Description:** Logging verbosity level
**Type:** String
**Default Value:** "INFO"
**Valid Values:** "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"

**Purpose:**
- Controls amount of runtime information output
- Balances debugging capability with performance
- Provides visibility into system behavior

**Usage Examples:**
```cpp
// Detailed debugging information
config.set("log_level", "DEBUG");

// Standard operational information (default)
config.set("log_level", "INFO");

// Only serious problems (quiet operation)
config.set("log_level", "ERROR");
```

**Impact:**
- Affects computational overhead
- Determines availability of debugging information
- Influences file I/O operations
- Helps with performance profiling

**Performance Considerations:**
- DEBUG: High overhead, detailed information
- INFO: Moderate overhead, useful for monitoring
- WARNING/ERROR: Low overhead, problem-focused
- File logging: Significant I/O overhead

### log_to_file

**Description:** Whether to log to file in addition to console
**Type:** Boolean
**Default Value:** false
**Valid Values:** true, false

**Purpose:**
- Provides persistent logging for analysis
- Enables post-simulation analysis
- Facilitates debugging of long-running simulations

**Usage Examples:**
```cpp
// Enable file logging
config.set("log_to_file", true);

// Disable file logging for performance
config.set("log_to_file", false);
```

**Impact:**
- Significant I/O overhead when enabled
- Provides historical data for analysis
- Enables automated monitoring
- Useful for production deployments

**Performance Considerations:**
- File I/O is much slower than console output
- Large simulations generate huge log files
- Consider log rotation for long runs
- Buffer logging to reduce I/O impact

### log_filename

**Description:** Filename for log output when file logging is enabled
**Type:** String
**Default Value:** "nlm.log"
**Valid Values:** Any valid filename

**Purpose:**
- Identifies log file for analysis
- Allows organization of multiple simulation logs
- Enables automated log processing

**Usage Examples:**
```cpp
config.set("log_filename", "simulation_run_001.log");
config.set("log_filename", "experiment_results.log");
```

**Impact:**
- Affects file system usage
- Determines log file organization
- Enables log file management
- Helps with debugging and analysis

## Performance Optimization Parameters

### max_simulation_steps

**Description:** Maximum number of simulation steps before termination
**Type:** Integer (`uint64_t`)
**Default Value:** 10000
**Valid Range:** 1000 to 1000000

**Purpose:**
- Prevents infinite simulation runs
- Controls computational budget
- Enables time-bounded experiments

**Usage Examples:**
```cpp
// Short exploratory simulation
config.set("max_simulation_steps", 1000);

// Long-term learning experiment
config.set("max_simulation_steps", 1000000);
```

**Impact:**
- Directly limits computational cost
- Determines learning time course
- Affects experimental conclusions
- Enables resource management

**Performance Considerations:**
- 1000 steps: Quick test, limited learning
- 10000 steps: Standard experiment (default)
- 1000000 steps: Long-term study, requires optimization
- Must balance scientific goals with computational resources

### simulation_time_limit

**Description:** Maximum simulation time in seconds (0 = unlimited)
**Type:** Floating-point (`double`)
**Default Value:** 0.0 (unlimited)
**Valid Range:** 0.0 to 86400.0 (24 hours)

**Purpose:**
- Alternative to step-based termination
- Real-time simulation control
- Resource management for long experiments

**Usage Examples:**
```cpp
// Time-bounded experiment
config.set("simulation_time_limit", 3600.0);

// Unlimited (until step limit)
config.set("simulation_time_limit", 0.0);
```

**Impact:**
- Provides alternative termination condition
- Useful for real-time applications
- Enables resource management
- Complements step-based limits

**Performance Considerations:**
- 3600s = 1 hour, typical for long experiments
- 86400s = 24 hours, maximum practical limit
- Time limits prevent runaway simulations
- Must be balanced with step limits

### visualization_enabled

**Description:** Whether to enable visualization (if available)
**Type:** Boolean
**Default Value:** false
**Valid Values:** true, false

**Purpose:**
- Provides real-time visualization of neural activity
- Enables interactive exploration
- Helps with debugging and understanding

**Usage Examples:**
```cpp
// Enable visualization for interactive debugging
config.set("visualization_enabled", true);

// Disable for performance (production)
config.set("visualization_enabled", false);
```

**Impact:**n**Description:**
- Significantly reduces computational performance
- Provides visual feedback and debugging
- Enables interactive exploration
- Requires additional system resources

**Performance Considerations:**
- Visualization typically adds 10-50x computational overhead
- Should be disabled for production simulations
- Useful for development and debugging
- Consider frame rate limitations for real-time

## Configuration File Examples

### Complete Configuration Template

```ini
# NLM Configuration File
# Phase 1 Configuration Template

# Core Parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Architecture
connection_probability = 0.1

# Learning
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation
dopamine_baseline = 0.1

# Reward
reward_discount_factor = 0.99

# Environment
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Performance
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
```

### Environment-Specific Configurations

**For High-Performance Computing:**
```ini
# Optimized for computational efficiency
neuron_count = 100000
region_count = 5
connection_probability = 0.05
log_level = WARNING
log_to_file = true
max_simulation_steps = 100000
visualization_enabled = false
```

**For Detailed Biological Modeling:**
```ini
# Optimized for biological accuracy
neuron_count = 10000
region_count = 20
connection_probability = 0.2
stdp_time_constant = 50.0
development_synaptogenesis_rate = 0.01
development_pruning_rate = 0.001
log_level = INFO
log_to_file = true
max_simulation_steps = 1000000
visualization_enabled = true
```

**For Quick Testing:**
```ini
# Optimized for rapid experimentation
neuron_count = 100
region_count = 2
connection_probability = 0.1
log_level = DEBUG
max_simulation_steps = 1000
simulation_time_limit = 300.0
```

## Configuration Validation

### Parameter Compatibility

Configuration parameters must be consistent:

1. **Architecture-Compute Balance:**
   - More neurons require more efficient connectivity
   - sparse connections enable scaling
   - Parameters must match available computational resources

2. **Learning-Stability Tradeoff:**
   - Higher learning rates require stronger stability mechanisms
   - Plasticity parameters must balance adaptation and retention
   - Development parameters must match architectural scale

3. **Temporal Resolution:**
   - Simulation timestep affects maximum spike rates
   - STDP time constant must match biological timescales
   - Environment dynamics must match simulation speed

### Recommended Parameter Ranges

| Parameter | Recommended Range | Best Practice |
|-----------|-------------------|----------------|
| random_seed | 1-2^64-1 | Use prime numbers for better distribution |
| simulation_timestep | 0.001-0.01 | Balance accuracy and performance |
| neuron_count | 1000-100000 | Start small, scale up |
| connection_probability | 0.01-0.2 | Sparse networks perform better |
| plasticity_learning_rate | 0.001-0.1 | Match biological learning rates |
| stdp_time_constant | 10-50 | Typical biological range |

## Configuration Management Best Practices

### Experimental Reproducibility

1. **Document All Parameters:** Record every configuration used
2. **Use Deterministic Seeds:** Always set random_seed explicitly
3. **Version Configuration Files:** Track changes to parameter sets
4. **Store Complete Configurations:** Save final parameter states

### Performance Optimization

1. **Start Conservative:** Begin with minimal configurations
2. **Profile and Iterate:** Measure performance bottlenecks
3. **Use Appropriate Scales:** Match parameters to computational resources
4. **Enable Efficient Algorithms:** Use sparse data structures

### Configuration Validation

1. **Check Parameter Compatibility:** Ensure parameters work together
2. **Validate Ranges:** Confirm all parameters are within valid ranges
3. **Test Edge Cases:** Verify boundary conditions
4. **Benchmark Performance:** Measure computational cost

## Configuration API Documentation

### Config::set() Method

```cpp
void Config::set(const std::string& key, const ConfigValue& value,
                 ConfigSource source = ConfigSource::Runtime)
```

**Parameters:**
- `key`: Configuration parameter name
- `value`: Parameter value (must match expected type)
- `source`: How the parameter was set (affects override priority)

**Override Priority:**
1. Command-line arguments (highest priority)
2. Configuration files
3. Runtime API calls (lowest priority)

### Config::get() Method

```cpp
template<typename T>
std::optional<T> Config::get(const std::string& key) const
```

**Returns:**
- `std::nullopt` if key doesn't exist or type mismatch
- `T` if key exists and type matches

### Config::getOr() Method

```cpp
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const
```

**Returns:**
- Configuration value if key exists
- defaultValue if key doesn't exist

### Configuration Parameter Examples

```cpp
// Set a string parameter
config.set("environment_name", "GridWorld");

// Set an integer parameter
config.set("neuron_count", 10000);

// Set a floating-point parameter
config.set("plasticity_learning_rate", 0.01);

// Set a boolean parameter
config.set("log_to_file", true);

// Get with type checking
auto timestep = config.get<double>("simulation_timestep");
if (timestep) {
    use_timestep = *timestep;
}

// Get with default
auto neurons = config.getOr<int64_t>("neuron_count", 1000);
```

## Configuration Validation Script

```bash
#!/bin/bash
# validate_config.sh - Validate NLM configuration parameters

CONFIG_FILE="configs/default.cfg"

validate_parameter() {
    local param=$1
    local value=$2
    local type=$3
    local min=$4
    local max=$5
    
    echo "Validating $param = $value ($type)..."
    
    # Type validation (simplified)
    case $type in
        int|integer)
            if ! [[ $value =~ ^[0-9]+$ ]]; then
                echo "ERROR: $param must be an integer"
                return 1
            fi
            ;;
        float|double|real)
            if ! [[ $value =~ ^-?[0-9]*\.?[0-9]+$ ]]; then
                echo "ERROR: $param must be a floating-point number"
                return 1
            fi
            ;;
        bool|boolean)
            if [[ $value != "true" && $value != "false" ]]; then
                echo "ERROR: $param must be true or false"
                return 1
            fi
            ;;
        string)
            if [[ -z $value ]]; then
                echo "ERROR: $param cannot be empty"
                return 1
            fi
            ;;
    esac
    
    # Range validation (if provided)
    if [[ -n $min ]]; then
        if (( $(echo "$value < $min" | bc -l) )); then
            echo "ERROR: $param must be >= $min"
            return 1
        fi
    fi
    
    if [[ -n $max ]]; then
        if (( $(echo "$value > $max" | bc -l) )); then
            echo "ERROR: $param must be <= $max"
            return 1
        fi
    fi
    
    echo "  OK: $param is valid"
    return 0
}

# Parameter definitions (simplified)
cat $CONFIG_FILE | while read -r line; do
    if [[ $line =~ ^([^=]+)=(.*)$ ]]; then
        param=$(echo $line | cut -d'=' -f1 | tr -d ' ')
        value=$(echo $line | cut -d'=' -f2- | tr -d ' ')
        
        # Skip comments and empty lines
        if [[ $param =~ ^#.*$ ]] || [[ -z $param ]]; then
            continue
        fi
        
        # Validate parameter (basic validation)
        case $param in
            random_seed|neuron_count|region_count|max_simulation_steps)
                validate_parameter "$param" "$value" "int" "0" "1000000"
                ;;
            simulation_timestep|plasticity_learning_rate|stdp_ltp_weight|stdp_ltd_weight|stdp_time_constant|reward_discount_factor|dopamine_baseline)
                validate_parameter "$param" "$value" "float" "0.0" "100.0"
                ;;
            connection_probability|development_synaptogenesis_rate|development_pruning_rate)
                validate_parameter "$param" "$value" "float" "0.0" "1.0"
                ;;
            log_to_file|visualization_enabled)
                validate_parameter "$param" "$value" "bool"
                ;;
            environment_width|environment_height)
                validate_parameter "$param" "$value" "int" "1" "100"
                ;;
            max_simulation_steps)
                validate_parameter "$param" "$value" "int" "1" "1000000"
                ;;
            simulation_time_limit)
                validate_parameter "$param" "$value" "float" "0.0" "86400.0"
                ;;
        esac
    fi
done

echo "Configuration validation complete."
