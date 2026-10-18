#include "ConfigurationManager.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

bool ConfigurationManager::backupConfig(const std::shared_ptr<Config>& config, const std::string& backupPath) {
    try {
        std::ofstream file(backupPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Configuration Backup\n";
        file << "# Created: " << getCurrentTimestamp() << "\n";
        file << "# Source: " << backupPath << "\n\n";
        
        auto keys = config->getKeys();
        file << "# Configuration entries: " << keys.size() << "\n\n";
        
        for (const auto& key : keys) {
            auto valueOpt = config->get<std::string>(key);
            if (valueOpt) {
                file << key << " = \"" << *valueOpt << "\"\n";
            } else {
                auto intVal = config->get<int>(key);
                if (intVal) {
                    file << key << " = " << *intVal << "\n";
                } else {
                    auto doubleVal = config->get<double>(key);
                    if (doubleVal) {
                        file << key << " = " << *doubleVal << "\n";
                    } else {
                        auto boolVal = config->get<bool>(key);
                        if (boolVal) {
                            file << key << " = " << (*boolVal ? "true" : "false") << "\n";
                        }
                    }
                }
            }
        }
        
        file << "\n# End of backup\n";
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigurationManager::restoreConfig(std::shared_ptr<Config>& config, const std::string& backupPath) {
    try {
        config->clear();
        
        std::ifstream file(backupPath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config->set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ConfigurationManager::compareConfigs(const std::shared_ptr<Config>& config1,
                                                 const std::shared_ptr<Config>& config2) {
    std::stringstream ss;
    ss << "=== Configuration Comparison ===\n";
    
    auto keys1 = config1->getKeys();
    auto keys2 = config2->getKeys();
    
    std::set<std::string> set1(keys1.begin(), keys1.end());
    std::set<std::string> set2(keys2.begin(), keys2.end());
    
    std::set<std::string> onlyIn1, onlyIn2, inBoth;
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                          std::inserter(inBoth, inBoth.begin()));
    std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
                        std::inserter(onlyIn1, onlyIn1.begin()));
    std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(),
                        std::inserter(onlyIn2, onlyIn2.begin()));
    
    ss << "Keys only in config1: " << onlyIn1.size() << "\n";
    for (const auto& key : onlyIn1) {
        auto val1 = config1->get<std::string>(key);
        auto val2 = config2->get<std::string>(key);
        ss << "  " << key << " = \"" << (val1 ? *val1 : "") << "\"\n";
    }
    
    ss << "\nKeys only in config2: " << onlyIn2.size() << "\n";
    for (const auto& key : onlyIn2) {
        auto val1 = config1->get<std::string>(key);
        auto val2 = config2->get<std::string>(key);
        ss << "  " << key << " = \"" << (val2 ? *val2 : "") << "\"\n";
    }
    
    ss << "\nKeys in both configs: " << inBoth.size() << "\n";
    for (const auto& key : inBoth) {
        auto val1 = config1->get<std::string>(key);
        auto val2 = config2->get<std::string>(key);
        if (val1 != val2) {
            ss << "  " << key << " - DIFFERENT:\n";
            ss << "    Config1: \"" << (val1 ? *val1 : "") << "\"\n";
            ss << "    Config2: \"" << (val2 ? *val2 : "") << "\"\n";
        }
    }
    
    return ss.str();
}

std::string ConfigurationManager::generateDocumentation(const std::shared_ptr<Config>& config,
                                                         const std::string& title,
                                                         const std::string& contextPrefix) {
    std::stringstream ss;
    ss << contextPrefix << "# " << title << "\n\n";
    ss << contextPrefix << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    auto keys = config->getKeys();
    ss << contextPrefix << "## Configuration Parameters (" << keys.size() << " total)\n\n";
    
    for (const auto& key : keys) {
        ss << contextPrefix << "### " << key << "\n";
        
        auto valueOpt = config->get<std::string>(key);
        std::string valueStr;
        if (valueOpt) {
            valueStr = *valueOpt;
        } else {
            auto intVal = config->get<int>(key);
            if (intVal) {
                valueStr = std::to_string(*intVal);
            } else {
                auto doubleVal = config->get<double>(key);
                if (doubleVal) {
                    valueStr = std::to_string(*doubleVal);
                } else {
                    auto boolVal = config->get<bool>(key);
                    if (boolVal) {
                        valueStr = *boolVal ? "true" : "false";
                    }
                }
            }
        }
        
        ss << contextPrefix << "- **Value**: `" << valueStr << "`\n";
        ss << contextPrefix << "- **Type**: " << (valueOpt ? "string" : 
                            intVal ? "int" :
                            doubleVal ? "double" :
                            boolVal ? "bool" : "unknown") << "\n";
        ss << contextPrefix << "- **Description**: TODO - Add description for " << key << "\n\n";
    }
    
    return ss.str();
}

std::string ConfigurationManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string ConfigurationManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm