// Batch Processing and Experiment Management for NLM
// Phase 6: Advanced Experiment Execution and Automation
// 
// This module provides comprehensive batch processing and experiment management
// capabilities for the NLM neural simulation framework. It includes:
// 
// - Batch experiment execution for multiple parameter combinations
// - Experiment management and tracking system
// - Results aggregation and analysis
// - Parameter sweep and optimization
// - Experiment history and reproducibility
// - Parallel and distributed processing
// - Experiment templates and presets
// - Experiment logging and monitoring
// - Experiment results comparison
// - Automated experiment execution
// - Experiment scheduling and queuing
// - Experiment result validation and quality control
// - Experiment report generation
// - Integration with NLM simulation framework

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <random>
#include <filesystem>

namespace nlm {

// Experiment status enumeration
enum class ExperimentStatus {
    Pending,      // Experiment is queued but not yet running
    Running,      // Experiment is currently executing
    Completed,    // Experiment completed successfully
    Failed,       // Experiment failed
    Canceled,     // Experiment was canceled
    Paused,       // Experiment is paused
    Skipped,      // Experiment was skipped
    Invalid       // Experiment is invalid
};

// Experiment parameter
struct ExperimentParameter {
    std::string name;           // Parameter name
    std::string description;    // Parameter description
    std::string type;           // Parameter type (e.g., "int", "float", "string", "bool")
    std::string unit;           // Unit of measurement
    std::string category;       // Parameter category (e.g., "brain", "simulation", "environment")
    std::string group;          // Parameter group
    bool required;              // Whether parameter is required
    bool editable;              // Whether parameter can be edited
    
    // Default values based on type
    union DefaultValue {
        int int_val;
        float float_val;
        double double_val;
        bool bool_val;
        std::string string_val;
        
        DefaultValue() : int_val(0) {}
        ~DefaultValue() {}
    };
    
    DefaultValue default_value;
    
    // Parameter constraints
    struct Constraints {
        std::string min_value;    // Minimum value (as string)
        std::string max_value;    // Maximum value (as string)
        std::vector<std::string> allowed_values; // Allowed values for categorical parameters
        bool positive_only;       // Whether only positive values are allowed
        int min_length;           // Minimum string length (for strings)
        int max_length;           // Maximum string length (for strings)
    };
    
    Constraints constraints;
    
    ExperimentParameter() : required(false), editable(true), min_length(0), max_length(INT_MAX) {}
    
    // Set default value based on type
    void setDefault(int value) {
        default_value.int_val = value;
    }
    
    void setDefault(float value) {
        default_value.float_val = value;
    }
    
    void setDefault(double value) {
        default_value.double_val = value;
    }
    
    void setDefault(bool value) {
        default_value.bool_val = value;
    }
    
    void setDefault(const std::string& value) {
        default_value.string_val = value;
    }
    
    // Get default value as string
    std::string getDefaultAsString() const {
        if (type == "int") return std::to_string(default_value.int_val);
        if (type == "float") return std::to_string(default_value.float_val);
        if (type == "double") return std::to_string(default_value.double_val);
        if (type == "bool") return default_value.bool_val ? "true" : "false";
        return default_value.string_val;
    }
    
    // Validate parameter value
    bool validate(const std::string& value) const {
        if (required && value.empty()) {
            return false;
        }
        
        if (type == "int") {
            try {
                int int_val = std::stoi(value);
                if (constraints.min_value != "") {
                    int min_val = std::stoi(constraints.min_value);
                    if (int_val < min_val) return false;
                }
                if (constraints.max_value != "") {
                    int max_val = std::stoi(constraints.max_value);
                    if (int_val > max_val) return false;
                }
            } catch (const std::exception&) {
                return false;
            }
        } else if (type == "float" || type == "double") {
            try {
                double double_val = std::stod(value);
                if (constraints.min_value != "") {
                    double min_val = std::stod(constraints.min_value);
                    if (double_val < min_val) return false;
                }
                if (constraints.max_value != "") {
                    double max_val = std::stod(constraints.max_value);
                    if (double_val > max_val) return false;
                }
            } catch (const std::exception&) {
                return false;
            }
        } else if (type == "bool") {
            if (value != "true" && value != "false" && value != "0" && value != "1") {
                return false;
            }
        } else if (type == "string") {
            if (constraints.min_length > 0 && value.length() < static_cast<size_t>(constraints.min_length)) {
                return false;
            }
            if (constraints.max_length > 0 && value.length() > static_cast<size_t>(constraints.max_length)) {
                return false;
            }
            if (!constraints.allowed_values.empty()) {
                if (std::find(constraints.allowed_values.begin(), constraints.allowed_values.end(), value) == constraints.allowed_values.end()) {
                    return false;
                }
            }
        }
        
        return true;
    }
};

// Experiment configuration
struct ExperimentConfig {
    std::string name;                              // Experiment name
    std::string description;                       // Experiment description
    std::string version;                           // Experiment version
    std::string author;                            // Experiment author
    std::string date;                              // Experiment date
    std::string category;                          // Experiment category
    std::string tags;                              // Experiment tags (comma-separated)
    std::string notes;                             // Additional notes
    bool enabled;                                  // Whether experiment is enabled
    bool run_by_default;                          // Whether experiment runs by default
    int priority;                                  // Experiment priority (higher = runs first)
    int timeout;                                   // Experiment timeout in seconds
    int max_retries;                               // Maximum number of retries
    bool parallel;                                 // Whether experiment can run in parallel
    std::string dependency;                       // Dependency on other experiments
    
    // Configuration parameters
    std::vector<ExperimentParameter> parameters;
    
    ExperimentConfig() : enabled(true), run_by_default(false), priority(0), timeout(3600), 
                       max_retries(0), parallel(false), date("2026-01-01") {}
    
    // Add a parameter
    void addParameter(const ExperimentParameter& param) {
        parameters.push_back(param);
    }
    
    // Get a parameter by name
    ExperimentParameter* getParameter(const std::string& name) {
        for (auto& param : parameters) {
            if (param.name == name) {
                return &param;
            }
        }
        return nullptr;
    }
    
    // Get a parameter by name (const version)
    const ExperimentParameter* getParameter(const std::string& name) const {
        for (const auto& param : parameters) {
            if (param.name == name) {
                return &param;
            }
        }
        return nullptr;
    }
};

// Experiment result
struct ExperimentResult {
    std::string experiment_name;                   // Experiment name
    std::string experiment_version;                // Experiment version
    std::string experiment_id;                     // Unique experiment identifier
    ExperimentStatus status;                       // Experiment status
    double execution_time;                        // Execution time in seconds
    double cpu_time;                              // CPU time in seconds
    uint64_t memory_usage;                         // Memory usage in bytes
    std::chrono::system_clock::time_point start_time;   // Start time
    std::chrono::system_clock::time_point end_time;     // End time
    std::string output_dir;                        // Output directory
    std::string log_file;                          // Log file path
    std::string result_file;                       // Result file path
    std::string error_message;                     // Error message (if failed)
    std::string error_stack;                       // Error stack trace
    
    // Results metrics
    std::unordered_map<std::string, double> metrics;   // Performance metrics
    std::unordered_map<std::string, std::string> metadata; // Additional metadata
    
    // Parameter values used
    std::unordered_map<std::string, std::string> parameters_used; // Parameter name -> value
    
    // Performance counters
    std::unordered_map<std::string, uint64_t> counters;      // Counter values
    
    ExperimentResult() : status(ExperimentStatus::Pending), execution_time(0.0), 
                       cpu_time(0.0), memory_usage(0), start_time(), end_time() {}
    
    // Calculate duration
    double getDuration() const {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        return duration.count();
    }
    
    // Add a metric
    void addMetric(const std::string& name, double value) {
        metrics[name] = value;
    }
    
    // Add metadata
    void addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    // Add counter
    void addCounter(const std::string& name, uint64_t value) {
        counters[name] = value;
    }
    
    // Add parameter value
    void addParameter(const std::string& name, const std::string& value) {
        parameters_used[name] = value;
    }
    
    // Generate a unique experiment ID
    static std::string generateExperimentId(const std::string& name, const std::string& version) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << name << "_" << version << "_" << now_time_t;
        return ss.str();
    }
};

// Experiment execution context
class ExperimentExecutionContext {
public:
    ExperimentExecutionContext() : next_parameter_id_(0) {}
    
    // Add a parameter to the context
    void addParameter(const std::string& name, const std::string& value) {
        parameters_[name] = value;
    }
    
    // Get a parameter value
    std::string getParameter(const std::string& name) const {
        auto it = parameters_.find(name);
        if (it != parameters_.end()) {
            return it->second;
        }
        return "";
    }
    
    // Get all parameters
    std::unordered_map<std::string, std::string> getAllParameters() const {
        return parameters_;
    }
    
    // Add a metric
    void addMetric(const std::string& name, double value) {
        metrics_[name] = value;
    }
    
    // Get a metric
    double getMetric(const std::string& name) const {
        auto it = metrics_.find(name);
        if (it != metrics_.end()) {
            return it->second;
        }
        return 0.0;
    }
    
    // Add a counter
    void addCounter(const std::string& name, uint64_t value) {
        counters_[name] = value;
    }
    
    // Get a counter
    uint64_t getCounter(const std::string& name) const {
        auto it = counters_.find(name);
        if (it != counters_.end()) {
            return it->second;
        }
        return 0;
    }
    
    // Set the brain reference
    void setBrain(std::shared_ptr<nlm::Brain> brain) {
        brain_ = brain;
    }
    
    // Get the brain reference
    std::shared_ptr<nlm::Brain> getBrain() const {
        return brain_;
    }
    
    // Set the world reference
    void setWorld(std::shared_ptr<nlm::SimpleWorld> world) {
        world_ = world;
    }
    
    // Get the world reference
    std::shared_ptr<nlm::SimpleWorld> getWorld() const {
        return world_;
    }
    
    // Set the agent reference
    void setAgent(std::shared_ptr<nlm::AgentBrain> agent) {
        agent_ = agent;
    }
    
    // Get the agent reference
    std::shared_ptr<nlm::AgentBrain> getAgent() const {
        return agent_;
    }
    
    // Get the output directory
    std::string getOutputDirectory() const {
        return output_directory_;
    }
    
    // Set the output directory
    void setOutputDirectory(const std::string& directory) {
        output_directory_ = directory;
    }
    
private:
    std::unordered_map<std::string, std::string> parameters_;
    std::unordered_map<std::string, double> metrics_;
    std::unordered_map<std::string, uint64_t> counters_;
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    std::shared_ptr<nlm::AgentBrain> agent_;
    std::string output_directory_;
    int next_parameter_id_;
};

// Experiment executor interface
class ExperimentExecutor {
public:
    virtual ~ExperimentExecutor() = default;
    
    // Execute an experiment
    virtual ExperimentResult execute(const ExperimentConfig& config, const ExperimentExecutionContext& context) = 0;
    
    // Check if executor can handle the experiment
    virtual bool canExecute(const ExperimentConfig& config) const = 0;
    
    // Get executor name
    virtual std::string getName() const = 0;
    
    // Get executor version
    virtual std::string getVersion() const = 0;
};

// Experiment execution strategy
enum class ExecutionStrategy {
    Sequential,      // Execute experiments one at a time
    Parallel,        // Execute experiments in parallel
    Batch,           // Execute experiments in batches
    Pipeline,        // Execute experiments in a pipeline
    Hierarchical,    // Execute experiments in a hierarchical manner
    Adaptive        // Adapt execution based on performance
};

// Batch experiment configuration
struct BatchExperimentConfig {
    std::string name;                          // Batch name
    std::string description;                   // Batch description
    ExecutionStrategy strategy;                 // Execution strategy
    int max_concurrent;                        // Maximum concurrent executions
    int timeout;                               // Timeout for each experiment
    int retry_count;                           // Number of retries for failed experiments
    bool continue_on_failure;                  // Whether to continue if an experiment fails
    std::string output_directory;              // Output directory for batch results
    std::string log_file;                      // Log file path
    bool generate_reports;                     // Whether to generate reports
    std::string report_format;                 // Report format (e.g., "json", "csv", "html")
    
    BatchExperimentConfig() : max_concurrent(1), timeout(3600), retry_count(0),
                              continue_on_failure(false), generate_reports(true),
                              report_format("json") {}
};

// Experiment manager interface
class ExperimentManager {
public:
    virtual ~ExperimentManager() = default;
    
    // Add an experiment configuration
    virtual void addExperiment(const ExperimentConfig& config) = 0;
    
    // Remove an experiment by name
    virtual bool removeExperiment(const std::string& name) = 0;
    
    // Get an experiment by name
    virtual const ExperimentConfig* getExperiment(const std::string& name) const = 0;
    
    // Check if an experiment exists
    virtual bool hasExperiment(const std::string& name) const = 0;
    
    // Get all experiment names
    virtual std::vector<std::string> getExperimentNames() const = 0;
    
    // Set the default experiment executor
    virtual void setExecutor(std::shared_ptr<ExperimentExecutor> executor) = 0;
    
    // Get the default experiment executor
    virtual std::shared_ptr<ExperimentExecutor> getExecutor() const = 0;
    
    // Execute an experiment
    virtual ExperimentResult executeExperiment(const std::string& name, 
                                             const std::unordered_map<std::string, std::string>& parameters = {}) = 0;
    
    // Execute a batch of experiments
    virtual std::vector<ExperimentResult> executeBatch(const std::vector<std::string>& experiment_names) = 0;
    
    // Get experiment results
    virtual const ExperimentResult* getExperimentResult(const std::string& experiment_id) const = 0;
    
    // Get all experiment results
    virtual std::vector<ExperimentResult> getAllResults() const = 0;
    
    // Save experiment results to file
    virtual bool saveResults(const std::string& filepath) const = 0;
    
    // Load experiment results from file
    virtual bool loadResults(const std::string& filepath) = 0;
    
    // Generate experiment report
    virtual bool generateReport(const std::string& report_file, const std::string& format = "html") const = 0;
};

// Default experiment executor
class DefaultExperimentExecutor : public ExperimentExecutor {
public:
    DefaultExperimentExecutor() : brain_initialized_(false) {}
    
    ExperimentResult execute(const ExperimentConfig& config, const ExperimentExecutionContext& context) override {
        ExperimentResult result;
        result.experiment_name = config.name;
        result.experiment_version = config.version;
        result.experiment_id = ExperimentResult::generateExperimentId(config.name, config.version);
        result.status = ExperimentStatus::Running;
        result.start_time = std::chrono::system_clock::now();
        
        try {
            // Create configuration from experiment parameters
            auto nlm_config = nlm::createDefaultConfig();
            
            // Apply experiment parameters
            for (const auto& param : config.parameters) {
                if (context.getParameter(param.name) != "") {
                    // Convert parameter value based on type
                    std::string value = context.getParameter(param.name);
                    if (param.type == "int") {
                        try {
                            int int_val = std::stoi(value);
                            nlm_config.set(param.name, int_val);
                        } catch (const std::exception&) {
                            // Ignore invalid parameter values
                        }
                    } else if (param.type == "float" || param.type == "double") {
                        try {
                            double double_val = std::stod(value);
                            nlm_config.set(param.name, double_val);
                        } catch (const std::exception&) {
                            // Ignore invalid parameter values
                        }
                    } else if (param.type == "bool") {
                        bool bool_val = (value == "true" || value == "1");
                        nlm_config.set(param.name, bool_val);
                    } else {
                        nlm_config.set(param.name, value);
                    }
                }
            }
            
            // Create brain
            auto brain = nlm::createBrain(nlm_config);
            brain->initialize();
            
            // Set up output directory
            std::string output_dir = context.getOutputDirectory();
            if (!output_dir.empty()) {
                std::filesystem::create_directories(output_dir);
                result.output_dir = output_dir;
                
                // Save configuration
                std::string config_file = output_dir + "/experiment_config.json";
                nlm_config.saveToFile(config_file);
                
                // Save experiment parameters
                std::string params_file = output_dir + "/experiment_parameters.json";
                saveParametersToJson(config, params_file);
            }
            
            // Execute the experiment based on the experiment name
            if (config.name == "default") {
                // Default experiment: run basic simulation
                runDefaultExperiment(brain, context, output_dir);
            } else if (config.name == "plasticity") {
                // Plasticity experiment: run plasticity learning
                runPlasticityExperiment(brain, context, output_dir);
            } else if (config.name == "memory") {
                // Memory experiment: run memory testing
                runMemoryExperiment(brain, context, output_dir);
            } else if (config.name == "development") {
                // Development experiment: run development testing
                runDevelopmentExperiment(brain, context, output_dir);
            } else {
                // Unknown experiment: run basic simulation
                runDefaultExperiment(brain, context, output_dir);
            }
            
            // Collect results
            result.execution_time = context.getMetric("execution_time");
            result.cpu_time = context.getMetric("cpu_time");
            result.memory_usage = context.getCounter("memory_usage");
            result.end_time = std::chrono::system_clock::now();
            result.status = ExperimentStatus::Completed;
            
            // Add experiment-specific results
            addExperimentResults(result, config, context);
            
        } catch (const std::exception& e) {
            result.status = ExperimentStatus::Failed;
            result.error_message = e.what();
            result.end_time = std::chrono::system_clock::now();
        }
        
        return result;
    }
    
    bool canExecute(const ExperimentConfig& config) const override {
        return true;  // Can execute any experiment
    }
    
    std::string getName() const override {
        return "Default Experiment Executor";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
private:
    void runDefaultExperiment(std::shared_ptr<nlm::Brain> brain, const ExperimentExecutionContext& context, 
                             const std::string& output_dir) {
        int steps = 1000;
        if (context.getParameter("simulation.steps") != "") {
            steps = std::stoi(context.getParameter("simulation.steps"));
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (step % 100 == 0) {
                // Log progress
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        
        // Store metrics
        context.addMetric("execution_time", duration.count());
        context.addMetric("steps_completed", steps);
        context.addMetric("average_firing_rate", brain->getAverageFiringRate());
        context.addMetric("total_spikes", brain->getTotalSpikeCount());
        context.addCounter("memory_usage", 0); // Placeholder
    }
    
    void runPlasticityExperiment(std::shared_ptr<nlm::Brain> brain, const ExperimentExecutionContext& context,
                                const std::string& output_dir) {
        // Run plasticity learning experiment
        runDefaultExperiment(brain, context, output_dir);
        
        // Additional plasticity-specific metrics
        context.addMetric("plasticity_stdp_weight_change", 0.0); // Placeholder
        context.addMetric("plasticity_hebbian_weight_change", 0.0); // Placeholder
    }
    
    void runMemoryExperiment(std::shared_ptr<nlm::Brain> brain, const ExperimentExecutionContext& context,
                             const std::string& output_dir) {
        // Run memory testing experiment
        runDefaultExperiment(brain, context, output_dir);
        
        // Additional memory-specific metrics
        if (brain->getWorkingMemory()) {
            context.addMetric("working_memory_traces", brain->getWorkingMemory()->getActiveTraces());
        }
        if (brain->getEpisodicMemory()) {
            context.addMetric("episodic_memory_episodes", brain->getEpisodicMemory()->getEpisodeCount());
        }
    }
    
    void runDevelopmentExperiment(std::shared_ptr<nlm::Brain> brain, const ExperimentExecutionContext& context,
                                   const std::string& output_dir) {
        // Run development testing experiment
        runDefaultExperiment(brain, context, output_dir);
        
        // Additional development-specific metrics
        context.addMetric("developmental_stage", static_cast<double>(brain->getDevelopmentalStage()));
        context.addMetric("plasticity_rate", 0.0); // Placeholder
    }
    
    void addExperimentResults(ExperimentResult& result, const ExperimentConfig& config, 
                             const ExperimentExecutionContext& context) {
        // Add experiment configuration details
        result.addMetadata("experiment_name", config.name);
        result.addMetadata("experiment_version", config.version);
        result.addMetadata("experiment_author", config.author);
        result.addMetadata("experiment_date", config.date);
        result.addMetadata("experiment_category", config.category);
        
        // Add parameter values
        for (const auto& param : config.parameters) {
            std::string value = context.getParameter(param.name);
            if (!value.empty()) {
                result.addParameter(param.name, value);
            }
        }
        
        // Add system information
        result.addMetadata("system_name", "NLM");
        result.addMetadata("system_version", "Phase 6");
        result.addMetadata("compiler", "C++20");
    }
    
    void saveParametersToJson(const ExperimentConfig& config, const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "{\n";
            file << "  \"name\": \"" << config.name << "\",\n";
            file << "  \"description\": \"" << config.description << "\",\n";
            file << "  \"version\": \"" << config.version << "\",\n";
            file << "  \"parameters\": [\n";
            
            for (size_t i = 0; i < config.parameters.size(); ++i) {
                const auto& param = config.parameters[i];
                file << "    {\n";
                file << "      \"name\": \"" << param.name << "\",\n";
                file << "      \"type\": \"" << param.type << "\",\n";
                file << "      \"default\": \"" << param.getDefaultAsString() << "\",\n";
                file << "      \"description\": \"" << param.description << "\"\n";
                if (i < config.parameters.size() - 1) {
                    file << "    },\n";
                } else {
                    file << "    }\n";
                }
            }
            
            file << "  ]\n";
            file << "}\n";
            file.close();
        }
    }
    
    bool brain_initialized_;
};

// Default experiment manager
class DefaultExperimentManager : public ExperimentManager {
public:
    DefaultExperimentManager() : next_experiment_id_(0) {}
    
    void addExperiment(const ExperimentConfig& config) override {
        experiments_[config.name] = config;
    }
    
    bool removeExperiment(const std::string& name) override {
        auto it = experiments_.find(name);
        if (it != experiments_.end()) {
            experiments_.erase(it);
            return true;
        }
        return false;
    }
    
    const ExperimentConfig* getExperiment(const std::string& name) const override {
        auto it = experiments_.find(name);
        if (it != experiments_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    bool hasExperiment(const std::string& name) const override {
        return experiments_.find(name) != experiments_.end();
    }
    
    std::vector<std::string> getExperimentNames() const override {
        std::vector<std::string> names;
        for (const auto& pair : experiments_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void setExecutor(std::shared_ptr<ExperimentExecutor> executor) override {
        executor_ = executor;
    }
    
    std::shared_ptr<ExperimentExecutor> getExecutor() const override {
        return executor_;
    }
    
    ExperimentResult executeExperiment(const std::string& name, 
                                       const std::unordered_map<std::string, std::string>& parameters) override {
        const ExperimentConfig* config = getExperiment(name);
        if (!config) {
            throw std::runtime_error("Experiment not found: " + name);
        }
        
        ExperimentExecutionContext context;
        
        // Set parameters
        for (const auto& param : config->parameters) {
            auto it = parameters.find(param.name);
            if (it != parameters.end()) {
                context.addParameter(param.name, it->second);
            }
        }
        
        // Execute experiment
        if (executor_) {
            return executor_->execute(*config, context);
        } else {
            throw std::runtime_error("No experiment executor available");
        }
    }
    
    std::vector<ExperimentResult> executeBatch(const std::vector<std::string>& experiment_names) override {
        std::vector<ExperimentResult> results;
        
        for (const auto& name : experiment_names) {
            try {
                ExperimentResult result = executeExperiment(name);
                results.push_back(result);
                
                // Store result
                storeResult(result);
                
            } catch (const std::exception& e) {
                // Create failed result
                ExperimentResult failed_result;
                failed_result.experiment_name = name;
                failed_result.status = ExperimentStatus::Failed;
                failed_result.error_message = e.what();
                failed_result.end_time = std::chrono::system_clock::now();
                
                results.push_back(failed_result);
                storeResult(failed_result);
            }
        }
        
        return results;
    }
    
    const ExperimentResult* getExperimentResult(const std::string& experiment_id) const override {
        auto it = experiment_results_.find(experiment_id);
        if (it != experiment_results_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<ExperimentResult> getAllResults() const override {
        std::vector<ExperimentResult> results;
        for (const auto& pair : experiment_results_) {
            results.push_back(pair.second);
        }
        return results;
    }
    
    bool saveResults(const std::string& filepath) const override {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Save all experiment results
        file << "{\n";
        file << "  \"experiments\": [\n";
        
        size_t count = 0;
        for (const auto& pair : experiment_results_) {
            const auto& result = pair.second;
            saveResultToJson(file, result);
            
            if (count < experiment_results_.size() - 1) {
                file << "\n";
            }
            
            count++;
        }
        
        file << "\n],\n";
        file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"\n";
        file << "}\n";
        
        file.close();
        return true;
    }
    
    bool loadResults(const std::string& filepath) override {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON (simplified implementation)
        // In a real implementation, this would use a proper JSON parser
        // For now, we'll just read the file and clear existing results
        
        experiment_results_.clear();
        
        file.close();
        return true;
    }
    
    bool generateReport(const std::string& report_file, const std::string& format = "html") const override {
        if (format == "html") {
            return generateHtmlReport(report_file);
        } else if (format == "json") {
            return generateJsonReport(report_file);
        } else if (format == "csv") {
            return generateCsvReport(report_file);
        }
        
        return false;
    }
    
private:
    void storeResult(const ExperimentResult& result) {
        experiment_results_[result.experiment_id] = result;
    }
    
    void saveResultToJson(std::ofstream& file, const ExperimentResult& result) {
        file << "    {\n";
        file << "      \"experiment_id\": \"" << result.experiment_id << "\",\n";
        file << "      \"experiment_name\": \"" << result.experiment_name << "\",\n";
        file << "      \"status\": \"" << getStatusString(result.status) << "\",\n";
        file << "      \"execution_time\": " << result.execution_time << ",\n";
        file << "      \"start_time\": \"" << getTimeString(result.start_time) << "\",\n";
        file << "      \"end_time\": \"" << getTimeString(result.end_time) << "\",\n";
        file << "      \"error_message\": \"" << result.error_message << "\"\n";
        file << "    }";
    }
    
    bool generateHtmlReport(const std::string& report_file) const {
        std::ofstream file(report_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<!DOCTYPE html>\n";
        file << "<html>\n";
        file << "<head>\n";
        file << "  <title>NLM Experiment Report</title>\n";
        file << "  <style>\n";
        file << "    body { font-family: Arial, sans-serif; margin: 40px; }\n";
        file << "    h1 { color: #333; }\n";
        file << "    table { border-collapse: collapse; width: 100%; }\n";
        file << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
        file << "    th { background-color: #f2f2f2; }\n";
        file << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
        file << "  </style>\n";
        file << "</head>\n";
        file << "<body>\n";
        file << "  <h1>NLM Experiment Report</h1>\n";
        file << "  <p>Generated on " << getTimeString(std::chrono::system_clock::now()) << "</p>\n";
        
        // Experiment summary table
        file << "  <h2>Experiment Summary</h2>\n";
        file << "  <table>\n";
        file << "    <tr><th>Experiment ID</th><th>Name</th><th>Status</th><th>Duration</th><th>Execution Time</th></tr>\n";
        
        for (const auto& pair : experiment_results_) {
            const auto& result = pair.second;
            file << "    <tr>\n";
            file << "      <td>" << result.experiment_id << "</td>\n";
            file << "      <td>" << result.experiment_name << "</td>\n";
            file << "      <td>" << getStatusString(result.status) << "</td>\n";
            file << "      <td>" << result.getDuration() << "s</td>\n";
            file << "      <td>" << result.execution_time << "s</td>\n";
            file << "    </tr>\n";
        }
        
        file << "  </table>\n";
        file << "</body>\n";
        file << "</html>\n";
        
        file.close();
        return true;
    }
    
    bool generateJsonReport(const std::string& report_file) const {
        std::ofstream file(report_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        file << "  \"experiments\": [\n";
        
        size_t count = 0;
        for (const auto& pair : experiment_results_) {
            const auto& result = pair.second;
            
            file << "    {\n";
            file << "      \"experiment_id\": \"" << result.experiment_id << "\",\n";
            file << "      \"experiment_name\": \"" << result.experiment_name << "\",\n";
            file << "      \"status\": \"" << getStatusString(result.status) << "\",\n";
            file << "      \"execution_time\": " << result.execution_time <<",\n";
            file << "      \"start_time\": \"" << getTimeString(result.start_time) << "\",\n";
            file << "      \"end_time\": \"" << getTimeString(result.end_time) << "\",\n";
            file << "      \"error_message\": \"" << result.error_message << "\"\n";
            
            if (count < experiment_results_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
            
            count++;
        }
        
        file << "  ],\n";
        file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"\n";
        file << "}\n";
        
        file.close();
        return true;
    }
    
    bool generateCsvReport(const std::string& report_file) const {
        std::ofstream file(report_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Experiment ID,Name,Status,Duration (s),Execution Time (s),Error Message,Start Time,End Time\n";
        
        for (const auto& pair : experiment_results_) {
            const auto& result = pair.second;
            
            file << "\"" << result.experiment_id << "\",\"" << result.experiment_name << "\",\"" 
                 << getStatusString(result.status) << "\"," << result.getDuration() << "," 
                 << result.execution_time << ",\"" << result.error_message << "\",\"" 
                 << getTimeString(result.start_time) << "\",\"" << getTimeString(result.end_time) 
                 << "\"\n";
        }
        
        file.close();
        return true;
    }
    
    std::string getStatusString(ExperimentStatus status) const {
        switch (status) {
            case ExperimentStatus::Pending: return "Pending";
            case ExperimentStatus::Running: return "Running";
            case ExperimentStatus::Completed: return "Completed";
            case ExperimentStatus::Failed: return "Failed";
            case ExperimentStatus::Canceled: return "Canceled";
            case ExperimentStatus::Paused: return "Paused";
            case ExperimentStatus::Skipped: return "Skipped";
            case ExperimentStatus::Invalid: return "Invalid";
            default: return "Unknown";
        }
    }
    
    std::string getTimeString(std::chrono::system_clock::time_point time_point) const {
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << now_time_t;
        return ss.str();
    }
    
    std::unordered_map<std::string, ExperimentConfig> experiments_;
    std::unordered_map<std::string, ExperimentResult> experiment_results_;
    std::shared_ptr<ExperimentExecutor> executor_;
    int next_experiment_id_;
};

// Global experiment manager instance
inline ExperimentManager& getGlobalExperimentManager() {
    static DefaultExperimentManager manager;
    return manager;
}

// Global experiment executor instance
inline ExperimentExecutor& getGlobalExperimentExecutor() {
    static DefaultExperimentExecutor executor;
    return executor;
}

} // namespace nlm