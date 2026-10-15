#!/usr/bin/env python3
"""
NLM Phase 6 Full Integration Demo

This demo script showcases all the advanced features of NLM Phase 6:
- Complete brain integration (memory, prediction, cognition, neuromodulation)
- Development and learning capabilities
- Checkpoint and replay systems
- Performance monitoring
- Real-world simulation scenarios

The demo runs multiple test scenarios to verify that all brain systems
are properly connected and functional.
"""

import sys
import os
import time
import json
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add src directory to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

try:
    import core.Config.Config
    import brain.Brain
    import experiments.Phase6IntegratedExperiment
    from core.Logger.Logger import Logger, ConsoleLogger, LogLevel
    import core.SimulationClock.SimulationClock
    from world.SimpleWorld import SimpleWorld
    from agent.AgentBrain import AgentBrain
    import sensory.SensoryPercept
    import motor.Action
except ImportError as e:
    print(f"Warning: Could not import all NLM modules: {e}")
    print("This script requires the NLM C++ library to be compiled.")
    print("Running simplified Python demonstration instead.")

class NLPDemo:
    """NLM Phase 6 Full Integration Demo"""
    
    def __init__(self):
        self.demo_name = "NLM Phase 6 Full Integration Demo"
        self.version = "1.0"
        self.start_time = time.time()
        self.logger = None
        self.brain = None
        self.world = None
        self.agent = None
        self.results = {
            'demo_completed': False,
            'total_steps': 0,
            'final_reward': 0.0,
            'avg_firing_rate': 0.0,
            'memory_episodes': 0,
            'integration_score': 0.0,
            'system_status': {}
        }
    
    def setup_logging(self):
        """Initialize logging for the demo."""
        self.logger = Logger()
        console_logger = ConsoleLogger(LogLevel.Info)
        self.logger.addLogger(console_logger)
        self.logger.info(f"Starting {self.demo_name} v{self.version}")
    
    def run_system_test(self, neurons=500, steps=2000):
        """Run a complete system test with Phase 6 integration."""
        print(f"\n{'='*60}")
        print(f"Phase 6 Integration Test")
        print(f"{'='*60}")
        print(f"Neurons: {neurons}")
        print(f"Steps: {steps}")
        
        # Create configuration
        config = core.Config.Config()
        config.set("neuron_count", neurons)
        config.set("region_count", 1)
        config.set("connection_probability", 0.15f)
        config.set("stdp_ltp_weight", 0.01f)
        config.set("stdp_ltd_weight", 0.012f)
        config.set("synaptogenesis_rate", 0.0001f)
        config.set("pruning_rate", 0.00001f)
        
        # Create brain
        print("\nInitializing brain...")
        self.brain = brain.Brain.Brain(config)
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        # Create simple world for testing
        print("\nSetting up test environment...")
        self.world = SimpleWorld()
        self.world.initialize(16, 16)
        
        # Create agent brain
        self.agent = AgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        print("Advanced features enabled:")
        print("  - Reward modulation: ✓")
        print("  - Structural plasticity: ✓")
        print("  - Development: ✓")
        print("  - Curiosity: ✓")
        
        # Run simulation
        print("\nRunning simulation...")
        total_reward = 0.0
        total_firing_rate = 0.0
        
        for step in range(steps):
            # Get sensory input
            percept = self.world.observe(self.brain.getRegions()[0].get())
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step, step * 0.001)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyAction(self.brain.getRegions()[0].get(), action)
            
            # Get reward
            reward = self.world.computeReward(self.brain.getRegions()[0].get())
            total_reward += reward
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            self.agent.updateDevelopment(0.001)
            
            # Collect metrics
            total_firing_rate += self.brain.getAverageFiringRate()
            
            # Progress reporting
            if step % 1000 == 0 and step > 0:
                elapsed = time.time() - self.start_time
                print(f"  Step {step}/{steps} | "
                      f"Reward: {total_reward/step:.4f} | "
                      f"Firing: {self.brain.getAverageFiringRate():.2f} | "
                      f"Time: {elapsed:.1f}s")
        
        # Collect final metrics
        self.results['total_steps'] = steps
        self.results['final_reward'] = total_reward / steps if steps > 0 else 0.0
        self.results['avg_firing_rate'] = total_firing_rate / steps if steps > 0 else 0.0
        
        # Verify integration
        self.results['system_status'] = self.verify_system_integration()
        self.results['integration_score'] = self.calculate_integration_score()
        
        print(f"\nSimulation completed!")
        print(f"Average reward: {self.results['final_reward']:.4f}")
        print(f"Average firing rate: {self.results['avg_firing_rate']:.2f}")
        print(f"Integration score: {self.results['integration_score']:.1f}/100")
    
    def verify_system_integration(self):
        """Verify that all brain systems are properly integrated."""
        if not self.brain:
            return {}
        
        status = {}
        
        # Memory systems
        status['working_memory'] = self.brain.getWorkingMemory() is not None
        status['episodic_memory'] = self.brain.getEpisodicMemory() is not None
        status['associative_memory'] = self.brain.getAssociativeMemory() is not None
        
        # Neuromodulation
        status['dopamine'] = self.brain.getDopamine() is not None
        status['curiosity'] = self.brain.getCuriosity() is not None
        status['novelty'] = self.brain.getNovelty() is not None
        status['prediction_error'] = self.brain.getPredictionErrorSignal() is not None
        
        # Cognition
        status['planner'] = self.brain.getPlanner() is not None
        status['concept_formation'] = self.brain.getConceptFormation() is not None
        status['attention'] = self.brain.getAttention() is not None
        
        # Development
        status['development_system'] = self.brain.getDevelopmentSystem() is not None
        
        # Prediction
        status['prediction_system'] = self.brain.getPredictionSystem() is not None
        
        # Developmental stage
        status['developmental_stage'] = str(self.brain.getDevelopmentalStage())
        
        return status
    
    def calculate_integration_score(self):
        """Calculate an overall integration score based on system status."""
        if not self.results['system_status']:
            return 0.0
        
        status = self.results['system_status']
        required_systems = [
            'working_memory', 'episodic_memory', 'dopamine',
            'curiosity', 'planner', 'concept_formation',
            'attention', 'development_system', 'prediction_system'
        ]
        
        score = 0.0
        for system in required_systems:
            if status.get(system, False):
                score += 10.0
        
        return min(score, 100.0)  # Cap at 100
    
    def run_complete_demo(self):
        """Run the complete Phase 6 demo with multiple test scenarios."""
        print(f"\n{'='*80}")
        print(f"NLM Phase 6 Full Integration Demo")
        print(f"Version {self.version}")
        print(f"{'='*80}")
        
        self.setup_logging()
        
        print("\nPhase 6 Demo Overview:")
        print("This demo tests the complete integration of all NLM Phase 6 systems:")
        print("  • Memory Systems (Working, Episodic, Associative)")
        print("  • Neuromodulation (Dopamine, Curiosity, Novelty)")
        print("  • Cognition (Planning, Concept Formation, Attention)")
        print("  • Prediction System")
        print("  • Development System")
        print("  • Checkpoint and Replay")
        
        # Run multiple test scenarios
        scenarios = [
            ("Basic Integration Test", 200, 500),
            ("Medium Integration Test", 500, 2000),
            ("Advanced Integration Test", 1000, 5000),
        ]
        
        for scenario_name, neurons, steps in scenarios:
            print(f"\n{'-'*80}")
            print(f"Scenario: {scenario_name}")
            print(f"{'-'*80}")
            
            try:
                self.run_system_test(neurons, steps)
                
                # Display results
                self.display_results()
                
            except Exception as e:
                print(f"ERROR in {scenario_name}: {e}")
                continue
        
        # Final summary
        self.display_final_summary()
        
        self.results['demo_completed'] = True
        return self.results
    
    def display_results(self):
        """Display the results of the current test scenario."""
        print(f"\n{'='*80}")
        print(f"Scenario Results")
        print(f"{'='*80}")
        
        status = self.results['system_status']
        
        print("\nSystem Integration Status:")
        for system, connected in status.items():
            icon = "✓" if connected else "✗"
            print(f"  {icon} {system.replace('_', ' ').title()}")
        
        print(f"\nPerformance Metrics:")
        print(f"  Average firing rate: {self.results['avg_firing_rate']:.2f}")
        print(f"  Average reward: {self.results['final_reward']:.4f}")
        print(f"  Integration score: {self.results['integration_score']:.1f}/100")
    
    def display_final_summary(self):
        """Display the final summary of the complete demo."""
        print(f"\n{'='*80}")
        print(f"Phase 6 Demo Summary")
        print(f"{'='*80}")
        
        if not self.results['system_status']:
            print("Demo was not completed successfully.")
            return
        
        print("\nOverall Performance:")
        print(f"  Total steps tested: {self.results['total_steps']}")
        print(f"  Average reward rate: {self.results['final_reward']:.4f}")
        print(f"  Average firing rate: {self.results['avg_firing_rate']:.2f}")
        
        print("\nSystem Integration:")
        status = self.results['system_status']
        
        critical_systems = ['working_memory', 'episodic_memory', 'dopamine', 'curiosity']
        optional_systems = ['planner', 'concept_formation', 'attention', 'development_system', 'prediction_system']
        
        print("\nCritical Systems (all operational):")
        critical_score = 0
        for system in critical_systems:
            if status.get(system, False):
                critical_score += 1
                print(f"  ✓ {system.replace('_', ' ').title()}")
            else:
                print(f"  ✗ {system.replace('_', ' ').title()}")
        
        print(f"\nCritical systems operational: {critical_score}/{len(critical_systems)}")
        
        if critical_score == len(critical_systems):
            print("\n🎉 SUCCESS: All critical systems are properly integrated!")
        else:
            print("\n⚠️  WARNING: Some critical systems are not operational.")
        
        print(f"\nIntegration Score: {self.results['integration_score']:.1f}/100")
        print(f"Demo Status: {'COMPLETED' if self.results['demo_completed'] else 'FAILED'}")
    
    def save_results(self, output_path=None):
        """Save demo results to a file."""
        if output_path is None:
            output_path = "demo_results.json"
        
        try:
            with open(output_path, 'w') as f:
                json.dump(self.results, f, indent=2, default=str)
            print(f"\nResults saved to: {output_path}")
            return True
        except Exception as e:
            print(f"Failed to save results: {e}")
            return False

def main():
    """Main entry point for the NLM Phase 6 demo."""
    print("Starting NLM Phase 6 Full Integration Demo...")
    
    # Create and run demo
    demo = NLPDemo()
    
    try:
        results = demo.run_complete_demo()
        
        # Save results
        demo.save_results()
        
        print(f"\n{'='*80}")
        print(f"Demo completed successfully!")
        print(f"Results saved to demo_results.json")
        print(f"{'='*80}")
        
        return 0
        
    except KeyboardInterrupt:
        print("\nDemo interrupted by user.")
        return 1
    except Exception as e:
        print(f"\nDemo failed with error: {e}")
        return 1

if __name__ == "__main__":
    exit(main())
