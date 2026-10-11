#include "Errors.hpp"

namespace nlm {
    std::vector<std::shared_ptr<ErrorReporter>> ErrorReporterRegistry::reporters_;
}
