#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>

namespace py = pybind11;
namespace nlm {

/**
 * ModuleRegistrar base class for automatic class registration
 * Classes that want to be exposed to Python should inherit from this class
 * and call REGISTER_MODULE_CLASS in their constructor or initialization.
 */
class ModuleRegistrar {
public:
    template<typename T>
    static void registerClass(py::module_& m, const std::string& name = "") {
        PythonBindingSystem::register_class_internal<T>(m, name);
    }
    
    template<typename T>
    static void registerFunction(py::module_& m, const std::string& name) {
        PythonBindingSystem::register_function_internal<T>(m, name);
    }
    
    template<typename T>
    static void registerCustomCommand(py::module_& m, const std::string& name) {
        PythonBindingSystem::register_custom_command_internal<T>(m, name);
    }
protected:
    ModuleRegistrar() {
        // Register this class with the binding system
        onRegister();
    }
    
    virtual ~ModuleRegistrar() = default;
    
    virtual void onRegister() {
        // Override in derived classes to specify registration
    }
};

/**
 * CustomCommand class for user-defined Python commands
 * Allows Python users to register their own commands at runtime
 */
class CustomCommand {
public:
    virtual ~CustomCommand() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual py::object execute(py::args args, py::kwargs kwargs) = 0;
};

/**
 * Template CustomCommand for user convenience
 */
template<typename Func>
class TemplateCustomCommand : public CustomCommand {
public:
    TemplateCustomCommand(Func func, const std::string& name, const std::string& description)
        : func_(func), name_(name), description_(description) {}
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    py::object execute(py::args args, py::kwargs kwargs) override {
        return func_(args, kwargs);
    }
    
private:
    Func func_;
    std::string name_;
    std::string description_;
};

/**
 * PythonBindingSystem class - manages pybind11 module creation and registration
 * Provides automatic registration for all registered classes and custom commands
 */
class PythonBindingSystem {
public:
    PythonBindingSystem() = default;
    ~PythonBindingSystem() = default;
    
    // Initialize the pybind11 module
    py::module_ initializeModule(const std::string& moduleName, const std::string& docString = "") {
        module_ = py::module_::create_extension_module(moduleName, moduleName.c_str(), docString);
        return module_;
    }
    
    // Register a class with pybind11
    template<typename T>
    void registerClass(py::module_& m, const std::string& name = "") {
        std::string className = name.empty() ? getClassName<T>() : name;
        
        // Store registration info
        ClassRegistration reg;
        reg.type = std::type_index(typeid(T));
        reg.className = className;
        reg.registrationFunc = [this, &m, className]() {
            register_class_impl<T>(m, className);
        };
        
        registrations_.push_back(reg);
    }
    
    // Register a function with pybind11
    template<typename T>
    void registerFunction(py::module_& m, const std::string& name) {
        FunctionRegistration reg;
        reg.type = std::type_index(typeid(T));
        reg.name = name;
        reg.registrationFunc = [this, &m, name]() {
            register_function_impl<T>(m, name);
        };
        
        functionRegistrations_.push_back(reg);
    }
    
    // Register a custom command
    void registerCustomCommand(std::unique_ptr<CustomCommand> command) {
        customCommands_.push_back(std::move(command));
    }
    
    // Execute all registrations (call after initializeModule)
    void executeRegistrations(py::module_& m) {
        for (const auto& reg : registrations_) {
            reg.registrationFunc();
        }
        
        for (const auto& reg : functionRegistrations_) {
            reg.registrationFunc();
        }
        
        // Register custom commands as module attributes
        for (const auto& cmd : customCommands_) {
            m.def(cmd->getName(), &CustomCommand::execute, 
                  cmd->getDescription().c_str(), py::arg("args"), py::arg("kwargs"));
        }
    }
    
    // Register exceptions
    template<typename T>
    void registerException(py::module_& m, const std::string& name = "") {
        std::string exceptionName = name.empty() ? getClassName<T>() : name;
        py::register_exception<std::runtime_error>(m, exceptionName.c_str());
    }
    
    // Get the module
    py::module_& getModule() { return module_; }
    
    // Helper to get class name
    template<typename T>
    static std::string getClassName() {
        return typeid(T).name();
    }
    
private:
    // Internal registration implementations
    template<typename T>
    void register_class_internal(py::module_& m, const std::string& name) {
        register_class_impl<T>(m, name);
    }
    
    template<typename T>
    void register_function_internal(py::module_& m, const std::string& name) {
        register_function_impl<T>(m, name);
    }
    
    template<typename T>
    void register_custom_command_internal(py::module_& m, const std::string& name) {
        // Forward to the custom command registration
    }
    
    // Concrete implementations
    template<typename T>
    void register_class_impl(py::module_& m, const std::string& name) {
        // This is where individual class registrations would go
        // For now, we just store the registration info
    }
    
    template<typename T>
    void register_function_impl(py::module_& m, const std::string& name) {
        // This is where individual function registrations would go
        // For now, we just store the registration info
    }
    
    struct ClassRegistration {
        std::type_index type;
        std::string className;
        std::function<void()> registrationFunc;
    };
    
    struct FunctionRegistration {
        std::type_index type;
        std::string name;
        std::function<void()> registrationFunc;
    };
    
    std::vector<ClassRegistration> registrations_;
    std::vector<FunctionRegistration> functionRegistrations_;
    std::vector<std::unique_ptr<CustomCommand>> customCommands_;
    py::module_ module_;
};

// Macro for easy class registration
#define REGISTER_CLASS(system, classType, className) \
    system.registerClass<classType>(system.getModule(), className)

#define REGISTER_FUNCTION(system, functionType, functionName) \
    system.registerFunction<functionType>(system.getModule(), functionName)

#define REGISTER_CUSTOM_COMMAND(system, command) \
    system.registerCustomCommand(std::make_unique<decltype(command)>(command))

// Global instance of the binding system
inline PythonBindingSystem& getBindingSystem() {
    static PythonBindingSystem instance;
    return instance;
}

// Helper function to register a class with the system
inline void registerModuleClass(py::module_& m, const std::string& name = "") {
    // This will be called from the binding system
}

// Custom command helper template
inline auto makeCustomCommand(py::module_& m, const std::string& name, 
                             const std::string& description, 
                             std::function<py::object(py::args, py::kwargs)> func) {
    return TemplateCustomCommand<std::function<py::object(py::args, py::kwargs)>>(
        func, name, description);
}

} // namespace nlm