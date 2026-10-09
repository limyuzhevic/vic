#include "nlm/batch/NLMBatch.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

namespace NLMBatch {

    // Batch file parser implementation
    std::vector<BatchResult> BatchFileParser::parse(const std::string& filename) {
        std::vector<BatchResult> experiments;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open batch file: " + filename);
        }
        
        std::string line;
        size_t lineNumber = 0;
        size_t experimentCount = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines
            if (line.empty()) {
                continue;
            }
            
            // Skip comments
            if (line[0] == '#') {
                continue;
            }
            
            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) {
                continue;
            }
            
            try {
                std::vector<std::string> tokens = parseLine(line);
                if (tokens.empty()) {
                    continue;
                }
                
                BatchResult result;
                result.experimentName = tokens[0];
                result.success = false;
                result.errorMessage = "Not yet initialized";
                
                // Parse parameters
                std::map<std::string, std::string> params = parseParameters(tokens);
                
                experiments.push_back(result);
                experimentCount++;
                
            } catch (const std::exception& e) {
                std::cerr << "Error parsing line " << lineNumber << " in batch file: " << e.what() << std::endl;
                if (!"continueOnError") { // Would need config access
                    throw;
                }
            }
        }
        
        return experiments;
    }
    
    bool BatchFileParser::validateFile(const std::string& filename, std::vector<std::string>& errors) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            errors.push_back("Cannot open batch file: " + filename);
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Validate line format
            if (line.find(',') == std::string::npos) {
                errors.push_back(std::string("Line ") + std::to_string(lineNumber) + 
                               ": Invalid format. Expected 'experiment_name,param=value'");
            }
            
            // Check line length
            if (line.length() > 1024) {
                errors.push_back(std::string("Line ") + std::to_string(lineNumber) + 
                               ": Line too long (max 1024 characters)");
            }
        }
        
        return errors.empty();
    }
    
    std::vector<std::string> BatchFileParser::parseLine(const std::string& line) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            // Remove whitespace from token
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }
    
    std::map<std::string, std::string> BatchFileParser::parseParameters(const std::vector<std::string>& tokens) {
        std::map<std::string, std::string> params;
        
        for (size_t i = 1; i < tokens.size(); ++i) {
            std::string token = tokens[i];
            size_t pos = token.find('=');
            
            if (pos != std::string::npos) {
                std::string key = token.substr(0, pos);
                std::string value = token.substr(pos + 1);
                
                // Remove whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                params[key] = value;
            }
        }
        
        return params;
    }

    // Batch experiment runner implementation
    std::vector<BatchResult> BatchExperimentRunner::run(const std::vector<BatchResult>& experiments) {
        std::vector<BatchResult> results;
        totalExperiments = experiments.size();
        completedExperiments = 0;
        
        if (config.numThreads <= 1 || experiments.size() <= 1) {
            // Sequential execution
            for (const auto& experiment : experiments) {
                BatchResult result = runSingleExperiment(experiment);
                results.push_back(result);
                updateProgress(result);
                
                if (!shouldContinue(result)) {
                    break;
                }
            }
        } else {
            // Parallel execution
            std::vector<BatchResult> workingCopy = experiments;
            results = Parallel::executeParallel(workingCopy, [this](BatchResult& exp) {
                return runSingleExperiment(exp);
            }, config.numThreads);
            
            for (const auto& result : results) {
                updateProgress(result);
            }
        }
        
        if (config.generateReport) {
            generateReport(results);
        }
        
        return results;
    }
    
    BatchResult BatchExperimentRunner::runSingleExperiment(const BatchResult& experiment) {
        BatchResult result = experiment;
        auto startTime = std::chrono::high_resolution_clock::now();
        result.startTime = startTime;
        
        try {
            // Create experiment
            Phase6IntegratedExperiment nlmExperiment;
            
            // Create configuration from experiment parameters
            auto config = std::make_shared<Config>();
            
            // Set default values
            config->set("neuron_count", 1000, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.1, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.01, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.012, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
            config->set("pruning_rate", 0.00001, ConfigSource::Default);
            config->set("enable_development", true, ConfigSource::Default);
            
            // Apply experiment-specific parameters (would need mapping from string keys to actual config keys)
            
            Phase6Config expConfig;
            expConfig.neuronCount = config->getOr<size_t>("neuron_count", 1000);
            expConfig.maxSteps = 2000; // Default
            expConfig.enableCheckpointing = true;
            expConfig.enableReplay = true;
            expConfig.enableDevelopment = true;
            
            // Run experiment
            result.result = nlmExperiment.run(expConfig);
            
            // Update result
            result.success = true;
            result.errorMessage = "";
            result.rewardAchieved = result.result.totalReward;
            result.neuronsUsed = expConfig.neuronCount;
            result.stepsExecuted = expConfig.maxSteps;
            
            // Save checkpoint if requested
            if (config->getOr<bool>("enable_checkpointing", true) && 
                !result.result.checkpointingWorks) {
                std::string checkpointPath = config->getOr<std::string>("checkpoint_dir", "./checkpoints") + 
                                            "/" + config->getOr<std::string>("checkpoint_prefix", "batch_") + 
                                            + result.experimentName + ".bin";
                nlmExperiment.save(checkpointPath);
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = std::string("Exception: ") + e.what();
            std::cerr << "Error in experiment " << result.experimentName << ": " << e.what() << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.endTime = endTime;
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        totalReward += result.rewardAchieved;
        totalExecutionTime += result.executionTime;
        
        std::lock_guard<std::mutex> lock(mutex);
        completedExperiments++;
        
        return result;
    }
    
    void BatchExperimentRunner::updateProgress(const BatchResult& result) {
        if (!config.showProgress) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        double progress = static_cast<double>(completedExperiments) / totalExperiments * 100.0;
        
        std::cout << "\rProgress: " << std::fixed << std::setprecision(1) 
                  << progress << "% (" << completedExperiments << "/" << totalExperiments << ")";
        std::cout.flush();
        
        if (completedExperiments % 10 == 0 || completedExperiments == totalExperiments) {
            std::cout << std::endl;
            if (result.success) {
                std::cout << "  " << result.experimentName << ": SUCCESS (Reward: " 
                          << std::fixed << std::setprecision(4) << result.rewardAchieved 
                          << ", Time: " << formatDuration(result.executionTime) << ")" << std::endl;
            } else {
                std::cout << "  " << result.experimentName << ": FAILED - " << result.errorMessage << std::endl;
            }
        }
    }
    
    void BatchExperimentRunner::saveResult(const BatchResult& result) {
        if (config.outputFormat == "console" || config.outputFile.empty()) {
            return;
        }
        
        std::ofstream file(config.outputFile, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Cannot open output file: " << config.outputFile << std::endl;
            return;
        }
        
        if (config.outputFormat == "csv") {
            file << result.experimentName << ","
                 << (result.success ? "SUCCESS" : "FAILED") << ","
                 << result.executionTime << ","
                 << result.rewardAchieved << ","
                 << result.errorMessage << "\n";
        } else {
            file << "Experiment: " << result.experimentName << "\n"
                 << "Success: " << (result.success ? "Yes" : "No") << "\n"
                 << "Execution Time: " << formatDuration(result.executionTime) << "\n"
                 << "Reward Achieved: " << result.rewardAchieved << "\n"
                 << "Error: " << result.errorMessage << "\n"
                 << "---\n";
        }
    }
    
    void BatchExperimentRunner::generateReport(const std::vector<BatchResult>& results) {
        if (config.outputFormat == "console") {
            generateTextReport(results);
        } else if (config.outputFormat == "json") {
            std::string jsonReport = generateJSONReport(results);
            std::ofstream file(config.outputFile + ".json");
            if (file.is_open()) {
                file << jsonReport;
            }
        } else if (config.outputFormat == "html") {
            std::string htmlReport = generateHTMLReport(results);
            std::ofstream file(config.outputFile + ".html");
            if (file.is_open()) {
                file << htmlReport;
            }
        }
    }
    
    bool BatchExperimentRunner::shouldContinue(const BatchResult& result) {
        if (config.continueOnError) {
            return true;
        }
        
        return result.success;
    }

    // Utility functions implementation
    std::string BatchExperimentRunner::generateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string BatchExperimentRunner::formatDuration(double seconds) {
        std::stringstream ss;
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds / 60) % 60);
        double secs = seconds % 60;
        
        if (hours > 0) {
            ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(3) << secs << "s";
        } else if (minutes > 0) {
            ss << minutes << "m " << std::fixed << std::setprecision(3) << secs << "s";
        } else {
            ss << std::fixed << std::setprecision(3) << secs << "s";
        }
        
        return ss.str();
    }
    
    std::string BatchExperimentRunner::escapeCSV(const std::string& str) {
        if (str.find(',') == std::string::npos && str.find('"') == std::string::npos) {
            return str;
        }
        
        std::string escaped = "\"";
        for (char c : str) {
            if (c == '"') {
                escaped += """"""""";
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    
    std::string BatchExperimentRunner::generateJSONReport(const std::vector<BatchResult>& results) {
        std::stringstream ss;
        ss << "{\n  \"summary\": {\n";
        ss << "    \"total_experiments\": " << results.size() << ",\n";
        ss << "    \"successful\": " << std::count_if(results.begin(), results.end(), 
                            [](const BatchResult& r) { return r.success; }) << ",\n";
        ss << "    \"total_reward\": " << std::accumulate(results.begin(), results.end(), 0.0,
                            [](double sum, const BatchResult& r) { return sum + r.rewardAchieved; }) << ",\n";
        ss << "    \"total_time\": " << std::accumulate(results.begin(), results.end(), 0.0,
                            [](double sum, const BatchResult& r) { return sum + r.executionTime; }) << ",\n";
        ss << "    \"average_reward\": " << (results.empty() ? 0.0 : 
                            std::accumulate(results.begin(), results.end(), 0.0,
                                [](double sum, const BatchResult& r) { return sum + r.rewardAchieved; }) / results.size()) << "\n";
        ss << "  },\n";
        ss << "  \"experiments\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            ss << "    {\n";
            ss << "      \"name\": \"" << result.experimentName << "\",\n";
            ss << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
            ss << "      \"execution_time\": " << result.executionTime << ",\n";
            ss << "      \"reward\": " << result.rewardAchieved << ",\n";
            ss << "      \"neurons_used\": " << result.neuronsUsed << ",\n";
            ss << "      \"steps_executed\": " << result.stepsExecuted << ",\n";
            ss << "      \"error\": \"" << result.errorMessage << "\"\n";
            if (i < results.size() - 1) {
                ss << "    },\n";
            } else {
                ss << "    }\n";
            }
        }
        
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string BatchExperimentRunner::generateHTMLReport(const std::vector<BatchResult>& results) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n<html>\n<head>\n";
        ss << "  <title>NLM Batch Experiment Report</title>\n";
        ss << "  <style>\n";
        ss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
        ss << "    h1 { color: #333; }\n";
        ss << "    table { border-collapse: collapse; width: 100%; }\n";
        ss << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
        ss << "    th { background-color: #f2f2f2; }\n";
        ss << "    .success { background-color: #d4edda; }\n";
        ss << "    .failed { background-color: #f8d7da; }\n";
        ss << "    .summary { background-color: #e9ecef; padding: 15px; border-radius: 5px; }\n";
        ss << "  </style>\n";
        ss << "</head>\n<body>\n";
        ss << "  <h1>NLM Batch Experiment Report</h1>\n";
        ss << "  <div class=\"summary\">\n";
        ss << "    <h2>Summary</h2>\n";
        ss << "    <p><strong>Total Experiments:</strong> " << results.size() << "</p>\n";
        ss << "    <p><strong>Successful:</strong> " << std::count_if(results.begin(), results.end(),
                              [](const BatchResult& r) { return r.success; }) << "</p>\n";
        ss << "    <p><strong>Total Reward:</strong> " << std::accumulate(results.begin(), results.end(), 0.0,
                              [](double sum, const BatchResult& r) { return sum + r.rewardAchieved; }) << "</p>\n";
        ss << "    <p><strong>Total Execution Time:</strong> " << std::accumulate(results.begin(), results.end(), 0.0,
                              [](double sum, const BatchResult& r) { return sum + r.executionTime; }) << " seconds</p>\n";
        ss << "  </div>\n";
        ss << "  <h2>Detailed Results</h2>\n";
        ss << "  <table>\n";
        ss << "    <thead>\n";
        ss << "      <tr>\n";
        ss << "        <th>Experiment Name</th>\n";
        ss << "        <th>Status</th>\n";
        ss << "        <th>Execution Time (s)</th>\n";
        ss << "        <th>Reward</th>\n";
        ss << "        <th>Neurons Used</th>\n";
        ss << "        <th>Steps Executed</th>\n";
        ss << "        <th>Error Message</th>\n";
        ss << "      </tr>\n";
        ss << "    </thead>\n";
        ss << "    <tbody>\n";
        
        for (const auto& result : results) {
            ss << "      <tr class=\"" << (result.success ? "success" : "failed") << "\">\n";
            ss << "        <td>" << result.experimentName << "</td>\n";
            ss << "        <td>" << (result.success ? "SUCCESS" : "FAILED") << "</td>\n";
            ss << "        <td>" << std::fixed << std::setprecision(3) << result.executionTime << "</td>\n";
            ss << "        <td>" << std::fixed << std::setprecision(6) << result.rewardAchieved << "</td>\n";
            ss << "        <td>" << result.neuronsUsed << "</td>\n";
            ss << "        <td>" << result.stepsExecuted << "</td>\n";
            ss << "        <td>" << result.errorMessage << "</td>\n";
            ss << "      </tr>\n";
        }
        
        ss << "    </tbody>\n";
        ss << "  </table>\n";
        ss << "</body>\n</html>\n";
        
        return ss.str();
    }
    
    std::string BatchExperimentRunner::generateTextReport(const std::vector<BatchResult>& results) {
        std::stringstream ss;
        ss << "NLM Batch Experiment Report\n";
        ss << std::string(50, '=') << "\n\n";
        
        ss << "Summary:\n";
        ss << "  Total Experiments: " << results.size() << "\n";
        ss << "  Successful: " << std::count_if(results.begin(), results.end(),
                            [](const BatchResult& r) { return r.success; }) << "\n";
        ss << "  Total Reward: " << std::accumulate(results.begin(), results.end(), 0.0,
                            [](double sum, const BatchResult& r) { return sum + r.rewardAchieved; }) << "\n";
        ss << "  Total Execution Time: " << std::accumulate(results.begin(), results.end(), 0.0,
                            [](double sum, const BatchResult& r) { return sum + r.executionTime; }) << " seconds\n\n";
        
        ss << "Detailed Results:\n";
        ss << std::string(50, '-') << "\n";
        
        for (const auto& result : results) {
            ss << "Experiment: " << result.experimentName << "\n";
            ss << "  Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
            ss << "  Execution Time: " << std::fixed << std::setprecision(3) << result.executionTime << " seconds\n";
            ss << "  Reward Achieved: " << std::fixed << std::setprecision(6) << result.rewardAchieved << "\n";
            ss << "  Neurons Used: " << result.neuronsUsed << "\n";
            ss << "  Steps Executed: " << result.stepsExecuted << "\n";
            if (!result.errorMessage.empty()) {
                ss << "  Error: " << result.errorMessage << "\n";
            }
            ss << "\n";
        }
        
        return ss.str();
    }

} // namespace NLMBatch
