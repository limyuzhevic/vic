#include "ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

std::exception_ptr ErrorHandler::convertToStandardException(const NLMException& nlmException) {
    // Convert NLM exception to appropriate standard exception type
    switch (nlmException.getCategory()) {
        case NLMErrorCategory::ConfigError:
            // Re-throw as std::runtime_error for config errors
            return std::make_exception_ptr(std::runtime_error(nlmException.what()));
        case NLMErrorCategory::MemoryError:
            // Re-throw as std::bad_alloc for memory errors
            return std::make_exception_ptr(std::bad_alloc(nlmException.what()));
        case NLMErrorCategory::NeuralError:
            // Re-throw as custom neural exception
            return std::make_exception_ptr(std::runtime_error("Neural Error: " + std::string(nlmException.what())));
        default:
            // Default to runtime_error
            return std::make_exception_ptr(std::runtime_error(nlmException.what()));
    }
}

void ErrorHandler::logError(const std::string& errorMessage, NLMErrorCategory category) {
    std::string level;
    
    switch (category) {
        case NLMErrorCategory::ConfigError:
        case NLMErrorCategory::MemoryError:
            level = "ERROR";
            break;
        case NLMErrorCategory::NeuralError:
        case NLMErrorCategory::PlasticityError:
            level = "WARNING";
            break;
        case NLMErrorCategory::IntegrationError:
            level = "INFO";
            break;
        default:
            level = "ERROR";
            break;
    }
    
    NLM_LOG_LEVEL(level.c_str()) << "NLM " << level << ": " << errorMessage;
}

void ErrorHandler::validateFileOperation(const std::string& filepath, bool operationSuccessful, 
                                       const std::string& operationName) {
    if (!operationSuccessful) {
        throw ConfigException(std::format("Failed to {0} file: {1}", operationName, filepath), 
                            ConfigErrorCode::FileNotFound);
    }
}

} // namespace nlm