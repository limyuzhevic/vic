#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

// Command execution result
struct CommandResult {
    bool success;
    std::string output;
    std::string errorMessage;
    std::chrono::milliseconds executionTime;
    std::map<std::string, std::string> metadata;
    
    CommandResult() : success(false), output(), errorMessage(), executionTime(0), metadata() {}
    CommandResult(bool s, const std::string& out, const std::string& err = "") 
        : success(s), output(out), errorMessage(err), executionTime(0), metadata() {}
};

// Command execution status
enum class CommandStatus {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled
};

// Base command interface
class CommandBase {
public:
    virtual ~CommandBase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual std::string getHelp() const = 0;
    
    virtual CommandStatus getStatus() const = 0;
    virtual double getProgress() const = 0;
    virtual std::string getProgressText() const = 0;
    
    virtual bool execute() = 0;
    virtual bool cancel() = 0;
    virtual bool isCancellable() const = 0;
    
    virtual CommandResult getResult() const = 0;
    virtual std::vector<std::string> getArguments() const = 0;
    virtual void setArguments(const std::vector<std::string>& args) = 0;
    
    virtual bool validateArguments() const = 0;
    virtual std::string getLastError() const = 0;
    
    virtual std::chrono::milliseconds getEstimatedExecutionTime() const = 0;
    
    // Progress callback
    using ProgressCallback = std::function<void(double, const std::string&)>;
    
    virtual void setProgressCallback(const ProgressCallback& callback) = 0;
    
    // Command categories
    enum class Category {
        General,
        Processing,
        ExportImport,
        Configuration,
        Debugging,
        Monitoring,
        Performance
    };
    
    virtual Category getCategory() const = 0;
    
    // Command metadata
    virtual std::map<std::string, std::string> getMetadata() const = 0;
    
protected:
    mutable CommandStatus status = CommandStatus::Pending;
    mutable CommandResult result;
    ProgressCallback progressCallback;
    std::chrono::steady_clock::time_point startTime;
    
    void setStatus(CommandStatus s) const {
        status = s;
        if (progressCallback) {
            progressCallback(getProgress(), getProgressText());
        }
    }
    
    void setResult(bool success, const std::string& output, const std::string& error = "") {
        result = CommandResult(success, output, error);
        if (progressCallback) {
            progressCallback(success ? 1.0 : 0.0, success ? "Completed" : "Failed");
        }
    }
    
    void startTiming() {
        startTime = std::chrono::steady_clock::now();
    }
    
    void stopTiming() {
        auto endTime = std::chrono::steady_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    }
};

} // namespace nlm