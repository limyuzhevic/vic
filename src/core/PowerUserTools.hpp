// nlm_power_user.h - Advanced tools and features for power users
//
// This header provides sophisticated tools for advanced NLM users,
// including scripting capabilities, batch processing, experiment management,
// and advanced configuration tools.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <curl/curl.h>
#include <json/json.h>

namespace nlm {

class PowerUserTools {
public:
    struct ExperimentConfig {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        std::string scriptPath;
        std::vector<std::string> dependencies;
        bool enabled = true;
        std::chrono::system_clock::time_point lastRun;
        double lastDuration;
        bool lastSuccess;
    };
    
    struct BatchJob {
        std::string jobId;
        std::string scriptPath;
        std::map<std::string, std::string> parameters;
        std::string outputPath;
        std::string status;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double duration;
        bool success;
        std::string errorMessage;
    };
    
    struct ScriptContext {
        std::map<std::string, std::string> variables;
        std::map<std::string, std::function<std::string(std::vector<std::string>)>> functions;
        std::vector<std::string> imports;
        std::string workingDirectory;
        std::map<std::string, std::string> environmentVariables;
    };
    
    PowerUserTools() {
        initialize();
    }
    
    void initialize() {
        enableScripting = false;
        enableBatchProcessing = false;
        enableRemoteExecution = false;
        enableExperimentManager = false;
        verboseLogging = false;
        maxConcurrentJobs = 4;
        defaultTimeout = 3600; // 1 hour
        
        // Initialize CURL for remote execution
        curl_global_init(CURL_GLOBAL_ALL);
    }
    
    // Experiment Management
    bool createExperiment(const std::string& name, const std::string& description, 
                         const std::map<std::string, std::string>& parameters) {
        ExperimentConfig experiment;
        experiment.name = name;
        experiment.description = description;
        experiment.parameters = parameters;
        experiment.enabled = true;
        experiment.lastSuccess = false;
        experiment.lastDuration = 0.0;
        
        experiments[name] = experiment;
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Created experiment: " << name << std::endl;
        }
        
        return true;
    }
    
    bool runExperiment(const std::string& name, const std::map<std::string, std::string>& overrides = {}) {
        auto it = experiments.find(name);
        if (it == experiments.end()) {
            std::cerr << "Error: Experiment not found: " << name << std::endl;
            return false;
        }
        
        ExperimentConfig& experiment = it->second;
        
        if (!experiment.enabled) {
            std::cerr << "Error: Experiment is disabled: " << name << std::endl;
            return false;
        }
        
        // Merge overrides with experiment parameters
        std::map<std::string, std::string> config = experiment.parameters;
        for (const auto& pair : overrides) {
            config[pair.first] = pair.second;
        }
        
        // Create script context
        ScriptContext context;
        context.variables = config;
        context.workingDirectory = ".";
        
        // Set up environment variables
        for (const auto& pair : config) {
            std::string envVar = "NLM_" + pair.first;
            context.environmentVariables[envVar] = pair.second;
        }
        
        // Execute experiment script
        bool success = executeScript(experiment.scriptPath, context);
        
        if (success) {
            experiment.lastRun = std::chrono::system_clock::now();
            experiment.lastSuccess = true;
            
            if (verboseLogging) {
                std::cout << "[PowerUserTools] Experiment completed successfully: " << name << std::endl;
            }
        } else {
            experiment.lastSuccess = false;
            
            if (verboseLogging) {
                std::cout << "[PowerUserTools] Experiment failed: " << name << std::endl;
            }
        }
        
        return success;
    }
    
    bool scheduleExperiment(const std::string& name, 
                            const std::chrono::system_clock::time_point& scheduleTime,
                            const std::map<std::string, std::string>& overrides = {}) {
        ScheduledExperiment scheduled;
        scheduled.experimentName = name;
        scheduled.scheduleTime = scheduleTime;
        scheduled.overrides = overrides;
        scheduled.id = generateJobId();
        
        schedules[scheduled.id] = scheduled;
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Scheduled experiment: " << name 
                      << " (ID: " << scheduled.id << ")" << std::endl;
        }
        
        return true;
    }
    
    // Batch Processing
    std::string createBatchJob(const std::string& scriptPath, 
                               const std::map<std::string, std::string>& parameters,
                               const std::string& outputPath = "") {
        BatchJob job;
        job.jobId = generateJobId();
        job.scriptPath = scriptPath;
        job.parameters = parameters;
        job.outputPath = outputPath.empty() ? "./batch_output_" + job.jobId + ".json" : outputPath;
        job.status = "queued";
        job.success = false;
        job.duration = 0.0;
        
        batchJobs[job.jobId] = job;
        jobQueue.push(job.jobId);
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Created batch job: " << job.jobId 
                      << " (script: " << scriptPath << ")" << std::endl;
        }
        
        return job.jobId;
    }
    
    bool runBatch(const std::string& jobId) {
        auto it = batchJobs.find(jobId);
        if (it == batchJobs.end()) {
            std::cerr << "Error: Batch job not found: " << jobId << std::endl;
            return false;
        }
        
        BatchJob& job = it->second;
        
        if (job.status != "queued") {
            std::cerr << "Error: Batch job not in queued state: " << jobId << std::endl;
            return false;
        }
        
        // Update job status
        job.status = "running";
        job.startTime = std::chrono::system_clock::now();
        
        // Execute batch job
        bool success = executeBatchJob(job);
        
        // Update job status
        job.endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(job.endTime - job.startTime);
        job.duration = duration.count();
        job.success = success;
        job.status = success ? "completed" : "failed";
        
        if (!success) {
            job.errorMessage = "Batch job execution failed";
        }
        
        return success;
    }
    
    // Scripting Engine
    bool executeScript(const std::string& scriptPath, ScriptContext& context) {
        // Create script context
        ScriptContext oldContext = context;
        
        try {
            // Load script
            std::ifstream file(scriptPath);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open script file: " << scriptPath << std::endl;
                return false;
            }
            
            std::string scriptContent((std::istreambuf_iterator<char>(file)), 
                                     std::istreambuf_iterator<char>());
            
            // Simple script execution - in a real implementation, this would use
            // a full scripting engine like Lua, Python, or JavaScript
            bool success = executeSimpleScript(scriptContent, context);
            
            return success;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: Script execution failed: " << e.what() << std::endl;
            context = oldContext; // Restore context on error
            return false;
        }
    }
    
    bool executeSimpleScript(const std::string& script, ScriptContext& context) {
        // This is a very simplified script interpreter for demonstration
        // In a real implementation, this would be replaced with a full scripting engine
        
        std::istringstream stream(script);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            if (line.empty() || line.find("//") == 0 || line.find("#") == 0) {
                continue;
            }
            
            // Parse simple commands
            if (line.find("set ") == 0) {
                // Set variable: set var_name value
                size_t space1 = line.find(' ', 4);
                if (space1 != std::string::npos) {
                    std::string varName = line.substr(4, space1 - 4);
                    std::string varValue = line.substr(space1 + 1);
                    context.variables[varName] = varValue;
                }
            } else if (line.find("print ") == 0) {
                // Print variable: print var_name
                std::string varName = line.substr(6);
                auto it = context.variables.find(varName);
                if (it != context.variables.end()) {
                    std::cout << it->second << std::endl;
                }
            } else if (line.find("function ") == 0) {
                // Define function: function func_name { ... }
                size_t bracePos = line.find('{');
                if (bracePos != std::string::npos) {
                    std::string funcName = line.substr(9, bracePos - 10);
                    std::string funcBody = line.substr(bracePos + 1);
                    // For simplicity, we'll just store the function definition
                    context.functions[funcName] = [funcBody](std::vector<std::string> args) {
                        return "Function: " + funcBody;
                    };
                }
            } else if (line.find("call ") == 0) {
                // Call function: call func_name arg1 arg2
                size_t space1 = line.find(' ', 5);
                if (space1 != std::string::npos) {
                    std::string funcName = line.substr(5, space1 - 5);
                    std::string argsPart = line.substr(space1 + 1);
                    
                    // Split arguments
                    std::vector<std::string> args;
                    std::istringstream argStream(argsPart);
                    std::string arg;
                    while (argStream >> arg) {
                        args.push_back(arg);
                    }
                    
                    auto it = context.functions.find(funcName);
                    if (it != context.functions.end()) {
                        std::string result = it->second(args);
                        std::cout << result << std::endl;
                    }
                }
            }
        }
        
        return true;
    }
    
    // Remote Execution
    bool executeRemote(const std::string& command, const std::map<std::string, std::string>& envVars = {}) {
        if (!enableRemoteExecution) {
            std::cerr << "Error: Remote execution is not enabled" << std::endl;
            return false;
        }
        
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Error: Could not initialize CURL" << std::endl;
            return false;
        }
        
        // Set up CURL options
        curl_easy_setopt(curl, CURLOPT_URL, command.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        
        // Set environment variables
        for (const auto& pair : envVars) {
            std::string envVar = pair.first + "=" + pair.second;
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, envVar.c_str());
        }
        
        // Execute command
        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "Error: CURL execution failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
        
        curl_easy_cleanup(curl);
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Remote execution completed: " << command << std::endl;
        }
        
        return true;
    }
    
    // Export and Import
    bool exportExperiments(const std::string& filename) {
        Json::Value root;
        
        for (const auto& pair : experiments) {
            const ExperimentConfig& experiment = pair.second;
            Json::Value experimentJson;
            
            experimentJson["name"] = experiment.name;
            experimentJson["description"] = experiment.description;
            experimentJson["enabled"] = experiment.enabled;
            
            Json::Value paramsJson;
            for (const auto& paramPair : experiment.parameters) {
                paramsJson[paramPair.first] = paramPair.second;
            }
            experimentJson["parameters"] = paramsJson;
            
            root["experiments"][pair.first] = experimentJson;
        }
        
        // Write JSON file
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }
        
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(root, &file);
        
        file.close();
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Exported experiments to: " << filename << std::endl;
        }
        
        return true;
    }
    
    bool importExperiments(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
            return false;
        }
        
        Json::Value root;
        std::ifstream importFile(filename);
        Json::CharReaderBuilder readerBuilder;
        std::string errors;
        
        if (!Json::parseFromStream(readerBuilder, importFile, &root, &errors)) {
            std::cerr << "Error: Failed to parse JSON file: " << errors << std::endl;
            return false;
        }
        
        // Import experiments
        for (const auto& experimentPair : root["experiments"].getObject()) {
            ExperimentConfig experiment;
            
            experiment.name = experimentPair.first;
            experiment.description = experimentPair.second["description"].asString();
            experiment.enabled = experimentPair.second["enabled"].asBool();
            
            const Json::Value& paramsJson = experimentPair.second["parameters"];
            for (const auto& paramPair : paramsJson.getObject()) {
                experiment.parameters[paramPair.first] = paramPair.second.asString();
            }
            
            experiments[experiment.name] = experiment;
        }
        
        file.close();
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Imported experiments from: " << filename << std::endl;
        }
        
        return true;
    }
    
    // Configuration Management
    bool createConfigTemplate(const std::string& filename, 
                              const std::map<std::string, std::string>& defaults) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << "# NLM Configuration Template" << std::endl;
        file << "# Generated by NLM Power User Tools" << std::endl;
        file << std::endl;
        
        for (const auto& pair : defaults) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        file.close();
        
        if (verboseLogging) {
            std::cout << "[PowerUserTools] Created config template: " << filename << std::endl;
        }
        
        return true;
    }
    
    void setVerboseLogging(bool enable) {
        verboseLogging = enable;
    }
    
    void setMaxConcurrentJobs(size_t maxJobs) {
        maxConcurrentJobs = maxJobs;
    }
    
    void setDefaultTimeout(double timeoutSeconds) {
        defaultTimeout = timeoutSeconds;
    }
    
    void enableScripting(bool enable) {
        enableScripting = enable;
    }
    
    void enableBatchProcessing(bool enable) {
        enableBatchProcessing = enable;
    }
    
    void enableRemoteExecution(bool enable) {
        enableRemoteExecution = enable;
    }
    
    void enableExperimentManager(bool enable) {
        enableExperimentManager = enable;
    }
    
    // Status and Monitoring
    void printStatus() const {
        std::cout << "=== NLM Power User Tools Status ===" << std::endl;
        std::cout << "Scripting: " << (enableScripting ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Batch Processing: " << (enableBatchProcessing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Remote Execution: " << (enableRemoteExecution ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Experiment Manager: " << (enableExperimentManager ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Max Concurrent Jobs: " << maxConcurrentJobs << std::endl;
        std::cout << "Default Timeout: " << defaultTimeout << " seconds" << std::endl;
        std::cout << "Verbose Logging: " << (verboseLogging ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Experiments: " << experiments.size() << std::endl;
        for (const auto& pair : experiments) {
            const ExperimentConfig& experiment = pair.second;
            std::cout << "  - " << experiment.name 
                      << " (enabled: " << (experiment.enabled ? "yes" : "no") 
                      << ", last success: " << (experiment.lastSuccess ? "yes" : "no") << ")" << std::endl;
        }
        
        std::cout << "Batch Jobs: " << batchJobs.size() << std::endl;
        std::cout << "Scheduled Jobs: " << schedules.size() << std::endl;
    }
    
    size_t getExperimentCount() const {
        return experiments.size();
    }
    
    size_t getBatchJobCount() const {
        return batchJobs.size();
    }
    
    bool isJobRunning(const std::string& jobId) const {
        auto it = batchJobs.find(jobId);
        return it != batchJobs.end() && it->second.status == "running";
    }
    
    static PowerUserTools& getInstance() {
        static PowerUserTools instance;
        return instance;
    }
    
private:
    struct ScheduledExperiment {
        std::string id;
        std::string experimentName;
        std::chrono::system_clock::time_point scheduleTime;
        std::map<std::string, std::string> overrides;
    };
    
    std::string generateJobId() {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "job_" << now_time_t << "_" 
           << std::chrono::duration_cast<std::chrono::microseconds>(
               now.time_since_epoch()).count() % 1000000;
        
        return ss.str();
    }
    
    bool executeBatchJob(BatchJob& job) {
        // Create script context from job parameters
        ScriptContext context;
        context.variables = job.parameters;
        context.workingDirectory = ".";
        
        // Execute script
        bool success = executeScript(job.scriptPath, context);
        
        return success;
    }
    
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t totalSize = size * nmemb;
        responseData.append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
    
    std::map<std::string, ExperimentConfig> experiments;
    std::map<std::string, BatchJob> batchJobs;
    std::map<std::string, ScheduledExperiment> schedules;
    std::queue<std::string> jobQueue;
    
    bool enableScripting;
    bool enableBatchProcessing;
    bool enableRemoteExecution;
    bool enableExperimentManager;
    bool verboseLogging;
    size_t maxConcurrentJobs;
    double defaultTimeout;
    
    static std::string responseData;
};

static std::string PowerUserTools::responseData;