// ErrorHandling.cpp - Error Handling Infrastructure Implementation

#include "ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>

namespace nlm {
    // Implementation of ErrorHandler static instance
    std::unique_ptr<ErrorHandler> ErrorHandler::instance = nullptr;
    std::mutex ErrorHandler::instanceMutex;
    
    // Implementation details for ErrorHandler
    namespace {
        // Helper function to generate unique operation IDs
        std::string generateOperationId(const std::string& operation) {
            static std::atomic<uint64_t> counter{0};
            return operation + "_" + std::to_string(counter++);
        }
    }
    
    // ConsoleErrorReporter implementation
    void ConsoleErrorReporter::report(const std::string& message, ErrorReportLevel level,
                                     const std::string& context) {
        if (level < minLevel) return;
        
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_r(&time_t_now, &tm_buf);
        
        std::ostringstream oss;
        oss << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "]";
        oss << " [";
        
        switch (level) {
            case ErrorReportLevel::Silent: oss << "SILENT"; break;
            case ErrorReportLevel::Warning: oss << "WARNING"; break;
            case ErrorReportLevel::Error: oss << "ERROR"; break;
            case ErrorReportLevel::Critical: oss << "CRITICAL"; break;
        }
        
        oss << "]";
        if (!context.empty()) {
            oss << " [" << context << "]";
        }
        oss << " " << message;
        
        // Output to console
        std::cout << oss.str() << std::endl;
    }
    
    // ErrorHandler implementation
    void ErrorHandler::report(const std::string& message, ErrorReportLevel level,
                             const std::string& context, bool throwException) {
        std::lock_guard<std::mutex> lock(reportMutex);
        
        stats.totalErrors++;
        stats.lastErrorTime = std::chrono::system_clock::now();
        stats.lastErrorMessage = message;
        
        // Create detailed message
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage += " [Context: " + context + "]";
        }
        
        // Report to all enabled reporters
        for (const auto& reporter : reporters) {
            if (reporter->isEnabled(level)) {
                reporter->report(fullMessage, level, context);
            }
        }
        
        if (throwException) {
            // Determine appropriate error code based on message content
            ErrorCode errorCode = ErrorCode::SystemError;
            std::string lowerMsg = message;
            std::transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(),
                          [](unsigned char c) { return std::tolower(c); });
            
            if (lowerMsg.find("config") != std::string::npos) {
                errorCode = ErrorCode::ConfigValidationError;
            } else if (lowerMsg.find("brain") != std::string::npos) {
                errorCode = ErrorCode::BrainInitFailure;
            } else if (lowerMsg.find("neuron") != std::string::npos) {
                errorCode = ErrorCode::NeuronStateError;
            } else if (lowerMsg.find("synapse") != std::string::npos) {
                errorCode = ErrorCode::SynapseConnectionError;
            } else if (lowerMsg.find("simulation") != std::string::npos) {
                errorCode = ErrorCode::SimulationRuntimeError;
            }
            
            throw NLMException(errorCode, fullMessage);
        }
    }
    
    void ErrorHandler::reportException(const NLMException& exception) {
        std::lock_guard<std::mutex> lock(reportMutex);
        
        stats.totalErrors++;
        stats.lastErrorTime = std::chrono::system_clock::now();
        stats.lastErrorMessage = exception.getMessage();
        
        // Report to all reporters
        for (const auto& reporter : reporters) {
            reporter->reportException(exception);
        }
    }
    
    template<typename TException, typename... Args>
    void ErrorHandler::handleError(const std::string& operation,
                                  const std::function<TException(Args...)>>& createException,
                                  Args&&... args, RecoveryMode mode) {
        try {
            if (strategy == ExceptionStrategy::Throw) {
                throw createException(std::forward<Args>(args)...);
            } else if (strategy == ExceptionStrategy::Catch) {
                try {
                    throw createException(std::forward<Args>(args)...);
                } catch (const TException& e) {
                    reportException(e);
                    
                    if (mode == RecoveryMode::Retry) {
                        // TODO: Implement retry logic
                    } else if (mode == RecoveryMode::Graceful) {
                        // TODO: Implement graceful degradation
                    }
                    
                    stats.errorsHandled++;
                }
            }
            // Ignore strategy - just report
            else {
                report("Operation failed: " + operation, ErrorReportLevel::Warning);
                stats.errorsHandled++;
            }
        } catch (...) {
            // Unexpected exception
            std::exception_ptr ep = std::current_exception();
            try {
                std::rethrow_exception(ep);
            } catch (const NLMException& e) {
                reportException(e);
            } catch (const std::exception& e) {
                report(std::string("Unexpected error: ") + e.what(), ErrorReportLevel::Critical);
            }
            throw;
        }
    }
    
    // CheckpointManager implementation
    bool ErrorHandler::Recovery::CheckpointManager::save(const std::string& identifier, const std::string& data) {
        if (checkpointDirectory.empty()) return false;
        
        // Create directory if it doesn't exist
        std::filesystem::create_directories(checkpointDirectory);
        
        std::string filepath = checkpointDirectory + "/" + identifier + ".nlmcheckpoint";
        
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write timestamp
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();
            file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
            
            // Write data
            file.write(data.c_str(), data.size());
            
            return file.good();
        } catch (...) {
            return false;
        }
    }
    
    std::optional<std::string> ErrorHandler::Recovery::CheckpointManager::load(const std::string& identifier) {
        if (checkpointDirectory.empty()) return std::nullopt;
        
        std::string filepath = checkpointDirectory + "/" + identifier + ".nlmcheckpoint";
        
        try {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return std::nullopt;
            }
            
            // Skip timestamp (first 8 bytes)
            file.seekg(sizeof(uint64_t), std::ios::beg);
            
            // Read data
            std::string data((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
            
            return data;
        } catch (...) {
            return std::nullopt;
        }
    }
    
    void ErrorHandler::Recovery::CheckpointManager::cleanup() {
        if (checkpointDirectory.empty()) return;
        
        try {
            std::filesystem::path dir(checkpointDirectory);
            if (!std::filesystem::exists(dir)) return;
            
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".nlmcheckpoint") {
                    auto fileTime = std::filesystem::last_write_time(entry.path());
                    auto fileTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                        fileTime.time_since_epoch());
                    
                    auto age = std::chrono::duration_cast<std::chrono::hours>(now_ms - fileTimeMs);
                    
                    // Remove checkpoints older than 24 hours
                    if (age.count() > 24) {
                        std::filesystem::remove(entry.path());
                    }
                }
            }
        } catch (...) {
            // Ignore cleanup errors
        }
    }
}
