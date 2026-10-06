#pragma once

#include "../core/Logger/Logger.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace nlm {

class PluginTemplateGenerator {
public:
    static bool generateBasicPluginTemplate(const std::string& outputPath, 
                                          const std::string& pluginName) {
        std::string pluginDirectory = outputPath + "/" + pluginName;
        
        // Create directory
        if (!std::filesystem::exists(pluginDirectory)) {
            std::filesystem::create_directories(pluginDirectory);
        }
        
        // Generate Plugin.hpp
        std::string pluginHeaderPath = pluginDirectory + "/" + pluginName + ".hpp";
        generatePluginHeader(pluginHeaderPath, pluginName);
        
        // Generate Plugin.cpp
        std::string pluginSourcePath = pluginDirectory + "/" + pluginName + ".cpp";
        generatePluginSource(pluginSourcePath, pluginName);
        
        // Generate README.md
        std::string readmePath = pluginDirectory + "/README.md";
        generatePluginReadme(readmePath, pluginName);
        
        // Generate CMakeLists.txt
        std::string cmakePath = pluginDirectory + "/CMakeLists.txt";
        generatePluginCMake(cmakePath, pluginName);
        
        NLM_LOG_INFO(std::string("Generated basic plugin template at: ") + pluginDirectory);
        return true;
    }
    
    static bool generateServicePluginTemplate(const std::string& outputPath,
                                            const std::string& pluginName,
                                            const std::string& serviceName) {
        std::string pluginDirectory = outputPath + "/" + pluginName;
        
        // Create directory
        if (!std::filesystem::exists(pluginDirectory)) {
            std::filesystem::create_directories(pluginDirectory);
        }
        
        // Generate ServicePlugin.hpp
        std::string pluginHeaderPath = pluginDirectory + "/" + pluginName + ".hpp";
        generateServicePluginHeader(pluginHeaderPath, pluginName, serviceName);
        
        // Generate ServicePlugin.cpp
        std::string pluginSourcePath = pluginDirectory + "/" + pluginName + ".cpp";
        generateServicePluginSource(pluginSourcePath, pluginName, serviceName);
        
        // Generate README.md
        std::string readmePath = pluginDirectory + "/README.md";
        generatePluginReadme(readmePath, pluginName);
        
        // Generate CMakeLists.txt
        std::string cmakePath = pluginDirectory + "/CMakeLists.txt";
        generatePluginCMake(cmakePath, pluginName);
        
        NLM_LOG_INFO(std::string("Generated service plugin template at: ") + pluginDirectory);
        return true;
    }
    
    static bool generateBrainExtensionPluginTemplate(const std::string& outputPath,
                                                    const std::string& pluginName,
                                                    const std::string& extensionType) {
        std::string pluginDirectory = outputPath + "/" + pluginName;
        
        // Create directory
        if (!std::filesystem::exists(pluginDirectory)) {
            std::filesystem::create_directories(pluginDirectory);
        }
        
        // Generate BrainExtensionPlugin.hpp
        std::string pluginHeaderPath = pluginDirectory + "/" + pluginName + ".hpp";
        generateBrainExtensionPluginHeader(pluginHeaderPath, pluginName, extensionType);
        
        // Generate BrainExtensionPlugin.cpp
        std::string pluginSourcePath = pluginDirectory + "/" + pluginName + ".cpp";
        generateBrainExtensionPluginSource(pluginSourcePath, pluginName, extensionType);
        
        // Generate README.md
        std::string readmePath = pluginDirectory + "/README.md";
        generateBrainExtensionPluginReadme(readmePath, pluginName, extensionType);
        
        // Generate CMakeLists.txt
        std::string cmakePath = pluginDirectory + "/CMakeLists.txt";
        generatePluginCMake(cmakePath, pluginName);
        
        NLM_LOG_INFO(std::string("Generated brain extension plugin template at: ") + pluginDirectory);
        return true;
    }
    
private:
    static void generatePluginHeader(const std::string& filePath, const std::string& pluginName) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "#pragma once\n\n";
        file << "#include \"../Plugin.hpp\"\n\n";
        file << "namespace nlm {\n\n";
        file << "class " << pluginName << " : public Plugin {\n";
        file << "public:\n";
        file << "    " << pluginName << "();\n";
        file << "    ~" << pluginName << "() override;\n\n";
        file << "    const PluginMetadata& getMetadata() const override;\n";
        file << "    bool initialize() override;\n";
        file << "    void shutdown() override;\n";
        file << "    bool isInitialized() const override;\n";
        file << "    std::vector<ServiceInfo> getProvidedServices() const override;\n";
        file << "    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override;\n";
        file << "    bool onPluginEvent(const PluginEvent& event) override;\n";
        file << "    bool execute(const ExecutionContext& context) override;\n";
        file << "    void onBrainUpdate(const BrainUpdateEvent& event) override;\n";
        file << "    std::string getName() const override;\n";
        file << "    std::string getVersion() const override;\n";
        file << "\nprivate:\n";
        file << "    struct Impl;\n";
        file << "    std::unique_ptr<Impl> pImpl;\n";
        file << "};\n\n";
        file << "} // namespace nlm\n";
        
        file.close();
    }
    
    static void generatePluginSource(const std::string& filePath, const std::string& pluginName) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "#pragma once\n\n";
        file << "#include \"" << pluginName << ".hpp\"\n";
        file << "#include \"../core/Logger/Logger.hpp\"\n\n";
        file << "namespace nlm {\n\n";
        file << pluginName << "::" << pluginName << "() : pImpl(std::make_unique<Impl>()) {\n";
        file << "    // Initialize plugin metadata\n";
        file << "}\n\n";
        file << pluginName << "::~" << pluginName << "() = default;\n\n";
        file << "const PluginMetadata& " << pluginName << "::getMetadata() const {\n";
        file << "    static PluginMetadata metadata;\n";
        file << "    metadata.name = \"" << pluginName << "\";\n";
        file << "    metadata.version = \"1.0.0\";\n";
        file << "    metadata.description = \"Basic " << pluginName << " plugin for NLM\" ;\n";
        file << "    metadata.author = \"NLM Team\";\n";
        file << "    metadata.requiresBrain = false;\n";
        file << "    metadata.autoEnable = false;\n";
        file << "    metadata.type = PluginType::General;\n";
        file << "    return metadata;\n";
        file << "}\n\n";
        file << "// Implementation of other methods\n";
        file << "// ...\n\n";
        file << "} // namespace nlm\n";
        
        file.close();
    }
    
    static void generatePluginReadme(const std::string& filePath, const std::string& pluginName) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "# " << pluginName << " Plugin\n\n";
        file << "This is a basic plugin template for the NLM (熙然) project.\n\n";
        file << "## Description\n\n";
        file << "This plugin provides basic functionality for the NLM brain simulation.\n\n";
        file << "## Building\n\n";
        file << "```bash\n";
        file << "mkdir build\n";
        file << "cd " << pluginName << "\n";
        file << "cmake .. -DCMAKE_BUILD_TYPE=Release\n";
        file << "make -j4\n";
        file << "```\n\n";
        file << "## Usage\n\n";
        file << "1. Compile the plugin\n";
        file << "2. Add the plugin library path to NLM configuration\n";
        file << "3. Start NLM with the plugin system\n\n";
        file << "## Plugin Interface\n\n";
        file << "The plugin implements the Plugin interface and provides the following services:\n\n";
        file << "- Configuration management\n";
        file << "- Data storage\n";
        file << "- Event handling\n\n";
        file << "## License\n\n";
        file << "MIT\n";
        
        file.close();
    }
    
    static void generatePluginCMake(const std::string& filePath, const std::string& pluginName) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "cmake_minimum_required(VERSION 3.10)\n\n";
        file << "project(" << pluginName << " VERSION 1.0.0 LANGUAGES CXX)\n\n";
        file << "set(CMAKE_CXX_STANDARD 17)\n";
        file << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
        file << "# Find NLM library\n";
        file << "find_package(NLM REQUIRED)\n\n";
        file << "# Create plugin library\n";
        file << "add_library(" << pluginName << " SHARED\n";
        file << "    " << pluginName << ".cpp\n";
        file << ")\n\n";
        file << "# Link with NLM\n";
        file << "target_link_libraries(" << pluginName << " PRIVATE NLM::nlm)\n\n";
        file << "# Set output properties\n";
        file << "set_target_properties(" << pluginName << " PROPERTIES\n";
        file << "    PREFIX \"lib\"\n";
        file << "    SUFFIX \".so\"\n";
        file << "    SOVERSION ${PROJECT_VERSION_MAJOR}\n";
        file << ")\n";
        
        file.close();
    }
    
    static void generateServicePluginHeader(const std::string& filePath, 
                                          const std::string& pluginName,
                                          const std::string& serviceName) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "#pragma once\n\n";
        file << "#include \"../Plugin.hpp\"\n\n";
        file << "namespace nlm {\n\n";
        file << "class " << pluginName << " : public Plugin {\n";
        file << "public:\n";
        file << "    " << pluginName << "();\n";
        file << "    ~" << pluginName << "() override;\n\n";
        file << "    const PluginMetadata& getMetadata() const override;\n";
        file << "    bool initialize() override;\n";
        file << "    void shutdown() override;\n";
        file << "    bool isInitialized() const override;\n";
        file << "    std::vector<ServiceInfo> getProvidedServices() const override;\n";
        file << "    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override;\n";
        file << "    bool onPluginEvent(const PluginEvent& event) override;\n";
        file << "    bool execute(const ExecutionContext& context) override;\n";
        file << "    void onBrainUpdate(const BrainUpdateEvent& event) override;\n";
        file << "    std::string getName() const override;\n";
        file << "    std::string getVersion() const override;\n";
        file << "\nprivate:\n";
        file << "    struct Impl;\n";
        file << "    std::unique_ptr<Impl> pImpl;\n";
        file << "};\n\n";
        file << "} // namespace nlm\n";
        
        file.close();
    }
    
    static void generateServicePluginSource(const std::string& filePath,
                                          const std::string& pluginName,
                                          const std::string& serviceName) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "#pragma once\n\n";
        file << "#include \"" << pluginName << ".hpp\"\n";
        file << "#include \"../core/Logger/Logger.hpp\"\n\n";
        file << "namespace nlm {\n\n";
        file << pluginName << "::" << pluginName << "() : pImpl(std::make_unique<Impl>()) {\n";
        file << "    // Register services in constructor\n";
        file << "}\n\n";
        file << pluginName << "::~" << pluginName << "() = default;\n\n";
        file << "// Service implementation\n";
        file << "// ...\n\n";
        file << "} // namespace nlm\n";
        
        file.close();
    }
    
    static void generateBrainExtensionPluginHeader(const std::string& filePath,
                                                  const std::string& pluginName,
                                                  const std::string& extensionType) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "#pragma once\n\n";
        file << "#include \"../Plugin.hpp\"\n\n";
        file << "namespace nlm {\n\n";
        file << "class " << pluginName << " : public Plugin {\n";
        file << "public:\n";
        file << "    " << pluginName << "();\n";
        file << "    ~" << pluginName << "() override;\n\n";
        file << "    const PluginMetadata& getMetadata() const override;\n";
        file << "    bool initialize() override;\n";
        file << "    void shutdown() override;\n";
        file << "    bool isInitialized() const override;\n";
        file << "    std::vector<ServiceInfo> getProvidedServices() const override;\n";
        file << "    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override;\n";
        file << "    bool onPluginEvent(const PluginEvent& event) override;\n";
        file << "    bool execute(const ExecutionContext& context) override;\n";
        file << "    void onBrainUpdate(const BrainUpdateEvent& event) override;\n";
        file << "    std::string getName() const override;\n";
        file << "    std::string getVersion() const override;\n";
        file << "\nprivate:\n";
        file << "    struct Impl;\n";
        file << "    std::unique_ptr<Impl> pImpl;\n";
        file << "};\n\n";
        file << "} // namespace nlm\n";
        
        file.close();
    }
    
    static void generateBrainExtensionPluginSource(const std::string& filePath,
                                                  const std::string& pluginName,
                                                  const std::string& extensionType) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "#pragma once\n\n";
        file << "#include \"" << pluginName << ".hpp\"\n";
        file << "#include \"../core/Logger/Logger.hpp\"\n\n";
        file << "namespace nlm {\n\n";
        file << pluginName << "::" << pluginName << "() : pImpl(std::make_unique<Impl>()) {\n";
        file << "    // Initialize brain extension plugin\n";
        file << "}\n\n";
        file << pluginName << "::~" << pluginName << "() = default;\n\n";
        file << "// Brain extension implementation\n";
        file << "// ...\n\n";
        file << "} // namespace nlm\n";
        
        file.close();
    }
    
    static void generateBrainExtensionPluginReadme(const std::string& filePath,
                                                  const std::string& pluginName,
                                                  const std::string& extensionType) {
        std::ofstream file(filePath);
        if (!file) {
            NLM_LOG_ERROR(std::string("Failed to create file: ") + filePath);
            return;
        }
        
        file << "# " << pluginName << " Brain Extension Plugin\n\n";
        file << "This plugin extends the NLM (熙然) brain with " << extensionType << " capabilities.\n\n";
        file << "## Description\n\n";
        file << "This plugin extends the core brain functionality with custom behavior for " << extensionType << ".\n\n";
        file << "## Integration Points\n\n";
        file << "- Memory systems extension\n";
        file << "- Neuromodulation enhancement\n";
        file << "- Cognition extension\n";
        file << "- Sensory processing enhancement\n\n";
        file << "## Building\n\n";
        file << "```bash\n";
        file << "mkdir build\n";
        file << "cd " << pluginName << "\n";
        file << "cmake .. -DCMAKE_BUILD_TYPE=Release\n";
        file << "make -j4\n";
        file << "```\n\n";
        file << "## Usage\n\n";
        file << "1. Compile the plugin\n";
        file << "2. Configure NLM to load the plugin\n";
        file << "3. Start NLM with the plugin system enabled\n\n";
        file << "## Plugin Interface\n\n";
        file << "The plugin implements the Plugin interface and provides the following services:\n\n";
        file << "- " << extensionType << " service\n";
        file << "- " << extensionType << " event handlers\n";
        file << "- " << extensionType << " data processors\n\n";
        file << "## License\n\n";
        file << "MIT\n";
        
        file.close();
    }
};

} // namespace nlm
