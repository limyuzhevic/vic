#pragma once

#include "performance_bindings.h"
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
    
    class PerformanceModule : public BindingModule {
    public:
        std::string getName() const override { return "performance"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Performance system bindings for NLM (ParallelProcessing, CheckpointSystem, etc.)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Performance class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // ParallelProcessing class
            builder.class_<ParallelProcessing>("ParallelProcessing", R"pbdoc(Parallel processing system)pbdoc")
                .def(py::init<>())
                .def("setNumThreads", &ParallelProcessing::setNumThreads, py::arg("threads"),
                     "Set number of threads for parallel processing")
                .def("getNumThreads", &ParallelProcessing::getNumThreads,
                     "Get number of threads")
                .def("processParallel", &ParallelProcessing::processParallel, py::arg("data"),
                     "Process data in parallel")
                .def("isEnabled", &ParallelProcessing::isEnabled,
                     "Check if parallel processing is enabled")
                .def("enable", &ParallelProcessing::enable, "Enable parallel processing")
                .def("disable", &ParallelProcessing::disable, "Disable parallel processing");
            
            // CheckpointSystem class
            builder.class_<CheckpointSystem>("CheckpointSystem", R"pbdoc(Checkpoint and recovery system)pbdoc")
                .def(py::init<>())
                .def("saveCheckpoint", &CheckpointSystem::saveCheckpoint, py::arg("checkpointName"),
                     "Save checkpoint with given name")
                .def("loadCheckpoint", &CheckpointSystem::loadCheckpoint, py::arg("checkpointName"),
                     "Load checkpoint with given name")
                .def("deleteCheckpoint", &CheckpointSystem::deleteCheckpoint, py::arg("checkpointName"),
                     "Delete checkpoint with given name")
                .def("listCheckpoints", &CheckpointSystem::listCheckpoints,
                     py::return_value_policy::reference_internal,
                     "List all checkpoints")
                .def("getLastCheckpointTime", &CheckpointSystem::getLastCheckpointTime,
                     "Get time of last checkpoint")
                .def("setAutoSaveInterval", &CheckpointSystem::setAutoSaveInterval, py::arg("interval"),
                     "Set automatic save interval")
                .def("getAutoSaveInterval", &CheckpointSystem::getAutoSaveInterval,
                     "Get automatic save interval")
                .def("enableAutoSave", &CheckpointSystem::enableAutoSave, "Enable automatic checkpoint saving")
                .def("disableAutoSave", &CheckpointSystem::disableAutoSave, "Disable automatic checkpoint saving");
            
            // OptimizedBrain class
            builder.class_<OptimizedBrain>("OptimizedBrain", R"pbdoc(Optimized brain implementation)pbdoc")
                .def(py::init<>())
                .def("setOptimizationLevel", &OptimizedBrain::setOptimizationLevel, py::arg("level"),
                     "Set optimization level")
                .def("getOptimizationLevel", &OptimizedBrain::getOptimizationLevel,
                     "Get optimization level")
                .def("enableSIMD", &OptimizedBrain::enableSIMD, "Enable SIMD optimizations")
                .def("disableSIMD", &OptimizedBrain::disableSIMD, "Disable SIMD optimizations")
                .def("enableSparseConnectivity", &OptimizedBrain::enableSparseConnectivity, "Enable sparse connectivity")
                .def("disableSparseConnectivity", &OptimizedBrain::disableSparseConnectivity, "Disable sparse connectivity")
                .def("enableMemoryPool", &OptimizedBrain::enableMemoryPool, "Enable memory pool optimization")
                .def("disableMemoryPool", &OptimizedBrain::disableMemoryPool, "Disable memory pool optimization")
                .def("getPerformanceMetrics", &OptimizedBrain::getPerformanceMetrics,
                     py::return_value_policy::reference_internal,
                     "Get performance metrics");
            
            // EventQueue class
            builder.class_<EventQueue>("EventQueue", R"pbdoc(Event queue system)pbdoc")
                .def(py::init<>())
                .def("pushEvent", &EventQueue::pushEvent, py::arg("eventId"), py::arg("priority"),
                     "Push event with priority")
                .def("popEvent", &EventQueue::popEvent,
                     "Pop next event from queue")
                .def("peekEvent", &EventQueue::peekEvent,
                     py::return_value_policy::reference_internal,
                     "Peek at next event without removing")
                .def("getEventCount", &EventQueue::getEventCount,
                     "Get number of events in queue")
                .def("clear", &EventQueue::clear, "Clear all events")
                .def("getEventProcessingTime", &EventQueue::getEventProcessingTime,
                     "Get average event processing time");
            
            // SIMDVectorization class
            builder.class_<SIMDVectorization>("SIMDVectorization", R"pbdoc(SIMD vectorization utilities)pbdoc")
                .def(py::init<>())
                .def("addVector", &SIMDVectorization::addVector, py::arg("vec1"), py::arg("vec2"),
                     "Add two vectors using SIMD")
                .def("multiplyVector", &SIMDVectorization::multiplyVector, py::arg("vec1"), py::arg("scalar"),
                     "Multiply vector by scalar using SIMD")
                .def("dotProduct", &SIMDVectorization::dotProduct, py::arg("vec1"), py::arg("vec2"),
                     "Calculate dot product using SIMD")
                .def("isSIMDAvailable", &SIMDVectorization::isSIMDAvailable,
                     "Check if SIMD is available on this system")
                .def("getSIMDLevel", &SIMDVectorization::getSIMDLevel,
                     "Get SIMD instruction set level available");
            
            // MemoryPool class
            builder.class_<MemoryPool>("MemoryPool", R"pbdoc(Memory pool management)pbdoc")
                .def(py::init<>())
                .def("allocate", &MemoryPool::allocate, py::arg("size"),
                     "Allocate memory from pool")
                .def("deallocate", &MemoryPool::deallocate, py::arg("ptr"), py::arg("size"),
                     "Deallocate memory back to pool")
                .def("getTotalAllocated", &MemoryPool::getTotalAllocated,
                     "Get total allocated memory")
                .def("getAvailableMemory", &MemoryPool::getAvailableMemory,
                     "Get available memory in pool")
                .def("setMemoryLimit", &MemoryPool::setMemoryLimit, py::arg("limit"),
                     "Set memory limit for pool")
                .def("getMemoryLimit", &MemoryPool::getMemoryLimit,
                     "Get memory limit for pool")
                .def("enableAutoCleanup", &MemoryPool::enableAutoCleanup, "Enable automatic memory cleanup")
                .def("disableAutoCleanup", &MemoryPool::disableAutoCleanup, "Disable automatic memory cleanup");
            
            // SparseConnectivity class
            builder.class_<SparseConnectivity>("SparseConnectivity", R"pbdoc(Sparse connectivity representation)pbdoc")
                .def(py::init<>())
                .def("addConnection", &SparseConnectivity::addConnection, py::arg("sourceId"), py::arg("targetId"), py::arg("weight"),
                     "Add sparse connection between neurons")
                .def("removeConnection", &SparseConnectivity::removeConnection, py::arg("sourceId"), py::arg("targetId"),
                     "Remove connection between neurons")
                .def("getConnection", &SparseConnectivity::getConnection, py::arg("sourceId"), py::arg("targetId"),
                     "Get connection weight between neurons")
                .def("getOutgoingConnections", &SparseConnectivity::getOutgoingConnections, py::arg("sourceId"),
                     py::return_value_policy::reference_internal,
                     "Get all outgoing connections from neuron")
                .def("getIncomingConnections", &SparseConnectivity::getIncomingConnections, py::arg("targetId"),
                     py::return_value_policy::reference_internal,
                     "Get all incoming connections to neuron")
                .def("getConnectionCount", &SparseConnectivity::getConnectionCount,
                     "Get total number of connections")
                .def("getDensity", &SparseConnectivity::getDensity,
                     "Get connection density")
                .def("enableCompression", &SparseConnectivity::enableCompression, "Enable connection compression")
                .def("disableCompression", &SparseConnectivity::disableCompression, "Disable connection compression");
            
            // Context manager support for performance operations
            builder.class_<ParallelProcessing>("ParallelProcessingContext", R"pbdoc(Context manager for ParallelProcessing operations)pbdoc")
                .def(py::init<ParallelProcessing>())
                .def("enter", [](ParallelProcessing& self) { return self; })
                .def("exit", [](ParallelProcessing&, const py::args& args) { return true; });
            
            builder.class_<CheckpointSystem>("CheckpointSystemContext", R"pbdoc(Context manager for CheckpointSystem operations)pbdoc")
                .def(py::init<CheckpointSystem>())
                .def("enter", [](CheckpointSystem& self) { return self; })
                .def("exit", [](CheckpointSystem&, const py::args& args) { return true; });
            
            // Performance factory functions
            m.def("create_parallel_processing", []() {
                return std::make_shared<ParallelProcessing>();
            }, "Create a new parallel processing system");
            
            m.def("create_checkpoint_system", []() {
                return std::make_shared<CheckpointSystem>();
            }, "Create a new checkpoint system");
            
            m.def("create_optimized_brain", []() {
                return std::make_shared<OptimizedBrain>();
            }, "Create an optimized brain implementation");
            
            m.def("create_event_queue", []() {
                return std::make_shared<EventQueue>();
            }, "Create a new event queue");
            
            m.def("create_simd_vectorization", []() {
                return std::make_shared<SIMDVectorization>();
            }, "Create SIMD vectorization utilities");
            
            m.def("create_memory_pool", []() {
                return std::make_shared<MemoryPool>();
            }, "Create a memory pool");
            
            m.def("create_sparse_connectivity", []() {
                return std::make_shared<SparseConnectivity>();
            }, "Create sparse connectivity representation");
            
            // Performance utilities
            m.def("get_performance_system_names", []() {
                return std::vector<std::string>{
                    "parallel_processing", "checkpoint_system", "optimized_brain",
                    "event_queue", "simd_vectorization", "memory_pool", "sparse_connectivity"
                };
            }, "Get list of available performance systems");
            
            m.def("performance_system_info", [](const std::string& systemName) {
                // Implementation would return system information
                return std::map<std::string, std::string>{{ "name", systemName }, { "description", "" }};
            }, py::arg("systemName"), "Get information about a performance system");
            
            // Performance monitoring utilities
            m.def("get_system_performance_metrics", []() {
                // Implementation would return system performance metrics
                std::map<std::string, double> metrics;
                metrics["cpu_usage"] = 0.0;
                metrics["memory_usage"] = 0.0;
                metrics["thread_count"] = 0.0;
                metrics["simd_available"] = false;
                return metrics;
            }, "Get system performance metrics");
            
            m.def("get_performance_benchmark", [](const std::string& operation) {
                // Implementation would run performance benchmark
                return 0.0;
            }, py::arg("operation"), "Run performance benchmark");
            
            // Performance validation
            m.def("validate_performance_state", [](const ParallelProcessing& parallel) {
                // Implementation would validate parallel processing state
                return parallel.isEnabled();
            }, py::arg("parallel"), "Validate parallel processing state");
            
            m.def("validate_checkpoint_state", [](const CheckpointSystem& checkpoint) {
                // Implementation would validate checkpoint system state
                return true;
            }, py::arg("checkpoint"), "Validate checkpoint system state");
            
            // Performance simulation utilities
            m.def("simulate_performance_load", [](const ParallelProcessing& parallel, double load) {
                // Implementation would simulate performance under load
                parallel.setNumThreads(static_cast<int>(load));
            }, py::arg("parallel"), py::arg("load"), "Simulate performance under load");
            
            m.def("benchmark_vectorization", [](const SIMDVectorization& simd, const std::vector<double>& vec1, const std::vector<double>& vec2) {
                // Implementation would benchmark vectorization performance
                return simd.dotProduct(vec1, vec2);
            }, py::arg("simd"), py::arg("vec1"), py::arg("vec2"), "Benchmark vectorization performance");
        }
        
        void initialize() override {
            // Initialize performance module
        }
        
        void shutdown() override {
            // Cleanup performance module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "performance_factory" || feature == "performance_monitoring" || 
                   feature == "performance_benchmark" || feature == "performance_simulation" ||
                   feature == "performance_context" || feature == "performance_validation";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "performance_factory") {
                return py::module_::import("nlm.performance").attr("performance_factory");
            } else if (feature == "performance_monitoring") {
                return py::module_::import("nlm.performance").attr("performance_monitoring");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the performance module
    static PerformanceModule register_performance_module() {
        auto module = std::make_shared<PerformanceModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(performance_bindings, m) {
        static PerformanceModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Performance Python Bindings
            ------------------------------
            
            This module provides Python bindings for the NLM performance system,
            including ParallelProcessing, CheckpointSystem, OptimizedBrain, EventQueue,
            SIMDVectorization, MemoryPool, and SparseConnectivity classes.
            
            It provides performance interfaces for optimizing neural computation
            with parallel processing, checkpointing, vectorization, and memory management,
            with context managers and performance utilities.
            
            .. currentmodule:: performance_bindings
            
            Performance API
            =================""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<PerformanceModule>(module));
    }
}