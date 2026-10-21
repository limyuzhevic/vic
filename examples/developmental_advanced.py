"""
Advanced Development Systems Examples

This module demonstrates advanced developmental processes in NLM.
Key features include:
- Custom developmental stages
- Developmental plasticity modulation
- Learning and maturation processes
- Age-dependent behavior changes
- Developmental trajectory modeling

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import numpy as np
from typing import List, Dict, Any
import time
class DevelopmentalStage:
    """
    Custom developmental stage with specific characteristics.
    
    Each developmental stage has:
    - Name and description
    - Duration in simulation steps
    - Plasticity factor (0.0 to 1.0)
    - Learning rate modifier
    - Behavior characteristics
    - Environmental response patterns
    """
    
    def __init__(self, name: str, plasticity: float, duration: int,
                 learning_rate: float = 1.0, behavior_modifiers: Dict[str, float] = None):
        self.name = name
        self.plasticity = plasticity
        self.duration = duration
        self.learning_rate = learning_rate
        self.behavior_modifiers = behavior_modifiers or {}
        self.progress = 0
        self.start_time = time.time()
        
        # Validate plasticity
        if not 0.0 <= plasticity <= 1.0:
            raise ValueError(f"Plasticity must be between 0.0 and 1.0, got {plasticity}")
            
    def updateProgress(self, time_step: float):
        """Update stage progress."""
        self.progress += time_step
        
    def isComplete(self) -> bool:
        """Check if stage is complete."""
        return self.progress >= self.duration
        
    def getProgressRatio(self) -> float:
        """Get progress as ratio (0.0 to 1.0)."""
        return min(1.0, self.progress / self.duration)
class DevelopmentalEvent:
    """
    Developmental event or milestone.
    
    Developmental events include:
    - Critical period openings/closings
    - Behavioral transitions
    - Physiological changes
    - Learning capability changes
    """
    
    def __init__(self, event_type: str, stage_name: str, trigger_condition: Callable[[float], bool]):
        self.event_type = event_type
        self.stage_name = stage_name
        self.trigger_condition = trigger_condition
        self.activated = False
        self.activation_time = 0.0
        self.effects = {}
        
    def activate(self, current_time: float):
        """Activate the developmental event."""
        if not self.activated and self.trigger_condition(current_time):
            self.activated = True
            self.activation_time = current_time
            
    def addEffect(self, key: str, value: Any):
        """Add an effect of this event."""
        self.effects[key] = value
class AgeDependentParameters:
    """
    Age-dependent parameters for different life stages.
    
    This class manages:
    - Age-specific neural parameters
    - Developmental stage transitions
    - Age-dependent learning rates
    - Physiological changes
    """
    
    def __init__(self):
        self.age = 0.0
        self.current_stage = None
        self.stage_transitions = []
        self.age_specific_parameters = {}
        
    def addAgeSpecificParameter(self, age_range: tuple, parameters: Dict[str, Any]):
        """
        Add parameters for specific age range.
        
        Args:
            age_range: Tuple (min_age, max_age)
            parameters: Dictionary of parameters for this age range
        """
        self.age_specific_parameters[age_range] = parameters
        
    def getParametersForAge(self, age: float) -> Dict[str, Any]:
        """Get parameters for current age."""
        for age_range, parameters in self.age_specific_parameters.items():
            min_age, max_age = age_range
            if min_age <= age <= max_age:
                return parameters
                
        # Return default parameters if no match
        return {}
class AdvancedDevelopment:
    """
    Advanced developmental system with custom stages and processes.
    
    This system implements:
    - Custom developmental trajectories
    - Age-dependent plasticity
    - Behavioral development
    - Learning capability maturation
    - Physiological development
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        
        # Initialize developmental components
        self.developmental_stages = []
        self.current_stage_index = 0
        self.developmental_events = []
        self.age_parameters = AgeDependentParameters()
        
        # Development state
        self.age = 0.0
        self.development_time = 0.0
        self.maturity_level = 0.0
        self.learning_ability = 1.0
        self.behavior_stability = 0.0
        
        # Setup developmental stages
        self.setupDevelopmentalStages()
        
        # Setup developmental events
        self.setupDevelopmentalEvents()
        
        # Setup age-specific parameters
        self.setupAgeSpecificParameters()
        
    def setupDevelopmentalStages(self):
        """Setup custom developmental stages."""
        # Define custom developmental stages
        stages = [
            DevelopmentalStage(
                name="Infancy",
                plasticity=1.0,
                duration=200,
                learning_rate=1.5,
                behavior_modifiers={'exploration_rate': 0.8, 'risk_tolerance': 0.6}
            ),
            DevelopmentalStage(
                name="Early Learning",
                plasticity=0.8,
                duration=300,
                learning_rate=1.2,
                behavior_modifiers={'exploration_rate': 0.5, 'risk_tolerance': 0.4}
            ),
            DevelopmentalStage(
                name="Maturation",
                plasticity=0.5,
                duration=400,
                learning_rate=1.0,
                behavior_modifiers={'exploration_rate': 0.3, 'risk_tolerance': 0.2}
            ),
            DevelopmentalStage(
                name="Adulthood",
                plasticity=0.2,
                duration=600,
                learning_rate=0.8,
                behavior_modifiers={'exploration_rate': 0.1, 'risk_tolerance': 0.1}
            ),
            DevelopmentalStage(
                name="Aging",
                plasticity=0.1,
                duration=1000,
                learning_rate=0.6,
                behavior_modifiers={'exploration_rate': 0.05, 'risk_tolerance': 0.05}
            )
        ]
        
        self.developmental_stages = stages
        self.current_stage = stages[0] if stages else None
        
    def setupDevelopmentalEvents(self):
        """Setup developmental events and milestones."""
        events = [
            DevelopmentalEvent(
                event_type="critical_period_open",
                stage_name="Infancy",
                trigger_condition=lambda t: t > 50 and t < 100
            ),
            DevelopmentalEvent(
                event_type="critical_period_close",
                stage_name="Early Learning",
                trigger_condition=lambda t: t > 200 and t < 250
            ),
            DevelopmentalEvent(
                event_type="behavior_transition",
                stage_name="Maturation",
                trigger_condition=lambda t: t > 400 and t < 450
            ),
            DevelopmentalEvent(
                event_type="learning_capacity_change",
                stage_name="Adulthood",
                trigger_condition=lambda t: t > 800 and t < 850
            )
        ]
        
        self.developmental_events = events
        
    def setupAgeSpecificParameters(self):
        """Setup age-specific parameters."""
        # Neuron properties
        self.age_parameters.addAgeSpecificParameter(
            (0, 100),
            {
                'max_firing_rate': 50.0,
                'membrane_time_constant': 10.0,
                'plasticity_rate': 0.8,
                'learning_rate': 1.5
            }
        )
        
        self.age_parameters.addAgeSpecificParameter(
            (100, 300),
            {
                'max_firing_rate': 40.0,
                'membrane_time_constant': 15.0,
                'plasticity_rate': 0.6,
                'learning_rate': 1.2
            }
        )
        
        self.age_parameters.addAgeSpecificParameter(
            (300, 600),
            {
                'max_firing_rate': 30.0,
                'membrane_time_constant': 20.0,
                'plasticity_rate': 0.4,
                'learning_rate': 1.0
            }
        )
        
        self.age_parameters.addAgeSpecificParameter(
            (600, 1000),
            {
                'max_firing_rate': 25.0,
                'membrane_time_constant': 25.0,
                'plasticity_rate': 0.2,
                'learning_rate': 0.8
            }
        )
        
        self.age_parameters.addAgeSpecificParameter(
            (1000, 2000),
            {
                'max_firing_rate': 20.0,
                'membrane_time_constant': 30.0,
                'plasticity_rate': 0.1,
                'learning_rate': 0.6
            }
        )
        
    def update(self, time_step: float):
        """
        Update developmental processes.
        
        Args:
            time_step: Time step duration
        """
        # Update age
        self.age += time_step
        self.development_time += time_step
        
        # Update current developmental stage
        if self.current_stage:
            self.current_stage.updateProgress(time_step)
            
            # Check if stage is complete
            if self.current_stage.isComplete():
                self.transitionToNextStage()
        
        # Activate developmental events
        current_time = time.time() - self.development_time
        for event in self.developmental_events:
            event.activate(current_time)
            
        # Apply developmental effects
        self.applyDevelopmentalEffects()
        
        # Update age-specific parameters
        age_params = self.age_parameters.getParametersForAge(self.age)
        self.updateParametersBasedOnAge(age_params)
        
    def transitionToNextStage(self):
        """Transition to next developmental stage."""
        self.current_stage_index += 1
        
        if self.current_stage_index < len(self.developmental_stages):
            self.current_stage = self.developmental_stages[self.current_stage_index]
            print(f"Transitioned to developmental stage: {self.current_stage.name}")
            
            # Apply stage-specific effects
            for modifier, value in self.current_stage.behavior_modifiers.items():
                print(f"  {modifier}: {value}")
        else:
            self.current_stage = None
            print("Reached maximum developmental stage")
            
    def applyDevelopmentalEffects(self):
        """Apply developmental effects to brain systems."""
        if not self.current_stage:
            return
            
        # Update plasticity based on developmental stage
        plasticity_factor = self.current_stage.plasticity
        learning_rate_factor = self.current_stage.learning_rate
        
        # Modify brain plasticity
        if self.brain.getSTDP():
            # Adjust STDP learning rates
            current_ltp = self.brain.getSTDP()->getLTPWeight()
            current_ltd = self.brain.getSTDP()->getLTDWeight()
            
            # Scale learning rates
            self.brain.getSTDP()->setLTPWeight(current_ltp * learning_rate_factor)
            self.brain.getSTDP()->setLTDWeight(current_ltd * learning_rate_factor)
        
        # Modify behavior characteristics
        for modifier, value in self.current_stage.behavior_modifiers.items():
            if modifier == 'exploration_rate':
                # Modify exploration behavior
                pass
            elif modifier == 'risk_tolerance':
                # Modify risk tolerance
                pass
        
        # Update maturity
        self.maturity_level = min(1.0, self.development_time / 2000.0)
        self.learning_ability = self.current_stage.plasticity * self.current_stage.learning_rate
        self.behavior_stability = self.maturity_level
        
    def updateParametersBasedOnAge(self, age_params: Dict[str, Any]):
        """Update brain parameters based on age."""
        if not age_params:
            return
            
        # In a real implementation, this would update brain parameters
        # For now, we just log the changes
        print(f"Age {self.age:.1f}: Applied age-specific parameters:")
        for param, value in age_params.items():
            print(f"  {param}: {value}")
            
    def getDevelopmentalReport(self) -> Dict[str, Any]:
        """Get comprehensive developmental report."""
        report = {
            'current_age': self.age,
            'development_time': self.development_time,
            'current_stage': self.current_stage.name if self.current_stage else None,
            'stage_progress': self.current_stage.getProgressRatio() if self.current_stage else 0,
            'maturity_level': self.maturity_level,
            'learning_ability': self.learning_ability,
            'behavior_stability': self.behavior_stability,
            'plasticity_factor': self.current_stage.plasticity if self.current_stage else 0,
            'learning_rate_factor': self.current_stage.learning_rate if self.current_stage else 1
        }
        
        # Add event status
        event_status = {}
        for event in self.developmental_events:
            event_status[event.event_type] = {
                'activated': event.activated,
                'activation_time': event.activation_time,
                'effects': event.effects
            }
        report['developmental_events'] = event_status
        
        return report
    
    def runDevelopmentSimulation(self, duration: float, time_step: float = 0.01):
        """
        Run developmental simulation.
        
        Args:
            duration: Simulation duration
            time_step: Time step size
        """
        print(f"Running developmental simulation for {duration}s...")
        print(f"Time step: {time_step}s")
        
        steps = int(duration / time_step)
        
        for step in range(steps):
            # Update developmental processes
            self.update(time_step)
            
            # Log progress periodically
            if step % 100 == 0:
                report = self.getDevelopmentalReport()
                print(f"Step {step}: Age={report['current_age']:.1f}, "
                      f"Stage={report['current_stage']}, "
                      f"Maturity={report['maturity_level']:.3f}, "
                      f"Learning={report['learning_ability']:.3f}")
        
        print("\n=== Developmental Simulation Complete ===")
        
    def getDevelopmentalTrajectory(self) -> List[Dict[str, Any]]:
        """Get developmental trajectory over time."""
        trajectory = []
        
        # Create synthetic trajectory data
        for stage in self.developmental_stages:
            stage_data = {
                'stage': stage.name,
                'start_age': sum(s.duration for s in self.developmental_stages[:self.developmental_stages.index(stage)]),
                'duration': stage.duration,
                'plasticity': stage.plasticity,
                'learning_rate': stage.learning_rate,
                'behavior_modifiers': stage.behavior_modifiers
            }
            trajectory.append(stage_data)
        
        return trajectory
def runAdvancedDevelopmentExample():
    """Run the advanced development example."""
    print("=== Advanced Development Systems Example ===\n")
    
    # Create brain
    config = pynlm.createDemoConfig(1500, 0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize advanced development system
    development = AdvancedDevelopment(brain)
    
    print("Advanced development system initialized with:")
    print(f"- Custom developmental stages")
    print(f"- Age-dependent parameters")
    print(f"- Developmental events and milestones")
    print(f"- Plasticity modulation")
    print()
    
    # Run development simulation
    print("Running developmental simulation...")
    
    development.runDevelopmentSimulation(duration=20.0, time_step=0.01)
    
    # Display developmental report
    print("\n=== Final Developmental Report ===")
    report = development.getDevelopmentalReport()
    
    for key, value in report.items():
        if isinstance(value, dict):
            print(f"{key.replace('_', ' ').title()}:")
            for sub_key, sub_value in value.items():
                print(f"  {sub_key}: {sub_value}")
        else:
            print(f"{key.replace('_', ' ').title()}: {value}")
    
    # Display developmental trajectory
    print("\n=== Developmental Trajectory ===")
    trajectory = development.getDevelopmentalTrajectory()
    
    for stage_data in trajectory:
        print(f"Stage: {stage_data['stage']}")
        print(f"  Duration: {stage_data['duration']} steps")
        print(f"  Plasticity: {stage_data['plasticity']:.2f}")
        print(f"  Learning Rate: {stage_data['learning_rate']:.2f}")
        print(f"  Behavior Modifiers: {stage_data['behavior_modifiers']}")
        print()
    
    print("=== Advanced Development Complete ===")
if __name__ == "__main__":
    runAdvancedDevelopmentExample()