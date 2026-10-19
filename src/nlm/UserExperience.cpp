// User Experience Enhancement - Implementation

#include "UserExperience.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace nlm {

// Initialize user experience
void initializeUserExperience() {
    UXUtilities::preferences = UXUtilities::UserPreferences();
    EnhancedTestFramework::testResults.clear();
    EnhancedTestFramework::testsRunning = false;
    EnhancedTestFramework::reportsEnabled = true;
    InteractiveUI::CommandProcessor::commands.clear();
    
    showWelcomeMessage();
    setupDefaultPreferences();
    setupDefaultCommands();
}

void setupDefaultPreferences() {
    UXUtilities::preferences.enableVerboseOutput = false;
    UXUtilities::preferences.enableProgressIndicators = true;
    UXUtilities::preferences.enableConfirmationPrompts = true;
    UXUtilities::preferences.enableDetailedReports = true;
    UXUtilities::preferences.outputFormat = "text";
    UXUtilities::preferences.logLevel = "info";
}

void setupDefaultCommands() {
    InteractiveUI::CommandProcessor::registerCommand({
        "help", "Show help information", 
        [](const std::vector<std::string>&) { InteractiveUI::CommandProcessor::printHelp(); return true; },
        {}
    });
    
    InteractiveUI::CommandProcessor::registerCommand({
        "quit", "Exit the application",
        [](const std::vector<std::string>&) { return false; }, // Signal to quit
        {}
    });
}

void showWelcomeMessage() {
    std::cout << "=== NLM Phase 6 Enhanced User Experience ===" << std::endl;
    std::cout << "Welcome to the Neural Learning Machine Phase 6 integration demo!" << std::endl;
    std::cout << "This enhanced version provides interactive controls, advanced analysis, and comprehensive reporting." << std::endl;
    std::cout << std::endl;
    std::cout << "Key Features:" << std::endl;
    std::cout << "  • Interactive brain exploration and control" << std::endl;
    std::cout << "  • Advanced analysis and profiling" << std::endl;
    std::cout << "  • Comprehensive session management" << std::endl;
    std::cout << "  • Detailed reporting and documentation" << std::endl;
    std::cout << "  • Performance benchmarking" << std::endl;
    std::cout << std::endl;
    std::cout << "For help, type 'help' or use --help flag." << std::endl;
    std::cout << std::endl;
}

void showHelpForNewUsers() {
    std::cout << "=== NLM Phase 6 User Help ===" << std::endl;
    std::cout << "Getting Started:" << std::endl;
    std::cout << "  1. Start with ./nlm_phase6_demo --interactive" << std::endl;
    std::cout << "  2. Type 'help' to see available commands" << std::endl;
    std::cout << "  3. Use 'status' to check brain health" << std::endl;
    std::cout << "  4. Use 'memory' to analyze memory usage" << std::endl;
    std::cout << "  5. Use 'synapses' to analyze connectivity" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Features:" << std::endl;
    std::cout << "  --interactive     Enable interactive control" << std::endl;
    std::cout << "  --advanced-analysis Enable advanced analysis" << std::endl;
    std::cout << "  --memory-profiling Enable memory profiling" << std::endl;
    std::cout << "  --neuromodulation-profiling Enable neuromodulation analysis" << std::endl;
    std::cout << "  --learning-analysis Enable learning analysis" << std::endl;
    std::cout << std::endl;
    std::cout << "Session Management:" << std::endl;
    std::cout << "  Sessions are automatically saved when you exit" << std::endl;
    std::cout << "  Check ~/.nlm/sessions/ for saved session history" << std::endl;
    std::cout << std::endl;
}

// UXUtilities implementation
void UXUtilities::showProgress(const std::string& task, float progress, int width) {
    if (!preferences.enableProgressIndicators) return;
    
    std::cout << "[" << task << "] ";
    int pos = static_cast<int>(width * progress);
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << " " << static_cast<int>(progress * 100.0f) << "%\r" << std::flush;
    
    if (progress >= 1.0f) {
        std::cout << std::endl;
    }
}

void UXUtilities::showStatus(const std::string& component, const std::string& status, 
                           const std::string& color) {
    if (preferences.enableVerboseOutput) {
        if (!color.empty()) {
            std::cout << color << "[" << component << "] " << status << " " << color << "\033[0m" << std::endl;
        } else {
            std::cout << "[" << component << "] " << status << std::endl;
        }
    }
}

bool UXUtilities::confirm(const std::string& question, bool defaultAnswer) {
    if (!preferences.enableConfirmationPrompts) return defaultAnswer;
    
    std::cout << question << " (y/n) [default: " << (defaultAnswer ? "y" : "n") << "]: ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) return defaultAnswer;
    
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input == "y" || input == "yes";
}

std::string UXUtilities::prompt(const std::string& question, const std::string& defaultValue) {
    std::cout << question;
    if (!defaultValue.empty()) {
        std::cout << " [default: " << defaultValue << "]:";
    }
    std::cout << " ";
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) return defaultValue;
    return input;
}

int UXUtilities::promptInt(const std::string& question, int min, int max) {
    while (true) {
        std::string input = prompt(question);
        try {
            int value = std::stoi(input);
            if (value >= min && value <= max) {
                return value;
            } else {
                std::cout << "Value must be between " << min << " and " << max << ". Try again." << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid integer. Try again." << std::endl;
        }
    }
}

float UXUtilities::promptFloat(const std::string& question, float min, float max) {
    while (true) {
        std::string input = prompt(question);
        try {
            float value = std::stof(input);
            if (value >= min && value <= max) {
                return value;
            } else {
                std::cout << "Value must be between " << min << " and " << max << ". Try again." << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid number. Try again." << std::endl;
        }
    }
}

void UXUtilities::UserPreferences::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = StringUtils::trim(line.substr(0, pos));
        std::string value = StringUtils::trim(line.substr(pos + 1));
        
        if (key == "enableVerboseOutput") {
            enableVerboseOutput = (value == "true" || value == "1");
        } else if (key == "enableProgressIndicators") {
            enableProgressIndicators = (value == "true" || value == "1");
        } else if (key == "enableConfirmationPrompts") {
            enableConfirmationPrompts = (value == "true" || value == "1");
        } else if (key == "enableDetailedReports") {
            enableDetailedReports = (value == "true" || value == "1");
        } else if (key == "outputFormat") {
            outputFormat = value;
        } else if (key == "logLevel") {
            logLevel = value;
        }
    }
}

void UXUtilities::UserPreferences::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "enableVerboseOutput=" << (enableVerboseOutput ? "true" : "false") << std::endl;
    file << "enableProgressIndicators=" << (enableProgressIndicators ? "true" : "false") << std::endl;
    file << "enableConfirmationPrompts=" << (enableConfirmationPrompts ? "true" : "false") << std::endl;
    file << "enableDetailedReports=" << (enableDetailedReports ? "true" : "false") << std::endl;
    file << "outputFormat=" << outputFormat << std::endl;
    file << "logLevel=" << logLevel << std::endl;
}

void UXUtilities::UserPreferences::printSettings() const {
    std::cout << "=== User Preferences ===" << std::endl;
    std::cout << "Verbose Output: " << (enableVerboseOutput ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Progress Indicators: " << (enableProgressIndicators ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Confirmation Prompts: " << (enableConfirmationPrompts ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Detailed Reports: " << (enableDetailedReports ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Output Format: " << outputFormat << std::endl;
    std::cout << "Log Level: " << logLevel << std::endl;
    std::cout << std::endl;
}

std::string UXUtilities::SessionManager::createSessionId() {
    return "nlm_session_" + StringUtils::formatNumber(static_cast<size_t>(std::chrono::system_clock::now().time_since_epoch().count()));
}

UXUtilities::SessionManager::SessionInfo UXUtilities::SessionManager::startSession(const std::string& configFile) {
    SessionInfo session;
    session.sessionId = createSessionId();
    session.startTime = std::chrono::system_clock::now();
    session.configFile = configFile;
    session.successful = false;
    session.totalSteps = 0;
    session.totalReward = 0.0f;
    session.neuronsUsed = 0;
    
    sessionHistory.push_back(session);
    return session;
}

void UXUtilities::SessionManager::endSession(const SessionInfo& sessionInfo, bool success) {
    auto it = std::find_if(sessionHistory.begin(), sessionHistory.end(),
                          [&sessionInfo](const SessionInfo& s) { return s.sessionId == sessionInfo.sessionId; });n    
    if (it != sessionHistory.end()) {
        it->endTime = std::chrono::system_clock::now();
        it->successful = success;
    }
}

void UXUtilities::SessionManager::saveSessionHistory(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "SessionId,StartTime,EndTime,ConfigFile,TotalSteps,TotalReward,NeuronsUsed,Successful" << std::endl;
    
    for (const auto& session : sessionHistory) {
        file << session.sessionId << ",";
        file << TimeUtils::formatTimestamp(session.startTime) << ",";
        file << (session.endTime.time_since_epoch().count() > 0 ? TimeUtils::formatTimestamp(session.endTime) : "") << ",";
        file << session.configFile << ",";
        file << session.totalSteps << ",";
        file << session.totalReward << ",";
        file << session.neuronsUsed << ",";
        file << (session.successful ? "true" : "false") << std::endl;
    }
}

std::vector<UXUtilities::SessionManager::SessionInfo> UXUtilities::SessionManager::loadSessionHistory(const std::string& filename) {
    std::vector<SessionInfo> history;
    std::ifstream file(filename);
    if (!file.is_open()) return history;
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        SessionInfo session;
        
        std::getline(ss, token, ','); session.sessionId = token;
        std::getline(ss, token, ','); 
        if (!token.empty()) session.startTime = TimeUtils::parseTimestamp(token);
        std::getline(ss, token, ',');
        if (!token.empty()) session.endTime = TimeUtils::parseTimestamp(token);
        std::getline(ss, token, ','); session.configFile = token;
        std::getline(ss, token, ','); session.totalSteps = std::stoull(token);
        std::getline(ss, token, ','); session.totalReward = std::stof(token);
        std::getline(ss, token, ','); session.neuronsUsed = std::stoul(token);
        std::getline(ss, token, ','); session.successful = (token == "true" || token == "1");
        
        history.push_back(session);
    }
    
    return history;
}

// EnhancedTestFramework implementation
void EnhancedTestFramework::addTest(const std::string& testName, std::function<bool()> testFunc) {
    testResults.push_back(TestResult(testName));
    testResults.back().passed = testFunc();
}

void EnhancedTestFramework::runAllTests() {
    testsRunning = true;
    
    // Run basic connectivity tests
    addTest("Brain Initialization", []() {
        auto cfg = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(cfg);
        return brain->initialize();
    });
    
    addTest("Memory System Integration", []() {
        auto cfg = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        return brain->getWorkingMemory() != nullptr && brain->getEpisodicMemory() != nullptr;
    });
    
    addTest("Neuromodulation Systems", []() {
        auto cfg = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        return brain->getDopamine() != nullptr && brain->getCuriosity() != nullptr;
    });
    
    testsRunning = false;
}

void EnhancedTestFramework::runPerformanceTests() {
    std::cout << "=== Performance Tests ===" << std::endl;
    
    // Test different brain sizes
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Testing " << neuronCount << " neurons...";
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
        cfg->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(cfg);
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (brain->initialize()) {
            for (uint64_t step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime);
            
            std::cout << " Initialized and ran 100 steps in " << duration.count() << "s" << std::endl;
        } else {
            std::cout << " FAILED" << std::endl;
        }
    }
}

void EnhancedTestFramework::runIntegrationTests() {
    std::cout << "=== Integration Tests ===" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 500);
    cfg->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cout << "FAILED: Brain initialization failed" << std::endl;
        return;
    }
    
    // Test memory system functionality
    brain->getWorkingMemory()->storeToNeuron(1, 0.5f);
    if (brain->getWorkingMemory()->getActiveTraces() == 0) {
        std::cout << "FAILED: Working memory not storing traces" << std::endl;
        return;
    }
    
    // Test episodic memory
    EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.5f;
    episode.activeNeurons.push_back(1);
    episode.neuronActivations.push_back(0.8f);
    brain->getEpisodicMemory()->storeEpisode(episode);
    
    if (brain->getEpisodicMemory()->getEpisodeCount() == 0) {
        std::cout << "FAILED: Episodic memory not storing episodes" << std::endl;
        return;
    }
    
    // Test neuromodulation
    brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
    brain->step(1, 0.001);
    
    std::cout << "PASSED: Integration tests completed successfully" << std::endl;
}

void EnhancedTestFramework::runRegressionTests() {
    std::cout << "=== Regression Tests ===" << std::endl;
    std::cout << "Running regression tests... (feature not yet implemented)" << std::endl;
}

bool EnhancedTestFramework::testFileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool EnhancedTestFramework::testDirectoryExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

bool EnhancedTestFramework::compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary | std::ios::ate);
    std::ifstream f2(file2, std::ios::binary | std::ios::ate);
    
    if (!f1.is_open() || !f2.is_open()) return false;
    
    return f1.tellg() == f2.tellg();
}

std::string EnhancedTestFramework::generateTestData(size_t size) {
    std::string data;
    data.reserve(size);
    
    for (size_t i = 0; i < size; ++i) {
        data.push_back(static_cast<char>('A' + (i % 26)));
    }
    
    return data;
}

bool EnhancedTestFramework::validateTestData(const std::string& data) {
    return !data.empty() && data.length() <= 1000000; // Max 1MB
}

void EnhancedTestFramework::generateTestReport(const std::string& filename, const std::vector<TestResult>& results) {
    if (!reportsEnabled) return;
    
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "=== NLM Test Results ===" << std::endl;
    file << "Generated: " << TimeUtils::getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    int passed = 0;
    for (const auto& test : results) {
        file << test.testName << ": " << (test.passed ? "PASSED" : "FAILED") << std::endl;
        if (test.passed) passed++;
        file << "  Message: " << test.message << std::endl;
        file << "  Execution Time: " << test.executionTime.count() << "s" << std::endl;
        file << std::endl;
    }
    
    file << "=== Summary ===" << std::endl;
    file << "Total Tests: " << results.size() << std::endl;
    file << "Passed: " << passed << std::endl;
    file << "Failed: " << (results.size() - passed) << std::endl;
    file << "Success Rate: " << (passed * 100.0f / results.size()) << "%" << std::endl;
}

void EnhancedTestFramework::generatePerformanceReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "=== NLM Performance Results ===" << std::endl;
    file << "Generated: " << TimeUtils::getCurrentTimestamp() << std::endl;
    file << std::endl;
    file << "Performance testing would be implemented here." << std::endl;
}

void EnhancedTestFramework::generateComparisonReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "=== NLM Comparison Results ===" << std::endl;
    file << "Generated: " << TimeUtils::getCurrentTimestamp() << std::endl;
    file << std::endl;
    file << "Comparison testing would be implemented here." << std::endl;
}

// StringUtils implementation
std::string StringUtils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string StringUtils::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    
    return str.substr(start, end - start);
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

std::string StringUtils::formatNumber(size_t number, char thousandsSeparator) {
    std::stringstream ss;
    ss << number;
    std::string str = ss.str();
    
    size_t pos = str.size();
    while (pos > 3 && thousandsSeparator != '\0') {
        pos -= 3;
        str.insert(pos, 1, thousandsSeparator);
    }
    
    return str;
}

std::string StringUtils::formatFloat(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string StringUtils::colorize(const std::string& text, const std::string& color) {
    if (color == "red") return "\033[31m" + text + "\033[0m";
    else if (color == "green") return "\033[32m" + text + "\033[0m";
    else if (color == "yellow") return "\033[33m" + text + "\033[0m";
    else if (color == "blue") return "\033[34m" + text + "\033[0m";
    else if (color == "magenta") return "\033[35m" + text + "\033[0m";
    else if (color == "cyan") return "\033[36m" + text + "\033[0m";
    else if (color == "white") return "\033[37m" + text + "\033[0m";
    else return text;
}

// FileUtils implementation
bool FileUtils::ensureDirectoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // Directory does not exist
        if (mkdir(path.c_str(), 0755) != 0) {
            return false;
        }
    } else if (!(info.st_mode & S_IFDIR)) {
        // Path exists but is not a directory
        return false;
    }
    return true;
}

std::string FileUtils::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos || pos == filename.size() - 1) {
        return "";
    }
    return filename.substr(pos + 1);
}

std::string FileUtils::getBaseFilename(const std::string& filename) {
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        return filename.substr(pos + 1);
    }
    return filename;
}

std::string FileUtils::getTimestampString() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

std::string FileUtils::createUniqueFilename(const std::string& base, const std::string& extension) {
    return base + "_" + getTimestampString() + 
           (extension.empty() ? "" : "." + extension);
}

bool FileUtils::safeCopy(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(destination, std::ios::binary);
    
    if (!src || !dst) return false;
    
    dst << src.rdbuf();
    return src.good() && dst.good();
}

bool FileUtils::safeMove(const std::string& source, const std::string& destination) {
    return std::rename(source.c_str(), destination.c_str()) == 0;
}

// TimeUtils implementation
std::string TimeUtils::formatDuration(std::chrono::duration<double> duration) {
    double seconds = duration.count();
    
    if (seconds < 60) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600) {
        int minutes = static_cast<int>(seconds / 60);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    }
}

std::string TimeUtils::formatTimestamp(const std::chrono::system_clock::time_point& timePoint) {
    auto in_time_t = std::chrono::system_clock::to_time_t(timePoint);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point TimeUtils::parseTimestamp(const std::string& timestamp) {
    std::tm tm = {};
    std::stringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return timePoint;
}

double TimeUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration<double>(epoch).count();
}

} // namespace nlm
