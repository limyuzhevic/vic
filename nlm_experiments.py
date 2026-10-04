#!/usr/bin/env python3
"""
NLM Experimental Features and Examples

Collection of experimental features, examples, and demonstrations for NLM (Neural Learning Machine).

This script provides various experimental examples including:
- Development stage experiments
- Neuromodulation studies
- Network plasticity experiments
- Environment interaction studies
- Advanced learning algorithm demonstrations
- Visualization and analysis tools

Usage: python nlm_experiments.py <command> [options]
"""

import argparse
import sys
import time
import json
import numpy as np
from typing import Dict, List, Any, Optional

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some experimental features will be limited.", file=sys.stderr)

class ExperimentRunner:
    """Base class for running NLM experiments."""
    
    def __init__(self, name: str):
        self.name = name
        self.results = {}
        self.start_time = None
        self.end_time = None
    
    def run(self) -> Dict[str, Any]:
        """Run the experiment and return results."""
        self.start_time = time.time()
        try:
            self._run_experiment()
            self.end_time = time.time()
            return self._get_results()
        except Exception as e:
            return {
                'experiment': self.name,
                'status': 'error',
                'error': str(e),
                'start_time': self.start_time,
                'end_time': time.time() if self.end_time is None else self.end_time
            }
    
    def _run_experiment(self):
        """Implement the experiment logic (to be overridden)."""
        raise NotImplementedError("Subclasses must implement _run_experiment")
    
    def _get_results(self) -> Dict[str, Any]:
        """Collect and return experiment results."""
        duration = (self.end_time or time.time()) - self.start_time
        
        return {
            'experiment': self.name,
            'status': 'success',
            'start_time': self.start_time,
            'end_time': self.end_time or time.time(),
            'duration_seconds': duration,
            'results': self.results,
            'timestamp': time.time()
        }

class DevelopmentExperiment(ExperimentRunner):
    """Experiment focusing on developmental stages and plasticity."""
    
    def __init__(self, name: str, config: Dict):
        super().__init__(name)
        self.config = config
        self.development_stages = []
        self.plasticity_metrics = []
        self.development_events = []
    
    def _run_experiment(self):
        """Run developmental stages experiment."""
        # Create brain with developmental configuration
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Configure for developmental experiments
        config = brain.getConfig()
        config.set("brain.neuron_count", self.config.get('neuron_count', 500))
        config.set("brain.connection_probability", self.config.get('connection_prob', 0.1))
        
        # Enable all plasticity mechanisms
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        agent.initialize(world)
        
        # Enable all plasticity subsystems
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run developmental simulation
        stages_data = []
        for step in range(self.config.get('total_steps', 1000)):
            world.update(0.1)
            
            # Get developmental stage
            stage = brain.getDevelopmentalStage()
            self.development_stages.append(stage)
            
            # Record plasticity metrics
            plasticity_metric = {
                'step': step,
                'stage': stage,
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'dopamine_level': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError()
            }
            self.plasticity_metrics.append(plasticity_metric)
            
            # Process sensory input and act
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and development updates
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, agent.getPredictionError())
            agent.updateDevelopment(0.1)
            
            # Record developmental events
            if step % 100 == 0:
                self.development_events.append({
                    'step': step,
                    'stage': stage,
                    'milestone': self._check_developmental_milestone(brain, agent)
                })
        
        self.results = {
            'brain': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'region_count': brain.getRegionCount()
            },
            'development_summary': {
                'stages_visited': list(set(self.development_stages)),
                'final_stage': self.development_stages[-1] if self.development_stages else None,
                'stage_transitions': self._count_stage_transitions(self.development_stages)
            },
            'plasticity_metrics': self.plasticity_metrics,
            'development_events': self.development_events
        }
    
    def _check_developmental_milestone(self, brain, agent) -> str:
        """Check for developmental milestones."""
        milestones = []
        
        if brain.getTotalSynapseCount() > 100:
            milestones.append("high_synaptogenesis")
        
        if agent.getCuriosityLevel() > 0.5:
            milestones.append("high_curiosity")
        
        if agent.getNoveltyLevel() > 0.3:
            milestones.append("high_novelty")
        
        if agent.getNeuromodulationLevel() > 0.5:
            milestones.append("high_dopamine")
        
        if brain.getExcitationInhibitionRatio() > 1.5:
            milestones.append("high_excitatory")
        
        if brain.getExcitationInhibitionRatio() < 0.5:
            milestones.append("high_inhibitory")
        
        return "|".join(milestones) if milestones else "none"
    
    def _count_stage_transitions(self, stages: List) -> Dict[str, int]:
        """Count transitions between developmental stages."""
        transitions = {}
        if not stages:
            return transitions
        
        current_stage = stages[0]
        stage_changes = {}
        
        for stage in stages[1:]:
            if stage != current_stage:
                stage_changes[current_stage] = stage_changes.get(current_stage, 0) + 1
                current_stage = stage
        
        return stage_changes

class NeuromodulationExperiment(ExperimentRunner):
    """Experiment focusing on neuromodulation and learning."""
    
    def __init__(self, name: str, config: Dict):
        super().__init__(name)
        self.config = config
        self.neuromodulation_data = []
        self.learning_curves = []
    
    def _run_experiment(self):
        """Run neuromodulation experiment."""
        # Create brain with neuromodulation focus
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", self.config.get('neuron_count', 300))
        config.set("plasticity.stdp.learning_rate", self.config.get('stdp_rate', 0.01))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        agent.initialize(world)
        
        # Configure neuromodulation systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run neuromodulation study
        neuromod_data = []
        learning_curve = []
        
        for step in range(self.config.get('total_steps', 500)):
            world.update(0.1)
            
            # Collect neuromodulation metrics
            neuromod_metric = {
                'step': step,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dopamine': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError(),
                'reward': world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            }
            neuromod_data.append(neuromod_metric)
            
            # Learning progress metric
            learning_progress = {
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'novelty_contribution': agent.getNoveltyLevel() * agent.getCuriosityLevel()
            }
            learning_curve.append(learning_progress)
            
            # Simulation steps
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = neuromod_metric['reward']
            pred_error = neuromod_metric['prediction_error']
            agent.applyRewardModulation(reward, pred_error)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        self.results = {
            'neuromodulation_summary': {
                'final_curiosity': neuromod_data[-1]['curiosity'] if neuromod_data else 0,
                'final_novelty': neuromod_data[-1]['novelty'] if neuromod_data else 0,
                'final_dopamine': neuromod_data[-1]['dopamine'] if neuromod_data else 0,
                'peak_curiosity': max(m['curiosity'] for m in neuromod_data) if neuromod_data else 0,
                'peak_novelty': max(m['novelty'] for m in neuromod_data) if neuromod_data else 0
            },
            'learning_dynamics': {
                'final_firing_rate': learning_curve[-1]['firing_rate'] if learning_curve else 0,
                'learning_efficiency': self._calculate_learning_efficiency(learning_curve),
                'stability_score': self._calculate_stability(neuromod_data)
            },
            'neuromodulation_data': neuromod_data,
            'learning_curve': learning_curve
        }
    
    def _calculate_learning_efficiency(self, learning_curve: List[Dict]) -> float:
        """Calculate learning efficiency based on performance improvement."""
        if len(learning_curve) < 2:
            return 0.0
        
        # Calculate improvements in key metrics
        firing_rates = [lc['firing_rate'] for lc in learning_curve]
        active_neurons = [lc['active_neurons'] for lc in learning_curve]
        spikes = [lc['total_spikes'] for lc in learning_curve]
        
        # Calculate changes over time
        firing_change = (firing_rates[-1] - firing_rates[0]) / (firing_rates[0] + 0.001)
        active_change = (active_neurons[-1] - active_neurons[0]) / (active_neurons[0] + 0.001)
        spike_change = (spikes[-1] - spikes[0]) / (spikes[0] + 0.001)
        
        # Combine into efficiency score (higher is better)
        efficiency = (firing_change + active_change + spike_change) / 3
        
        return max(-1.0, min(1.0, efficiency)) * 100  # Convert to percentage
    
    def _calculate_stability(self, neuromod_data: List[Dict]) -> float:
        """Calculate stability of neuromodulation levels."""
        if not neuromod_data:
            return 0.0
        
        curiosity = [m['curiosity'] for m in neuromod_data]
        novelty = [m['novelty'] for m in neuromod_data]
        dopamine = [m['dopamine'] for m in neuromod_data]
        
        # Calculate coefficient of variation (lower is more stable)
        curiosity_cv = np.std(curiosity) / (np.mean(curiosity) + 0.001)
        novelty_cv = np.std(novelty) / (np.mean(novelty) + 0.001)
        dopamine_cv = np.std(dopamine) / (np.mean(dopamine) + 0.001)
        
        # Stability score (higher is more stable)
        stability = (1.0 / (1.0 + curiosity_cv) + 
                    1.0 / (1.0 + novelty_cv) + 
                    1.0 / (1.0 + dopamine_cv)) / 3 * 100
        
        return stability

class PlasticityExperiment(ExperimentRunner):
    """Experiment focusing on different plasticity mechanisms."""
    
    def __init__(self, name: str, config: Dict):
        super().__init__(name)
        self.config = config
        self.plasticity_effects = []
        self.synapse_analysis = []
    
    def _run_experiment(self):
        """Run plasticity experiment."""
        # Create brain with plasticity focus
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", self.config.get('neuron_count', 400))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable specific plasticity mechanisms
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=18, height=18, visionWidth=7, visionHeight=7)
        world.reset()
        world.setAgentStart(9.0, 9.0)
        agent.initialize(world)
        
        # Configure plasticity
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Run plasticity experiment
        plasticity_effects = []
        synapse_analysis = []
        
        for step in range(self.config.get('total_steps', 800)):
            world.update(0.1)
            
            # Collect plasticity metrics
            plasticity_metric = {
                'step': step,
                'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
                'development_enabled': agent.isDevelopmentEnabled(),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'average_synaptic_weight': brain.getAverageSynapticWeight()
            }
            plasticity_effects.append(plasticity_metric)
            
            # Analyze synapse dynamics
            synapse_data = self._analyze_synapse_dynamics(brain)
            synapse_analysis.append(synapse_data)
            
            # Simulation steps
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
        
        self.results = {
            'plasticity_summary': {
                'structural_plasticity_duration': len([m for m in plasticity_effects if m['structural_plasticity_enabled']]),
                'development_duration': len([m for m in plasticity_effects if m['development_enabled']]),
                'final_synapse_count': plasticity_effects[-1]['synapse_count'] if plasticity_effects else 0,
                'synapse_change_rate': self._calculate_synapse_change_rate(plasticity_effects),
                'final_synaptic_weight': plasticity_effects[-1]['average_synaptic_weight'] if plasticity_effects else 0
            },
            'plasticity_effects': plasticity_effects,
            'synapse_analysis': synapse_analysis
        }
    
    def _analyze_synapse_dynamics(self, brain) -> Dict[str, Any]:
        """Analyze synapse dynamics and properties."""
        synapse_data = {
            'step': self.current_step,
            'total_synapses': brain.getTotalSynapseCount(),
            'average_weight': brain.getAverageSynapticWeight(),
            'weight_variance': self._calculate_weight_variance(brain),
            'excitatory_synapses': 0,  # Would need to count by type
            'inhibitory_synapses': 0,  # Would need to count by type
            'plasticity_events': 0  # Would need to count plasticity events
        }
        
        return synapse_data
    
    def _calculate_weight_variance(self, brain) -> float:
        """Calculate variance of synaptic weights."""
        # This would require access to individual synapse weights
        # For now, return a placeholder calculation
        return 0.1
    
    def _calculate_synapse_change_rate(self, plasticity_effects: List[Dict]) -> float:
        """Calculate rate of synapse formation/pruning."""
        if len(plasticity_effects) < 2:
            return 0.0
        
        synapse_counts = [eff['synapse_count'] for eff in plasticity_effects]
        
        # Calculate average change rate
        changes = []
        for i in range(1, len(synapse_counts)):
            change = synapse_counts[i] - synapse_counts[i-1]
            changes.append(change)
        
        if not changes:
            return 0.0
        
        avg_change = np.mean(changes)
        return avg_change

class EnvironmentInteractionExperiment(ExperimentRunner):
    """Experiment focusing on environment interaction and learning."""
    
    def __init__(self, name: str, config: Dict):
        super().__init__(name)
        self.config = config
        self.environment_interactions = []
        self.behavioral_patterns = []
    
    def _run_experiment(self):
        """Run environment interaction experiment."""
        # Create agent with world
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", self.config.get('neuron_count', 250))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=self.config.get('world_width', 12), 
                       height=self.config.get('world_height', 12),
                       visionWidth=self.config.get('vision_width', 5),
                       visionHeight=self.config.get('vision_height', 5))
        world.reset()
        world.setAgentStart(self.config.get('start_x', 6.0), 
                          self.config.get('start_y', 6.0))
        agent.initialize(world)
        
        # Configure for environment interaction
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run environment interaction experiment
        interactions = []
        behaviors = []
        
        for step in range(self.config.get('total_steps', 600)):
            world.update(0.1)
            
            # Record environment state and agent response
            environment_state = {
                'step': step,
                'world_time': world.getSimulationTime(),
                'agent_x': world.getAgentBody().x,
                'agent_y': world.getAgentBody().y,
                'agent_orientation': world.getAgentBody().orientation,
                'vision_objects': self._count_vision_objects(world),
                'rewards_received': self._get_recent_rewards(world),
                'actions_taken': self._get_action_history(agent)
            }
            interactions.append(environment_state)
            
            # Analyze behavioral patterns
            behavior = {
                'step': step,
                'movement_pattern': self._analyze_movement_pattern(interactions),
                'exploration_vs_exploitation': self._calculate_exploration_exploitation(agent),
                'decision_confidence': self._calculate_decision_confidence(agent),
                'environmental_sensitivity': self._calculate_environmental_sensitivity(interactions)
            }
            behaviors.append(behavior)
            
            # Simulation steps
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            pred_error = agent.getPredictionError()
            agent.applyRewardModulation(reward, pred_error)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        self.results = {
            'environment_summary': {
                'final_position': {
                    'x': interactions[-1]['agent_x'] if interactions else 0,
                    'y': interactions[-1]['agent_y'] if interactions else 0
                },
                'total_steps': len(interactions),
                'environment_complexity': self._calculate_environment_complexity(interactions),
                'behavioral_stability': self._calculate_behavioral_stability(behaviors)
            },
            'environment_interactions': interactions,
            'behavioral_patterns': behaviors
        }
    
    def _count_vision_objects(self, world) -> int:
        """Count visible objects in agent's vision."""
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        # Count non-zero pixels as objects
        return np.count_nonzero(vision) if vision else 0
    
    def _get_recent_rewards(self, world) -> List[float]:
        """Get recent rewards from environment."""
        # This would need to track reward history
        return [0.0]  # Placeholder
    
    def _get_action_history(self, agent) -> List[str]:
        """Get recent action history."""
        # This would need to track action history
        return ["move_forward"]  # Placeholder
    
    def _analyze_movement_pattern(self, interactions: List[Dict]) -> Dict[str, float]:
        """Analyze movement patterns."""
        if not interactions:
            return {'movement_entropy': 0.0, 'directionality': 0.0}
        
        # Calculate movement entropy and directionality
        movements = [self._calculate_movement_from_interaction(interactions[i], interactions[i+1])
                    for i in range(len(interactions)-1)]
        
        if not movements:
            return {'movement_entropy': 0.0, 'directionality': 0.0}
        
        # Calculate entropy
        unique_movements = set(movements)
        entropy = -sum((movements.count(m) / len(movements)) * 
                      np.log(movements.count(m) / len(movements)) 
                      for m in unique_movements)
        
        # Calculate directionality (straightness of path)
        positions = [(inter['agent_x'], inter['agent_y']) for inter in interactions]
        if len(positions) < 2:
            return {'movement_entropy': entropy, 'directionality': 0.0}
        
        # Calculate average turning angle
        angles = []
        for i in range(len(positions)-2):
            angle1 = np.arctan2(positions[i+1][1] - positions[i][1], 
                             positions[i+1][0] - positions[i][0])
            angle2 = np.arctan2(positions[i+2][1] - positions[i+1][1], 
                             positions[i+2][0] - positions[i+1][0])
            angle_diff = abs(angle1 - angle2)
            angles.append(angle_diff)
        
        directionality = 1.0 / (1.0 + np.mean(angles)) if angles else 0.0
        
        return {'movement_entropy': entropy, 'directionality': directionality}
    
    def _calculate_movement_from_interaction(self, interaction1: Dict, interaction2: Dict) -> str:
        """Calculate movement from two interactions."""
        dx = interaction2['agent_x'] - interaction1['agent_x']
        dy = interaction2['agent_y'] - interaction1['agent_y']
        
        if abs(dx) > abs(dy):
            return 'right' if dx > 0 else 'left'
        else:
            return 'up' if dy > 0 else 'down'
    
    def _calculate_exploration_exploitation(self, agent) -> Dict[str, float]:
        """Calculate exploration vs exploitation balance."""
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        exploration_score = curiosity * novelty
        exploitation_score = 1.0 - exploration_score
        
        return {
            'exploration_ratio': exploration_score,
            'exploitation_ratio': exploitation_score,
            'curiosity_balance': min(1.0, curiosity * 2)
        }
    
    def _calculate_decision_confidence(self, agent) -> float:
        """Calculate decision confidence."""
        # This would require access to agent's decision confidence metrics
        return 0.5  # Placeholder
    
    def _calculate_environmental_sensitivity(self, interactions: List[Dict]) -> float:
        """Calculate sensitivity to environmental changes."""
        if len(interactions) < 2:
            return 0.0
        
        # Calculate changes in behavior based on environment changes
        sensitivity_changes = []
        for i in range(1, len(interactions)):
            prev_state = interactions[i-1]
            curr_state = interactions[i]
            
            state_change = abs(curr_state['vision_objects'] - prev_state['vision_objects'])
            position_change = ((curr_state['agent_x'] - prev_state['agent_x'])**2 + 
                             (curr_state['agent_y'] - prev_state['agent_y'])**2)**0.5
            
            sensitivity = state_change / (position_change + 0.001)
            sensitivity_changes.append(sensitivity)
        
        return np.mean(sensitivity_changes) if sensitivity_changes else 0.0
    
    def _calculate_environment_complexity(self, interactions: List[Dict]) -> float:
        """Calculate environment complexity."""
        if not interactions:
            return 0.0
        
        # Calculate based on object diversity and movement patterns
        object_counts = [inter['vision_objects'] for inter in interactions]
        position_changes = [((inter['agent_x'] - interactions[0]['agent_x'])**2 + 
                            (inter['agent_y'] - interactions[0]['agent_y'])**2)**0.5)
                          for inter in interactions]
        
        object_variance = np.var(object_counts) if len(object_counts) > 1 else 0
        movement_variance = np.var(position_changes) if len(position_changes) > 1 else 0
        
        complexity = (object_variance + movement_variance) / 2
        return min(complexity, 10.0)  # Cap complexity score
    
    def _calculate_behavioral_stability(self, behaviors: List[Dict]) -> float:
        """Calculate behavioral stability."""
        if not behaviors:
            return 0.0
        
        # Calculate stability based on consistency of behavioral metrics
        exploration_scores = [b['exploration_vs_exploitation']['exploration_ratio'] 
                             for b in behaviors]
        directionality_scores = [b['movement_pattern']['directionality'] 
                               for b in behaviors]
        
        exploration_stability = 1.0 / (1.0 + np.std(exploration_scores))
        directionality_stability = 1.0 / (1.0 + np.std(directionality_scores))
        
        return (exploration_stability + directionality_stability) / 2

def run_development_experiment():
    """Command to run development experiment."""
    parser = argparse.ArgumentParser(description='Run NLM development experiment')
    parser.add_argument('--config', type=str, default='config.json', 
                       help='Configuration file path')
    parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for experiment results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Load configuration
        if args.config and Path(args.config).exists():
            with open(args.config, 'r') as f:
                config = json.load(f)
        else:
            config = {
                'neuron_count': 500,
                'connection_prob': 0.1,
                'total_steps': args.steps
            }
        
        # Run experiment
        experiment = DevelopmentExperiment('Development Stage Experiment', config)
        results = experiment.run()
        
        # Save results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Development experiment results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def run_neuromodulation_experiment():
    """Command to run neuromodulation experiment."""
    parser = argparse.ArgumentParser(description='Run NLM neuromodulation experiment')
    parser.add_argument('--config', type=str, default='config.json',
                       help='Configuration file path')
    parser.add_argument('--steps', type=int, default=500, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for experiment results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Load configuration
        if args.config and Path(args.config).exists():
            with open(args.config, 'r') as f:
                config = json.load(f)
        else:
            config = {
                'neuron_count': 300,
                'stdp_rate': 0.01,
                'total_steps': args.steps
            }
        
        # Run experiment
        experiment = NeuromodulationExperiment('Neuromodulation Experiment', config)
        results = experiment.run()
        
        # Save results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Neuromodulation experiment results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def run_plasticity_experiment():
    """Command to run plasticity experiment."""
    parser = argparse.ArgumentParser(description='Run NLM plasticity experiment')
    parser.add_argument('--config', type=str, default='config.json',
                       help='Configuration file path')
    parser.add_argument('--steps', type=int, default=800, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for experiment results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Load configuration
        if args.config and Path(args.config).exists():
            with open(args.config, 'r') as f:
                config = json.load(f)
        else:
            config = {
                'neuron_count': 400,
                'total_steps': args.steps
            }
        
        # Run experiment
        experiment = PlasticityExperiment('Plasticity Dynamics Experiment', config)
        results = experiment.run()
        
        # Save results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Plasticity experiment results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def run_environment_experiment():
    """Command to run environment interaction experiment."""
    parser = argparse.ArgumentParser(description='Run NLM environment interaction experiment')
    parser.add_argument('--config', type=str, default='config.json',
                       help='Configuration file path')
    parser.add_argument('--steps', type=int, default=600, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for experiment results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Load configuration
        if args.config and Path(args.config).exists():
            with open(args.config, 'r') as f:
                config = json.load(f)
        else:
            config = {
                'neuron_count': 250,
                'world_width': 12,
                'world_height': 12,
                'vision_width': 5,
                'vision_height': 5,
                'start_x': 6.0,
                'start_y': 6.0,
                'total_steps': args.steps
            }
        
        # Run experiment
        experiment = EnvironmentInteractionExperiment('Environment Interaction Experiment', config)
        results = experiment.run()
        
        # Save results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Environment experiment results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description='NLM Experimental Features and Examples')
    subparsers = parser.add_subparsers(dest='command', help='Available experiments')
    
    # Create subparsers for different experiments
    dev_parser = subparsers.add_parser('development', help='Run development stage experiment')
    dev_parser.set_defaults(func=run_development_experiment)
    
    neuromod_parser = subparsers.add_parser('neuromodulation', help='Run neuromodulation experiment')
    neuromod_parser.set_defaults(func=run_neuromodulation_experiment)
    
    plasticity_parser = subparsers.add_parser('plasticity', help='Run plasticity dynamics experiment')
    plasticity_parser.set_defaults(func=run_plasticity_experiment)
    
    env_parser = subparsers.add_parser('environment', help='Run environment interaction experiment')
    env_parser.set_defaults(func=run_environment_experiment)
    
    # If no command provided, show help
    if len(sys.argv) == 1:
        parser.print_help()
        return 1
    
    try:
        return subparsers.choices[sys.argv[1]].func()
    except KeyError:
        print(f"Unknown command: {sys.argv[1]}", file=sys.stderr)
        return 1

if __name__ == '__main__':
    sys.exit(main())