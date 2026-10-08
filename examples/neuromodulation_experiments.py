import pynlm
import numpy as np
import time
from enum import Enum
from typing import Dict, List, Tuple
from collections import defaultdict

class NeuromodulationExperiment:
    """
    Comprehensive neuromodulation experiments exploring the interaction between
    dopamine, curiosity, novelty, and prediction error systems.
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize neuromodulation experiment.
        
        Args:
            brain: The brain instance to experiment with
        """
        self.brain = brain
        self.config = brain.getConfig()
        
        # Experimental parameters
        self.experiment_phase = "baseline"
        self.current_step = 0
        self.max_steps = 2000
        
        # Neuromodulation tracking
        self.neuromodulation_data = {
            "dopamine": [],
            "curiosity": [],
            "novelty": [],
            "prediction_error": [],
            "reward_history": [],
            "behavior_history": []
        }
        
        # Experimental conditions
        self.conditions = {
            "baseline": {"dopamine_gain": 1.0, "curiosity_threshold": 0.5, "novelty_sensitivity": 1.0},
            "high_dopamine": {"dopamine_gain": 2.5, "curiosity_threshold": 0.3, "novelty_sensitivity": 1.5},
            "high_curiosity": {"dopamine_gain": 1.0, "curiosity_threshold": 0.1, "novelty_sensitivity": 0.8},
            "high_novelty": {"dopamine_gain": 1.5, "curiosity_threshold": 0.5, "novelty_sensitivity": 2.0}
        }
        
        # Behavioral patterns
        self.behavioral_patterns = {
            "exploitation": 0,
            "exploration": 0,
            "avoidance": 0,
            "curiosity_driven": 0
        }
    
    def run_complete_experiment(self):
        """Run complete neuromodulation experiment with multiple phases."""
        print("Starting Neuromodulation Experiment...")
        print(f"Total duration: {self.max_steps} simulation steps")
        
        # Initialize world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.setAgentStart(10, 10)
        
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Experimental phases
        phases = [
            ("baseline", 500, self.conditions["baseline"]),
            ("high_dopamine", 500, self.conditions["high_dopamine"]),
            ("high_curiosity", 500, self.conditions["high_curiosity"]),
            ("high_novelty", 500, self.conditions["high_novelty"])
        ]
        
        for phase_name, phase_duration, condition in phases:
            print(f"\n--- Phase: {phase_name.upper()} ---")
            print(f"Duration: {phase_duration} steps")
            print(f"Parameters: Dopamine gain={condition['dopamine_gain']}, "
                  f"Curiosity threshold={condition['curiosity_threshold']}, "
                  f"Novelty sensitivity={condition['novelty_sensitivity']}")
            
            # Set neuromodulation parameters
            self._set_neuromodulation_parameters(condition)
            
            # Run phase
            phase_data = self._run_phase(agent, world, phase_duration, phase_name)
            
            # Store results
            for key in self.neuromodulation_data:
                if key in phase_data:
                    self.neuromodulation_data[key].extend(phase_data[key])
        
        # Analyze and report results
        self._analyze_results()
        
        return self.neuromodulation_data
    
    def _set_neuromodulation_parameters(self, condition: Dict):
        """Set neuromodulation parameters based on experimental condition."""
        # In a real implementation, these would be passed to the neuromodulation systems
        # For this example, we track them and analyze their effects
        self.current_condition = condition
    
    def _run_phase(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                   duration: int, phase_name: str) -> Dict:
        """
        Run a single experimental phase.
        
        Args:
            agent: The agent brain interface
            world: The world environment
            duration: Phase duration in steps
            phase_name: Name of the experimental phase
        
        Returns:
            Dictionary with phase results
        """
        phase_data = {
            "dopamine": [],
            "curiosity": [],
            "novelty": [],
            "prediction_error": [],
            "reward_history": [],
            "behavior_history": []
        }
        
        for step in range(duration):
            # Update step counter
            self.current_step = step
            
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Track neuromodulation levels
            if self.brain.getDopamine():
                phase_data["dopamine"].append(self.brain.getDopamine().getCurrentLevel())
            if self.brain.getCuriosity():
                phase_data["curiosity"].append(self.brain.getCuriosity().getCurrentLevel())
            if self.brain.getNovelty():
                phase_data["novelty"].append(self.brain.getNovelty().getCurrentLevel())
            if self.brain.getPredictionErrorSignal():
                phase_data["prediction_error"].append(self.brain.getPredictionErrorSignal().getCurrentError())
            
            # Decode action and apply to world
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate context-dependent reward
            reward = self._calculate_contextual_reward(world, agent, percept)
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, agent.getPredictionError())
            
            # Track behavior
            behavior = self._classify_behavior(action, world, agent)
            phase_data["behavior_history"].append(behavior)
            phase_data["reward_history"].append(reward)
            
            # Record agent state
            if step % 100 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Dopamine={phase_data['dopamine'][-1]:.3f}, "
                      f"Curiosity={phase_data['curiosity'][-1]:.3f}, "
                      f"Novelty={phase_data['novelty'][-1]:.3f}")
        
        return phase_data
    
    def _calculate_contextual_reward(self, world: pynlm.SimpleWorld, 
                                    agent: pynlm.AgentBrain, 
                                    percept: pynlm.SensoryPercept) -> float:
        """
        Calculate context-dependent reward based on multiple factors.
        
        Args:
            world: Current world state
            agent: Agent brain interface
            percept: Current sensory percept
        
        Returns:
            Calculated reward value
        """
        agent_body = world.getAgentBody()
        reward = 0.0
        
        # Resource-based reward
        vision = percept.getVision()
        resource_locations = []
        for y in range(len(vision)):
            for x in range(len(vision[0])):
                if vision[y][x] == 5:  # Resource type
                    resource_locations.append((x, y))
        
        if resource_locations:
            closest = min(resource_locations,
                         key=lambda pos: (pos[0] - agent_body.x)**2 + (pos[1] - agent_body.y)**2)
            distance = ((closest[0] - agent_body.x)**2 + (closest[1] - agent_body.y)**2)**0.5
            resource_reward = 10.0 / (1.0 + distance)
            reward += resource_reward
        
        # Novelty-based reward (exploration bonus)
        if self.brain.getNovelty():
            novelty = self.brain.getNovelty().getCurrentLevel()
            if novelty > 0.7:  # High novelty bonus
                reward += novelty * 5.0
        
        # Development-based reward
        if self.brain.getDevelopmentSystem():
            developmental_progress = self.brain.getDevelopmentalStage()
            reward += float(developmental_progress) * 0.5
        
        # Curiosity-based exploration bonus
        if self.brain.getCuriosity():
            curiosity = self.brain.getCuriosity().getCurrentLevel()
            if curiosity > 0.6:
                reward += curiosity * 3.0
        
        # Energy-based cost
        energy_cost = agent_body.energy * 0.1
        reward -= energy_cost
        
        # Danger avoidance penalty
        if agent_body.health < 50:
            reward -= 2.0
        
        # Prediction error bonus
        if self.brain.getPredictionErrorSignal():
            prediction_error = abs(self.brain.getPredictionErrorSignal().getCurrentError())
            if prediction_error > 0.5:
                reward += prediction_error * 2.0
        
        return reward
    
    def _classify_behavior(self, action: pynlm.MotorCommand, 
                          world: pynlm.SimpleWorld, 
                          agent: pynlm.AgentBrain) -> str:
        """
        Classify behavioral pattern based on action and context.
        
        Args:
            action: Motor command action
            world: Current world state
            agent: Agent brain interface
        
        Returns:
            Behavior classification
        """
        agent_body = world.getAgentBody()
        
        # Simple behavior classification
        if action == pynlm.MotorCommand.MoveForward or action == pynlm.MotorCommand.MoveRight:
            return "exploitation"
        elif action == pynlm.MotorCommand.TurnLeft or action == pynlm.MotorCommand.TurnRight:
            return "exploration"
        elif action == pynlm.MotorCommand.Look or action == pynlm.MotorCommand.LookUp:
            return "curiosity_driven"
        elif action == pynlm.MotorCommand.Interact:
            return "exploitation"
        else:
            return "avoidance"
    
    def _analyze_results(self):
        """Analyze and report neuromodulation experiment results."""
        print("\n=== NEUROMODULATION EXPERIMENT RESULTS ===\n")
        
        for phase_name in self.conditions.keys():
            phase_start = sum(p["duration"] for p in [
                ("baseline", 500), ("high_dopamine", 500), 
                ("high_curiosity", 500), ("high_novelty", 500)
            ][:list(self.conditions.keys()).index(phase_name)])
            phase_end = phase_start + 500
            
            # Extract phase-specific data
            phase_indices = slice(phase_start, phase_end)
            
            print(f"Phase: {phase_name.upper()}")
            print(f"  Dopamine Levels: {np.mean(self.neuromodulation_data['dopamine'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['dopamine'][phase_indices]):.3f}")
            print(f"  Curiosity Levels: {np.mean(self.neuromodulation_data['curiosity'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['curiosity'][phase_indices]):.3f}")
            print(f"  Novelty Levels: {np.mean(self.neuromodulation_data['novelty'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['novelty'][phase_indices]):.3f}")
            print(f"  Prediction Error: {np.mean(self.neuromodulation_data['prediction_error'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['prediction_error'][phase_indices]):.3f}")
            
            # Behavior distribution
            behaviors = self.neuromodulation_data['behavior_history'][phase_indices]
            behavior_counts = {}
            for behavior in behaviors:
                behavior_counts[behavior] = behavior_counts.get(behavior, 0) + 1
            
            print(f"  Behavior Distribution:")
            for behavior, count in behavior_counts.items():
                percentage = (count / len(behaviors)) * 100
                print(f"    {behavior}: {count} ({percentage:.1f}%)")
            
            print()
        
        print("=== KEY FINDINGS ===")
        
        # Analyze neuromodulation effectiveness
        dopamine_effectiveness = self._analyze_neuromodulator_effectiveness('dopamine')
        curiosity_effectiveness = self._analyze_neuromodulator_effectiveness('curiosity')
        novelty_effectiveness = self._analyze_neuromodulator_effectiveness('novelty')
        
        print(f"Dopamine effectiveness: {dopamine_effectiveness:.2%}")
        print(f"Curiosity effectiveness: {curiosity_effectiveness:.2%}")
        print(f"Novelty effectiveness: {novelty_effectiveness:.2%}")
        
        # Determine optimal configuration
        optimal_config = self._determine_optimal_config()
        print(f"\nOptimal neuromodulation configuration:")
        for key, value in optimal_config.items():
            print(f"  {key}: {value}")
    
    def _analyze_neuromodulator_effectiveness(self, neuromodulator: str) -> float:
        """
        Calculate effectiveness metric for a neuromodulator.
        
        Args:
            neuromodulator: Name of neuromodulator ('dopamine', 'curiosity', or 'novelty')
        
        Returns:
            Effectiveness metric (0-1)
        """
        if not self.neuromodulation_data[neuromodulator]:
            return 0.0
        
        # Effectiveness defined as the ability to modulate behavior meaningfully
        # Calculated as the ratio of behavioral change to neuromodulator level
        data = self.neuromodulation_data[neuromodulator]
        mean_level = np.mean(data)
        level_variability = np.std(data)
        
        # Higher variability indicates better modulation capability
        effectiveness = min(level_variability / mean_level, 1.0) if mean_level > 0 else 0.0
        return effectiveness
    
    def _determine_optimal_config(self) -> Dict:
        """
        Determine optimal neuromodulation configuration based on experiment results.
        
        Returns:
            Dictionary with optimal configuration parameters
        """
        # Simple heuristic: find configuration with best balance of all neuromodulators
        optimal_config = self.conditions["baseline"].copy()  # Start with baseline
        
        # Evaluate each configuration
        for config_name, config_params in self.conditions.items():
            # Calculate balance score (higher is better)
            dopamine_balance = 1.0 / (1.0 + abs(config_params['dopamine_gain'] - 1.0))
            curiosity_balance = 1.0 / (1.0 + abs(config_params['curiosity_threshold'] - 0.5))
            novelty_balance = 1.0 / (1.0 + abs(config_params['novelty_sensitivity'] - 1.0))
            
            balance_score = (dopamine_balance + curiosity_balance + novelty_balance) / 3.0
            
            # Update optimal if this is better
            if config_name == "high_dopamine" or balance_score > 0.7:
                optimal_config = config_params.copy()
        
        return optimal_config

if __name__ == "__main__":
    # Create experiment configuration
    config = pynlm.createDefaultConfig()
    
    # Enable comprehensive neuromodulation systems
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.enable_novelty", True)
    config.set("neuromodulation.enable_prediction_error", True)
    config.set("brain.region_count", 5)
    config.set("brain.neurons_per_region", 100)
    config.set("memory.working_memory_size", 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run neuromodulation experiment
    experiment = NeuromodulationExperiment(brain)
    results = experiment.run_complete_experiment()
