#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/sensory/SensoryInput.hpp"

namespace py = pybind11;

void bind_prediction_system(py::module& m) {
    py::class_<nlm::PredictionSystem>(m, "PredictionSystem")
        .def(py::init<>())
        .def("predictNextState", &nlm::PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Generate prediction for next state")
        .def("updatePredictions", &nlm::PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update prediction model based on actual observation")
        .def("getPredictionError", &nlm::PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("getConfidence", &nlm::PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &nlm::PredictionSystem::getErrorHistory,
             "Get history of prediction errors")
        .def("clearHistory", &nlm::PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &nlm::PredictionSystem::train,
             py::arg("observation"), "Train prediction model")
        .def("reset", &nlm::PredictionSystem::reset,
             "Reset prediction system");
}

void bind_neural_prediction(py::module& m) {
    py::class_<nlm::NeuralPrediction>(m, "NeuralPrediction")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralPrediction::initialize,
             py::arg("brain"), "Initialize with brain reference")
        .def("predict", &nlm::NeuralPrediction::predict,
             py::arg("state"), "Make prediction for given state")
        .def("update", &nlm::NeuralPrediction::update,
             py::arg("predictionError"), "Update prediction based on error")
        .def("getConfidence", &nlm::NeuralPrediction::getConfidence,
             "Get prediction confidence")
        .def("getPredictionError", &nlm::NeuralPrediction::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::NeuralPrediction::reset,
             "Reset prediction system");
}

void init_prediction_bindings(py::module& m) {
    bind_prediction_system(m);
    bind_neural_prediction(m);
}
