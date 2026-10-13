#!/usr/bin/env python3
"""
NLM Enhanced Neuron Models Module for Python

This module provides Python interface to advanced neuron models including
LIF, Hodgkin-Huxley, and Izhikevich models with enhanced capabilities.
"""

import sys
import os
from typing import Dict, Any, Optional, List
from enum import Enum
import json

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

# Import the neuron model classes
try:
    from brain.NeuronModelFactory import NeuronModelFactory
    from brain.NeuronModel import NeuronModel, NeuronConfig, NeuronModelType
    from brain.LIFModel import LIFModel
    from brain.HHModel import HHModel
    from brain.IzhikevichModel import IzhikevichModel
    _CPP_BINDINGS_AVAILABLE = True
except ImportError as e:
    print(f"Warning: C++ neuron model bindings not available: {e}")
    _CPP_BINDINGS_AVAILABLE = False

# Neuron model enumeration
class NeuronModelType(Enum):
    LIF = "LIF"
    HODGKIN_HUXLEY = "Hodgkin-Huxley"
    IZHIKEVICH = "Izhikevich"

# Neuron type enumeration (from original system)
class NeuronType(Enum):
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    SENSORY = "Sensory"
    MOTOR = "Motor"
    INTERNAL = "Internal"

# Enhanced neuron configuration
class EnhancedNeuronConfig:
    """Enhanced neuron configuration with model-specific parameters"""
    
    def __init__(self):
        # Common parameters
        self.resting_potential = -70.0
        self.threshold = -55.0
        self.reset_potential = -70.0
        self.membrane_time_constant = 20.0
        self.refractory_period = 2.0
        self.adaptation_strength = 0.0
        self.timestep = 0.001
        self.noise_level = 0.0
        
        # Model-specific parameters
        self.model_type = NeuronModelType.LIF
        self.model_name = "LIF"
        
        # HH parameters
        self.max_sodium_conductance = 120.0
        self.min_potassium_conductance = 36.0
        self.leakage_conductance = 0.3
        self.sodium_reversal = 50.0
        self.potassium_reversal = -77.0
        
        # Izhikevich parameters
        self.a = 0.02
        self.b = 0.2
        self.c = -65.0
        self.d = 8.0
        
        # Simulation parameters
        self.use_adaptive_integration = False
        self.max_spike_history = 100
        
        # Type-specific parameters
        self.neuron_type = NeuronType.INTERNAL
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert config to dictionary"""
        return {
            'resting_potential': self.resting_potential,
            'threshold': self.threshold,
            'reset_potential': self.reset_potential,
            'membrane_time_constant': self.membrane_time_constant,
            'refractory_period': self.refractory_period,
            'adaptation_strength': self.adaptation_strength,
            'timestep': self.timestep,
            'noise_level': self.noise_level,
            'model_type': self.model_type.value,
            'model_name': self.model_name,
            'max_sodium_conductance': self.max_sodium_conductance,
            'min_potassium_conductance': self.min_potassium_conductance,
            'leakage_conductance': self.leakage_conductance,
            'sodium_reversal': self.sodium_reversal,
            'potassium_reversal': self.potassium_reversal,
            'a': self.a,
            'b': self.b,
            'c': self.c,
            'd': self.d,
            'use_adaptive_integration': self.use_adaptive_integration,
            'max_spike_history': self.max_spike_history,
            'neuron_type': self.neuron_type.value,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'EnhancedNeuronConfig':
        """Create config from dictionary"""
        config = cls()
        for key, value in data.items():
            if hasattr(config, key):
                # Convert enum values if needed
                if isinstance(value, str) and key == 'model_type':
                    value = NeuronModelType(value)
                elif isinstance(value, str) and key == 'neuron_type':
                    value = NeuronType(value)
                elif isinstance(value, str) and key in ['model_name']:
                    pass  # Keep as string
                setattr(config, key, value)
        return config
    
    def set_model_type(self, model_type: NeuronModelType):
        """Set model type and update model-specific parameters"""
        self.model_type = model_type
        self.model_name = model_type.value
        
        # Reset model-specific parameters based on model type
        if model_type == NeuronModelType.LIF:
            # LIF parameters already set in __init__
            pass
        elif model_type == NeuronModelType.HODGKIN_HUXLEY:
            # Ensure HH parameters are set
            pass
        elif model_type == NeuronModelType.IZHIKEVICH:
            # Ensure Izhikevich parameters are set
            pass
    
    def get_model_specific_params(self) -> Dict[str, Any]:
        """Get model-specific parameters"""
        params = {}
        if self.model_type == NeuronModelType.LIF:
            params = {
                'type': 'LIF',
                'tau': self.membrane_time_constant,
                'adaptation_strength': self.adaptation_strength,
            }
        elif self.model_type == NeuronModelType.HODGKIN_HUXLEY:
            params = {
                'type': 'Hodgkin-Huxley',
                'max_sodium_conductance': self.max_sodium_conductance,
                'min_potassium_conductance': self.min_potassium_conductance,
                'leakage_conductance': self.leakage_conductance,
                'sodium_reversal': self.sodium_reversal,
                'potassium_reversal': self.potassium_reversal,
            }
        elif self.model_type == NeuronModelType.IZHIKEVICH:
            params = {
                'type': 'Izhikevich',
                'a': self.a,
                'b': self.b,
                'c': self.c,
                'd': self.d,
            }
        return params

class EnhancedNeuron:
    """Python interface to enhanced neuron with model support"""
    
    def __init__(self, neuron_id: int, neuron_type: NeuronType = NeuronType.INTERNAL):
        self.neuron_id = neuron_id
        self.neuron_type = neuron_type
        self.config = EnhancedNeuronConfig()
        self.config.neuron_type = neuron_type
        
        # Neuron state
        self.membrane_potential = self.config.resting_potential
        self.firing_rate = 0.0
        self.adaptation_variable = 0.0
        self.spike_count = 0
        self.active = False
        self.firing_state = "Resting"
        
        # History tracking
        self.spike_history = []
        self.activity_history = []
        
        # Model instance
        self.model_instance = None
        
        # Initialize model
        self._create_model()
    
    def _create_model(self):
        """Create neuron model based on configuration"""
        if not _CPP_BINDINGS_AVAILABLE:
            # Use Python fallback implementation
            self._create_python_model()
        else:
            # Use C++ implementation
            try:
                from brain.NeuronModelFactory import NeuronModelFactory
                self.model_instance = NeuronModelFactory.createModel(
                    self.neuron_type, 
                    self._convert_config_to_cpp()
                )
            except Exception as e:
                print(f"Warning: Could not create C++ model: {e}")
                self._create_python_model()
    
    def _create_python_model(self):
        """Create Python implementation of neuron model"""
        # Simplified Python implementation for demonstration
        model_name = self.config.model_type.value
        if model_name == "LIF":
            # LIF model in Python
            self.model_instance = {
                'type': 'LIF',
                'v': self.config.resting_potential,
                'v_reset': self.config.reset_potential,
                'threshold': self.config.threshold,
                'tau': self.config.membrane_time_constant,
                'refractory_remaining': 0,
                'adaptation': 0.0,
                'external_input': 0.0,
                'spiked': False,
                'C': 1.0,  # membrane capacitance
            }
        elif model_name == "Hodgkin-Huxley":
            # HH model in Python
            self.model_instance = {
                'type': 'Hodgkin-Huxley',
                'v': self.config.resting_potential,
                'm': 0.05,
                'h': 0.6,
                'n': 0.32,
                'dm_dt': 0.0,
                'dh_dt': 0.0,
                'dn_dt': 0.0,
                'refractory_remaining': 0,
                'external_input': 0.0,
                'spiked': False,
                'C': 1.0,
            }
        elif model_name == "Izhikevich":
            # Izhikevich model in Python
            self.model_instance = {
                'type': 'Izhikevich',
                'v': self.config.resting_potential,
                'u': 0.0,
                'a': self.config.a,
                'b': self.config.b,
                'c': self.config.c,
                'd': self.config.d,
                'refractory_remaining': 0,
                'external_input': 0.0,
                'spiked': False,
            }
    
    def _convert_config_to_cpp(self):
        """Convert Python config to C++ config structure"""
        if _CPP_BINDINGS_AVAILABLE:
            from brain.NeuronModel import NeuronConfig, NeuronModelType as CPPModelType
            
            cpp_config = NeuronConfig()
            cpp_config.restingPotential = self.config.resting_potential
            cpp_config.threshold = self.config.threshold
            cpp_config.resetPotential = self.config.reset_potential
            cpp_config.membraneTimeConstant = self.config.membrane_time_constant
            cpp_config.refractoryPeriod = self.config.refractory_period
            cpp_config.adaptationStrength = self.config.adaptation_strength
            
            # Convert model type
            if self.config.model_type == NeuronModelType.LIF:
                cpp_config.modelType = CPPModelType.LIF
            elif self.config.model_type == NeuronModelType.HODGKIN_HUXLEY:
                cpp_config.modelType = CPPModelType.HodgkinHuxley
            elif self.config.model_type == NeuronModelType.IZHIKEVICH:
                cpp_config.modelType = CPPModelType.Izhikevich
            
            cpp_config.modelName = self.config.model_name
            cpp_config.timestep = self.config.timestep
            cpp_config.noiseLevel = self.config.noise_level
            
            return cpp_config
        return None
    
    def set_external_input(self, input_value: float):
        """Set external input to neuron"""
        if self.model_instance and hasattr(self.model_instance, 'set_external_input'):
            self.model_instance.set_external_input(input_value)
        elif isinstance(self.model_instance, dict):
            self.model_instance['external_input'] = input_value
        else:
            self.membrane_potential += input_value
    
    def update(self, timestep: float, inputs: List[float] = None) -> bool:
        """Update neuron state with new inputs"""
        if not self.model_instance:
            return False
        
        if hasattr(self.model_instance, 'update'):
            # Use C++ model update method
            spiked = self.model_instance.update(timestep, inputs or [])
            if spiked:
                self.spike_count += 1
                self.active = True
                self.spike_history.append(timestep)
        elif isinstance(self.model_instance, dict):
            # Use Python model update
            model = self.model_instance
            if model['type'] == 'LIF':
                spiked = self._update_lif(timestep, inputs or [])
            elif model['type'] == 'Hodgkin-Huxley':
                spiked = self._update_hh(timestep, inputs or [])
            elif model['type'] == 'Izhikevich':
                spiked = self._update_izhikevich(timestep, inputs or [])
            else:
                spiked = False
        else:
            return False
        
        # Update Python attributes
        self._update_attributes_from_model()
        return spiked
    
    def _update_lif(self, timestep: float, inputs: List[float]) -> bool:
        """LIF model update in Python"""
        model = self.model_instance
        
        if model['refractory_remaining'] > 0:
            model['refractory_remaining'] -= timestep
            if model['refractory_remaining'] <= 0:
                model['refractory_remaining'] = 0
            return False
        
        # LIF integration
        I_total = model['external_input']
        if inputs:
            for input_val in inputs:
                I_total += input_val
        
        dv = (self.config.resting_potential - model['v']) / model['tau'] + I_total / model['C']
        model['v'] += dv * timestep
        
        # Adaptation
        if self.config.adaptation_strength > 0.0:
            model['adaptation'] += self.config.adaptation_strength
            model['v'] -= model['adaptation'] * 0.01
            model['adaptation'] *= 0.95
        
        # Spike detection
        if model['v'] >= self.config.threshold:
            model['spiked'] = True
            model['v'] = self.config.reset_potential
            model['refractory_remaining'] = self.config.refractory_period
            return True
        
        return False
    
    def _update_hh(self, timestep: float, inputs: List[float]) -> bool:
        """Hodgkin-Huxley model update in Python"""
        model = self.model_instance
        
        if model['refractory_remaining'] > 0:
            model['refractory_remaining'] -= timestep
            if model['refractory_remaining'] <= 0:
                model['refractory_remaining'] = 0
            return False
        
        # HH model equations (simplified)
        I_total = model['external_input']
        if inputs:
            for input_val in inputs:
                I_total += input_val
        
        # Calculate currents (simplified HH equations)
        I_Na = model['maxSodiumConductance'] * model['m']**3 * model['h'] * (model['v'] - 50.0)
        I_K = model['minPotassiumConductance'] * model['n']**4 * (model['v'] + 77.0)
        I_L = model['leakageConductance'] * (model['v'] + 70.0)
        
        I_ion = I_Na + I_K + I_L
        dv_dt = (I_total - I_ion) / model['C']
        
        # Update gating variables
        alpha_m = (0.1 * (model['v'] + 40)) / (1 - exp(-(model['v'] + 40) / 10))
        beta_m = 4 * exp(-(model['v'] + 65) / 18)
        alpha_h = 0.07 * exp(-(model['v'] + 65) / 20)
        beta_h = 1 / (exp(-(model['v'] + 35) / 10) + 1)
        alpha_n = (0.01 * (model['v'] + 55)) / (1 - exp(-(model['v'] + 55) / 10))
        beta_n = 0.125 * exp(-(model['v'] + 65) / 80)
        
        dm_dt = alpha_m * (1 - model['m']) - beta_m * model['m']
        dh_dt = alpha_h * (1 - model['h']) - beta_h * model['h']
        dn_dt = alpha_n * (1 - model['n']) - beta_n * model['n']
        
        # Update
        model['m'] += dm_dt * timestep
        model['h'] += dh_dt * timestep
        model['n'] += dn_dt * timestep
        model['v'] += dv_dt * timestep
        
        # Spike detection
        if model['v'] >= self.config.threshold:
            model['spiked'] = True
            model['v'] = self.config.reset_potential
            model['refractory_remaining'] = self.config.refractory_period
            return True
        
        return False
    
    def _update_izhikevich(self, timestep: float, inputs: List[float]) -> bool:
        """Izhikevich model update in Python"""
        model = self.model_instance
        
        if model['refractory_remaining'] > 0:
            model['refractory_remaining'] -= timestep
            if model['refractory_remaining'] <= 0:
                model['refractory_remaining'] = 0
            return False
        
        # Izhikevich equations
        I_total = model['external_input']
        if inputs:
            for input_val in inputs:
                I_total += input_val
        
        # Update membrane potential and adaptation variable
        dv = model['v'] * (1 - model['a']) * model['v'] - model['u'] + model['b'] + I_total
        du = model['d'] * (model['a'] * model['v'] - model['u'])
        
        model['v'] += dv * timestep
        model['u'] += du * timestep
        
        # Spike detection
        if model['v'] >= self.config.threshold:
            model['spiked'] = True
            model['v'] = model['c']
            model['u'] = model['u'] + model['d']
            model['refractory_remaining'] = self.config.refractory_period
            return True
        
        return False
    
    def _update_attributes_from_model(self):
        """Update Python attributes from model state"""
        if not self.model_instance:
            return
        
        if hasattr(self.model_instance, 'getMembranePotential'):
            self.membrane_potential = self.model_instance.getMembranePotential()
            self.firing_rate = self.model_instance.getFiringRate()
            self.adaptation_variable = self.model_instance.getAdaptationVariable()
            self.active = self.model_instance.hasSpiked()
        elif isinstance(self.model_instance, dict):
            self.membrane_potential = self.model_instance.get('v', self.config.resting_potential)
            self.adaptation_variable = self.model_instance.get('u', 0.0)
            self.active = self.model_instance.get('spiked', False)
        
        # Calculate firing rate based on spike history
        if self.spike_history:
            time_window = max(0.1, self.spike_history[-1] - self.spike_history[0])
            self.firing_rate = len(self.spike_history) / time_window
    
    def reset(self):
        """Reset neuron to initial state"""
        if self.model_instance and hasattr(self.model_instance, 'reset'):
            self.model_instance.reset()
        elif isinstance(self.model_instance, dict):
            if self.model_instance['type'] == 'LIF':
                self.model_instance['v'] = self.config.resting_potential
                self.model_instance['refractory_remaining'] = 0
                self.model_instance['adaptation'] = 0.0
                self.model_instance['external_input'] = 0.0
                self.model_instance['spiked'] = False
            elif self.model_instance['type'] == 'Hodgkin-Huxley':
                self.model_instance['v'] = self.config.resting_potential
                self.model_instance['m'] = 0.05
                self.model_instance['h'] = 0.6
                self.model_instance['n'] = 0.32
                self.model_instance['refractory_remaining'] = 0
                self.model_instance['external_input'] = 0.0
                self.model_instance['spiked'] = False
            elif self.model_instance['type'] == 'Izhikevich':
                self.model_instance['v'] = self.config.resting_potential
                self.model_instance['u'] = self.config.b * self.config.resting_potential
                self.model_instance['refractory_remaining'] = 0
                self.model_instance['external_input'] = 0.0
                self.model_instance['spiked'] = False
        
        # Reset Python attributes
        self.membrane_potential = self.config.resting_potential
        self.firing_rate = 0.0
        self.adaptation_variable = 0.0
        self.active = False
        self.spike_history.clear()
        self.activity_history.clear()
    
    def get_model_name(self) -> str:
        """Get name of current model"""
        if self.model_instance and hasattr(self.model_instance, 'getModelName'):
            return self.model_instance.getModelName()
        elif isinstance(self.model_instance, dict):
            return self.model_instance.get('type', 'Unknown')
        return 'Unknown'
    
    def get_model_type(self):
        """Get type of current model"""
        return self.config.model_type
    
    def set_model_type(self, model_type: NeuronModelType):
        """Change neuron model type"""
        self.config.set_model_type(model_type)
        self.reset()
        self._create_model()
    
    def add_spike(self, timestamp: float):
        """Manually add a spike"""
        self.spike_history.append(timestamp)
        self.spike_count += 1
        self.active = True
    
    def get_spike_count(self) -> int:
        """Get total spike count"""
        return self.spike_count
    
    def get_last_spike_time(self) -> Optional[float]:
        """Get last spike time"""
        if self.spike_history:
            return self.spike_history[-1]
        return None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert neuron to dictionary"""
        return {
            'id': self.neuron_id,
            'type': self.neuron_type.value,
            'model_name': self.get_model_name(),
            'model_type': self.config.model_type.value,
            'membrane_potential': self.membrane_potential,
            'firing_rate': self.firing_rate,
            "adaptation_variable': self.adaptation_variable,
            'active': self.active,
            'firing_state': self.firing_state,
            'spike_count': self.spike_count,
            'last_spike': self.get_last_spike_time(),
            'config': self.config.to_dict(),
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'EnhancedNeuron':
        """Create neuron from dictionary"""
        neuron = cls(
            neuron_id=data['id'],
            neuron_type=NeuronType(data['type'])
        )
        
        # Restore config
        if 'config' in data:
            neuron.config = EnhancedNeuronConfig.from_dict(data['config'])
        
        # Restore state
        neuron.membrane_potential = data.get('membrane_potential', neuron.config.resting_potential)
        neuron.firing_rate = data.get('firing_rate', 0.0)
        neuron.adaptation_variable = data.get('adaptation_variable', 0.0)
        neuron.active = data.get('active', False)
        neuron.firing_state = data.get('firing_state', 'Resting')
        neuron.spike_count = data.get('spike_count', 0)
        
        if 'last_spike' in data:
            neuron.spike_history = [data['last_spike']]
        
        return neuron

# Python bindings for enhanced neuron models
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R""pbdoc(
        NLM Enhanced Neuron Models Module
        -------------------------------
        Advanced neuron models with enhanced capabilities.
        Provides LIF, Hodgkin-Huxley, and Izhikevich models.
    )pbdoc"";
    
    py::enum_<NeuronModelType>(m, "NeuronModelType", R""pbdoc(Available neuron model types)pbdoc""")
        .value("LIF", NeuronModelType::LIF)
        .value("HodgkinHuxley", NeuronModelType::HodgkinHuxley)
        .value("Izhikevich", NeuronModelType::Izhikevich)
        .export_values();
    
    py::class_<NeuronType>(m, "NeuronType", R""pbdoc(Available neuron types)pbdoc""")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::class_<EnhancedNeuronConfig>(m, "EnhancedNeuronConfig", R""pbdoc(Neuron model configuration)pbdoc""")
        .def(py::init<>())
        .def_readwrite("resting_potential", &EnhancedNeuronConfig::resting_potential)
        .def_readwrite("threshold", &EnhancedNeuronConfig::threshold)
        .def_readwrite("reset_potential", &EnhancedNeuronConfig::reset_potential)
        .def_readwrite("membrane_time_constant", &EnhancedNeuronConfig::membrane_time_constant)
        .def_readwrite("refractory_period", &EnhancedNeuronConfig::refractory_period)
        .def_readwrite("adaptation_strength", &EnhancedNeuronConfig::adaptation_strength)
        .def_readwrite("timestep", &EnhancedNeuronConfig::timestep)
        .def_readwrite("noise_level", &EnhancedNeuronConfig::noise_level)
        .def_readwrite("model_type", &EnhancedNeuronConfig::model_type)
        .def_readwrite("model_name", &EnhancedNeuronConfig::model_name);
    
    py::class_<EnhancedNeuron>(m, "EnhancedNeuron", R""pbdoc(Enhanced neuron with model support)pbdoc""")
        .def(py::init<int, NeuronType>(), py::arg("id"), py::arg("type") = NeuronType::Internal)
        .def("set_external_input", &EnhancedNeuron::set_external_input, py::arg("input"), R""pbdoc(Set external input)pbdoc""")
        .def("update", &EnhancedNeuron::update, py::arg("timestep"), py::arg("inputs") = py::list(), R""pbdoc(Update neuron)pbdoc""")
        .def("reset", &EnhancedNeuron::reset, R""pbdoc(Reset neuron)pbdoc""")
        .def("get_model_name", &EnhancedNeuron::get_model_name, R""pbdoc(Get model name)pbdoc""")
        .def("get_model_type", &EnhancedNeuron::get_model_type, R""pbdoc(Get model type)pbdoc""")
        .def("set_model_type", &EnhancedNeuron::set_model_type, py::arg("model_type"), R""pbdoc(Set model type)pbdoc""")
        .def("get_membrane_potential", &EnhancedNeuron::get_membrane_potential, R""pbdoc(Get membrane potential)pbdoc""")
        .def("get_firing_rate", &EnhancedNeuron::get_firing_rate, R""pbdoc(Get firing rate)pbdoc""")
        .def("has_spiked", &EnhancedNeuron::has_spiked, R""pbdoc(Check if spiked)pbdoc""")
        .def("get_adaptation_variable", &EnhancedNeuron::get_adaptation_variable, R""pbdoc(Get adaptation variable)pbdoc""")
        .def("get_spike_count", &EnhancedNeuron::get_spike_count, R""pbdoc(Get spike count)pbdoc""")
        .def("get_last_spike_time", &EnhancedNeuron::get_last_spike_time, R""pbdoc(Get last spike time)pbdoc""")
        .def("to_dict", &EnhancedNeuron::to_dict, R""pbdoc(Convert to dictionary)pbdoc""")
        .def_static("from_dict", &EnhancedNeuron::from_dict, py::arg("data"), R""pbdoc(Create from dictionary)pbdoc"");
    
    // Convenience functions
    m.def("create_neuron", [](int id, std::string type_str) -> EnhancedNeuron {
        NeuronType type = NeuronType::Internal;
        if (type_str == "Excitatory") type = NeuronType::Excitatory;
        else if (type_str == "Inhibitory") type = NeuronType::Inhibitory;
        else if (type_str == "Modulatory") type = NeuronType::Modulatory;
        else if (type_str == "Sensory") type = NeuronType::Sensory;
        else if (type_str == "Motor") type = NeuronType::Motor;
        return EnhancedNeuron(id, type);
    }, py::arg("id"), py::arg("type") = "Internal", R""pbdoc(Create neuron)pbdoc"");
    
    m.def("create_lif_neuron", [](int id) {
        EnhancedNeuron neuron(id, NeuronType::Internal);
        neuron.set_model_type(NeuronModelType::LIF);
        return neuron;
    }, py::arg("id"), R""pbdoc(Create LIF neuron)pbdoc""");
    
    m.def("create_hh_neuron", [](int id) {
        EnhancedNeuron neuron(id, NeuronType::Internal);
        neuron.set_model_type(NeuronModelType::HodgkinHuxley);
        return neuron;
    }, py::arg("id"), R""pbdoc(Create Hodgkin-Huxley neuron)pbdoc""");
    
    m.def("create_izhikevich_neuron", [](int id) {
        EnhancedNeuron neuron(id, NeuronType::Internal);
        neuron.set_model_type(NeuronModelType::Izhikevich);
        return neuron;
    }, py::arg("id"), R""pbdoc(Create Izhikevich neuron)pbdoc""");
    
    // Example usage function
    m.def("example_usage", []() {
        std::cout << "Example enhanced neuron usage:\n";
        std::cout << "1. Create neuron with default model:\n";
        std::cout << "   neuron = EnhancedNeuron(1, 'Internal')\n\n";
        std::cout << "2. Set LIF model:\n";
        std::cout << "   neuron.set_model_type(NeuronModelType.LIF)\n\n";
        std::cout << "3. Update neuron:\n";
        std::cout << "   neuron.update(0.001, [0.1, 0.2])\n\n";
        std::cout << "4. Check state:\n";
        std::cout << "   potential = neuron.get_membrane_potential()\n";
        std::cout << "   spiked = neuron.has_spiked()\n";
        return "Example printed to stdout";
    }, R""pbdoc(Print usage examples)pbdoc"");
}

} // namespace nlm
