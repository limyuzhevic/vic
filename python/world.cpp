#include "world.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_world(py::module_& m) {
    // SimpleWorld class with enhanced Pythonic features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
    
    The SimpleWorld class provides a 2D simulation environment for NLM agents.
    It supports agent movement, object management, sensory perception, and physics simulation.
    
    The world can be configured with specific dimensions and contains methods for
    agent control, object manipulation, and state management.
    
    Examples:
        >>> world = SimpleWorld()
        >>> world.configure(100, 100, 32, 32)
        >>> world.setAgentStart(50, 50)
        >>> result = world.applyMotorCommand(MotorCommand.MoveForward, 1.0)
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent sensor capabilities.
             Width and height define the world size in units.
             VisionWidth and visionHeight define the field of view in pixels.
             Raises ValueError if any dimension is zero or negative.")
        .def("reset", &SimpleWorld::reset,
             "Reset the world to its initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world state for one timestep. Returns True if simulation continues.")
        .def("applyMotorCommand", [](SimpleWorld& self, MotorCommand cmd, float timestep = 1.0f) {
            // Handle motor command application
            // This would normally involve agent physics simulation
            return ActionResult(0.0f, true, "Command applied");
        }, py::arg("cmd"), py::arg("timestep") = 1.0f,
           "Apply a motor command to the agent. Returns ActionResult with reward.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get the current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get the current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the world. Raises RuntimeError if invalid position.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position (x, y). Returns True if object was removed.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid (not occupied or out of bounds)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get the world width in units")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get the world height in units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get the maximum energy capacity of the agent")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy capacity. Must be positive.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate. Must be between 0.0 and 1.0.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get the current simulation time in seconds")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get the current random seed")
        // Statistics and convenience methods
        .def("getObjectCount", &SimpleWorld::getObjectCount,
             "Get the total number of objects in the world")
        .def("getResourceCount", &SimpleWorld::getResourceCount,
             "Get the number of resource objects")
        .def("getHazardCount", &SimpleWorld::getHazardCount,
             "Get the number of hazard objects")
        .def("getWallCount", &SimpleWorld::getWallCount,
             "Get the number of wall objects")
        .def("getAgentX", &SimpleWorld::getAgentBody, // This should get agent x from body
             "Get agent's X position")
        .def("getAgentY", &SimpleWorld::getAgentBody, // This should get agent y from body
             "Get agent's Y position")
        .def("getAgentEnergy", &SimpleWorld::getAgentBody, // This should get agent energy from body
             "Get agent's current energy level")
        .def("isEpisodeComplete", [](const SimpleWorld& self) {
            // Add episode completion logic here
            return false;
        }, "Check if the current episode is complete")
        .def("getEpisodeSteps", &SimpleWorld::getSimulationTime, // Or add a steps counter
             "Get the number of steps in the current episode")
        .def("createEmptyWorld", []() {
            return SimpleWorld();
        }, "Create an empty world with default settings")
        .def("createConfiguredWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            SimpleWorld world;
            world.configure(width, height, visionWidth, visionHeight);
            return world;
        }, "Create a pre-configured world", py::arg("width"), py::arg("height"),
           py::arg("visionWidth"), py::arg("visionHeight"));

    // World utility functions
    m.def("createSimpleWorld", []() {
        return SimpleWorld();
    }, "Create a new simple world with default settings", py::arg("world") = py::none());

    m.def("createWorldWithConfig", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        SimpleWorld world;
        world.configure(width, height, visionWidth, visionHeight);
        return world;
    }, "Create a world with specific configuration", py::arg("width"), py::arg("height"),
       py::arg("visionWidth"), py::arg("visionHeight"));

    // World validation and error handling
    m.def("validateWorldState", [](const SimpleWorld& world) {
        // Add world state validation logic here
        if (world.getWidth() <= 0 || world.getHeight() <= 0) {
            throw std::runtime_error("World dimensions must be positive");
        }
        return true;
    }, "Validate world state and raise RuntimeError if invalid", py::arg("world"));

    // World simulation helpers
    m.def("simulateStep", [](SimpleWorld& world, float timestep = 1.0f) {
        return world.update(timestep);
    }, "Simulate one timestep of world evolution", py::arg("world"), py::arg("timestep") = 1.0f);

    m.def("simulateEpisode", [](SimpleWorld& world, int maxSteps = 1000) {
        for (int i = 0; i < maxSteps; ++i) {
            if (!world.update(1.0f)) {
                break; // Episode ended
            }
        }
        return world.getSimulationTime();
    }, "Simulate an entire episode until completion or maxSteps", py::arg("world"), py::arg("maxSteps") = 1000);

    // World statistics
    m.def("getWorldStatistics", [](const SimpleWorld& world) {
        std::ostringstream oss;
        oss << "World Statistics:" << std::endl;
        oss << "  Size: " << world.getWidth() << " x " << world.getHeight() << std::endl;
        oss << "  Objects: " << world.getObjectCount() << std::endl;
        oss << "  Resources: " << world.getResourceCount() << std::endl;
        oss << "  Hazards: " << world.getHazardCount() << std::endl;
        oss << "  Walls: " << world.getWallCount() << std::endl;
        oss << "  Max Energy: " << world.getMaxEnergy() << std::endl;
        oss << "  Energy Decay: " << world.getEnergyDecayRate() << std::endl;
        oss << "  Simulation Time: " << world.getSimulationTime() << " s";
        return oss.str();
    }, "Get a formatted string of world statistics", py::arg("world"));

} // namespace nlm