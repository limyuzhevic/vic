// SystemError implementation
namespace nlm {

SystemError::SystemError(const std::string& message,
                       SystemErrorType type,
                       const std::string& subsystem,
                       const std::string& suggestion)
    : NLMError("System Error: " + message, ErrorSeverity::Critical, ErrorCategory::System),
      systemErrorType_(type),
      subsystem_(subsystem),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string SystemError::getRecoveryHint() const {
    switch (systemErrorType_) {
        case SystemErrorType::FileSystemError:
            return "Check file system. Fix file system error or use alternative file system.";
        case SystemErrorType::NetworkError:
            return "Check network connectivity. Fix network error or use alternative network.";
        case SystemErrorType::ProcessError:
            return "Check process. Fix process error or restart process.";
        case SystemErrorType::ThreadError:
            return "Check thread. Fix thread error or recreate thread.";
        case SystemErrorType::SynchronizationError:
            return "Check synchronization. Fix synchronization error or increase timeout.";
        case SystemErrorType::ResourceUnavailable:
            return "Release resources or check resource availability. Use alternative resources.";
        case SystemErrorType::PermissionDenied:
            return "Check permissions. Grant required permissions or use alternative account.";
        case SystemErrorType::TimeoutError:
            return "Increase timeout value or optimize performance. Check system resources.";
        case SystemErrorType::DeadlockDetected:
            return "Check deadlock. Fix deadlock or restart system.";
        case SystemErrorType::SignalError:
            return "Check signal handling. Fix signal error or use alternative signal handling.";
        case SystemErrorType::InterruptError:
            return "Check interrupt handling. Fix interrupt error or use alternative interrupt handling.";
        default:
            return "Check system resources. Fix system error or restart system.";
    }
}

} // namespace nlm