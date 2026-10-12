#include <sstream>
#include <string>
#include <vector>
#include <iostream>

// Helper function to format any type for ostream output
template<typename T>
std::string format_arg(const T& arg) {
    std::ostringstream oss;
    oss << arg;
    return oss.str();
}

// Specialization for strings to handle quotes
void format_string_arg(std::ostringstream& oss, const std::string& arg) {
    oss << "\"" << arg << "\"";
}

// Specialization for vector of strings
void format_string_vector_arg(std::ostringstream& oss, const std::vector<std::string>& arg) {
    oss << "[\"";
    for (size_t i = 0; i < arg.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << arg[i] << "\"";
    }
    oss << "]";
}

// Specialization for vector of ints
void format_int_vector_arg(std::ostringstream& oss, const std::vector<int>& arg) {
    oss << "[";
    for (size_t i = 0; i < arg.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arg[i];
    }
    oss << "]";
}

// Specialization for vector of doubles
void format_double_vector_arg(std::ostringstream& oss, const std::vector<double>& arg) {
    oss << "[";
    for (size_t i = 0; i < arg.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arg[i];
    }
    oss << "]";
}

// Template version for other types
template<typename T>
void format_other_arg(std::ostringstream& oss, const T& arg) {
    oss << arg;
}
