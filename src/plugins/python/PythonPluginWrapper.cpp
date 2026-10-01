// PythonPluginWrapper.cpp
// Python plugin wrapper for C++ plugins implementation

#include "PluginLoader.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace nlm {

    PythonPluginWrapper::PythonPluginWrapper(const std::string& moduleName, const pybind11::module_& module)
        : moduleName_(moduleName), module_(module), state_(PluginState::UNLOADED) {
    }

    PythonPluginWrapper::~PythonPluginWrapper() {
    try {
        unload();
    } catch (...) {
        // Ignore exceptions during destruction
    }
    }

    const PluginMetadata& PythonPluginWrapper::getMetadata() const {
        return metadata_;
    }

    PluginState PythonPluginWrapper::getState() const {
        return state_;
    }

    bool PythonPluginWrapper::load() {
        if (state_ != PluginState::UNLOADED) {
            return false;
        }

        try {
            // Check if module has load function
            if (module_.attr("__dict__").contains("load")) {
                pybind11::function loadFunc = module_.attr("load");
                if (!pybind11::none().is(loadFunc)) {
                    loadFunc();
                }
            }

            state_ = PluginState::LOADED;
            return true;
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Load error: ") + pybind11::str(e.what());
            state_ = PluginState::ERROR;
            return false;
        }
    }

    bool PythonPluginWrapper::unload() {
        if (state_ == PluginState::UNLOADED || state_ == PluginState::ERROR) {
            return true;
        }

        try {
            // Check if module has unload function
            if (module_.attr("__dict__").contains("unload")) {
                pybind11::function unloadFunc = module_.attr("unload");
                if (!pybind11::none().is(unloadFunc)) {
                    unloadFunc();
                }
            }

            state_ = PluginState::UNLOADED;
            return true;
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Unload error: ") + pybind11::str(e.what());
            state_ = PluginState::ERROR;
            return false;
        }
    }

    bool PythonPluginWrapper::configure(const std::map<std::string, std::any>& config) {
        if (state_ != PluginState::LOADED && state_ != PluginState::CONFIGURED) {
            return false;
        }

        try {
            // Convert C++ config to Python dict
            pybind11::dict pyConfig = anyToPyDict(config);

            // Check if module has configure function
            if (module_.attr("__dict__").contains("configure")) {
                pybind11::function configureFunc = module_.attr("configure");
                if (!pybind11::none().is(configureFunc)) {
                    configureFunc(pyConfig);
                }
            }

            config_ = config;
            state_ = PluginState::CONFIGURED;
            return true;
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Configure error: ") + pybind11::str(e.what());
            state_ = PluginState::ERROR;
            return false;
        }
    }

    bool PythonPluginWrapper::initialize() {
        if (state_ != PluginState::CONFIGURED) {
            return false;
        }

        try {
            // Check if module has initialize function
            if (module_.attr("__dict__").contains("initialize")) {
                pybind11::function initializeFunc = module_.attr("initialize");
                if (!pybind11::none().is(initializeFunc)) {
                    initializeFunc();
                }
            }

            state_ = PluginState::RUNNING;
            return true;
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Initialize error: ") + pybind11::str(e.what());
            state_ = PluginState::ERROR;
            return false;
        }
    }

    bool PythonPluginWrapper::start() {
        if (state_ != PluginState::RUNNING) {
            return false;
        }

        try {
            // Check if module has start function
            if (module_.attr("__dict__").contains("start")) {
                pybind11::function startFunc = module_.attr("start");
                if (!pybind11::none().is(startFunc)) {
                    startFunc();
                }
            }

            state_ = PluginState::RUNNING;
            return true;
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Start error: ") + pybind11::str(e.what());
            state_ = PluginState::ERROR;
            return false;
        }
    }

    bool PythonPluginWrapper::stop() {
        if (state_ != PluginState::RUNNING) {
            return false;
        }

        try {
            // Check if module has stop function
            if (module_.attr("__dict__").contains("stop")) {
                pybind11::function stopFunc = module_.attr("stop");
                if (!pybind11::none().is(stopFunc)) {
                    stopFunc();
                }
            }

            state_ = PluginState::STOPPED;
            return true;
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Stop error: ") + pybind11::str(e.what());
            state_ = PluginState::ERROR;
            return false;
        }
    }

    std::vector<std::string> PythonPluginWrapper::getRequiredInterfaces() const {
        std::vector<std::string> result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_required_interfaces")) {
                pybind11::function func = dict["get_required_interfaces"];
                if (!pybind11::none().is(func)) {
                    pybind11::object obj = func();
                    if (!obj.is_none()) {
                        result = obj.cast<std::vector<std::string>>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    std::vector<std::string> PythonPluginWrapper::getProvidedInterfaces() const {
        std::vector<std::string> result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_provided_interfaces")) {
                pybind11::function func = dict["get_provided_interfaces"];
                if (!pybind11::none().is(func)) {
                    pybind11::object obj = func();
                    if (!obj.is_none()) {
                        result = obj.cast<std::vector<std::string>>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    void PythonPluginWrapper::onEvent(const std::string& eventType, const std::any& eventData) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("on_event")) {
                pybind11::function func = dict["on_event"];
                if (!pybind11::none().is(func)) {
                    pybind11::dict pyEventData;
                    if (eventData.type() == typeid(std::map<std::string, std::any>)) {
                        auto& mapData = std::any_cast<const std::map<std::string, std::any>&>(eventData);
                        pyEventData = anyToPyDict(mapData);
                    }
                    
                    func(eventType, pyEventData);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Event error: ") + pybind11::str(e.what());
        }
    }

    void PythonPluginWrapper::onPluginLoaded(std::shared_ptr<PluginInterface> plugin) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("on_plugin_loaded")) {
                pybind11::function func = dict["on_plugin_loaded"];
                if (!pybind11::none().is(func)) {
                    func();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Plugin loaded error: ") + pybind11::str(e.what());
        }
    }

    void PythonPluginWrapper::onPluginUnloaded(std::shared_ptr<PluginInterface> plugin) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("on_plugin_unloaded")) {
                pybind11::function func = dict["on_plugin_unloaded"];
                if (!pybind11::none().is(func)) {
                    func();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Plugin unloaded error: ") + pybind11::str(e.what());
        }
    }

    bool PythonPluginWrapper::validate() {
        try {
            // Check if module has validate function
            if (module_.attr("__dict__").contains("validate")) {
                pybind11::function func = module_.attr("validate");
                if (!pybind11::none().is(func)) {
                    return func();
                }
            }
            
            // Basic validation: check if metadata is available
            return !metadata_.name.empty();
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Validation error: ") + pybind11::str(e.what());
            return false;
        }
    }

    std::string PythonPluginWrapper::getError() const {
        return error_;
    }

    void PythonPluginWrapper::reset() {
        try {
            // Check if module has reset function
            if (module_.attr("__dict__").contains("reset")) {
                pybind11::function func = module_.attr("reset");
                if (!pybind11::none().is(func)) {
                    func();
                }
            }
            
            error_.clear();
            config_.clear();
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Reset error: ") + pybind11::str(e.what());
        }
    }

    // Python-specific methods
    pybind11::dict PythonPluginWrapper::getPluginInfo() const {
        pybind11::dict result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_plugin_info")) {
                pybind11::function func = dict["get_plugin_info"];
                if (!pybind11::none().is(func)) {
                    result = func().cast<pybind11::dict>();
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    pybind11::list PythonPluginWrapper::getPluginDependencies() const {
        pybind11::list result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_plugin_dependencies")) {
                pybind11::function func = dict["get_plugin_dependencies"];
                if (!pybind11::none().is(func)) {
                    pybind11::object obj = func();
                    if (!obj.is_none()) {
                        result = obj.cast<pybind11::list>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    pybind11::list PythonPluginWrapper::getPluginCapabilities() const {
        pybind11::list result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_plugin_capabilities")) {
                pybind11::function func = dict["get_plugin_capabilities"];
                if (!pybind11::none().is(func)) {
                    pybind11::object obj = func();
                    if (!obj.is_none()) {
                        result = obj.cast<pybind11::list>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    pybind11::dict PythonPluginWrapper::getPluginConfig() const {
        return anyToPyDict(config_);
    }

    // Neural component plugin interface
    bool PythonPluginWrapper::registerComponent(const std::string& name, const std::any& component) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("register_component")) {
                pybind11::function func = dict["register_component"];
                if (!pybind11::none().is(func)) {
                    func(name, component);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Register component error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::unregisterComponent(const std::string& name) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("unregister_component")) {
                pybind11::function func = dict["unregister_component"];
                if (!pybind11::none().is(func)) {
                    func(name);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Unregister component error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    std::any PythonPluginWrapper::getComponent(const std::string& name) const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_component")) {
                pybind11::function func = dict["get_component"];
                if (!pybind11::none().is(func)) {
                    return func(name);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Get component error: ") + pybind11::str(e.what());
        }
        
        return std::any();
    }

    std::vector<std::string> PythonPluginWrapper::getComponentNames() const {
        std::vector<std::string> result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_component_names")) {
                pybind11::function func = dict["get_component_names"];
                if (!pybind11::none().is(func)) {
                    pybind11::object obj = func();
                    if (!obj.is_none()) {
                        result = obj.cast<std::vector<std::string>>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    bool PythonPluginWrapper::hasComponent(const std::string& name) const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("has_component")) {
                pybind11::function func = dict["has_component"];
                if (!pybind11::none().is(func)) {
                    return func(name);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Has component error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    // Memory plugin interface
    bool PythonPluginWrapper::store(const std::string& key, const std::any& value, int ttl) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("store")) {
                pybind11::function func = dict["store"];
                if (!pybind11::none().is(func)) {
                    func(key, value, ttl);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Store error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::retrieve(const std::string& key, std::any& value) const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("retrieve")) {
                pybind11::function func = dict["retrieve"];
                if (!pybind11::none().is(func)) {
                    value = func(key);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Retrieve error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::remove(const std::string& key) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("remove")) {
                pybind11::function func = dict["remove"];
                if (!pybind11::none().is(func)) {
                    func(key);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Remove error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::clear() {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("clear")) {
                pybind11::function func = dict["clear"];
                if (!pybind11::none().is(func)) {
                    func();
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Clear error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::exists(const std::string& key) const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("exists")) {
                pybind11::function func = dict["exists"];
                if (!pybind11::none().is(func)) {
                    return func(key);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Exists error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    size_t PythonPluginWrapper::size() const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("size")) {
                pybind11::function func = dict["size"];
                if (!pybind11::none().is(func)) {
                    return func().cast<size_t>();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Size error: ") + pybind11::str(e.what());
        }
        
        return 0;
    }

    std::vector<std::string> PythonPluginWrapper::getKeys() const {
        std::vector<std::string> result;
        
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_keys")) {
                pybind11::function func = dict["get_keys"];
                if (!pybind11::none().is(func)) {
                    pybind11::object obj = func();
                    if (!obj.is_none()) {
                        result = obj.cast<std::vector<std::string>>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    bool PythonPluginWrapper::cleanup() {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("cleanup")) {
                pybind11::function func = dict["cleanup"];
                if (!pybind11::none().is(func)) {
                    return func();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Cleanup error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::compact() {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("compact")) {
                pybind11::function func = dict["compact"];
                if (!pybind11::none().is(func)) {
                    return func();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Compact error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    // Processing plugin interface
    bool PythonPluginWrapper::process(const std::any& input, std::any& output) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("process")) {
                pybind11::function func = dict["process"];
                if (!pybind11::none().is(func)) {
                    output = func(input);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Process error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::preProcess(const std::any& input) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("pre_process")) {
                pybind11::function func = dict["pre_process"];
                if (!pybind11::none().is(func)) {
                    return func(input);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Pre-process error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::postProcess(const std::any& output) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("post_process")) {
                pybind11::function func = dict["post_process"];
                if (!pybind11::none().is(func)) {
                    return func(output);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Post-process error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::setInputSchema(const std::string& schema) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("set_input_schema")) {
                pybind11::function func = dict["set_input_schema"];
                if (!pybind11::none().is(func)) {
                    return func(schema);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Set input schema error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    bool PythonPluginWrapper::setOutputSchema(const std::string& schema) {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("set_output_schema")) {
                pybind11::function func = dict["set_output_schema"];
                if (!pybind11::none().is(func)) {
                    return func(schema);
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Set output schema error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    std::string PythonPluginWrapper::getInputSchema() const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_input_schema")) {
                pybind11::function func = dict["get_input_schema"];
                if (!pybind11::none().is(func)) {
                    return func().cast<std::string>();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Get input schema error: ") + pybind11::str(e.what());
        }
        
        return "";
    }

    std::string PythonPluginWrapper::getOutputSchema() const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_output_schema")) {
                pybind11::function func = dict["get_output_schema"];
                if (!pybind11::none().is(func)) {
                    return func().cast<std::string>();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Get output schema error: ") + pybind11::str(e.what());
        }
        
        return "";
    }

    bool PythonPluginWrapper::getStats(std::map<std::string, std::any>& stats) const {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_stats")) {
                pybind11::function func = dict["get_stats"];
                if (!pybind11::none().is(func)) {
                    pybind11::dict pyStats = func().cast<pybind11::dict>();
                    stats = pyDictToAny(pyStats);
                    return true;
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Get stats error: ") + pybind11::str(e.what());
        }
        
        return false;
    }

    void PythonPluginWrapper::resetStats() {
        try {
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("reset_stats")) {
                pybind11::function func = dict["reset_stats"];
                if (!pybind11::none().is(func)) {
                    func();
                }
            }
        } catch (const pybind11::error_already_set& e) {
            error_ = std::string("Reset stats error: ") + pybind11::str(e.what());
        }
    }

    bool PythonPluginWrapper::validatePythonPlugin() const {
        try {
            // Basic validation
            if (moduleName_.empty()) {
                return false;
            }
            
            // Check if module has required attributes
            pybind11::dict dict = module_.attr("__dict__");
            
            if (dict.contains("get_plugin_metadata")) {
                pybind11::function func = dict["get_plugin_metadata"];
                if (!pybind11::none().is(func)) {
                    pybind11::object metadataObj = func();
                    if (!metadataObj.is_none()) {
                        // Extract metadata
                        pybind11::dict metaDict = metadataObj.cast<pybind11::dict>();
                        
                        if (metaDict.contains("name") && !pybind11::none().is(metaDict["name"])) {
                            return true;
                        }
                    }
                }
            }
            
            return false;
        } catch (const pybind11::error_already_set& e) {
            return false;
        }
    }

    pybind11::dict PythonPluginWrapper::anyToPyDict(const std::map<std::string, std::any>& config) const {
        pybind11::dict result;
        
        for (const auto& pair : config) {
            result[pair.first] = pair.second;
        }
        
        return result;
    }

} // namespace nlm