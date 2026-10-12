#!/usr/bin/env python3
"""
Advanced Neural Models

Custom neuron and synaptic models for advanced neural computation.

This module provides comprehensive tools for designing custom neural models
with support for adaptive threshold neurons, spike-frequency adaptation,
short-term plasticity, neuromodulation, dendritic computation, and axonal
propagation.

Features:
- Custom neuron models (adaptive threshold, spike-frequency adaptation)
- Advanced synaptic models (short-term plasticity, neuromodulation)
- Dendritic computation models
- Axonal propagation models
- Neural network integration
- Simulation and analysis tools

Usage:
    python scripts/advanced_neural.py --create --type adaptive_threshold --config neuron_config.json
    python scripts/advanced_neural.py --simulate --model my_neural_model --steps 1000
    python scripts/advanced_neural.py --analyze --model my_neural_model --metrics firing_rate
"""

import sys
from pathlib import Path
import json
import copy
from typing import Dict, List, Any, Optional, Union, Tuple
import numpy as np
from enum import Enum
import warnings
import math

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm.models import BaseNeuralModel
from nlm.neuromodulation import NeuromodulationSystem


class NeuronType(Enum):
    """Types of neurons."""
    ADAPTIVE_THRESHOLD = "adaptive_threshold"
    SPIKE_FREQ_ADAPTATION = "spike_freq_adaptation"
    POISSON = "poisson"
    LEaky_RELU = "leaky_relu"
    THRESHOLDLESS = "thresholdless"
    RESONANT = "resonant"
    GRADIENT = "gradient"
    ADAPTIVE_INTEGRATE_AND_FIRE = "adaptive_integrate_and_fire"
    STP_NEURON = "stp_neuron"


class SynapticModelType(Enum):
    """Types of synaptic models."""
    STDP = "STDP"  # Spike-Timing Dependent Plasticity
    BCM = "BCM"  # Bayesian Complementarity Mapping
    Oja = "oja"
    HEBBIAN = "hebbian"
    ANTI_HEBBIAN = "anti_hebbian"
    LTP = "LTP"  # Long-Term Potentiation
    LTD = "LTD"  # Long-Term Depression
    SHORT_TERM = "short_term"
    MODULATORY = "modulatory"


class DendriticModelType(Enum):
    """Types of dendritic models."""
    LINEAR = "linear"
    NONLINEAR = "nonlinear"
    SEGMENTED = "segmented"
    BRANCHING = "branching"
    COMPARTMENTAL = "compartmental"


class AxonalModelType(Enum):
    """Types of axonal models."""
    PASSIVE = "passive"
    ACTIVE = "active"
    SALTATORY = "saltatory"
    MYELINATED = "myelinated"
    REGENERATIVE = "regenerative"


@dataclass
class AdaptiveThresholdNeuron:
    """Neuron with adaptive threshold mechanism."""
    
    def __init__(self, 
                 v_rest: float = -70.0,
                 v_th: float = -50.0,
                 v_reset: float = -65.0,
                 tau_m: float = 10.0,
                 tau_adp: float = 100.0,
                 a_adp: float = 4.0,
                 b_adp: float = 80.0,
                 learning_rate: float = 0.01):
        """Initialize adaptive threshold neuron.
        
        Args:
            v_rest: Resting membrane potential (mV)
            v_th: Threshold voltage (mV)
            v_reset: Reset voltage (mV)
            tau_m: Membrane time constant (ms)
            tau_adp: Adaptation time constant (ms)
            a_adp: Adaptation current strength
            b_adp: Adaptation threshold shift
            learning_rate: Learning rate for adaptation
        """
        self.v_rest = v_rest
        self.v_th = v_th
        self.v_reset = v_reset
        self.tau_m = tau_m
        self.tau_adp = tau_adp
        self.a_adp = a_adp
        self.b_adp = b_adp
        self.learning_rate = learning_rate
        
        # State variables
        self.v = v_rest
        self.i_adp = 0.0
        self.spike_count = 0
        self.spike_times = []
        self.refractory_period = 0.0
    
    def update(self, input_current: float, dt: float = 1.0) -> Tuple[bool, float]:
        """Update neuron state.
        
        Args:
            input_current: Input current (nA)
            dt: Time step (ms)
            
        Returns:
            Tuple of (spiked, output_voltage)
        """
        # Check refractory period
        if self.refractory_period > 0:
            self.refractory_period -= dt
            self.v = self.v_reset
            return False, self.v
        
        # Update membrane potential
        dv = ((-self.v + self.v_rest) + input_current) / self.tau_m * dt
        self.v += dv
        
        # Update adaptation current
        di_adp = (-self.i_adp) / self.tau_adp * dt
        self.i_adp += di_adp
        
        # Check for spike
        threshold = self.v_th + self.i_adp * self.b_adp
        spiked = self.v >= threshold
        
        if spiked:
            # Record spike
            self.spike_count += 1
            self.spike_times.append(self.v)
            
            # Reset neuron
            self.v = self.v_reset
            self.refractory_period = 2.0  # ms
            
            # Update adaptation current
            self.i_adp += self.a_adp
        
        return spiked, self.v
    
    def get_firing_rate(self, window_size: float = 1000.0) -> float:
        """Calculate firing rate over a time window.
        
        Args:
            window_size: Time window (ms)
            
        Returns:
            Firing rate (Hz)
        """
        if not self.spike_times:
            return 0.0
        
        # Count spikes within window
        recent_spikes = [t for t in self.spike_times if t > window_size]
        return len(recent_spikes) / (window_size / 1000.0)
    
    def reset(self):
        """Reset neuron state."""
        self.v = self.v_rest
        self.i_adp = 0.0
        self.spike_count = 0
        self.spike_times = []
        self.refractory_period = 0.0


@dataclass
class SpikeFrequencyAdapterNeuron:
    """Neuron with spike-frequency adaptation."""
    
    def __init__(self,
                 v_rest: float = -70.0,
                 v_th: float = -50.0,
                 v_reset: float = -65.0,
                 tau_m: float = 10.0,
                 tau_adapt: float = 50.0,
                 a_adapt: float = 0.5,
                 b_adapt: float = 0.1,
                 learning_rate: float = 0.01):
        """Initialize spike-frequency adapter neuron.
        
        Args:
            v_rest: Resting membrane potential (mV)
            v_th: Threshold voltage (mV)
            v_reset: Reset voltage (mV)
            tau_m: Membrane time constant (ms)
            tau_adapt: Adaptation time constant (ms)
            a_adapt: Adaptation strength
            b_adapt: Adaptation slope
            learning_rate: Learning rate for adaptation
        """
        self.v_rest = v_rest
        self.v_th = v_th
        self.v_reset = v_reset
        self.tau_m = tau_m
        self.tau_adapt = tau_adapt
        self.a_adapt = a_adapt
        self.b_adapt = b_adapt
        self.learning_rate = learning_rate
        
        # State variables
        self.v = v_rest
        self.w = 0.0  # Adaptation variable
        self.spike_count = 0
        self.spike_times = []
        self.refractory_period = 0.0
    
    def update(self, input_current: float, dt: float = 1.0) -> Tuple[bool, float]:
        """Update neuron state.
        
        Args:
            input_current: Input current (nA)
            dt: Time step (ms)
            
        Returns:
            Tuple of (spiked, output_voltage)
        """
        # Check refractory period
        if self.refractory_period > 0:
            self.refractory_period -= dt
            self.v = self.v_reset
            return False, self.v
        
        # Update membrane potential with adaptation
        dv = ((-self.v + self.v_rest) + input_current - self.w) / self.tau_m * dt
        self.v += dv
        
        # Update adaptation variable
        dw = (-self.w) / self.tau_adapt * dt
        self.w += dw
        
        # Check for spike
        threshold = self.v_th - self.b_adapt * self.w
        spiked = self.v >= threshold
        
        if spiked:
            # Record spike
            self.spike_count += 1
            self.spike_times.append(self.v)
            
            # Reset neuron
            self.v = self.v_reset
            self.refractory_period = 2.0  # ms
            
            # Increase adaptation variable
            self.w += self.a_adapt
        
        return spiked, self.v
    
    def get_firing_rate(self, window_size: float = 1000.0) -> float:
        """Calculate firing rate over a time window.
        
        Args:
            window_size: Time window (ms)
            
        Returns:
            Firing rate (Hz)
        """
        if not self.spike_times:
            return 0.0
        
        # Count spikes within window
        recent_spikes = [t for t in self.spike_times if t > window_size]
        return len(recent_spikes) / (window_size / 1000.0)
    
    def reset(self):
        """Reset neuron state."""
        self.v = self.v_rest
        self.w = 0.0
        self.spike_count = 0
        self.spike_times = []
        self.refractory_period = 0.0


@dataclass
class ShortTermPlasticitySynapse:
    """Synapse with short-term plasticity."""
    
    def __init__(self,
                 weight: float = 1.0,
                 u: float = 0.3,  # Utilization parameter
                 tau_f: float = 10.0,  # facilitation time constant
                 tau_d: float = 100.0,  # depression time constant
                 x: float = 0.0,  # facilitation variable
                 y: float = 1.0):  # depression variable
        """Initialize short-term plasticity synapse.
        
        Args:
            weight: Initial synaptic weight
            u: Utilization parameter
            tau_f: Facilitation time constant (ms)
            tau_d: Depression time constant (ms)
            x: Initial facilitation variable
            y: Initial depression variable
        """
        self.weight = weight
        self.u = u
        self.tau_f = tau_f
        self.tau_d = tau_d
        self.x = x
        self.y = y
        self.max_weight = 10.0
        self.min_weight = -10.0
    
    def update(self, pre_spike: bool, dt: float = 1.0) -> float:
        """Update synapse state based on presynaptic activity.
        
        Args:
            pre_spike: Whether presynaptic neuron spiked
            dt: Time step (ms)
            
        Returns:
            Effective synaptic weight
        """
        # Update facilitation variable
        if pre_spike:
            dx = (1.0 - self.x) / self.tau_f * dt
            self.x += dx
        
        # Update depression variable
        dy = (self.y - 1.0) / self.tau_d * dt
        self.y += dy
        
        # Calculate effective weight
        w_eff = self.weight * self.y * (self.u * self.x + (1.0 - self.u))
        
        # Clip to valid range
        self.weight = max(self.min_weight, min(self.max_weight, self.weight))
        
        return w_eff
    
    def store(self, post_spike: bool, dt: float = 1.0) -> None:
        """Store spike pair for STDP.
        
        Args:
            post_spike: Whether postsynaptic neuron spiked
            dt: Time step (ms)
        """
        if post_spike:
            # Depression when postsynaptic spikes without presynaptic spike
            dy = (self.y - 1.0) / self.tau_d * dt
            self.y += dy


@dataclass
class NeuromodulatorySynapticModel:
    """Synaptic model with neuromodulation."""
    
    def __init__(self,
                 base_weight: float = 1.0,
                 neuromodulator_influence: Dict[str, float] = None,
                 learning_rate: float = 0.01,
                 homeostasis: bool = True):
        """Initialize neuromodulatory synaptic model.
        
        Args:
            base_weight: Base synaptic weight
            neuromodulator_influence: Influence of neuromodulators on weight
            learning_rate: Learning rate
            homeostasis: Whether to use homeostatic plasticity
        """
        self.base_weight = base_weight
        self.neuromodulator_influence = neuromodulator_influence or {}
        self.learning_rate = learning_rate
        self.homeostasis = homeostasis
        self.target_weight = base_weight
        self.weight_history = [base_weight]
    
    def update(self, pre_spike: bool, post_spike: bool, neuromodulators: Dict[str, float], dt: float = 1.0) -> float:
        """Update synapse weight based on spike timing and neuromodulators.
        
        Args:
            pre_spike: Whether presynaptic neuron spiked
            post_spike: Whether postsynaptic neuron spiked
            neuromodulators: Neuromodulator concentrations
            dt: Time step (ms)
            
        Returns:
            Updated synaptic weight
        """
        # Calculate weight change
        dw = 0.0
        
        if pre_spike and post_spike:
            # Coincidence detection with neuromodulatory modulation
            coincidence_strength = 1.0
            for mod_name, influence in self.neuromodulator_influence.items():
                if mod_name in neuromodulators:
                    coincidence_strength *= (1.0 + influence * (neuromodulators[mod_name] - 0.5))
            
            dw = self.learning_rate * coincidence_strength
        elif pre_spike and not post_spike:
            # Presynaptic spike without postsynaptic spike -> depression
            dw = -self.learning_rate * 0.5
        
        # Apply weight change
        new_weight = self.base_weight + dw
        
        # Apply homeostasis if enabled
        if self.homeostasis:
            # Target weight is the average of recent weights
            if len(self.weight_history) > 10:
                target_weight = np.mean(self.weight_history[-10:])
                homeo_dw = self.learning_rate * 0.1 * (target_weight - new_weight)
                new_weight += homeo_dw
        
        # Update base weight
        self.base_weight = new_weight
        self.weight_history.append(new_weight)
        
        # Clip to reasonable range
        self.base_weight = max(-10.0, min(10.0, self.base_weight))
        
        return self.base_weight
    
    def set_neuromodulator_influence(self, neuromodulator: str, influence: float):
        """Set neuromodulator influence on synaptic weight.
        
        Args:
            neuromodulator: Neuromodulator name
            influence: Influence factor (positive or negative)
        """
        self.neuromodulator_influence[neuromodulator] = influence


@dataclass
class DendriticSegment:
    """Dendritic segment with nonlinear computation."""
    
    def __init__(self,
                 length: float = 100.0,
                 radius: float = 1.0,
                 model_type: DendriticModelType = DendriticModelType.NONLINEAR):
        """Initialize dendritic segment.
        
        Args:
            length: Segment length (µm)
            radius: Segment radius (µm)
            model_type: Type of dendritic model
        """
        self.length = length
        self.radius = radius
        self.model_type = model_type
        
        # Segment properties
        self.capacitance = 1.0  # µF/cm²
        self.resistance = 100.0  # Ω·cm
        self.area = 2 * np.pi * radius * length
        
        # State variables
        self.potential = 0.0
        self.current = 0.0
        self.spike_probability = 0.0
        
        # Input compartments
        self.input_compartments = []
        
        # Segment-specific properties
        self.model_parameters = self._initialize_model_parameters()
    
    def _initialize_model_parameters(self) -> Dict[str, Any]:
        """Initialize model-specific parameters."""
        if self.model_type == DendriticModelType.LINEAR:
            return {
                "conductance_multiplier": 1.0,
                "threshold": 1.0,
                "slope": 1.0
            }
        elif self.model_type == DendriticModelType.NONLINEAR:
            return {
                "conductance_multiplier": 1.0,
                "threshold": 1.0,
                "slope": 1.0,
                "nonlinearity": 2.0,
                "saturation": 5.0
            }
        elif self.model_type == DendriticModelType.SEGMENTED:
            return {
                "num_segments": 10,
                "segment_length": self.length / 10,
                "inter_segment_resistance": 10.0
            }
        elif self.model_type == DendriticModelType.BRANCHING:
            return {
                "branch_angle": np.pi / 4,
                "branch_length": self.length * 0.7,
                "branch_radius": self.radius * 0.5
            }
        else:  # COMPARTMENTAL
            return {
                "num_compartments": 20,
                "compartment_length": self.length / 20,
                "compartment_resistance": 100.0
            }
    
    def add_input(self, input_current: float, input_position: Tuple[float, float, float] = (0, 0, 0)) -> float:
        """Add input to dendritic segment.
        
        Args:
            input_current: Input current
            input_position: Position of input (x, y, z) relative to segment
            
        Returns:
            Output from segment
        """
        # Calculate distance from segment center
        dist = np.sqrt(input_position[0]**2 + input_position[1]**2 + input_position[2]**2)
        
        # Attenuation based on distance
        if self.model_type == DendriticModelType.LINEAR:
            attenuation = max(0, 1 - dist / self.length)
        elif self.model_type == DendriticModelType.NONLINEAR:
            attenuation = max(0, 1 - dist / self.length) ** self.model_parameters["nonlinearity"]
        elif self.model_type == DendriticModelType.SEGMENTED:
            segment_idx = int(dist / self.model_parameters["segment_length"])
            attenuation = max(0, 1 - (segment_idx + 1) / self.model_parameters["num_segments"])
        elif self.model_type == DendriticModelType.BRANCHING:
            # Branching attenuation
            angle_dist = np.arctan2(input_position[1], input_position[0])
            attenuation = max(0, 1 - abs(angle_dist - self.model_parameters["branch_angle"]) / (np.pi / 2))
        else:  # COMPARTMENTAL
            # Compartmental model with linear attenuation
            attenuation = max(0, 1 - dist / self.length)
        
        # Apply model-specific transformation
        if self.model_type == DendriticModelType.LINEAR:
            output = input_current * attenuation * self.model_parameters["conductance_multiplier"]
        elif self.model_type == DendriticModelType.NONLINEAR:
            output = (input_current * attenuation) ** self.model_parameters["nonlinearity"] * self.model_parameters["conductance_multiplier"]
            output = min(output, self.model_parameters["saturation"])
        elif self.model_type == DendriticModelType.SEGMENTED:
            output = 0.0
            # Simplified compartmental model
            for i in range(self.model_parameters["num_segments"]):
                segment_attenuation = max(0, 1 - (i + 1) / self.model_parameters["num_segments"])
                output += input_current * segment_attenuation * self.model_parameters["conductance_multiplier"]
            output /= self.model_parameters["num_segments"]
        elif self.model_type == DendriticModelType.BRANCHING:
            output = input_current * attenuation * self.model_parameters["conductance_multiplier"]
        else:  # COMPARTMENTAL
            output = input_current * attenuation * self.model_parameters["conductance_multiplier"]
        
        # Check threshold
        if output >= self.model_parameters["threshold"]:
            self.spike_probability = min(1.0, self.spike_probability + 0.1)
        else:
            self.spike_probability *= 0.9
        
        return output
    
    def update(self, dt: float = 1.0) -> Tuple[bool, float]:
        """Update segment state.
        
        Args:
            dt: Time step (ms)
            
        Returns:
            Tuple of (spiked, output_potential)
        """
        # Update potential based on current
        dV = -self.current * dt / self.capacitance
        self.potential += dV
        
        # Update current
        self.current = self.potential / self.resistance
        
        # Check for spike
        spiked = self.potential >= 20.0  # Spike threshold (mV)
        
        if spiked:
            self.potential = -70.0  # Reset
        
        return spiked, self.potential
    
    def get_input_compartment(self, position: Tuple[float, float, float]) -> Optional[int]:
        """Get input compartment for given position.
        
        Args:
            position: Position (x, y, z)
            
        Returns:
            Compartment index or None
        """
        if self.model_type == DendriticModelType.SEGMENTED:
            dist = np.sqrt(position[0]**2 + position[1]**2 + position[2]**2)
            return int(dist / self.model_parameters["segment_length"])
        else:
            return 0  # Single compartment for other models


@dataclass
class AxonalSegment:
    """Axonal segment with propagation model."""
    
    def __init__(self,
                 length: float = 1000.0,
                 diameter: float = 1.0,
                 model_type: AxonalModelType = AxonalModelType.ACTIVE,
                 myelinated: bool = False):
        """Initialize axonal segment.
        
        Args:
            length: Axon length (µm)
            diameter: Axon diameter (µm)
            model_type: Type of axonal model
            myelinated: Whether axon is myelinated
        """
        self.length = length
        self.diameter = diameter
        self.model_type = model_type
        self.myelinated = myelinated
        
        # Axon properties
        self.resistance_per_unit = 100.0 / (np.pi * (diameter/2)**2)  # Ω
        self.capacitance_per_unit = 1.0 * np.pi * (diameter/2)  # µF
        self.conductance = 1.0 / self.resistance_per_unit
        
        # State variables
        self.potential = -70.0  # Resting potential (mV)
        self.current = 0.0
        self.spike propagating = False
        self.spike_queue = []
        
        # Axon-specific parameters
        self.model_parameters = self._initialize_model_parameters()
    
    def _initialize_model_parameters(self) -> Dict[str, Any]:
        """Initialize model-specific parameters."""
        if self.model_type == AxonalModelType.PASSIVE:
            return {
                "membrane_resistance": 1000.0,
                "time_constant": 10.0,
                "decay_factor": 0.95
            }
        elif self.model_type == AxonalModelType.ACTIVE:
            return {
                "threshold": -55.0,
                "refractory_period": 2.0,
                "max_conductance": 10.0,
                "activation_speed": 0.1
            }
        elif self.model_type == AxonalModelType.SALTATORY:
            return {
                "node_spacing": 1.0,
                "node_conductance": 1.0,
                "internode_resistance": 100.0
            }
        elif self.model_type == AxonalModelType.MYELINATED:
            return {
                "internode_length": 10.0,
                "node_conductance": 1.0,
                "internode_resistance": 1000.0,
                "saltatory_factor": 5.0
            }
        else:  # REGENERATIVE
            return {
                "regeneration_factor": 1.0,
                "threshold": -40.0,
                "refractory_period": 1.0,
                "max_regeneration": 2.0
            }
    
    def propagate_spike(self, spike_amplitude: float = 100.0, delay: float = 0.0) -> None:
        """Propagate spike along axon.
        
        Args:
            spike_amplitude: Spike amplitude (mV)
            delay: Transmission delay (ms)
        """
        self.spike_queue.append({
            "time": len(self.spike_queue) * self.model_parameters.get("time_step", 1.0) + delay,
            "amplitude": spike_amplitude
        })
    
    def update(self, dt: float = 1.0) -> Tuple[bool, float]:
        """Update axon segment state.
        
        Args:
            dt: Time step (ms)
            
        Returns:
            Tuple of (spiked, output_potential)
        """
        # Check for queued spikes
        if self.spike_queue and self.spike_queue[0]["time"] <= dt:
            spike = self.spike_queue.pop(0)
            
            # Apply model-specific spike processing
            if self.model_type == AxonalModelType.PASSIVE:
                # Passive propagation
                self.potential += spike["amplitude"] * self.model_parameters["decay_factor"]
            elif self.model_type == AxonalModelType.ACTIVE:
                # Active propagation with threshold
                if self.potential + spike["amplitude"] >= self.model_parameters["threshold"]:
                    self.potential = self.model_parameters["threshold"]
                    self.spike_propagating = True
            elif self.model_type == AxonalModelType.SALTATORY:
                # Saltatory propagation
                self.potential += spike["amplitude"] * self.model_parameters["node_conductance"]
            elif self.model_type == AxonalModelType.MYELINATED:
                # Myelinated propagation (saltatory)
                self.potential += spike["amplitude"] * self.model_parameters["saltatory_factor"]
            else:  # REGENERATIVE
                # Regenerative propagation
                if self.potential + spike["amplitude"] >= self.model_parameters["threshold"]:
                    self.potential = self.model_parameters["threshold"] * self.model_parameters["regeneration_factor"]
                    self.spike_propagating = True
            
            return True, self.potential
        
        # Update passive properties
        if self.model_type == AxonalModelModelType.PASSIVE:
            # Passive decay
            self.potential *= self.model_parameters["decay_factor"]
        
        # Check if still propagating
        if self.spike_propagating:
            self.spike_propagating = False
        
        return False, self.potential
    
    def get_signal_velocity(self) -> float:
        """Calculate signal propagation velocity.
        
        Returns:
            Signal velocity (m/s)
        """
        if self.model_type == AxonalModelType.MYELINATED:
            # Myelinated axons have faster propagation
            return 120.0  # m/s
        elif self.model_type == AxonalModelType.SALTATORY:
            # Saltatory propagation
            return 80.0  # m/s
        elif self.model_type == AxonalModelType.PASSIVE:
            # Passive propagation
            return 0.5  # m/s
        else:
            # Active or regenerative propagation
            return 20.0  # m/s


@dataclass
class NeuralModelBuilder:
    """Builder for custom neural models."""
    
    def __init__(self):
        """Initialize the neural model builder."""
        self.neuron_templates = {}
        self.synaptic_templates = {}
        self.dendritic_templates = {}
        self.axial_templates = {}
    
    def create_adaptive_neuron_model(self,
                                     num_neurons: int = 10,
                                     neuron_type: NeuronType = NeuronType.ADAPTIVE_THRESHOLD,
                                     connectivity_probability: float = 0.1) -> Dict[str, Any]:
        """Create adaptive neuron model.
        
        Args:
            num_neurons: Number of neurons in the model
            neuron_type: Type of neuron model
            connectivity_probability: Probability of synaptic connection
            
        Returns:
            Neural model
        """
        model = {
            "name": f"adaptive_neuron_model_{int(time.time())}",
            "type": "adaptive_neuron",
            "neuron_type": neuron_type.value,
            "num_neurons": num_neurons,
            "neurons": {},
            "synapses": {},
            "dendrites": {},
            "axons": {},
            "parameters": {
                "connectivity_probability": connectivity_probability,
                "simulation_dt": 1.0,
                "max_time": 1000.0
            }
        }
        
        # Create neurons
        for i in range(num_neurons):
            neuron_id = f"neuron_{i}"
            model["neurons"][neuron_id] = {
                "type": neuron_type.value,
                "position": (np.random.uniform(-100, 100), np.random.uniform(-100, 100)),
                "state": self._get_initial_neuron_state(neuron_type)
            }
        
        # Create synapses
        for pre_id, pre_neuron in model["neurons"].items():
            for post_id, post_neuron in model["neurons"].items():
                if pre_id != post_id and np.random.random() < connectivity_probability:
                    synapse_id = f"synapse_{pre_id}_{post_id}"
                    model["synapses"][synapse_id] = {
                        "source": pre_id,
                        "target": post_id,
                        "weight": np.random.normal(0, 1),
                        "type": "plastic" if neuron_type != NeuronType.POISSON else "static"
                    }
        
        return model
    
    def create_advanced_neural_network(self,
                                       architecture: str = "deep_feedforward",
                                       num_layers: int = 3,
                                       neurons_per_layer: int = 50,
                                       activation_type: str = "relu",
                                       learning_rule: str = "STDP") -> Dict[str, Any]:
        """Create advanced neural network.
        
        Args:
            architecture: Network architecture type
            num_layers: Number of layers
            neurons_per_layer: Number of neurons per layer
            activation_type: Activation function type
            learning_rule: Learning rule type
            
        Returns:
            Neural network model
        """
        model = {
            "name": f"advanced_neural_network_{int(time.time())}",
            "type": "advanced_neural_network",
            "architecture": architecture,
            "num_layers": num_layers,
            "neurons_per_layer": neurons_per_layer,
            "activation_type": activation_type,
            "learning_rule": learning_rule,
            "layers": {},
            "connections": {},
            "parameters": {
                "input_size": neurons_per_layer,
                "output_size": neurons_per_layer,
                "learning_rate": 0.01,
                "weight_init_std": 0.1
            }
        }
        
        # Create layers
        for layer in range(num_layers):
            layer_id = f"layer_{layer}"
            model["layers"][layer_id] = {
                "neurons": {},
                "type": "hidden" if layer < num_layers - 1 else "output",
                "activation": activation_type
            }
            
            # Create neurons in layer
            for neuron_idx in range(neurons_per_layer):
                neuron_id = f"{layer_id}_neuron_{neuron_idx}"
                model["layers"][layer_id]["neurons"][neuron_id] = {
                    "type": "adaptive_threshold",
                    "position": (layer, neuron_idx),
                    "state": self._get_initial_neuron_state(NeuronType.ADAPTIVE_THRESHOLD),
                    "dendrites": self._create_dendrites(neuron_id),
                    "axon": self._create_axon(neuron_id)
                }
        
        # Create connections between layers
        if architecture == "deep_feedforward":
            for layer in range(num_layers - 1):
                prev_layer_id = f"layer_{layer}"
                next_layer_id = f"layer_{layer + 1}"
                
                for pre_neuron_id, pre_neuron in model["layers"][prev_layer_id]["neurons"].items():
                    for post_neuron_id, post_neuron in model["layers"][next_layer_id]["neurons"].items():
                        connection_id = f"conn_{pre_neuron_id}_{post_neuron_id}"
                        model["connections"][connection_id] = {
                            "source": pre_neuron_id,
                            "target": post_neuron_id,
                            "weight": np.random.normal(0, model["parameters"]["weight_init_std"]),
                            "type": learning_rule,
                            "delay": np.random.uniform(0, 5),
                            "plasticity": True
                        }
        
        return model
    
    def _get_initial_neuron_state(self, neuron_type: NeuronType) -> Dict[str, Any]:
        """Get initial state for neuron type.
        
        Args:
            neuron_type: Neuron type
            
        Returns:
            Initial neuron state
        """
        if neuron_type == NeuronType.ADAPTIVE_THRESHOLD:
            return {
                "v_rest": -70.0,
                "v_th": -50.0,
                "v_reset": -65.0,
                "tau_m": 10.0,
                "tau_adp": 100.0,
                "a_adp": 4.0,
                "b_adp": 80.0,
                "learning_rate": 0.01,
                "v": -70.0,
                "i_adp": 0.0,
                "spike_count": 0,
                "spike_times": [],
                "refractory_period": 0.0
            }
        elif neuron_type == NeuronType.SPIKE_FREQ_ADAPTATION:
            return {
                "v_rest": -70.0,
                "v_th": -50.0,
                "v_reset": -65.0,
                "tau_m": 10.0,
                "tau_adapt": 50.0,
                "a_adapt": 0.5,
                "b_adapt": 0.1,
                "learning_rate": 0.01,
                "v": -70.0,
                "w": 0.0,
                "spike_count": 0,
                "spike_times": [],
                "refractory_period": 0.0
            }
        else:
            return {
                "v_rest": -70.0,
                "v_th": -50.0,
                "v_reset": -65.0,
                "tau_m": 10.0,
                "v": -70.0,
                "spike_count": 0,
                "spike_times": []
            }
    
    def _create_dendrites(self, neuron_id: str) -> Dict[str, Any]:
        """Create dendrites for neuron.
        
        Args:
            neuron_id: Neuron ID
            
        Returns:
            Dendrites configuration
        """
        return {
            "dendrite_1": {
                "type": "nonlinear",
                "length": 100.0,
                "radius": 1.0,
                "compartments": 10
            },
            "dendrite_2": {
                "type": "linear",
                "length": 80.0,
                "radius": 0.8,
                "compartments": 8
            }
        }
    
    def _create_axon(self, neuron_id: str) -> Dict[str, Any]:
        """Create axon for neuron.
        
        Args:
            neuron_id: Neuron ID
            
        Returns:
            Axon configuration
        """
        return {
            "axon_type": "active",
            "length": 1000.0,
            "diameter": 1.0,
            "myelinated": True,
            "propagation_speed": 120.0
        }


class AdvancedNeuralModel:
    """Advanced neural model with custom neuron and synaptic models."""
    
    def __init__(self, model: Dict[str, Any]):
        """Initialize advanced neural model.
        
        Args:
            model: Neural model
        """
        self.model = model
        self.builder = NeuralModelBuilder()
    
    def simulate(self, duration: float, input_patterns: List[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Simulate neural model.
        
        Args:
            duration: Simulation duration (ms)
            input_patterns: Input patterns (neuron_id: input_current)
            
        Returns:
            Simulation results
        """
        results = {
            "time_steps": [],
            "neuron_states": {},
            "spike_traces": {},
            "input_responses": {},
            "performance_metrics": {}
        }
        
        # Initialize state
        dt = self.model["parameters"]["simulation_dt"]
        time_steps = int(duration / dt)
        
        # Initialize neuron states
        neuron_states = {}
        for neuron_id, neuron_data in self.model["neurons"].items():
            neuron_states[neuron_id] = neuron_data["state"].copy()
        
        # Initialize input patterns
        if input_patterns is None:
            input_patterns = {}
            for neuron_id in self.model["neurons"].keys():
                input_patterns[neuron_id] = []
        
        # Simulation loop
        for step in range(time_steps):
            time = step * dt
            results["time_steps"].append(time)
            
            # Update each neuron
            for neuron_id, neuron_data in self.model["neurons"].items():
                neuron_type = neuron_data["type"]
                state = neuron_states[neuron_id]
                
                # Get input current
                input_current = input_patterns.get(neuron_id, [0])[step] if step < len(input_patterns.get(neuron_id, [])) else 0
                
                # Update neuron based on type
                if neuron_type == NeuronType.ADAPTIVE_THRESHOLD:
                    spiked, v = self._update_adaptive_threshold_neuron(state, input_current, dt)
                    state["v"] = v
                elif neuron_type == NeuronType.SPIKE_FREQ_ADAPTATION:
                    spiked, v = self._update_spike_freq_adapt_neuron(state, input_current, dt)
                    state["v"] = v
                else:
                    spiked, v = self._update_simple_neuron(state, input_current, dt)
                    state["v"] = v
                
                # Record spike
                if neuron_id not in results["spike_traces"]:
                    results["spike_traces"][neuron_id] = []
                
                results["spike_traces"][neuron_id].append({
                    "time": time,
                    "voltage": v,
                    "spiked": spiked
                })
                
                if spiked:
                    state["spike_count"] += 1
                    state["spike_times"].append(time)
                
                # Store neuron state
                results["neuron_states"][neuron_id] = {
                    "time": time,
                    "voltage": v,
                    "spiked": spiked,
                    "spike_count": state["spike_count"],
                    "adaptation": state.get("w", 0.0),  # Adaptation variable
                    "refractory": state.get("refractory_period", 0.0)
                }
            
            # Update synapses
            for connection_id, connection_data in self.model["synapses"].items():
                pre_neuron_id = connection_data["source"]
                post_neuron_id = connection_data["target"]
                
                # Check for pre-synaptic spike
                pre_spiked = False
                if pre_neuron_id in results["spike_traces"]:
                    pre_spiked = results["spike_traces"][pre_neuron_id][-1]["spiked"]
                
                # Check for post-synaptic spike
                post_spiked = False
                if post_neuron_id in results["spike_traces"]:
                    post_spiked = results["spike_traces"][post_neuron_id][-1]["spiked"]
                
                # Update synapse weight if plastic
                if connection_data["type"] == "plastic" and pre_spiked:
                    self._update_synaptic_plasticity(connection_id, pre_spiked, post_spiked)
        
        # Calculate performance metrics
        results["performance_metrics"] = self._calculate_performance_metrics(results)
        
        return results
    
    def _update_adaptive_threshold_neuron(self, state: Dict[str, Any], input_current: float, dt: float) -> Tuple[bool, float]:
        """Update adaptive threshold neuron.
        
        Args:
            state: Neuron state
            input_current: Input current
            dt: Time step
            
        Returns:
            Tuple of (spiked, voltage)
        """
        v = state["v"]
        
        # Update membrane potential
        dv = ((-v + state["v_rest"]) + input_current) / state["tau_m"] * dt
        v += dv
        
        # Update adaptation current
        di_adp = (-state["i_adp"]) / state["tau_adp"] * dt
        state["i_adp"] += di_adp
        
        # Check for spike
        threshold = state["v_th"] + state["i_adp"] * state["b_adp"]
        spiked = v >= threshold
        
        if spiked:
            # Reset neuron
            v = state["v_reset"]
            state["refractory_period"] = 2.0  # ms
            
            # Update adaptation current
            state["i_adp"] += state["a_adp"]
        
        return spiked, v
    
    def _update_spike_freq_adapt_neuron(self, state: Dict[str, Any], input_current: float, dt: float) -> Tuple[bool, float]:
        """Update spike-frequency adaptation neuron.
        
        Args:
            state: Neuron state
            input_current: Input current
            dt: Time step
            
        Returns:
            Tuple of (spiked, voltage)
        """
        v = state["v"]
        
        # Update membrane potential with adaptation
        dv = ((-v + state["v_rest"]) + input_current - state["w"]) / state["tau_m"] * dt
        v += dv
        
        # Update adaptation variable
        dw = (-state["w"]) / state["tau_adapt"] * dt
        state["w"] += dw
        
        # Check for spike
        threshold = state["v_th"] - state["b_adapt"] * state["w"]
        spiked = v >= threshold
        
        if spiked:
            # Reset neuron
            v = state["v_reset"]
            state["refractory_period"] = 2.0  # ms
            
            # Increase adaptation variable
            state["w"] += state["a_adapt"]
        
        return spiked, v
    
    def _update_simple_neuron(self, state: Dict[str, Any], input_current: float, dt: float) -> Tuple[bool, float]:
        """Update simple neuron model.
        
        Args:
            state: Neuron state
            input_current: Input current
            dt: Time step
            
        Returns:
            Tuple of (spiked, voltage)
        """
        v = state["v"]
        
        # Update membrane potential
        dv = ((-v + state["v_rest"]) + input_current) / state["tau_m"] * dt
        v += dv
        
        # Check for spike
        spiked = v >= state["v_th"]
        
        if spiked:
            # Reset neuron
            v = state["v_reset"]
            state["refractory_period"] = 2.0  # ms
        
        return spiked, v
    
    def _update_synaptic_plasticity(self, connection_id: str, pre_spike: bool, post_spike: bool) -> None:
        """Update synaptic plasticity.
        
        Args:
            connection_id: Connection ID
            pre_spike: Whether presynaptic neuron spiked
            post_spike: Whether postsynaptic neuron spiked
        """
        if connection_id in self.model["synapses"]:
            connection = self.model["synapses"][connection_id]
            
            # Apply STDP if enabled
            if connection["type"] == "STDP":
                if pre_spike and post_spike:
                    # Coincidence detection
                    connection["weight"] += 0.01
                elif pre_spike and not post_spike:
                    # Depression
                    connection["weight"] -= 0.005
                elif not pre_spike and post_spike:
                    # Potentiation
                    connection["weight"] += 0.01
    
    def _calculate_performance_metrics(self, simulation_results: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate performance metrics from simulation results.
        
        Args:
            simulation_results: Simulation results
            
        Returns:
            Performance metrics
        """
        metrics = {}
        
        if not simulation_results["spike_traces"]:
            return metrics
        
        # Calculate firing rates for each neuron
        firing_rates = {}
        for neuron_id, spike_trace in simulation_results["spike_traces"].items():
            spike_times = [point["time"] for point in spike_trace if point["spiked"]]
            if spike_times:
                duration = max(spike_times) - min(spike_times) if len(spike_times) > 1 else 100.0
                firing_rates[neuron_id] = len(spike_times) / (duration / 1000.0)
            else:
                firing_rates[neuron_id] = 0.0
        
        metrics["firing_rates"] = firing_rates
        metrics["mean_firing_rate"] = np.mean(list(firing_rates.values())) if firing_rates else 0.0
        
        # Calculate synchrony
        if len(simulation_results["spike_traces"]) >= 2:
            spike_counts = []
            for neuron_id, spike_trace in simulation_results["spike_traces"].items():
                spike_counts.append([point["time"] for point in spike_trace if point["spiked"]])
            
            # Calculate pairwise synchrony
            synchrony_matrix = np.zeros((len(spike_counts), len(spike_counts)))
            for i, spikes_i in enumerate(spike_counts):
                for j, spikes_j in enumerate(spike_counts):
                    if i != j:
                        # Calculate time difference between spikes
                        time_diffs = []
                        for t_i in spikes_i:
                            for t_j in spikes_j:
                                time_diffs.append(abs(t_i - t_j))
                        
                        if time_diffs:
                            avg_time_diff = np.mean(time_diffs)
                            synchrony = max(0, 1 - avg_time_diff / 100.0)  # Normalize by 100ms
                            synchrony_matrix[i, j] = synchrony
            
            metrics["synchrony_matrix"] = synchrony_matrix.tolist()
            metrics["mean_synchrony"] = np.mean(synchrony_matrix) if synchrony_matrix.size > 0 else 0.0
        
        return metrics


def main():
    """Example usage of AdvancedNeuralModel."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Advanced Neural Models")
    parser.add_argument("--create", action="store_true", help="Create a neural model")
    parser.add_argument("--type", dest="model_type", choices=[m.value for m in NeuronType],
                       help="Neuron type")
    parser.add_argument("--network", action="store_true", help="Create neural network")
    parser.add_argument("--name", help="Model name")
    parser.add_argument("--simulate", action="store_true", help="Simulate model")
    parser.add_argument("--steps", type=float, default=1000.0, help="Simulation duration (ms)")
    parser.add_argument("--config", help="Model configuration file")
    parser.add_argument("--import", dest="import_file", help="Import model from file")
    parser.add_argument("--export", help="Export model to file")
    parser.add_argument("--analyze", action="store_true", help="Analyze model")
    
    args = parser.parse_args()
    
    if args.import_file:
        print(f"Importing neural model from {args.import_file}...")
        with open(args.import_file, 'r') as f:
            model = json.load(f)
        
        neural_model = AdvancedNeuralModel(model)
        print(f"✓ Neural model imported successfully")
        print(f"  Model name: {model.get('name', 'Unnamed')}")
        print(f"  Type: {model.get('type', 'Unknown')}")
        print(f"  Neurons: {len(model.get('neurons', {}))}")
        
        # Simulate if requested
        if args.simulate:
            print(f"\nSimulating model for {args.steps} ms...")
            results = neural_model.simulate(args.steps)
            print(f"✓ Simulation completed")
            print(f"  Mean firing rate: {results['performance_metrics'].get('mean_firing_rate', 0):.2f} Hz")
        
        # Analyze if requested
        if args.analyze:
            print(f"\nAnalyzing model...")
            # Analysis would be performed here
            print(f"✓ Analysis completed (placeholder)")
        
        # Export if requested
        if args.export:
            neural_model.model['exported_at'] = datetime.now().isoformat()
            with open(args.export, 'w') as f:
                json.dump(neural_model.model, f, indent=2)
            print(f"\n✓ Model exported to {args.export}")
    
    elif args.create and args.model_type:
        print(f"Creating {args.model_type} neural model...")
        
        builder = NeuralModelBuilder()
        
        # Create adaptive neuron model
        neuron_model = builder.create_adaptive_neuron_model(
            num_neurons=10,
            neuron_type=NeuronType.ADAPTIVE_THRESHOLD,
            connectivity_probability=0.1
        )
        
        # Create advanced neural network
        network_model = builder.create_advanced_neural_network(
            architecture="deep_feedforward",
            num_layers=3,
            neurons_per_layer=50,
            activation_type="relu",
            learning_rule="STDP"
        )
        
        # Use network model as the main model
        model = network_model
        
        print(f"✓ Neural model created successfully")
        print(f"  Model name: {model.get('name', 'Unnamed')}")
        print(f"  Type: {model.get('type', 'Unknown')}")
        print(f"  Layers: {len(model.get('layers', {}))}")
        print(f"  Connections: {len(model.get('connections', {}))}")
        
        # Simulate if requested
        if args.simulate:
            print(f"\nSimulating model for {args.steps} ms...")
            
            # Create simple input pattern
            input_patterns = {}
            for neuron_id in model["layers"]["layer_0"]["neurons"].keys():
                input_patterns[neuron_id] = [10.0] * int(args.steps / model["parameters"]["simulation_dt"])
            
            results = AdvancedNeuralModel(model).simulate(args.steps, input_patterns)
            print(f"✓ Simulation completed")
            print(f"  Mean firing rate: {results['performance_metrics'].get('mean_firing_rate', 0):.2f} Hz")
        
        # Export if requested
        if args.export:
            model['exported_at'] = datetime.now().isoformat()
            with open(args.export, 'w') as f:
                json.dump(model, f, indent=2)
            print(f"\n✓ Model exported to {args.export}")
    
    else:
        print("Please specify either --create with --type or --import to load an existing model")
        print("\nExamples:")
        print("  python scripts/advanced_neural.py --create --type adaptive_threshold --simulate --steps 1000")
        print("  python scripts/advanced_neural.py --import my_model.json --simulate")


if __name__ == "__main__":
    main()
