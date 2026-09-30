// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * User-friendly error message generator
 */
class UserFriendlyErrorMessages {
public:
    static std::string formatErrorMessage(const BrainException& error);
    static std::string getSuggestedRemedy(const BrainException::ErrorCode& errorCode);
    static std::string getContextInformation(const BrainException& error);
    static std::string getTroubleshootingGuide(const BrainException& error);
    static std::string getBeginnerFriendlyMessage(const BrainException& error);
    static std::string getAdvancedUserMessage(const BrainException& error);
    
    // Error severity levels for user feedback
    enum class Severity {
        Info,
        Warning,
        Error,
        Critical
    };
    
    static Severity getSeverity(BrainException::ErrorCode errorCode);
};

} // namespace nlm
