// Include the header
#include "ErrorHandling.hpp"
#include <pybind11/pybind11.h>
#include <sstream>
#include <iomanip>

namespace nlm {

// Global error manager instance
GlobalErrorManager& getGlobalErrorManager() {
    return GlobalErrorManager::getInstance();
}

// Initialize error handling
void initializeErrorHandling(std::shared_ptr<Config> config) {
    getGlobalErrorManager().initialize(config);
    setupPythonErrorHandling();
}

void initializeErrorHandling() {
    initializeErrorHandling(nullptr);
}

void shutdownErrorHandling() {
    getGlobalErrorManager().shutdown();
}

void reportError(const std::exception& e) {
    getGlobalErrorManager().handleError(e);
}

void reportError(const std::string& message, const std::string& component) {
    getGlobalErrorManager().handleError(message, component);
}

void reportError(const std::string& message, const std::string& context,
                 const std::string& component) {
    getGlobalErrorManager().handleError(message, context, component);
}

void enableErrorReporting(bool enable) {
    getGlobalErrorManager().enableErrorReporting(enable);
}

bool isErrorReportingEnabled() {
    return getGlobalErrorManager().isErrorReportingEnabled();
}

void setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    getGlobalErrorManager().setErrorHandler(handler);
}

std::shared_ptr<IErrorHandler> getCurrentErrorHandler() {
    return getGlobalErrorManager().getCurrentErrorHandler();
}

std::string getErrorReport() {
    return getGlobalErrorManager().getErrorReport();
}

void addCustomErrorHandler(const std::string& name, std::shared_ptr<IErrorHandler> handler) {
    getGlobalErrorManager().addCustomErrorHandler(name, handler);
}

bool removeCustomErrorHandler(const std::string& name) {
    return getGlobalErrorManager().removeCustomErrorHandler(name);
}

std::vector<std::string> getCustomErrorHandlerNames() {
    return getGlobalErrorManager().getCustomErrorHandlerNames();
}

bool handleWithCustomErrorHandler(const std::string& name, const std::exception& e) {
    return getGlobalErrorManager().handleWithCustomErrorHandler(name, e);
}

std::vector<std::string> getRecentErrors() {
    return getGlobalErrorManager().getRecentErrors();
}

void clearErrorHistory() {
    getGlobalErrorManager().clearErrorHistory();
}

double getErrorRecoveryProgress() {
    return getGlobalErrorManager().getErrorRecoveryProgress();
}

bool isInCriticalState() {
    return getGlobalErrorManager().isInCriticalState();
}

// Python-specific error handling functions
void setupPythonErrorHandling() {
    try {
        py::module m = py::module::import("pynlm");
        
        py::register_exception<NLMException>(m, "NLMException");
        py::register_exception<ConfigException>(m, "ConfigException");
        py::register_exception<BrainInitException>(m, "BrainInitException");
        py::register_exception<SimulationException>(m, "SimulationException");
        py::register_exception<AgentException>(m, "AgentException");
        py::register_exception<WorldException>(m, "WorldException");
        py::register_exception<PythonBindingException>(m, "PythonBindingException");
        
        // Add Python-specific error info
        auto nlm_exc = py::cast<NLMException>(NLMException("Test", "", ""));
        m.attr("NLM_ERROR_TYPE") = "NLMException";
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to setup Python error handling: " + std::string(e.what()));
    }
}

// Function to create custom error handlers for specific scenarios
std::shared_ptr<IErrorHandler> createBrainErrorHandler() {
    class BrainErrorHandler : public IErrorHandler {
    public:
        bool canHandle(const std::exception& e) const override {
            return dynamic_cast<const BrainInitException*>(&e) != nullptr;
        }
        
        bool handle(const std::exception& e, 
                   std::shared_ptr<ErrorRecoveryContext> context) override {
            if (context) {
                context->addError(e.what());
                context->recoveryInProgress = true;
                
                // For brain initialization errors, try to reset
                NLM_LOG_INFO("Brain initialization error - attempting recovery");
                context->currentState = "brain_recovery_attempted";
                context->recoveryInProgress = false;
                return true;
            }
            return false;
        }
        
        RecoveryStrategy getDefaultStrategy() const override {
            return RecoveryStrategy::ResetState;
        }
        
        std::string getName() const override {
            return "BrainErrorHandler";
        }
        
        std::string getDescription() const override {
            return "Error handler specialized for brain initialization errors";
        }
    };
    
    return std::make_shared<BrainErrorHandler>();
}

std::shared_ptr<IErrorHandler> createSimulationErrorHandler() {
    class SimulationErrorHandler : public IErrorHandler {
    public:
        bool canHandle(const std::exception& e) const override {
            return dynamic_cast<const SimulationException*>(&e) != nullptr;
        }
        
        bool handle(const std::exception& e, 
                   std::shared_ptr<ErrorRecoveryContext> context) override {
            if (context) {
                context->addError(e.what());
                context->recoveryInProgress = true;
                
                // For simulation errors, try to roll back
                auto simError = dynamic_cast<const SimulationException&>(e);
                uint64_t step = simError.getStep();
                
                NLM_LOG_INFO("Simulation error at step " + std::to_string(step) + " - attempting rollback");
                context->currentState = "simulation_rollback_attempted";
                context->recoveryInProgress = false;
                return true;
            }
            return false;
        }
        
        RecoveryStrategy getDefaultStrategy() const override {
            return RecoveryStrategy::RollbackStep;
        }
        
        std::string getName() const override {
            return "SimulationErrorHandler";
        }
        
        std::string getDescription() const override {
            return "Error handler specialized for simulation errors";
        }
    };
    
    return std::make_shared<SimulationErrorHandler>();
}

std::shared_ptr<IErrorHandler> createConfigErrorHandler() {
    class ConfigErrorHandler : public IErrorHandler {
    public:
        bool canHandle(const std::exception& e) const override {
            return dynamic_cast<const ConfigException*>(&e) != nullptr;
        }
        
        bool handle(const std::exception& e, 
                   std::shared_ptr<ErrorRecoveryContext> context) override {
            if (context) {
                context->addError(e.what());
                context->recoveryInProgress = true;
                
                // For configuration errors, try to restore from backup
                NLM_LOG_INFO("Configuration error - attempting recovery");
                context->currentState = "config_recovery_attempted";
                context->recoveryInProgress = false;
                return true;
            }
            return false;
        }
        
        RecoveryStrategy getDefaultStrategy() const override {
            return RecoveryStrategy::ConfigurationRecovery;
        }
        
        std::string getName() const override {
            return "ConfigErrorHandler";
        }
        
        std::string getDescription() const override {
            return "Error handler specialized for configuration errors";
        }
    };
    
    return std::make_shared<ConfigErrorHandler>();
}

std::shared_ptr<IErrorHandler> createPythonBindingErrorHandler() {
    class PythonBindingErrorHandler : public IErrorHandler {
    public:
        bool canHandle(const std::exception& e) const override {
            return dynamic_cast<const PythonBindingException*>(&e) != nullptr;
        }
        
        bool handle(const std::exception& e, 
                   std::shared_ptr<ErrorRecoveryContext> context) override {
            if (context) {
                context->addError(e.what());
                context->recoveryInProgress = true;
                
                // For Python binding errors, require manual intervention
                auto pyError = dynamic_cast<const PythonBindingException&>(e);
                std::string function = pyError.getDetails();
                
                NLM_LOG_INFO("Python binding error in function '" + function + "' - manual recovery required");
                context->currentState = "manual_recovery_required";
                context->recoveryInProgress = true;
                return false; // Cannot auto-recover
            }
            return false;
        }
        
        RecoveryStrategy getDefaultStrategy() const override {
            return RecoveryStrategy::ManualRecovery;
        }
        
        std::string getName() const override {
            return "PythonBindingErrorHandler";
        }
        
        std::string getDescription() const override {
            return "Error handler specialized for Python binding errors";
        }
    };
    
    return std::make_shared<PythonBindingErrorHandler>();
}

// Function to setup comprehensive error handling
void setupComprehensiveErrorHandling(std::shared_ptr<Config> config) {
    // Initialize global error manager
    initializeErrorHandling(config);
    
    // Create and add specialized error handlers
    auto brainHandler = createBrainErrorHandler();
    auto simulationHandler = createSimulationErrorHandler();
    auto configHandler = createConfigErrorHandler();
    auto pythonHandler = createPythonBindingErrorHandler();
    
    // Add them in order of priority
    getGlobalErrorManager().addCustomErrorHandler("brain", brainHandler);
    getGlobalErrorManager().addCustomErrorHandler("simulation", simulationHandler);
    getGlobalErrorManager().addCustomErrorHandler("config", configHandler);
    getGlobalErrorManager().addCustomErrorHandler("python", pythonHandler);
    
    // Set up default error handler
    auto basicHandler = std::make_shared<BasicErrorHandler>();
    getGlobalErrorManager().setErrorHandler(basicHandler);
    
    NLM_LOG_INFO("Comprehensive error handling setup completed");
}

// Function to get error statistics
py::dict getErrorStatistics() {
    auto context = getGlobalErrorManager().getRecoveryContext();
    
    py::dict stats;
    stats["total_errors"] = context->errorCount;
    stats["recent_errors_count"] = context->recentErrors.size();
    stats["recovery_progress"] = context->getRecoveryProgress();
    stats["critical_state"] = getGlobalErrorManager().isInCriticalState();
    stats["error_reporting_enabled"] = isErrorReportingEnabled();
    
    // Count errors by type
    std::unordered_map<std::string, int> errorCounts;
    for (const auto& error : context->recentErrors) {
        // Extract error type (simplified)
        if (error.find("ConfigException") != std::string::npos) {
            errorCounts["config"]++;
        } else if (error.find("BrainInitException") != std::string::npos) {
            errorCounts["brain_init"]++;
        } else if (error.find("SimulationException") != std::string::npos) {
            errorCounts["simulation"]++;
        } else if (error.find("AgentException") != std::string::npos) {
            errorCounts["agent"]++;
        } else if (error.find("WorldException") != std::string::npos) {
            errorCounts["world"]++;
        } else if (error.find("PythonBindingException") != std::string::npos) {
            errorCounts["python"]++;
        } else {
            errorCounts["other"]++;
        }
    }
    
    py::dict errorTypeStats;
    for (const auto& pair : errorCounts) {
        errorTypeStats[pair.first] = pair.second;
    }
    stats["error_types"] = errorTypeStats;
    
    return stats;
}

// Function to export error history
std::string exportErrorHistory() {
    auto context = getGlobalErrorManager().getRecoveryContext();
    std::stringstream ss;
    
    ss << "=== NLM Error History ===\n";
    ss << "Total errors: " << context->errorCount << "\n";
    ss << "Session ID: " << context->sessionId << "\n";
    ss << "Start time: " << formatTime(context->startTime) << "\n";
    ss << "Current state: " << context->currentState << "\n";
    ss << "Recovery progress: " << context->getRecoveryProgress() * 100 << "%\n";
    ss << "\nRecent Errors:\n";
    
    for (size_t i = 0; i < std::min(context->recentErrors.size(), static_cast<size_t>(50)); ++i) {
        ss << (i + 1) << ". " << context->recentErrors[i] << "\n";
    }
    
    return ss.str();
}

std::string formatTime(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Function to clear error history
void clearErrorHistory() {
    getGlobalErrorManager().clearErrorHistory();
}

// Python-specific error handling for pybind11
void setupPythonErrorHandling(pybind11::module m) {
    // Register Python exceptions
    pybind11::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_RuntimeError, 
                                    ("NLM Error: " + e.getMessage()).c_str());
        } catch (const ConfigException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_ValueError,
                                    ("Config Error: " + e.getMessage()).c_str());
        } catch (const BrainInitException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_RuntimeError,
                                    ("Brain Init Error: " + e.getMessage()).c_str());
        } catch (const SimulationException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_RuntimeError,
                                    ("Simulation Error: " + e.getMessage()).c_str());
        } catch (const AgentException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_RuntimeError,
                                    ("Agent Error: " + e.getMessage()).c_str());
        } catch (const WorldException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_RuntimeError,
                                    ("World Error: " + e.getMessage()).c_str());
        } catch (const PythonBindingException& e) {
            pybind11::PyErr_SetString(pybind11::PyExc_RuntimeError,
                                    ("Python Binding Error: " + e.getMessage()).c_str());
        }
    });
}

} // namespace nlm
