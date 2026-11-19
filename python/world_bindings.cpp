// World bindings implementation
#include "world_bindings.h"

namespace nlm {

void bindSimpleWorld(py::module_& m) {
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Add high-level convenience methods
        .def("updateWorld", [](SimpleWorld& self, float timestep, const py::dict& actions) {
            // Parse actions from dict and apply them
            // Implementation would convert dict to actual actions
            self.update(static_cast<double>(timestep));
        }, py::arg("timestep"), py::arg("actions"),
             "Update world with actions from dictionary (convenience method)")
        .def("getWorldState", [](const SimpleWorld& self, const std::string& format) {
            // Basic implementation - could be enhanced with JSON output
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["vision_width"] = static_cast<float>(self.getVisionWidth());
            state["vision_height"] = static_cast<float>(self.getVisionHeight());
            state["max_energy"] = self.getMaxEnergy();
            state["energy_decay_rate"] = self.getEnergyDecayRate();
            state["simulation_time"] = self.getSimulationTime();
            return state;
        }, py::arg("format") = "dict",
             "Get world state in specified format (dict or json)")
        .def("isPositionSafe", [](SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y) && self.getObjectAt(x, y) == nullptr;
        }, py::arg("x"), py::arg("y"), "Check if position is safe for agent");
}

void bindWorldObject(py::module_& m) {
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        // Add convenience methods
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNear", [](const WorldObject& self, float x, float y, float distance) {
            return self.distanceTo(x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if near position")
        .def("getName", &WorldObject::getName, "Get object name")
        .def("setName", &WorldObject::setName, py::arg("name"), "Set object name");
}

} // namespace nlm