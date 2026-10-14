std::string NLMCompareTool::getDescription() const {
    return "Compare different simulation results for NLM. Supports comparison of firing rates, spike counts, and performance metrics.";
}

std::string NLMCompareTool::getVersion() const {
    return "1.0.0";
}

std::string NLMCompareTool::getUsage() const {
    return "nlm_compare <command> [options]"; 
}

void NLMCompareTool::shutdown() {
    if (logger_) {
        Logger::setGlobal(nullptr);
        logger_.reset();
    }
}

void NLMCompareTool::log(const std::string& message, bool error) const {
    std::cout << (error ? "[ERROR] " : "[INFO] ") << message << std::endl;
}

} // namespace nlm