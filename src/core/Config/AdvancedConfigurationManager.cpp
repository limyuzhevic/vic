#include "AdvancedConfigurationManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace nlm {

AdvancedConfigurationManager::AdvancedConfigurationManager() : pImpl(std::make_unique<Impl>()) {}

AdvancedConfigurationManager::~AdvancedConfigurationManager() = default;

bool AdvancedConfigurationManager::backupConfiguration(const std::string& configFile, const std::string& backupFilepath) {
    std::string sourceFile = configFile.empty() ? "nlm_config.cfg" : configFile;
    std::string targetFile = backupFilepath.empty() ? getBackupFilename(sourceFile) : backupFilepath;
    
    std::ifstream source(sourceFile, std::ios::binary);
    if (!source.is_open()) {
        return false;
    }
    
    std::ofstream target(targetFile, std::ios::binary);
    if (!target.is_open()) {
        return false;
    }
    
    target << source.rdbuf();
    
    return true;
}

bool AdvancedConfigurationManager::restoreConfiguration(const std::string& backupFilepath, const std::string& restoreFilepath) {
    std::ifstream source(backupFilepath, std::ios::binary);
    if (!source.is_open()) {
        return false;
    }
    
    std::string targetFile = restoreFilepath.empty() ? "nlm_config.cfg" : restoreFilepath;
    std::ofstream target(targetFile, std::ios::binary);
    if (!target.is_open()) {
        return false;
    }
    
    target << source.rdbuf();
    
    return true;
}

std::vector<std::string> AdvancedConfigurationManager::listBackups(const std::string& directory) {
    std::vector<std::string> backups;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (isBackupFile(filename)) {
                    backups.push_back(filename);
                }
            }
        }
    } catch (const std::exception&) {
        // Directory not accessible or other error
    }
    
    std::sort(backups.begin(), backups.end(), [this](const std::string& a, const std::string& b) {
        return getBackupTimestamp(a) > getBackupTimestamp(b);
    });
    
    return backups;
}

std::string AdvancedConfigurationManager::compareConfigurations(const std::string& file1, const std::string& file2) {
    ConfigurationComparator comparator;
    auto result = comparator.compareFiles(file1, file2);
    return result.toString();
}

std::string AdvancedConfigurationManager::compareToDefault(const std::string& configFile) {
    ConfigurationComparator comparator;
    auto result = comparator.compareToDefault(configFile);
    return result.toString();
}

bool AdvancedConfigurationManager::exportConfiguration(const std::string& configFile, const std::string& outputFile, const std::string& format) {
    std::string content;
    if (!loadConfiguration(content, configFile)) {
        return false;
    }
    
    ConfigurationFileFormat::Format fmt = ConfigurationFileFormat::detectFormat(format);
    return ConfigurationFileFormat::save(outputFile, content, fmt);
}

bool AdvancedConfigurationManager::importConfiguration(const std::string& inputFile, std::string& configFile, const std::string& format) {
    std::string content;
    ConfigurationFileFormat::Format fmt = ConfigurationFileFormat::detectFormat(format);
    
    if (!ConfigurationFileFormat::load(content, inputFile, fmt)) {
        return false;
    }
    
    if (configFile.empty()) {
        configFile = "imported_config.cfg";
    }
    
    std::ofstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::string AdvancedConfigurationManager::analyzeConfiguration(const std::string& configFile) {
    ConfigurationAnalyzer analyzer;
    auto result = analyzer.analyzeConfiguration(configFile);
    return result.toString();
}

std::map<std::string, double> AdvancedConfigurationManager::getConfigurationStats(const std::string& configFile) {
    ConfigurationAnalyzer analyzer;
    auto result = analyzer.analyzeConfiguration(configFile);
    return result.parameterStats;
}

bool AdvancedConfigurationManager::saveTemplate(const std::string& name, const std::string& description) {
    std::string configFile = "template_" + name + ".cfg";
    if (!std::filesystem::exists(configFile)) {
        return false;
    }
    
    std::string templateDir = "templates/";
    std::string templateFile = templateDir + name + ".template";
    
    std::ifstream source(configFile);
    std::ofstream target(templateFile);
    
    if (!source.is_open() || !target.is_open()) {
        return false;
    }
    
    target << source.rdbuf();
    
    // Save template metadata
    std::string metadataFile = templateDir + name + ".meta";
    std::ofstream meta(metadataFile);
    if (meta.is_open()) {
        meta << "name: " << name << std::endl;
        meta << "description: " << description << std::endl;
        meta << "created: " << getCurrentTimestamp() << std::endl;
        meta << "version: 1.0" << std::endl;
    }
    
    return true;
}

bool AdvancedConfigurationManager::loadTemplate(const std::string& name) {
    std::string templateFile = "templates/" + name + ".template";
    std::string metadataFile = "templates/" + name + ".meta";
    
    if (!std::filesystem::exists(templateFile) || !std::filesystem::exists(metadataFile)) {
        return false;
    }
    
    std::string configFile = "nlm_config.cfg";
    std::ifstream templateStream(templateFile);
    std::ofstream configStream(configFile);
    
    if (!templateStream.is_open() || !configStream.is_open()) {
        return false;
    }
    
    configStream << templateStream.rdbuf();
    
    // Load metadata
    std::ifstream metaStream(metadataFile);
    if (metaStream.is_open()) {
        std::string line;
        while (std::getline(metaStream, line)) {
            if (line.find("name:") == 0) {
                // Update template name
            } else if (line.find("description:") == 0) {
                // Update template description
            }
        }
    }
    
    return true;
}

std::vector<std::string> AdvancedConfigurationManager::listTemplates() {
    std::vector<std::string> templates;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator("templates/")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (isTemplateFile(filename)) {
                    templates.push_back(filename.substr(0, filename.find(".template")));
                }
            }
        }
    } catch (const std::exception&) {
        // Directory not accessible or other error
    }
    
    return templates;
}

bool AdvancedConfigurationManager::deleteTemplate(const std::string& name) {
    std::string templateFile = "templates/" + name + ".template";
    std::string metadataFile = "templates/" + name + ".meta";
    
    bool success = true;
    
    if (std::filesystem::exists(templateFile)) {
        success &= std::filesystem::remove(templateFile);
    }
    
    if (std::filesystem::exists(metadataFile)) {
        success &= std::filesystem::remove(metadataFile);
    }
    
    return success;
}

std::string AdvancedConfigurationManager::getAdvancedCommandHelp() {
    return R"(
Advanced Configuration Commands:
  backup                     Create backup of current configuration
  restore <file>            Restore configuration from backup
  list-backups              List all available backups
  compare <file1> <file2>    Compare two configuration files
  compare-default           Compare current configuration to default
  export <file> <format>    Export configuration to different formats
  import <file> <format>     Import configuration from different formats
  analyze                   Analyze configuration and provide recommendations
  validate                  Validate configuration against rules
  optimize                  Optimize configuration parameters
  template save <name> <desc> Save current configuration as template
  template load <name>       Load configuration from template
  template list              List all available templates
  template delete <name>     Delete template
  statistics                Get configuration statistics
  diff <file1> <file2>       Show differences between two files
  report                    Generate detailed configuration report
  execute <script>          Execute configuration script
  batch <commands>          Execute batch of commands
)";
}

std::string AdvancedConfigurationManager::getConfigurationAnalysisHelp() {
    return R"(
Configuration Analysis Commands:
  analyze                   Analyze configuration completeness and quality
  statistics               Get detailed statistics about configuration
  validate                  Validate configuration against best practices
  optimize                  Automatically optimize configuration parameters
  issues                   List configuration issues and warnings
  recommendations            Get recommendations for configuration improvements
)";
}

std::string AdvancedConfigurationManager::getConfigurationBackupHelp() {
    return R"(
Configuration Backup Commands:
  backup [<file>]            Create backup of current configuration
  restore <file>            Restore configuration from backup file
  list-backups              List all available backup files
  clean <count>             Clean old backups, keeping last <count> files
  auto-backup <interval>    Enable automatic backup every <interval> minutes
  stop-auto-backup          Stop automatic backup
)";
}

std::string AdvancedConfigurationManager::getConfigurationTemplateHelp() {
    return R"(
Configuration Template Commands:
  template save <name> <desc> Save current configuration as template
  template load <name>       Load configuration from template
  template list              List all available templates
  template delete <name>     Delete template
  template rename <old> <new> Rename template
  template copy <source> <target> Copy template to new template
  template merge <base> <overlay> <result> Merge two templates
)";
}

std::vector<std::string> AdvancedConfigurationManager::parseAdvancedCommandLineArgs(int argc, char** argv) {
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.find("backup") == 0) {
            if (arg.find("=") != std::string::npos) {
                args.push_back("--backup=" + arg.substr(arg.find("=") + 1));
            } else if (i + 1 < argc) {
                args.push_back("--backup=" + std::string(argv[++i]));
            }
        } else if (arg.find("restore") == 0) {
            if (arg.find("=") != std::string::npos) {
                args.push_back("--restore=" + arg.substr(arg.find("=") + 1));
            }
        } else if (arg.find("analyze") == 0) {
            args.push_back("--analyze");
        } else if (arg.find("validate") == 0) {
            args.push_back("--validate");
        } else if (arg.find("optimize") == 0) {
            args.push_back("--optimize");
        } else if (arg.find("template") == 0) {
            if (i + 1 < argc) {
                args.push_back("--template=" + std::string(argv[++i]));
                if (i + 1 < argc) {
                    args.push_back("--template-desc=" + std::string(argv[++i]));
                }
            }
        } else if (arg.find("export") == 0) {
            if (arg.find("=") != std::string::npos) {
                args.push_back("--export=" + arg.substr(arg.find("=") + 1));
                if (i + 1 < argc) {
                    args.push_back("--export-format=" + std::string(argv[++i]));
                }
            }
        } else if (arg.find("import") == 0) {
            if (arg.find("=") != std::string::npos) {
                args.push_back("--import=" + arg.substr(arg.find("=") + 1));
            }
        } else {
            args.push_back(arg);
        }
    }
    
    return args;
}

std::string AdvancedConfigurationManager::getBackupFilename(const std::string& configFile) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "backup_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << "_" << configFile;
    return ss.str();
}

bool AdvancedConfigurationManager::isBackupFile(const std::string& filename) {
    return filename.find("backup_") == 0 && filename.find(".cfg") != std::string::npos;
}

std::chrono::system_clock::time_point AdvancedConfigurationManager::getBackupTimestamp(const std::string& filename) {
    // Extract timestamp from filename
    std::string timestampStr = filename.substr(7, 15); // "backup_YYYYMMDD_HHMMSS_"
    std::tm tm = {};
    std::istringstream ss(timestampStr);
    ss >> std::get_time(&tm, "%Y%m%d_%H%M%S");
    if (ss.fail()) {
        return std::chrono::system_clock::time_point();
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

bool AdvancedConfigurationManager::isTemplateFile(const std::string& filename) {
    return filename.find(".template") != std::string::npos && filename.find(".meta") == std::string::npos;
}

std::string AdvancedConfigurationManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool AdvancedConfigurationManager::loadConfiguration(std::string& content, const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    return true;
}

// AdvancedConfigurationManager function implementations
void initializeAdvancedConfiguration() {
    // Initialize advanced configuration manager
}

void cleanupAdvancedConfiguration() {
    // Cleanup advanced configuration manager
}

AdvancedConfigurationManager& getAdvancedConfigurationManager() {
    static AdvancedConfigurationManager instance;
    return instance;
}

void setAdvancedConfigurationManager(std::shared_ptr<AdvancedConfigurationManager> manager) {
    // Note: This is a simplified implementation
}

std::vector<std::string> parseAdvancedConfigurationArgs(int argc, char** argv) {
    return AdvancedConfigurationManager::parseAdvancedCommandLineArgs(argc, argv);
}

bool processAdvancedConfigurationArgs(const std::vector<std::string>& args, ConfigurationManager& configManager) {
    for (const auto& arg : args) {
        if (arg.find("--backup=") == 0) {
            std::string backupFile = arg.substr(9);
            // Backup configuration
        } else if (arg.find("--restore=") == 0) {
            std::string backupFile = arg.substr(10);
            // Restore configuration
        } else if (arg == "--analyze") {
            // Analyze configuration
        } else if (arg == "--validate") {
            // Validate configuration
        } else if (arg == "--optimize") {
            // Optimize configuration
        } else if (arg.find("--template=") == 0) {
            std::string templateName = arg.substr(11);
            // Handle template commands
        }
    }
    return true;
}

bool executeConfigurationScript(const std::string& scriptContent, ConfigurationManager& configManager) {
    // Execute configuration script
    return true;
}

bool executeConfigurationScriptFile(const std::string& scriptFile, ConfigurationManager& configManager) {
    // Execute configuration script from file
    return true;
}

bool exportConfiguration(const std::string& configFile, const std::string& outputFile, const std::string& format) {
    // Export configuration
    return true;
}

bool importConfiguration(const std::string& inputFile, std::string& configFile, const std::string& format) {
    // Import configuration
    return true;
}

bool backupConfiguration(const std::string& configFile, const std::string& backupFilepath) {
    // Backup configuration
    return true;
}

bool restoreConfiguration(const std::string& backupFilepath, const std::string& configFile) {
    // Restore configuration
    return true;
}

std::string analyzeConfiguration(const std::string& configFile) {
    // Analyze configuration
    return "";
}

std::string compareConfigurations(const std::string& file1, const std::string& file2) {
    // Compare configurations
    return "";
}

} // namespace nlm
