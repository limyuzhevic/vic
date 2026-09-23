# High-level API for NLM
"""
High-level Python API for NLM (Neural Learning Machine) - Phase 6 Edition.

This module provides a user-friendly interface to the NLM brain simulation framework,
extending the basic bindings with enhanced functionality, advanced commands,
and expert-level features.

Features:
- Simplified brain creation and configuration
- Advanced simulation presets
- Performance monitoring and statistics
- Checkpoint management
- Experiment management
- Visualization helpers
- Advanced learning algorithms
"""

import pynlm
from typing import Tuple, Optional, Dict, Any, Callable
import json
import time

# Re-export everything from the core module
from .pynlm import (
    # Core classes
    Brain, Config, AgentBrain, SimpleWorld, WorldObject,
    SensoryPercept, MotorCommand, NeuronType, DevelopmentalStage,
    ActionType, WorldObjectType,
    
    # Factory functions
    createBrain, createDefaultConfig, createAgentBrain, 
    createSimpleWorld,
    
    # Utility functions
    get_version, print_banner,
    
    # Integration tests
    run_phase6_integration_test,
    
    # Checkpoint operations
    save_brain_checkpoint, load_brain_checkpoint,
    
    # Advanced creation functions
    create_advanced_brain, create_complex_world,
    
    # Advanced simulations
    run_advanced_simulation,
)

class NLMSimulation:
    """High-level simulation manager for NLM experiments."""
    
    def __init__(self, config: Optional[Config] = None, 
                 world: Optional[SimpleWorld] = None):
        """Initialize simulation with optional config and world."""
        self.config = config or createDefaultConfig()
        self.world = world or createSimpleWorld()
        self.brain = None
        self.agent = None
        self.is_running = False
        self.step_count = 0
        self.metrics = {
            'spikes': [],
            'curiosity': [],
            'novelty': [],
            'rewards': [],
            'fitness': []
        }
        
    def setup_brain(self, brain_type: str = "standard") -> 'Brain':
        """Setup brain with different configurations."""
        if brain_type == "standard":
            self.brain = create_advanced_brain(2000, 0.12)
        elif brain_type == "expert":
            self.brain = create_advanced_brain(5000, 0.15)
        elif brain_type == "compact":
            self.brain = create_advanced_brain(1000, 0.08)
        else:
            raise ValueError(f"Unknown brain type: {brain_type}")
            
        self.brain.initialize()
        
        # Setup agent
        if self.world:
            self.agent = createAgentBrain(self.brain)
            self.agent->initialize(*self.world)
            
        return self.brain
    
    def setup_complex_world(self, width: int = 30, height: int = 30,
                           vision_x: int = 8, vision_y: int = 8) -> 'SimpleWorld':
        """Setup complex world with obstacles and resources."""
        self.world = create_complex_world(width, height, vision_x, vision_y)
        
        # Setup agent with new world
        if self.brain and self.agent:
            self.agent->initialize(*self.world)
            
        return self.world
    
    def run_episode(self, steps: int = 1000, enable_learning: bool = True,
                    verbose: bool = False) -> Dict[str, Any]:
        """Run a complete simulation episode."""
        if not self.brain or not self.agent:
            raise RuntimeError("Brain or agent not initialized. Call setup_brain() first.")
            
        self.is_running = True
        self.step_count = 0
        
        # Enable all learning subsystems
        self.agent->enableRewardModulation(enable_learning)
        self.agent->enableStructuralPlasticity(enable_learning)
        self.agent->enableDevelopment(enable_learning)
        self.agent->enableCuriosity(enable_learning)
        
        # Reset metrics
        for key in self.metrics:
            self.metrics[key].clear()
        
        if verbose:
            print_banner()
            print(f"Starting NLM Phase 6 Simulation")
            print(f"Brain type: {type(self.brain).__name__}")
            print(f"World size: {self.world.getWidth()}x{self.world.getHeight()}")
            print(f"Total steps: {steps}")
            print("=" * 50)
        
        start_time = time.time()
        
        for step in range(steps):
            # Update world
            self.world->update(0.1)
            
            # Get sensory input
            percept = self.world->getSensoryPercept()
            
            # Process sensory input
            self.agent->processSensoryInput(percept)
            
            # Brain thinks
            self.brain->step(step)
            
            # Get motor command
            action = self.agent->decodeMotorCommand()
            
            # Apply action to world
            self.world->applyMotorCommand(action, self.world->getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if not percept.getInternal().empty():
                reward = percept.getInternal()[0]
            self.agent->applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent->updateDevelopment(0.1)
            
            # Record metrics
            self.metrics['spikes'].append(self.brain->getTotalSpikeCount())
            self.metrics['curiosity'].append(self.agent->getCuriosityLevel())
            self.metrics['novelty'].append(self.agent->getNoveltyLevel())
            self.metrics['rewards'].append(reward)
            self.metrics['fitness'].append(self.calculate_fitness(step))
            
            self.step_count = step + 1
            
            if verbose and step % 100 == 0:
                self.print_step_summary(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        self.is_running = False
        
        # Return final statistics
        stats = self.get_episode_statistics(duration)
        
        if verbose:
            print("=" * 50)
            print(f"Episode completed in {duration:.2f} seconds")
            print(f"Final statistics: {stats}")
        
        return stats
    
    def calculate_fitness(self, step: int) -> float:
        """Calculate fitness score based on behavior."""
        fitness = 0.0
        
        # Curiosity bonus
        curiosity = self.agent->getCuriosityLevel()
        fitness += curiosity * 10.0
        
        # Novelty bonus
        novelty = self.agent->getNoveltyLevel()
        fitness += novelty * 5.0
        
        # Spike activity (not too low, not too high)
        spike_rate = self.brain->getAverageFiringRate()
        if 5.0 <= spike_rate <= 50.0:
            fitness += 20.0
        elif spike_rate < 5.0:
            fitness -= 5.0
        else:
            fitness -= 10.0
        
        # Prediction error learning indicator
        pred_error = abs(self.agent->getPredictionError())
        if pred_error > 0.5:
            fitness += 15.0
        
        # Developmental progress
        dev_stage = self.agent->getDevelopmentalStage()
        if dev_stage == DevelopmentalStage::Adult:
            fitness += 25.0
        elif dev_stage == DevelopmentalStage::CriticalPeriod:
            fitness += 15.0
            
        return fitness
    
    def get_episode_statistics(self, duration: float) -> Dict[str, Any]:
        """Calculate comprehensive episode statistics."""
        stats = {
            'duration': duration,
            'steps': self.step_count,
            'final_spike_count': self.brain->getTotalSpikeCount(),
            'final_firing_rate': self.brain->getAverageFiringRate(),
            'max_curiosity': max(self.metrics['curiosity']) if self.metrics['curiosity'] else 0.0,
            'max_novelty': max(self.metrics['novelty']) if self.metrics['novelty'] else 0.0,
            'avg_reward': sum(self.metrics['rewards']) / len(self.metrics['rewards']) if self.metrics['rewards'] else 0.0,
            'final_fitness': self.metrics['fitness'][-1] if self.metrics['fitness'] else 0.0,
            'avg_fitness': sum(self.metrics['fitness']) / len(self.metrics['fitness']) if self.metrics['fitness'] else 0.0,
            'developmental_stage': self.agent->getDevelopmentalStage(),
            'neuromodulation_level': self.agent->getNeuromodulationLevel(),
            'prediction_error': self.agent->getPredictionError()
        }
        
        return stats
    
    def print_step_summary(self, step: int):
        """Print step-by-step summary."""
        print(f"Step {step:4d}: "
              f"Spikes={self.brain->getTotalSpikeCount():6d} "
              f"Curiosity={self.agent->getCuriosityLevel():.3f} "
              f"Novelty={self.agent->getNoveltyLevel():.3f} "
              f"Dev={self.agent->getDevelopmentalStage():12s}")
    
    def save_checkpoint(self, filename: str):
        """Save simulation state to checkpoint file."""
        if self.brain and self.agent and self.world:
            save_brain_checkpoint(self.brain, filename)
            
            # Save simulation state
            state = {
                'step_count': self.step_count,
                'metrics': self.metrics,
                'config': self.config,
                'world_size': (self.world.getWidth(), self.world.getHeight())
            }
            
            with open(f"{filename}_sim.json", 'w') as f:
                json.dump(state, f, indent=2)
            
            if verbose:
                print(f"Checkpoint saved: {filename}")
    
    def load_checkpoint(self, filename: str):
        """Load simulation state from checkpoint file."""
        if not filename.endswith('.bin'):
            raise ValueError("Checkpoint filename must end with .bin")
            
        self.brain = load_brain_checkpoint(filename)
        
        # Load simulation state if available
        sim_filename = filename.replace('.bin', '_sim.json')
        try:
            with open(sim_filename, 'r') as f:
                state = json.load(f)
                self.step_count = state['step_count']
                self.metrics = state['metrics']
        except FileNotFoundError:
            pass
            
        if self.brain and self.world:
            self.agent = createAgentBrain(self.brain)
            self.agent->initialize(*self.world)
            
        if verbose:
            print(f"Checkpoint loaded: {filename}")

# Experiment management
class ExperimentManager:
    """Manage NLM experiments with different configurations."""
    
    def __init__(self):
        self.experiments = {}
        
    def create_experiment(self, name: str, **kwargs) -> 'NLMSimulation':
        """Create a new experiment with given parameters."""
        sim = NLMSimulation()
        self.experiments[name] = sim
        
        # Apply experiment-specific settings
        if 'brain_type' in kwargs:
            sim.setup_brain(kwargs['brain_type'])
        else:
            sim.setup_brain()
            
        if 'world_complexity' in kwargs:
            sim.setup_complex_world()
            
        return sim
    
    def run_experiments(self, names: list, steps: int = 1000) -> Dict[str, Dict]:
        """Run multiple experiments and collect results."""
        results = {}
        
        for name in names:
            if name not in self.experiments:
                print(f"Warning: Experiment '{name}' not found")
                continue
                
            sim = self.experiments[name]
            print(f"Running experiment: {name}")
            
            results[name] = sim.run_episode(steps, verbose=True)
            
        return results
    
    def compare_experiments(self, names: list, steps: int = 1000):
        """Compare multiple experiments side by side."""
        results = self.run_experiments(names, steps)
        
        print("\n" + "="*60)
        print("EXPERIMENT COMPARISON RESULTS")
        print("="*60)
        
        for name, result in results.items():
            print(f"\n{name.upper()}:")
            print(f"  Duration: {result['duration']:.2f}s")
            print(f"  Final Spike Count: {result['final_spike_count']}")
            print(f"  Final Firing Rate: {result['final_firing_rate']:.2f}")
            print(f"  Max Curiosity: {result['max_curiosity']:.3f}")
            print(f"  Max Novelty: {result['max_novelty']:.3f}")
            print(f"  Avg Reward: {result['avg_reward']:.3f}")
            print(f"  Final Fitness: {result['final_fitness']:.2f}")
            print(f"  Developmental Stage: {result['developmental_stage']}")
            
        # Determine winner
        if len(results) > 1:
            best = max(results.keys(), key=lambda k: results[k]['final_fitness'])
            print(f"\n🏆 BEST PERFORMING EXPERIMENT: {best.upper()} (Fitness: {results[best]['final_fitness']:.2f})")
            
        return results

# Preset simulation configurations
presets = {
    'quick_test': {
        'brain_type': 'compact',
        'world_size': (10, 10),
        'steps': 100,
        'learning': False
    },
    'standard_test': {
        'brain_type': 'standard', 
        'world_size': (20, 20),
        'steps': 500,
        'learning': True
    },
    'expert_test': {
        'brain_type': 'expert',
        'world_size': (40, 40),
        'steps': 2000,
        'learning': True
    },
    'integration_test': {
        'brain_type': 'standard',
        'world_size': (30, 30),
        'steps': 1000,
        'learning': True,
        'run_phase6_test': True
    }
}

# Convenience functions for quick usage
def quick_test(steps: int = 100) -> Dict[str, Any]:
    """Run a quick test with minimal configuration."""
    sim = NLMSimulation()
    sim.setup_brain('compact')
    sim.setup_complex_world(15, 15, 6, 6)
    return sim.run_episode(steps, verbose=True)

def standard_test(steps: int = 500) -> Dict[str, Any]:
    """Run a standard test with balanced configuration."""
    sim = NLMSimulation()
    sim.setup_brain('standard')
    sim.setup_complex_world(25, 25, 8, 8)
    return sim.run_episode(steps, verbose=True)

def expert_test(steps: int = 2000) -> Dict[str, Any]:
    """Run an expert-level test with advanced configuration."""
    sim = NLMSimulation()
    sim.setup_brain('expert')
    sim.setup_complex_world(50, 50, 10, 10)
    return sim.run_episode(steps, verbose=True)

def run_integration_test() -> Dict[str, Any]:
    """Run the complete Phase 6 integration test."""
    return run_phase6_integration_test()

def save_experiment_config(name: str, **kwargs):
    """Save experiment configuration to file."""
    if name not in presets:
        presets[name] = kwargs
    else:
        presets[name].update(kwargs)
    
    print(f"Configuration '{name}' saved to presets")

# Error handling and validation
def validate_simulation_config(brain_size: int, world_size: tuple, 
                              steps: int, learning: bool) -> bool:
    """Validate simulation configuration parameters."""
    errors = []
    
    if brain_size < 100 or brain_size > 10000:
        errors.append(f"Brain size {brain_size} out of range [100, 10000]")
        
    if world_size[0] < 5 or world_size[1] < 5:
        errors.append(f"World size {world_size} too small, minimum [5, 5]")
        
    if steps < 10 or steps > 100000:
        errors.append(f"Steps {steps} out of range [10, 100000]")
        
    if learning and steps < 100:
        errors.append("Learning enabled but steps < 100 - insufficient for learning")
        
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  ❌ {error}")
        return False
        
    print("✅ Configuration validation passed")
    return True

# Version and information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "High-level NLM Phase 6 API with advanced features"

# Export everything
__all__ = [
    'NLMSimulation', 'ExperimentManager', 'presets',
    'quick_test', 'standard_test', 'expert_test', 
    'run_integration_test', 'save_experiment_config',
    'validate_simulation_config',
    'print_banner', 'get_version'
]