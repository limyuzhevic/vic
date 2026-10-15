#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <fstream>
#include <cmath>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

class ExperimentUtils {
public:
    // String utilities
    static std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    static std::string toUpperCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            return "";
        }
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }

    static bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    static bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    static std::string generateExperimentName(const std::string& prefix = "exp", size_t suffix = 0) {
        if (suffix == 0) {
            // Use timestamp if no suffix provided
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();
            suffix = static_cast<size_t>(timestamp);
        }
        return prefix + "_" + std::to_string(suffix);
    }

    // Parameter parsing utilities
    static std::map<std::string, std::string> parseParameterString(const std::string& paramString) {
        std::map<std::string, std::string> parameters;
        std::stringstream ss(paramString);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            size_t equalsPos = item.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = trim(item.substr(0, equalsPos));
                std::string value = trim(item.substr(equalsPos + 1));
                parameters[key] = value;
            }
        }
        
        return parameters;
    }

    static std::string parameterToString(const std::map<std::string, std::string>& parameters) {
        std::stringstream ss;
        bool first = true;
        
        for (const auto& param : parameters) {
            if (!first) ss << ",";
            ss << param.first << "=" << param.second;
            first = false;
        }
        
        return ss.str();
    }

    // File path utilities
    static std::string getDefaultResultDirectory() {
        return "results/";
    }

    static std::string getDefaultCheckpointDirectory() {
        return "checkpoints/";
    }

    static std::string getDefaultParameterFile() {
        return "parameters.json";
    }

    // Validation utilities
    static bool isValidParameterName(const std::string& name) {
        if (name.empty() || !std::isalpha(name[0])) {
            return false;
        }
        for (char c : name) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        return true;
    }

    static bool isValidParameterValue(const std::string& value) {
        // Check if value is not empty
        return !value.empty();
    }

    static bool validateExperimentName(const std::string& name) {
        return !name.empty() && isValidParameterName(name);
    }

    // Time utilities
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
        return ss.str();
    }

    static double calculateTimeRemaining(double progress, double elapsedTime) {
        if (progress <= 0.0) {
            return 0.0;
        }
        return elapsedTime * (1.0 / progress - 1.0);
    }

    // Statistics utilities
    static double calculateMean(const std::vector<double>& values) {
        if (values.empty()) {
            return 0.0;
        }
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }

    static double calculateStandardDeviation(const std::vector<double>& values, double mean) {
        if (values.size() < 2) {
            return 0.0;
        }
        double sum = 0.0;
        for (double value : values) {
            sum += (value - mean) * (value - mean);
        }
        return std::sqrt(sum / (values.size() - 1));
    }

    static double calculatePercentile(const std::vector<double>& values, double percentile) {
        if (values.empty()) {
            return 0.0;
        }
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        
        double index = (percentile / 100.0) * (sorted.size() - 1);
        size_t lowerIndex = static_cast<size_t>(std::floor(index));
        size_t upperIndex = static_cast<size_t>(std::ceil(index));
        
        if (lowerIndex == upperIndex) {
            return sorted[lowerIndex];
        }
        
        double weight = index - lowerIndex;
        return sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight;
    }

    // JSON utilities
    static std::string jsonEscape(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < 32 || c == 127) {
                        // Control characters
                        result += "\\u" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
            }
        }
        return result;
    }

    static std::string generateJSONFromExperiment(const std::shared_ptr<BaseExperiment>& experiment) {
        json j;
        j["experiment_name"] = experiment->getName();
        j["experiment_type"] = experiment->getExperimentType();
        j["progress"] = experiment->getProgress();
        j["current_step"] = experiment->getCurrentStep();
        j["total_steps"] = experiment->getTotalSteps();
        j["execution_time"] = experiment->getExecutionTime().count();
        
        auto metrics = experiment->getMetrics();
        j["metrics"] = json::object();
        for (const auto& metric : metrics) {
            j["metrics"][metric.first] = metric.second;
        }
        
        return j.dump(2);
    }

    // Export utilities
    static bool exportToCSV(const std::string& filepath, const std::vector<std::shared_ptr<BaseExperiment>>& experiments) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "Experiment Name,Experiment Type,Progress,Current Step,Total Steps,Performance Score" << std::endl;
        
        for (const auto& experiment : experiments) {
            file << experiment->getName() << ",";
            file << experiment->getExperimentType() << ",";
            file << experiment->getProgress() << ",";
            file << experiment->getCurrentStep() << ",";
            file << experiment->getTotalSteps() << ";";
            file << experiment->calculatePerformanceScore() << std::endl;
        }
        
        return true;
    }

    static bool exportToJSON(const std::string& filepath, const std::vector<std::shared_ptr<BaseExperiment>>& experiments) {
        json j;
        j["experiments"] = json::array();
        
        for (const auto& experiment : experiments) {
            json expJson;
            expJson["name"] = experiment->getName();
            expJson["type"] = experiment->getExperimentType();
            expJson["progress"] = experiment->getProgress();
            expJson["performance_score"] = experiment->calculatePerformanceScore();
            
            auto metrics = experiment->getMetrics();
            expJson["metrics"] = json::object();
            for (const auto& metric : metrics) {
                expJson["metrics"][metric.first] = metric.second;
            }
            
            j["experiments"].push_back(expJson);
        }
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            return true;
        }
        
        return false;
    }

    static bool exportToMarkdown(const std::string& filepath, const std::vector<std::shared_ptr<BaseExperiment>>& experiments) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Experiment Results Report" << std::endl;
        file << std::endl;
        
        for (const auto& experiment : experiments) {
            file << "## " << experiment->getName() << " (" << experiment->getExperimentType() << ")" << std::endl;
            file << std::endl;
            file << "- **Progress**: " << experiment->getProgress() * 100 << "%" << std::endl;
            file << "- **Current Step**: " << experiment->getCurrentStep() << "/" << experiment->getTotalSteps() << std::endl;
            file << "- **Performance Score**: " << experiment->calculatePerformanceScore() << std::endl;
            file << std::endl;
            
            auto metrics = experiment->getMetrics();
            if (!metrics.empty()) {
                file << "### Metrics" << std::endl;
                file << std::endl;
                file << "| Metric | Value |" << std::endl;
                file << "|--------|-------|" << std::endl;
                
                for (const auto& metric : metrics) {
                    file << "| " << metric.first << " | " << metric.second << " |" << std::endl;
                }
                
                file << std::endl;
            }
            
            file << std::endl;
        }
        
        return true;
    }

    // IO utilities
    static bool fileExists(const std::string& filepath) {
        return std::filesystem::exists(filepath);
    }

    static bool createDirectory(const std::string& path) {
        return std::filesystem::create_directories(path);
    }

    static bool copyFile(const std::string& source, const std::string& destination) {
        try {
            std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }

    static std::vector<std::string> listFilesInDirectory(const std::string& directory, const std::string& extension) {
        std::vector<std::string> files;
        
        if (extension == "*") {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().filename().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    files.push_back(entry.path().filename().string());
                }
            }
        }
        
        return files;
    }
};

} // namespace nlm