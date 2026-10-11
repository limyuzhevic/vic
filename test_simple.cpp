#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

struct LogStream {
    std::ostringstream oss;
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss << value;
        return *this;
    }
};

struct ConfigValue {
    std::vector<std::string> vec;
};

int main() {
    LogStream ls;
    ConfigValue cv = {"test", "vector"};
    ls << cv.vec;
    std::cout << ls.oss.str() << std::endl;
    return 0;
}
