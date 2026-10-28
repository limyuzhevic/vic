# NLM Python Development Module
#
# Development utilities for NLM (Neural Learning Machine) brains.
# Provides high-level interfaces for developmental stages, structural plasticity,
# and age-related learning capabilities.

from typing import List, Dict, Any, Optional, Enum
import time

from .nlm import NLMConfiguration, NLMBrain

class DevelopmentalStage(Enum):
    """Developmental stages for NLM brains."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
    
    @classmethod
    def get_stage_name(cls, stage: int) -> str:
        """Get human-readable stage name."""
        stage_names = {
            0: "Initial",
            1: "Critical Period", 
            2: "Maturation",
            3: "Adult",
            4: "Aging"
        }
        return stage_names.get(stage, f"Unknown Stage {stage}")

class DevelopmentalCharacteristics:
    """Developmental characteristics and capabilities for NLM brains."""
    
    def __init__(self):
        self.initial_neurons = 100
        self.initial_synapses = 500
        self.synapse_formation_rate = 0.01
        self.pruning_rate = 0.001
        self.plasticity_scaling = 1.0
        self.memory_capacity_growth = 0.8
        self.excitability_growth = 0.6
        
    def get_characteristics(self, stage: DevelopmentalStage, age: float) -> Dict[str, Any]:
        """Get developmental characteristics for a given stage and age."""
        base_characteristics = {
            "synaptogenesis_rate": self.synapse_formation_rate,
            "pruning_rate": self.pruning_rate,
            "plasticity_learning_rate": 0.01 * self.plasticity_scaling,
            "working_memory_capacity": int(100 * (1 + self.memory_capacity_growth * age)),
            "episodic_memory_capacity": int(50 * (1 + self.memory_capacity_growth * age)),
            "neural_excitability": 1.0 + (0.5 * age),  # Increases with age
            "attention_span": min(100.0, 10.0 + (5.0 * age)),  # Improves then plateaus
            "concept_formation_rate": 0.1 * self.plasticity_scaling,
            "development_energy_cost": 1.0 + (0.1 * age)
        }
        
        # Stage-specific modifiers
        if stage == DevelopmentalStage.CRITICAL_PERIOD:
            base_characteristics.update({
                "synaptogenesis_rate": self.synapse_formation_rate * 3.0,
                "plasticity_learning_rate": 0.05 * self.plasticity_scaling,
                "neural_excitability": 1.5 + (0.8 * age),
                "attention_span": 20.0 + (10.0 * age),
                "development_energy_cost": 2.0 + (0.2 * age)
            })
        
        elif stage == DevelopmentalStage.MATURATION:
            base_characteristics.update({
                "synaptogenesis_rate": self.synapse_formation_rate * 0.5,
                "pruning_rate": self.pruning_rate * 2.0,
                "plasticity_learning_rate": 0.02 * self.plasticity_scaling,
                "concept_formation_rate": 0.15 * self.plasticity_scaling,
                "development_energy_cost": 1.5 + (0.15 * age)
            })
        
        elif stage == DevelopmentalStage.AGING:
            base_characteristics.update({
                "synaptogenesis_rate": self.synapse_formation_rate * 0.1,
                "pruning_rate": self.pruning_rate * 5.0,
                "plasticity_learning_rate": 0.005 * self.plasticity_scaling,
                "working_memory_capacity": int(base_characteristics["working_memory_capacity"] * 0.7),
                "neural_excitability": max(0.5, 1.0 + (0.2 * age)),  # Decreases with age
                "development_energy_cost": 1.0 + (0.3 * age)
            })
        
        return base_characteristics

class NLMDeveloper:
    """High-level interface for NLM brain development and maturation."""
    
    def __init__(self, brain: NLMBrain):
        self.brain = brain
        self.characteristics = DevelopmentalCharacteristics()
        self.current_stage = DevelopmentalStage.INITIAL
        self.development_time = 0.0
        self.stage_start_time = 0.0
        self.development_metrics = {
            "stage_transitions": [],
            "plasticity_events": [],
            "structural_changes": []
        }
    
    def get_current_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self.current_stage
    
    def get_development_time(self) -> float:
        """Get total development time."""
        return self.development_time
    
    def get_developmental_age(self) -> float:
        """Get developmental age in arbitrary time units."""
        return self.development_time / 1000.0  # Normalize
    
    def should_update_stage(self, time_step: float) -> bool:
        """Check if developmental stage should be updated based on time."""
        # Stage transition timing (in simulation time steps)
        stage_durations = {
            DevelopmentalStage.INITIAL: 1000,
            DevelopmentalStage.CRITICAL_PERIOD: 2000,
            DevelopmentalStage.MATURATION: 3000,
            DevelopmentalStage.ADULT: float('inf'),  # Adult stage lasts forever
            DevelopmentalStage.AGING: 0  # Can enter aging anytime
        }
        
        # Simple stage progression logic
        current_age = self.get_developmental_age()
        
        if self.current_stage == DevelopmentalStage.INITIAL and current_age >= 1.0:
            return True
        elif self.current_stage == DevelopmentalStage.CRITICAL_PERIOD and current_age >= 3.0:
            return True
        elif self.current_stage == DevelopmentalStage.MATURATION and current_age >= 6.0:
            return True
        elif self.current_stage == DevelopmentalStage.ADULT and current_age >= 10.0:
            return True
        elif self.current_stage == DevelopmentalStage.ADULT and current_age >= 15.0:
            return True  # Enter aging stage
        
        return False
    
    def update_stage(self, time_step: float) -> Optional[DevelopmentalStage]:
        """Update developmental stage if needed."""
        if self.should_update_stage(time_step):
            old_stage = self.current_stage
            
            if old_stage == DevelopmentalStage.INITIAL:
                self.current_stage = DevelopmentalStage.CRITICAL_PERIOD
            elif old_stage == DevelopmentalStage.CRITICAL_PERIOD:
                self.current_stage = DevelopmentalStage.MATURATION
            elif old_stage == DevelopmentalStage.MATURATION:
                self.current_stage = DevelopmentalStage.ADULT
            elif old_stage == DevelopmentalStage.ADULT:
                self.current_stage = DevelopmentalStage.AGING
            
            # Record stage transition
            transition_time = self.development_time
            self.development_metrics["stage_transitions"].append({
                "from_stage": old_stage.value,
                "to_stage": self.current_stage.value,
                "time": transition_time,
                "age": transition_time / 1000.0
            })
            
            return self.current_stage
        
        return None
    
    def advance_development(self, time_step: float) -> Dict[str, Any]:
        """Advance development by a time step and return development metrics."""
        self.development_time += time_step
        
        # Update stage if needed
        new_stage = self.update_stage(time_step)
        
        # Get current developmental characteristics
        characteristics = self.characteristics.get_characteristics(
            self.current_stage, self.get_developmental_age()
        )
        
        # Log development events
        development_event = {
            "time": self.development_time,
            "age": self.get_developmental_age(),
            "stage": self.current_stage.value,
            "characteristics": characteristics,
            "stage_changed": new_stage is not None,
            "new_stage": new_stage.value if new_stage else None
        }
        
        self.development_metrics["developmental_events"].append(development_event)
        
        return {
            "current_stage": self.current_stage.value,
            "age": self.get_developmental_age(),
            "characteristics": characteristics,
            "stage_changed": new_stage is not None,
            "new_stage": new_stage.value if new_stage else None,
            "total_development_time": self.development_time
        }
    
    def analyze_development(self) -> Dict[str, Any]:
        """Analyze development history and return insights."""
        if not self.development_metrics["developmental_events"]:
            return {"error": "No development history available"}
        
        events = self.development_metrics["developmental_events"]
        
        # Analyze stage durations
        stage_durations = {}
        for i, event in enumerate(events):
            stage = event["stage"]
            duration = event["time"] - (events[i-1]["time"] if i > 0 else 0)
            if stage not in stage_durations:
                stage_durations[stage] = 0
            stage_durations[stage] += duration
        
        # Analyze plasticity events
        plasticity_events = self.development_metrics["plasticity_events"]
        
        # Analyze structural changes
        structural_changes = self.development_metrics["structural_changes"]
        
        analysis = {
            "current_stage": self.current_stage.value,
            "total_development_time": self.development_time,
            "developmental_age": self.get_developmental_age(),
            "stage_durations": stage_durations,
            "stage_transitions": self.development_metrics["stage_transitions"],
            "development_phases": self._identify_development_phases(events),
            "plasticity_activity": self._analyze_plasticity_activity(plasticity_events),
            "structural_maturation": self._analyze_structural_maturation(structural_changes),
            "critical_periods": self._identify_critical_periods(events)
        }
        
        return analysis
    
    def _identify_development_phases(self, events: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Identify major development phases from event history."""
        phases = []
        
        if len(events) < 3:
            return phases
        
        # Detect rapid change periods (critical periods)
        rapid_change_threshold = 2.0  # Arbitrary threshold
        
        for i in range(1, len(events) - 1):
            prev_time = events[i-1]["time"]
            curr_time = events[i]["time"]
            next_time = events[i+1]["time"] if i < len(events) - 1 else curr_time + 1000
            
            # Calculate rate of change in characteristics
            prev_char = events[i-1]["characteristics"]
            curr_char = events[i]["characteristics"]
            next_char = events[i+1]["characteristics"]
            
            # Simple rate of change calculation
            char_change_rate = 0
            for key in prev_char:
                if key in curr_char and key in next_char:
                    prev_val = prev_char[key]
                    curr_val = curr_char[key]
                    next_val = next_char[key]
                    
                    if isinstance(prev_val, (int, float)) and isinstance(curr_val, (int, float)):
                        change_rate = abs((curr_val - prev_val) / (curr_time - prev_time))
                        char_change_rate += change_rate
            
            if char_change_rate > rapid_change_threshold:
                phases.append({
                    "phase_start": prev_time,
                    "phase_end": next_time,
                    "rate_of_change": char_change_rate,
                    "stage_at_center": events[i]["stage"]
                })
        
        return phases
    
    def _analyze_plasticity_activity(self, plasticity_events: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze plasticity activity patterns."""
        if not plasticity_events:
            return {"error": "No plasticity events recorded"}
        
        # Calculate activity statistics
        plasticity_strengths = [event.get("strength", 0) for event in plasticity_events]
        
        analysis = {
            "total_events": len(plasticity_events),
            "average_strength": sum(plasticity_strengths) / len(plasticity_strengths) if plasticity_strengths else 0,
            "max_strength": max(plasticity_strengths) if plasticity_strengths else 0,
            "min_strength": min(plasticity_strengths) if plasticity_strengths else 0,
            "strength_variance": self._variance(plasticity_strengths) if plasticity_strengths else 0,
            "recent_activity": len([e for e in plasticity_events if e.get("time", 0) > self.development_time - 100]) if plasticity_events else 0
        }
        
        return analysis
    
    def _analyze_structural_maturation(self, structural_changes: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze structural maturation patterns."""
        if not structural_changes:
            return {"error": "No structural changes recorded"}
        
        synapse_changes = [change.get("synapse_count", 0) for change in structural_changes]
        neuron_changes = [change.get("neuron_count", 0) for change in structural_changes]
        
        analysis = {
            "total_structural_events": len(structural_changes),
            "synapse_change_rate": self._calculate_rate_of_change(synapse_changes),
            "neuron_change_rate": self._calculate_rate_of_change(neuron_changes),
            "peak_synapse_growth": max(synapse_changes) if synapse_changes else 0,
            "peak_neuron_growth": max(neuron_changes) if neuron_changes else 0
        }
        
        return analysis
    
    def _identify_critical_periods(self, events: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Identify critical periods in development."""
        critical_periods = []
        
        # Look for periods of rapid characteristic change
        for i in range(2, len(events) - 2):
            curr_event = events[i]
            
            # Calculate rate of change in key characteristics
            chars = curr_event["characteristics"]
            
            # Check for high rates of change in plasticity or structure
            if chars.get("plasticity_learning_rate", 0) > 0.04 or \
               chars.get("synaptogenesis_rate", 0) > 0.02 or \
               chars.get("attention_span", 0) > 15.0:
                
                critical_periods.append({
                    "center_time": curr_event["time"],
                    "center_age": curr_event["age"],
                    "stage": curr_event["stage"],
                    "characteristics": chars,
                    "duration": 200  # Estimated duration
                })
        
        return critical_periods
    
    def _variance(self, values: List[float]) -> float:
        """Calculate variance of a list of values."""
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        squared_diffs = [(x - mean) ** 2 for x in values]
        return sum(squared_diffs) / len(squared_diffs)
    
    def _calculate_rate_of_change(self, values: List[float]) -> List[float]:
        """Calculate rate of change for a series of values."""
        if len(values) < 2:
            return []
        
        rates = []
        for i in range(1, len(values)):
            rate = (values[i] - values[i-1]) / 1.0  # Assuming unit time step
            rates.append(rate)
        
        return rates

class StructuralPlasticitySimulator:
    """Simulator for structural plasticity processes (synaptogenesis and pruning)."""
    
    def __init__(self, brain: NLMBrain, developer: NLMDeveloper):
        self.brain = brain
        self.developer = developer
        self.synaptogenesis_events = []
        self.pruning_events = []
    
    def simulate_synaptogenesis(self, duration: float, rate: float, strength: float = 1.0) -> List[Dict[str, Any]]:
        """Simulate synaptogenesis (formation of new synapses) over time."""
        events = []
        current_synapses = self.brain._brain.getTotalSynapseCount()
        
        for time_step in range(int(duration * 1000)):  # Convert to simulation time steps
            # Probabilistic synapse formation based on rate and developmental stage
            stage = self.developer.get_current_stage()
            age = self.developer.get_developmental_age()
            
            # Adjust rate based on developmental stage and age
            effective_rate = rate
            if stage == DevelopmentalStage.CRITICAL_PERIOD:
                effective_rate *= 3.0
            elif stage == DevelopmentalStage.AGING:
                effective_rate *= 0.1
            
            # Age-related modulation
            effective_rate *= (1.0 + 0.1 * age)  # Rate increases slightly with age
            
            # Stochastic synapse formation
            if effective_rate * 1000 > 1:  # Probability per time step
                new_synapses = int(effective_rate * 1000 * strength)
                if new_synapses > 0:
                    event = {
                        "time": time_step / 1000.0,
                        "age": self.developer.get_developmental_age(),
                        "stage": stage.value,
                        "new_synapses": new_synapses,
                        "strength": strength,
                        "rate_used": effective_rate,
                        "total_synapses": current_synapses + new_synapses
                    }
                    events.append(event)
                    current_synapses += new_synapses
        
        self.synaptogenesis_events.extend(events)
        return events
    
    def simulate_pruning(self, duration: float, rate: float, strength: float = 1.0) -> List[Dict[str, Any]]:
        """Simulate synaptic pruning over time."""
        events = []
        current_synapses = self.brain._brain.getTotalSynapseCount()
        
        for time_step in range(int(duration * 1000)):
            stage = self.developer.get_current_stage()
            age = self.developer.get_developmental_age()
            
            # Adjust pruning rate based on developmental stage and age
            effective_rate = rate
            if stage == DevelopmentalStage.MATURATION:
                effective_rate *= 2.0
            elif stage == DevelopmentalStage.AGING:
                effective_rate *= 5.0
            
            # Age-related modulation
            effective_rate *= (1.0 + 0.05 * age)  # Rate increases with age
            
            # Stochastic synapse pruning
            max_prunable = min(current_synapses, int(effective_rate * 1000 * strength))
            if max_prunable > 0:
                pruned = int(max_prunable * 0.8)  # Prune 80% of eligible synapses
                
                event = {
                    "time": time_step / 1000.0,
                    "age": self.developer.get_developmental_age(),
                    "stage": stage.value,
                    "pruned_synapses": pruned,
                    "strength": strength,
                    "rate_used": effective_rate,
                    "total_synapses": current_synapses - pruned
                }
                events.append(event)
                current_synapses -= pruned
        
        self.pruning_events.extend(events)
        return events
    
    def run_structural_simulation(self, duration: float) -> Dict[str, Any]:
        """Run complete structural simulation for given duration."""
        # Get developmental characteristics
        stage = self.developer.get_current_stage()
        age = self.developer.get_developmental_age()
        
        characteristics = self.developer.characteristics.get_characteristics(stage, age)
        
        # Simulate both processes
        synaptogenesis_events = self.simulate_synaptogenesis(
            duration, characteristics.get("synaptogenesis_rate", 0.001)
        )
        
        pruning_events = self.simulate_pruning(
            duration, characteristics.get("pruning_rate", 0.0001)
        )
        
        # Analyze results
        total_synaptogenesis = sum(e["new_synapses"] for e in synaptogenesis_events)
        total_pruning = sum(e["pruned_synapses"] for e in pruning_events)
        net_change = total_synaptogenesis - total_pruning
        
        final_synapses = self.brain._brain.getTotalSynapseCount()
        
        simulation_result = {
            "duration": duration,
            "start_stage": stage.value,
            "end_stage": self.developer.get_current_stage().value,
            "final_synapses": final_synapses,
            "synaptogenesis_events": len(synaptogenesis_events),
            "pruning_events": len(pruning_events),
            "total_synaptogenesis": total_synaptogenesis,
            "total_pruning": total_pruning,
            "net_synapse_change": net_change,
            "final_characteristics": characteristics
        }
        
        return simulation_result
