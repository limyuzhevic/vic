#!/usr/bin/env python3
"""Improved NLM Python API with better error handling and user experience."""

import pynlm
import sys
import traceback
from typing import Optional, List, Dict, Any

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class BrainInitializationError(NLMError):
    """Brain initialization failed."""
    pass

class SimulationError(NLMError):
    """Simulation step failed."""
    pass

class WorldError(NLMError):
    """World configuration error."""
    pass

class ImprovedBrain:
    """Enhanced brain wrapper with better error handling and convenience methods."""
    
    def __init__(self, config=None):
        """Initialize improved brain with optional custom config."""
        try:
            if config is None:
                self._config = pynlm.createDefaultConfig()
            else:
                self._config = config
            
            self._brain = pynlm.createBrain(self._config)
            self._brain.initialize()
            
        except Exception as e:
            raise BrainInitializationError(f"Failed to initialize brain: {e}")
    
    def run_simulation(self, steps: int, auto_reset: bool = True) -> Dict[str, Any]:
        """Run simulation with automatic statistics collection."""
        if steps <= 0:
            raise SimulationError("Number of steps must be positive")
        
        stats = {
            'steps_completed': 0,
            'total_spikes': 0,
            'firing_neurons': [],
            'average_firing_rate': 0.0,
            'errors': []
        }
        
        try:
            for step in range(steps):
                self._brain.step(step)
                
                if auto_reset and step % 100 == 99:
                    self._brain.reset()
                    self._brain.initialize()
                
                stats['steps_completed'] = step + 1
                
        except Exception as e:
            stats['errors'].append(str(e))
            raise SimulationError(f"Simulation failed at step {step}: {e}")
        
        # Collect final statistics
        stats['total_spikes'] = self._brain.getTotalSpikeCount()
        stats['firing_neurons'] = self._brain.getFiringNeuronCount()
        stats['average_firing_rate'] = self._brain.getAverageFiringRate()
        
        return stats
    
    def save_brain(self, filepath: str) -> bool:
        """Save brain state with error checking."""
        if not filepath:
            raise ValueError("Filepath cannot be empty")
        
        try:
            self._brain.save(filepath)
            return True
        except Exception as e:
            raise NLMError(f"Failed to save brain to {filepath}: {e}")
    
    def load_brain(self, filepath: str) -> bool:
        """Load brain state with error checking."""
        if not filepath:
            raise ValueError("Filepath cannot be empty")
        
        try:
            self._brain.load(filepath)
            return True
        except Exception as e:
            raise NLMError(f"Failed to load brain from {filepath}: {e}")
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        return {
            'neuron_count': self._brain.getTotalNeuronCount(),
            'synapse_count': self._brain.getTotalSynapseCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'developmental_stage': self._brain.getDevelopmentalStage().name,
            'config_summary': self._config.summary()
        }
    
    def __del__(self):
        """Cleanup on deletion."""
        if hasattr(self, '_brain'):
            self._brain = None

class ImprovedWorld:
    """Enhanced world wrapper with better user experience."""
    
    def __init__(self, width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8):
        """Initialize improved world with validation."""
        if width <= 0 or height <= 0:
            raise WorldError("World dimensions must be positive")
        if vision_width <= 0 or vision_height <= 0:
            raise WorldError("Vision dimensions must be positive")
        
        self._world = pynlm.createSimpleWorld()
        self._world.configure(width, height, vision_width, vision_height)
        self._world.reset()
    
    def create_agent(self, brain) -> 'ImprovedAgent':
        """Create improved agent with this world."""
        return ImprovedAgent(brain, self._world)
    
    def __getattr__(self, name):
        """Delegate other methods to underlying world."""
        return getattr(self._world, name)

class ImprovedAgent:
    """Enhanced agent with better interface and learning features."""
    
    def __init__(self, brain, world):
        """Initialize improved agent."""
        self._brain = brain
        self._agent = pynlm.createAgentBrain(brain)
        self._agent.initialize(world)
        
        # Pre-enabled useful subsystems
        self._agent.enableRewardModulation(True)
        self._agent.enableCuriosity(True)
        self._agent.enableDevelopment(True)
    
    def run_episode(self, max_steps: int = 1000) -> Dict[str, Any]:
        """Run complete episode with automatic tracking."""
        episode_stats = {
            'steps': 0,
            'actions_taken': 0,
            'total_reward': 0.0,
            'novelty_level': 0.0,
            'curiosity_level': 0.0,
            'predictions_correct': 0,
            'errors': []
        }
        
        try:
            for step in range(max_steps):
                # Get sensory input
                percept = self._world.getSensoryPercept()
                
                # Process input
                self._agent.processSensoryInput(percept)
                
                # Brain thinks
                self._brain.step(step)
                
                # Get action
                action = self._agent.decodeMotorCommand()
                
                # Apply action
                self._world.applyMotorCommand(action, self._world.getSimulationTime())
                
                # Get reward and novelty
                internal_signals = percept.getInternal()
                reward = internal_signals[0] if internal_signals else 0.0
                
                # Apply reward modulation
                predicted = 0.0  # Could be implemented with prediction system
                self._agent.applyRewardModulation(reward, predicted)
                
                # Update development
                self._agent.updateDevelopment(0.1)
                
                # Collect stats
                episode_stats['steps'] += 1
                episode_stats['actions_taken'] += 1
                episode_stats['total_reward'] += reward
                episode_stats['novelty_level'] = self._agent.getNoveltyLevel()
                episode_stats['curiosity_level'] = self._agent.getCuriosityLevel()
                episode_stats['predictions_correct'] += 1  # Simplified
                
                # Check if episode should end
                if self._should_end_episode(episode_stats):
                    break
                    
        except Exception as e:
            episode_stats['errors'].append(str(e))
            raise SimulationError(f"Episode failed: {e}")
        
        return episode_stats
    
    def _should_end_episode(self, stats: Dict[str, Any]) -> bool:
        """Determine if episode should end."""
        # Default: end after max_steps or if agent is stuck
        if stats['steps'] >= 1000:
            return True
        return False
    
    def enable_advanced_features(self):
        """Enable all advanced features."""
        self._agent.enableStructuralPlasticity(True)
        self._agent.enableCuriosity(True)
        self._agent.enableDevelopment(True)
        self._agent.enableRewardModulation(True)
    
    def __getattr__(self, name):
        """Delegate other methods to underlying agent."""
        return getattr(self._agent, name)

def create_advanced_simulation(config=None, **kwargs) -> Dict[str, Any]:
    """Create and run an advanced simulation with all improvements."""
    try:
        # Create world with validated parameters
        width = kwargs.get('world_width', 20)
        height = kwargs.get('world_height', 20)
        vision_width = kwargs.get('vision_width', 8)
        vision_height = kwargs.get('vision_height', 8)
        
        world = ImprovedWorld(width, height, vision_width, vision_height)
        
        # Create brain
        if config is None:
            config = pynlm.createDefaultConfig()
        
        brain = ImprovedBrain(config)
        
        # Create agent
        agent = ImprovedAgent(brain, world)
        
        # Enable advanced features
        agent.enable_advanced_features()
        
        # Run simulation
        steps = kwargs.get('steps', 100)
        stats = agent.run_episode(steps)
        
        return {
            'success': True,
            'brain': brain,
            'world': world,
            'agent': agent,
            'stats': stats,
            'brain_stats': brain.get_brain_stats()
        }
        
    except Exception as e:
        return {
            'success': False,
            'error': str(e),
            'traceback': traceback.format_exc()
        }

# Convenience functions
def quick_test():
    """Quick test of the improved API."""
    print("Testing Improved NLM API...")
    
    try:
        # Quick test
        config = pynlm.createDefaultConfig()
        brain = ImprovedBrain(config)
        
        stats = brain.run_simulation(10, auto_reset=False)
        print(f"Quick test passed! Steps: {stats['steps_completed']}, Spikes: {stats['total_spikes']}")
        
        return True
        
    except Exception as e:
        print(f"Quick test failed: {e}")
        return False

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Improved NLM Python API")
    parser.add_argument("--quick-test", action="store_true", help="Run quick test")
    parser.add_argument("--advanced-simulation", action="store_true", help="Run advanced simulation")n    
    args = parser.parse_args()
    
    if args.quick_test:
        quick_test()
    elif args.advanced_simulation:
        result = create_advanced_simulation(steps=50)
        if result['success']:
            print("Advanced simulation completed successfully!")
            print(f"Final stats: {result['brain_stats']}")
        else:
            print(f"Advanced simulation failed: {result['error']}")
            sys.exit(1)
    else:
        parser.print_help()
