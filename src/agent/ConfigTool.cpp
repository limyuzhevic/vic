// NLM Configuration Management Tool
// Manage experiment configurations, presets, and templates

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <filesystem>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"

using namespace nlm;
namespace fs = std::filesystem;

void printConfigBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Configuration Manager                   ║
║                                                               ║
║  Manage configurations, presets, and experiment templates    ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

class NLMConfigManager {
public:
    struct ConfigPreset {
        std::string name;
        std::string description;
        std::string category;
        std::map<std::string, std::string> values;
        std::string author;
        std::string date;
        bool isDefault;
    };

    struct ExperimentConfig {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        std::vector<std::string> tags;
        std::string createdDate;
        std::string lastModified;
        std::string version;
    };

    NLMConfigManager() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    void createPreset(const std::string& name, const std::string& description,
                      const std::string& category, const std::map<std::string, std::string>& values,
                      const std::string& author = "User") {
        ConfigPreset preset;
        preset.name = name;
        preset.description = description;
        preset.category = category;
        preset.values = values;
        preset.author = author;
        preset.date = getCurrentDate();
        preset.isDefault = false;
        
        presets_[name] = preset;
        
        // Save to file
        savePreset(preset);
        
        NLM_LOG_INFO("Created preset: " + name);
    }

    void createExperiment(const std::string& name, const std::string& description,
                          const std::map<std::string, std::string>& parameters,
                          const std::vector<std::string>& tags = {}) {
        ExperimentConfig config;
        config.name = name;
        config.description = description;
        config.parameters = parameters;
        config.tags = tags;
        config.createdDate = getCurrentDate();
        config.lastModified = getCurrentDate();
        config.version = "1.0";
        
        experiments_[name] = config;
        
        // Save to file
        saveExperiment(config);
        
        NLM_LOG_INFO("Created experiment: " + name);
    }

    void listPresets() {
        std::cout << "=== Available Presets ===" << std::endl;
        std::cout << std::left << std::setw(20) << "Name" 
                  << std::setw(15) << "Category" 
                  << std::setw(30) << "Description" 
                  << std::setw(15) << "Author" 
                  << std::setw(15) << "Default" << std::endl;
        std::cout << std::string(110, '-') << std::endl;
        
        for (const auto& pair : presets_) {
            const auto& preset = pair.second;
            std::cout << std::left << std::setw(20) << preset.name
                      << std::setw(15) << preset.category
                      << std::setw(30) << preset.description
                      << std::setw(15) << preset.author
                      << std::setw(15) << (preset.isDefault ? "Yes" : "No") << std::endl;
        }
    }

    void listExperiments() {
        std::cout << "=== Available Experiments ===" << std::string(30, ' ') << std::endl;
        std::cout << std::left << std::setw(20) << "Name" 
                  << std::setw(30) << "Description" 
                  << std::setw(20) << "Tags" 
                  << std::setw(15) << "Version" 
                  << std::setw(15) << "Created" << std::endl;
        std::cout << std::string(105, '-') << std::endl;
        
        for (const auto& pair : experiments_) {
            const auto& exp = pair.second;
            std::string tagsStr = "";
            for (size_t i = 0; i < exp.tags.size() && i < 3; ++i) {
                tagsStr += exp.tags[i];
                if (i < exp.tags.size() - 1) tagsStr += ", ";
            }
            if (exp.tags.size() > 3) tagsStr += "...";
            
            std::cout << std::left << std::setw(20) << exp.name
                      << std::setw(30) << exp.description
                      << std::setw(20) << tagsStr
                      << std::setw(15) << exp.version
                      << std::setw(15) << exp.createdDate << std::endl;
        }
    }

    void showPreset(const std::string& name) {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            std::cout << "Preset not found: " << name << std::endl;
            return;
        }
        
        const auto& preset = it->second;
        
        std::cout << "=== Preset Details: " << preset.name << " ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Description: " << preset.description << std::endl;
        std::cout << "Category: " << preset.category << std::endl;
        std::cout << "Author: " << preset.author << std::endl;
        std::cout << "Created: " << preset.date << std::endl;
        std::cout << "Is Default: " << (preset.isDefault ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        std::cout << "Parameters:" << std::endl;
        
        for (const auto& param : preset.values) {
            std::cout << "  " << param.first << " = " << param.second << std::endl;
        }
        
        std::cout << std::endl;
    }

    void showExperiment(const std::string& name) {
        auto it = experiments_.find(name);
        if (it == experiments_.end()) {
            std::cout << "Experiment not found: " << name << std::endl;
            return;
        }
        
        const auto& exp = it->second;
        
        std::cout << "=== Experiment Details: " << exp.name << " ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Description: " << exp.description << std::endl;
        std::cout << "Version: " << exp.version << std::endl;
        std::cout << "Created: " << exp.createdDate << std::endl;
        std::cout << "Last Modified: " << exp.lastModified << std::endl;
        std::cout << std::endl;
        
        if (!exp.tags.empty()) {
            std::cout << "Tags: ";
            for (size_t i = 0; i < exp.tags.size(); ++i) {
                std::cout << exp.tags[i];
                if (i < exp.tags.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "Parameters:" << std::endl;
        for (const auto& param : exp.parameters) {
            std::cout << "  " << param.first << " = " << param.second << std::endl;
        }
    }

    void exportPreset(const std::string& name, const std::string& outputFile) {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            std::cout << "Preset not found: " << name << std::endl;
            return;
        }
        
        const auto& preset = it->second;
        savePreset(preset, outputFile);
        std::cout << "Preset exported to " << outputFile << std::endl;
    }

    void importPreset(const std::string& inputFile) {
        if (!fs::exists(inputFile)) {
            std::cout << "File not found: " << inputFile << std::endl;
            return;
        }
        
        ConfigPreset preset = loadPreset(inputFile);
        if (!preset.name.empty()) {
            presets_[preset.name] = preset;
            std::cout << "Preset imported: " << preset.name << std::endl;
        }
    }

    void exportExperiment(const std::string& name, const std::string& outputFile) {
        auto it = experiments_.find(name);
        if (it == experiments_.end()) {
            std::cout << "Experiment not found: " << name << std::endl;
            return;
        }
        
        const auto& exp = it->second;
        saveExperiment(exp, outputFile);
        std::cout << "Experiment exported to " << outputFile << std::endl;
    }

    void importExperiment(const std::string& inputFile) {
        if (!fs::exists(inputFile)) {
            std::cout << "File not found: " << inputFile << std::endl;
            return;
        }
        
        ExperimentConfig exp = loadExperiment(inputFile);
        if (!exp.name.empty()) {
            experiments_[exp.name] = exp;
            std::cout << "Experiment imported: " << exp.name << std::endl;
        }
    }

    std::map<std::string, std::string> getPresetValues(const std::string& name) {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            return {};
        }
        return it->second.values;
    }

    std::map<std::string, std::string> getExperimentParameters(const std::string& name) {
        auto it = experiments_.find(name);
        if (it == experiments_.end()) {
            return {};
        }
        return it->second.parameters;
    }

private:
    std::map<std::string, ConfigPreset> presets_;
    std::map<std::string, ExperimentConfig> experiments_;

    std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    void savePreset(const ConfigPreset& preset, const std::string& filename = "") {
        if (filename.empty()) {
            filename = "presets/" + preset.name + ".preset";
        }
        
        fs::create_directories("presets");
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "PRESET" << std::endl;
        file << "name=" << preset.name << std::endl;
        file << "description=" << preset.description << std::endl;
        file << "category=" << preset.category << std::endl;
        file << "author=" << preset.author << std::endl;
        file << "date=" << preset.date << std::endl;
        file << "isDefault=" << (preset.isDefault ? "true" : "false") << std::endl;
        file << "values=" << std::endl;
        
        for (const auto& param : preset.values) {
            file << "  " << param.first << "=" << param.second << std::endl;
        }
        
        file.close();
    }

    ConfigPreset loadPreset(const std::string& filename) {
        ConfigPreset preset;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            return preset;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("name=") == 0) {
                preset.name = line.substr(5);
            } else if (line.find("description=") == 0) {
                preset.description = line.substr(11);
            } else if (line.find("category=") == 0) {
                preset.category = line.substr(9);
            } else if (line.find("author=") == 0) {
                preset.author = line.substr(7);
            } else if (line.find("date=") == 0) {
                preset.date = line.substr(5);
            } else if (line.find("isDefault=") == 0) {
                preset.isDefault = (line.substr(10) == "true");
            } else if (line.find("  ") == 0 && line.size() > 2) {
                size_t pos = line.find('=');
                if (pos != std::string::npos && pos > 2) {
                    std::string key = line.substr(2, pos - 2);
                    std::string value = line.substr(pos + 1);
                    preset.values[key] = value;
                }
            }
        }
        
        file.close();
        return preset;
    }

    void saveExperiment(const ExperimentConfig& exp, const std::string& filename = "") {
        if (filename.empty()) {
            filename = "experiments/" + exp.name + ".exp";
        }
        
        fs::create_directories("experiments");
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "EXPERIMENT" << std::endl;
        file << "name=" << exp.name << std::endl;
        file << "description=" << exp.description << std::endl;
        file << "version=" << exp.version << std::endl;
        file << "created=" << exp.createdDate << std::endl;
        file << "modified=" << exp.lastModified << std::endl;
        file << "tags=" << std::endl;
        
        for (const auto& tag : exp.tags) {
            file << "  " << tag << std::endl;
        }
        
        file << "parameters=" << std::endl;
        
        for (const auto& param : exp.parameters) {
            file << "  " << param.first << "=" << param.second << std::endl;
        }
        
        file.close();
    }

    ExperimentConfig loadExperiment(const std::string& filename) {
        ExperimentConfig exp;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            return exp;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("name=") == 0) {
                exp.name = line.substr(5);
            } else if (line.find("description=") == 0) {
                exp.description = line.substr(11);
            } else if (line.find("version=") == 0) {
                exp.version = line.substr(8);
            } else if (line.find("created=") == 0) {
                exp.createdDate = line.substr(7);
            } else if (line.find("modified=") == 0) {
                exp.lastModified = line.substr(9);
            } else if (line.find("  ") == 0 && line.size() > 2 && line.find("=", 2) != std::string::npos) {
                size_t pos = line.find('=');
                if (pos > 2) {
                    std::string key = line.substr(2, pos - 2);
                    std::string value = line.substr(pos + 1);
                    exp.parameters[key] = value;
                }
            } else if (line.find("  ") == 0 && line.size() > 2) {
                exp.tags.push_back(line.substr(2));
            }
        }
        
        file.close();
        return exp;
    }
};

int main(int argc, char** argv) {
    printConfigBanner();
    
    if (argc < 2) {
        std::cout << "Usage: nlm_config <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  list-presets              List all saved presets" << std::endl;
        std::cout << "  list-experiments          List all experiment configurations" << std::endl;
        std::cout << "  show-preset <name>        Show preset details" << std::endl;
        std::cout << "  show-experiment <name>    Show experiment details" << std::endl;
        std::cout << "  create-preset <name> [options]  Create a new preset" << std::endl;
        std::cout << "  create-experiment <name> [options] Create a new experiment config" << std::endl;
        std::cout << "  export-preset <name> <file>   Export preset to file" << std::endl;
        std::cout << "  import-preset <file>         Import preset from file" << std::endl;
        std::cout << "  export-experiment <name> <file> Export experiment to file" << std::endl;
        std::cout << "  import-experiment <file>      Import experiment from file" << std::endl;
        std::cout << std::endl;
        return 0;
    }

    std::string command(argv[1]);
    NLMConfigManager manager;
    
    if (command == "list-presets") {
        manager.listPresets();
    } else if (command == "list-experiments") {
        manager.listExperiments();
    } else if (command == "show-preset" && argc >= 3) {
        manager.showPreset(argv[2]);
    } else if (command == "show-experiment" && argc >= 3) {
        manager.showExperiment(argv[2]);
    } else if (command == "create-preset" && argc >= 3) {
        std::cout << "Creating preset: " << argv[2] << std::endl;
        // In real implementation, parse additional options
        std::cout << "Preset creation functionality would be implemented here" << std::endl;
    } else if (command == "create-experiment" && argc >= 3) {
        std::cout << "Creating experiment: " << argv[2] << std::endl;
        // In real implementation, parse additional options
        std::cout << "Experiment creation functionality would be implemented here" << std::endl;
    } else if (command == "export-preset" && argc >= 4) {
        manager.exportPreset(argv[2], argv[3]);
    } else if (command == "import-preset" && argc >= 3) {
        manager.importPreset(argv[2]);
    } else if (command == "export-experiment" && argc >= 4) {
        manager.exportExperiment(argv[2], argv[3]);
    } else if (command == "import-experiment" && argc >= 3) {
        manager.importExperiment(argv[2]);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        return 1;
    }
    
    return 0;
}
