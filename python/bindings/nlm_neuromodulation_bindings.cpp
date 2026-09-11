"""
Phase 6 Neuromodulation System Python Bindings

This module provides Python bindings for all Phase 6 neuromodulation systems:
- Dopamine: Reward and reinforcement learning signal
- Curiosity: Exploration motivation based on novelty and prediction error
- Novelty: Novelty detection for learning
- PredictionError: Error signals for prediction and learning
"""

import pybind11
import pybind11.stl
import pybind11.functional
from typing import List, Optional, Any

# Import core NLM types
from ..core.types.types import (
    NeuronId, SimulationStep, Timestamp, TimestepDuration,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)

# Import core NLM classes
from ..brain.brain import Brain
from ..neuromodulation.neuromodulator import Neuromodulator
from ..neuromodulation.dopamine import Dopamine
from ..neuromodulation.curiosity import Curiosity
from ..neuromodulation.prediction_error import PredictionError
from ..neuromodulation.novelty import Novelty
from ..neuromodulation.acetylcholine import Acetylcholine
from ..neuromodulation.norepinephrine import Norepinephrine
from ..neuromodulation.serotonin import Serotonin
from ..environment.observation import Observation

__all__ = [
    # Neuromodulator base class
    'Neuromodulator',
    
    # Specific neuromodulators
    'Dopamine', 'Curiosity', 'PredictionError', 
    'Novelty', 'Acetylcholine', 'Norepinephrine', 'Serotonin',
    
    # Helper types
    'Observation',
]

# Create Python module
module = pybind11::module_::module("nlm_neuromodulation");

// Neuromodulator base class
pybind11::class_<Neuromodulator> neuromodulator_class(
    module,
    "Neuromodulator",
    "Abstract base class for neuromodulatory signals"
);

neuromodulator_class
    .def("getName", &Neuromodulator::getName, "Get modulator name")
    .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
    .def("setLevel", &Neuromodulator::setLevel, pybind11::arg("level"),
         "Set current concentration/level")
    .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
         "Get neuromodulatory effect on plasticity")
    .def("update", &Neuromodulator::update, pybind11::arg("dt"),
         "Update neuromodulator state");

// Dopamine class
pybind11::class_<Dopamine, Neuromodulator> dopamine_class(
    module,
    "Dopamine",
    "Dopamine: Reward and reinforcement learning signal"
);

dopamine_class
    .def(pybind11::init<>())
    .def("getName", &Dopamine::getName)
    .def("getLevel", &Dopamine::getLevel)
    .def("setLevel", &Dopamine::setLevel, pybind11::arg("level"))
    .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
    .def("update", &Dopamine::update, pybind11::arg("dt"))
    .def("signalReward", &Dopamine::signalReward, pybind11::arg("reward"),
         "Signal reward event")
    .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
         pybind11::arg("error"), "Signal reward prediction error");

// Curiosity class
pybind11::class_<Curiosity, Neuromodulator> curiosity_class(
    module,
    "Curiosity",
    "Curiosity drive: exploration motivation based on novelty and prediction error"
);

curiosity_class
    .def(pybind11::init<>())
    .def("initialize", &Curiosity::initialize, pybind11::arg("brain"),
         "Initialize with brain reference")
    .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
    .def("getExplorationDrive", &Curiosity::getExplorationDrive,
         "Get exploration drive (same as level)")
    .def("update", &Curiosity::update, pybind11::arg("novelty"), 
         pybind11::arg("predictionError"), pybind11::arg("dt"),
         "Update curiosity based on novelty and prediction error")
    .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, pybind11::arg("weight"),
         "Set curiosity parameters")
    .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight,
         pybind11::arg("weight"), "Set curiosity parameters")
    .def("reset", &Curiosity::reset, "Reset");

// PredictionError class
pybind11::class_<PredictionError, Neuromodulator> prediction_error_class(
    module,
    "PredictionError",
    "Prediction error signal for curiosity and learning"
);

prediction_error_class
    .def(pybind11::init<>())
    .def("initialize", &PredictionError::initialize, pybind11::arg("brain"),
         "Initialize with brain reference")
    .def("getError", &PredictionError::getError, "Get error value")
    .def("computeError", &PredictionError::computeError, 
         pybind11::arg("predicted"), pybind11::arg("actual"),
         "Compute prediction error")
    .def("updatePrediction", &PredictionError::updatePrediction,
         pybind11::arg("newPrediction"), "Update prediction")
    .def("getHistory", &PredictionError::getHistory,
         "Get history of prediction errors")
    .def("clearHistory", &PredictionError::clearHistory, "Clear history")
    .def("getMagnitude", &PredictionError::getMagnitude,
         "Get error magnitude for neuromodulation");

// Novelty class
pybind11::class_<Novelty, Neuromodulator> novelty_class(
    module,
    "Novelty",
    "Novelty detection for learning and curiosity"
);

novelty_class
    .def(pybind11::init<>())
    .def("getName", &Novelty::getName)
    .def("getLevel", &Novelty::getLevel)
    .def("setLevel", &Novelty::setLevel, pybind11::arg("level"))
    .def("getPlasticityFactor", &Novelty::getPlasticityFactor)
    .def("update", &Novelty::update, pybind11::arg("dt"))
    .def("detectNovelty", &Novelty::detectNovelty, pybind11::arg("pattern"),
         pybind11::arg("previousPattern"), "Detect novelty in input pattern")
    .def("reset", &Novelty::reset, "Reset");

// Acetylcholine class
pybind11::class_<Acetylcholine, Neuromodulator> acetylcholine_class(
    module,
    "Acetylcholine",
    "Acetylcholine: Attention and memory consolidation"
);

acetylcholine_class
    .def(pybind11::init<>())
    .def("getName", &Acetylcholine::getName)
    .def("getLevel", &Acetylcholine::getLevel)
    .def("setLevel", &Acetylcholine::setLevel, pybind11::arg("level"))
    .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor)
    .def("update", &Acetylcholine::update, pybind11::arg("dt"))
    .def("enhanceAttention", &Acetylcholine::enhanceAttention, pybind11::arg("duration"),
         "Enhance attentional focus");

// Norepinephrine class
pybind11::class_<Norepinephrine, Neuromodulator> norepinephrine_class(
    module,
    "Norepinephrine",
    "Norepinephrine: Arousal and vigilance"
);

norepinephrine_class
    .def(pybind11::init<>())
    .def("getName", &Norepinephrine::getName)
    .def("getLevel", &Norepinephrine::getLevel)
    .def("setLevel", &Norepinephrine::setLevel, pybind11::arg("level"))
    .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor)
    .def("update", &Norepinephrine::update, pybind11::arg("dt"))
    .def("increase alertness", &Norepinephrine::increaseAlertness,
         "Increase arousal and vigilance");

// Serotonin class
pybind11::class_<Serotonin, Neuromodulator> serotonin_class(
    module,
    "Serotonin",
    "Serotonin: Mood, impulsivity, and social behavior"
);

serotonin_class
    .def(pybind11::init<>())
    .def("getName", &Serotonin::getName)
    .def("getLevel", &Serotonin::getLevel)
    .def("setLevel", &Serotonin::setLevel, pybind11::arg("level"))
    .def("getPlasticityFactor", &Serotonin::getPlasticityFactor)
    .def("update", &Serotonin::update, pybind11::arg("dt"))
    .def("regulateMood", &Serotonin::regulateMood, pybind11::arg("targetMood"),
         "Regulate mood and impulsivity");

// Helper function to create neuromodulators
module.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
    return std::make_shared<Dopamine>();
}, "Create a dopamine neuromodulator");

module.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
    return std::make_shared<Curiosity>();
}, "Create a curiosity neuromodulator");

module.def("createPredictionError", []() -> std::shared_ptr<PredictionError> {
    return std::make_shared<PredictionError>();
}, "Create a prediction error neuromodulator");

module.def("createNovelty", []() -> std::shared_ptr<Novelty> {
    return std::make_shared<Novelty>();
}, "Create a novelty detector");

module.def("createAcetylcholine", []() -> std::shared_ptr<Acetylcholine> {
    return std::make_shared<Acetylcholine>();
}, "Create an acetylcholine neuromodulator");

module.def("createNorepinephrine", []() -> std::shared_ptr<Norepinephrine> {
    return std::make_shared<Norepinephrine>();
}, "Create a norepinephrine neuromodulator");

module.def("createSerotonin", []() -> std::shared_ptr<Serotonin> {
    return std::make_shared<Serotonin>();
}, "Create a serotonin neuromodulator");

// Create submodule
PYBIND11_PLUGIN("nlm_neuromodulation");
