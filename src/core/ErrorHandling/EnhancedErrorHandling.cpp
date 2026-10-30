#include "EnhancedErrorHandling.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <filesystem>

namespace nlm {

// ErrorReporter implementation
struct ErrorReporter::Impl {
    std::vector<ErrorInfo> errors;
    std::vector<std::string> suggestions;
    std::map<std::string, std::string> details;
    std::chrono::system_clock::time_point session_start;
};

ErrorReporter::ErrorReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->session_start = std::chrono::system_clock::now();
}

ErrorReporter::~ErrorReporter() = default;

void ErrorReporter::addError(ErrorCategory category, ErrorSeverity severity,
                            const std::string& message, const std::string& context,
                            const std::string& file, int line) {
    errors_.emplace_back(category, severity, message, context, file, line);
}

void ErrorReporter::addSuggestion(const std::string& suggestion) {
    suggestions_.push_back(suggestion);
}

void ErrorReporter::addDetail(const std::string& key, const std::string& value) {
    details_[key] = value;
}

std::vector<ErrorInfo> ErrorReporter::getErrorsByCategory(ErrorCategory category) const {
    std::vector<ErrorInfo> result;
    for (const auto& error : errors_) {
        if (error.category == category) {
            result.push_back(error);
        }
    }
    return result;
}

std::vector<ErrorInfo> ErrorReporter::getErrorsBySeverity(ErrorSeverity severity) const {
    std::vector<ErrorInfo> result;
    for (const auto& error : errors_) {
        if (error.severity == severity) {
            result.push_back(error);
        }
    }
    return result;
}

size_t ErrorReporter::getErrorCount(ErrorSeverity severity) const {
    return std::count_if(errors_.begin(), errors_.end(),
                        [severity](const ErrorInfo& error) {
                            return error.severity == severity;
                        });
}

std::string ErrorReporter::generateReport() const {
    std::ostringstream oss;
    oss << "=== Error Report ===" << std::endl;
    oss << "Session duration: " << 
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - pImpl->session_start).count() << " seconds" << std::endl;
    oss << "Total errors: " << errors_.size() << std::endl;
    
    if (!suggestions_.empty()) {
        oss << std::endl;
        oss << "Suggestions:" << std::endl;
        for (const auto& suggestion : suggestions_) {
            oss << "  - " << suggestion << std::endl;
        }
    }
    
    if (!details_.empty()) {
        oss << std::endl;
        oss << "Details:" << std::endl;
        for (const auto& detail : details_) {
            oss << "  " << detail.first << ": " << detail.second << std::endl;
        }
    }
    
    // Group errors by category and severity
    oss << std::endl;
    oss << "Errors by Category and Severity:" << std::endl;
    oss << "  Category | Severity | Count" << std::endl;
    oss << "  ---------|----------|------" << std::endl;
    
    std::map<ErrorCategory, std::map<ErrorSeverity, size_t>> errorCounts;
    for (const auto& error : errors_) {
        errorCounts[error.category][error.severity]++;
    }
    
    for (const auto& [category, severity_counts] : errorCounts) {
        for (const auto& [severity, count] : severity_counts) {
            oss << std::setw(9) << static_cast<int>(category) << " | "
                << std::setw(10) << static_cast<int>(severity) << " | "
                << count << std::endl;
        }
    }
    
    oss << std::endl;
    oss << "Recent Errors:" << std::endl;
    oss << "  " << std::string(80, '-') << std::endl;
    
    size_t recent_count = std::min(size_t(10), errors_.size());
    for (size_t i = 0; i < recent_count; ++i) {
        const auto& error = errors_[errors_.size() - 1 - i]; // Show most recent first
        oss << "  " << error.getFormattedMessage() << std::endl;
    }
    
    if (errors_.size() > recent_count) {
        oss << "  ... and " << errors_.size() - recent_count << " more errors" << std::endl;
    }
    
    return oss.str();
}

std::string ErrorReporter::generateHTMLReport() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head><title>NLM Error Report</title></head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "<h1>NLM Error Report</h1>" << std::endl;
    oss << "<p>Session duration: " <<
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - pImpl->session_start).count() << " seconds</p>" << std::endl;
    oss << "<p>Total errors: " << errors_.size() << "</p>" << std::endl;
    oss << "<h2>Errors</h2>" << std::endl;
    oss << "<table border=\"1\">" << std::endl;
    oss << "<tr><th>Category</th><th>Severity</th><th>Message</th><th>Context</th></tr>" << std::endl;
    
    for (const auto& error : errors_) {
        oss << "<tr>" << std::endl;
        oss << "<td>" << static_cast<int>(error.category) << "</td>" << std::endl;
        oss << "<td>" << static_cast<int>(error.severity) << "</td>" << std::endl;
        oss << "<td>" << error.message << "</td>" << std::endl;
        oss << "<td>" << error.context << "</td>" << std::endl;
        oss << "</tr>" << std::endl;
    }
    
    oss << "</table>" << std::endl;
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string ErrorReporter::generateJSONReport() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"total_errors\": " << errors_.size() << "," << std::endl;
    oss << "  \"session_duration_seconds\": " <<
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - pImpl->session_start).count() << "," << std::endl;
    oss << "  \"errors\": [" << std::endl;
    
    for (size_t i = 0; i < errors_.size(); ++i) {
        const auto& error = errors_[i];
        oss << "    {" << std::endl;
        oss << "      \"category\": " << static_cast<int>(error.category) << "," << std::endl;
        oss << "      \"severity\": " << static_cast<int>(error.severity) << "," << std::endl;
        oss << "      \"message\": \"" << escapeJSON(error.message) << "\"," << std::endl;
        oss << "      \"context\": \"" << escapeJSON(error.context) << "\"," << std::endl;
        oss << "      \"file\": \"" << escapeJSON(error.file) << "\"," << std::endl;
        oss << "      \"line\": " << error.line << std::endl;
        oss << "    }" << (i < errors_.size() - 1 ? "," : "") << std::endl;
    }
    
    oss << "  ]" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

bool ErrorReporter::saveReport(const std::string& filename, const std::string& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content;
    if (format == "html") {
        content = generateHTMLReport();
    } else if (format == "json") {
        content = generateJSONReport();
    } else {
        content = generateReport();
    }
    
    file << content;
    file.close();
    return true;
}

std::string ErrorReporter::getSummary() const {
    std::ostringstream oss;
    oss << "NLM Error Summary: " << errors_.size() << " errors in session" << std::endl;
    oss << "  " << std::string(50, '=') << std::endl;
    
    size_t critical_count = getErrorCount(ErrorSeverity::Critical);
    size_t error_count = getErrorCount(ErrorSeverity::Error);
    size_t warning_count = getErrorCount(ErrorSeverity::Warning);
    size_t info_count = getErrorCount(ErrorSeverity::Info);
    
    if (critical_count > 0) oss << "  Critical: " << critical_count << std::endl;
    if (error_count > 0) oss << "  Error: " << error_count << std::endl;
    if (warning_count > 0) oss << "  Warning: " << warning_count << std::endl;
    if (info_count > 0) oss << "  Info: " << info_count << std::endl;
    
    return oss.str();
}

// NLMException implementation
std::string NLMException::formatMessage(const std::string& message,
                                        const std::string& context) {
    std::ostringstream oss;
    oss << message;
    if (!context.empty()) {
        oss << " (Context: " << context << ")";
    }
    return oss.str();
}

// ConfigErrorHandler implementation
bool ConfigErrorHandler::validateConfigFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    return validateConfigFileSyntax(file);
}

std::vector<std::string> ConfigErrorHandler::validateConfigFileSyntax(const std::string& content) {
    std::vector<std::string> errors;
    std::istringstream stream(content);
    std::string line;
    int line_number = 0;
    
    while (std::getline(stream, line)) {
        line_number++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            errors.push_back("Line " + std::to_string(line_number) + ": Invalid format. Expected 'key = value'");
        } else {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                errors.push_back("Line " + std::to_string(line_number) + ": Empty key");
            }
            
            if (value.empty()) {
                errors.push_back("Line " + std::to_string(line_number) + ": Empty value for key: " + key);
            }
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigErrorHandler::validateConfigValues(const Config& config) {
    std::vector<std::string> errors;
    
    // Check for required parameters
    std::vector<std::string> required_keys = {"neuron_count", "simulation_timestep"};
    for (const auto& key : required_keys) {
        if (!config.has(key)) {
            errors.push_back("Missing required parameter: " + key);
        }
    }
    
    // Validate parameter values
    if (config.has("neuron_count")) {
        auto neuron_count = config.getOr<int64_t>("neuron_count", 0);
        if (neuron_count <= 0) {
            errors.push_back("neuron_count must be positive, got: " + std::to_string(neuron_count));
        } else if (neuron_count > 100000) {
            errors.push_back("neuron_count too large, max is 100000, got: " + std::to_string(neuron_count));
        }
    }
    
    if (config.has("connection_probability")) {
        auto conn_prob = config.getOr<double>("connection_probability", 0.0);
        if (conn_prob < 0.0 || conn_prob > 1.0) {
            errors.push_back("connection_probability must be between 0.0 and 1.0, got: " + std::to_string(conn_prob));
        }
    }
    
    if (config.has("simulation_timestep")) {
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            errors.push_back("simulation_timestep must be positive and <= 1.0, got: " + std::to_string(timestep));
        }
    }
    
    return errors;
}

std::string ConfigErrorHandler::getConfigFileErrorSuggestion(const std::string& error) {
    if (error.find("Invalid format") != std::string::npos) {
        return "Use format: key = value";
    } else if (error.find("Empty key") != std::string::npos) {
        return "Provide a valid parameter name for the key";
    } else if (error.find("Empty value") != std::string::npos) {
        return "Provide a value for the parameter";
    } else if (error.find("must be positive") != std::string::npos) {
        return "Ensure the value is greater than 0";
    } else if (error.find("too large") != std::string::npos) {
        return "Reduce the value to within acceptable limits";
    } else if (error.find("between 0.0 and 1.0") != std::string::npos) {
        return "Ensure probability values are between 0.0 and 1.0";
    }
    return "Check the parameter format and value range";
}

// NeuralErrorHandler implementation
bool NeuralErrorHandler::validateNeuronParameters(double v, double thresh, double leakConductance) {
    if (v < -100.0 || v > 100.0) {
        return false;
    }
    if (thresh <= 0.0 || thresh > 100.0) {
        return false;
    }
    if (leakConductance <= 0.0 || leakConductance > 1.0) {
        return false;
    }
    return true;
}

std::string NeuralErrorHandler::getNeuronErrorSuggestion(double v, double thresh, double leakConductance) {
    if (v < -100.0 || v > 100.0) {
        return "Membrane potential should be within reasonable bounds (-100 to 100 mV)";
    }
    if (thresh <= 0.0 || thresh > 100.0) {
        return "Threshold should be positive and reasonable (0 to 100 mV)";
    }
    if (leakConductance <= 0.0 || leakConductance > 1.0) {
        return "Leak conductance should be positive and <= 1.0";
    }
    return "Check neuron parameter ranges";
}

bool NeuralErrorHandler::validateSynapseParameters(double weight, double delay) {
    if (std::isnan(weight) || std::isinf(weight)) {
        return false;
    }
    if (delay < 0.0) {
        return false;
    }
    if (weight < -1000.0 || weight > 1000.0) {
        return false;
    }
    if (delay > 1000.0) {
        return false;
    }
    return true;
}

std::string NeuralErrorHandler::getSynapseErrorSuggestion(double weight, double delay) {
    if (std::isnan(weight) || std::isinf(weight)) {
        return "Synaptic weight should be a finite number";
    }
    if (delay < 0.0) {
        return "Synaptic delay should be non-negative";
    }
    if (weight < -1000.0 || weight > 1000.0) {
        return "Synaptic weight is outside reasonable bounds (-1000 to 1000)";
    }
    if (delay > 1000.0) {
        return "Synaptic delay is too large (max 1000 ms)";
    }
    return "Check synaptic parameter ranges";
}

bool NeuralErrorHandler::validateSpikeParameters(const class Neuron& neuron) {
    const auto& state = neuron.getState();
    return !(std::isnan(state.membranePotential) || std::isinf(state.membranePotential));
}

std::string NeuralErrorHandler::getSpikeErrorSuggestion(const class Neuron& neuron) {
    return "Check neuron state stability and parameters";
}

// EnhancedLogger implementation
struct EnhancedLogger::Impl {
    std::ofstream log_file;
    bool file_logging_enabled;
    size_t max_log_file_size;
    bool console_output_enabled;
    bool colored_output;
    std::string current_session_id;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->file_logging_enabled = false;
    pImpl->max_log_file_size = 10 * 1024 * 1024; // 10 MB
    pImpl->console_output_enabled = true;
    pImpl->colored_output = true;
    
    // Generate session ID
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time_t_now, &tm_buf);
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
    pImpl->current_session_id = oss.str();
    
    // Setup log file
    std::string log_filename = "nlm_log_" + pImpl->current_session_id + ".txt";
    pImpl->log_file.open(log_filename, std::ios::out | std::ios::trunc);
    if (pImpl->log_file.is_open()) {
        pImpl->file_logging_enabled = true;
        pImpl->log_file << "=== NLM Log Session: " << pImpl->current_session_id << " ===" << std::endl;
        pImpl->log_file << "Started: " << std::ctime(&time_t_now);
    }
}

EnhancedLogger::~EnhancedLogger() {
    if (pImpl->file_logging_enabled && pImpl->log_file.is_open()) {
        pImpl->log_file.close();
    }
}

void EnhancedLogger::log(ErrorCategory category, ErrorSeverity severity,
                         const std::string& message, const std::string& context,
                         const std::string& file, int line) {
    std::string formatted_message = formatErrorMessage(category, severity, message, context, file, line);
    
    // Output to console
    if (pImpl->console_output_enabled) {
        std::cout << formatted_message << std::endl;
    }
    
    // Output to log file
    if (pImpl->file_logging_enabled) {
        pImpl->log_file << formatted_message << std::endl;
        
        // Check file size and rotate if necessary
        if (pImpl->log_file.tellp() > pImpl->max_log_file_size) {
            pImpl->log_file.close();
            std::string new_filename = "nlm_log_" + pImpl->current_session_id + "_rotated.txt";
            pImpl->log_file.open(new_filename, std::ios::out | std::ios::trunc);
            if (pImpl->log_file.is_open()) {
                pImpl->file_logging_enabled = true;
                pImpl->current_session_id += "_rotated";
            } else {
                pImpl->file_logging_enabled = false;
            }
        }
    }
    
    // Add to error reporter
    error_reporter_.addError(category, severity, message, context, file, line);
}

void EnhancedLogger::logInfo(const std::string& message, const std::string& context) {
    log(ErrorCategory::System, ErrorSeverity::Info, message, context, "", 0);
}

void EnhancedLogger::logWarning(const std::string& message, const std::string& context) {
    log(ErrorCategory::System, ErrorSeverity::Warning, message, context, "", 0);
}

void EnhancedLogger::logError(const std::string& message, const std::string& context) {
    log(ErrorCategory::System, ErrorSeverity::Error, message, context, "", 0);
}

void EnhancedLogger::logCritical(const std::string& message, const std::string& context) {
    log(ErrorCategory::System, ErrorSeverity::Critical, message, context, "", 0);
}

void EnhancedLogger::logNeural(const std::string& message, const std::string& context) {
    log(ErrorCategory::Neural, ErrorSeverity::Info, message, context, "", 0);
}

void EnhancedLogger::logPlasticity(const std::string& message, const std::string& context) {
    log(ErrorCategory::Plasticity, ErrorSeverity::Info, message, context, "", 0);
}

void EnhancedLogger::logMemory(const std::string& message, const std::string& context) {
    log(ErrorCategory::MemorySystem, ErrorSeverity::Info, message, context, "", 0);
}

void EnhancedLogger::logSystem(const std::string& message, const std::string& context) {
    log(ErrorCategory::System, ErrorSeverity::Info, message, context, "", 0);
}

std::string EnhancedLogger::generateHealthReport() const {
    std::ostringstream oss;
    oss << "=== System Health Report ===" << std::endl;
    oss << "Session ID: " << pImpl->current_session_id << std::endl;
    oss << "Total errors: " << error_reporter_.getErrors().size() << std::endl;
    oss << "System status: " << (isHealthy() ? "HEALTHY" : "UNHEALTHY") << std::endl;
    oss << std::endl;
    oss << error_reporter_.getSummary() << std::endl;
    return oss.str();
}

void EnhancedLogger::printErrorSummary() const {
    std::cout << error_reporter_.getSummary() << std::endl;
}

std::string EnhancedLogger::formatErrorMessage(ErrorCategory category, ErrorSeverity severity,
                                               const std::string& message,
                                               const std::string& context,
                                               const std::string& file, int line) {
    std::ostringstream oss;
    
    // Add color if enabled
    if (pImpl->colored_output) {
        switch (severity) {
            case ErrorSeverity::Debug:
                oss << "\033[36m"; // Cyan
                break;
            case ErrorSeverity::Info:
                oss << "\033[32m"; // Green
                break;
            case ErrorSeverity::Warning:
                oss << "\033[33m"; // Yellow
                break;
            case ErrorSeverity::Error:
                oss << "\033[31m"; // Red
                break;
            case ErrorSeverity::Critical:
                oss << "\033[41m\033[37m"; // Red background, white text
                break;
        }
    }
    
    oss << "[" << static_cast<int>(severity) << "]";
    oss << "[" << static_cast<int>(category) << "]";
    
    if (!file.empty() && line > 0) {
        oss << "(" << file << ":" << line << ")";
    }
    
    oss << " " << message;
    
    if (!context.empty()) {
        oss << " (Context: " << context << ")";
    }
    
    if (pImpl->colored_output) {
        oss << "\033[0m"; // Reset color
    }
    
    return oss.str();
}

// Helper function for JSON escaping
std::string escapeJSON(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < ' ' || c == 127) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

// trim helper function
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm

// Global error reporter instance
nlm::EnhancedLogger& getGlobalLogger() {
    static nlm::EnhancedLogger logger;
    return logger;
}