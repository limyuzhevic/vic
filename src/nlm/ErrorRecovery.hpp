#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <any>
#include <functional>
#include <chrono>
#include <iostream>

namespace nlm {

// Error recovery system for NLM

class ErrorRecoveryManager {
public:
    ErrorRecoveryManager() : error_count_(0), last_error_timestamp_(0) {}
    
    void record_error(const NLMError& error);
    void backup_state(const std::string& component, const std::map<std::string, std::any>& state);
    std::map<std::string, std::any> restore_state(const std::string& component);
    void add_recovery_action(const std::string& error_code, std::function<void(const NLMError&)> action);
    void clear_recovery_actions();
    void clear_state_backups();
    
    size_t get_error_count() const { return error_count_; }
    const NLMError* get_last_error() const { return last_error_.c_str() ? last_error_.data() : nullptr; }
    std::chrono::system_clock::time_point get_last_error_timestamp() const { return last_error_timestamp_; }
    std::map<std::string, std::any> get_state_backup(const std::string& component) const;
    std::vector<std::string> get_error_codes() const;
    
    void clear_last_error() { last_error_.clear(); last_error_timestamp_ = std::chrono::system_clock::time_point(); }
    bool has_recovery_action(const std::string& error_code) const;
    bool has_state_backup(const std::string& component) const;
    
    void log_error(const NLMError& error);
    std::string get_error_summary() const;
    
    // Error context management
    void add_error_context(const std::string& context_key, const std::any& context_value);
    std::any get_error_context(const std::string& context_key) const;
    void clear_error_contexts();
    
    // Recovery statistics
    size_t get_recovery_action_count(const std::string& error_code) const;
    double get_recovery_success_rate(const std::string& error_code) const;
    std::map<std::string, size_t> get_recovery_statistics() const;
    
    // State management
    void save_recovery_state(const std::string& filename);
    void load_recovery_state(const std::string& filename);
    void export_recovery_state(std::ostream& output) const;
    void import_recovery_state(std::istream& input);
    
    // Configuration
    void set_auto_recover(bool enabled) { auto_recover_ = enabled; }
    bool get_auto_recover() const { return auto_recover_; }
    
    void set_max_state_backups(size_t max_backups) { max_state_backups_ = max_backups; }
    size_t get_max_state_backups() const { return max_state_backups_; }
    
    void set_error_threshold(size_t threshold) { error_threshold_ = threshold; }
    size_t get_error_threshold() const { return error_threshold_; }
    
private:
    size_t error_count_;
    std::chrono::system_clock::time_point last_error_timestamp_;
    std::string last_error_; // Simple storage for last error
    std::map<std::string, std::function<void(const NLMError&)>> recovery_actions_;
    std::map<std::string, std::map<std::string, std::any>> state_backups_;
    std::map<std::string, std::any> error_contexts_;
    bool auto_recover_;
    size_t max_state_backups_;
    size_t error_threshold_;
    
    std::chrono::system_clock::time_point get_current_timestamp() const {
        return std::chrono::system_clock::now();
    }
    
    void trim_state_backups();
    size_t count_recovery_action_executions(const std::string& error_code) const;
    size_t count_recovery_action_successes(const std::string& error_code) const;
};

// Context manager for safe NLM operations

class SafeNLMOperation {
public:
    SafeNLMOperation(const std::string& operation_name, 
                    std::shared_ptr<ErrorRecoveryManager> recovery_manager = nullptr,
                    std::function<bool(const NLMError&)> error_handler = nullptr)
        : operation_name_(operation_name), 
          recovery_manager_(recovery_manager),
          error_handler_(error_handler) {}
    
    template<typename Func, typename... Args>
    auto execute(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        auto start_time = std::chrono::system_clock::now();
        
        try {
            // Execute operation
            auto result = func(std::forward<Args>(args)...);
            
            // Record successful execution
            record_operation_success(start_time);
            
            return result;
            
        } catch (const NLMError& e) {
            // Handle NLM errors
            handle_error(e);
            throw; // Re-throw after handling
            
        } catch (const std::exception& e) {
            // Convert to NLMError and handle
            NLMError nlm_error(std::string("Operation ')') " + operation_name_ + "' failed: " + std::string(e.what()), "UNKNOWN_ERROR");
            handle_error(nlm_error);
            throw nlm_error;
        }
    }
    
    template<typename Func, typename... Args>
    auto execute_safely(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        auto start_time = std::chrono::system_clock::now();
        
        try {
            // Execute operation with error recovery
            auto result = func(std::forward<Args>(args)...);
            
            // Record successful execution
            record_operation_success(start_time);
            
            return result;
            
        } catch (const NLMError& e) {
            // Try automatic recovery
            if (recovery_manager_ && recovery_manager_->get_auto_recover()) {
                if (attempt_recovery(e)) {
                    // Recovery successful, retry operation
                    return execute_safely(std::forward<Func>(func), std::forward<Args>(args)...);
                }
            }
            
            // Handle error
            handle_error(e);
            throw;
            
        } catch (const std::exception& e) {
            // Convert to NLMError
            NLMError nlm_error(std::string("Safe operation '('") + operation_name_ + "' failed: " + std::string(e.what()), "UNKNOWN_ERROR");
            
            // Try automatic recovery
            if (recovery_manager_ && recovery_manager_->get_auto_recover()) {
                if (attempt_recovery(nlm_error)) {
                    // Recovery successful, retry operation
                    return execute_safely(std::forward<Func>(func), std::forward<Args>(args)...);
                }
            }
            
            // Handle error
            handle_error(nlm_error);
            throw nlm_error;
        }
    }
    
    // Context manager interface
    SafeNLMOperation& enter() {
        record_operation_start();
        return *this;
    }
    
    template<typename Func, typename... Args>
    auto with_context(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        return enter().execute(std::forward<Func>(func), std::forward<Args>(args)...);
    }
    
    template<typename Func, typename... Args>
    auto with_safety(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        return enter().execute_safely(std::forward<Func>(func), std::forward<Args>(args)...);
    }
    
    void exit() {
        record_operation_end();
    }
    
    // Error handling
    void handle_error(const NLMError& error) {
        if (recovery_manager_) {
            recovery_manager_->record_error(error);
        }
        
        if (error_handler_) {
            error_handler_(error);
        } else {
            // Default error handling
            std::cerr << "[NLM ERROR] " << error.what() << std::endl;
        }
    }
    
    bool attempt_recovery(const NLMError& error) {
        if (!recovery_manager_) return false;
        
        std::string error_code = error.getErrorCode();
        if (recovery_manager_->has_recovery_action(error_code)) {
            try {
                recovery_manager_->execute_recovery_action(error_code, error);
                return true;
            } catch (...) {
                // Recovery failed
                return false;
            }
        }
        
        return false;
    }
    
    // State management
    void backup_operation_state(const std::string& component, const std::map<std::string, std::any>& state) {
        if (recovery_manager_) {
            recovery_manager_->backup_state(component, state);
        }
    }
    
    void restore_operation_state(const std::string& component) {
        if (recovery_manager_) {
            recovery_manager_->restore_state(component);
        }
    }
    
    // Statistics
    double get_success_rate() const { return success_rate_; }
    double get_failure_rate() const { return 1.0 - success_rate_; }
    size_t get_total_operations() const { return total_operations_; }
    size_t get_successful_operations() const { return successful_operations_; }
    size_t get_failed_operations() const { return failed_operations_; }
    
private:
    std::string operation_name_;
    std::shared_ptr<ErrorRecoveryManager> recovery_manager_;
    std::function<bool(const NLMError&)> error_handler_;
    
    size_t total_operations_;
    size_t successful_operations_;
    size_t failed_operations_;
    double success_rate_;
    std::chrono::system_clock::time_point operation_start_time_;
    
    void record_operation_start() {
        operation_start_time_ = std::chrono::system_clock::now();
    }
    
    void record_operation_success(const std::chrono::system_clock::time_point& start_time) {
        auto end_time = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        total_operations_++;
        successful_operations_++;
        success_rate_ = (successful_operations_ / static_cast<double>(total_operations_)) * 100.0;
        
        // Log successful operation
        std::cout << "[NLM SUCCESS] Operation '" << operation_name_ << "' completed in " << duration << "ms" << std::endl;
    }
    
    void record_operation_end() {
        exit(); // Call parent's exit
    }
};

// Legacy compatibility layer

class LegacyErrorHandler {
public:
    LegacyErrorHandler() : recovery_manager_(std::make_shared<ErrorRecoveryManager>()) {}
    
    template<typename Func, typename... Args>
    auto execute_safely(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        SafeNLMOperation operation("LegacyOperation", recovery_manager_);
        return operation.execute_safely(std::forward<Func>(func), std::forward<Args>(args)...);
    }
    
    template<typename Func, typename... Args>
    auto execute_with_context(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        SafeNLMOperation operation("LegacyContextOperation", recovery_manager_);
        return operation.with_context(std::forward<Func>(func), std::forward<Args>(args)...);
    }
    
    std::shared_ptr<ErrorRecoveryManager> get_recovery_manager() const { return recovery_manager_; }
    
private:
    std::shared_ptr<ErrorRecoveryManager> recovery_manager_;
};

} // namespace nlm