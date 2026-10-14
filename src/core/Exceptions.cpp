#include "core/Exceptions.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <typeinfo>

namespace nlm {

std::string NLMException::toString() const {
    std::ostringstream oss;
    
    // Simple string representation
    std::string severityStr;
    switch (severity_) {
        case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
        case ErrorSeverity::Info: severityStr = "INFO"; break;
        case ErrorSeverity::Warning: severityStr = "WARNING"; break;
        case ErrorSeverity::Error: severityStr = "ERROR"; break;
        case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
        case ErrorSeverity::Fatal: severityStr = "FATAL"; break;
        default: severityStr = "UNKNOWN"; break;
    }
    
    oss << "[" << severityStr << "] " << what();
    if (!file_.empty()) {
        oss << " (at " << file_ << ":" << line_ << ")";
    }
    
    return oss.str();
}

} // namespace nlm