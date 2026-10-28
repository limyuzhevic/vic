# Python NLM API Improvements

This document outlines comprehensive improvements to the NLM Python API to make it more intuitive, better documented, and feature-rich.

## Current State Analysis

The existing Python bindings (`/workspace/app/python/bindings.cpp`) provide:
- Basic brain, world, and agent interfaces
- Configuration management
- Sensory processing and motor output
- Simulation controls

However, the API has several issues:
1. **Inconsistent documentation** - Method docstrings don't match actual implementation
2. **Missing advanced features** - Many Phase 6 capabilities not exposed to Python
3. **Poor Python idioms** - Some methods don't follow Python conventions
4. **Incomplete coverage** - Some important components missing

## Proposed Improvements

### 1. Core Brain Interface Improvements

#### Current Issues:
- `brain.initialize()` returns `bool` but Python developers expect exceptions
- `brain.step()` has ambiguous signature overloads
- Memory system accessors return raw pointers (error-prone)

#### Proposed Solutions:

```python
# Simplified brain interface with Pythonic methods
class Brain:
    def __init__(self, config=None):
        """Create brain with optional configuration"""
        
    def initialize(self, timestep=0.001):
        """Initialize brain - raises RuntimeError on failure"""
        
    def step(self, steps=1, timestep=0.001):
        """Take simulation steps - accepts both single and multiple steps"""
        
    # Property-based access to memory systems
    @property
    def working_memory(self):
        """Get working memory traces"""
        
    @property  
    def episodic_memory(self):
        """Get episodic memory storage"""
        
    @property
    def associative_memory(self):
        """Get associative memory"""
        
    @property
    def prediction_system(self):
        """Get prediction system"""
        
    @property
    def planner(self):
        """Get neural planner"""
        
    @property
    def concept_formation(self):
        """Get concept formation system"""
        
    @property
    def attention(self):
        """Get attention system"""
        
    @property
    def development_system(self):
        """Get development system"""
        
    @property
    def neuromodulation(self):
        """Get neuromodulation systems"""
```

### 2. Enhanced Agent Interface

#### Current Issues:
- AgentBrain has complex initialization requiring both brain and world
- Neuromodulation is fragmented across multiple methods
- Development tracking is scattered

#### Proposed Solutions:

```python
class AgentBrain:
    def __init__(self, brain, world=None):
        """Create agent with brain and optional world"""
        
    def setup(self, world, timestep=0.001):
        """Complete agent setup with world"""
        
    # Simplified neuromodulation access
    @property
    def curiosity(self):
        """Get current curiosity level"""
        
    @property
    def novelty(self):
        """Get novelty detection"""
        
    @property
    def dopamine(self):
        """Get dopamine/neuromodulation level"""
        
    @property
    def prediction_error(self):
        """Get prediction error"""
        
    # Development tracking
    def update_development(self, timestep=0.001):
        """Update development - automatically tracks stage changes"""
        
    def get_development_info(self):
        """Get comprehensive development information"""
        return {
            'stage': self.stage,
            'age': self.age,
            'plasticity': self.plasticity_modifier,
            'progress': self.get_development_progress()
        }
```

### 3. Simple World Interface

#### Current Issues:
- World API is overly complex with many redundant methods
- Action system is fragmented
- Collision and interaction systems are hard to use

#### Proposed Solutions:

```python
class SimpleWorld:
    def __init__(self, width=20.0, height=20.0, vision_width=16, vision_height=16):
        """Create world with standard dimensions"""
        
    # Simplified action creation
    def move(self, x, y=None, angle=None):
        """Create movement action"""
        
    def look(self, angle):
        """Create look action"""
        
    def interact(self, x=None, y=None):
        """Create interaction action"""
        
    # Easy observation access
    def get_vision(self, agent_x, agent_y, agent_angle):
        """Get vision observation at agent position"""
        
    # Object management helpers
    def add_resource(self, x, y, value=10.0):
        """Add resource at location"""
        
    def add_hazard(self, x, y, damage=-15.0):
        """Add hazard at location"""
        
    # Environment interaction
    def step(self, action, timestep=0.1):
        """Take world step with action"""
        
    def get_reward(self, action):
        """Get reward for action"""
```

### 4. Advanced Neural Network Features

#### Addressed Missing Phase 6 Capabilities:

```python
class AdvancedBrain:
    def __init__(self, config):
        """Create brain with full Phase 6 integration"""
        
    # Memory system controls
    def enable_memory_replay(self, interval=100, duration=50):
        """Enable episodic memory replay"""
        
    def get_memory_stats(self):
        """Get comprehensive memory statistics"""
        return {
            'working_memory_traces': self.get_working_memory().active_traces,
            'episodic_episodes': self.get_episodic_memory().get_episode_count(),
            'episodes_for_replay': self.get_episodic_memory().get_episodes_for_replay(5),
            'memory_activity': self.get_working_memory().get_memory_activity()
        }
        
    # Neuromodulation control
    def enable_curiosity(self, enabled=True):
        """Enable/disable curiosity-driven exploration"""
        
    def enable_reward_learning(self, enabled=True):
        """Enable/disable reward-based learning"""
        
    def enable_prediction(self, enabled=True):
        """Enable/disable prediction system"""
        
    # Advanced brain dynamics
    def set_plasticity_rate(self, rate):
        """Adjust global plasticity rate"""
        
    def get_network_stats(self):
        """Get comprehensive neural network statistics"""
        return {
            'excitatory_inhibition_ratio': self.get_excitation_inhibition_ratio(),
            'average_firing_rate': self.get_average_firing_rate(),
            'total_spikes': self.get_total_spike_count(),
            'active_neurons': self.get_active_neuron_count(),
            'region_stats': self.get_region_statistics()
        }
```

### 5. Session and Experiment Management

#### Add Comprehensive Session Tracking:

```python
class NLMExperiment:
    def __init__(self, config):
        """Create experiment session"""
        
    def run_episode(self, brain, world, steps, verbose=False):
        """Run a complete episode with tracking"""
        
    def run_simulation(self, brain, world, steps, **kwargs):
        """Run extended simulation with comprehensive monitoring"""
        
    def get_session_stats(self):
        """Get comprehensive session statistics"""
        return {
            'total_steps': self.steps,
            'avg_reward': self.total_reward / self.steps if self.steps > 0 else 0,
            'memory_usage': self.get_memory_stats(),
            'learning_curve': self.learning_curve,
            'neuromodulation_history': self.neuromodulation_history,
            'development_timeline': self.development_timeline
        }
```

### 6. Convenience Factory Functions

#### Create High-Level API:

```python
# Easy setup functions for common scenarios
def create_agent_brain(config=None, enable_features=True):
    """Create fully configured agent brain with all features enabled"""
    
    brain = create_brain(config)
    agent = AgentBrain(brain)
    
    if enable_features:
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
    return agent

def create_learning_world(width=15, height=15, complexity='medium'):
    """Create learning world with objects and challenges"""
    
    world = create_simple_world(width, height)
    
    # Add resources based on complexity
    if complexity in ['medium', 'high']:
        for _ in range(5):
            x = random.uniform(1, width-1)
            y = random.uniform(1, height-1)
            world.add_resource(x, y, random.uniform(5, 20))
            
    # Add hazards based on complexity
    if complexity == 'high':
        for _ in range(2):
            x = random.uniform(1, width-1)
            y = random.uniform(1, height-1)
            world.add_hazard(x, y, random.uniform(-10, -5))
            
    return world

def run_agent_training(agent, world, episodes=100, steps_per_episode=200):
    """Standard agent training loop with comprehensive monitoring"""
    
    total_reward = 0
    learning_metrics = {
        'rewards': [],
        'curiosity_levels': [],
        'memory_usage': [],
        'development_stages': []
    }
    
    for episode in range(episodes):
        episode_reward = 0
        
        for step in range(steps_per_episode):
            # Get observation
            percept = world.get_sensory_percept()
            
            # Process sensory input
            agent.process_sensory_input(percept)
            
            # Get brain action
            brain = agent.get_brain()
            brain.step(step)
            
            # Decode motor command
            action = agent.decode_motor_command()
            
            # Apply action to world
            result = world.apply_motor_command(action, step * 0.1)
            episode_reward += result.reward
            
            # Apply reward modulation
            agent.apply_reward_modulation(result.reward, 0.0)
            
            # Update development
            agent.update_development(0.1)
            
        total_reward += episode_reward
        
        # Record metrics
        learning_metrics['rewards'].append(episode_reward)
        learning_metrics['curiosity_levels'].append(agent.get_curiosity_level())
        learning_metrics['memory_usage'].append(agent.get_working_memory().get_active_traces())
        learning_metrics['development_stages'].append(agent.get_developmental_stage())
        
        if episode % 10 == 0:
            print(f"Episode {episode}: Reward={episode_reward:.2f}, "
                  f"Curiosity={agent.get_curiosity_level():.3f}")
            
    avg_reward = total_reward / episodes
    print(f"Training complete. Average reward: {avg_reward:.2f}")
    
    return learning_metrics
```

## Implementation Priority

### High Priority (Critical Fixes):
1. Fix `NeuralAssociativeMemory` implementation issues
2. Ensure all Python bindings have correct documentation
3. Fix build system integration

### Medium Priority (Feature Enhancements):
1. Implement advanced Python API with improved interfaces
2. Add comprehensive session tracking
3. Create convenience factory functions
4. Update documentation to reflect Phase 6 capabilities

### Low Priority (Optimization):
1. Performance optimizations
2. Additional example scripts
3. Advanced tutorials

## Benefits of These Improvements

1. **Improved Usability**: Python developers can now use NLM with familiar Python idioms
2. **Better Documentation**: Clear, accurate documentation matches actual implementation
3. **Advanced Features**: Full Phase 6 capabilities exposed to Python users
4. **Power User Support**: Advanced functions for researchers and developers
5. **Simplified Setup**: High-level factory functions for common scenarios
6. **Comprehensive Monitoring**: Built-in tracking and statistics collection
7. **Error Handling**: Better error messages and exception handling
8. **Consistency**: Consistent API across all NLM components

## Next Steps

1. Analyze current Python bindings for inconsistencies
2. Fix `NeuralAssociativeMemory` implementation
3. Create improved Python bindings with better documentation
4. Implement advanced API features
5. Update documentation to reflect Phase 6
6. Test and verify all improvements
7. Create examples and tutorials

These improvements will make NLM significantly more accessible to Python developers while maintaining full access to the powerful Phase 6 neural simulation capabilities.