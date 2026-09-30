#include "UserFriendlyErrorMessages.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

std::string UserFriendlyErrorMessages::formatErrorMessage(const BrainException& error) {
    std::ostringstream oss;
    
    // Get the base error message
    oss << "[NLM Error " << static_cast<int>(error.getErrorCode()) << "] " << error.what();
    
    // Add component information if available
    if (!error.getComponent().empty()) {
        oss << "\nComponent: " << error.getComponent();
    }
    
    // Add context information if available
    if (!error.getContext().empty()) {
        oss << "\n\nContext:";
        for (const auto& pair : error.getContext()) {
            oss << "\n  " << pair.first << ": " << pair.second;
        }
    }
    
    // Add suggestions if available
    if (!error.getSuggestions().empty()) {
        oss << "\n\nSuggested Remedies:";
        for (const auto& sug : error.getSuggestions()) {
            oss << "\n  - " << sug;
        }
    }
    
    return oss.str();
}

std::string UserFriendlyErrorMessages::getSuggestedRemedy(const BrainException::ErrorCode& errorCode) {
    switch (errorCode) {
        case BrainException::ErrorCode::BrainInitializationFailed:
            return "1. Check all configuration files for errors\n"
                   "2. Ensure all required system components are installed\n"
                   "3. Verify system resources (memory, CPU) are sufficient\n"
                   "4. Check system logs for detailed error information";
        
        case BrainException::ErrorCode::MemoryCapacityExceeded:
            return "1. Reduce the number of concurrent memory traces\n"
                   "2. Increase memory capacity by adjusting configuration\n"
                   "3. Clear unused memory traces\n"
                   "4. Consider using memory compression techniques";
        
        case BrainException::ErrorCode::ConfigurationError:
            return "1. Validate configuration file format (JSON/YAML)\n"
                   "2. Check for syntax errors in configuration files\n"
                   "3. Verify required parameters are present\n"
                   "4. Ensure parameter values are within valid ranges";
        
        case BrainException::ErrorCode::PlasticityRuleConflict:
            return "1. Disable conflicting plasticity rules\n"
                   "2. Review plasticity rule compatibility\n"
                   "3. Consider using only one plasticity mechanism at a time\n"
                   "4. Adjust plasticity rule parameters to resolve conflicts";
        
        case BrainException::ErrorCode::FileIOError:
            return "1. Check file permissions\n"
                   "2. Ensure the file path is correct\n"
                   "3. Verify disk space is available\n"
                   "4. Check if the file is locked by another process\n"
                   "5. Try using an absolute file path";
        
        case BrainException::ErrorCode::NeuromodulationLevelInvalid:
            return "1. Check neuromodulator parameter ranges\n"
                   "2. Verify neuromodulator configuration\n"
                   "3. Ensure neuromodulation signals are within valid bounds\n"
                   "4. Reset neuromodulator levels to default values";
        
        case BrainException::ErrorCode::DevelopmentalStageTransitionFailed:
            return "1. Check developmental stage transition logic\n"
                   "2. Verify critical period constraints\n"
                   "3. Ensure all prerequisites for stage transition are met\n"
                   "4. Review developmental system configuration";
        
        case BrainException::ErrorCode::InvalidParameter:
            return "1. Check parameter type and value\n"
                   "2. Verify parameter is within valid range\n"
                   "3. Ensure parameter dependencies are satisfied\n"
                   "4. Use correct parameter naming convention";
        
        case BrainException::ErrorCode::OutOfRange:
            return "1. Adjust parameter value to be within valid range\n"
                   "2. Check minimum and maximum allowed values\n"
                   "3. Verify parameter scaling\n"
                   "4. Consider using a different parameter if appropriate";
        
        case BrainException::ErrorCode::SystemFailure:
            return "1. Check system resources (memory, CPU)\n"
                   "2. Verify all system components are properly initialized\n"
                   "3. Check for memory corruption or other system issues\n"
                   "4. Restart the system if necessary\n"
                   "5. Check system logs for detailed error information";
        
        default:
            return "1. Check system logs for detailed error information\n"
                   "2. Verify system configuration\n"
                   "3. Ensure all required components are installed\n"
                   "4. Try reducing system load or complexity";
    }
}

std::string UserFriendlyErrorMessages::getContextInformation(const BrainException& error) {
    std::ostringstream oss;
    
    oss << "Error Code: " << static_cast<int>(error.getErrorCode()) << "\n"
        << "Error Type: " << error.getComponent() << "\n"
        << "Timestamp: [current time]";
    
    if (!error.getContext().empty()) {
        oss << "\n\nError Context:";
        for (const auto& pair : error.getContext()) {
            oss << "\n  " << pair.first << ": " << pair.second;
        }
    }
    
    return oss.str();
}

std::string UserFriendlyErrorMessages::getTroubleshootingGuide(const BrainException& error) {
    std::string baseMessage = "Troubleshooting Guide:\n\n";
    baseMessage += "1. Error Summary: " + std::string(error.what()) + "\n\n";
    baseMessage += "2. Suggested Initial Steps:\n";
    
    switch (error.getErrorCode()) {
        case BrainException::ErrorCode::BrainInitializationFailed:
            baseMessage += "   - Check configuration files for syntax errors\n"
                          "   - Verify all required dependencies are installed\n"
                          "   - Ensure sufficient system resources\n";
            break;
        
        case BrainException::ErrorCode::MemoryCapacityExceeded:
            baseMessage += "   - Clear unused memory traces\n"
                          "   - Increase system memory or reduce memory usage\n"
                          "   - Check for memory leaks in the system\n";
            break;
        
        case BrainException::ErrorCode::ConfigurationError:
            baseMessage += "   - Validate configuration file format\n"
                          "   - Check for missing required parameters\n"
                          "   - Verify parameter types and values\n";
            break;
        
        case BrainException::ErrorCode::FileIOError:
            baseMessage += "   - Check file permissions and path\n"
                          "   - Verify disk space is available\n"
                          "   - Ensure the file is not locked by another process\n";
            break;
        
        default:
            baseMessage += "   - Check system logs for more details\n"
                          "   - Verify system configuration\n"
                          "   - Ensure all system components are running\n";
    }
    
    baseMessage += "\n3. Advanced Debugging:\n"
                  "   - Enable debug logging in configuration\n"
                  "   - Check system resource usage\n"
                  "   - Review recent system changes\n"
                  "   - Consider running with a debugger for more details\n";
    
    return baseMessage;
}

std::string UserFriendlyErrorMessages::getBeginnerFriendlyMessage(const BrainException& error) {
    std::ostringstream oss;
    
    oss << "Oops! Something went wrong in the brain system.\n\n";
    
    switch (error.getErrorCode()) {
        case BrainException::ErrorCode::BrainInitializationFailed:
            oss << "The brain system couldn't start up properly. This is usually because:\n"
                << "  • Configuration files are missing or have errors\n"
                << "  • Some required software isn't installed\n"
                << "  • The system doesn't have enough memory or processing power\n\n"
                << "What you can do:\n"
                << "  • Check that all required files are present\n"
                << "  • Run the setup wizard to install missing components\n"
                << "  • Try running on a system with more resources\n";
            break;
        
        case BrainException::ErrorCode::MemoryCapacityExceeded:
            oss << "The brain is trying to remember too much information at once. This happens when:\n"
                << "  • Too many memories are stored simultaneously\n"
                << "  • Each memory is too large\n"
                << "  • The brain's memory capacity is too small\n\n"
                << "What you can do:\n"
                << "  • Use shorter memory traces\n"
                << "  • Clear some old memories\n"
                << "  • Increase the brain's memory capacity in settings\n";
            break;
        
        case BrainException::ErrorCode::ConfigurationError:
            oss << "There's a problem with how the brain is configured. This is usually because:\n"
                << "  • Configuration files have syntax errors\n"
                << "  • Some required settings are missing\n"
                << "  • Settings have invalid values\n\n"
                << "What you can do:\n"
                << "  • Check configuration files for typos\n"
                << "  • Use the configuration editor to fix errors\n"
                << "  • Restart the brain with default settings if needed\n";
            break;
        
        default:
            oss << "An unexpected error occurred. This could be due to:\n"
                << "  • A bug in the brain software\n"
                << "  • Incompatible system configuration\n"
                << "  • Insufficient system resources\n\n"
                << "What you can do:\n"
                << "  • Check the error messages above for more details\n"
                << "  • Try restarting the system\n"
                << "  • Contact support if the problem persists\n";
    }
    
    return oss.str();
}

std::string UserFriendlyErrorMessages::getAdvancedUserMessage(const BrainException& error) {
    std::ostringstream oss;
    
    oss << "NLM System Error Report\n"
        << "======================\n\n"
        << "Error Code: 0x" << std::hex << static_cast<int>(error.getErrorCode()) << "\n"
        << "Component: " << error.getComponent() << "\n"
        << "Timestamp: [current timestamp]\n\n";
    
    oss << "Technical Details:\n";
    oss << "  - Error Message: " << error.what() << "\n";
    oss << "  - Stack Trace: [see system logs]\n";
    
    if (!error.getContext().empty()) {
        oss << "  - Context:\n";
        for (const auto& pair : error.getContext()) {
            oss << "    * " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    oss << "\nSuggested Actions:\n";
    oss << "  1. " << getSuggestedRemedy(error.getErrorCode()) << "\n\n";
    
    oss << "System State:\n";
    oss << "  - Current Step: [see system state]\n"
        << "  - Memory Usage: [see system metrics]\n"
        << "  - CPU Load: [see system metrics]\n";
    
    return oss.str();
}

ErrorMessages::Severity ErrorMessages::getSeverity(BrainException::ErrorCode errorCode) {
    switch (errorCode) {
        case BrainException::ErrorCode::BrainInitializationFailed:
        case BrainException::ErrorCode::MemoryCorruption:
        case BrainException::ErrorCode::SystemFailure:
            return Severity::Critical;
            
        case BrainException::ErrorCode::ConfigurationError:
        case BrainException::ErrorCode::InvalidParameter:
        case BrainException::ErrorCode::PlasticityRuleConflict:
        case BrainException::ErrorCode::NeuromodulationLevelInvalid:
            return Severity::Error;
            
        default:
            return Severity::Warning;
    }
}

} // namespace nlm
