"""
Development module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Tuple, Union
from enum import Enum
import numpy as np

try:
    from pynlm.bindings import createBrain, createDefaultConfig, createSimpleWorld
    from pynlm.bindings import createAgentBrain, ActionType
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Development module limited.")


class DevelopmentalStage(Enum):
    """Developmental stages."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"


class DevelopmentalProcess:
    """Represents a developmental process."""
    
    def __init__(self, process_type: str, stage: DevelopmentalStage,
                 strength: float = 1.0, duration: float = 100.0):
        """
        Initialize developmental process.
        
        Args:
            process_type: Type of process
            stage: Developmental stage
            strength: Process strength (0.0 to 1.0)
            duration: Duration in time units
        """
        self.type = process_type
        self.stage = stage
        self.strength = max(0.0, min(1.0, strength))
        self.duration = duration
        self.current_progress = 0.0
        self.start_time = time.time()
        self.is_active = True
        self.efficiency = 1.0
        self.optimality = 0.5
    
    def update(self, timestep: float = 0.1):
        """
        Update developmental process.
        
        Args:
            timestep: Time step
        """
        if not self.is_active:
            return
        
        self.current_progress += timestep
        
        # Decay efficiency
        if self.current_progress > self.duration * 0.5:
            self.efficiency *= 0.995
        
        # Check if complete
        if self.current_progress >= self.duration:
            self.is_active = False
            self.optimality = min(1.0, self.optimality + 0.1)
    
    def get_progress(self) -> float:
        """Get process progress (0.0 to 1.0)."""
        return min(1.0, self.current_progress / self.duration)
    
    def get_remaining_time(self) -> float:
        """Get remaining time."""
        return max(0.0, self.duration - self.current_progress)
    
    def get_effectiveness(self) -> float:
        """Get process effectiveness."""
        return self.efficiency * self.optimality
    
    def enhance(self, amount: float = 0.1):
        """Enhance process effectiveness."""
        self.efficiency = min(1.0, self.efficiency + amount)
    
    def __repr__(self) -> str:
        return f"DevelopmentalProcess({self.type}, stage={self.stage.value}, progress={self.get_progress():.2f}, effectiveness={self.get_effectiveness():.3f})"


class DevelopmentalSystem:
    """Integrated developmental system."""
    
    def __init__(self, brain=None):
        """
        Initialize developmental system.
        
        Args:
            brain: Brain object to develop
        """
        self.brain = brain
        self.current_stage = DevelopmentalStage.INITIAL
        self.current_age = 0.0
        self.maturity_level = 0.0
        self.plasticity_rate = 1.0
        self.development_history = []
        self.processes = []
        self.stage_transitions = {}
        
        # Stage characteristics
        self.stage_characteristics = {
            DevelopmentalStage.INITIAL: {
                'plasticity': 2.0,
                'growth_rate': 0.5,
                'learning_capacity': 0.8,
                'optimal_duration': 50.0,
            },
            DevelopmentalStage.CRITICAL_PERIOD: {
                'plasticity': 3.0,
                'growth_rate': 1.0,
                'learning_capacity': 1.0,
                'optimal_duration': 100.0,
            },
            DevelopmentalStage.MATURATION: {
                'plasticity': 1.5,
                'growth_rate': 0.3,
                'learning_capacity': 0.6,
                'optimal_duration': 200.0,
            },
            DevelopmentalStage.ADULT: {
                'plasticity': 1.0,
                'growth_rate': 0.1,
                'learning_capacity': 0.4,
                'optimal_duration': 300.0,
            },
            DevelopmentalStage.AGING: {
                'plasticity': 0.8,
                'growth_rate': 0.05,
                'learning_capacity': 0.3,
                'optimal_duration': 400.0,
            },
        }
        
        # Development triggers
        self.development_triggers = {
            'experience': [],
            'age': [],
            'stress': [],
            'reward': [],
        }
        
        # Performance tracking
        self.development_metrics = {
            'stage_changes': [],
            'process_completions': [],
            'plasticity_events': [],
            'learning_events': [],
        }
    
    def update(self, timestep: float = 0.1):
        """
        Update developmental system.
        
        Args:
            timestep: Time step
        """
        self.current_age += timestep
        self.maturity_level = min(1.0, self.current_age / 1000.0)
        
        # Update processes
        for process in self.processes[:]:  # Copy list to avoid modification during iteration
            process.update(timestep)
            if not process.is_active:
                self.development_metrics['process_completions'].append({
                    'process_type': process.type,
                    'stage': process.stage.value,
                    'duration': process.duration,
                    'effectiveness': process.get_effectiveness(),
                    'timestamp': time.time(),
                })
                self.processes.remove(process)
        
        # Check for stage transition
        self._check_stage_transition()
        
        # Record development history
        self.development_history.append({
            'age': self.current_age,
            'stage': self.current_stage.value,
            'maturity': self.maturity_level,
            'plasticity': self.plasticity_rate,
            'processes_active': len(self.processes),
            'timestamp': time.time(),
        })
    
    def _check_stage_transition(self):
        """Check if stage transition should occur."""
        characteristics = self.stage_characteristics[self.current_stage]
        optimal_age = self._get_stage_optimal_age()
        
        # Check if age is appropriate for transition
        if self.current_age >= optimal_age:
            # Determine next stage
            stage_order = list(DevelopmentalStage)
            current_index = stage_order.index(self.current_stage)
            
            if current_index < len(stage_order) - 1:
                next_stage = stage_order[current_index + 1]
                
                # Check transition conditions
                if self._can_transition_to(next_stage):
                    old_stage = self.current_stage
                    self.current_stage = next_stage
                    
                    # Update plasticity rate
                    self.plasticity_rate = self.stage_characteristics[next_stage]['plasticity']
                    
                    # Record transition
                    self.development_metrics['stage_changes'].append({
                        'from_stage': old_stage.value,
                        'to_stage': next_stage.value,
                        'age': self.current_age,
                        'plasticity_change': self.plasticity_rate - characteristics['plasticity'],
                        'timestamp': time.time(),
                    })
                    
                    # Trigger plasticity changes
                    self._trigger_plasticity_change(next_stage)
    
    def _get_stage_optimal_age(self) -> float:
        """Get optimal age for current stage."""
        characteristics = self.stage_characteristics[self.current_stage]
        return self.current_age + characteristics['optimal_duration'] * 0.1  # Near optimal
    
    def _can_transition_to(self, target_stage: DevelopmentalStage) -> bool:
        """
        Check if transition to target stage is possible.
        
        Args:
            target_stage: Target developmental stage
        """
        # Simple age-based check
        stage_order = list(DevelopmentalStage)
        target_index = stage_order.index(target_stage)
        current_index = stage_order.index(self.current_stage)
        
        return target_index == current_index + 1
    
    def _trigger_plasticity_change(self, new_stage: DevelopmentalStage):
        """Trigger plasticity changes for new stage."""
        self.development_metrics['plasticity_events'].append({
            'stage': new_stage.value,
            'plasticity_rate': self.plasticity_rate,
            'learning_capacity': self.stage_characteristics[new_stage]['learning_capacity'],
            'timestamp': time.time(),
        })
    
    def add_development_trigger(self, trigger_type: str, condition: Dict[str, Any],
                               effect: Dict[str, Any]):
        """
        Add development trigger.
        
        Args:
            trigger_type: Trigger type ('experience', 'age', 'stress', 'reward')
            condition: Trigger condition
            effect: Effect when trigger activates
        """
        self.development_triggers[trigger_type].append({
            'condition': condition,
            'effect': effect,
            'active': True,
        })
    
    def check_triggers(self, stimulus: str, value: float = None):
        """
        Check if any triggers should activate.
        
        Args:
            stimulus: Stimulus type
            value: Stimulus value
        """
        if stimulus not in self.development_triggers:
            return
        
        for trigger in self.development_triggers[stimulus]:
            if trigger['active']:
                condition = trigger['condition']
                if self._evaluate_condition(condition, stimulus, value):
                    self._apply_trigger_effect(trigger['effect'])
    
    def _evaluate_condition(self, condition: Dict[str, Any], stimulus: str, value: float) -> bool:
        """Evaluate trigger condition."""
        if stimulus == 'age':
            return condition.get('min_age', 0) <= value <= condition.get('max_age', float('inf'))
        elif stimulus == 'reward':
            return condition.get('min_reward', 0) <= value <= condition.get('max_reward', float('inf'))
        elif stimulus == 'stress':
            return value >= condition.get('min_stress', 0)
        else:
            return True  # Default true for experience triggers
    
    def _apply_trigger_effect(self, effect: Dict[str, Any]):
        """Apply trigger effect."""
        effect_type = effect.get('type')
        
        if effect_type == 'process_start':
            process_type = effect.get('process_type')
            stage = effect.get('stage', self.current_stage)
            strength = effect.get('strength', 0.5)
            
            process = DevelopmentalProcess(process_type, stage, strength)
            self.processes.append(process)
            
            self.development_metrics['process_start'].append({
                'process_type': process_type,
                'stage': stage.value,
                'strength': strength,
                'timestamp': time.time(),
            })
        
        elif effect_type == 'plasticity_change':
            change = effect.get('change', 0.0)
            old_plasticity = self.plasticity_rate
            self.plasticity_rate += change
            
            self.development_metrics['plasticity_events'].append({
                'change': change,
                'old_plasticity': old_plasticity,
                'new_plasticity': self.plasticity_rate,
                'timestamp': time.time(),
            })
    
    def start_plasticity_event(self, event_type: str, intensity: float = 1.0):
        """
        Start plasticity event.
        
        Args:
            event_type: Event type
            intensity: Event intensity (0.0 to 1.0)
        """
        process = DevelopmentalProcess(
            f'plasticity_{event_type}',
            self.current_stage,
            intensity,
            duration=50.0
        )
        self.processes.append(process)
    
    def enhance_learning_capacity(self, amount: float = 0.1):
        """
        Enhance learning capacity.
        
        Args:
            amount: Enhancement amount
        """
        characteristics = self.stage_characteristics[self.current_stage]
        characteristics['learning_capacity'] = min(1.0, 
            characteristics['learning_capacity'] + amount)
    
    def get_development_summary(self) -> Dict[str, Any]:
        """Get development system summary."""
        return {
            'current_stage': self.current_stage.value,
            'current_age': self.current_age,
            'maturity_level': self.maturity_level,
            'plasticity_rate': self.plasticity_rate,
            'active_processes': len(self.processes),
            'development_history_length': len(self.development_history),
            'stage_characteristics': self.stage_characteristics[self.current_stage],
            'development_metrics': self.development_metrics,
        }
    
    def get_stage_info(self) -> Dict[str, Any]:
        """Get current stage information."""
        characteristics = self.stage_characteristics[self.current_stage]
        
        # Calculate stage progress
        stage_order = list(DevelopmentalStage)
        current_index = stage_order.index(self.current_stage)
        stage_progress = min(1.0, (self.current_age / (current_index * 100 + 50)))
        
        return {
            'stage': self.current_stage.value,
            'stage_index': current_index,
            'characteristics': characteristics,
            'progress': stage_progress,
            'time_remaining': self._get_stage_optimal_age() - self.current_age,
            'next_stage': stage_order[current_index + 1].value if current_index < len(stage_order) - 1 else None,
        }
    
    def save_developmental_state(self, filepath: str = "nlm_development_state.json"):
        """Save developmental state to file."""
        import json
        
        state = {
            'current_stage': self.current_stage.value,
            'current_age': self.current_age,
            'maturity_level': self.maturity_level,
            'plasticity_rate': self.plasticity_rate,
            'development_history': self.development_history,
            'development_metrics': self.development_metrics,
            'current_processes': [
                {
                    'type': p.type,
                    'stage': p.stage.value,
                    'strength': p.strength,
                    'duration': p.duration,
                    'current_progress': p.current_progress,
                    'is_active': p.is_active,
                }
                for p in self.processes
            ],
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
    
    def load_developmental_state(self, filepath: str = "nlm_development_state.json"):
        """Load developmental state from file."""
        import json
        
        with open(filepath, 'r') as f:
            state = json.load(f)
        
        self.current_stage = DevelopmentalStage(state['current_stage'])
        self.current_age = state['current_age']
        self.maturity_level = state['maturity_level']
        self.plasticity_rate = state['plasticity_rate']
        self.development_history = state['development_history']
        self.development_metrics = state['development_metrics']
        
        # Recreate processes
        self.processes = []
        for process_state in state['current_processes']:
            process = DevelopmentalProcess(
                process_state['type'],
                DevelopmentalStage(process_state['stage']),
                process_state['strength'],
                process_state['duration']
            )
            process.current_progress = process_state['current_progress']
            process.is_active = process_state['is_active']
            self.processes.append(process)
    
    def reset(self):
        """Reset developmental system."""
        self.current_stage = DevelopmentalStage.INITIAL
        self.current_age = 0.0
        self.maturity_level = 0.0
        self.plasticity_rate = 1.0
        
        self.development_history.clear()
        self.processes.clear()
        
        # Reset metrics
        for key in self.development_metrics:
            if isinstance(self.development_metrics[key], list):
                self.development_metrics[key].clear()
    
    def __repr__(self) -> str:
        return f"DevelopmentalSystem(stage={self.current_stage.value}, age={self.current_age:.1f}, maturity={self.maturity_level:.3f})"


def create_development_system(brain=None) -> DevelopmentalSystem:
    """Create developmental system."""
    return DevelopmentalSystem(brain)


def create_developmental_process(process_type: str, stage: DevelopmentalStage,
                                strength: float = 1.0, duration: float = 100.0) -> DevelopmentalProcess:
    """Create developmental process."""
    return DevelopmentalProcess(process_type, stage, strength, duration)


def run_developmental_simulation(brain, world, agent, steps: int = 100):
    """
    Run simulation with developmental processes.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of steps
    """
    # Create developmental system
    developmental_system = create_development_system(brain)
    
    for step in range(steps):
        # Update developmental system
        developmental_system.update(0.1)
        
        # Check developmental triggers
        # (In real usage, would pass actual sensory input, rewards, etc.)
        developmental_system.check_triggers('age', developmental_system.current_age)
        developmental_system.check_triggers('reward', agent.getNeuromodulationLevel())
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode and apply action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return developmental_system


def analyze_developmental_progress(developmental_system: DevelopmentalSystem) -> Dict[str, Any]:
    """Analyze developmental progress."""
    summary = developmental_system.get_development_summary()
    stage_info = developmental_system.get_stage_info()
    
    analysis = {
        'current_stage': summary['current_stage'],
        'development_phase': stage_info['stage_index'],
        'overall_maturity': summary['maturity_level'],
        'plasticity_status': summary['plasticity_rate'],
        'active_processes': summary['active_processes'],
        'development_metrics': {},
    }
    
    # Analyze stage characteristics
    characteristics = stage_info['characteristics']
    analysis['stage_characteristics'] = {
        'plasticity': characteristics['plasticity'],
        'growth_rate': characteristics['growth_rate'],
        'learning_capacity': characteristics['learning_capacity'],
        'optimal_duration': characteristics['optimal_duration'],
    }
    
    # Analyze development history
    if summary['development_history_length'] > 0:
        history = summary['development_history']
        analysis['history_analysis'] = {
            'total_time': history[-1]['age'] if history else 0,
            'avg_plasticity': sum(h['plasticity'] for h in history) / len(history),
            'stage_transitions': len([m for m in summary['development_metrics']['stage_changes']]),
            'process_completions': len(summary['development_metrics']['process_completions']),
            'last_stage_change': max(
                [m['timestamp'] for m in summary['development_metrics']['stage_changes']],
                default=0
            ),
        }
    
    # Analyze current processes
    if summary['active_processes'] > 0:
        processes = developmental_system.processes
        analysis['process_analysis'] = {
            'process_types': list(set(p.type for p in processes)),
            'avg_process_progress': sum(p.get_progress() for p in processes) / len(processes),
            'strongest_process': max(processes, key=lambda p: p.strength).type if processes else None,
            'total_remaining_time': sum(p.get_remaining_time() for p in processes),
        }
    
    return analysis
