#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

ErrorHandling::ErrorHandler ErrorHandling::globalErrorHandler_ = nullptr;

const std::unordered_map<ErrorCategory, std::string> ErrorHandling::categoryNames_ = {
    {ErrorCategory::Configuration, "Configuration"},
    {ErrorCategory::Resource, "Resource"},
    {ErrorCategory::Initialization, "Initialization"},
    {ErrorCategory::Validation, "Validation"},
    {ErrorCategory::Recovery, "Recovery"},
    {ErrorCategory::System, "System"}
};

const std::unordered_map<ErrorSeverity, std::string> ErrorHandling::severityNames_ = {
    {ErrorSeverity::Warning, "Warning"},
    {ErrorSeverity::Error, "Error"},
    {ErrorSeverity::Critical, "Critical"},
    {ErrorSeverity::Fatal, "Fatal"}
};

void ErrorHandling::registerErrorHandler(ErrorHandler handler) {
    globalErrorHandler_ = handler;
}

bool ErrorHandling::handleError(const ErrorInfo& error) {
    if (globalErrorHandler_) {
        globalErrorHandler_(error);
        return true;
    }
    return false;
}

std::string ErrorHandling::formatError(const ErrorInfo& error) {
    std::ostringstream oss;
    oss << "[" << severityNames_.at(error.severity) << "] "
        << "[" << categoryNames_.at(error.category) << "] "
        << error.message;
    if (!error.code.empty()) {
        oss << " (code: " << error.code << ")";
    }
    if (!error.context.empty()) {
        oss << "\nContext:";
        for (const auto& pair : error.context) {
            oss << "\n  " << pair.first << " = " << pair.second;
        }
    }
    return oss.str();
}

bool ErrorHandling::validateNeuronCount(size_t count, const std::string& context) {
    const size_t MIN_NEURONS = 10;
    const size_t MAX_NEURONS = 1000000;
    
    if (count < MIN_NEURONS) {
        ErrorInfo error("Neuron count too small: " + std::to_string(count) + 
                       ". Minimum recommended is " + std::to_string(MIN_NEURONS) +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Use default neuron count", "Use minimum viable count"};
        handleError(error);
        return false;
    }
    
    if (count > MAX_NEURONS) {
        ErrorInfo error("Neuron count too large: " + std::to_string(count) + 
                       ". Maximum recommended is " + std::to_string(MAX_NEURONS) +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Use maximum allowable count", "Reduce for performance"};
        handleError(error);
        return false;
    }
    
    return true;
}

bool ErrorHandling::validateSynapseWeight(float weight, const std::string& context) {
    const float MIN_WEIGHT = -10.0f;
    const float MAX_WEIGHT = 10.0f;
    
    if (weight < MIN_WEIGHT || weight > MAX_WEIGHT) {
        ErrorInfo error("Synapse weight out of range: " + std::to_string(weight) +
                       ". Valid range is [" + std::to_string(MIN_WEIGHT) + ", " +
                       std::to_string(MAX_WEIGHT) + "]" +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Warning);
        handleError(error);
        return false;
    }
    
    return true;
}

bool ErrorHandling::validateSimulationTimestep(double timestep, const std::string& context) {
    const double MIN_TIMESTEP = 1e-6;
    const double MAX_TIMESTEP = 0.1;
    
    if (timestep <= 0.0) {
        ErrorInfo error("Invalid timestep: " + std::to_string(timestep) +
                       ". Must be positive" +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Use default timestep"};
        handleError(error);
        return false;
    }
    
    if (timestep < MIN_TIMESTEP) {
        ErrorInfo error("Timestep too small: " + std::to_string(timestep) +
                       ". May cause numerical instability" +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Warning);
        handleError(error);
        return false;
    }
    
    if (timestep > MAX_TIMESTEP) {
        ErrorInfo error("Timestep too large: " + std::to_string(timestep) +
                       ". May cause simulation instability" +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Warning);
        handleError(error);
        return false;
    }
    
    return true;
}

bool ErrorHandling::validateConnectionProbability(float prob, const std::string& context) {
    if (prob < 0.0f || prob > 1.0f) {
        ErrorInfo error("Connection probability out of range [0, 1]: " + std::to_string(prob) +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Clamp probability to [0, 1]"};
        handleError(error);
        return false;
    }
    
    return true;
}

bool ErrorHandling::validateRegionCount(size_t count, const std::string& context) {
    const size_t MIN_REGIONS = 1;
    const size_t MAX_REGIONS = 1000;
    
    if (count < MIN_REGIONS) {
        ErrorInfo error("Region count too small: " + std::to_string(count) +
                       ". Minimum is " + std::to_string(MIN_REGIONS) +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Use minimum region count"};
        handleError(error);
        return false;
    }
    
    if (count > MAX_REGIONS) {
        ErrorInfo error("Region count too large: " + std::to_string(count) +
                       ". Maximum is " + std::to_string(MAX_REGIONS) +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Use maximum region count"};
        handleError(error);
        return false;
    }
    
    return true;
}

std::optional<size_t> ErrorHandling::suggestFallbackNeuronCount(size_t requested) {
    if (requested < 10) {
        return 100;  // Use a reasonable minimum
    } else if (requested > 1000000) {
        return 10000;  // Use a reasonable maximum
    }
    return std::nullopt;  // No suggestion needed
}

std::optional<float> ErrorHandling::suggestFallbackWeight(float requested) {
    if (requested < -10.0f) {
        return -1.0f;
    } else if (requested > 10.0f) {
        return 1.0f;
    }
    return std::nullopt;
}

std::optional<double> ErrorHandling::suggestFallbackTimestep(double requested) {
    if (requested <= 0.0) {
        return 0.001;  // Default timestep
    } else if (requested > 0.1) {
        return 0.01;   // Conservative timestep
    }
    return std::nullopt;
}

bool ErrorHandling::validateConfigFileFormat(const std::string& content, const std::string& filepath) {
    if (content.empty()) {
        ErrorInfo error("Configuration file is empty: " + filepath,
                       ErrorCategory::Configuration, ErrorSeverity::Error);
        handleError(error);
        return false;
    }
    
    // Basic format validation
    size_t commentLines = 0;
    size_t malformedLines = 0;
    std::istringstream stream(content);
    std::string line;
    size_t lineNum = 0;
    
    while (std::getline(stream, line)) {
        ++lineNum;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            ++commentLines;
            continue;
        }
        
        // Check for key=value format
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            ++malformedLines;
            continue;
        }
        
        std::string key = trim(line.substr(0, equals_pos));
        std::string value = trim(line.substr(equals_pos + 1));
        
        if (key.empty()) {
            ++malformedLines;
        }
    }
    
    if (malformedLines > 0) {
        ErrorInfo error("Configuration file has " + std::to_string(malformedLines) +
                       " malformed lines (line 1-" + std::to_string(lineNum) + ")",
                       ErrorCategory::Configuration, ErrorSeverity::Warning);
        handleError(error);
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> ErrorHandling::safeGetParameter(const Config& config,
                                                 const std::string& key,
                                                 const T& defaultValue,
                                                 const std::string& context,
                                                 std::function<bool(const T&)> validator) {
    auto value = config.get<T>(key);
    if (!value.has_value()) {
        ErrorInfo error("Missing configuration parameter: " + key +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Configuration, ErrorSeverity::Warning);
        error.recoveryActions = {"Use provided default value"};
        handleError(error);
        return defaultValue;
    }
    
    if (validator && !validator(*value)) {
        ErrorInfo error("Invalid configuration value for parameter: " + key +
                       " = " + std::to_string(*value) +
                       (context.empty() ? "" : " in context: " + context),
                       ErrorCategory::Validation, ErrorSeverity::Error);
        error.recoveryActions = {"Use provided default value"};
        handleError(error);
        return defaultValue;
    }
    
    return *value;
}

bool ErrorHandling::isMoreSevere(ErrorSeverity sev1, ErrorSeverity sev2) {
    const int severityOrder[] = {
        static_cast<int>(ErrorSeverity::Warning),
        static_cast<int>(ErrorSeverity::Error),
        static_cast<int>(ErrorSeverity::Critical),
        static_cast<int>(ErrorSeverity::Fatal)
    };
    
    return severityOrder[static_cast<int>(sev1)] > severityOrder[static_cast<int>(sev2)];
}

void ErrorHandling::logError(const ErrorInfo& error) {
    std::string formatted = formatError(error);
    switch (error.severity) {
        case ErrorSeverity::Warning:
            NLM_LOG_WARNING(formatted);
            break;
        case ErrorSeverity::Error:
        case ErrorSeverity::Critical:
            NLM_LOG_ERROR(formatted);
            break;
        case ErrorSeverity::Fatal:
            NLM_LOG_CRITICAL(formatted);
            break;
    }
}

} // namespace nlm
