# Example 3: Development and Learning Simulation
# ============================================

## Overview

This example demonstrates how development stages and learning mechanisms interact during a simulation. It covers developmental trajectories, critical periods, synaptic plasticity, and long-term learning processes in artificial neural systems.

## Learning Objectives

- Understand developmental trajectories in artificial neural systems
- Learn how plasticity changes with development
- Observe critical period effects and learning windows
- Understand developmental stage-dependent learning
- Experience long-term simulation tracking and analysis
- Master NLM developmental concepts and applications

## Code Example

```python
#!/usr/bin/env python3
"""
Example 3: Development and Learning Simulation
=============================================

This example demonstrates how development stages and learning mechanisms
interact during a simulation.
"""

import pynlm
import json
import os
from pathlib import Path

def development_learning_example():
    """Simulate development and learning over time."""
    print("=== Development and Learning Example ===")
    
    # Create a configuration optimized for development studies
    config = pynlm.createDefaultConfig()
    
    # Set development parameters
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_start", 100)
    config.set("development.critical_period_end", 500)
    config.set("development.pruning_rate", 0.0001)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable development and learning
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Track development stages
    developmental_stages = []
    plasticity_rates = []
    firing_rates = []
    
    # Run simulation with developmental tracking
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get motor command and apply
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Record development metrics
        if step % 50 == 0:
            developmental_stages.append({
                "step": step,
                "stage": brain.getDevelopmentalStage().value,
                "plasticity_rate": brain.getSynapticPlasticityRate(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel()
            })
            
            # Update development
            agent.updateDevelopment(0.1)
    
    # Save developmental data
    dev_data = {
        "config": {
            "neuron_count": config.get("brain.neuron_count", 1000),
            "plasticity_learning_rate": config.get("plasticity.stdp.learning_rate", 0.001)
        },
        "developmental_stages": developmental_stages
    }
    
    output_dir = os.path.expanduser("~/nlm_dev_data")
    os.makedirs(output_dir, exist_ok=True)
    
    with open(f"{output_dir}/development_simulation.json", 'w') as f:
        json.dump(dev_data, f, indent=2)
    
    print(f"Development simulation completed!")
    print(f"Recorded {len(developmental_stages)} developmental snapshots")
    print(f"Saved to: {output_dir}/development_simulation.json")
    
    # Print summary
    print(f"\nDevelopment Summary:")
    print(f"  Final developmental stage: {developmental_stages[-1]['stage']}")
    print(f"  Average firing rate: {sum(d['firing_rate'] for d in developmental_stages) / len(developmental_stages):.3f}")
    print(f"  Final curiosity level: {developmental_stages[-1]['curiosity']:.3f}")
    
    return brain, agent, world, dev_data

if __name__ == "__main__":
    development_learning_example()
```

## Key Concepts

### 1. Developmental Stages

```python
def developmental_stages_tracking(brain, steps):
    """Track developmental stages over simulation."""
    stage_history = []
    
    for step in range(steps):
        brain.step(step)
        
        # Record developmental stage
        stage = brain.getDevelopmentalStage()
        stage_info = {
            'step': step,
            'stage': stage.value,
            'stage_name': stage.name,
            'plasticity_rate': brain.getSynapticPlasticityRate(),
            'firing_rate': brain.getAverageFiringRate()
        }
        
        stage_history.append(stage_info)
        
        # Update development (if agent available)
        # This would be called from the main simulation loop
    
    return stage_history
```

### 2. Critical Period Dynamics

```python
class CriticalPeriod:
    """Manage critical period dynamics in development."""
    
    def __init__(self, start_step, end_step):
        self.start_step = start_step
        self.end_step = end_step
        self.current_step = 0
        self.in_critical_period = False
        self.plasticity_modifiers = {}
    
    def update(self):
        """Update critical period status."""
        self.current_step += 1
        self.in_critical_period = self.start_step <= self.current_step <= self.end_step
        
        # Calculate plasticity modifier
        if self.in_critical_period:
            # Peak plasticity during critical period
            age_in_cp = self.current_step - self.start_step
            cp_duration = self.end_step - self.start_step
            self.plasticity_modifiers['critical_period'] = 1.0 + 0.5 * (age_in_cp / cp_duration)
        else:
            # Reduced plasticity outside critical period
            self.plasticity_modifiers['critical_period'] = 0.3
        
        # Apply other modifiers
        self.apply_plasticity_modifiers()
    
    def apply_plasticity_modifiers(self):
        """Apply all plasticity modifiers to brain."""
        # This would integrate with brain plasticity system
        # For demonstration, we just return the modifiers
        return self.plasticity_modifiers
```

### 3. Synaptic Plasticity and Pruning

```python
def synaptic_plasticity_dynamics(brain, config):
    """Model synaptic plasticity and pruning over development."""
    
    # Initial plasticity
    initial_plasticity = config.get("development.initial_plasticity", 1.0)
    synaptogenesis_rate = config.get("development.synaptogenesis_rate", 0.001)
    pruning_rate = config.get("development.pruning_rate", 0.0001)
    
    # Track synaptic dynamics
    synaptic_stats = {
        'initial_synapses': 0,
        'synaptogenesis_events': 0,
        'pruning_events': 0,
        'current_synapses': 0
    }
    
    for step in range(1000):
        # Synaptogenesis (formation of new synapses)
        new_synapses = synaptogenesis_rate * brain.getTotalNeuronCount()
        synaptic_stats['synaptogenesis_events'] += new_synapses
        
        # Pruning (elimination of synapses)
        pruned_synapses = pruning_rate * synaptic_stats['current_synapses']
        synaptic_stats['pruning_events'] += pruned_synapses
        
        # Update current synapse count
        synaptic_stats['current_synapses'] += new_synapses - pruned_synapses
        
        # Record stats
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Synaptogenesis: {new_synapses:.1f} synapses")
            print(f"  Pruning: {pruned_synapses:.1f} synapses")
            print(f"  Current synapses: {synaptic_stats['current_synapses']:.1f}")
    
    return synaptic_stats
```

### 4. Learning Across Developmental Stages

```python
def developmental_learning_curves(brain, agent, world, steps):
    """Track learning across different developmental stages."""
    
    learning_data = {
        'stage_performance': {},
        'learning_rates': {},
        'skill_acquisition': {}
    }
    
    # Define developmental stages
    stages = {
        1: {'name': 'Infancy', 'range': (0, 100), 'plasticity_factor': 1.5},
        2: {'name': 'Childhood', 'range': (100, 300), 'plasticity_factor': 1.0},
        3: {'name': 'Adolescence', 'range': (300, 500), 'plasticity_factor': 0.7},
        4: {'name': 'Adulthood', 'range': (500, steps), 'plasticity_factor': 0.5}
    }
    
    current_stage = 1
    
    for step in range(steps):
        # Update stage if needed
        for stage_num, stage_info in stages.items():
            if stage_num > current_stage and step >= stage_info['range'][0]:
                current_stage = stage_num
                print(f"\nEntering developmental stage {current_stage}: {stage_info['name']}")
                print(f"Plasticity factor: {stage_info['plasticity_factor']}")
        
        # Run simulation
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record learning metrics
        if step % 100 == 0:
            stage_performance = {
                'stage': current_stage,
                'firing_rate': brain.getAverageFiringRate(),
                'plasticity_rate': brain.getSynapticPlasticityRate(),
                'learning_rate': calculate_learning_rate(agent, step),
                'skill_level': calculate_skill_level(brain, step)
            }
            
            learning_data['stage_performance'][step] = stage_performance
            learning_data['learning_rates'][step] = stage_performance['learning_rate']
            learning_data['skill_acquisition'][step] = stage_performance['skill_level']
            
            print(f"  Performance: {stage_performance['firing_rate']:.3f} Hz")
            print(f"  Plasticity: {stage_performance['plasticity_rate']:.3f}")
            print(f"  Learning rate: {stage_performance['learning_rate']:.3f}")
            print(f"  Skill level: {stage_performance['skill_level']:.3f}")
    
    return learning_data
```

## Expected Output

### Sample Simulation Output

```
=== Development and Learning Example ===
Development simulation completed!
Recorded 20 developmental snapshots
Saved to: ~/nlm_dev_data/development_simulation.json

Development Summary:
  Final developmental stage: 4
  Average firing rate: 142.3
  Final curiosity level: 0.789

Developmental Stage Analysis:
  Stage 1 (0-100): Learning rate = 1.0, Firing rate = 125.4
  Stage 2 (100-300): Learning rate = 0.6, Firing rate = 138.7
  Stage 3 (300-500): Learning rate = 0.3, Firing rate = 145.2
  Stage 4 (500-1000): Learning rate = 0.1, Firing rate = 148.9

Synaptic Dynamics:
  Initial synapses: 0
  Synaptogenesis events: 125.0
  Pruning events: 0.0
  Current synapses: 125.0
```

### Detailed Developmental Tracking

```
=== Developmental Stage Details ===
Stage 1: Infancy (Steps 0-100)
  Plasticity factor: 1.5
  Initial learning rate: 0.8
  Learning rate trajectory: High -> Moderate
  Skill acquisition: Rapid

Stage 2: Childhood (Steps 100-300)
  Plasticity factor: 1.0
  Initial learning rate: 0.5
  Learning rate trajectory: Moderate -> Low
  Skill acquisition: Steady

Stage 3: Adolescence (Steps 300-500)
  Plasticity factor: 0.7
  Initial learning rate: 0.3
  Learning rate trajectory: Low -> Minimal
  Skill acquisition: Fine-tuning

Stage 4: Adulthood (Steps 500-1000)
  Plasticity factor: 0.5
  Initial learning rate: 0.1
  Learning rate trajectory: Minimal -> Stable
  Skill acquisition: Maintenance
```

## Key Concepts

### 1. Developmental Trajectories

```python
# Different developmental patterns
class DevelopmentalTrajectories:
    def __init__(self):
        self.trajectories = {
            'typical': {'plasticity_curve': 'inverted_u', 'learning_curve': 'sigmoid'},
            'accelerated': {'plasticity_curve': 'stretched_u', 'learning_curve': 'steeper_sigmoid'},
            'delayed': {'plasticity_curve': 'shifted_u', 'learning_curve': 'delayed_sigmoid'},
            'extended': {'plasticity_curve': 'prolonged_u', 'learning_curve': 'prolonged_sigmoid'}
        }
    
    def simulate_trajectory(self, trajectory_type, steps):
        """Simulate development with specific trajectory."""
        if trajectory_type not in self.trajectories:
            raise ValueError(f"Unknown trajectory type: {trajectory_type}")
        
        curve_params = self.trajectories[trajectory_type]
        
        # Simulate developmental progression
        development_data = []
        
        for step in range(steps):
            # Calculate plasticity based on trajectory
            plasticity = self.calculate_plasticity(
                step, steps, curve_params['plasticity_curve']
            )
            
            # Calculate learning based on trajectory
            learning = self.calculate_learning(
                step, steps, curve_params['learning_curve']
            )
            
            development_data.append({
                'step': step,
                'plasticity': plasticity,
                'learning': learning,
                'stage': self.determine_developmental_stage(step, steps)
            })
        
        return development_data
    
    def calculate_plasticity(self, step, total_steps, curve_type):
        """Calculate plasticity level based on developmental trajectory."""
        age_ratio = step / total_steps
        
        if curve_type == 'inverted_u':
            # Typical inverted U curve
            return 1.0 * (1.0 - abs(2.0 * age_ratio - 1.0))
        elif curve_type == 'stretched_u':
            # Extended critical period
            return 1.0 * (1.0 - abs(1.5 * age_ratio - 1.0))
        elif curve_type == 'shifted_u':
            # Delayed critical period
            shifted_age = max(0, age_ratio - 0.3)
            return 1.0 * (1.0 - abs(2.0 * shifted_age - 1.0))
        elif curve_type == 'prolonged_u':
            # Prolonged critical period
            stretched_age = min(1.0, age_ratio * 0.7)
            return 1.0 * (1.0 - abs(1.43 * stretched_age - 1.0))
        
        return 0.0
    
    def calculate_learning(self, step, total_steps, curve_type):
        """Calculate learning rate based on developmental trajectory."""
        age_ratio = step / total_steps
        
        if curve_type == 'sigmoid':
            # Standard sigmoid learning curve
            return 1.0 / (1.0 + np.exp(-10 * (age_ratio - 0.5)))
        elif curve_type == 'steeper_sigmoid':
            # Faster learning curve
            return 1.0 / (1.0 + np.exp(-15 * (age_ratio - 0.5)))
        elif curve_type == 'delayed_sigmoid':
            # Delayed learning curve
            shifted_age = max(0, age_ratio - 0.2)
            return 1.0 / (1.0 + np.exp(-10 * (shifted_age - 0.5)))
        elif curve_type == 'prolonged_sigmoid':
            # Prolonged learning curve
            stretched_age = min(1.0, age_ratio * 0.8)
            return 1.0 / (1.0 + np.exp(-8 * (stretched_age - 0.5)))
        
        return 0.0
```

### 2. Plasticity Dynamics

```python
# Plasticity regulation and control
class PlasticityController:
    def __init__(self, config):
        self.config = config
        self.plasticity_history = []
        self.synapse_strengths = []
        self.learning_rates = []
    
    def apply_developmental_plasticity(self, brain, step):
        """Apply plasticity rules based on developmental stage."""
        # Get current developmental stage
        dev_stage = brain.getDevelopmentalStage()
        
        # Get developmental parameters
        initial_plasticity = self.config.get("development.initial_plasticity", 1.0)
        critical_period_start = self.config.get("development.critical_period_start", 100)
        critical_period_end = self.config.get("development.critical_period_end", 500)
        
        # Calculate current plasticity level
        plasticity_level = self.calculate_current_plasticity(
            step, initial_plasticity, critical_period_start, critical_period_end
        )
        
        # Get STDP parameters
        stdp_lr = self.config.get("plasticity.stdp.learning_rate", 0.01)
        stdp_ltp = self.config.get("plasticity.stdp.ltp_weight", 0.01)
        stdp_ltd = self.config.get("plasticity.stdp.ltd_weight", 0.012)
        
        # Apply plasticity to brain
        # This would integrate with actual brain plasticity mechanisms
        current_plasticity = self.modulate_stdp_parameters(
            stdp_lr, stdp_ltp, stdp_ltd, plasticity_level
        )
        
        # Store plasticity history
        self.plasticity_history.append({
            'step': step,
            'developmental_stage': dev_stage.value,
            'plasticity_level': plasticity_level,
            'stdp_learning_rate': current_plasticity['stdp_learning_rate'],
            'stdp_ltp_weight': current_plasticity['stdp_ltp_weight'],
            'stdp_ltd_weight': current_plasticity['stdp_ltd_weight']
        })
        
        return current_plasticity
    
    def calculate_current_plasticity(self, step, initial_plasticity, 
                                     critical_period_start, critical_period_end):
        """Calculate current plasticity level based on developmental stage."""
        if step < critical_period_start:
            # Pre-critical period: high plasticity
            return initial_plasticity * 1.2
        elif step <= critical_period_end:
            # Critical period: peak plasticity
            age_in_cp = step - critical_period_start
            cp_duration = critical_period_end - critical_period_start
            cp_factor = age_in_cp / cp_duration
            return initial_plasticity * (1.0 + 0.5 * cp_factor)
        else:
            # Post-critical period: reduced plasticity
            return initial_plasticity * 0.6
    
    def modulate_stdp_parameters(self, base_ltp, base_ltd, base_lr, plasticity_modifier):
        """Modulate STDP parameters based on plasticity level."""
        return {
            'stdp_learning_rate': base_lr * plasticity_modifier,
            'stdp_ltp_weight': base_ltp * plasticity_modifier,
            'stdp_ltd_weight': base_ltd * plasticity_modifier
        }
    
    def track_synapse_dynamics(self, brain, steps):
        """Track synapse formation and elimination over time."""
        synapse_stats = {
            'initial_synapses': 0,
            'current_synapses': 0,
            'synaptogenesis_events': 0,
            'pruning_events': 0,
            'total_events': 0
        }
        
        for step in range(steps):
            # Get current brain stats
            neuron_count = brain.getTotalNeuronCount()
            
            # Estimate synapse formation (simplified)
            synaptogenesis = int(neuron_count * 0.1 * 0.001)  # 10% connectivity * rate
            
            # Estimate synapse elimination
            pruning = int(synapse_stats['current_synapses'] * 0.001)  # 0.1% pruning rate
            
            # Update synapse count
            synapse_stats['current_synapses'] += synaptogenesis - pruning
            
            # Record events
            synapse_stats['synaptogenesis_events'] += synaptogenesis
            synapse_stats['pruning_events'] += pruning
            synapse_stats['total_events'] += synaptogenesis + pruning
            
            if step % 100 == 0:
                print(f"Step {step}: {synapse_stats['current_synapses']} synapses")
                print(f"  Synaptogenesis: {synapse_stats['synaptogenesis_events']}")
                print(f"  Pruning: {synapse_stats['pruning_events']}")
        
        return synapse_stats
```

## Practical Applications

### 1. Educational Research

```python
def educational_research_simulation():
    """Simulate learning in educational settings."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("development.initial_plasticity", 1.2)
    config.set("development.critical_period_start", 50)
    config.set("development.critical_period_end", 300)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable development and learning
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Simulate learning stages
    learning_stages = {
        'implicit_learning': [],
        'explicit_learning': [],
        'skill_acquisition': [],
        'knowledge_retention': []
    }
    
    for step in range(1000):
        # Educational learning simulation
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        agent.updateDevelopment(0.1)
        
        # Record educational metrics
        if step % 100 == 0:
            learning_stages['implicit_learning'].append(agent.getImplicitLearningLevel())
            learning_stages['explicit_learning'].append(agent.getExplicitLearningLevel())
            learning_stages['skill_acquisition'].append(agent.getSkillLevel())
            learning_stages['knowledge_retention'].append(agent.getKnowledgeRetention())
    
    return brain, agent, world, learning_stages
```

### 2. Animal Behavior Studies

```python
def animal_behavior_simulation():
    """Simulate animal behavior and learning patterns."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 600)
    config.set("development.initial_plasticity", 1.5)
    config.set("development.critical_period_start", 100)
    config.set("development.critical_period_end", 400)
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # Enable animal-specific behaviors
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableSocialLearning(True)
    
    # Animal behavior simulation
    behavior_data = {
        'exploration_patterns': [],
        'learning_curves': [],
        'social_interactions': [],
        'memory_usage': []
    }
    
    for step in range(1200):
        # Animal behavior simulation
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        agent.updateDevelopment(0.1)
        
        # Record behavior patterns
        if step % 150 == 0:
            behavior_data['exploration_patterns'].append(agent.getExplorationPattern())
            behavior_data['learning_curves'].append(agent.getLearningCurve())
            behavior_data['social_interactions'].append(agent.getSocialInteractionLevel())
            behavior_data['memory_usage'].append(agent.getMemoryUsage())
    
    return brain, agent, world, behavior_data
```

### 3. Therapeutic Applications

```python
def therapeutic_simulation():
    """Simulate therapeutic interventions and treatments."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 700)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_start", 150)
    config.set("development.critical_period_end", 500)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable therapeutic interventions
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCognitiveEnhancement(True)
    
    # Therapeutic intervention simulation
    intervention_data = {
        'intervention_effects': [],
        'recovery_progress': [],
        'skill_improvements': [],
        'neuroplasticity': []
    }
    
    for step in range(1500):
        # Therapeutic intervention
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        agent.updateDevelopment(0.1)
        
        # Record intervention effects
        if step % 200 == 0:
            intervention_data['intervention_effects'].append(agent.getInterventionEffect())
            intervention_data['recovery_progress'].append(agent.getRecoveryProgress())
            intervention_data['skill_improvements'].append(agent.getSkillImprovement())
            intervention_data['neuroplasticity'].append(agent.getNeuroplasticityLevel())
    
    return brain, agent, world, intervention_data
```

## Comparison with Other Examples

### Example 3 vs Example 1

| Feature | Example 1 | Example 3 |
|---------|-----------|-----------|
| **Focus** | Basic brain simulation | Developmental learning |
| **Time Scale** | Short-term | Long-term development |
| **Key Concepts** | Brain creation, basic simulation | Developmental stages, plasticity |
| **Complexity** | Low | Medium |

### Example 3 vs Example 2

| Feature | Example 2 | Example 3 |
|---------|-----------|-----------|
| **Goal** | Demonstrate integration | Study developmental processes |
| **Components** | Brain + Agent + World | Brain + Agent + World |
| **Focus** | Real-time behavior | Developmental dynamics |
| **Learning** | Sensorimotor skills | Critical period effects |

### Example 3 vs Example 5

| Feature | Example 3 | Example 5 |
|---------|-----------|-----------|
| **Objective** | Study developmental processes | Optimize performance |
| **Metrics** | Developmental indicators | Performance metrics |
| **Approach** | Process observation | Systematic optimization |
| **Focus** | How development works | How well system works |

## Error Handling and Debugging

### Common Issues

1. **Critical Period Miscalculation**
   - **Problem**: Critical period timing incorrect
   - **Solution**: Validate critical period parameters
   - **Command**: Use config.set("development.critical_period_start", X) with valid values

2. **Developmental Stage Tracking**
   - **Problem**: Development stage progression incorrect
   - **Solution**: Check development stage logic and transitions
   - **Command**: Add debugging to developmental stage updates

3. **Plasticity Regulation**
   - **Problem**: Plasticity changes not as expected
   - **Solution**: Verify plasticity calculation and application
   - **Command**: Debug plasticity regulation code

### Debugging Tools

```python
import pynlm
import time

def debug_development_simulation():
    """Debug development simulation with detailed logging."""
    print("=== Development Simulation Debug ===")
    
    # Create debug configuration
    config = pynlm.createDefaultConfig()
    config.set("log_level", "DEBUG")
    
    # Create agent with development enabled
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    # Enable debug logging
    debug_mode = True
    
    # Run simulation with debug output
    for step in range(100):
        if debug_mode:
            print(f"\n--- Step {step} ---")
            print(f"Development stage: {brain.getDevelopmentalStage().name}")
            print(f"Plasticity rate: {brain.getSynapticPlasticityRate():.3f}")
            print(f"Learning rate: {agent.getLearningRate():.3f}")
            print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        
        # Run simulation step
        world.update(timestep=0.1)
        
        if debug_mode:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            agent.updateDevelopment(0.1)
            
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Development stage progression: {brain.getDevelopmentalStage().value}")
    
    return brain, agent, world
```

## Performance Considerations

### Development Simulation Efficiency

```python
def optimize_development_simulation(agent, world, steps):
    """Optimize development simulation for performance."""
    
    # Batch processing for faster execution
    def batch_development_simulation(start_step, end_step):
        for step in range(start_step, end_step):
            # Optimized development simulation loop
            world.update(timestep=0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            agent.updateDevelopment(0.1)
        
        return end_step - start_step
    
    # Run simulation in optimized batches
    batch_size = 200
    for batch_start in range(0, steps, batch_size):
        batch_end = min(batch_start + batch_size, steps)
        batch_development_simulation(batch_start, batch_end)
        
        # Optional: save developmental data
        save_developmental_state(agent, world, batch_end)
    
    print(f"Completed {steps} steps in optimized batches of {batch_size}")
```

### Memory Management for Development

```python
def memory_efficient_development(agent, world, steps, memory_limit_mb=800):
    """Run development simulation with memory management."""
    import psutil
    import gc
    
    process = psutil.Process()
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    max_memory = initial_memory
    
    for step in range(steps):
        # Monitor memory
        current_memory = process.memory_info().rss / 1024 / 1024
        max_memory = max(max_memory, current_memory)
        
        # Check memory limit
        if max_memory > memory_limit_mb:
            print(f"Memory limit exceeded: {max_memory:.1f} MB > {memory_limit_mb} MB")
            break
        
        # Run development step
        run_development_step(agent, world, step)
        
        # Periodic garbage collection
        if step % 200 == 0:
            gc.collect()
            print(f"Step {step}: Memory {current_memory:.1f} MB")
    
    final_memory = process.memory_info().rss / 1024 / 1024
    print(f"Development simulation completed. Final memory: {final_memory:.1f} MB")
    print(f"Peak memory: {max_memory:.1f} MB")
    
    return brain, agent, world, max_memory
```

## Extensions and Customization

### 1. Cross-Species Comparisons

```python
def cross_species_development_simulation():
    """Simulate development across different species."""
    
    # Species-specific development parameters
    species_configs = {
        'human': {
            'neuron_count': 1000,
            'initial_plasticity': 1.2,
            'critical_period_start': 100,
            'critical_period_end': 500,
            'development_rate': 0.8
        },
        'mouse': {
            'neuron_count': 500,
            'initial_plasticity': 1.5,
            'critical_period_start': 50,
            'critical_period_end': 200,
            'development_rate': 1.2
        },
        'fruit_fly': {
            'neuron_count': 200,
            'initial_plasticity': 2.0,
            'critical_period_start': 10,
            'critical_period_end': 50,
            'development_rate': 2.0
        }
    }
    
    results = {}
    
    for species_name, params in species_configs.items():
        print(f"\nSimulating {species_name} development...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params['neuron_count'])
        config.set("development.initial_plasticity", params['initial_plasticity'])
        config.set("development.critical_period_start", params['critical_period_start'])
        config.set("development.critical_period_end", params['critical_period_end'])
        
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        agent.initialize(world)
        
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run species-specific development
        results[species_name] = run_species_development(brain, agent, world, 1000, params)
    
    return results
```

### 2. Environmental Influence Studies

```python
def environmental_influence_simulation():
    """Study environmental effects on development."""
    
    # Different environmental conditions
    environments = {
        'enriched': {
            'width': 30, 'height': 30,
            'stimulus_variety': 'high',
            'social_interactions': 'high',
            'novelty_rate': 'high'
        },
        'standard': {
            'width': 20, 'height': 20,
            'stimulus_variety': 'medium',
            'social_interactions': 'medium',
            'novelty_rate': 'medium'
        },
        'deprived': {
            'width': 15, 'height': 15,
            'stimulus_variety': 'low',
            'social_interactions': 'low',
            'novelty_rate': 'low'
        }
    }
    
    environmental_results = {}
    
    for env_name, env_config in environments.items():
        print(f"\nSimulating {env_name} environment...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("development.initial_plasticity", 1.0)
        
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(**env_config)
        agent.initialize(world)
        
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run environmental simulation
        env_results = run_environmental_development(brain, agent, world, 1200, env_config)
        environmental_results[env_name] = env_results
    
    return environmental_results
```

## Conclusion

Example 3 provides a comprehensive foundation for understanding developmental processes in NLM. By working through this example, you can:

1. **Master Developmental Concepts**: Learn how NLM models developmental trajectories
2. **Understand Plasticity**: Study synaptic plasticity, critical periods, and learning windows
3. **Explore Applications**: Apply developmental principles to research, education, and therapeutic contexts
4. **Analyze Extensions**: Create cross-species comparisons and environmental influence studies
5. **Build Expertise**: Develop deep understanding of developmental neuroscience applications

This example serves as a bridge between basic NLM usage and advanced developmental neuroscience, introducing key concepts and patterns used throughout biological and artificial intelligence research.

---

*This example is part of the NLM (Neural Learning Machine) examples suite.
For additional examples, documentation, and support, visit the NLM project.*
