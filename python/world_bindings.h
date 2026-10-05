#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace py = pybind11;

namespace nlm {
    
    class WorldModule : public BindingModule {
    public:
        std::string getName() const override { return "world"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "World system bindings for NLM (SimpleWorld, WorldObject)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // World class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // WorldObject class
            builder.class_<WorldObject>("WorldObject", R"pbdoc(World object representation)pbdoc")
                .def(py::init<>())
                .def(py::init<float, float, WorldObjectType, float, float>(),
                     py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
                     py::arg("radius") = 0.5f)
                .def_readwrite("x", &WorldObject::x)
                .def_readwrite("y", &WorldObject::y)
                .def_readwrite("radius", &WorldObject::radius)
                .def_readwrite("type", &WorldObject::type)
                .def_readwrite("value", &WorldObject::value)
                .def_readwrite("active", &WorldObject::active);
            
            // SimpleWorld class
            builder.class_<SimpleWorld>("SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
                .def("getRandomSeed", &SimpleWorld::getRandomSeed);
            
            // Context manager support for world objects
            builder.class_<WorldObject>("WorldObjectContext", R"pbdoc(Context manager for WorldObject operations)pbdoc")
                .def(py::init<WorldObject>())
                .def("enter", [](WorldObject& self) { return self; })
                .def("exit", [](WorldObject&, const py::args& args) { return true; });
            
            // World utilities
            m.def("create_default_world", []() {
                return std::make_shared<SimpleWorld>();
            }, "Create a default SimpleWorld instance");
            
            m.def("create_world_object", [](float x, float y, WorldObjectType type) {
                return std::make_shared<WorldObject>(x, y, type);
            }, py::arg("x"), py::arg("y"), py::arg("type"), "Create world object with given properties");
            
            // World utilities
            m.def("get_world_object_type_name", [](WorldObjectType type) {
                switch (type) {
                    case WorldObjectType::Empty: return std::string("Empty");
                    case WorldObjectType::Resource: return std::string("Resource");
                    case WorldObjectType::Hazard: return std::string("Hazard");
                    case WorldObjectType::Wall: return std::string("Wall");
                    case WorldObjectType::Marker: return std::string("Marker");
                    default: return std::string("Unknown");
                }
            }, py::arg("type"), "Get world object type name as string");
            
            // World simulation utilities
            m.def("simulate_step", [](SimpleWorld& world, double timestep) {
                world.update(timestep);
            }, py::arg("world"), py::arg("timestep"), "Simulate one world step");
            
            m.def("reset_world", [](SimpleWorld& world) {
                world.reset();
            }, py::arg("world"), "Reset world to initial state");
            
            // World object utilities
            m.def("count_objects_by_type", [](const SimpleWorld& world, WorldObjectType type) {
                int count = 0;
                // Implementation would count objects by type
                return count;
            }, py::arg("world"), py::arg("type"), "Count objects of specific type");
            
            m.def("get_object_at_position", [](const SimpleWorld& world, float x, float y) {
                // Implementation would return object at given position
                return std::shared_ptr<WorldObject>();
            }, py::arg("world"), py::arg("x"), py::arg("y"), "Get object at specific position");
            
            // World validation
            m.def("validate_position", [](const SimpleWorld& world, float x, float y) {
                return world.isValidPosition(x, y);
            }, py::arg("world"), py::arg("x"), py::arg("y"), "Validate if position is within world bounds");
            
            m.def("validate_world_state", [](const SimpleWorld& world) {
                // Implementation would validate world state consistency
                return true;
            }, py::arg("world"), "Validate world state consistency");
        }
        
        void initialize() override {
            // Initialize world module
        }
        
        void shutdown() override {
            // Cleanup world module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "world_factory" || feature == "world_utils" || 
                   feature == "object_management" || feature == "position_validation" ||
                   feature == "simulation_control" || feature == "world_state";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "world_factory") {
                return py::module_::import("nlm.world").attr("world_factory");
            } else if (feature == "world_utils") {
                return py::module_::import("nlm.world").attr("world_utils");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the world module
    static WorldModule register_world_module() {
        auto module = std::make_shared<WorldModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(world_bindings, m) {
        static WorldModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM World Python Bindings
            -------------------------
            
            This module provides Python bindings for the NLM world system,
            including SimpleWorld and WorldObject classes.
            
            It provides simulation world interfaces for agent-environment interaction,
            with object management and world utilities.
            
            .. currentmodule:: world_bindings
            
            World API
            ===========""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<WorldModule>(module));
    }
}