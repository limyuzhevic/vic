// Error Handling Integration Examples for NLM
// Integration examples showing standardized error handling in Brain, AgentBrain, and other components

#ifndef NLM_ERROR_INTEGRATION_HPP
#define NLM_ERROR_INTEGRATION_HPP

#include "Errors.hpp"
#include "Logging.hpp"
#include "ErrorHandling.hpp"
#include "ErrorContext.hpp"
#include <memory>
#include <vector>
#include <string>

// Integration Example 1: Brain Class Error Handling
class BrainIntegrationExample {
public:
    // Neural simulation error handling with comprehensive error context
    static void simulateNeuron(int neuronId, double input, double threshold) {
        NLM_CONTEXT_PUSH("neuron_simulation", "brain_neuron");
        NLM_CONTEXT_SET_VALUE("neuron_id", neuronId);
        NLM_CONTEXT_SET_VALUE("input", input);
        NLM_CONTEXT_SET_VALUE("threshold", threshold);
        
        try {
            NLM_VALIDATE(input >= 0.0, ValidationError, 
                        "Input must be non-negative", "neuron_simulation");
            
            NLM_VALIDATE(threshold > 0.0, ValidationError,
                        "Threshold must be positive", "neuron_simulation");
            
            NLM_VALIDATE(neuronId >= 0, ValidationError,
                        "Neuron ID must be non-negative", "neuron_simulation");
            
            // Simulate neural computation
            double output = input * 0.5; // Simplified neural computation
            
            if (output >= threshold) {
                NLM_LOG_ERROR(LogCategory::NEURAL,
                    "Neuron " + std::to_string(neuronId) + 
                    " fired: output " + std::to_string(output) +
                    " exceeded threshold " + std::to_string(threshold));
            }
            
            NLM_CONTEXT_SET_VALUE("output", output);
            NLM_CONTEXT_SET_VALUE("fired", output >= threshold);
            
            NLM_LOG_INFO(LogCategory::NEURAL,
                "Neuron " + std::to_string(neuronId) + " simulation completed successfully");
                
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(1001, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(1002, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (const MemoryError& e) {
            NLM_CONTEXT_SET_ERROR(1003, "MEMORY");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(9999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during neuron simulation");
            throw NLMException("Unknown error during neuron simulation", 
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Batch neuron processing with error recovery
    static std::vector<double> processNeuronsBatch(
        const std::vector<std::tuple<int, double, double>>& neurons) {
        
        std::vector<double> outputs;
        outputs.reserve(neurons.size());
        
        NLM_CONTEXT_PUSH("batch_neuron_processing", "brain_batch");
        NLM_CONTEXT_SET_VALUE("batch_size", static_cast<int>(neurons.size()));
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            auto [neuronId, input, threshold] = neurons[i];
            
            try {
                simulateNeuron(neuronId, input, threshold);
                outputs.push_back(input * 0.5); // Simulated output
                
                NLM_CONTEXT_SET_VALUE("processed_neuron", neuronId);
                NLM_LOG_INFO(LogCategory::NEURAL,
                    "Processed neuron " + std::to_string(neuronId) + 
                    " in batch (" + std::to_string(i + 1) + "/" + 
                    std::to_string(neurons.size()) + ")");
                
            } catch (const ValidationError& e) {
                // Graceful degradation: skip invalid neuron
                NLM_LOG_WARNING(LogCategory::VALIDATION,
                    "Skipping invalid neuron " + std::to_string(neuronId) + 
                    ": " + e.getMessage());
                
                // Add default output for skipped neuron
                outputs.push_back(0.0);
                NLM_CONTEXT_SET_VALUE("skipped_neuron", neuronId);
                
            } catch (const NeuronError& e) {
                // Critical error: abort batch processing
                NLM_LOG_ERROR(LogCategory::NEURAL,
                    "Critical error in neuron " + std::to_string(neuronId) + 
                    ": " + e.getMessage());
                throw; // Re-throw critical error
            }
        }
        
        NLM_CONTEXT_SET_VALUE("successful_operations", static_cast<int>(outputs.size() - 
            std::count(outputs.begin(), outputs.end(), 0.0)));
        
        return outputs;
    }
};

// Integration Example 2: AgentBrain Class Error Handling
class AgentBrainIntegrationExample {
public:
    // Sensor processing error handling with state management
    static std::vector<double> processSensors(
        const std::vector<double>& sensorData,
        const std::string& agentId) {
        
        NLM_CONTEXT_PUSH("sensor_processing", "agent_brain");
        NLM_CONTEXT_SET_VALUE("agent_id", agentId);
        NLM_CONTEXT_SET_VALUE("sensor_count", static_cast<int>(sensorData.size()));
        
        try {
            NLM_VALIDATE(!sensorData.empty(), ValidationError,
                        "Sensor data cannot be empty", "sensor_processing");
            
            std::vector<double> processedData;
            processedData.reserve(sensorData.size());
            
            for (size_t i = 0; i < sensorData.size(); ++i) {
                double value = sensorData[i];
                
                // Validate sensor reading
                NLM_VALIDATE(std::isfinite(value), ValidationError,
                    "Sensor reading must be finite", "sensor_processing");
                
                // Process sensor data with noise
                double noise = 0.01 * (rand() % 100 - 50) / 50.0; // Random noise
                double processed = value + noise;
                
                // Apply sensor-specific validation
                if (i == 0) { // Temperature sensor
                    NLM_VALIDATE(processed >= -50.0 && processed <= 150.0, 
                                ValidationError, "Temperature out of range", "sensor_processing");
                } else if (i == 1) { // Pressure sensor
                    NLM_VALIDATE(processed >= 0.0 && processed <= 10.0, 
                                ValidationError, "Pressure out of range", "sensor_processing");
                }
                
                processedData.push_back(processed);
                NLM_CONTEXT_SET_VALUE("sensor_" + std::to_string(i), processed);
                
                NLM_LOG_DEBUG(LogCategory::NEURAL,
                    "Processed sensor " + std::to_string(i) + " value: " + 
                    std::to_string(processed));
            }
            
            NLM_CONTEXT_SET_VALUE("processing_success", true);
            NLM_LOG_INFO(LogCategory::NEURAL,
                "Successfully processed " + std::to_string(sensorData.size()) + 
                " sensors for agent " + agentId);
                
            return processedData;
            
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(2001, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(2002, "WORLD");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(2999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during sensor processing");
            throw NLMException("Unknown error during sensor processing",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Agent decision making with error recovery
    static std::string makeDecision(
        const std::vector<double>& sensorReadings,
        const std::vector<std::string>& possibleActions) {
        
        NLM_CONTEXT_PUSH("decision_making", "agent_brain");
        NLM_CONTEXT_SET_VALUE("sensor_count", static_cast<int>(sensorReadings.size()));
        NLM_CONTEXT_SET_VALUE("action_options", possibleActions.size());
        
        try {
            // Validate inputs
            NLM_VALIDATE(!possibleActions.empty(), ValidationError,
                        "No action options available", "decision_making");
            
            NLM_VALIDATE(sensorReadings.size() == possibleActions.size(),
                        ValidationError, "Sensor readings and actions must match", "decision_making");
            
            // Calculate action scores (simplified decision logic)
            std::vector<double> scores;
            scores.reserve(possibleActions.size());
            
            for (size_t i = 0; i < sensorReadings.size(); ++i) {
                double score = sensorReadings[i] * 0.3 + rand() % 100 / 300.0; // Add randomness
                scores.push_back(score);
                NLM_CONTEXT_SET_VALUE("action_score_" + possibleActions[i], score);
            }
            
            // Find best action
            double maxScore = -std::numeric_limits<double>::max();
            int bestActionIndex = -1;
            
            for (size_t i = 0; i < scores.size(); ++i) {
                if (scores[i] > maxScore) {
                    maxScore = scores[i];
                    bestActionIndex = i;
                }
            }
            
            std::string bestAction = possibleActions[bestActionIndex];
            
            NLM_CONTEXT_SET_VALUE("selected_action", bestAction);
            NLM_CONTEXT_SET_VALUE("confidence", maxScore);
            NLM_CONTEXT_SET_VALUE("decision_success", true);
            
            NLM_LOG_INFO(LogCategory::NEURAL,
                "Agent selected action: " + bestAction + " with confidence: " + 
                std::to_string(maxScore));
                
            return bestAction;
            
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(2003, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(2004, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(2999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during decision making");
            throw NLMException("Unknown error during decision making",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
};

// Integration Example 3: Configuration System Error Handling
class ConfigurationIntegrationExample {
public:
    // Configuration validation with detailed error reporting
    static void validateConfiguration(const std::string& configFile) {
        NLM_CONTEXT_PUSH("configuration_validation", "core_config");
        NLM_CONTEXT_SET_VALUE("config_file", configFile);
        
        try {
            // Check if file exists
            std::ifstream configStream(configFile);
            NLM_CHECK_FILE(configStream, FileSystemError,
                         "Cannot open configuration file", "configuration_validation");
            
            // Parse configuration (simplified)
            std::string line;
            int lineNumber = 0;
            bool hasNeuronCount = false;
            bool hasLearningRate = false;
            
            while (std::getline(configStream, line)) {
                lineNumber++;
                NLM_CONTEXT_SET_VALUE("current_line", lineNumber);
                
                if (line.find("neuronCount") != std::string::npos) {
                    hasNeuronCount = true;
                    NLM_CONTEXT_SET_VALUE("found_setting", "neuronCount");
                } else if (line.find("learningRate") != std::string::npos) {
                    hasLearningRate = true;
                    NLM_CONTEXT_SET_VALUE("found_setting", "learningRate");
                }
            }
            
            // Validate required settings
            NLM_VALIDATE(hasNeuronCount, ConfigurationError,
                        "Configuration file must contain neuronCount setting", "configuration_validation");
            
            NLM_VALIDATE(hasLearningRate, ConfigurationError,
                        "Configuration file must contain learningRate setting", "configuration_validation");
            
            NLM_LOG_INFO(LogCategory::CONFIGURATION,
                "Configuration validation completed successfully for file: " + configFile);
                
        } catch (const FileSystemError& e) {
            NLM_CONTEXT_SET_ERROR(3001, "FILESYSTEM");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ConfigurationError& e) {
            NLM_CONTEXT_SET_ERROR(3002, "CONFIGURATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(3003, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(3999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during configuration validation");
            throw NLMException("Unknown error during configuration validation",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Safe configuration loading with recovery
    static std::map<std::string, std::string> loadConfiguration(const std::string& configFile) {
        std::map<std::string, std::string> config;
        
        NLM_CONTEXT_PUSH("configuration_loading", "core_config");
        NLM_CONTEXT_SET_VALUE("config_file", configFile);
        
        try {
            // Try to load configuration
            config = loadConfigurationInternal(configFile);
            
            NLM_CONTEXT_SET_VALUE("loaded_settings", static_cast<int>(config.size()));
            NLM_LOG_INFO(LogCategory::CONFIGURATION,
                "Configuration loaded successfully: " + std::to_string(config.size()) + 
                " settings from " + configFile);
                
            return config;
            
        } catch (const FileSystemError& e) {
            NLM_CONTEXT_SET_ERROR(3101, "FILESYSTEM");
            // Graceful degradation: return empty config
            NLM_LOG_WARNING(LogCategory::CONFIGURATION,
                "Failed to load configuration, using defaults: " + e.getMessage());
            
            NLM_CONTEXT_SET_VALUE("fallback_used", true);
            return getDefaultConfiguration();
            
        } catch (const ConfigurationError& e) {
            NLM_CONTEXT_SET_ERROR(3102, "CONFIGURATION");
            // Partial failure: log error and continue with available settings
            NLM_LOG_ERROR(LogCategory::CONFIGURATION,
                "Partial configuration error, continuing with available settings: " + e.getMessage());
            
            NLM_CONTEXT_SET_VALUE("partial_failure", true);
            return loadConfigurationInternal(configFile, true); // Continue with available settings
            
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(3103, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        }
    }
    
private:
    static std::map<std::string, std::string> loadConfigurationInternal(
        const std::string& configFile, bool continueOnError = false) {
        std::map<std::string, std::string> config;
        std::ifstream configStream(configFile);
        
        if (!configStream.is_open()) {
            throw FileSystemError("Cannot open configuration file: " + configFile);
        }
        
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(configStream, line)) {
            lineNumber++;
            
            // Skip comments and empty lines
            if (line.empty() || line.find("#") == 0) {
                continue;
            }
            
            size_t delimiter = line.find("=");
            if (delimiter != std::string::npos) {
                std::string key = line.substr(0, delimiter);
                std::string value = line.substr(delimiter + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                if (!key.empty()) {
                    config[key] = value;
                }
            }
        }
        
        return config;
    }
    
    static std::map<std::string, std::string> getDefaultConfiguration() {
        std::map<std::string, std::string> defaults;
        defaults["neuronCount"] = "1000";
        defaults["learningRate"] = "0.01";
        defaults["maxMemory"] = "1024";
        defaults["logLevel"] = "INFO";
        return defaults;
    }
};

// Integration Example 4: File I/O Operations with Comprehensive Error Handling
class FileIOIntegrationExample {
public:
    // Safe file reading with error recovery
    static std::string readFileSafely(const std::string& filePath) {
        NLM_CONTEXT_PUSH("file_reading", "filesystem_io");
        NLM_CONTEXT_SET_VALUE("file_path", filePath);
        
        try {
            // Check if file exists and is readable
            std::ifstream file(filePath);
            NLM_CHECK_FILE(file, FileSystemError,
                         "Cannot read file: " + filePath, "file_reading");
            
            // Get file size
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            NLM_CONTEXT_SET_VALUE("file_size", static_cast<int>(fileSize));
            
            // Validate file size
            NLM_VALIDATE(fileSize < 100 * 1024 * 1024, FileSystemError, // 100MB limit
                        "File too large: " + filePath, "file_reading");
            
            // Read file content
            std::string content(fileSize, '\0');
            if (!file.read(&content[0], fileSize).good()) {
                throw FileSystemError("Failed to read file: " + filePath);
            }
            
            NLM_LOG_INFO(LogCategory::FILESYSTEM,
                "Successfully read file: " + filePath + " (" + std::to_string(fileSize) + " bytes)");
                
            return content;
            
        } catch (const FileSystemError& e) {
            NLM_CONTEXT_SET_ERROR(4001, "FILESYSTEM");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(4002, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(4999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during file reading");
            throw NLMException("Unknown error during file reading",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Safe file writing with backup and recovery
    static bool writeFileWithBackup(const std::string& filePath, const std::string& content) {
        NLM_CONTEXT_PUSH("file_writing", "filesystem_io");
        NLM_CONTEXT_SET_VALUE("file_path", filePath);
        NLM_CONTEXT_SET_VALUE("content_size", static_cast<int>(content.size()));
        
        try {
            // Create backup if file exists
            std::string backupPath = filePath + ".backup";
            if (std::filesystem::exists(filePath)) {
                NLM_VALIDATE(std::filesystem::copy_file(filePath, backupPath,
                                         std::filesystem::copy_options::overwrite_existing),
                           FileSystemError, "Failed to create backup", "file_writing");
                NLM_CONTEXT_SET_VALUE("backup_created", true);
                NLM_LOG_INFO(LogCategory::FILESYSTEM,
                    "Backup created: " + backupPath);
            }
            
            // Write new file
            std::ofstream file(filePath);
            NLM_CHECK_FILE(file, FileSystemError,
                         "Cannot write file: " + filePath, "file_writing");
            
            if (!file.write(content.c_str(), content.size()).good()) {
                throw FileSystemError("Failed to write file: " + filePath);
            }
            
            file.flush();
            NLM_VALIDATE(file.good(), FileSystemError,
                        "File write error: " + filePath, "file_writing");
            
            NLM_LOG_INFO(LogCategory::FILESYSTEM,
                "Successfully wrote file: " + filePath + " (" + std::to_string(content.size()) + " bytes)");
                
            return true;
            
        } catch (const FileSystemError& e) {
            NLM_CONTEXT_SET_ERROR(4101, "FILESYSTEM");
            
            // Attempt to restore from backup if backup exists
            if (std::filesystem::exists(backupPath)) {
                try {
                    NLM_LOG_WARNING(LogCategory::FILESYSTEM,
                        "Attempting to restore from backup after write failure");
                    std::filesystem::copy_file(backupPath, filePath,
                                             std::filesystem::copy_options::overwrite_existing);
                    NLM_LOG_INFO(LogCategory::FILESYSTEM,
                        "File restored from backup: " + backupPath);
                    NLM_CONTEXT_SET_VALUE("backup_restored", true);
                    return false; // Indicate recovery was successful
                } catch (...) {
                    NLM_LOG_ERROR(LogCategory::FILESYSTEM,
                        "Failed to restore from backup: " + backupPath);
                }
            }
            
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(4102, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(4999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during file writing");
            throw NLMException("Unknown error during file writing",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
};

// Integration Example 5: Performance Monitoring and Error Handling
class PerformanceIntegrationExample {
public:
    // Performance-aware operations with error handling
    template<typename Func, typename... Args>
    static auto executeWithPerformanceMonitoring(
        const std::string& operationName,
        const std::chrono::milliseconds& timeout,
        Func&& func, Args&&... args) {
        
        NLM_CONTEXT_PUSH("performance_monitoring", "core_performance");
        NLM_CONTEXT_SET_VALUE("operation_name", operationName);
        NLM_CONTEXT_SET_VALUE("timeout_ms", timeout.count());
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Execute with timeout protection
            auto future = std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...);
            
            // Wait for result with timeout
            auto status = future.wait_for(timeout);
            
            if (status == std::future_status::timeout) {
                NLM_CONTEXT_SET_ERROR(5001, "PERFORMANCE");
                NLM_LOG_ERROR(LogCategory::PERFORMANCE,
                    "Operation timed out: " + operationName + " after " + 
                    std::to_string(timeout.count()) + "ms");
                
                // Create performance error
                PerformanceError perfError("Operation timed out: " + operationName,
                                          NLM_CURRENT_LOCATION, operationName);
                perfError.getContext().executionTime = timeout.count();
                throw perfError;
            }
            
            auto result = future.get();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            NLM_CONTEXT_SET_EXECUTION_TIME(duration.count());
            
            // Check performance thresholds
            if (duration.count() > timeout.count() * 2) { // Warning if > 2x timeout
                NLM_LOG_WARNING(LogCategory::PERFORMANCE,
                    "Performance warning: " + operationName + " took " + 
                    std::to_string(duration.count()) + "ms (timeout: " + 
                    std::to_string(timeout.count()) + "ms)");
            }
            
            NLM_LOG_INFO(LogCategory::PERFORMANCE,
                "Operation completed: " + operationName + " in " + 
                std::to_string(duration.count()) + "ms");
                
            return result;
            
        } catch (const PerformanceError& e) {
            NLM_CONTEXT_SET_ERROR(5002, "PERFORMANCE");
            NLM_LOG_ERROR_THROW(e);
        } catch (const MemoryError& e) {
            NLM_CONTEXT_SET_ERROR(5003, "MEMORY");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(5999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during performance-monitored operation");
            throw NLMException("Unknown error during performance-monitored operation",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Memory-aware operation with error handling
    template<typename Container>
    static void executeWithMemoryMonitoring(
        const std::string& operationName,
        const Container& data) {
        
        NLM_CONTEXT_PUSH("memory_monitoring", "core_memory");
        NLM_CONTEXT_SET_VALUE("operation_name", operationName);
        NLM_CONTEXT_SET_VALUE("data_size", static_cast<int>(data.size()));
        
        try {
            // Check memory availability
            size_t requiredMemory = data.size() * sizeof(typename Container::value_type);
            size_t availableMemory = getAvailableMemory();
            
            NLM_VALIDATE(requiredMemory <= availableMemory * 0.8, // Use 80% of available memory
                        MemoryError, "Insufficient memory for operation: " + operationName, "memory_monitoring");
            
            NLM_CONTEXT_SET_VALUE("required_memory_mb", requiredMemory / (1024 * 1024));
            NLM_CONTEXT_SET_VALUE("available_memory_mb", availableMemory / (1024 * 1024));
            
            // Execute operation
            executeOperation(data);
            
            NLM_LOG_INFO(LogCategory::MEMORY,
                "Memory operation completed: " + operationName + " with " + 
                std::to_string(data.size()) + " items");
                
        } catch (const MemoryError& e) {
            NLM_CONTEXT_SET_ERROR(6001, "MEMORY");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(6002, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(6999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during memory-monitored operation");
            throw NLMException("Unknown error during memory-monitored operation",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
private:
    static void executeOperation(const auto& data) {
        // Simplified operation implementation
        volatile double sum = 0.0;
        for (const auto& item : data) {
            sum += item;
        }
        (void)sum; // Avoid unused variable warning
    }
    
    static size_t getAvailableMemory() {
        // Simplified memory availability check
        return 1024 * 1024 * 1024; // 1GB (simplified)
    }
};

#endif // NLM_ERROR_INTEGRATION_HPP