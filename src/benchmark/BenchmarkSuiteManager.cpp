#include "benchmark/BenchmarkSuiteManager.hpp"
#include <iomanip>
#include <limits>
#include <sstream>

namespace nlm {

// DefaultBenchmarkSuiteManager implementation
void DefaultBenchmarkSuiteManager::saveResultToJson(std::ofstream& file, const BenchmarkResult& result) {
    file << "    {\n";
    file << "      \"benchmark_id\": \"" << result.benchmark_id << "\",\n";
    file << "      \"benchmark_name\": \"" << result.benchmark_name << "\",\n";
    file << "      \"benchmark_type\": \"" << result.benchmark_type << "\",\n";
    file << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
    file << "      \"total_time\": " << result.total_time << ",\n";
    file << "      \"memory_usage\": " << result.memory_usage << ",\n";
    file << "      \"cpu_usage\": " << result.cpu_usage << ",\n";
    file << "      \"error_message\": \"" << result.error_message << "\"\n";
    file << "    }";
}

} // namespace nlm