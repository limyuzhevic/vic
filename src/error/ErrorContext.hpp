// Error Context Header
#pragma once

#include <string>
#include <map>
#include "NLMErrors.hpp"

namespace nlm {

/**
 * Error context
 */
class ErrorContext {
public:
    ErrorContext();
    ~ErrorContext();
    
    /**
     * Set context value
     */
    void setContext(const std::string& key, const std::string& value);
    
    /**
     * Get context value
     */
    std::string getContext(const std::string& key) const;
    
    /**
     * Get all context
     */
    const std::map<std::string, std::string>& getAllContext() const;
    
    /**
     * Clear context
     */
    void clearContext();
    
    /**
     * Set context from error
     */
    void setContextFromError(const NLMError& error);
    
private:
    std::map<std::string, std::string> _context;
};

} // namespace nlm