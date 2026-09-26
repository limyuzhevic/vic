# PHASE6_INTEGRATION_EXAMPLES.md

# Phase 6 Integration Examples

## Overview

This document provides comprehensive examples demonstrating the Phase 6 integration of all NLM brain systems. Phase 6 represents the final integration where all previously developed components work together as a coherent artificial brain system.

## Phase 6 Integration Architecture

Phase 6 integrates the following core systems:

### 1. Memory Systems Integration
- **Working Memory**: Persistent activity and competitive selection
- **Episodic Memory**: Experience encoding and replay mechanisms
- **Associative Memory**: Hebbian pattern associations and concept formation
- **Memory-Neural Coupling**: All memory systems connected to neural processing

### 2. Neuromodulation Integration
- **Dopamine System**: Affects neural excitability and plasticity
- **Curiosity System**: Drives exploration behavior
- **Novelty Detection**: Integrated with sensory processing
- **Neuromodulator Coordination**: All systems work together for learning

### 3. Prediction System Integration
- **Prediction Error Computation**: Real-time prediction error calculation
- **Learning Signal Integration**: Prediction errors affect all neuromodulators
- **Confidence Tracking**: Implementation of confidence in predictions

### 4. Cognition Systems Integration
- **Neural Planning**: Action sequence evaluation and decision making
- **Concept Formation**: Pattern discovery and categorization
- **Attention System**: Competitive selection dynamics
- **Perception-Action Loop**: All cognition systems connected

### 5. Development Integration
- **Developmental Stages**: Plasticity rates affected by age
- **Structural Plasticity**: Neural connectivity modulated by development
- **Neural Excitability**: Changes with developmental stage

### 6. Persistence and Replay
- **Checkpoint System**: Save/load brain state
- **Memory Replay**: Episodic memory consolidation during rest
- **Sleep Integration**: Memory consolidation with sleep cycle

### 7. Reward System
- **Real Reward Computation**: Environmental feedback and goal proximity
- **Reinforcement Learning**: Reward signals drive adaptive behavior
- **Reward-Neuromodulator Coupling**: Integrated with all learning systems

## Example 1: Complete Phase 6 Brain Simulation

### Overview
This example demonstrates a complete Phase 6 brain simulation with all integrated systems working together.

```python
#!/usr/bin/env python3
"""
Complete Phase 6 Integration Example
====================================

This example demonstrates all Phase 6 integrated systems:
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction system with real prediction errors
- Cognition systems (planning, concept formation, attention)
- Development integration
- Reward-driven learning
- Persistence and replay
"""

import pynlm
import numpy as np
import time
from typing import Dict, List, Any

class Phase6BrainSimulation:
    """Complete Phase 6 brain simulation with all integrated systems."""
    
    def __init__(self, config: Dict[str, Any] = None):
        """Initialize Phase 6 simulation with all systems."""
        print("Initializing Phase 6 Brain Simulation...")
        
        # Create configuration with Phase 6 optimal settings
        self.config = config or self._create_phase6_config()
        
        # Initialize brain with Phase 6 integration
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=40,
            height=40,
            visionWidth=16,
            visionHeight=16,
            visionFOV=120,
            maxEnergy=1000
        )
        
        # Initialize systems
        self._initialize_all_systems()
        
        # Simulation statistics
        self.simulation_stats = self._initialize_stats()
        
        print("✓ Phase 6 simulation initialized successfully")
        print(f"  - Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Memory systems: Working, Episodic, Associative")
        print(f"  - Neuromodulators: Dopamine, Curiosity, Novelty, Prediction Error")
        print(f"  - Cognitive subsystems: Planning, Concept Formation, Attention")
    
    def _create_phase6_config(self) -> Dict[str, Any]:
        """Create optimal configuration for Phase 6 integration."""
        return {
            # Brain configuration for Phase 6
            "brain": {
                "neuron_count": 3000,
                "synapse_density": 0.08,
                "connection_probability": 0.04,
                "v_thresh": -45.0,  # Higher threshold for Phase 6
                "v_rest": -65.0,
                "v_reset": -70.0,
                "tau_mem": 25.0,  # Slower dynamics for integration
                "tau_ref": 3.0,
                "enable_predictive_coding": True,
                "enable_concept_formation": True,
                "enable_neural_planning": True
            },
            
            # Plasticity configuration for Phase 6
            "plasticity": {
                "stdp": {
                    "enable": True,
                    "learning_rate": 0.002,  # Enhanced for integration
                    "tau_plus": 25.0,
                    "tau_minus": 25.0,
                    "learning_window": 50.0  # Longer window for temporal integration
                },
                "hebbian": {
                    "enable": True,
                    "learning_rate": 0.001,
                    "correlation_window": 30.0
                },
                "structural": {
                    "enable": True,
                    "growth_rate": 0.001,
                    "pruning_rate": 0.0005,
                    "developmental_modulation": True
                },
                "development": {
                    "modulate_plasticity": True,
                    "critical_period_duration": 200,
                    "maturation_rate": 0.005
                }
            },
            
            # Neuromodulation configuration for Phase 6
            "neuromod": {
                "dopamine": {
                    "scale": 1.5,  # Enhanced influence
                    "baseline": 0.2,
                    "prediction_error_influence": True
                },
                "curiosity": {
                    "enable": True,
                    "sensitivity": 2.0,  # High curiosity for exploration
                    "exploration_bonus": 0.5,
                    "prediction_error_drive": True
                },
                "novelty": {
                    "enable": True,
                    "threshold": 0.3,
                    "detection_sensitivity": 1.5,
                    "prediction_error_modulation": True
                },
                "prediction_error": {
                    "sensitivity": 2.0,
                    "temporal_integration": True,
                    "confidence_computation": True
                }
            },
            
            # Memory configuration for Phase 6
            "memory": {
                "working_memory": {
                    "capacity": 100,
                    "competition_strength": 0.8,
                    "decay_rate": 0.001
                },
                "episodic_memory": {
                    "encoding_strength": 0.9,
                    "replay_frequency": 0.1,
                    "consolidation_threshold": 0.7
                },
                "associative_memory": {
                    "pattern_threshold": 0.6,
                    "association_strength": 0.5,
                    "concept_formulation": True
                }
            }
        }
    
    def _initialize_all_systems(self):
        """Initialize all integrated systems."""
        # Reset world and set agent
        self.world.reset()
        self.world.setAgentStart(20.0, 20.0)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable all subsystems for Phase 6
        self._enable_phase6_subsystems()
        
        # Configure world objects
        self._setup_world_environment()
        
        print("✓ All integrated systems initialized")
    
    def _enable_phase6_subsystems(self):
        """Enable all subsystems for Phase 6 integration."""
        # Core neuromodulation systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableNovelty(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Advanced cognitive systems
        self.agent.enablePredictionSystem(True)
        self.agent.enableConceptFormation(True)
        self.agent.enableAttentionSystem(True)
        self.agent.enableNeuralPlanning(True)
        
        print("  - Reward modulation enabled")
        print("  - Curiosity system enabled")
        print("  - Novelty detection enabled")
        print("  - Structural plasticity enabled")
        print("  - Development system enabled")
        print("  - Prediction system enabled")
        print("  - Concept formation enabled")
        print("  - Attention system enabled")
        print("  - Neural planning enabled")
    
    def _setup_world_environment(self):
        """Set up the world environment for Phase 6 testing."""
        # Add resources
        resources = [
            (5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0),
            (35.0, 5.0, pynlm.WorldObjectType.Resource, 15.0),
            (5.0, 35.0, pynlm.WorldObjectType.Resource, 20.0),
            (35.0, 35.0, pynlm.WorldObjectType.Resource, 25.0)
        ]
        
        for x, y, obj_type, value in resources:
            self.world.addObject(pynlm.WorldObject(x, y, obj_type, value=value))
        
        # Add hazards
        for x in [15.0, 25.0]:
            for y in [15.0, 25.0]:
                self.world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=5.0))
        
        print("  - World environment configured")
        print(f"  - {len(resources)} resources placed")
        print(f"  - {8} hazards placed")
    
    def _initialize_stats(self) -> Dict[str, Any]:
        """Initialize simulation statistics."""
        return {
            "steps": 0,
            "memory_stats": [],
            "neuromod_stats": [],
            "prediction_stats": [],
            "development_stats": [],
            "reward_stats": [],
            "performance_metrics": {}
        }
    
    def run_simulation_step(self, step: int) -> Dict[str, Any]:
        """Run a single simulation step with Phase 6 integration."""
        # Update world physics
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input through all integrated systems
        self.agent.processSensoryInput(percept)
        
        # Run brain computation (integrates all systems)
        self.brain.step(step)
        
        # Decode motor command
        motor_cmd = self.agent.decodeMotorCommand()
        
        # Apply motor command to world
        self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
        
        # Calculate and apply reward
        reward = self._calculate_reward(percept)
        predicted_reward = self.agent.getPredictionError()
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development state
        self.agent.updateDevelopment(0.1)
        
        # Collect statistics
        stats = self._collect_step_stats(step, percept, reward)
        
        return stats
    
    def _calculate_reward(self, percept) -> float:
        """Calculate reward based on world state and brain state."""
        reward = 0.0
        
        # Resource collection reward
        body = self.world.getAgentBody()
        for obj in body.getObjects():
            if obj.getType() == pynlm.WorldObjectType.Resource:
                reward += obj.getValue() * 0.1
        
        # Novelty reward (curiosity-driven)
        novelty = self.agent.getNoveltyLevel()
        if novelty > 0.5:
            reward += novelty * 0.2
        
        # Exploration reward (movement)
        if hasattr(self, '_last_position'):
            dx = self.world.getAgentX() - self._last_position[0]
            dy = self.world.getAgentY() - self._last_position[1]
            distance = np.sqrt(dx*dx + dy*dy)
            if distance > 0.1:
                reward += distance * 0.05
        
        self._last_position = (self.world.getAgentX(), self.world.getAgentY())
        
        # Prediction error reward (learning signal)
        prediction_error = self.agent.getPredictionError()
        reward += abs(prediction_error) * 0.1
        
        return reward
    
    def _collect_step_stats(self, step: int, percept, reward: float) -> Dict[str, Any]:
        """Collect statistics for current step."""
        stats = {
            "step": step,
            "timestamp": self.world.getSimulationTime(),
            "reward": reward,
            "memory_usage": self.brain.getMemoryUsage(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "activity_level": self.brain.getAverageActivity(),
            "curiosity": self.agent.getCuriosityLevel(),
            "novelty": self.agent.getNoveltyLevel(),
            "prediction_error": self.agent.getPredictionError(),
            "developmental_stage": self.brain.getDevelopmentalStage().name,
            "plasticity_rate": self.agent.getPlasticityRate(),
            "working_memory_size": len(self.brain.getWorkingMemory()),
            "episodic_memory_count": len(self.brain.getEpisodicMemory()),
            "association_patterns": self.brain.getAssociativeMemory().getNumPatterns()
        }
        
        # Update statistics
        self.simulation_stats["steps"] += 1
        self.simulation_stats["memory_stats"].append({
            "working_memory": stats["working_memory_size"],
            "episodic_memory": stats["episodic_memory_count"],
            "associative_patterns": stats["association_patterns"]
        })
        
        self.simulation_stats["neuromod_stats"].append({
            "curiosity": stats["curiosity"],
            "novelty": stats["novelty"],
            "prediction_error": stats["prediction_error"],
            "activity_level": stats["activity_level"]
        })
        
        self.simulation_stats["reward_stats"].append(stats["reward"])
        
        return stats
    
    def run_phase6_simulation(self, num_steps: int = 1000) -> Dict[str, Any]:
        """Run complete Phase 6 simulation."""
        print(f"\nStarting Phase 6 simulation: {num_steps} steps")
        print("=" * 50)
        
        start_time = time.time()
        
        for step in range(num_steps):
            # Run simulation step
            step_stats = self.run_simulation_step(step)
            
            # Print progress every 100 steps
            if step % 100 == 0:
                self._print_progress(step, step_stats)
        
        end_time = time.time()
        
        # Calculate final statistics
        final_stats = self._calculate_final_statistics(end_time - start_time)
        
        print("\n" + "=" * 50)
        print("Phase 6 Simulation Complete!")
        self._print_final_results(final_stats)
        
        return final_stats
    
    def _print_progress(self, step: int, stats: Dict[str, Any]):
        """Print progress information."""
        print(f"\nStep {step} ({(step/num_steps)*100:.1f}% complete):")
        print(f"  Activity: {stats['activity_level']:.3f}")
        print(f"  Curiosity: {stats['curiosity']:.3f}")
        print(f"  Novelty: {stats['novelty']:.3f}")
        print(f"  Reward: {stats['reward']:.3f}")
        print(f"  Dev Stage: {stats['developmental_stage']}")
        print(f"  Working Memory: {stats['working_memory_size']}/{len(self.brain.getWorkingMemory())}")
    
    def _calculate_final_statistics(self, duration: float) -> Dict[str, Any]:
        """Calculate final simulation statistics."""
        stats = {
            "duration_seconds": duration,
            "steps_per_second": self.simulation_stats["steps"] / duration,
            "average_activity": np.mean([s["activity_level"] for s in self.simulation_stats["neuromod_stats"]]),
            "average_curiosity": np.mean([s["curiosity"] for s in self.simulation_stats["neuromod_stats"]]),
            "average_novelty": np.mean([s["novelty"] for s in self.simulation_stats["neuromod_stats"]]),
            "total_reward": np.sum(self.simulation_stats["reward_stats"]),
            "peak_activity": max(s["activity_level"] for s in self.simulation_stats["neuromod_stats"]),
            "final_development_stage": self.brain.getDevelopmentalStage().name,
            "final_plasticity_rate": self.agent.getPlasticityRate(),
            "final_memory_patterns": self.brain.getAssociativeMemory().getNumPatterns(),
            "integration_success": self._check_integration_success()
        }
        
        return stats
    
    def _check_integration_success(self) -> bool:
        """Check if all Phase 6 systems are properly integrated."""
        # Check if all key systems are active
        checks = [
            self.agent.getCuriosityLevel() > 0.1,
            self.agent.getNoveltyLevel() > 0.1,
            self.agent.getPredictionError() != 0,
            self.brain.getDevelopmentalStage().name != "Initial",
            self.brain.getMemoryUsage() > 1000,
            self.brain.getAssociativeMemory().getNumPatterns() > 0
        ]
        
        return all(checks)
    
    def _print_final_results(self, stats: Dict[str, Any]):
        """Print final simulation results."""
        print("\nFINAL RESULTS:")
        print(f"✓ Simulation completed in {stats['duration_seconds']:.2f} seconds")
        print(f"✓ Performance: {stats['steps_per_second']:.1f} steps/second")
        print(f"✓ Average brain activity: {stats['average_activity']:.3f}")
        print(f"✓ Average curiosity level: {stats['average_curiosity']:.3f}")
        print(f"✓ Average novelty level: {stats['average_novelty']:.3f}")
        print(f"✓ Total reward accumulated: {stats['total_reward']:.2f}")
        print(f"✓ Peak activity achieved: {stats['peak_activity']:.3f}")
        print(f"✓ Final developmental stage: {stats['final_development_stage']}")
        print(f"✓ Final plasticity rate: {stats['final_plasticity_rate']:.3f}")
        print(f"✓ Final memory patterns: {stats['final_memory_patterns']}")
        print(f"✓ Integration success: {'✓ YES' if stats['integration_success'] else '✗ NO'}")
        
        if stats['integration_success']:
            print("\n🎉 ALL PHASE 6 SYSTEMS SUCCESSFULLY INTEGRATED!")
            print("The NLM brain is functioning as a coherent artificial intelligence system.")
        else:
            print("\n⚠️  Some Phase 6 systems may not be fully integrated.")
            print("Further tuning may be required for optimal integration.")

# Run the demonstration
if __name__ == "__main__":
    print("🚀 NLM Phase 6 Integration Example")
    print("==================================")
    
    # Create and run Phase 6 simulation
    simulation = Phase6BrainSimulation()
    results = simulation.run_phase6_simulation(1000)
    
    print(f"\nSimulation completed with {results['integration_success']} integration success.")