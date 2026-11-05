# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Advanced Configuration

NLM now includes an advanced configuration system for fine-tuning brain parameters and behavior. This allows users to customize the neural network's properties, plasticity rules, memory systems, neuromodulation, cognition, development, performance, checkpointing, reward computation, sensory/motor sensitivity, and environmental settings.

### Configuration Categories

1. **Neural** - Neuron properties and dynamics
   - neuron_excitability (0.0-1.0)
   - neuron_threshold (0.0-1.0)
   - neuron_leak (0.0-1.0)
   - neuron_reset_potential (-1.0-1.0)

2. **Plasticity** - Learning and adaptation mechanisms
   - stdp_learning_rate (0.0-0.1)
   - stdp_time_constant (1-100)
   - hebbian_strength (0.0-1.0)
   - structural_plasticity_rate (0.0-0.001)

3. **Memory** - Memory system parameters
   - working_memory_capacity (10-1000)
   - episodic_memory_limit (10-10000)
   - associative_memory_strength (0.0-1.0)

4. **Neuromodulation** - Neuromodulator settings
   - dopamine_baseline (0.0-1.0)
   - dopamine_sensitivity (0.0-1.0)
   - curiosity_threshold (0.0-1.0)
   - novelty_decay_rate (0.0-1.0)

5. **Cognition** - Cognitive processing parameters
   - planner_planning_depth (1-10)
   - concept_formation_threshold (0.0-1.0)
   - attention_strength (0.0-1.0)
   - neural_planning_enabled (true/false)

6. **Development** - Developmental stage parameters
   - developmental_plasticity_rate (0.0-1.0)
   - critical_period_duration (0-10000)
   - maturation_rate (0.0-1.0)
   - development_stages (comma-separated list)

7. **Performance** - Performance and optimization settings
   - spike_propagation_speed (0.1-10.0)
   - plasticity_update_frequency (0-100)
   - memory_update_frequency (0-100)
   - checkpointer_frequency (0-100000)

8. **Checkpoint** - Checkpoint and persistence settings
   - checkpoint_compression (0=off, 1=balanced, 2=max)
   - checkpoint_backup_count (0-100)
   - checkpoint_auto_save (true/false)
   - checkpoint_integrity_check (true/false)

9. **Reward** - Reward computation settings
   - reward_computation_method (classic/phasic/actor-critic)
   - reward_scale_factor (0.0-10.0)
   - reward_decay_rate (0.0-1.0)
   - prediction_error_weight (0.0-1.0)

10. **Sensory/Motor** - Sensory and motor sensitivity
    - vision_sensitivity (0.0-1.0)
    - touch_sensitivity (0.0-1.0)
    - proprioception_sensitivity (0.0-1.0)
    - motor_noise_level (0.0-1.0)

11. **Environment** - Environment interaction settings
    - environment_complexity (0=simple, 1=complex)
    - reward_distribution (sparse/dense)
    - task_difficulty (0.0-1.0)

### Configuration Commands

#### Basic Configuration

```bash
# Load configuration from file
nlm_config_tool --load config.txt

# Save configuration to file
nlm_config_tool --save config_modified.txt

# Validate configuration
nlm_config_tool --validate
```

#### Advanced Configuration

```bash
# Show all advanced options
nlm_config_tool --show-advanced

# Show configuration for a specific category
nlm_config_tool --category neural
nlm_config_tool --category plasticity
nlm_config_tool --category memory

# Set configuration options
nlm_config_tool --neuron_excitability 0.8
nlm_config_tool --stdp_learning_rate 0.05
nlm_config_tool --working_memory_capacity 500
nlm_config_tool --dopamine_baseline 0.2

# Using short format
nlm_config_tool -e 0.8 -l 0.05 -w 500
```

#### Configuration File Format

NLM configuration files use a simple key=value format:

```
# Neural configuration
neuron_excitability = 0.5
neuron_threshold = 0.5
neuron_leak = 0.05

# Plasticity configuration
stdp_learning_rate = 0.01
stdp_time_constant = 20.0
hebbian_strength = 0.5

# Memory configuration
working_memory_capacity = 100
episodic_memory_limit = 1000

# Save to file
nlm_config_tool --save config.txt
```

### Configuration Validation

Before running your customized NLM simulation, you can validate the configuration:

```bash
nlm_config_tool --validate
```

This will check for invalid parameter ranges and report any issues.

### Configuration for Advanced Users

The advanced configuration system allows you to:

1. **Fine-tune neural properties** for specific experimental conditions
2. **Adjust plasticity rules** to control learning speed and strength
3. **Configure memory systems** to balance memory capacity and computational efficiency
4. **Customize neuromodulation** to control reward and motivation systems
5. **Tune cognitive parameters** to influence planning and decision-making
6. **Control development trajectories** to study age-dependent learning
7. **Optimize performance settings** for specific hardware or use cases
8. **Configure checkpointing** for persistence and recovery
9. **Customize reward computation** for different task environments
10. **Adjust sensory/motor sensitivity** for different interaction modalities

### Configuration Examples

Here are some example configurations for common use cases:

#### High-Performance Learning
```
# High-performance learning configuration
spike_propagation_speed = 5.0
plasticity_update_frequency = 10
memory_update_frequency = 20
checkpointer_frequency = 5000

# Strong plasticity
stdp_learning_rate = 0.05
stdp_time_constant = 30.0
hebbian_strength = 0.8
structural_plasticity_rate = 0.001

# Large working memory
working_memory_capacity = 500
episodic_memory_limit = 5000

# Balanced neuromodulation
dopamine_baseline = 0.2
dopamine_sensitivity = 0.7
curiosity_threshold = 0.4
```

#### Developmental Learning
```
# Developmental learning configuration
developmental_plasticity_rate = 0.8
critical_period_duration = 2000
maturation_rate = 0.3
development_stages = "Initial,CriticalPeriod,Maturation,Adult"

# High early plasticity, lower later
neuron_excitability = 0.6
neuron_threshold = 0.4
neuron_leak = 0.03

# Strong reward modulation for exploration
reward_computation_method = "actor-critic"
reward_scale_factor = 2.0
prediction_error_weight = 0.8
```

#### Simple Environment Learning
```
# Simple environment learning
environment_complexity = 0.3
reward_distribution = "dense"
task_difficulty = 0.2

# Efficient settings for simple tasks
spike_propagation_speed = 2.0
plasticity_update_frequency = 5
memory_update_frequency = 5
checkpointer_frequency = 20000

# Conservative plasticity
stdp_learning_rate = 0.005
stdp_time_constant = 15.0
hebbian_strength = 0.3
```

### Configuration Best Practices

1. **Start with default values** and modify only what's necessary
2. **Validate configuration** before running simulations
3. **Use separate configuration files** for different experimental conditions
4. **Document your configuration choices** for reproducibility
5. **Test configuration changes** incrementally
6. **Monitor performance** when using performance-related settings
7. **Consider memory constraints** when configuring memory systems
8. **Balance exploration vs. exploitation** in neuromodulation settings
9. **Validate developmental timelines** against your experimental timeline
10. **Test checkpointing** to ensure proper persistence and recovery

### Configuration Troubleshooting

If you encounter issues with your configuration:

1. **Use `--validate`** to check for parameter range errors
2. **Check file syntax** for configuration files
3. **Review documentation** for valid parameter values
4. **Start with minimal changes** and add more gradually
5. **Use `--show-advanced`** to see all available options
6. **Backup your configuration** before making major changes
7. **Test in stages** to isolate problematic settings

### Configuration File Management

Advanced users can manage configuration files:

1. **Create template configurations** for common experimental setups
2. **Use version control** for configuration changes
3. **Create configuration libraries** for different experiment types
4. **Document configuration rationale** for future reference
5. **Use environment variables** for dynamic configuration
6. **Implement configuration inheritance** for modular setups

The advanced configuration system provides NLM users with unprecedented control over brain parameters, enabling sophisticated experimental designs and optimized performance across a wide range of use cases.
