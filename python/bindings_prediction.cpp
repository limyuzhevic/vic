// Prediction system bindings

#include "bindings.h"

namespace nlm {

void bind_prediction_module(py::module_& m) {
    // PredictionSystem class
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(
        Prediction system for sensory prediction and error computation.
        
        PredictionSystem provides:
        - Sensory prediction using neural dynamics
        - Prediction error computation
        - Confidence tracking
        - Learning from prediction errors
        
        Key features:
        - Forward models for sensory prediction
        - Prediction error signals
        - Confidence-based weighting
        - Adaptive prediction tuning
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("predictSensoryInput", &PredictionSystem::predictSensoryInput,
             py::arg("currentState"), "Predict next sensory input")
        .def("computePredictionError", &PredictionSystem::computePredictionError,
             py::arg("actualInput"), "Compute prediction error")
        .def("updatePrediction", &PredictionSystem::updatePrediction,
             py::arg("actualInput"), "Update prediction based on actual input")
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence,
             "Get prediction confidence (0.0 to 1.0)")
        .def("setPredictionConfidence", &PredictionSystem::setPredictionConfidence,
             py::arg("confidence"), "Set prediction confidence")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("getPredictedState", &PredictionSystem::getPredictedState,
             py::return_value_policy::reference_internal, "Get last predicted state")
        .def("getActualState", &PredictionSystem::getActualState,
             py::return_value_policy::reference_internal, "Get last actual state")
        .def("reset", &PredictionSystem::reset, "Reset prediction system");

    // NeuralPrediction class
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(
        Neural prediction using spiking dynamics.
        
        NeuralPrediction provides:
        - Spiking-based prediction
        - Temporal sequence prediction
        - Pattern completion
        - Activity-dependent prediction
        
        Key features:
        - Use neural activity for prediction
        - Temporal sequence modeling
        - Pattern completion based on partial input
        - Dynamic prediction updating
    )pbdoc")
        .def(py::init<>())
        .def("train", &NeuralPrediction::train, py::arg("pattern"), py::arg("reward"),
             "Train prediction with pattern and reward")
        .def("predict", &NeuralPrediction::predict, py::arg("pattern"), "Predict next pattern")
        .def("getConfidence", &NeuralPrediction::getConfidence, py::arg("pattern"),
             "Get prediction confidence for pattern")
        .def("getPrediction", &NeuralPrediction::getPrediction, py::arg("pattern"),
             py::return_value_policy::reference_internal, "Get prediction for pattern")
        .def("clear", &NeuralPrediction::clear, "Clear all predictions");
}

} // namespace nlm