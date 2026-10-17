# NLM - Enhanced Easy Usage Guide for Beginners

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

---

## What's New in Enhanced NLM

### 🚀 **Major New Features**

1. **Enhanced Configuration System** - JSON support, multiple presets, validation
2. **Developmental Learning** - Brains that mature and change over time
3. **Curiosity-Driven Exploration** - Agents that explore based on novelty
4. **Structural Plasticity** - Brains that can grow new connections
5. **Advanced Memory Systems** - Working, episodic, and semantic memory
6. **Better Error Handling** - Robust error detection and reporting
7. **Performance Optimizations** - Enhanced performance for large simulations
8. **Comprehensive Documentation** - Detailed guides and examples for all features

### 📚 **Quick Start (Enhanced Version)**

```python
import pynlm

# Step 1: Create a brain with enhanced configuration
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Step 2: Create an enhanced agent with development
agent = pynlm.create_agent_brain(brain)
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)  # NEW! Grow new connections
agent.enable_development(True)            # NEW! Brain matures over time
agent.enable_curiosity(True)              # NEW! Explore new things

print("Enhanced agent created!")
print("Neurons:", brain.get_total_neuron_count())
print("Available features:", agent.get_developmental_stage())
```

### 🎯 **Example 2: Complete Enhanced Simulation**

```python
import pynlm

def run_enhanced_simulation(num_steps=1000):
    """Run a complete agent simulation with all enhanced features."""
    
    # 1. Create brain with Phase 6 configuration (NEW!)
    config = pynlm.create_phase6_config(neuronCount=2000, maxSteps=num_steps)
    
    # 2. Create and initialize brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.create_agent_brain(brain)
    
    # 4. Create and configure world
    world = pynlm.create_simple_world()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    world.set_agent_start(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable ALL advanced subsystems (NEW!)
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 7. Run simulation loop with enhanced features
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input in brain
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decode_motor_command()
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Compute reward
        reward = world.compute_reward(brain.get_regions()[0].get())
        
        # Apply ENHANCED reward modulation (NEW!)
        predicted_reward = 0.0  # You could implement prediction
        agent.apply_reward_modulation(reward, predicted_reward)
        
        # Update development (NEW!)
        agent.update_development(0.1)
        
        # Print enhanced statistics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.get_firing_neuron_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Development stage: {agent.get_developmental_stage()}")
            print(f"  Memory episodes: {brain.get_episodic_memory().get_episode_count() if brain.get_episodic_memory() else 0}")
    
    return brain, agent, world

# Run enhanced simulation
brain, agent, world = run_enhanced_simulation(1000)
print("Enhanced simulation complete!")
```

---

## What's New in Enhanced NLM

### 🔧 **Enhanced Configuration Features**

```python
import pynlm

# Create configuration
config = pynlm.create_default_config()

# NEW! JSON configuration support
json_config = """
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.1,
        "development_enabled": true
    },
    "neuromod": {
        "curiosity_enabled": true,
        "novelty_threshold": 0.3
    }
}
"""

# Load from JSON
config.from_json(json_config)

# Get values with type safety and defaults
neuron_count = config.get_int("brain.neuron_count", 1000)
development_enabled = config.get_bool("brain.development_enabled", false)

# NEW! Configuration validation
invalid_keys = config.validate()
if invalid_keys:
    print("Invalid keys:", invalid_keys)

# NEW! Configuration reporting
config.generate_report()
```

### 🆕 **New Configuration Methods**

| Method | Description |
|--------|-------------|
| `create_minimal_config()` | Create minimal configuration |
| `create_phase6_config()` | Create Phase 6 optimized configuration |
| `create_performance_config()` | Create performance optimized configuration |
| `create_config_preset()` | Create configuration using preset |
| `from_json()` | Load configuration from JSON string |
| `to_json()` | Convert configuration to JSON string |
| `validate()` | Validate configuration and return invalid keys |
| `generate_report()` | Generate detailed configuration report |

### 📊 **New Configuration Types**

```python
# 1. Minimal configuration for basic operation
minimal_config = pynlm.create_minimal_config()

# 2. Phase 6 configuration (NEW!)
phase6_config = pynlm.create_phase6_config(neuronCount=2000, maxSteps=10000)

# 3. Performance optimized configuration (NEW!)
perf_config = pynlm.create_performance_config(neuronCount=5000, seed=42)

# 4. Configuration presets (NEW!)
experimental_config = pynlm.create_config_preset("phase6")
production_config = pynlm.create_config_preset("performance")
```

---

## Quick Reference - Enhanced Features

### 🆕 **New Classes and Methods**

| What you want | Code |
|--------------|------|
| Create default config | `pynlm.create_default_config()` |
| Create minimal config | `pynlm.create_minimal_config()` |
| Create Phase 6 config | `pynlm.create_phase6_config(neuronCount, maxSteps)` |
| Create performance config | `pynlm.create_performance_config(neuronCount, seed)` |
| Create config preset | `pynlm.create_config_preset("phase6")` |
| Load from JSON | `config.from_json(jsonStr)` |
| Export to JSON | `config.to_json()` |
| Validate config | `config.validate()` |
| Generate report | `config.generate_report()` |
| Get developmental stage | `agent.get_developmental_stage()` |
| Get curiosity level | `agent.get_curiosity_level()` |
| Get novelty level | `agent.get_novelty_level()` |
| Enable development | `agent.enable_development(True/False)` |
| Enable structural plasticity | `agent.enable_structural_plasticity(True/False)` |
| Enable curiosity | `agent.enable_curiosity(True/False)` |

### 🔧 **Enhanced Command Line Interface**

```bash
# Enhanced NLM with Phase 6 integration
./nlm_phase6_demo --config configs/advanced.cfg --checkpoint path/to/checkpoint

# Performance optimized
./nlm --brain.neuron_count=5000 --simulation.timestep=0.005 --checkpoint.enabled=true

# Development mode
./nlm --neuromod.curiosity.enabled=true --neuromod.novelty.threshold=0.2

# JSON configuration support
./nlm --config config.json
```

### 📁 **New Configuration File Types**

| File Type | Description |
|-----------|-------------|
| `*.cfg` | Advanced configuration (key=value format) |
| `*.json` | JSON configuration (NEW!) |
| `*.toml` | TOML configuration (planned) |

---

## 🚀 Getting Started with Enhanced Features

### Step 1: Install Enhanced NLM

```bash
pip install pynlm  # Install latest enhanced version
```

### Step 2: Create Your First Enhanced Agent

```python
import pynlm

# Quick start with enhanced features
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

agent = pynlm.create_agent_brain(brain)
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)  # NEW!
agent.enable_development(True)            # NEW!
agent.enable_curiosity(True)              # NEW!

print("Enhanced agent created successfully!")
```

### Step 3: Run Your First Enhanced Simulation

```python
import pynlm

# Create enhanced brain and agent
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
agent = pynlm.create_agent_brain(brain)

# Create world
world = pynlm.create_simple_world()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run enhanced simulation
for step in range(200):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())

print("Enhanced simulation complete!")
```

### Step 4: Explore Advanced Features

Once you're comfortable, try these enhanced features:

1. **Create different configuration types:** `create_phase6_config()`, `create_performance_config()`
2. **Use JSON configuration:** `config.from_json()`, `config.to_json()`
3. **Enable development:** `agent.enable_development(True)`
4. **Enable structural plasticity:** `agent.enable_structural_plasticity(True)`
5. **Use curiosity-driven exploration:** `agent.enable_curiosity(True)`
6. **Validate configurations:** `config.validate()`
7. **Generate reports:** `config.generate_report()`

---

## 🎯 What Makes NLM Enhanced

### New Features Summary

1. **Developmental Learning** - Brains that mature and change over time through developmental stages
2. **Curiosity-Driven Exploration** - Agents that explore based on novelty and prediction error
3. **Structural Plasticity** - Brains that can grow new connections and prune old ones
4. **Advanced Configuration** - Multiple configuration formats (JSON, presets, validation)
5. **Enhanced Memory Systems** - Working, episodic, semantic, and procedural memory
6. **Better Error Handling** - Robust error detection and comprehensive reporting
7. **Performance Optimizations** - Enhanced performance for large-scale simulations
8. **Comprehensive Documentation** - Detailed guides and examples for all features

### Comparison: Basic vs Enhanced

| Feature | Basic NLM | Enhanced NLM |
|---------|-----------|--------------|
| Configuration | Simple key=value | JSON + multiple presets |
| Development | None | Full developmental stages |
| Curiosity | None | Novelty-driven exploration |
| Structural Plasticity | None | Synaptogenesis and pruning |
| Memory | Basic | Working + Episodic + Semantic |
| Validation | Basic | Comprehensive validation |
| Error Handling | Basic | Robust error reporting |
| Performance | Basic | Optimized for large simulations |

---

## 📚 Enhanced Documentation

### Available Documentation Files

1. **`easy_usage.md`** - Enhanced beginner-friendly guide with new features
2. **`HOW_TO_USE.md`** - Enhanced comprehensive technical documentation
3. **`docs/ARCHITECTURE.md`** - System architecture overview
4. **`docs/SCIENCE.md`** - Scientific background
5. **`docs/ROADMAP.md`** - Future development roadmap
6. **`docs/PHASE6_FINAL_AUDIT.md`** - Phase 6 integration audit

### New Examples Included

- **Developmental Learning**: Complete simulation showing brain maturation
- **Curiosity-Driven Exploration**: Enhanced agent that explores based on novelty
- **Advanced Configuration**: JSON configuration and multiple presets
- **Performance Optimization**: Large-scale simulation configurations
- **Complete Agent Training**: Full agent simulation with all features

---

## 🎉 Welcome to Enhanced NLM!

You now have access to the **fully enhanced NLM** with:

1. **Advanced developmental learning** systems
2. **Curiosity-driven exploration** agents
3. **Structural plasticity** capabilities
4. **Enhanced configuration** options
5. **Multiple memory systems**
6. **Comprehensive error handling**
7. **Performance optimizations**
8. **Complete documentation**

Get started today and discover what an **enhanced computational brain** can do!

---

**Enhanced NLM - Making computational brains smarter, more adaptive, and more human-like!**