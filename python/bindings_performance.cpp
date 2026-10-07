// Performance system bindings

#include "bindings.h"

namespace nlm {

void bind_performance_module(py::module_& m) {
    // SpikeSystem class
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(
        Event-driven spike propagation system.
        
        The spike system provides:
        - Real-time spike generation and propagation
        - Event-driven simulation
        - Spike timing precision
        - Network-wide synchronization
        
        Key features:
        - Integrate-and-fire neuron dynamics
        - Spike event queue processing
        - Synaptic transmission timing
        - Refractory period management
    )pbdoc")
        .def(py::init<>())
        .def("addNeuron", &SpikeSystem::addNeuron, py::arg("neuron"), "Add neuron to spike system")
        .def("removeNeuron", &SpikeSystem::removeNeuron, py::arg("neuron"), "Remove neuron from spike system")
        .def("triggerSpike", &SpikeSystem::triggerSpike, py::arg("neuron"), "Manually trigger spike from neuron")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentTime"), "Process queued spikes")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount, "Get total spike count")
        .def("getActiveSpikeCount", &SpikeSystem::getActiveSpikeCount, "Get active spike count")
        .def("getLastSpikeTime", &SpikeSystem::getLastSpikeTime, "Get last spike time")
        .def("clearSpikes", &SpikeSystem::clearSpikes, "Clear all queued spikes")
        .def("getMaxFiringRate", &SpikeSystem::getMaxFiringRate, "Get maximum firing rate")
        .def("setMaxFiringRate", &SpikeSystem::setMaxFiringRate, py::arg("rate"), "Set maximum firing rate");

    // NeuralDynamics class
    py::class_<NeuralDynamics>(m, "NeuralDynamics", R"pbdoc(
        Neural dynamics and integration system.
        
        Neural dynamics provides:
        - Neuron state integration
        - Membrane potential dynamics
        - Ion channel modeling
        - Voltage-gated conductances
        
        Key features:
        - Real neuron physics simulation
        - Integrate-and-fire models
        - Conductance-based models
        - Synaptic current integration
    )pbdoc")
        .def(py::init<>())
        .def("integrateNeuron", &NeuralDynamics::integrateNeuron,
             py::arg("neuron"), py::arg("currentTime"), "Integrate neuron state")
        .def("getMembranePotential", &NeuralDynamics::getMembranePotential, py::arg("neuron"),
             py::return_value_policy::reference_internal, "Get membrane potential")
        .def("setMembranePotential", &NeuralDynamics::setMembranePotential,
             py::arg("neuron"), py::arg("potential"), "Set membrane potential")
        .def("getThreshold", &NeuralDynamics::getThreshold, py::arg("neuron"),
             py::return_value_policy::reference_internal, "Get firing threshold")
        .def("setThreshold", &NeuralDynamics::setThreshold,
             py::arg("neuron"), py::arg("threshold"), "Set firing threshold")
        .def("getRefractoryPeriod", &NeuralDynamics::getRefractoryPeriod, py::arg("neuron"),
             py::return_value_policy::reference_internal, "Get refractory period")
        .def("setRefractoryPeriod", &NeuralDynamics::setRefractoryPeriod,
             py::arg("neuron"), py::arg("period"), "Set refractory period");

    // MemoryPool class
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(
        Memory pool for efficient neural data storage.
        
        Memory pool provides:
        - Efficient memory allocation
        - Pre-allocated data structures
        - Reduced garbage collection
        - High-performance access patterns
        
        Key features:
        - Pre-allocated neuron memory
        - Synapse memory management
        - Event queue optimization
        - Fast data access patterns
    )pbdoc")
        .def(py::init<>())
        .def("allocateNeuronMemory", &MemoryPool::allocateNeuronMemory, py::arg("count"),
             "Allocate memory for neurons")
        .def("allocateSynapseMemory", &MemoryPool::allocateSynapseMemory, py::arg("count"),
             "Allocate memory for synapses")
        .def("getMemoryUsage", &MemoryPool::getMemoryUsage, "Get current memory usage")
        .def("getPeakMemoryUsage", &MemoryPool::getPeakMemoryUsage, "Get peak memory usage")
        .def("resetPeakMemoryUsage", &MemoryPool::resetPeakMemoryUsage, "Reset peak memory usage");

    // EventQueue class
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(
        Event queue for time-ordered processing.
        
        EventQueue provides:
        - Time-ordered event processing
        - Priority queue implementation
        - Event callback system
        - High-performance event handling
        
        Key features:
        - Process events in time order
        - Support for different event types
        - Efficient event insertion/removal
        - Callback registration system
    )pbdoc")
        .def(py::init<>())
        .def("addEvent", &EventQueue::addEvent, py::arg("event"), py::arg("time"),
             "Add event to queue with execution time")
        .def("processEvents", &EventQueue::processEvents, py::arg("currentTime"),
             "Process all events scheduled up to current time")
        .def("getEventCount", &EventQueue::getEventCount, "Get number of queued events")
        .def("clearEvents", &EventQueue::clearEvents, "Clear all events")
        .def("getNextEventTime", &EventQueue::getNextEventTime, "Get time of next event")
        .def("setMaxEventAge", &EventQueue::setMaxEventAge, py::arg("maxAge"), "Set maximum event age");

    // CheckpointSystem class
    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(
        Checkpoint system for brain state saving and loading.
        
        CheckpointSystem provides:
        - Brain state serialization
        - Automatic checkpoint creation
        - Incremental backup support
        - Efficient storage management
        
        Key features:
        - Save complete brain state
        - Load from checkpoints
        - Incremental state changes
        - Backup and recovery
    )pbdoc")
        .def(py::init<>())
        .def("createCheckpoint", &CheckpointSystem::createCheckpoint,
             py::arg("brain"), py::arg("filepath"), "Create checkpoint file")
        .def("restoreCheckpoint", &CheckpointSystem::restoreCheckpoint,
             py::arg("brain"), py::arg("filepath"), "Restore from checkpoint")
        .def("getCheckpointCount", &CheckpointSystem::getCheckpointCount, "Get checkpoint count")
        .def("listCheckpoints", &CheckpointSystem::listCheckpoints, "List available checkpoints")
        .def("deleteCheckpoint", &CheckpointSystem::deleteCheckpoint, py::arg("filepath"), "Delete checkpoint")
        .def("clearOldCheckpoints", &CheckpointSystem::clearOldCheckpoints,
             py::arg("maxCheckpoints"), "Clear old checkpoints");

    // ParallelProcessing class
    py::class_<ParallelProcessing>(m, "ParallelProcessing", R"pbdoc(
        Parallel processing for neural computation.
        
        ParallelProcessing provides:
        - Concurrent neural processing
        - Thread-safe operations
        - Load balancing
        - Scalable computation
        
        Key features:
        - Multi-threading support
        - Work distribution
        - Shared memory access
        - Performance optimization
    )pbdoc")
        .def(py::init<>())
        .def("enableParallel", &ParallelProcessing::enableParallel, py::arg("enable"),
             "Enable or disable parallel processing")
        .def("isParallelEnabled", &ParallelProcessing::isParallelEnabled, "Check if parallel enabled")
        .def("setThreadCount", &ParallelProcessing::setThreadCount, py::arg("count"),
             "Set number of threads")
        .def("getThreadCount", &ParallelProcessing::getThreadCount, "Get number of threads")
        .def("processBatch", &ParallelProcessing::processBatch,
             py::arg("items"), py::arg("processor"), "Process batch of items in parallel");
}

} // namespace nlm