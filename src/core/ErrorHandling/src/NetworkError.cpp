// NetworkError implementation
namespace nlm {

NetworkError::NetworkError(const std::string& message,
                         NetworkErrorType type,
                         const std::string& endpoint,
                         const std::string& suggestion)
    : NLMError("Network Error: " + message, ErrorSeverity::Error, ErrorCategory::Network),
      networkErrorType_(type),
      endpoint_(endpoint),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string NetworkError::getRecoveryHint() const {
    switch (networkErrorType_) {
        case NetworkErrorType::ConnectionFailed:
            return "Check network connectivity. Fix connection error or use alternative endpoint.";
        case NetworkErrorType::Timeout:
            return "Increase timeout value or optimize network performance. Check network latency.";
        case NetworkErrorType::PacketLoss:
            return "Check network reliability. Use retransmission or increase reliability.";
        case NetworkErrorType::BandwidthExceeded:
            return "Reduce bandwidth usage or increase bandwidth. Check network congestion.";
        case NetworkErrorType::ProtocolError:
            return "Check protocol. Fix protocol error or use alternative protocol.";
        case NetworkErrorType::DNSResolutionFailed:
            return "Check DNS configuration. Use alternative DNS server or check hostnames.";
        case NetworkErrorType::AuthenticationFailed:
            return "Check authentication. Fix authentication error or use alternative credentials.";
        case NetworkErrorType::RateLimitExceeded:
            return "Reduce request rate or increase rate limit. Check rate limiting configuration.";
        case NetworkErrorType::ConnectionDropped:
            return "Check connection. Fix connection drop or reconnect.";
        case NetworkErrorType::MalformedPacket:
            return "Check packet format. Fix packet format or use packet validation.";
        default:
            return "Check network connectivity. Fix network error or use alternative network.";
    }
}

} // namespace nlm