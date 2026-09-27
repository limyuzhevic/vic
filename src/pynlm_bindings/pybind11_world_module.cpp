// pybind11_world_module.cpp - World module bindings implementation

#include "pybind11_world_module.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace nlm {

void bind_world_module(py::module& m) {
    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world simulation environment)
    
    A simple 2D world simulation environment where NLM agents interact
    with objects and environments. The world provides:
    
    - Spatial representation (x, y coordinates)
    - Vision sensing (what agents can see)
    - Object management (resources, hazards, walls, markers)
    - Agent body state tracking
    - Action execution and feedback
    - Time-based updates
    - Random number generation
    
    World features:
    - Grid-based spatial representation
    - Vision cone for agent sensing
    - Physics simulation (simplified)
    - Energy and resource management
    - Obstacle avoidance
    - Goal-oriented behavior
    
    The world supports:
    - Agent movement and positioning
    - Object creation and removal
    - State serialization and persistence
    - Random event generation
    - Reward system for learning
    "pbdoc")
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure,
         py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
         "Configure world dimensions and agent vision")
    .def("reset", &SimpleWorld::reset, "Reset world to initial state")
    .def("set_agent_start", &SimpleWorld::setAgentStart,
         py::arg("x"), py::arg("y"), "Set agent's starting position")
    .def("update", &SimpleWorld::update,
         py::arg("timestep"), "Update world state by timestep")
    .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("current_time"), "Apply agent's motor command to world")
    .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal, "Get agent's sensory percept")
    .def("get_agent_body", &SimpleWorld::getAgentBody,
         py::return_value_policy::reference_internal, "Get agent's body state")
    .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
         "Add a world object at specified position")
    .def("remove_object", &SimpleWorld::removeObject,
         py::arg("x"), py::arg("y"), "Remove object at specified position")
    .def("is_valid_position", &SimpleWorld::isValidPosition,
         py::arg("x"), py::arg("y"), "Check if position is valid (not in obstacle)")
    .def("get_width", &SimpleWorld::getWidth, "Get world width")
    .def("get_height", &SimpleWorld::getHeight, "Get world height")
    .def("get_vision_width", [](const SimpleWorld& self) {
        // Vision dimensions are stored in the percept, not directly accessible
        // This is a limitation of the SimpleWorld API
        auto percept = self.getSensoryPercept();
        return percept.getVisionWidth();
    }, "Get agent vision width")
    .def("get_vision_height", [](const SimpleWorld& self) {
        auto percept = self.getSensoryPercept();
        return percept.getVisionHeight();
    }, "Get agent vision height")
    .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
    .def("set_max_energy", &SimpleWorld::setMaxEnergy,
         py::arg("e"), "Set maximum agent energy")
    .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
         "Get energy decay rate per timestep")
    .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
         py::arg("r"), "Set energy decay rate per timestep")
    .def("get_simulation_time", &SimpleWorld::getSimulationTime,
         "Get current simulation time")
    .def("set_random_seed", &SimpleWorld::setRandomSeed,
         py::arg("seed"), "Set random seed for reproducible behavior")
    .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get current random seed")
    .def("get_simulation_stats", [](const SimpleWorld& self) {
        py::dict stats;
        auto body = self.getAgentBody();
        auto percept = self.getSensoryPercept();
        
        stats["simulation_time"] = self.getSimulationTime();
        stats["world_width"] = self.getWidth();
        stats["world_height"] = self.getHeight();
        stats["agent_x"] = body.x;
        stats["agent_y"] = body.y;
        stats["agent_orientation"] = body.orientation;
        stats["agent_energy"] = body.energy;
        stats["agent_health"] = body.health;
        stats["agent_age"] = body.age;
        stats["is_moving"] = body.isMoving;
        stats["is_turning"] = body.isTurning;
        stats["vision_width"] = percept.getVisionWidth();
        stats["vision_height"] = percept.getVisionHeight();
        
        auto vision = percept.getVision();
        stats["vision_data_size"] = vision.size();
        
        auto touch = percept.getTouch();
        stats["touch_data_size"] = touch.size();
        
        auto internal = percept.getInternal();
        stats["internal_data_size"] = internal.size();
        
        auto proprio = percept.getProprioception();
        stats["proprioception_data_size"] = proprio.size();
        
        return stats;
    }, "Get comprehensive world simulation statistics")
    .def("run_episode", [](SimpleWorld& self, double timestep, size_t steps) {
        py::list episode_data;
        
        for (size_t i = 0; i < steps; ++i) {
            py::dict step_data;
            step_data["step"] = static_cast<int>(i);
            step_data["time"] = self.getSimulationTime();
            
            auto percept = self.getSensoryPercept();
            step_data["vision_size"] = percept.getVision().size();
            step_data["touch_size"] = percept.getTouch().size();
            step_data["internal_size"] = percept.getInternal().size();
            
            episode_data.append(step_data);
            
            if (i < steps - 1) {
                self.update(timestep);
            }
        }
        
        return episode_data;
    }, py::arg("timestep"), py::arg("steps"), 
         "Run complete episode and return step-by-step data")
    .def("is_game_over", [](const SimpleWorld& self) {
        auto body = self.getAgentBody();
        return body.health <= 0.0f || body.energy <= 0.0f;
    }, "Check if simulation is over (agent dead or out of energy)")
    .def("get_terminal_state", [](const SimpleWorld& self) {
        auto body = self.getAgentBody();
        if (body.health <= 0.0f) {
            return std::make_pair("death", 0.0f);
        } else if (body.energy <= 0.0f) {
            return std::make_pair("exhaustion", 0.0f);
        } else {
            return std::make_pair("running", 0.0f);
        }
    }, "Get terminal state if simulation is over");
}

} // namespace nlm
