#include "ErrorHandling/ErrorHandling.hpp"

namespace nlm {

// ErrorHandler implementation

ErrorHandler::ErrorHandler() : throwErrors(true), totalErrors(0) {}

void ErrorHandler::setErrorCallback(std::function<void(const NLMException&)>) {
    std::lock_guard<std::mutex> lock(mutex);
    errorCallback = std::move(errorCallback);
}

void ErrorHandler::logError(const NLMException& error) {
    std::lock_guard<std::mutex> lock(mutex);
    if (errorCallback) {
        errorCallback(error);
    }
    // Also print to stderr
    std::cerr << "NLM Error: " << error.what() << std::endl;
}

void ErrorHandler::handleError(const NLMException& error) {
    std::lock_guard<std::mutex> lock(mutex);
    totalErrors++;
    lastError = std::make_unique<NLMException>(error);
    
    if (throwErrors) {
        throw error;
    } else {
        if (errorCallback) {
            errorCallback(error);
        }
    }
}

std::string ErrorHandler::getErrorSummary() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::ostringstream oss;
    oss << "NLM Error Handler: " << totalErrors << " errors occurred";
    if (totalErrors > 0) {
        oss << ", last error: " << (lastError ? lastError->what() : "none");
    }
    return oss.str();
}

// Explicit template instantiations for Config.cpp
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm