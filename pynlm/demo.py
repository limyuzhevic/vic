"""Main demo script for NLM - Phase 6 Integration Demo."""

import sys
import time
import pynlm
from pynlm import (
    createDefaultConfig, createBrain, createAgentBrain, 
    createSimpleWorld, createAction, ActionType,
    getVersionInfo, validateConfig, NeuronType, SynapseType
)

class NLMIntegrationDemo:
    """Phase 6 integration demo for NLM system."""
    
    def __init__(self):
        self.brain = None
        self.agent = None
        self.world = None
        self.config = None
        self.current_step = 0
        self.start_time = 0.0
    
    def setup(self, neuron_count: int = 500, max_steps: int = 2000):
        """Setup the NLM system with specified parameters."""
        print("=== NLM Phase 6 Integration Demo ===")
        print("Setting up neural learning machine...")
        
        # Create configuration
        self.config = createDefaultConfig()
        self.config.neuron_count = neuron_count
        self.config.maxSteps = max_steps
        self.config.enableCheckpointing = True
        self.config.enableReplay = True
        self.config.enableDevelopment = True
        
        # Create and initialize brain
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = createSimpleWorld()
        self.world.configure(
            width=20, height=20,
            visionWidth=8, visionHeight=8
        )
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        # Create agent
        self.agent = createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        self.start_time = time.time()
        print(f"✓ System setup complete")
        print(f"  - Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Max simulation steps: {self.config.maxSteps}")
        print(f"  - World size: {self.world.getWidth()}x{self.world.getHeight()}")
    
    def run_simulation_step(self):
        """Run a single simulation step."""
        # Update world
        self.world.update(0.1)
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Brain step
        self.brain.step(self.current_step)
        
        # Get motor command
        action = self.agent.decodeMotorCommand()
        
        # Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply reward modulation
        reward = self._calculate_reward()
        self.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        self.current_step += 1
        
        # Return metrics
        return {
            'step': self.current_step,
            'reward': reward,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'dopamine': self.agent.getNeuromodulationLevel(),
            'development_stage': self.brain.getDevelopmentalStage()
        }
    
    def _calculate_reward(self) -> float:
        """Calculate reward based on agent state."""
        reward = 0.0
        
        # Energy-based reward
        if hasattr(self.world.getAgentBody(), 'energy'):
            reward += self.world.getAgentBody().energy * 0.001
        
        # Health-based reward
        if hasattr(self.world.getAgentBody(), 'health'):
            reward += self.world.getAgentBody().health * 0.1
        
        # Activity-based reward
        reward += self.brain.getAverageFiringRate() * 0.01
        
        return reward
    
    def run_demo(self, max_steps: int = 2000):
        """Run the complete demo simulation."""
        print(f"\nRunning simulation for {max_steps} steps...")
        
        # Initial status
        self._print_status(0)
        
        # Run simulation
        for step in range(max_steps):
            metrics = self.run_simulation_step()
            
            # Print status every 200 steps
            if step % 200 == 0 and step > 0:
                self._print_status(step)
        
        # Final status
        self._print_final_results()
        
        # Calculate total wall clock time
        wall_clock_time = time.time() - self.start_time
        
        return self._get_final_results(wall_clock_time)
    
    def _print_status(self, step: int):
        """Print current simulation status."""
        print(f"  Step {step:4d}: "
              f"reward={metrics['reward']:6.3f}, "
              f"firing={metrics['firing_neurons']:4d}, "
              f"curiosity={metrics['curiosity']:.3f}, "
              f"dev={metrics['development_stage'].name}")
    
    def _print_final_results(self):
        """Print final simulation results."""
        print(f"\n=== Simulation Complete ===")
        print(f"Total steps: {self.current_step}")
        print(f"Final brain status:")
        print(f"  - Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"  - Final development stage: {self.brain.getDevelopmentalStage().name}")
        print(f"  - Final curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  - Final novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  - Final neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
    
    def _get_final_results(self, wall_clock_time: float) -> Dict:
        """Get final results as dictionary."""
        return {
            'total_steps': self.current_step,
            'total_reward': self.agent.getNeuromodulationLevel(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'final_dopamine': self.agent.getNeuromodulationLevel(),
            'final_development_stage': self.brain.getDevelopmentalStage(),
            'wall_clock_time': wall_clock_time,
            'integration_status': {
                'working_memory': self.brain.getWorkingMemory() is not None,
                'episodic_memory': self.brain.getEpisodicMemory() is not None,
                'neuromodulation': self.brain.getDopamine() is not None,
                'prediction_system': self.brain.getPredictionSystem() is not None,
                'development_system': self.brain.getDevelopmentSystem() is not None
            }
        }

def print_demo_info():
    """Print demo information."""
    info = getVersionInfo()
    print("NLM Phase 6 Integration Demo")
    print("=" * 50)
    print(f"Version: {info['version']}")
    print(f"Author: {info['author']}")
    print()
    print("This demo demonstrates the Phase 6 integration of all NLM systems:")
    print("- Memory systems (working, episodic, associative)")
    print("- Neuromodulation (dopamine, curiosity, novelty)")
    print("- Prediction and learning systems")
    print("- Development and structural plasticity")
    print("- Complete agent-world interaction")
    print()
    print("The demo simulates an agent learning to interact with its environment")
    print("over 2000 simulation steps, demonstrating integration capabilities.")
    print()

def main(argv=None):
    """Main demo entry point."""
    if argv is None:
        argv = sys.argv[1:]
    
    if len(argv) < 1 or argv[0] == "--help" or argv[0] == "-h":
        print_demo_info()
        return 0
    
    # Parse command line arguments
    neuron_count = 500
    max_steps = 2000
    
    for i in range(len(argv)):
        arg = argv[i]
        if arg == "--neurons" and i + 1 < len(argv):
            try:
                neuron_count = int(argv[i + 1])
            except ValueError:
                print(f"Error: Invalid neuron count: {argv[i + 1]}")
                return 1
        elif arg == "--steps" and i + 1 < len(argv):
            try:
                max_steps = int(argv[i + 1])
            except ValueError:
                print(f"Error: Invalid step count: {argv[i + 1]}")
                return 1
        elif arg in ["--help", "-h"]:
            print_demo_info()
            return 0
    
    # Run demo
    demo = NLMIntegrationDemo()
    
    try:
        demo.setup(neuron_count, max_steps)
        results = demo.run_demo(max_steps)
        
        print(f"\n=== DEMO COMPLETED SUCCESSFULLY ===")
        print(f"Simulation completed in {results['wall_clock_time']:.2f} seconds")
        print(f"Total reward: {results['total_reward']:.3f}")
        print(f"Average firing rate: {results['avg_firing_rate']:.2f}")
        print(f"Total spikes: {results['total_spikes']}")
        
        # Print integration status
        print(f"\n=== Integration Status ===")
        status = results['integration_status']
        for system, integrated in status.items():
            print(f"  {system.replace('_', ' ').title()}: {'✓' if integrated else '✗'}")
        
        return 0
        
    except Exception as e:
        print(f"\n✗ Demo failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())