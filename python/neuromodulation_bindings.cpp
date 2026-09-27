#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace py = pybind11;

void bind_reward_neuromodulator(py::module& m) {
    py::class_<nlm::Reward>(m, "Reward")
        .def(py::init<>())
        .def("processReward", &nlm::Reward::processReward,
             py::arg("currentState"), py::arg("nextState"),
             py::arg("action"), py::arg("reward"),
             "Process reward signal")
        .def("getCurrentReward", &nlm::Reward::getCurrentReward,
             "Get current reward value")
        .def("getPredictionError", &nlm::Reward::getPredictionError,
             "Get reward prediction error")
        .def("reset", &nlm::Reward::reset,
             "Reset reward system");
}

void bind_curiosity_neuromodulator(py::module& m) {
    py::class_<nlm::Curiosity>(m, "Curiosity")
        .def(py::init<>())
        .def("processSensoryInput", &nlm::Curiosity::processSensoryInput,
             py::arg("input"), "Process sensory input for curiosity")
        .def("getCuriosityLevel", &nlm::Curiosity::getCuriosityLevel,
             "Get curiosity level")
        .def("update", &nlm::Curiosity::update,
             py::arg("dt"), "Update curiosity dynamics")
        .def("reset", &nlm::Curiosity::reset,
             "Reset curiosity system");
}

void bind_prediction_error_neuromodulator(py::module& m) {
    py::class_<nlm::PredictionError>(m, "PredictionError")
        .def(py::init<>())
        .def("processPrediction", &nlm::PredictionError::processPrediction,
             py::arg("predicted"), py::arg("actual"),
             "Process prediction error")
        .def("getPredictionError", &nlm::PredictionError::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &nlm::PredictionError::getConfidence,
             "Get prediction confidence")
        .def("reset", &nlm::PredictionError::reset,
             "Reset prediction error system");
}

void bind_novelty_neuromodulator(py::module& m) {
    py::class_<nlm::Novelty>(m, "Novelty")
        .def(py::init<>())
        .def("processInput", &nlm::Novelty::processInput,
             py::arg("input"), "Process input for novelty detection")
        .def("getNoveltyLevel", &nlm::Novelty::getNoveltyLevel,
             "Get novelty level")
        .def("getConfidence", &nlm::Novelty::getConfidence,
             "Get novelty confidence")
        .def("reset", &nlm::Novelty::reset,
             "Reset novelty system");
}

void init_neuromodulation_bindings(py::module& m) {
    bind_reward_neuromodulator(m);
    bind_curiosity_neuromodulator(m);
    bind_prediction_error_neuromodulator(m);
    bind_novelty_neuromodulator(m);
}
