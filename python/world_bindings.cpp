#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../python/nlm/core/binding_helpers.hpp"

namespace py = pybind11;
namespace nlm {

void bindWorldTypes(py::module_& m) {
    m.doc() = R"pbdoc(
        World Types
        ------------
        Types and classes related to world simulation.
    )pbdoc";
}

void bindWorldClasses(py::module_& m) {
    // Enhanced SimpleWorld class with advanced features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a simple 2D environment where the agent can move, interact with objects,
        and receive sensory input. The world manages physics, collisions, and agent state.
        
        The world simulates a grid-based environment with realistic physics
        including movement, collisions, energy decay, and resource management.
        
        Examples:
            >>> import pynlm as nlm
            >>> world = nlm.SimpleWorld(100, 100, 10, 10)
            >>> world.setAgentStart(50, 50)
            >>> 
            >>> # Add resources and hazards
            >>> resource = nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 50.0)
            >>> hazard = nlm.WorldObject(90, 90, nlm.WorldObjectType.Hazard, 30.0)
            >>> world.addObject(resource)
            >>> world.addObject(hazard)
            >>> 
            >>> # Run simulation
            >>> for step in range(100):
            ...     world.update(0.01)
            ...     percept = world.getSensoryPercept()
            ...     body = world.getAgentBody()
            ...     print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), Energy: {body.energy:.1f}")
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world state by the specified timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the agent and update world state")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the agent's perspective")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the environment")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove the world object at the specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if the specified position is valid (within bounds and not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get the world width in pixels")
        .def("getHeight", &SimpleWorld::getHeight, "Get the world height in pixels")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get the maximum energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set the maximum energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get the energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set the energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get the current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set the random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get the current random seed")
        .def("getWorldStats", [](const SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["vision_width"] = self.getVisionWidth();
            stats["vision_height"] = self.getVisionHeight();
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["simulation_time"] = self.getSimulationTime();
            return stats;
        }, "Get world statistics as a dictionary")
        .def("isAgentColliding", [](SimpleWorld& self, double x, double y) {
            return !self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Check if agent would collide at position (x, y)");

    // Enhanced WorldObject class with more features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the simulation environment.
        
        Represents objects in the world that the agent can interact with,
        such as resources, hazards, walls, or markers.
        
        World objects can have different types and values that determine
        their behavior and interaction with the agent.
        
        Examples:
            >>> import pynlm as nlm
            >>> # Create a resource
            >>> resource = nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 50.0)
            >>> print(f"Resource at ({resource.x}, {resource.y}) with value {resource.value}")
            >>> 
            >>> # Create a hazard
            >>> hazard = nlm.WorldObject(90, 90, nlm.WorldObjectType.Hazard, 30.0, 0.8)
            >>> print(f"Hazard radius: {hazard.radius}")
            >>> 
            >>> # Create a wall
            >>> wall = nlm.WorldObject(50, 50, nlm.WorldObjectType.Wall, 0.0, 2.0)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create a world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate of the object")
        .def_readwrite("y", &WorldObject::y, "Y coordinate of the object")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object for collision detection")
        .def_readwrite("type", &WorldObject::type, "Type of the world object")
        .def_readwrite("value", &WorldObject::value, "Value associated with the object (e.g., health, energy)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active in the simulation")
        .def("getTypeName", [](WorldObject& self) {
            // Get string representation of WorldObjectType
            switch (self.type) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        }, "Get the string name of the object type")
        .def("isResource", [](WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("isHazard", [](WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("isWall", [](WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        .def("distanceTo", [](WorldObject& self, double x, double y) {
            double dx = self.x - x;
            double dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("containsPoint", [](WorldObject& self, double x, double y) {
            double dx = self.x - x;
            double dy = self.y - y;
            return (dx * dx + dy * dy) <= (self.radius * self.radius);
        }, py::arg("x"), py::arg("y"), "Check if point (x, y) is within object's radius");

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
        
        Different object types have different behaviors and interactions with the agent.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space (walkable)")
        .value("Resource", WorldObjectType::Resource, "Resource that can be collected")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that damages the agent")
        .value("Wall", WorldObjectType::Wall, "Wall that blocks movement")
        .value("Marker", WorldObjectType::Marker, "Marker for navigation or goals")
        .export_values()
        .doc() = "Enumeration of world object types. Each type has different behaviors and interactions with the agent.";
}

void bindWorldFactories(py::module_& m) {
    // World factory functions
    m.def("createSimpleWorld", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        return factory::createSimpleWorld(width, height, visionWidth, visionHeight);
    }, py::arg("width").default_value(100), py::arg("height").default_value(100),
       py::arg("visionWidth").default_value(10), py::arg("visionHeight").default_value(10),
       "Create a new simple world with specified dimensions and vision parameters");
    
    m.def("createWorldObject", [](double x, double y, WorldObjectType type, double value, double radius) -> std::shared_ptr<WorldObject> {
        auto obj = std::make_shared<WorldObject>();
        obj->x = x;
        obj->y = y;
        obj->type = type;
        obj->value = value;
        obj->radius = radius;
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0, py::arg("radius") = 0.5,
        "Create a world object with the specified properties");
    
    m.def("createResource", [](double x, double y, double value, double radius) -> std::shared_ptr<WorldObject> {
        return factory::createWorldObject(x, y, WorldObjectType::Resource, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("value"), py::arg("radius") = 0.5,
        "Create a resource object");
    
    m.def("createHazard", [](double x, double y, double value, double radius) -> std::shared_ptr<WorldObject> {
        return factory::createWorldObject(x, y, WorldObjectType::Hazard, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("value"), py::arg("radius") = 0.5,
        "Create a hazard object");
    
    m.def("createWall", [](double x, double y, double radius) -> std::shared_ptr<WorldObject> {
        return factory::createWorldObject(x, y, WorldObjectType::Wall, 0.0, radius);
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 1.0,
        "Create a wall object");
}

void bindWorldAdvanced(py::module_& m) {
    // Advanced world APIs
    m.def("getWorldStats", [](const std::shared_ptr<SimpleWorld>& world) {
        if (!world) {
            throw WorldError("World cannot be null");
        }
        return world->getWorldStats();
    }, py::arg("world"), "Get world statistics as a dictionary");
    
    m.def("batchUpdateWorld", [](std::shared_ptr<SimpleWorld> world,
                               const std::vector<double>& timesteps) {
        if (!world) {
            throw WorldError("World cannot be null");
        }
        for (double timestep : timesteps) {
            world->update(timestep);
        }
    }, py::arg("world"), py::arg("timesteps"),
        "Update world state with multiple timesteps (batch operation)");
    
    m.def("checkAgentSafety", [](const std::shared_ptr<SimpleWorld>& world,
                                const std::shared_ptr<AgentBody>& body) {
        if (!world || !body) {
            throw WorldError("World or body cannot be null");
        }
        py::dict safety;
        safety["is_valid_position"] = world->isValidPosition(body->x, body->y);
        safety["is_healthy"] = body->health > 50.0;
        safety["is_safe"] = body->energy > 10.0 && body->health > 20.0;
        return safety;
    }, py::arg("world"), py::arg("body"), "Check agent safety status");
}

} // namespace nlm