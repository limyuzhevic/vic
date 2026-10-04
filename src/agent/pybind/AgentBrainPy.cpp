#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "src/agent/AgentBrain.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/core/Logger/Logger.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm_agent, m) {
    m.doc() = "NLM Agent module - Python bindings for AgentBrain and related components";

    // MotorCommand enum
    py::enum_<nlm::MotorCommand>(m, "MotorCommand")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    // ActionResult struct
    py::class_<nlm::ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message);

    // AgentBody struct
    py::class_<nlm::AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX)
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY)
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("isMoving", &nlm::AgentBody::isMoving)
        .def_readwrite("isTurning", &nlm::AgentBody::isTurning)
        .def_readwrite("lastActionTime", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset);

    // SensoryPercept class
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision)
        .def("getVisionWidth", &nlm::SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &nlm::SensoryPercept::getVisionHeight)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch)
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal)
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception)
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio)
        .def("getAllSignals", &nlm::SensoryPercept::getAllSignals)
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp)
        .def("setTimestamp", &nlm::SensoryPercept::setTimestamp);

    // AgentBrain class
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<class nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"))
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"))
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("reset", &nlm::AgentBrain::reset)
        .def("getBrain", &nlm::AgentBrain::getBrain)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled)
        .def("enable_reward_modulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enable_structural_plasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enable_development", &nlm::AgentBrain::enableDevelopment)
        .def("enable_curiosity", &nlm::AgentBrain::enableCuriosity)
        .def("is_reward_modulation_enabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &nlm::AgentBrain::isCuriosityEnabled)
        .def("get_dopamine_level", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("get_curiosity_level", &nlm::AgentBrain::getCuriosityLevel)
        .def("get_novelty_level", &nlm::AgentBrain::getNoveltyLevel)
        .def("get_prediction_error", &nlm::AgentBrain::getPredictionError)
        .def("get_developmental_stage", &nlm::AgentBrain::getDevelopmentalStage);
}
