#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

// world_bindings.cpp - SimpleWorld class
// Purpose: Defines Python bindings for the SimpleWorld simulation class

// Register SimpleWorld class bindings to the given module
void register_world(pybind11::module_& m) {

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld represents a complete simulation environment where the NLM brain
        interacts with the world through sensory input and produces motor actions.
        It provides a controlled environment for studying agent behavior, learning,
        and development in a simulated setting.
        
        The world manages multiple aspects:
        - Physical space with boundaries and obstacles
        - Objects (resources, hazards, markers)
        - Agent position and state
        - Time evolution
        - Sensory processing
        - Action execution
        
        Examples:
            >>> import nlm
            >>> world = nlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
            >>> # World ready for agent interaction
            >>> world.getWidth()
            20
            >>> world.getHeight()
            20
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure the world's dimensions and agent vision parameters.
             
             Args:
                 width: World width in grid units (default: 20)
                 height: World height in grid units (default: 20)
                 visionWidth: Maximum vision range in X direction (default: 8)
                 visionHeight: Maximum vision range in Y direction (default: 8)
             
             Examples:
                 >>> world = nlm.createSimpleWorld()
                 >>> world.configure(width=30, height=25, visionWidth=10, visionHeight=10)
                 >>> # Configure world for exploration
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset the world to its initial state.
             
             This clears all objects, resets the agent to its starting position,
             and resets simulation time to zero. Useful for starting a new
             episode or testing.
             
             Examples:
                 >>> world = nlm.createSimpleWorld()
                 >>> world.configure(width=20, height=20)
                 >>> world.reset()  # World reset to initial state
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set the agent's starting position in the world.
             
             Args:
                 x: X-coordinate for agent start (0 to width-1)
                 y: Y-coordinate for agent start (0 to height-1)
             
             Examples:
                 >>> world = nlm.createSimpleWorld()
                 >>> world.configure(width=20, height=20)
                 >>> world.setAgentStart(5, 5)  # Agent starts at (5,5)
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Advance the simulation by one timestep.
             
             Args:
                 timestep: Time step duration in seconds (default: 0.1)
             
             This updates the world state by the given timestep, allowing the
            agent to process sensory input, make decisions, and execute actions
            in the simulated environment.
             
             Examples:
                 >>> world = nlm.createSimpleWorld()
                 >>> world.configure(width=20, height=20)
                 >>> for i in range(100):
                 ...     world.update(0.1)  # Update for 100 timesteps
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply a motor command from the agent to the world.
             
             Args:
                 cmd: MotorCommand representing the agent's intended action
                 currentTime: Current simulation time in seconds
             
             This executes the agent's action in the world, updating the
            agent's position, handling collisions with objects, and applying
            any effects (rewards, hazards, etc.).
            
            Raises:
                ValueError: If cmd is invalid or command cannot be applied
            
            Examples:
                >>> action = nlm.Action(nlm.ActionType.MoveForward)
                >>> world.applyMotorCommand(action, 0.5)
            )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the current sensory percept from the agent in the world.
             
             Returns:
                 SensoryPercept: The agent's current sensory input containing
                                vision, audio, and internal signals.
            
            This provides the agent's current view of the environment,
            including visual information about nearby objects, audio
            information, and internal brain state.
            
            Examples:
                >>> world = nlm.createSimpleWorld()
                >>> world.configure(width=20, height=20)
                >>> percept = world.getSensoryPercept()
                >>> vision = percept.getVision()
                >>> vision.getWidth()
                8
            )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the agent's body state.
             
             Returns:
                 AgentBody: Current state of the agent including position,
                           velocity, orientation, energy, and health.
            
            Examples:
                >>> world = nlm.createSimpleWorld()
                >>> body = world.getAgentBody()
                >>> body.x
                0.0
                >>> body.y
                0.0
            )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add a world object to the simulation.
             
             Args:
                 obj: WorldObject to add to the world
            
            Raises:
                ValueError: If obj is None or position is invalid
                RuntimeError: If world is not properly configured
            
            Examples:
                >>> world = nlm.createSimpleWorld()
                >>> resource = nlm.WorldObject(5.0, 5.0, nlm.WorldObjectType.Resource, 10.0)
                >>> world.addObject(resource)
            )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove a world object at the specified position.
             
             Args:
                 x: X-coordinate of object to remove
                 y: Y-coordinate of object to remove
            
            Examples:
                >>> world = nlm.createSimpleWorld()
                >>> world.removeObject(5.0, 5.0)
            )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if a position is valid within the world boundaries.
             
             Args:
                 x: X-coordinate to check
                 y: Y-coordinate to check
            
            Returns:
                bool: True if position is within world bounds and valid
            
            Examples:
                >>> world = nlm.createSimpleWorld()
                >>> world.configure(width=20, height=20)
                >>> world.isValidPosition(5, 5)
                True
                >>> world.isValidPosition(25, 5)  # Outside bounds
                False
            )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get the world width in grid units)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get the world height in grid units)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get the maximum energy the agent can have)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set the maximum agent energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get the energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set the energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get the current simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set the random seed for reproducible simulations)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get the current random seed)pbdoc");

} // namespace nlm