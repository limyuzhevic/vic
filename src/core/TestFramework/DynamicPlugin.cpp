#include "PluginManager.hpp"
#include <stdexcept>
#include <filesystem>
#include <algorithm>

namespace nlm {

DynamicPlugin::DynamicPlugin(const std::string& libraryPath) 
    : libraryPath_(libraryPath), handle_(nullptr), initialized_(false) {
    // Check if library file exists
    if (!std::filesystem::exists(libraryPath_)) {
        throw std::runtime_error("Plugin library does not exist: " + libraryPath_);
    }
}

DynamicPlugin::~DynamicPlugin() {
    shutdown();
    unloadLibrary();
}

bool DynamicPlugin::initialize(const Config& config) {
    if (!loadLibrary()) {
        return false;
    }
    
    if (!initialized_) {
        // Try to initialize plugin
        if (initializePlugin_) {
            initialized_ = initializePlugin_(this, config);
        } else {
            // If no initialize function, assume plugin is initialized
            initialized_ = true;
        }
    }
    
    return initialized_;
}

void DynamicPlugin::shutdown() {
    if (initialized_ && destroyPlugin_) {
        destroyPlugin_(this);
    }
    initialized_ = false;
}

std::string DynamicPlugin::getName() const {
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        std::string name = plugin->getName();
        destroyPlugin_(plugin);
        return name;
    }
    return "Unknown Plugin";
}

std::string DynamicPlugin::getVersion() const {
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        std::string version = plugin->getVersion();
        destroyPlugin_(plugin);
        return version;
    }
    return "1.0.0";
}

std::string DynamicPlugin::getDescription() const {
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        std::string description = plugin->getDescription();
        destroyPlugin_(plugin);
        return description;
    }
    return "Dynamic plugin";
}

std::string DynamicPlugin::getAuthor() const {
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        std::string author = plugin->getAuthor();
        destroyPlugin_(plugin);
        return author;
    }
    return "Unknown Author";
}

std::string DynamicPlugin::getContact() const {
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        std::string contact = plugin->getContact();
        destroyPlugin_(plugin);
        return contact;
    }
    return "unknown@example.com";
}

std::vector<std::unique_ptr<TestBase>> DynamicPlugin::createTests() {
    std::vector<std::unique_ptr<TestBase>> tests;
    
    if (getTests_) {
        TestPlugin* plugin = createPlugin_();
        tests = getTests_(plugin);
        destroyPlugin_(plugin);
    }
    
    return tests;
}

bool DynamicPlugin::canHandle(const std::string& testType) const {
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        bool canHandle = plugin->canHandle(testType);
        destroyPlugin_(plugin);
        return canHandle;
    }
    return false;
}

std::vector<std::string> DynamicPlugin::getSupportedTestTypes() const {
    std::vector<std::string> types;
    
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        types = plugin->getSupportedTestTypes();
        destroyPlugin_(plugin);
    }
    
    return types;
}

std::vector<std::string> DynamicPlugin::getSupportedCategories() const {
    std::vector<std::string> categories;
    
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        categories = plugin->getSupportedCategories();
        destroyPlugin_(plugin);
    }
    
    return categories;
}

std::vector<std::string> DynamicPlugin::getSupportedPhases() const {
    std::vector<std::string> phases;
    
    if (createPlugin_) {
        TestPlugin* plugin = createPlugin_();
        phases = plugin->getSupportedPhases();
        destroyPlugin_(plugin);
    }
    
    return phases;
}

void DynamicPlugin::loadLibrary() {
    if (handle_ != nullptr) {
        return; // Already loaded
    }
    
    // Load the dynamic library
    handle_ = dlopen(libraryPath_.c_str(), RTLD_LAZY);
    if (!handle_) {
        throw std::runtime_error("Failed to load plugin library " + libraryPath_ + ": " + dlerror());
    }
    
    // Get function pointers
    createPlugin_ = reinterpret_cast<CreatePluginFunc>(dlsym(handle_, "create_plugin"));
    if (!createPlugin_) {
        throw std::runtime_error("Failed to find create_plugin function in " + libraryPath_ + ": " + dlerror());
    }
    
    destroyPlugin_ = reinterpret_cast<DestroyPluginFunc>(dlsym(handle_, "destroy_plugin"));
    if (!destroyPlugin_) {
        throw std::runtime_error("Failed to find destroy_plugin function in " + libraryPath_ + ": " + dlerror());
    }
    
    initializePlugin_ = reinterpret_cast<InitializePluginFunc>(dlsym(handle_, "initialize_plugin"));
    if (!initializePlugin_) {
        throw std::runtime_error("Failed to find initialize_plugin function in " + libraryPath_ + ": " + dlerror());
    }
    
    getTests_ = reinterpret_cast<GetTestsFunc>(dlsym(handle_, "get_tests"));
    if (!getTests_) {
        throw std::runtime_error("Failed to find get_tests function in " + libraryPath_ + ": " + dlerror());
    }
}

void DynamicPlugin::unloadLibrary() {
    if (handle_) {
        dlclose(handle_);
        handle_ = nullptr;
        createPlugin_ = nullptr;
        destroyPlugin_ = nullptr;
        initializePlugin_ = nullptr;
        getTests_ = nullptr;
    }
}

} // namespace nlm
