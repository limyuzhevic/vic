#include "NLMError.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

namespace nlm {

std::string NLMError::extractBacktrace() const {
    #ifdef _MSC_VER
        // Windows implementation using dbghelp
        static char buffer[16384];
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, nullptr, TRUE);
        
        void* stack[62];
        USHORT frames = CaptureStackBackTrace(2, 60, stack, nullptr);
        
        std::stringstream ss;
        for (USHORT i = 0; i < frames; i++) {
            DWORD64 address = reinterpret_cast<DWORD64>(stack[i]);
            ULONG64 displacement;
            DWORD64 moduleBase = 0;
            char symbolBuffer[sizeof(SYMBOL_INFO) + 256];
            PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = 255;
            
            if (SymFromAddr(process, address, &displacement, symbol)) {
                moduleBase = symGetModuleBase(process, address);
                ss << "  " << symbol->Name << "+0x" 
                   << std::hex << displacement << " (0x" 
                   << address - moduleBase << ")\n";
            } else {
                ss << "  0x" << std::hex << address << "\n";
            }
        }
        
        SymCleanup(process);
        std::strcpy(buffer, ss.str().c_str());
        return buffer;
    #else
        // Unix-like implementation using execinfo
        void* stack[64];
        int frames = backtrace(stack, 64);
        char** symbols = backtrace_symbols(stack, frames);
        
        std::stringstream ss;
        ss << "Backtrace:\n";
        for (int i = 2; i < frames; i++) { // Skip this function and extractBacktrace
            ss << "  " << i-2 << ": " << symbols[i] << "\n";
        }
        
        free(symbols);
        std::string result = ss.str();
        return result;
    #endif
}

std::string NLMError::toString() const {
    std::stringstream ss;
    ss << "[" << severityToString(severity_) << "] "
       << message_ << "\n"
       << "Source: " << getSourceLocation() << "\n"
       << "Thread: " << std::hex << threadId_ << "\n"
       << "Process: " << processId_ << "\n"
       << "Time: " << getFormattedTimestamp() << "\n"
       << "Category: " << static_cast<int>(category_) << "\n"
       << "Classification: " << static_cast<int>(classification_);
    
    if (!context_.empty()) {
        ss << "\nContext:";
        for (const auto& pair : context_) {
            ss << "\n  " << pair.first << "=" << pair.second;
        }
    }
    
    return ss.str();
}

std::string NLMError::toJSON() const {
    nlohmann::json j;
    j["message"] = message_;
    j["severity"] = severityToString(severity_);
    j["category"] = static_cast<int>(category_);
    j["classification"] = static_cast<int>(classification_);
    j["timestamp"] = getFormattedTimestamp();
    j["thread_id"] = std::hex << threadId_;
    j["process_id"] = processId_;
    j["source_file"] = sourceFile_;
    j["source_line"] = sourceLine_;
    j["source_function"] = sourceFunction_;
    
    if (!context_.empty()) {
        j["context"] = context_;
    }
    if (!diagnostics_.empty()) {
        j["diagnostics"] = diagnostics_;
    }
    if (!suggestions_.empty()) {
        j["suggestions"] = suggestions_;
    }
    if (!recoveryActions_.empty()) {
        j["recovery_actions"] = recoveryActions_;
    }
    if (!metadata_.empty()) {
        j["metadata"] = metadata_;
    }
    
    return j.dump(2);
}

std::string NLMError::toYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    emitter << YAML::Key "message" << YAML::Value message_;
    emitter << YAML::Key "severity" << YAML::Value severityToString(severity_);
    emitter << YAML::Key "category" << YAML::Value static_cast<int>(category_);
    emitter << YAML::Key "classification" << YAML::Value static_cast<int>(classification_);
    emitter << YAML::Key "timestamp" << YAML::Value getFormattedTimestamp();
    emitter << YAML::Key "thread_id" << YAML::Value std::hex << threadId_;
    emitter << YAML::Key "process_id" << YAML::Value processId_;
    emitter << YAML::Key "source_file" << YAML::Value sourceFile_;
    emitter << YAML::Key "source_line" << YAML::Value sourceLine_;
    emitter << YAML::Key "source_function" << YAML::Value sourceFunction_;
    
    if (!context_.empty()) {
        emitter << YAML::Key "context" << YAML::Value context_;
    }
    if (!diagnostics_.empty()) {
        emitter << YAML::Key "diagnostics" << YAML::Value diagnostics_;
    }
    if (!suggestions_.empty()) {
        emitter << YAML::Key "suggestions" << YAML::Value suggestions_;
    }
    if (!recoveryActions_.empty()) {
        emitter << YAML::Key "recovery_actions" << YAML::Value recoveryActions_;
    }
    if (!metadata_.empty()) {
        emitter << YAML::Key "metadata" << YAML::Value metadata_;
    }
    
    emitter << YAML::EndMap;
    
    std::stringstream ss;
    ss << emitter;
    return ss.str();
}

std::string NLMError::toXML() const {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<error>\n";
    ss << "  <message>" << escapeXML(message_) << "</message>\n";
    ss << "  <severity>" << severityToString(severity_) << "</severity>\n";
    ss << "  <category>" << static_cast<int>(category_) << "</category>\n";
    ss << "  <classification>" << static_cast<int>(classification_) << "</classification>\n";
    ss << "  <timestamp>" << getFormattedTimestamp() << "</timestamp>\n";
    ss << "  <thread_id>" << std::hex << threadId_ << "</thread_id>\n";
    ss << "  <process_id>" << processId_ << "</process_id>\n";
    ss << "  <source_file>" << escapeXML(sourceFile_) << "</source_file>\n";
    ss << "  <source_line>" << sourceLine_ << "</source_line>\n";
    ss << "  <source_function>" << escapeXML(sourceFunction_) << "</source_function>\n";
    
    if (!context_.empty()) {
        ss << "  <context>\n";
        for (const auto& pair : context_) {
            ss << "    <" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">\n";
        }
        ss << "  </context>\n";
    }
    if (!diagnostics_.empty()) {
        ss << "  <diagnostics>\n";
        for (const auto& pair : diagnostics_) {
            ss << "    <" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">\n";
        }
        ss << "  </diagnostics>\n";
    }
    if (!suggestions_.empty()) {
        ss << "  <suggestions>\n";
        for (const auto& pair : suggestions_) {
            ss << "    <" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">\n";
        }
        ss << "  </suggestions>\n";
    }
    if (!recoveryActions_.empty()) {
        ss << "  <recovery_actions>\n";
        for (const auto& pair : recoveryActions_) {
            ss << "    <" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">\n";
        }
        ss << "  </recovery_actions>\n";
    }
    if (!metadata_.empty()) {
        ss << "  <metadata>\n";
        for (const auto& pair : metadata_) {
            ss << "    <" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">\n";
        }
        ss << "  </metadata>\n";
    }
    
    ss << "</error>";
    return ss.str();
}

bool NLMError::operator==(const NLMError& other) const {
    return message_ == other.message_ &&
           severity_ == other.severity_ &&
           category_ == other.category_ &&
           classification_ == other.classification_ &&
           getSourceLocation() == other.getSourceLocation();
}

bool NLMError::matches(const std::string& pattern) const {
    // Simple pattern matching (can be enhanced)
    std::string lowerMessage = message_;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    std::string lowerPattern = pattern;
    std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
    
    return lowerMessage.find(lowerPattern) != std::string::npos;
}

bool NLMError::matchesSeverity(ErrorSeverity minSeverity) const {
    return severity_ >= minSeverity;
}

bool NLMError::matchesCategory(ErrorCategory category) const {
    return category_ == category;
}

void NLMError::logError() const {
    // Integration with existing Logger system
    // This would call into the Logger class to log the error
    // For now, just output to stderr
    std::cerr << "NLM Error: " << message_ << std::endl;
    std::cerr << "Location: " << getSourceLocation() << std::endl;
    std::cerr << "Time: " << getFormattedTimestamp() << std::endl;
}

void NLMError::reportError() const {
    // Send error to error reporting system
    // Could integrate with existing error reporting infrastructure
    std::cout << "Error Report: " << message_ << std::endl;
}

void NLMError::alertError() const {
    // Send urgent notification for critical errors
    if (severity_ >= ErrorSeverity::Critical) {
        std::cerr << "ALERT: Critical error detected: " << message_ << std::endl;
        // Could integrate with alert systems
    }
}

bool NLMError::attemptRecovery() const {
    // Check if recovery should be attempted based on classification
    switch (classification_) {
        case ErrorClassification::Retryable:
        case ErrorClassification::Fallback:
        case ErrorClassification::Degrade:
        case ErrorClassification::Checkpoint:
        case ErrorClassification::ResourceReclaim:
        case ErrorClassification::Isolate:
        case ErrorClassification::Wait:
        case ErrorClassification::Reset:
        case ErrorClassification::Migrate:
            return true;
        case ErrorClassification::Repair:
        case ErrorClassification::Escalate:
        case ErrorClassification::Ignore:
            return false;
        default:
            return false;
    }
}

// Factory methods implementation
std::unique_ptr<NLMError> NLMError::createConfigurationError(const std::string& message, const std::string& field, const std::string& suggestion) {
    return std::make_unique<ConfigurationError>(message, ConfigurationError::ConfigErrorType::InvalidFormat, field, suggestion);
}

std::unique_ptr<NLMError> NLMError::createValidationError(const std::string& message, const std::string& field, const std::string& suggestion) {
    return std::make_unique<ValidationError>(message, ValidationError::ValidationErrorType::TypeMismatch, field, suggestion);
}

std::unique_ptr<NLMError> NLMError::createSerializationError(const std::string& message, const std::string& component, const std::string& suggestion) {
    return std::make_unique<SerializationError>(message, SerializationError::SerializationErrorType::Unknown, component, suggestion);
}

std::unique_ptr<NLMError> NLMError::createNeuralError(const std::string& message, const std::string& operation, const std::string& suggestion) {
    return std::make_unique<NeuralError>(message, NeuralError::NeuralErrorType::NeuronInitializationFailed, operation, suggestion);
}

std::unique_ptr<NLMError> NLMError::createMemoryError(const std::string& message, size_t address, const std::string& suggestion) {
    return std::make_unique<MemoryError>(message, MemoryError::MemoryErrorType::AllocationFailed, address, suggestion);
}

std::unique_ptr<NLMError> NLMError::createSystemError(const std::string& message, const std::string& system, const std::string& suggestion) {
    return std::make_unique<SystemError>(message, SystemError::SystemErrorType::FileSystemError, system, suggestion);
}

// Helper functions for XML escaping
std::string NLMError::escapeXML(const std::string& input) const {
    std::string result;
    for (char c : input) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&apos;"; break;
            default: result += c; break;
        }
    }
    return result;
}

// Severity conversion helper
std::string NLMError::severityToString(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARNING";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
        case ErrorSeverity::Fatal: return "FATAL";
        case ErrorSeverity::Alert: return "ALERT";
        default: return "ERROR";
    }
}

} // namespace nlm