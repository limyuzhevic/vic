// Environment and visualization system bindings

#include "bindings.h"

namespace nlm {

void bind_environment_module(py::module_& m) {
    // Environment class
    py::class_<Environment>(m, "Environment", R"pbdoc(
        Environment interface for NLM simulation.
        
        Environment provides:
        - World state management
        - Reward computation
        - Feedback mechanisms
        - Environment reset capabilities
        
        Key features:
        - Dynamic environment changes
        - Reward/penalty signals
        - Sensory feedback
        - Environment modeling
    )pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset, "Reset environment to initial state")
        .def("step", &Environment::step, py::arg("action"), "Step environment forward with action")
        .def("getState", &Environment::getState, py::return_value_policy::reference_internal,
             "Get current environment state")
        .def("setState", &Environment::setState, py::arg("state"), "Set environment state")
        .def("getReward", &Environment::getReward, "Get current reward")
        .def("getRewardForState", &Environment::getRewardForState, py::arg("state"),
             "Get reward for given state")
        .def("isTerminal", &Environment::isTerminal, "Check if environment is in terminal state")
        .def("getObservation", &Environment::getObservation, py::return_value_policy::reference_internal,
             "Get current observation")
        .def("applyAction", &Environment::applyAction, py::arg("action"),
             "Apply action and return result")
        .def("getEnergy", &Environment::getEnergy, "Get current energy level")
        .def("setEnergy", &Environment::setEnergy, py::arg("energy"), "Set energy level")
        .def("getHealth", &Environment::getHealth, "Get current health")
        .def("setHealth", &Environment::setHealth, py::arg("health"), "Set health level");

    // Observation class
    py::class_<Observation>(m, "Observation", R"pbdoc(
        Observation of environment state.
        
        Observation provides:
        - Environmental state snapshot
        - Reward information
        - Terminal state indication
        - Action results
        
        Key features:
        - Complete environment snapshot
        - Reward prediction
        - Success/failure indicators
        - Action outcome tracking
    )pbdoc")
        .def(py::init<>())
        .def("getState", &Observation::getState, py::return_value_policy::reference_internal,
             "Get state from observation")
        .def("setState", &Observation::setState, py::arg("state"), "Set state in observation")
        .def("getReward", &Observation::getReward, "Get reward from observation")
        .def("setReward", &Observation::setReward, py::arg("reward"), "Set reward in observation")
        .def("isTerminal", &Observation::isTerminal, "Check if observation is terminal")
        .def("setTerminal", &Observation::setTerminal, py::arg("terminal"), "Set terminal state")
        .def("getActionResult", &Observation::getActionResult, py::return_value_policy::reference_internal,
             "Get action result from observation")
        .def("setActionResult", &Observation::setActionResult, py::arg("result"), "Set action result")
        .def("getTimestamp", &Observation::getTimestamp, "Get observation timestamp")
        .def("setTimestamp", &Observation::setTimestamp, py::arg("timestamp"), "Set observation timestamp");
}

} // namespace nlm