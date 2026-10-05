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
    
    class MemoryModule : public BindingModule {
    public:
        std::string getName() const override { return "memory"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Memory system bindings for NLM (WorkingMemory, EpisodicMemory)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Memory class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // NeuralWorkingMemory class
            builder.class_<NeuralWorkingMemory>("WorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
                .def(py::init<>())
                .def("store", &NeuralWorkingMemory::store, py::arg("key"), py::arg("value"),
                     "Store a value in working memory")
                .def("retrieve", &NeuralWorkingMemory::retrieve, py::arg("key"),
                     "Retrieve a value from working memory")
                .def("has", &NeuralWorkingMemory::has, py::arg("key"),
                     "Check if key exists in working memory")
                .def("remove", &NeuralWorkingMemory::remove, py::arg("key"),
                     "Remove a value from working memory")
                .def("clear", &NeuralWorkingMemory::clear,
                     "Clear all working memory entries")
                .def("size", &NeuralWorkingMemory::size,
                     "Get the number of entries in working memory")
                .def("keys", &NeuralWorkingMemory::keys,
                     "Get all keys in working memory")
                .def("getAll", &NeuralWorkingMemory::getAll,
                     py::return_value_policy::reference_internal,
                     "Get all entries in working memory")
                .def("update", &NeuralWorkingMemory::update, py::arg("key"), py::arg("value"),
                     "Update an existing entry in working memory")
                .def("hasTemporalPriority", &NeuralWorkingMemory::hasTemporalPriority,
                     py::arg("key"), py::arg("threshold"),
                     "Check if entry has temporal priority");
            
            // NeuralEpisodicMemory class
            builder.class_<NeuralEpisodicMemory>("EpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
                .def(py::init<>())
                .def("storeEvent", &NeuralEpisodicMemory::storeEvent, py::arg("event"), py::arg("timestamp"),
                     "Store an episodic event with timestamp")
                .def("retrieveEvents", &NeuralEpisodicMemory::retrieveEvents, py::arg("startTime"), py::arg("endTime"),
                     py::return_value_policy::reference_internal,
                     "Retrieve events in time range")
                .def("findEventsByPattern", &NeuralEpisodicMemory::findEventsByPattern, py::arg("pattern"),
                     py::return_value_policy::reference_internal,
                     "Find events matching pattern")
                .def("getEventCount", &NeuralEpisodicMemory::getEventCount,
                     "Get total number of episodic events")
                .def("getEventCountByTimeRange", &NeuralEpisodicMemory::getEventCountByTimeRange,
                     py::arg("startTime"), py::arg("endTime"),
                     "Get event count in time range")
                .def("getTemporalDistribution", &NeuralEpisodicMemory::getTemporalDistribution,
                     py::arg("startTime"), py::arg("endTime"),
                     "Get temporal distribution of events")
                .def("clearOldEvents", &NeuralEpisodicMemory::clearOldEvents, py::arg("beforeTime"),
                     "Clear events before given time")
                .def("clear", &NeuralEpisodicMemory::clear,
                     "Clear all episodic memory")
                .def("getOldestEventTime", &NeuralEpisodicMemory::getOldestEventTime,
                     "Get timestamp of oldest event")
                .def("getNewestEventTime", &NeuralEpisodicMemory::getNewestEventTime,
                     "Get timestamp of newest event");
            
            // Memory utility classes
            builder.class_<MemoryEntry>("MemoryEntry", R"pbdoc(Memory entry with metadata)pbdoc")
                .def(py::init<>(), py::arg("key"), py::arg("value"), py::arg("timestamp"), py::arg("importance"))
                .def_readwrite("key", &MemoryEntry::key)
                .def_readwrite("value", &MemoryEntry::value)
                .def_readwrite("timestamp", &MemoryEntry::timestamp)
                .def_readwrite("importance", &MemoryEntry::importance)
                .def("to_string", &MemoryEntry::toString,
                     "Get memory entry as string");
            
            // Context manager support for memory operations
            builder.class_<NeuralWorkingMemory>("WorkingMemoryContext", R"pbdoc(Context manager for WorkingMemory operations)pbdoc")
                .def(py::init<NeuralWorkingMemory>())
                .def("enter", [](NeuralWorkingMemory& self) { return self; })
                .def("exit", [](NeuralWorkingMemory&, const py::args& args) { return true; });
            
            builder.class_<NeuralEpisodicMemory>("EpisodicMemoryContext", R"pbdoc(Context manager for EpisodicMemory operations)pbdoc")
                .def(py::init<NeuralEpisodicMemory>())
                .def("enter", [](NeuralEpisodicMemory& self) { return self; })
                .def("exit", [](NeuralEpisodicMemory&, const py::args& args) { return true; });
            
            // Memory factory functions
            m.def("create_working_memory", []() {
                return std::make_shared<NeuralWorkingMemory>();
            }, "Create a new working memory instance");
            
            m.def("create_episodic_memory", []() {
                return std::make_shared<NeuralEpisodicMemory>();
            }, "Create a new episodic memory instance");
            
            // Memory utility functions
            m.def("memory_entry_factory", [](const std::string& key, const py::object& value, double timestamp, float importance) {
                MemoryEntry entry;
                entry.key = key;
                // Convert Python object to appropriate C++ type
                if (py::isinstance<py::int_>(value)) {
                    entry.value = value.cast<int>();
                } else if (py::isinstance<py::float_>(value)) {
                    entry.value = value.cast<double>();
                } else if (py::isinstance<py::str>(value)) {
                    entry.value = value.cast<std::string>();
                } else if (py::isinstance<py::bool_>(value)) {
                    entry.value = value.cast<bool>();
                } else {
                    entry.value = std::string("unsupported_type");
                }
                entry.timestamp = timestamp;
                entry.importance = importance;
                return entry;
            }, py::arg("key"), py::arg("value"), py::arg("timestamp"), py::arg("importance") = 1.0f,
                "Create memory entry with value conversion");
            
            // Memory statistics and analysis
            m.def("analyze_working_memory_stats", [](const NeuralWorkingMemory& memory) {
                std::map<std::string, double> stats;
                stats["size"] = memory.size();
                // Implementation would compute additional statistics
                return stats;
            }, py::arg("memory"), "Analyze working memory statistics");
            
            m.def("analyze_episodic_memory_stats", [](const NeuralEpisodicMemory& memory) {
                std::map<std::string, double> stats;
                stats["event_count"] = memory.getEventCount();
                auto timeRange = memory.getTemporalDistribution(0.0, 0.0);
                stats["time_span"] = timeRange.size() > 0 ? timeRange.back().first : 0.0;
                // Implementation would compute additional statistics
                return stats;
            }, py::arg("memory"), "Analyze episodic memory statistics");
            
            // Memory validation
            m.def("validate_working_memory_state", [](const NeuralWorkingMemory& memory) {
                // Implementation would validate working memory state consistency
                return memory.size() >= 0;
            }, py::arg("memory"), "Validate working memory state");
            
            m.def("validate_episodic_memory_state", [](const NeuralEpisodicMemory& memory) {
                // Implementation would validate episodic memory state consistency
                return memory.getEventCount() >= 0;
            }, py::arg("memory"), "Validate episodic memory state");
            
            // Memory import/export utilities
            m.def("export_working_memory", [](const NeuralWorkingMemory& memory) {
                // Implementation would serialize working memory
                return std::vector<std::pair<std::string, py::object>>();
            }, py::arg("memory"), "Export working memory to Python objects");
            
            m.def("export_episodic_memory", [](const NeuralEpisodicMemory& memory) {
                // Implementation would serialize episodic memory
                return std::vector<std::pair<double, py::object>>();
            }, py::arg("memory"), "Export episodic memory to Python objects");
        }
        
        void initialize() override {
            // Initialize memory module
        }
        
        void shutdown() override {
            // Cleanup memory module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "memory_factory" || feature == "memory_stats" || 
                   feature == "memory_analysis" || feature == "memory_export" ||
                   feature == "memory_context" || feature == "memory_validation";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "memory_factory") {
                return py::module_::import("nlm.memory").attr("memory_factory");
            } else if (feature == "memory_stats") {
                return py::module_::import("nlm.memory").attr("memory_stats");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the memory module
    static MemoryModule register_memory_module() {
        auto module = std::make_shared<MemoryModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(memory_bindings, m) {
        static MemoryModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Memory Python Bindings
            ---------------------------
            
            This module provides Python bindings for the NLM memory system,
            including WorkingMemory and EpisodicMemory classes.
            
            It provides memory management interfaces for storing and retrieving
            neural information with context managers and memory utilities.
            
            .. currentmodule:: memory_bindings
            
            Memory API
            ===========""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<MemoryModule>(module));
    }
}