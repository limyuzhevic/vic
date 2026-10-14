# Advanced NLM Example Scripts

## Overview

This document provides comprehensive example scripts for advanced NLM usage, covering complex scenarios, performance optimization, and real-world applications.

## 1. Advanced Multi-Modal Sensory Processing

### Example 1: Complex Sensory Integration
```python
import pynlm
import numpy as np
import time

class AdvancedSensoryProcessor:
    """Advanced sensory processor for complex environments."""
    
    def __init__(self, config):
        self.config = config
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=50, 
            height=50, 
            visionWidth=16, 
            visionHeight=16,
            audioChannels=8,
            touchSensors=4
        )
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Enable advanced subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        self.initialize()
    
    def initialize(self):
        """Initialize the advanced sensory processor."""
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Set up advanced sensory configuration
        self.sensory_history = []
        self.world_state_history = []
        self.performance_metrics = {}
    
    def process_multimodal_input(self, step):
        """Process multi-modal sensory input with advanced features."""
        
        # 1. Update world
        self.world.update(0.1)
        
        # 2. Get multi-modal sensory input
        visual_percept = self.world.getSensoryPercept()
        audio_percept = self.get_audio_percept()
        touch_percept = self.get_touch_percept()
        internal_state = self.get_internal_state()
        
        # 3. Combine and process modalities
        combined_percept = self.combine_modalities(
            visual_percept, audio_percept, touch_percept, internal_state
        )
        
        # 4. Process through agent
        self.agent.processSensoryInput(combined_percept)
        
        # 5. Run brain step
        self.brain.step(step)
        
        # 6. Get motor command
        action = self.agent.decodeMotorCommand()
        
        # 7. Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # 8. Update development
        self.agent.updateDevelopment(0.1)
        
        # 9. Record performance metrics
        self.record_performance_metrics(step, action, combined_percept)
        
        # 10. Return processed output
        return {
            'visual': visual_percept,
            'audio': audio_percept,
            'touch': touch_percept,
            'combined': combined_percept,
            'action': action,
            'performance': self.performance_metrics.get(step, {})
        }
    
    def combine_modalities(self, visual, audio, touch, internal):
        """Advanced multimodal sensory fusion."""
        
        # Create enhanced combined percept
        combined = pynlm.SensoryPercept()
        
        # Fuse visual information
        if visual:
            combined.addVisualFeatures(visual.getVisualFeatures())
            combined.setPosition(visual.getPosition())
        
        # Fuse audio information
        if audio:
            combined.addAudioFeatures(audio.getAudioFeatures())
            combined.setSoundDirection(audio.getSoundDirection())
        
        # Fuse touch information
        if touch:
            combined.addTouchFeatures(touch.getTouchFeatures())
            combined.setTemperature(touch.getTemperature())
        
        # Integrate internal state
        if internal:
            combined.setInternalState(internal.getInternalState())
        
        # Apply attention mechanism
        combined.applyAttention(self.agent.getAttentionWeights())
        
        # Apply novelty detection
        novelty_score = self.calculate_novelty(combined)
        combined.setNovelty(novelty_score)
        
        # Apply curiosity modulation
        curiosity_weight = self.agent.getCuriosityLevel()
        combined.applyCuriosityModulation(curiosity_weight)
        
        return combined
    
    def get_audio_percept(self):
        """Get advanced audio sensory input."""
        # Placeholder for advanced audio processing
        return None
    
    def get_touch_percept(self):
        """Get advanced touch sensory input."""
        # Placeholder for advanced touch processing
        return None
    
    def get_internal_state(self):
        """Get internal state information."""
        # Return brain internal state
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'memory_usage': len(self.brain.getSpikeSystem().getActiveNeurons()),
            'development_stage': self.brain.getDevelopmentalStage(),
            'curiosity_level': self.agent.getCuriosityLevel()
        }
    
    def calculate_novelty(self, percept):
        """Calculate novelty score for the percept."""
        # Implementation would use internal memory systems
        # For now, return a placeholder
        return 0.5
    
    def record_performance_metrics(self, step, action, percept):
        """Record advanced performance metrics."""
        self.performance_metrics[step] = {
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'action_type': str(action),
            'percept_novelty': self.calculate_novelty(percept),
            'processing_time': time.time()
        }
    
    def run_simulation(self, steps=1000):
        """Run advanced multi-modal simulation."""
        print("Starting advanced multi-modal simulation...")
        
        results = {
            'steps': list(range(steps)),
            'metrics': self.performance_metrics,
            'final_state': {
                'firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'development_stage': self.brain.getDevelopmentalStage()
            }
        }
        
        for step in range(steps):
            output = self.process_multimodal_input(step)
            
            if step % 100 == 0:
                print(f"Step {step}/{steps}: "
                      f"Firing Rate: {output['performance'].get('firing_rate', 0):.3f}, "
                      f"Curiosity: {output['performance'].get('curiosity_level', 0):.3f}")
        
        print("Simulation completed!")
        return results

# Usage example
if __name__ == "__main__":
    # Create advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    
    # Create advanced processor
    processor = AdvancedSensoryProcessor(config)
    
    # Run simulation
    results = processor.run_simulation(500)
```

## 2. Advanced Learning and Adaptation

### Example 2: Lifelong Learning Simulation
```python
import pynlm
import numpy as np
from typing import List, Dict, Any

class LifelongLearningAgent:
    """Agent that demonstrates lifelong learning capabilities."""
    
    def __init__(self, config):
        self.config = config
        self.learning_phases = self.initialize_learning_phases()
        self.knowledge_base = self.initialize_knowledge_base()
        self.skill_tree = self.initialize_skill_tree()
        
        # Create core components
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        self.world.configure(
            width=30, height=30, 
            visionWidth=12, visionHeight=12
        )
        
        # Initialize subsystems
        self.initialize_learning_subsystems()
    
    def initialize_learning_phases(self):
        """Initialize learning phases for development."""
        return {
            'sensory_learning': {'active': True, 'progress': 0.0},
            'motor_learning': {'active': True, 'progress': 0.0},
            'social_learning': {'active': False, 'progress': 0.0},
            'abstract_learning': {'active': False, 'progress': 0.0}
        }
    
    def initialize_knowledge_base(self):
        """Initialize knowledge representation system."""
        return {
            'facts': [],
            'patterns': [],
            'procedures': [],
            'concepts': []
        }
    
    def initialize_skill_tree(self):
        """Initialize skill development tree."""
        return {
            'basic_sensory': {'level': 0, 'requirements': [], 'rewards': []},
            'basic_motor': {'level': 0, 'requirements': [], 'rewards': []},
            'pattern_recognition': {'level': 0, 'requirements': [], 'rewards': []},
            'problem_solving': {'level': 0, 'requirements': [], 'rewards': []},
            'abstract_reasoning': {'level': 0, 'requirements': [], 'rewards': []}
        }
    
    def initialize_learning_subsystems(self):
        """Initialize advanced learning subsystems."""
        
        # Create agent brain
        self.agent.initialize(self.world)
        
        # Enable all learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Configure for lifelong learning
        self.configure_lifelong_learning()
    
    def configure_lifelong_learning(self):
        """Configure agent for lifelong learning."""
        
        # Set development parameters
        self.agent.setDevelopmentGoal('autonomous_skill_acquisition')
        self.agent.setLearningRate(0.01)
        self.agent.setExplorationRate(0.3)
        
        # Configure memory systems for lifelong learning
        self.brain.setMemoryConsolidationThreshold(0.7)
        self.brain.enableForgettingDecay(True)
        self.brain.setForgettingRate(0.001)
    
    def learn_from_experience(self, experience):
        """Learn from a single experience."""
        
        # Extract features from experience
        features = self.extract_experience_features(experience)
        
        # Update knowledge base
        self.update_knowledge_base(features)
        
        # Develop new skills if possible
        new_skills = self.detect_new_skill_necessity(features)
        
        # Apply structural plasticity for learning
        self.apply_structural_plasticity(features)
        
        # Update skill tree
        self.update_skill_tree(features, new_skills)
        
        # Adjust learning phases
        self.adjust_learning_phases(features)
    
    def extract_experience_features(self, experience):
        """Extract relevant features from experience."""
        
        features = {
            'sensory_input': experience.get('percept'),
            'action_taken': experience.get('action'),
            'outcome': experience.get('reward'),
            'context': experience.get('context'),
            'novelty': experience.get('novelty', 0.0),
            'difficulty': experience.get('difficulty', 0.5),
            'time_stamp': experience.get('time', 0)
        }
        
        # Add neural features
        features.update({
            'firing_pattern': self.extract_firing_pattern(),
            'memory_activation': self.get_memory_activation(),
            'attention_focus': self.get_attention_focus(),
            'emotional_state': self.get_emotional_state()
        })
        
        return features
    
    def extract_firing_pattern(self):
        """Extract current firing pattern from brain."""
        # Implementation would extract relevant firing patterns
        # For now, return a placeholder
        return {'pattern': 'default', 'strength': 0.5}
    
    def get_memory_activation(self):
        """Get current memory activation levels."""
        # Implementation would query memory systems
        return {'working_memory': 0.7, 'episodic_memory': 0.3}
    
    def get_attention_focus(self):
        """Get current attention focus."""
        # Implementation would query attention mechanisms
        return {'primary_task': 'navigation', 'secondary_tasks': []}
    
    def get_emotional_state(self):
        """Get current emotional state."""
        # Implementation would query neuromodulation systems
        return {
            'arousal': 0.5,
            'valence': 0.3,
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel()
        }
    
    def update_knowledge_base(self, features):
        """Update knowledge base with new features."""
        
        # Store fact if significant
        if features['novelty'] > 0.5:
            self.knowledge_base['facts'].append({
                'content': features['sensory_input'],
                'context': features['context'],
                'significance': features['novelty'],
                'timestamp': features['time_stamp']
            })
        
        # Update patterns if repeated
        similar_patterns = self.find_similar_patterns(features)
        if similar_patterns:
            self.knowledge_base['patterns'].append({
                'pattern': features,
                'frequency': len(similar_patterns),
                'last_occurrence': features['time_stamp']
            })
        
        # Update procedures based on actions and outcomes
        if features['outcome'] > 0:
            self.knowledge_base['procedures'].append({
                'action': features['action_taken'],
                'conditions': features['context'],
                'success_rate': features['outcome'],
                'timestamp': features['time_stamp']
            })
    
    def find_similar_patterns(self, features):
        """Find similar patterns in knowledge base."""
        # Implementation would search knowledge base for similar patterns
        return []  # Placeholder
    
    def detect_new_skill_necessity(self, features):
        """Detect if new skills are needed."""
        new_skills = []
        
        # Check if current skills are insufficient
        if features['difficulty'] > 0.7:
            new_skills.append('advanced_problem_solving')
        
        if features['novelty'] > 0.8:
            new_skills.append('adaptability')
        
        return new_skills
    
    def apply_structural_plasticity(self, features):
        """Apply structural plasticity for learning."""
        
        # Enable plasticity for new learning
        self.agent.enableStructuralPlasticity(True)
        
        # Create new synapses for new patterns
        if features['novelty'] > 0.6:
            # Implementation would create new connections
            pass
        
        # Strengthen effective connections
        if features['outcome'] > 0:
            # Implementation would strengthen existing connections
            pass
    
    def update_skill_tree(self, features, new_skills):
        """Update skill tree based on learning."""
        
        # Update skill levels
        for skill in new_skills:
            if skill in self.skill_tree:
                self.skill_tree[skill]['level'] += 1
        
        # Check for prerequisite completion
        for skill_name, skill_data in self.skill_tree.items():
            if self.check_prerequisites(skill_name):
                self.unlock_advanced_skill(skill_name)
    
    def check_prerequisites(self, skill_name):
        """Check if prerequisites for a skill are met."""
        # Implementation would check skill tree prerequisites
        return True  # Placeholder
    
    def unlock_advanced_skill(self, skill_name):
        """Unlock advanced skill when prerequisites are met."""
        # Implementation would unlock advanced skills
        pass
    
    def adjust_learning_phases(self, features):
        """Adjust learning phases based on experience."""
        
        # Update sensory learning phase
        if features['novelty'] > 0.3:
            self.learning_phases['sensory_learning']['progress'] += 0.1
        
        # Update motor learning phase if successful actions
        if features['outcome'] > 0:
            self.learning_phases['motor_learning']['progress'] += 0.05
        
        # Activate new phases if progress is sufficient
        if (self.learning_phases['sensory_learning']['progress'] > 0.8 and 
            not self.learning_phases['social_learning']['active']):
            self.learning_phases['social_learning']['active'] = True
        
        if (self.learning_phases['motor_learning']['progress'] > 0.6 and 
            not self.learning_phases['abstract_learning']['active']):
            self.learning_phases['abstract_learning']['active'] = True
    
    def run_lifelong_learning_simulation(self, steps=2000):
        """Run lifelong learning simulation."""
        
        print("Starting lifelong learning simulation...")
        print("Learning Phases:", self.learning_phases)
        
        self.world.reset()
        self.agent.initialize(self.world)
        
        learning_sessions = []
        
        for phase_name, phase_data in self.learning_phases.items():
            if phase_data['active']:
                print(f"\n--- Learning Phase: {phase_name} ---")
                phase_steps = self.get_phase_steps(phase_name)
                
                for step in range(phase_steps):
                    # Get sensory input
                    percept = self.world.getSensoryPercept()
                    
                    # Process sensory input
                    self.agent.processSensoryInput(percept)
                    
                    # Run brain step
                    self.brain.step(step)
                    
                    # Get action
                    action = self.agent.decodeMotorCommand()
                    
                    # Apply action
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                    
                    # Apply reward modulation
                    reward = self.calculate_phase_reward(phase_name, percept, action)
                    self.agent.applyRewardModulation(reward, 0.0)
                    
                    # Update development
                    self.agent.updateDevelopment(0.1)
                    
                    # Learn from experience
                    experience = {
                        'percept': percept,
                        'action': action,
                        'reward': reward,
                        'context': self.get_context(),
                        'novelty': self.agent.getNoveltyLevel(),
                        'difficulty': self.calculate_difficulty(percept, action),
                        'time': step
                    }
                    
                    self.learn_from_experience(experience)
                    
                    # Record session data
                    learning_sessions.append({
                        'phase': phase_name,
                        'step': step,
                        'experience': experience,
                        'knowledge_base_size': len(self.knowledge_base['facts']),
                        'skills_acquired': self.get_acquired_skills()
                    })
                    
                    if step % 200 == 0:
                        print(f"Phase {phase_name}: Step {step}/{phase_steps}")
                        print(f"  Knowledge facts: {len(self.knowledge_base['facts'])}")
                        print(f"  Skills: {self.get_acquired_skills()}")
                        print(f"  Development stage: {self.brain.getDevelopmentalStage()}")
        
        print("\nLifelong learning simulation completed!")
        return self.summarize_learning_sessions(learning_sessions)
    
    def get_phase_steps(self, phase_name):
        """Get number of steps for a learning phase."""
        phase_steps = {
            'sensory_learning': 500,
            'motor_learning': 400,
            'social_learning': 300,
            'abstract_learning': 200
        }
        return phase_steps.get(phase_name, 100)
    
    def calculate_phase_reward(self, phase_name, percept, action):
        """Calculate reward based on learning phase."""
        
        base_reward = 1.0
        
        if phase_name == 'sensory_learning':
            # Reward novel sensory input
            novelty = self.agent.getNoveltyLevel()
            return base_reward * (1.0 + novelty)
        
        elif phase_name == 'motor_learning':
            # Reward successful actions
            return base_reward
        
        elif phase_name == 'social_learning':
            # Reward social interaction success
            return base_reward * 1.5
        
        elif phase_name == 'abstract_learning':
            # Reward abstract thinking
            return base_reward * 2.0
        
        return base_reward
    
    def get_context(self):
        """Get current context for learning."""
        return {
            'world_state': self.world.getSimulationTime(),
            'brain_state': self.brain.getDevelopmentalStage(),
            'agent_state': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel()
        }
    
    def calculate_difficulty(self, percept, action):
        """Calculate difficulty of current task."""
        # Implementation would calculate task difficulty
        return 0.5  # Placeholder
    
    def get_acquired_skills(self):
        """Get list of acquired skills."""
        acquired = []
        for skill_name, skill_data in self.skill_tree.items():
            if skill_data['level'] > 0:
                acquired.append(skill_name)
        return acquired
    
    def summarize_learning_sessions(self, sessions):
        """Summarize learning sessions."""
        
        summary = {
            'total_sessions': len(sessions),
            'knowledge_facts': len(self.knowledge_base['facts']),
            'skills_acquired': self.get_acquired_skills(),
            'learning_phases': self.learning_phases,
            'final_development_stage': self.brain.getDevelopmentalStage(),
            'total_learning_time': sum(session['step'] for session in sessions)
        }
        
        print("\n=== Learning Session Summary ===")
        print(f"Total learning sessions: {summary['total_sessions']}")
        print(f"Knowledge facts acquired: {summary['knowledge_facts']}")
        print(f"Skills acquired: {summary['skills_acquired']}")
        print(f"Final development stage: {summary['final_development_stage']}")
        
        return summary

# Usage example
if __name__ == "__main__":
    # Create lifelong learning configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    config.set("enable_memory_consolidation", True)
    
    # Create lifelong learning agent
    agent = LifelongLearningAgent(config)
    
    # Run simulation
    results = agent.run_lifelong_learning_simulation(1000)
```

## 3. Performance and Optimization

### Example 3: Performance-Optimized Simulation
```python
import pynlm
import time
import threading
from typing import Dict, List
import psutil
import os

class PerformanceOptimizedSimulator:
    """Performance-optimized simulator for maximum efficiency."""
    
    def __init__(self, config, optimization_settings=None):
        self.config = config
        self.optimization_settings = optimization_settings or self.get_default_optimization_settings()
        
        # Monitor system resources
        self.system_monitor = SystemMonitor()
        self.performance_tracker = PerformanceTracker()
        
        # Create optimized components
        self.brain = self.create_optimized_brain()
        self.agent = self.create_optimized_agent()
        self.world = self.create_optimized_world()
        
        self.initialize_performance_systems()
    
    def get_default_optimization_settings(self):
        """Get default optimization settings."""
        return {
            'memory_optimization': True,
            'parallel_processing': True,
            'memory_pooling': True,
            'event_driven_updates': True,
            'adaptive_timestep': True,
            'resource_monitoring': True,
            'auto_scaling': True
        }
    
    def create_optimized_brain(self):
        """Create performance-optimized brain."""
        
        # Create brain with optimized settings
        brain = pynlm.createBrain(self.config)
        
        # Apply optimization settings
        if self.optimization_settings['memory_optimization']:
            brain.enableMemoryPooling(True)
            brain.setMemoryPoolSize('small')
        
        if self.optimization_settings['auto_scaling']:
            brain.enableAutoScaling(True)
            brain.setScalingStrategy('adaptive')
        
        # Initialize brain
        brain.initialize()
        
        return brain
    
    def create_optimized_agent(self):
        """Create performance-optimized agent."""
        
        agent = pynlm.createAgentBrain(self.brain)
        
        # Enable optimized subsystems
        agent.enableRewardModulation(self.optimization_settings.get('enable_reward', True))
        agent.enableCuriosity(self.optimization_settings.get('enable_curiosity', True))
        agent.enableStructuralPlasticity(self.optimization_settings.get('enable_structural', True))
        agent.enableDevelopment(self.optimization_settings.get('enable_development', True))
        
        # Configure for performance
        agent.setProcessingStrategy('optimized')
        agent.enableAsyncProcessing(True)
        
        return agent
    
    def create_optimized_world(self):
        """Create performance-optimized world."""
        
        world = pynlm.createSimpleWorld()
        
        # Configure for performance
        world.setUpdateStrategy('optimized')
        world.enableSpatialHashing(True)
        world.enableQuadTree(True)
        
        return world
    
    def initialize_performance_systems(self):
        """Initialize performance monitoring and optimization systems."""
        
        # Initialize system monitor
        if self.optimization_settings['resource_monitoring']:
            self.system_monitor.start_monitoring()
        
        # Initialize performance tracker
        self.performance_tracker.reset()
        
        # Start performance optimizer
        self.performance_optimizer = PerformanceOptimizer(self.optimization_settings)
        self.performance_optimizer.start_optimization()
    
    def run_performance_optimized_simulation(self, steps=5000):
        """Run performance-optimized simulation."""
        
        print("Starting performance-optimized simulation...")
        print(f"Optimization settings: {self.optimization_settings}")
        
        # Initialize systems
        self.world.configure(
            width=20, height=20,
            visionWidth=8, visionHeight=8
        )
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Performance tracking
        start_time = time.time()
        step_times = []
        memory_usage = []
        cpu_usage = []
        
        simulation_data = {
            'steps': list(range(steps)),
            'performance_metrics': {},
            'system_resources': {},
            'optimization_results': {}
        }
        
        for step in range(steps):
            step_start = time.time()
            
            # Monitor system resources
            if self.optimization_settings['resource_monitoring']:
                memory_usage.append(self.system_monitor.get_memory_usage())
                cpu_usage.append(self.system_monitor.get_cpu_usage())
            
            # Adaptive timestep based on performance
            if self.optimization_settings['adaptive_timestep']:
                timestep = self.performance_optimizer.get_optimal_timestep()
                self.world.update(timestep)
            else:
                self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input with async processing
            if self.optimization_settings.get('async_processing', True):
                future = self.agent.processSensoryInputAsync(percept)
                future.wait()  # Wait for completion
            else:
                self.agent.processSensoryInput(percept)
            
            # Run brain step with optimized processing
            brain_step_start = time.time()
            self.brain.step(step)
            brain_step_time = time.time() - brain_step_start
            
            # Get motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Measure step time
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            # Record performance metrics
            self.performance_tracker.record_step(
                step_time, brain_step_time, 
                self.brain.getAverageFiringRate(),
                self.agent.getCuriosityLevel()
            )
            
            # Record simulation data
            simulation_data['performance_metrics'][step] = {
                'step_time': step_time,
                'brain_step_time': brain_step_time,
                'firing_rate': self.brain.getAverageFiringRate(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'memory_usage': memory_usage[-1] if memory_usage else 0,
                'cpu_usage': cpu_usage[-1] if cpu_usage else 0
            }
            
            # Performance optimization every 100 steps
            if step % 100 == 0:
                self.performance_optimizer.optimize_performance(simulation_data)
                
                # Log performance
                avg_step_time = np.mean(step_times[-100:])
                current_fps = 1.0 / avg_step_time if avg_step_time > 0 else 0
                
                print(f"Step {step}/{steps}: "
                      f"Avg Step Time: {avg_step_time*1000:.1f}ms, "
                      f"FPS: {current_fps:.1f}, "
                      f"Memory: {memory_usage[-1]:.1f}%")
                
                # Auto-scale if needed
                if self.optimization_settings['auto_scaling']:
                    self.auto_scale_resources()
        
        total_time = time.time() - start_time
        
        # Finalize performance analysis
        self.performance_tracker.finalize()
        optimization_results = self.performance_optimizer.get_results()
        
        # Record system resources
        simulation_data['system_resources'] = {
            'peak_memory': max(memory_usage) if memory_usage else 0,
            'avg_cpu': np.mean(cpu_usage) if cpu_usage else 0,
            'peak_cpu': max(cpu_usage) if cpu_usage else 0
        }
        
        simulation_data['optimization_results'] = optimization_results
        
        print(f"\nPerformance simulation completed!")
        print(f"Total time: {total_time:.1f}s")
        print(f"Average step time: {np.mean(step_times)*1000:.1f}ms")
        print(f"Target FPS achieved: {1.0/(np.mean(step_times) if np.mean(step_times) > 0 else 1):.1f}")
        
        return simulation_data
    
    def auto_scale_resources(self):
        """Automatically scale resources based on performance."""
        
        current_performance = self.performance_tracker.get_current_performance()
        
        # Scale up if performance is below threshold
        if current_performance['step_time'] > self.optimization_settings['performance_threshold']:
            self.performance_optimizer.scale_up()
            print("Performance scaling up...")
        
        # Scale down if performance is above threshold and resources are available
        elif (current_performance['step_time'] < self.optimization_settings['excellent_performance_threshold'] and
              self.system_monitor.get_free_memory() > self.optimization_settings['min_free_memory']):
            self.performance_optimizer.scale_down()
            print("Performance scaling down...")
    
    def __del__(self):
        """Clean up resources."""
        if hasattr(self, 'performance_optimizer'):
            self.performance_optimizer.stop_optimization()
        if hasattr(self, 'system_monitor'):
            self.system_monitor.stop_monitoring()

class SystemMonitor:
    """Monitor system resources during simulation."""
    
    def __init__(self):
        self.monitoring = False
        self.memory_samples = []
        self.cpu_samples = []
    
    def start_monitoring(self):
        """Start system resource monitoring."""
        self.monitoring = True
        threading.Thread(target=self._monitor_resources, daemon=True).start()
    
    def _monitor_resources(self):
        """Monitor system resources in background thread."""
        while self.monitoring:
            try:
                process = psutil.Process(os.getpid())
                self.memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
                self.cpu_samples.append(process.cpu_percent() / psutil.cpu_count())
                time.sleep(0.1)
            except Exception:
                time.sleep(1)
    
    def get_memory_usage(self):
        """Get current memory usage in MB."""
        return self.memory_samples[-1] if self.memory_samples else 0
    
    def get_cpu_usage(self):
        """Get current CPU usage percentage."""
        return self.cpu_samples[-1] if self.cpu_samples else 0
    
    def get_free_memory(self):
        """Get available memory in MB."""
        return psutil.virtual_memory().available / 1024 / 1024
    
    def stop_monitoring(self):
        """Stop system resource monitoring."""
        self.monitoring = False

class PerformanceTracker:
    """Track performance metrics during simulation."""
    
    def __init__(self):
        self.step_times = []
        self.brain_step_times = []
        self.firing_rates = []
        self.curiosity_levels = []
        self.total_steps = 0
    
    def record_step(self, step_time, brain_step_time, firing_rate, curiosity_level):
        """Record performance metrics for a step."""
        self.step_times.append(step_time)
        self.brain_step_times.append(brain_step_time)
        self.firing_rates.append(firing_rate)
        self.curiosity_levels.append(curiosity_level)
        self.total_steps += 1
    
    def get_current_performance(self):
        """Get current performance metrics."""
        if not self.step_times:
            return {'step_time': 0, 'brain_step_time': 0}
        
        return {
            'step_time': np.mean(self.step_times[-10:]) if len(self.step_times) >= 10 else np.mean(self.step_times),
            'brain_step_time': np.mean(self.brain_step_times[-10:]) if len(self.brain_step_times) >= 10 else np.mean(self.brain_step_times)
        }
    
    def get_performance_summary(self):
        """Get performance summary statistics."""
        if not self.step_times:
            return {}
        
        return {
            'avg_step_time': np.mean(self.step_times),
            'min_step_time': np.min(self.step_times),
            'max_step_time': np.max(self.step_times),
            'std_step_time': np.std(self.step_times),
            'avg_brain_step_time': np.mean(self.brain_step_times),
            'avg_firing_rate': np.mean(self.firing_rates),
            'avg_curiosity_level': np.mean(self.curiosity_levels),
            'total_steps': self.total_steps
        }
    
    def reset(self):
        """Reset performance tracking."""
        self.step_times = []
        self.brain_step_times = []
        self.firing_rates = []
        self.curiosity_levels = []
        self.total_steps = 0
    
    def finalize(self):
        """Finalize performance tracking."""
        pass  # Clean up if needed

class PerformanceOptimizer:
    """Optimize simulation performance in real-time."""
    
    def __init__(self, settings):
        self.settings = settings
        self.optimization_history = []
        self.current_performance = {}
    
    def get_optimal_timestep(self):
        """Get optimal timestep based on current performance."""
        if not self.current_performance:
            return 0.1
        
        # Adaptive timestep based on performance
        step_time = self.current_performance.get('step_time', 0.1)
        
        if step_time < 0.001:  # Very fast - can increase timestep
            return 0.05
        elif step_time < 0.005:  # Fast - moderate timestep
            return 0.02
        elif step_time < 0.01:  # Normal - base timestep
            return 0.1
        else:  # Slow - reduce timestep
            return 0.01
    
    def optimize_performance(self, simulation_data):
        """Optimize performance based on simulation data."""
        
        # Analyze current performance
        current_perf = self.analyze_current_performance(simulation_data)
        self.current_performance = current_perf
        
        # Apply optimizations
        optimizations = []
        
        if current_perf['cpu_usage'] > self.settings['cpu_threshold']:
            optimizations.append('reduce_concurrency')
        
        if current_perf['memory_usage'] > self.settings['memory_threshold']:
            optimizations.append('enable_memory_pooling')
        
        if current_perf['step_time'] > self.settings['performance_threshold']:
            optimizations.append('enable_optimization')
        
        self.optimization_history.append({
            'timestamp': time.time(),
            'performance': current_perf,
            'optimizations': optimizations
        })
        
        # Apply optimizations
        for optimization in optimizations:
            self.apply_optimization(optimization)
    
    def analyze_current_performance(self, simulation_data):
        """Analyze current performance from simulation data."""
        
        if not simulation_data['performance_metrics']:
            return {
                'step_time': 0.1,
                'cpu_usage': 0,
                'memory_usage': 0,
                'firing_rate': 0
            }
        
        # Get last 100 steps for analysis
        last_steps = list(simulation_data['performance_metrics'].items())[-100:]
        
        if not last_steps:
            return {
                'step_time': 0.1,
                'cpu_usage': 0,
                'memory_usage': 0,
                'firing_rate': 0
            }
        
        avg_step_time = np.mean([step[1]['step_time'] for step in last_steps])
        avg_cpu = np.mean([step[1]['cpu_usage'] for step in last_steps])
        avg_memory = np.mean([step[1]['memory_usage'] for step in last_steps])
        avg_firing_rate = np.mean([step[1]['firing_rate'] for step in last_steps])
        
        return {
            'step_time': avg_step_time,
            'cpu_usage': avg_cpu,
            'memory_usage': avg_memory,
            'firing_rate': avg_firing_rate
        }
    
    def apply_optimization(self, optimization):
        """Apply specific optimization."""
        # Implementation would apply specific optimization
        # For now, just log it
        print(f"Applying optimization: {optimization}")
    
    def scale_up(self):
        """Scale up resources."""
        print("Scaling up resources...")
        # Implementation would scale up resources
    
    def scale_down(self):
        """Scale down resources."""
        print("Scaling down resources...")
        # Implementation would scale down resources
    
    def get_results(self):
        """Get optimization results."""
        return {
            'optimizations_applied': len(self.optimization_history),
            'performance_improvements': len([opt for opt in self.optimization_history if opt['optimizations']]),
            'current_performance': self.current_performance,
            'optimization_history': self.optimization_history
        }
    
    def start_optimization(self):
        """Start performance optimization."""
        print("Starting performance optimization...")
    
    def stop_optimization(self):
        """Stop performance optimization."""
        print("Stopping performance optimization...")

# Usage example
if __name__ == "__main__":
    # Create performance-optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("enable_structural_plasticity", False)  # Disable for performance
    config.set("enable_development", False)  # Disable for performance
    config.set("enable_curiosity", False)  # Disable for performance
    
    # Create optimization settings
    optimization_settings = {
        'memory_optimization': True,
        'parallel_processing': True,
        'memory_pooling': True,
        'event_driven_updates': True,
        'adaptive_timestep': True,
        'resource_monitoring': True,
        'auto_scaling': True,
        'enable_reward': False,
        'enable_curiosity': False,
        'enable_structural': False,
        'enable_development': False,
        'performance_threshold': 0.01,
        'excellent_performance_threshold': 0.005,
        'cpu_threshold': 80,
        'memory_threshold': 80,
        'min_free_memory': 100
    }
    
    # Create performance-optimized simulator
    simulator = PerformanceOptimizedSimulator(config, optimization_settings)
    
    # Run simulation
    results = simulator.run_performance_optimized_simulation(2000)
```

## 4. Integration and Deployment

### Example 4: Production Deployment
```python
import pynlm
import json
import time
import threading
from typing import Dict, List, Callable
import logging
from pathlib import Path

class ProductionNLMDeployment:
    """Production deployment of NLM system."""
    
    def __init__(self, config, deployment_config):
        self.config = config
        self.deployment_config = deployment_config
        
        # Setup logging
        self.setup_logging()
        
        # Initialize components
        self.brain = None
        self.agent = None
        self.world = None
        
        # Deployment state
        self.deployment_state = 'initializing'
        self.active_simulations = {}
        self.deployment_metrics = {}
        
        # Initialize production system
        self.initialize_production_system()
    
    def setup_logging(self):
        """Setup production logging."""
        
        log_level = self.deployment_config.get('log_level', 'INFO')
        log_file = self.deployment_config.get('log_file', 'nlm_production.log')
        
        # Create logger
        self.logger = logging.getLogger('nlm_production')
        self.logger.setLevel(getattr(logging, log_level))
        
        # File handler
        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(getattr(logging, log_level))
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(getattr(logging, log_level))
        
        # Formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        
        file_handler.setFormatter(formatter)
        console_handler.setFormatter(formatter)
        
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        # Suppress other loggers
        logging.getLogger('asyncio').setLevel(logging.WARNING)
        logging.getLogger('numpy').setLevel(logging.WARNING)
    
    def initialize_production_system(self):
        """Initialize production NLM system."""
        
        self.logger.info("Initializing production NLM system...")
        
        try:
            # Initialize brain
            self.brain = pynlm.createBrain(self.config)
            self.logger.info("Brain initialized")
            
            # Initialize agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.logger.info("Agent brain initialized")
            
            # Initialize world
            self.world = pynlm.createSimpleWorld()
            self.logger.info("World initialized")
            
            # Configure world for production
            self.configure_world_for_production()
            
            # Initialize agent with world
            self.agent.initialize(self.world)
            self.logger.info("Agent initialized with world")
            
            # Enable production subsystems
            self.enable_production_subsystems()
            
            # Load checkpoint if available
            self.load_checkpoint_if_available()
            
            self.deployment_state = 'ready'
            self.logger.info("Production NLM system initialized successfully")
            
        except Exception as e:
            self.deployment_state = 'error'
            self.logger.error(f"Failed to initialize production system: {e}")
            raise
    
    def configure_world_for_production(self):
        """Configure world for production environment."""
        
        world_config = self.deployment_config.get('world_config', {})
        
        self.world.configure(
            width=world_config.get('width', 20),
            height=world_config.get('height', 20),
            visionWidth=world_config.get('vision_width', 8),
            visionHeight=world_config.get('vision_height', 8)
        )
        
        # Set agent start position
        if 'agent_start' in world_config:
            x, y = world_config['agent_start']
            self.world.setAgentStart(x, y)
        
        # Configure world update rate
        self.world.setUpdateRate(self.deployment_config.get('world_update_rate', 10.0))
    
    def enable_production_subsystems(self):
        """Enable subsystems appropriate for production."""
        
        production_config = self.deployment_config.get('production_config', {})
        
        # Enable reward modulation if configured
        if production_config.get('enable_reward_modulation', True):
            self.agent.enableRewardModulation(True)
            self.logger.info("Reward modulation enabled")
        
        # Enable structural plasticity if configured
        if production_config.get('enable_structural_plasticity', False):
            self.agent.enableStructuralPlasticity(True)
            self.logger.info("Structural plasticity enabled")
        
        # Enable development if configured
        if production_config.get('enable_development', False):
            self.agent.enableDevelopment(True)
            self.logger.info("Development enabled")
        
        # Enable curiosity if configured
        if production_config.get('enable_curiosity', False):
            self.agent.enableCuriosity(True)
            self.logger.info("Curiosity enabled")
        
        # Enable checkpointing if configured
        if production_config.get('enable_checkpointing', True):
            self.enable_checkpointing()
            self.logger.info("Checkpointing enabled")
        
        # Enable replay system if configured
        if production_config.get('enable_replay', False):
            self.enable_replay_system()
            self.logger.info("Replay system enabled")
    
    def enable_checkpointing(self):
        """Enable production checkpointing."""
        
        checkpoint_config = self.deployment_config.get('checkpointing', {})
        
        # Create checkpoint directory
        checkpoint_dir = Path(checkpoint_config.get('checkpoint_dir', './checkpoints'))
        checkpoint_dir.mkdir(exist_ok=True)
        
        # Set up checkpointing
        self.checkpoint_interval = checkpoint_config.get('checkpoint_interval', 100.0)
        self.last_checkpoint_time = time.time()
        
        # Load latest checkpoint if available
        latest_checkpoint = self.find_latest_checkpoint(checkpoint_dir)
        if latest_checkpoint:
            self.load_checkpoint(latest_checkpoint)
    
    def enable_replay_system(self):
        """Enable production replay system."""
        
        replay_config = self.deployment_config.get('replay', {})
        
        # Initialize replay buffer
        self.replay_buffer = ReplayBuffer(
            max_size=replay_config.get('max_buffer_size', 1000),
            save_frequency=replay_config.get('save_frequency', 10)
        )
        
        # Start replay recording thread
        self.replay_thread = threading.Thread(
            target=self._record_replay_data,
            daemon=True
        )
        self.replay_thread.start()
    
    def _record_replay_data(self):
        """Record replay data in background thread."""
        
        while self.deployment_state == 'running':
            try:
                # Record current state
                state = self.get_current_state()
                self.replay_buffer.record_state(state)
                
                time.sleep(0.1)  # Record every 0.1 seconds
                
            except Exception as e:
                self.logger.error(f"Error recording replay data: {e}")
                time.sleep(1)
    
    def get_current_state(self):
        """Get current simulation state for replay."""
        
        return {
            'timestamp': time.time(),
            'world_state': self.get_world_state(),
            'brain_state': self.get_brain_state(),
            'agent_state': self.get_agent_state(),
            'simulation_time': self.world.getSimulationTime() if self.world else 0
        }
    
    def get_world_state(self):
        """Get current world state."""
        
        if not self.world:
            return {}
        
        return {
            'position': self.get_agent_position(),
            'vision': self.get_agent_vision(),
            'world_time': self.world.getSimulationTime()
        }
    
    def get_brain_state(self):
        """Get current brain state."""
        
        if not self.brain:
            return {}
        
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'neuron_count': self.brain.getTotalNeuronCount()
        }
    
    def get_agent_state(self):
        """Get current agent state."""
        
        if not self.agent:
            return {}
        
        return {
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'reward_history': self.get_reward_history(),
            'action_history': self.get_action_history()
        }
    
    def get_agent_position(self):
        """Get current agent position."""
        
        if not self.world:
            return {'x': 0, 'y': 0}
        
        # Implementation would get actual agent position
        return {'x': 10, 'y': 10}  # Placeholder
    
    def get_agent_vision(self):
        """Get current agent vision data."""
        
        if not self.world:
            return {}
        
        return {
            'visible_objects': self.world.getVisibleObjects(),
            'obstacles': self.world.getObstacles(),
            'resources': self.world.getResources()
        }
    
    def get_reward_history(self):
        """Get reward history for analysis."""
        
        if not hasattr(self, 'reward_history'):
            self.reward_history = []
        
        return self.reward_history[-100:]  # Last 100 rewards
    
    def get_action_history(self):
        """Get action history for analysis."""
        
        if not hasattr(self, 'action_history'):
            self.action_history = []
        
        return self.action_history[-100:]  # Last 100 actions
    
    def create_simulation(self, simulation_id, config_overrides=None):
        """Create new simulation with optional config overrides."""
        
        if self.deployment_state != 'ready':
            raise Exception(f"Deployment state is {self.deployment_state}, not 'ready'")
        
        # Create configuration
        simulation_config = self.config
        
        # Apply config overrides if provided
        if config_overrides:
            for key, value in config_overrides.items():
                simulation_config.set(key, value)
        
        # Create simulation
        simulation = {
            'id': simulation_id,
            'config': simulation_config,
            'state': 'initialized',
            'start_time': None,
            'end_time': None,
            'metrics': {},
            'thread': None
        }
        
        self.active_simulations[simulation_id] = simulation
        
        self.logger.info(f"Created simulation: {simulation_id}")
        
        return simulation
    
    def run_simulation(self, simulation_id, steps=1000):
        """Run simulation."""
        
        if simulation_id not in self.active_simulations:
            raise Exception(f"Simulation {simulation_id} not found")
        
        simulation = self.active_simulations[simulation_id]
        
        if simulation['state'] != 'initialized':
            raise Exception(f"Simulation {simulation_id} is not in initialized state")
        
        # Update simulation state
        simulation['state'] = 'running'
        simulation['start_time'] = time.time()
        
        try:
            # Run simulation
            self._run_single_simulation(simulation_id, steps)
            
            # Update simulation state
            simulation['state'] = 'completed'
            simulation['end_time'] = time.time()
            
            self.logger.info(f"Simulation {simulation_id} completed successfully")
            
        except Exception as e:
            simulation['state'] = 'error'
            simulation['error'] = str(e)
            
            self.logger.error(f"Simulation {simulation_id} failed: {e}")
            
            # Save error state
            self.save_simulation_state(simulation_id)
            
            raise
        
        return simulation
    
    def _run_single_simulation(self, simulation_id, steps):
        """Run single simulation."""
        
        simulation = self.active_simulations[simulation_id]
        
        # Reset brain and world
        if self.brain:
            self.brain.reset()
            self.brain.initialize()
        
        if self.world:
            self.world.reset()
        
        if self.agent:
            self.agent.initialize(self.world)
        
        # Run simulation loop
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record metrics
            self.record_simulation_metrics(simulation_id, step, reward)
            
            # Save checkpoint if needed
            self.save_checkpoint_if_needed()
            
            # Log progress
            if step % 100 == 0:
                self.logger.info(
                    f"Simulation {simulation_id}: "
                    f"Step {step}/{steps}, "
                    f"Firing Rate: {self.brain.getAverageFiringRate():.3f}"
                )
        
        # Save final state
        self.save_simulation_state(simulation_id)
    
    def record_simulation_metrics(self, simulation_id, step, reward):
        """Record simulation metrics."""
        
        simulation = self.active_simulations[simulation_id]
        
        if 'metrics' not in simulation:
            simulation['metrics'] = {}
        
        if 'steps' not in simulation['metrics']:
            simulation['metrics']['steps'] = []
        
        if 'rewards' not in simulation['metrics']:
            simulation['metrics']['rewards'] = []
        
        simulation['metrics']['steps'].append(step)
        simulation['metrics']['rewards'].append(reward)
        
        # Update deployment metrics
        if 'simulation_metrics' not in self.deployment_metrics:
            self.deployment_metrics['simulation_metrics'] = {}
        
        if simulation_id not in self.deployment_metrics['simulation_metrics']:
            self.deployment_metrics['simulation_metrics'][simulation_id] = {
                'total_steps': 0,
                'total_rewards': 0,
                'avg_firing_rate': 0,
                'simulation_count': 0
            }
        
        sim_metrics = self.deployment_metrics['simulation_metrics'][simulation_id]
        sim_metrics['total_steps'] += 1
        sim_metrics['total_rewards'] += reward
        sim_metrics['simulation_count'] += 1
    
    def save_checkpoint_if_needed(self):
        """Save checkpoint if interval has passed."""
        
        if hasattr(self, 'checkpoint_interval') and self.brain:
            current_time = time.time()
            if current_time - self.last_checkpoint_time > self.checkpoint_interval:
                self.save_checkpoint()
                self.last_checkpoint_time = current_time
    
    def save_checkpoint(self):
        """Save current simulation checkpoint."""
        
        if not self.brain:
            return
        
        checkpoint_config = self.deployment_config.get('checkpointing', {})
        checkpoint_dir = Path(checkpoint_config.get('checkpoint_dir', './checkpoints'))
        
        checkpoint_file = checkpoint_dir / f"checkpoint_{int(time.time())}.bin"
        
        try:
            self.brain.save(str(checkpoint_file))
            self.logger.info(f"Checkpoint saved: {checkpoint_file}")
            
        except Exception as e:
            self.logger.error(f"Failed to save checkpoint: {e}")
    
    def load_checkpoint_if_available(self):
        """Load latest checkpoint if available."""
        
        if not self.brain:
            return
        
        checkpoint_config = self.deployment_config.get('checkpointing', {})
        checkpoint_dir = Path(checkpoint_config.get('checkpoint_dir', './checkpoints'))
        
        latest_checkpoint = self.find_latest_checkpoint(checkpoint_dir)
        
        if latest_checkpoint:
            try:
                self.load_checkpoint(latest_checkpoint)
                self.logger.info(f"Loaded checkpoint: {latest_checkpoint}")
            except Exception as e:
                self.logger.error(f"Failed to load checkpoint: {e}")
    
    def find_latest_checkpoint(self, checkpoint_dir):
        """Find latest checkpoint file."""
        
        checkpoint_files = list(checkpoint_dir.glob('*.bin'))
        
        if not checkpoint_files:
            return None
        
        return max(checkpoint_files, key=lambda f: f.stat().st_mtime)
    
    def load_checkpoint(self, checkpoint_file):
        """Load checkpoint from file."""
        
        if self.brain:
            self.brain.load(str(checkpoint_file))
    
    def save_simulation_state(self, simulation_id):
        """Save simulation state to disk."""
        
        simulation = self.active_simulations[simulation_id]
        
        if 'metrics' in simulation:
            state_file = Path(f'./simulation_states/{simulation_id}_state.json')
            state_file.parent.mkdir(exist_ok=True)
            
            with open(state_file, 'w') as f:
                json.dump(simulation['metrics'], f, indent=2)
    
    def get_deployment_metrics(self):
        """Get deployment metrics."""
        
        return {
            'deployment_state': self.deployment_state,
            'active_simulations': len(self.active_simulations),
            'total_simulations_run': len(self.deployment_metrics.get('simulation_metrics', {})),
            'system_resources': self.get_system_resources(),
            'simulation_metrics': self.deployment_metrics.get('simulation_metrics', {})
        }
    
    def get_system_resources(self):
        """Get current system resource usage."""
        
        try:
            import psutil
            
            process = psutil.Process()
            
            return {
                'memory_usage': process.memory_info().rss / 1024 / 1024,  # MB
                'cpu_usage': process.cpu_percent(),
                'memory_percent': process.memory_percent()
            }
            
        except ImportError:
            return {
                'memory_usage': 0,
                'cpu_usage': 0,
                'memory_percent': 0
            }
    
    def stop_simulation(self, simulation_id):
        """Stop running simulation."""
        
        if simulation_id in self.active_simulations:
            simulation = self.active_simulations[simulation_id]
            
            if simulation['state'] == 'running':
                simulation['state'] = 'stopped'
                
                if 'thread' in simulation and simulation['thread']:
                    simulation['thread'].join(timeout=10)
                
                self.logger.info(f"Simulation {simulation_id} stopped")
    
    def cleanup(self):
        """Clean up production system."""
        
        self.logger.info("Cleaning up production system...")
        
        # Stop all simulations
        for simulation_id in list(self.active_simulations.keys()):
            self.stop_simulation(simulation_id)
        
        # Save final metrics
        self.save_final_metrics()
        
        # Stop monitoring
        self.stop_monitoring()
        
        # Shutdown
        self.deployment_state = 'shutdown'
        
        self.logger.info("Production NLM system cleaned up")
    
    def save_final_metrics(self):
        """Save final deployment metrics."""
        
        metrics_file = Path('./deployment_metrics.json')
        
        with open(metrics_file, 'w') as f:
            json.dump(self.deployment_metrics, f, indent=2)
    
    def stop_monitoring(self):
        """Stop monitoring systems."""
        
        # Stop replay recording
        if hasattr(self, 'replay_thread') and self.replay_thread:
            # Implementation would stop replay thread
            pass

class ReplayBuffer:
    """Replay buffer for recording simulation data."""
    
    def __init__(self, max_size=1000, save_frequency=10):
        self.max_size = max_size
        self.save_frequency = save_frequency
        self.buffer = []
        self.save_count = 0
    
    def record_state(self, state):
        """Record state to replay buffer."""
        
        self.buffer.append(state)
        
        # Maintain buffer size
        if len(self.buffer) > self.max_size:
            self.buffer.pop(0)
        
        # Save to disk periodically
        self.save_count += 1
        if self.save_count % self.save_frequency == 0:
            self.save_to_disk()
    
    def save_to_disk(self):
        """Save replay buffer to disk."""
        
        import json
        from pathlib import Path
        
        buffer_file = Path(f'./replay_buffers/replay_{int(time.time())}.json')
        buffer_file.parent.mkdir(exist_ok=True)
        
        with open(buffer_file, 'w') as f:
            json.dump(self.buffer, f, indent=2)

# Usage example
if __name__ == "__main__":
    # Create deployment configuration
    deployment_config = {
        'log_level': 'INFO',
        'log_file': './nlm_production.log',
        'world_config': {
            'width': 20,
            'height': 20,
            'vision_width': 8,
            'vision_height': 8,
            'agent_start': (10, 10)
        },
        'world_update_rate': 10.0,
        'production_config': {
            'enable_reward_modulation': True,
            'enable_structural_plasticity': False,
            'enable_development': False,
            'enable_curiosity': False,
            'enable_checkpointing': True,
            'enable_replay': False
        },
        'checkpointing': {
            'checkpoint_dir': './checkpoints',
            'checkpoint_interval': 100.0
        },
        'replay': {
            'max_buffer_size': 1000,
            'save_frequency': 10
        }
    }
    
    # Create production deployment
    deployment = ProductionNLMDeployment(
        config=pynlm.createDefaultConfig(),
        deployment_config=deployment_config
    )
    
    try:
        # Create simulation
        simulation = deployment.create_simulation(
            'memory_learning_001',
            {'brain.neuron_count': 2000}
        )
        
        # Run simulation
        result = deployment.run_simulation('memory_learning_001', 500)
        
        # Get deployment metrics
        metrics = deployment.get_deployment_metrics()
        
        print("Deployment metrics:", json.dumps(metrics, indent=2))
        
    finally:
        # Clean up
        deployment.cleanup()
```

## 5. Custom NLM Extensions

### Example 5: Custom Neuron and Synapse Types
```python
import pynlm
import numpy as np
from typing import Dict, Any

class CustomNeuron(pynlm.Neuron):
    """Custom neuron with advanced features."""
    
    def __init__(self, neuron_type='custom'):
        super().__init__()
        
        # Custom neuron properties
        self.neuron_type = neuron_type
        self.synapse_strength = 1.0
        self.plasticity_factor = 0.5
        self.energy_level = 1.0
        self.custom_state = {}
        
        # Custom dynamics parameters
        self.threshold = -50.0
        self.reset_potential = -70.0
        self.time_constant = 20.0
        
        # Custom plasticity rules
        self.stdp_enabled = True
        self.hebbian_enabled = True
        self.metaplasticity_enabled = False
    
    def update(self, dt):
        """Update neuron state with custom dynamics."""
        
        # Custom integration
        self.integrate_input(dt)
        
        # Custom threshold behavior
        if self.getMembranePotential() >= self.threshold:
            self.spike()
        
        # Custom decay
        self.decay_membrane_potential(dt)
        
        # Custom energy management
        self.update_energy_level(dt)
        
        # Custom plasticity
        self.apply_custom_plasticity(dt)
    
    def integrate_input(self, dt):
        """Integrate synaptic inputs with custom weighting."""
        
        total_input = 0.0
        
        for synapse in self.getIncomingSynapses():
            input_strength = synapse.getWeight() * self.synapse_strength
            total_input += input_strength
        
        # Apply custom scaling based on energy level
        energy_factor = min(self.energy_level, 1.0)
        total_input *= energy_factor
        
        # Update membrane potential
        self.addToMembranePotential(total_input * dt)
    
    def decay_membrane_potential(self, dt):
        """Custom membrane potential decay."""
        
        # Standard leak decay
        decay_factor = np.exp(-dt / self.time_constant)
        current_potential = self.getMembranePotential()
        self.setMembranePotential(current_potential * decay_factor)
        
        # Custom energy-dependent decay
        if self.energy_level < 0.5:
            self.setMembranePotential(current_potential * 0.9)
    
    def update_energy_level(self, dt):
        """Update neuron energy level."""
        
        # Energy consumption for spiking
        if self.hasSpiked():
            self.energy_level *= 0.9
        
        # Energy recovery when idle
        if not self.hasSpiked():
            self.energy_level = min(1.0, self.energy_level + 0.01)
        
        # Custom energy threshold
        if self.energy_level < 0.1:
            self.setMembranePotential(-80.0)  # Pin to rest potential
    
    def apply_custom_plasticity(self, dt):
        """Apply custom plasticity rules."""
        
        if not self.stdp_enabled and not self.hebbian_enabled:
            return
        
        # Metaplasticity (plasticity of plasticity)
        if self.metaplasticity_enabled:
            self.plasticity_factor *= 0.99
        
        # Custom STDP
        if self.stdp_enabled:
            self.apply_custom_stdp(dt)
        
        # Custom Hebbian learning
        if self.hebbian_enabled:
            self.apply_custom_hebbian()
    
    def apply_custom_stdp(self, dt):
        """Apply custom STDP rule."""
        
        # Custom STDP window
        time_window = 50.0  # ms
        
        for synapse in self.getIncomingSynapses():
            # Custom STDP weight change
            weight_change = self.calculate_custom_stdp(synapse, dt)
            
            # Apply weight change with plasticity factor
            new_weight = synapse.getWeight() + weight_change * self.plasticity_factor
            synapse.setWeight(new_weight)
    
    def calculate_custom_stdp(self, synapse, dt):
        """Calculate custom STDP weight change."""
        
        # Custom STDP formula
        pre_time = synapse.getLastPreTime()
        post_time = self.getLastPostTime()
        
        if pre_time == 0 or post_time == 0:
            return 0.0
        
        time_diff = post_time - pre_time
        
        # Custom STDP kernel
        if time_diff > 0:  # Post after pre (LTP)
            weight_change = 0.01 * np.exp(-time_diff / 20.0)
        else:  # Pre after post (LTD)
            weight_change = -0.012 * np.exp(time_diff / 20.0)
        
        return weight_change
    
    def apply_custom_hebbian(self):
        """Apply custom Hebbian learning."""
        
        # Custom Hebbian rule based on co-activation
        for other_neuron in self.getNearbyNeurons(10):  # Within 10 units
            co_activation = self.get_co_activation_strength(other_neuron)
            
            if co_activation > 0:
                weight_change = 0.001 * co_activation * self.plasticity_factor
                
                # Find synapse and update
                synapse = self.find_synapse_to(other_neuron)
                if synapse:
                    new_weight = synapse.getWeight() + weight_change
                    synapse.setWeight(new_weight)
    
    def get_co_activation_strength(self, other_neuron):
        """Get co-activation strength with another neuron."""
        
        # Implementation would calculate co-activation based on firing patterns
        return 0.5  # Placeholder
    
    def find_synapse_to(self, target_neuron):
        """Find synapse to target neuron."""
        
        # Implementation would find synapse
        return None  # Placeholder

class CustomSynapse(pynlm.Synapse):
    """Custom synapse with advanced features."""
    
    def __init__(self):
        super().__init__()
        
        # Custom synapse properties
        self.delay = 1.0  # ms
        self.retrace_enabled = True
        self.energy_cost = 0.5
        self.custom_modulation = 1.0
        
        # Advanced transmission properties
        self.success_probability = 1.0
        self.weight_history = []
        self.delay_history = []
    
    def transmit(self, post_neuron, dt):
        """Transmit signal to post-synaptic neuron with custom dynamics."""
        
        # Check transmission success
        if np.random.random() > self.success_probability:
            return  # Transmission failed
        
        # Apply delay
        if self.delay > 0:
            self.schedule_transmission(post_neuron, dt + self.delay)
            return
        
        # Immediate transmission
        self.deliver_signal(post_neuron, dt)
    
    def schedule_transmission(self, post_neuron, delay_time):
        """Schedule delayed transmission."""
        
        # Store transmission event
        self.delay_history.append({
            'time': delay_time,
            'post_neuron': post_neuron,
            'weight': self.getWeight()
        })
    
    def deliver_signal(self, post_neuron, dt):
        """Deliver signal to post-synaptic neuron."""
        
        # Calculate signal strength with custom modulation
        signal_strength = self.getWeight() * self.custom_modulation
        
        # Apply energy cost
        self.energy_cost *= 0.99  # Deplete energy over time
        
        # Deliver to post-synaptic neuron
        post_neuron.injectCurrent(signal_strength)
        
        # Record weight history
        self.weight_history.append({
            'time': dt,
            'weight': self.getWeight(),
            'signal_strength': signal_strength
        })
    
    def update_retrace(self, dt):
        """Update synapse retrace information."""
        
        if self.retrace_enabled:
            # Implementation would update retrace information
            pass

class CustomBrain(pynlm.Brain):
    """Custom brain with advanced features."""
    
    def __init__(self, config):
        super().__init__(config)
        
        # Custom brain properties
        self.custom_neurons = []
        self.custom_synapses = []
        self.region_configuration = {}
        
        # Performance tracking
        self.performance_metrics = {}
        self.learning_statistics = {}
    
    def add_custom_neuron(self, neuron):
        """Add custom neuron to brain."""
        
        if hasattr(neuron, 'getId'):
            self.custom_neurons.append(neuron)
    
    def add_custom_synapse(self, synapse):
        """Add custom synapse to brain."""
        
        if hasattr(synapse, 'getId'):
            self.custom_synapses.append(synapse)
    
    def configure_region(self, region_id, neuron_count, connectivity):
        """Configure brain region with custom settings."""
        
        self.region_configuration[region_id] = {
            'neuron_count': neuron_count,
            'connectivity': connectivity,
            'custom_settings': self.get_region_custom_settings(region_id)
        }
    
    def get_region_custom_settings(self, region_id):
        """Get custom settings for brain region."""
        
        # Implementation would return region-specific settings
        return {
            'neuron_type': 'custom',
            'plasticity_enabled': True,
            'energy_management': True
        }
    
    def collect_performance_metrics(self):
        """Collect performance metrics from brain."""
        
        self.performance_metrics = {
            'total_neurons': self.getTotalNeuronCount(),
            'total_synapses': self.getTotalSynapseCount(),
            'firing_rate': self.getAverageFiringRate(),
            'custom_neurons': len(self.custom_neurons),
            'custom_synapses': len(self.custom_synapses),
            'regions_configured': len(self.region_configuration),
            'memory_usage': self.getMemoryUsage(),
            'processing_time': self.getProcessingTime()
        }
    
    def get_learning_statistics(self):
        """Get learning statistics."""
        
        # Implementation would collect learning statistics
        return {
            'learning_rate': 0.01,
            'plasticity_events': 0,
            'knowledge_acquired': 0,
            'skills_developed': []
        }
    
    def getMemoryUsage(self):
        """Get memory usage of custom brain."""
        
        # Implementation would measure memory usage
        return 0  # Placeholder
    
    def getProcessingTime(self):
        """Get processing time of brain."""
        
        # Implementation would measure processing time
        return 0  # Placeholder

class CustomWorld(pynlm.SimpleWorld):
    """Custom world with advanced features."""
    
    def __init__(self):
        super().__init__()
        
        # Custom world properties
        self.environment_objects = {}
        self.terrain_data = {}
        self.weather_conditions = {}
        self.dynamic_obstacles = []
        
        # Custom physics simulation
        self.physics_engine = PhysicsEngine()
        self.collision_detector = CollisionDetector()
    
    def add_environment_object(self, obj_id, properties):
        """Add environment object to world."""
        
        self.environment_objects[obj_id] = {
            'properties': properties,
            'position': {'x': 0, 'y': 0},
            'last_update': time.time()
        }
    
    def set_terrain(self, terrain_type, parameters):
        """Set world terrain."""
        
        self.terrain_data = {
            'type': terrain_type,
            'parameters': parameters,
            'height_map': self.generate_height_map(parameters)
        }
    
    def set_weather(self, weather_type, parameters):
        """Set weather conditions."""
        
        self.weather_conditions = {
            'type': weather_type,
            'parameters': parameters,
            'last_update': time.time()
        }
    
    def add_dynamic_obstacle(self, obstacle):
        """Add dynamic obstacle to world."""
        
        self.dynamic_obstacles.append({
            'id': len(self.dynamic_obstacles),
            'properties': obstacle,
            'position': obstacle.get('position', {'x': 0, 'y': 0}),
            'velocity': obstacle.get('velocity', {'x': 0, 'y': 0}),
            'behavior': obstacle.get('behavior', 'static')
        })
    
    def update_dynamics(self, dt):
        """Update world dynamics."""
        
        # Update environment objects
        self.update_environment_objects(dt)
        
        # Update terrain effects
        self.update_terrain_effects(dt)
        
        # Update weather effects
        self.update_weather_effects(dt)
        
        # Update dynamic obstacles
        self.update_dynamic_obstacles(dt)
        
        # Run physics simulation
        self.physics_engine.update(dt)
    
    def update_environment_objects(self, dt):
        """Update environment object states."""
        
        for obj_id, obj_data in self.environment_objects.items():
            # Update object based on behavior
            if 'behavior' in obj_data['properties']:
                behavior = obj_data['properties']['behavior']
                if behavior == 'moving':
                    self.move_object(obj_id, dt)
                elif behavior == 'growing':
                    self.grow_object(obj_id, dt)
    
    def update_terrain_effects(self, dt):
        """Update terrain effects."""
        
        # Implementation would update terrain effects
        pass
    
    def update_weather_effects(self, dt):
        """Update weather effects."""
        
        # Implementation would update weather effects
        pass
    
    def update_dynamic_obstacles(self, dt):
        """Update dynamic obstacles."""
        
        for obstacle in self.dynamic_obstacles:
            # Update obstacle position
            self.move_obstacle(obstacle, dt)
            
            # Check for collisions
            self.check_collisions(obstacle)
    
    def move_object(self, obj_id, dt):
        """Move environment object."""
        
        obj_data = self.environment_objects[obj_id]
        if 'velocity' in obj_data['properties']:
            velocity = obj_data['properties']['velocity']
            obj_data['position']['x'] += velocity['x'] * dt
            obj_data['position']['y'] += velocity['y'] * dt
            obj_data['last_update'] = time.time()
    
    def grow_object(self, obj_id, dt):
        """Grow environment object."""
        
        obj_data = self.environment_objects[obj_id]
        if 'growth_rate' in obj_data['properties']:
            growth_rate = obj_data['properties']['growth_rate']
            # Implementation would grow object
            pass
    
    def move_obstacle(self, obstacle, dt):
        """Move dynamic obstacle."""
        
        obstacle['position']['x'] += obstacle['velocity']['x'] * dt
        obstacle['position']['y'] += obstacle['velocity']['y'] * dt
        
        # Handle boundary conditions
        self.handle_obstacle_boundaries(obstacle)
    
    def handle_obstacle_boundaries(self, obstacle):
        """Handle obstacle boundary conditions."""
        
        # Implementation would handle boundary conditions
        pass
    
    def check_collisions(self, obstacle):
        """Check for collisions with other obstacles."""
        
        # Implementation would check collisions
        pass
    
    def generate_height_map(self, parameters):
        """Generate terrain height map."""
        
        # Implementation would generate height map
        return [[0 for _ in range(10)] for _ in range(10)]  # Placeholder

class PhysicsEngine:
    """Physics engine for custom world simulation."""
    
    def __init__(self):
        self.forces = []
        self.accelerations = []
        self.velocities = []
    
    def update(self, dt):
        """Update physics simulation."""
        
        # Calculate forces
        self.calculate_forces()
        
        # Update accelerations
        self.update_accelerations()
        
        # Update velocities
        self.update_velocities(dt)
        
        # Update positions
        self.update_positions(dt)
    
    def calculate_forces(self):
        """Calculate forces acting on objects."""
        
        # Implementation would calculate forces
        pass
    
    def update_accelerations(self):
        """Update accelerations based on forces."""
        
        # Implementation would update accelerations
        pass
    
    def update_velocities(self, dt):
        """Update velocities based on accelerations."""
        
        # Implementation would update velocities
        pass
    
    def update_positions(self, dt):
        """Update positions based on velocities."""
        
        # Implementation would update positions
        pass

class CollisionDetector:
    """Collision detector for custom world."""
    
    def __init__(self):
        self.collision_rules = []
    
    def detect_collisions(self, objects):
        """Detect collisions between objects."""
        
        # Implementation would detect collisions
        return []

# Usage example
if __name__ == "__main__":
    # Create custom brain configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    # Create custom brain
    brain = CustomBrain(config)
    
    # Add custom neurons and synapses
    for i in range(100):
        neuron = CustomNeuron()
        brain.add_custom_neuron(neuron)
    
    for i in range(500):
        synapse = CustomSynapse()
        brain.add_custom_synapse(synapse)
    
    # Configure brain regions
    brain.configure_region('cortex', 1000, 0.15)
    brain.configure_region('hippocampus', 500, 0.25)
    brain.configure_region('amygdala', 300, 0.10)
    
    # Collect performance metrics
    brain.collect_performance_metrics()
    
    print("Custom brain performance metrics:", brain.performance_metrics)
    
    # Create custom world
    world = CustomWorld()
    
    # Set up environment
    world.set_terrain('mountainous', {'height_variance': 0.5})
    world.set_weather('rainy', {'precipitation': 0.8, 'wind_speed': 10})
    
    # Add environment objects
    world.add_environment_object('tree', {
        'type': 'tree',
        'height': 10,
        'radius': 2,
        'behavior': 'static'
    })
    
    world.add_environment_object('rock', {
        'type': 'rock',
        'mass': 100,
        'friction': 0.7,
        'behavior': 'static'
    })
    
    # Add dynamic obstacles
    world.add_dynamic_obstacle({
        'position': {'x': 5, 'y': 5},
        'velocity': {'x': 1, 'y': 0},
        'behavior': 'moving'
    })
    
    print("Custom world created successfully!")
    print("World terrain:", world.terrain_data['type'])
    print("World weather:", world.weather_conditions['type'])
    print("Environment objects:", list(world.environment_objects.keys()))
    print("Dynamic obstacles:", len(world.dynamic_obstacles))
```

## 6. Specialized Examples

### Example 6: Cognitive Development Simulation
```python
import pynlm
import numpy as np
from typing import Dict, List, Any

class CognitiveDevelopmentSimulator:
    """Simulate cognitive development over time."""
    
    def __init__(self, config):
        self.config = config
        self.development_stages = self.initialize_development_stages()
        self.cognitive_skills = self.initialize_cognitive_skills()
        self.attention_systems = self.initialize_attention_systems()
        
        # Create brain for simulation
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize subsystems
        self.initialize_cognitive_systems()
    
    def initialize_development_stages(self):
        """Initialize cognitive development stages."""
        
        return {
            'sensory_integration': {
                'age_start': 0,
                'age_end': 10,
                'critical_period': True,
                'learning_rate': 0.1,
                'capacity': 50
            },
            'motor_coordination': {
                'age_start': 5,
                'age_end': 15,
                'critical_period': True,
                'learning_rate': 0.08,
                'capacity': 100
            },
            'language_acquisition': {
                'age_start': 20,
                'age_end': 30,
                'critical_period': False,
                'learning_rate': 0.05,
                'capacity': 200
            },
            'abstract_reasoning': {
                'age_start': 40,
                'age_end': 60,
                'critical_period': False,
                'learning_rate': 0.03,
                'capacity': 300
            },
            'wisdom_integration': {
                'age_start': 60,
                'age_end': 120,
                'critical_period': False,
                'learning_rate': 0.01,
                'capacity': 400
            }
        }
    
    def initialize_cognitive_skills(self):
        """Initialize cognitive skill development."""
        
        return {
            'pattern_recognition': {
                'level': 0,
                'development_rate': 0.02,
                'required_experience': 100,
                'prerequisites': []
            },
            'working_memory': {
                'level': 0,
                'development_rate': 0.015,
                'required_experience': 80,
                'prerequisites': ['sensory_integration']
            },
            'attention_control': {
                'level': 0,
                'development_rate': 0.01,
                'required_experience': 60,
                'prerequisites': ['motor_coordination']
            },
            'problem_solving': {
                'level': 0,
                'development_rate': 0.025,
                'required_experience': 120,
                'prerequisites': ['working_memory', 'attention_control']
            },
            'concept_formation': {
                'level': 0,
                'development_rate': 0.02,
                'required_experience': 200,
                'prerequisites': ['pattern_recognition', 'problem_solving']
            },
            'planning': {
                'level': 0,
                'development_rate': 0.018,
                'required_experience': 150,
                'prerequisites': ['working_memory', 'concept_formation']
            }
        }
    
    def initialize_attention_systems(self):
        """Initialize attention control systems."""
        
        return {
            'selective_attention': {
                'capacity': 10,
                'switch_cost': 0.5,
                'distractibility': 0.7
            },
            'sustained_attention': {
                'capacity': 30,
                'fatigue_rate': 0.01,
                'recovery_rate': 0.02
            },
            'divided_attention': {
                'capacity': 5,
                'performance_degradation': 0.8,
                'training_effectiveness': 0.6
            }
        }
    
    def initialize_cognitive_systems(self):
        """Initialize cognitive subsystems."""
        
        # Configure brain for cognitive development
        self.brain.setDevelopmentMode('cognitive')
        self.brain.setCriticalPeriodMode(True)
        self.brain.setExperienceReplayEnabled(True)
        
        # Initialize agent
        self.agent.initialize()
        
        # Enable cognitive development subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Configure for cognitive development
        self.configure_cognitive_development()
    
    def configure_cognitive_development(self):
        """Configure for cognitive development."""
        
        # Set up development stages
        self.agent.setDevelopmentStages(self.development_stages)
        
        # Set up cognitive skills
        self.agent.setCognitiveSkills(self.cognitive_skills)
        
        # Set up attention systems
        self.agent.setAttentionSystems(self.attention_systems)
        
        # Configure plasticity for development
        self.agent.setDevelopmentPlasticity(True)
        self.agent.setCriticalPeriodPlasticity(True)
    
    def simulate_development(self, age_years=20):
        """Simulate cognitive development over time."""
        
        print("Starting cognitive development simulation...")
        print(f"Simulation duration: {age_years} years")
        
        # Convert age to simulation steps
        total_steps = int(age_years * 365 * 24)  # Days * hours
        
        # Initialize world for simulation
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        
        # Initialize agent
        self.agent.initialize(world)
        world.reset()
        
        # Track development progress
        development_data = {
            'age': 0,
            'steps': list(range(total_steps)),
            'development_stages': {},
            'cognitive_skills': {},
            'attention_systems': {},
            'brain_metrics': {},
            'environmental_stimuli': []
        }
        
        # Simulate development
        for step in range(total_steps):
            age_months = (step / 365) * 12  # Convert to months
            
            # Update environmental stimuli based on age
            stimuli = self.generate_age_appropriate_stimuli(age_months)
            development_data['environmental_stimuli'].append(stimuli)
            
            # Update world with stimuli
            self.update_world_with_stimuli(world, stimuli)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Update development based on age
            self.update_development(age_months)
            
            # Record development data
            development_data['development_stages'][step] = self.get_current_development_stages()
            development_data['cognitive_skills'][step] = self.get_current_cognitive_skills()
            development_data['attention_systems'][step] = self.get_current_attention_systems()
            development_data['brain_metrics'][step] = {
                'firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'learning_efficiency': self.calculate_learning_efficiency()
            }
            
            # Log progress
            if step % 1000 == 0:
                print(f"Development simulation: Step {step}/{total_steps} (Age: {age_months:.1f} months)")
                print(f"  Current stage: {self.get_current_development_stage()}")
                print(f"  Cognitive skills: {self.get_acquired_cognitive_skills()}")
                print(f"  Firing rate: {development_data['brain_metrics'][step]['firing_rate']:.3f}")
        
        print("\nCognitive development simulation completed!")
        return development_data
    
    def generate_age_appropriate_stimuli(self, age_months):
        """Generate age-appropriate environmental stimuli."""
        
        stimuli = {
            'type': 'age_appropriate',
            'age_months': age_months,
            'intensity': 1.0,
            'complexity': 1.0,
            'novelty': 0.5
        }
        
        # Adjust stimuli based on age
        if age_months < 12:
            stimuli.update({
                'type': 'infant',
                'complexity': 0.3,
                'novelty': 0.8,
                'sensory_focus': 'basic'
            })
        
        elif age_months < 36:
            stimuli.update({
                'type': 'toddler',
                'complexity': 0.5,
                'novelty': 0.7,
                'sensory_focus': 'exploratory'
            })
        
        elif age_months < 60:
            stimuli.update({
                'type': 'preschool',
                'complexity': 0.7,
                'novelty': 0.5,
                'sensory_focus': 'narrative'
            })
        
        elif age_months < 120:
            stimuli.update({
                'type': 'school_age',
                'complexity': 0.9,
                'novelty': 0.3,
                'sensory_focus': 'academic'
            })
        
        else:
            stimuli.update({
                'type': 'adult',
                'complexity': 1.0,
                'novelty': 0.2,
                'sensory_focus': 'expert'
            })
        
        return stimuli
    
    def update_world_with_stimuli(self, world, stimuli):
        """Update world with age-appropriate stimuli."""
        
        # Update world based on stimuli type
        if stimuli['type'] == 'infant':
            self.update_for_infant_stimuli(world, stimuli)
        elif stimuli['type'] == 'toddler':
            self.update_for_toddler_stimuli(world, stimuli)
        elif stimuli['type'] == 'preschool':
            self.update_for_preschool_stimuli(world, stimuli)
        elif stimuli['type'] == 'school_age':
            self.update_for_school_age_stimuli(world, stimuli)
        else:
            self.update_for_adult_stimuli(world, stimuli)
    
    def update_for_infant_stimuli(self, world, stimuli):
        """Update world for infant stimuli."""
        
        # Simple, high-contrast objects
        world.clearObjects()
        world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=1.0))
    
    def update_for_toddler_stimuli(self, world, stimuli):
        """Update world for toddler stimuli."""
        
        # Interactive, movable objects
        world.clearObjects()
        world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, value=1.0))
        world.addObject(pynlm.WorldObject(7, 7, pynlm.WorldObjectType.Resource, value=1.0))
    
    def update_for_preschool_stimuli(self, world, stimuli):
        """Update world for preschool stimuli."""
        
        # Narrative elements
        world.clearObjects()
        world.addObject(pynlm.WorldObject(2, 2, pynlm.WorldObjectType.Resource, value=1.0))
        world.addObject(pynlm.WorldObject(8, 8, pynlm.WorldObjectType.Resource, value=1.0))
    
    def update_for_school_age_stimuli(self, world, stimuli):
        """Update world for school-age stimuli."""
        
        # Educational elements
        world.clearObjects()
        for x in [2, 4, 6, 8]:
            for y in [2, 4, 6, 8]:
                world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    def update_for_adult_stimuli(self, world, stimuli):
        """Update world for adult stimuli."""
        
        # Complex environment
        world.clearObjects()
        for x in range(3, 8):
            for y in range(3, 8):
                world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    def update_development(self, age_months):
        """Update development based on age."""
        
        # Update development stages
        for stage_name, stage_data in self.development_stages.items():
            if stage_data['age_start'] <= age_months <= stage_data['age_end']:
                self.update_development_stage(stage_name, stage_data, age_months)
        
        # Update cognitive skills
        self.update_cognitive_skills(age_months)
        
        # Update attention systems
        self.update_attention_systems(age_months)
    
    def update_development_stage(self, stage_name, stage_data, age_months):
        """Update development stage progress."""
        
        # Calculate stage progress
        age_progress = (age_months - stage_data['age_start']) / (
            stage_data['age_end'] - stage_data['age_start']
        )
        
        age_progress = max(0.0, min(1.0, age_progress))
        
        # Update stage progress
        self.development_stages[stage_name]['progress'] = age_progress
        
        # Enable/disable critical period effects
        if stage_data['critical_period']:
            self.enable_critical_period_effects(stage_name)
        
        # Update learning rate based on age
        learning_rate = stage_data['learning_rate'] * age_progress
        self.agent.setLearningRate(learning_rate)
    
    def update_cognitive_skills(self, age_months):
        """Update cognitive skill development."""
        
        for skill_name, skill_data in self.cognitive_skills.items():
            # Check if prerequisites are met
            if self.check_skill_prerequisites(skill_name, age_months):
                # Update skill level based on development stage
                development_boost = self.get_development_stage_boost(age_months)
                skill_level_increase = skill_data['development_rate'] * development_boost
                
                self.cognitive_skills[skill_name]['level'] += skill_level_increase
    
    def update_attention_systems(self, age_months):
        """Update attention system development."""
        
        attention_systems = {
            'selective_attention': age_months / 120.0 * 0.8 + 0.2,
            'sustained_attention': age_months / 120.0 * 0.9 + 0.1,
            'divided_attention': age_months / 120.0 * 0.6 + 0.4
        }
        
        for system_name, level in attention_systems.items():
            self.attention_systems[system_name]['capacity'] = level
    
    def check_skill_prerequisites(self, skill_name, age_months):
        """Check if skill prerequisites are met."""
        
        skill_data = self.cognitive_skills[skill_name]
        
        for prerequisite in skill_data['prerequisites']:
            prereq_data = self.cognitive_skills[prerequisite]
            if prereq_data['level'] < 0.5:  # Require at least 50% development
                return False
        
        return True
    
    def get_development_stage_boost(self, age_months):
        """Get development boost based on age."""
        
        # Peak development during critical periods
        if age_months in [12, 24, 36]:  # Common critical periods
            return 1.5
        
        # Normal development
        return 1.0
    
    def enable_critical_period_effects(self, stage_name):
        """Enable critical period effects for development stage."""
        
        stage_data = self.development_stages[stage_name]
        
        # Implementation would enable critical period plasticity
        self.agent.enableCriticalPeriodPlasticity(True)
        
        # Implementation would increase learning rate
        self.agent.setCriticalPeriodLearningRate(0.1)
    
    def get_current_development_stages(self):
        """Get current development stage progress."""
        
        return {
            stage_name: stage_data['progress']
            for stage_name, stage_data in self.development_stages.items()
        }
    
    def get_current_cognitive_skills(self):
        """Get current cognitive skill levels."""
        
        return {
            skill_name: skill_data['level']
            for skill_name, skill_data in self.cognitive_skills.items()
        }
    
    def get_current_attention_systems(self):
        """Get current attention system levels."""
        
        return {
            system_name: system_data['capacity']
            for system_name, system_data in self.attention_systems.items()
        }
    
    def get_current_development_stage(self):
        """Get current primary development stage."""
        
        current_age = self.get_current_age()
        
        for stage_name, stage_data in self.development_stages.items():
            if stage_data['age_start'] <= current_age <= stage_data['age_end']:
                return stage_name
        
        return 'unknown'
    
    def get_current_age(self):
        """Get current age in months."""
        
        # Implementation would track simulation age
        return 0  # Placeholder
    
    def get_acquired_cognitive_skills(self):
        """Get list of acquired cognitive skills."""
        
        acquired = []
        for skill_name, skill_data in self.cognitive_skills.items():
            if skill_data['level'] > 0.3:  # Consider skill acquired if level > 0.3
                acquired.append(skill_name)
        
        return acquired
    
    def calculate_learning_efficiency(self):
        """Calculate current learning efficiency."""
        
        # Implementation would calculate learning efficiency
        return 0.5  # Placeholder
    
    def run_cognitive_development_simulation(self, duration_years=20):
        """Run cognitive development simulation."""
        
        print("Starting cognitive development simulation...")
        print(f"Duration: {duration_years} years")
        
        # Run development simulation
        development_data = self.simulate_development(duration_years)
        
        print("\n=== Cognitive Development Summary ===")
        print(f"Development stages completed:")
        for stage_name, stage_data in development_data['development_stages'][duration_years//2].items():
            print(f"  {stage_name}: {stage_data:.2%}")
        
        print(f"\nCognitive skills developed:")
        final_skills = development_data['cognitive_skills'][duration_years//2]
        for skill_name, skill_level in final_skills.items():
            if skill_level > 0:
                print(f"  {skill_name}: {skill_level:.2%}")
        
        print(f"\nFinal attention system capacities:")
        final_attention = development_data['attention_systems'][duration_years//2]
        for system_name, capacity in final_attention.items():
            print(f"  {system_name}: {capacity:.2%}")
        
        return development_data

# Usage example
if __name__ == "__main__":
    # Create cognitive development configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    config.set("enable_critical_period_plasticity", True)
    
    # Create cognitive development simulator
    simulator = CognitiveDevelopmentSimulator(config)
    
    # Run cognitive development simulation
    results = simulator.run_cognitive_development_simulation(20)
```

## 7. Summary

This comprehensive collection of advanced NLM example scripts provides power users with sophisticated tools for:

### Key Features:

1. **Advanced Multimodal Processing**
   - Complex sensory integration from multiple modalities
   - Advanced attention and novelty detection
   - Real-time performance monitoring

2. **Lifelong Learning Simulation**
   - Continuous skill acquisition and knowledge base expansion
   - Multi-stage learning with prerequisites
   - Development tracking and optimization

3. **Performance Optimization**
   - System resource monitoring and auto-scaling
   - Adaptive timesteps and memory optimization
   - Performance profiling and optimization

4. **Production Deployment**
   - Comprehensive logging and monitoring
   - Checkpointing and replay systems
   - Production-ready configuration management

5. **Custom Extensions**
   - Custom neuron and synapse types
   - Advanced plasticity rules
   - Custom world physics and dynamics

6. **Specialized Cognitive Development**
   - Age-appropriate environmental stimuli
   - Critical period simulation
   - Cognitive skill tracking and development

### Usage Patterns:

These examples demonstrate:

- **Research Applications**: Complex learning experiments and cognitive development studies
- **Production Systems**: Deployable, scalable NLM implementations
- **Development Tools**: Debugging, monitoring, and performance optimization
- **Educational Examples**: Teaching NLM concepts and capabilities

### Next Steps:

1. **Start with basic examples** to understand NLM functionality
2. **Experiment with advanced configurations** to optimize performance
3. **Customize examples** to match specific research or production needs
4. **Extend the framework** by adding custom components and rules
5. **Monitor and optimize** production deployments for maximum efficiency

These advanced examples provide a solid foundation for developing sophisticated neural network simulations with the NLM framework.

---

*For the latest updates, documentation, and community support, visit the NLM project documentation and forums.*
