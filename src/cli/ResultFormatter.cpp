// NLM CLI Result Formatter
// Formats command results for different output formats

#include "ResultFormatter.hpp"
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace nlm;

ResultFormatter::ResultFormatter() = default;

ResultFormatter::~ResultFormatter() = default;

std::string ResultFormatter::format(const CommandResult& result, const std::string& format) {
    if (format == "plaintext") {
        return formatPlaintext(result);
    } else if (format == "json") {
        return formatJSON(result);
    } else if (format == "table") {
        return formatTable(result);
    } else if (format == "csv") {
        return formatCSV(result);
    } else {
        return formatPlaintext(result);
    }
}

std::string ResultFormatter::formatPlaintext(const CommandResult& result) {
    std::stringstream ss;
    
    if (result.success) {
        ss << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        ss << "║                    Command Executed Successfully                    ║" << std::endl;
        ss << "╠══════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        if (!result.output.empty()) {
            ss << "║ Output:" << std::endl;
            ss << "║ " << result.output << std::endl;
        }
        
        ss << "╚═════════════════════════════════════════════════════════════════╝" << std::endl;
    } else {
        ss << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        ss << "║                     Command Execution Failed                     ║" << std::endl;
        ss << "╠══════════════════════════════════════════════════════════════════════╣" << std::endl;
        ss << "║ Error:" << std::endl;
        ss << "║ " << result.errorMessage << std::endl;
        ss << "╚═════════════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    return ss.str();
}

std::string ResultFormatter::formatJSON(const CommandResult& result) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
    
    if (result.success) {
        ss << "  \"output\": \"" << escapeJSON(result.output) << "\",\n";
    }
    
    ss << "  \"error\": \"" << escapeJSON(result.errorMessage) << "\"\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ResultFormatter::formatTable(const CommandResult& result) {
    std::stringstream ss;
    
    ss << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                    Command Execution Result                       ║" << std::endl;
    ss << "╠══════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    if (result.success) {
        ss << "║ Status      ║ " << std::setw(22) << std::left << "SUCCESS" << " ║" << std::endl;
    } else {
        ss << "║ Status      ║ " << std::setw(22) << std::left << "FAILED" << " ║" << std::endl;
    }
    
    ss << "║ Output      ║ " << std::setw(22) << std::left << (result.output.empty() ? "(none)" : "(see below)") << " ║" << std::endl;
    
    if (result.success && !result.output.empty()) {
        // Format output in rows
        std::vector<std::string> lines = splitLines(result.output);
        ss << "║ Output Lines║ " << std::setw(22) << std::left << std::to_string(lines.size()) << " ║" << std::endl;
        
        // Show first few lines
        int linesToShow = std::min(3, static_cast<int>(lines.size()));
        for (int i = 0; i < linesToShow; ++i) {
            std::string line = lines[i];
            if (line.length() > 48) {
                line = line.substr(0, 45) + "...";
            }
            ss << "║             ║ " << std::setw(22) << std::left << line << " ║" << std::endl;
        }
        
        if (lines.size() > 3) {
            ss << "║             ║ ... and " << (lines.size() - 3) << " more lines    ║" << std::endl;
        }
    }
    
    ss << "║ Error       ║ " << std::setw(22) << std::left << result.errorMessage << " ║" << std::endl;
    ss << "╚═════════════════════════════════════════════════════════════════╝" << std::endl;
    
    return ss.str();
}

std::string ResultFormatter::formatCSV(const CommandResult& result) {
    std::stringstream ss;
    ss << "success,output,error" << std::endl;
    ss << result.success << ",\" << escapeCSV(result.output) << "\"," << escapeCSV(result.errorMessage) << std::endl;
    return ss.str();
}

std::vector<std::string> ResultFormatter::splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;
    
    while (std::getline(ss, line, '\n')) {
        lines.push_back(line);
    }
    
    return lines;
}

std::string ResultFormatter::escapeJSON(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    std::stringstream ss;
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    result += ss.str();
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string ResultFormatter::escapeCSV(const std::string& text) {
    std::string result;
    if (text.find(',') != std::string::npos || text.find('"') != std::string::npos || text.find('\n') != std::string::npos) {
        result += "\"";
        for (char c : text) {
            if (c == '"') {
                result += "\"\"";
            } else {
                result += c;
            }
        }
        result += "\"";
    } else {
        result = text;
    }
    return result;
}