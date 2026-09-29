#include "DemoConstants.hpp"
#include <string>

namespace nlm {

const std::string DemoConstants::DEMO_NAME_PHASE2 = "NLM_Phase2_Real_Neural_Computation";
const std::string DemoConstants::DEMO_NAME_PLATONIC = "NLM_Platonic_Brain";
const std::string DemoConstants::DEMO_NAME_HIERARCHICAL = "NLM_Hierarchical_Brain";

const std::string DemoConstants::DEFAULT_CONFIG_FILE = "configs/default.cfg";
const std::string DemoConstants::DEMO_CONFIG_FILE = "configs/demo.cfg";
const std::string DemoConstants::LOG_CONFIG_FILE = "configs/log.cfg";

const std::string DemoConstants::EXPERIMENT_BASIC_CONNECTIVITY = "Basic_Connectivity";
const std::string DemoConstants::EXPERIMENT_PLASTICITY = "Plasticity_Learning";
const std::string DemoConstants::EXPERIMENT_STDP = "STDP_Verification";
const std::string DemoConstants::EXPERIMENT_INTEGRATED = "Integrated_Experiment";

const std::string DemoConstants::ERR_INSUFFICIENT_NEURONS = "Neuron count is insufficient for memory systems";
const std::string DemoConstants::ERR_INVALID_CONNECTION_PROBABILITY = "Connection probability must be between 0.0 and 1.0";
const std::string DemoConstants::ERR_INVALID_TIME_STEP = "Time step must be positive and less than 1.0";
const std::string DemoConstants::ERR_EXPERIMENT_ALREADY_RUNNING = "An experiment is already running";
const std::string DemoConstants::ERR_EXPERIMENT_NOT_FOUND = "Experiment not found";
const std::string DemoConstants::ERR_CONFIG_VALIDATION_FAILED = "Configuration validation failed";

const std::string DemoConstants::MSG_DEMO_INITIALIZED = "Demo system initialized successfully";
const std::string DemoConstants::MSG_EXPERIMENT_STARTED = "Experiment started: ";
const std::string DemoConstants::MSG_EXPERIMENT_COMPLETED = "Experiment completed: ";
const std::string DemoConstants::MSG_LEARNING_DETECTED = "Learning detected in neural network";
const std::string DemoConstants::MSG_PERFORMANCE_OPTIMIZED = "Performance optimization completed";

const std::vector<std::string> DemoConstants::LOG_LEVEL_NAMES = {
    "TRACE", "DEBUG", "INFO", "NOTICE", "WARNING", "ERROR", "CRITICAL", "FATAL"
};

const std::vector<std::string> DemoConstants::LOG_LEVEL_COLORS = {
    "\033[90m",  // Gray for TRACE
    "\033[36m",  // Cyan for DEBUG
    "\033[32m",  // Green for INFO
    "\033[94m",  // Blue for NOTICE
    "\033[33m",  // Yellow for WARNING
    "\033[31m",  // Red for ERROR
    "\033[35m",  // Magenta for CRITICAL
    "\033[41m"   // Red background for FATAL
};

const std::vector<std::string> DemoConstants::BENCHMARK_NAMES = {
    "neuron_creation",
    "synaptic_connection",
    "learning_speed",
    "memory_usage",
    "spike_rate",
    "plasticity_effectiveness"
};

const std::vector<std::string> DemoConstants::BENCHMARK_DESCRIPTIONS = {
    "Time to create neurons",
    "Time to establish synaptic connections",
    "Speed of learning and adaptation",
    "Memory consumption during operation",
    "Neural firing rate",
    "Effectiveness of plasticity rules"
};

const std::string DemoConstants::DEFAULT_OUTPUT_DIRECTORY = "output";
const std::string DemoConstants::DEFAULT_RESULTS_FILE = "output/results.csv";
const std::string DemoConstants::DEFAULT_LOG_FILE = "output/nlm.log";
const std::string DemoConstants::DEFAULT_PERFORMANCE_FILE = "output/performance.csv";

} // namespace nlm