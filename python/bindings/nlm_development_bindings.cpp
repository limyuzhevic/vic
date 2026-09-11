"""
Phase 6 Development System Python Bindings

This module provides Python bindings for the Phase 6 development system:
- Developmental stages progression (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Plasticity modulation based on developmental stage
- Synaptogenesis and pruning during development
- Structural plasticity changes over time
- Cognitive development and maturation
"""

import pybind11
import pybind11.stl
from typing import Optional, Any

# Import core NLM types
from ..core.types.types import (
    SimulationStep, Timestamp, TimestepDuration,
    DevelopmentalStage
)

# Import core NLM classes
from ..brain.brain import Brain
from ..core.random.random import RandomGenerator
from ..development.development_system import DevelopmentSystem
from ..development.synaptogenesis import Synaptogenesis
from ..development.pruning import Pruning
from ..development.maturation import Maturation

__all__ = [
    # Developmental stages
    'DevelopmentalStage',
    
    # Development system
    'DevelopmentSystem',
    
    # Developmental subsystems
    'Synaptogenesis', 'Pruning', 'Maturation',
    
    # Helper types
]

// Create Python module for development system
module = pybind11::module_::module("nlm_development");

// Bind DevelopmentalStage enum
pybind11::enum_<DevelopmentalStage> developmental_stage_enum(
    module,
    "DevelopmentalStage",
    "Developmental stage enumeration"
);

developmental_stage_enum
    .value("Initial", DevelopmentalStage::Initial)
    .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
    .value("Maturation", DevelopmentalStage::Maturation)
    .value("Adult", DevelopmentalStage::Adult)
    .value("Aging", DevelopmentalStage::Aging)
    .export_values();

// Bind DevelopmentSystem class
pybind11::class_<DevelopmentSystem> development_system_class(
    module,
    "DevelopmentSystem",
    "Development system: orchestrates neural development"
);

development_system_class
    .def(pybind11::init<>())
    .def("getStage", &DevelopmentSystem::getStage,
         "Get current developmental stage")
    .def("setStage", &DevelopmentSystem::setStage, pybind11::arg("stage"),
         "Set developmental stage")
    .def("advanceStage", &DevelopmentSystem::advanceStage,
         "Advance developmental stage")
    .def("getStageName", &DevelopmentSystem::getStageName,
         "Get stage name")
    .def("update", pybind11::overload_cast_<Brain*, SimulationStep>(
        &DevelopmentSystem::update
    ), pybind11::arg("brain"), pybind11::arg("currentStep"),
         "Update development (simple version)")
    .def("update", pybind11::overload_cast_<Brain*, RandomGenerator&, TimestepDuration>(
        &DevelopmentSystem::update
    ), pybind11::arg("brain"), pybind11::arg("rng"), pybind11::arg("dt"),
         "Update development with full parameters")
    .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
         "Get plasticity modifier for current stage")
    .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
         "Get critical period info")
    .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
         "Get critical period progress")
    .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
         "Get developmental age (in simulation time)")
    .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge,
         pybind11::arg("age"), "Set developmental age");

// Bind Synaptogenesis class
pybind11::class_<Synaptogenesis> synaptogenesis_class(
    module,
    "Synaptogenesis",
    "Synaptogenesis: new synapse formation"
);

synaptogenesis_class
    .def(pybind11::init<>())
    .def("getFormationRate", &Synaptogenesis::getFormationRate,
         "Get formation rate")
    .def("setFormationRate", &Synaptogenesis::setFormationRate,
         pybind11::arg("rate"), "Set formation rate")
    .def("getTargetDensity", &Synaptogenesis::getTargetDensity,
         "Get target density")
    .def("setTargetDensity", &Synaptogenesis::setTargetDensity,
         pybind11::arg("density"), "Set target density")
    .def("update", &Synaptogenesis::update, pybind11::arg("brain"), 
         pybind11::arg("rng"), "Update synaptogenesis");

// Bind Pruning class
pybind11::class_<Pruning> pruning_class(
    module,
    "Pruning",
    "Pruning: weak synapse removal"
);

pruning_class
    .def(pybind11::init<>())
    .def("getPruningRate", &Pruning::getPruningRate,
         "Get pruning rate")
    .def("setPruningRate", &Pruning::setPruningRate,
         pybind11::arg("rate"), "Set pruning rate")
    .def("getThreshold", &Pruning::getThreshold,
         "Get pruning threshold")
    .def("setThreshold", &Pruning::setThreshold,
         pybind11::arg("threshold"), "Set pruning threshold")
    .def("update", &Pruning::update, pybind11::arg("brain"), 
         pybind11::arg("rng"), "Update pruning");

// Bind Maturation class
pybind11::class_<Maturation> maturation_class(
    module,
    "Maturation",
    "Maturation: adult property development"
);

maturation_class
    .def(pybind11::init<>())
    .def("getMaturationRate", &Maturation::getMaturationRate,
         "Get maturation rate")
    .def("setMaturationRate", &Maturation::setMaturationRate,
         pybind11::arg("rate"), "Set maturation rate")
    .def("getNeuralComplexityTarget", &Maturation::getNeuralComplexityTarget,
         "Get neural complexity target")
    .def("setNeuralComplexityTarget", &Maturation::setNeuralComplexityTarget,
         pybind11::arg("target"), "Set neural complexity target")
    .def("update", &Maturation::update, pybind11::arg("brain"), 
         pybind11::arg("rng"), "Update maturation");

// Helper functions
module.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
    return std::make_shared<DevelopmentSystem>();
}, "Create a development system");

module.def("createSynaptogenesis", []() -> std::shared_ptr<Synaptogenesis> {
    return std::make_shared<Synaptogenesis>();
}, "Create a synaptogenesis system");

module.def("createPruning", []() -> std::shared_ptr<Pruning> {
    return std::make_shared<Pruning>();
}, "Create a pruning system");

module.def("createMaturation", []() -> std::shared_ptr<Maturation> {
    return std::make_shared<Maturation>();
}, "Create a maturation system");

// Create submodule
PYBIND11_PLUGIN("nlm_development");
