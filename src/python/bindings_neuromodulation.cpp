// Neuromodulation Systems Module for NLM Python Bindings
// This file contains Python bindings for neuromodulation-related systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/brain/Brain.hpp"

namespace py = pybind11;
namespace nlm {

void bind_neuromodulation_systems(py::module_ &m) {
    // === NEUROMODULATOR BASE CLASS ===
    py::class_<Neuromodulator>(m, "Neuromodulator", "Base class for neuromodulation systems")
        .def(py::init<>())
        .def("initialize", &Neuromodulator::initialize,
            py::arg("brain"), "Initialize neuromodulator")
        .def("update", &Neuromodulator::update,
            py::arg("timestep"), "Update neuromodulator state")
        .def("getLevel", &Neuromodulator::getLevel,
            "Get current neuromodulator level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
            "Get neuromodulation effect on plasticity")
        .def("isActive", &Neuromodulator::isActive,
            "Check if neuromodulator is active")
        .def("setActive", &Neuromodulator::setActive,
            py::arg("active"), "Set neuromodulator active state");
    
    // === DOPAMINE SYSTEM ===
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine",
        "Dopamine neuromodulation system for reward prediction")
        .def(py::init<>())
        .def("getRewardPredictionError", &Dopamine::getRewardPredictionError,
            py::arg("actualReward"), py::arg("predictedReward"),
            "Calculate reward prediction error")
        .def("setRewardPrediction", &Dopamine::setRewardPrediction,
            py::arg("prediction"), "Set expected reward")
        .def("updateLearningRate", &Dopamine::updateLearningRate,
            py::arg("learningRate"), "Update learning rate based on dopamine");
    
    // === CURIOSITY SYSTEM ===
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity",
        "Curiosity neuromodulation system for exploration")
        .def(py::init<>())
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
            "Get current exploration drive")
        .def("getNoveltySensitivity", &Curiosity::getNoveltySensitivity,
            "Get sensitivity to novel stimuli")
        .def("setNoveltyThreshold", &Curiosity::setNoveltyThreshold,
            py::arg("threshold"), "Set threshold for novelty detection")
        .def("getExplorationBonus", &Curiosity::getExplorationBonus,
            "Get exploration bonus for novel actions");
    
    // === NOVELTY SYSTEM ===
    py::class_<Novelty, Neuromodulator>(m, "Novelty",
        "Novelty detection system for change detection")
        .def(py::init<>())
        .def("updateNovelty", &Novelty::updateNovelty,
            py::arg("sensoryInput"), "Update novelty based on sensory input")
        .def("getNoveltyScore", &Novelty::getNoveltyScore,
            "Get current novelty score")
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold,
            py::arg("threshold"), "Set novelty threshold")
        .def("resetNovelty", &Novelty::resetNovelty,
            "Reset novelty detector");
    
    // === PREDICTION ERROR SYSTEM ===
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError",
        "Prediction error system for mismatch detection")
        .def(py::init<>())
        .def("updatePredictionError", &PredictionError::updatePredictionError,
            py::arg("expected"), py::arg("actual"),
            "Update prediction error")
        .def("getErrorMagnitude", &PredictionError::getErrorMagnitude,
            "Get magnitude of prediction error")
        .def("setLearningRate", &PredictionError::setLearningRate,
            py::arg("learningRate"), "Set learning rate for error correction")
        .def("getErrorPattern", &PredictionError::getErrorPattern,
            "Get current error pattern");
    
    // === NEUROMODULATION SIGNAL STRUCT ===
    py::class_<NeuromodulationSignal>(m, "NeuromodulationSignal",
        "Represents a neuromodulation signal")
        .def(py::init<>())
        .def_readwrite("type", &NeuromodulationSignal::type)
        .def_readwrite("level", &NeuromodulationSignal::level)
        .def_readwrite("sourceNeuron", &NeuromodulationSignal::sourceNeuron)
        .def_readwrite("targetSystem", &NeuromodulationSignal::targetSystem)
        .def_readwrite("timestamp", &NeuromodulationSignal::timestamp);
}

// === FACTORY FUNCTIONS ===

void bind_neuromodulation_factories(py::module_ &m) {
    m.def("createDopamine", []() {
        return std::make_unique<Dopamine>();
    }, "Create a dopamine neuromodulator instance");
    
    m.def("createCuriosity", []() {
        return std::make_unique<Curiosity>();
    }, "Create a curiosity neuromodulator instance");
    
    m.def("createNovelty", []() {
        return std::make_unique<Novelty>();
    }, "Create a novelty neuromodulator instance");
    
    m.def("createPredictionError", []() {
        return std::make_unique<PredictionError>();
    }, "Create a prediction error neuromodulator instance");
    
    m.def("createDefaultNeuromodulationSystem", []() {
        auto dopamine = std::make_unique<Dopamine>();
        auto curiosity = std::make_unique<Curiosity>();
        auto novelty = std::make_unique<Novelty>();
        auto predictionError = std::make_unique<PredictionError>();
        
        return std::tuple<std::unique_ptr<Dopamine>,
                         std::unique_ptr<Curiosity>,
                         std::unique_ptr<Novelty>,
                         std::unique_ptr<PredictionError>>(
            std::move(dopamine),
            std::move(curiosity),
            std::move(novelty),
            std::move(predictionError)
        );
    }, "Create a complete default neuromodulation system");
}

} // namespace nlm