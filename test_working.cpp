#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

struct SimpleLogger {
    std::ostringstream oss;
    
    template<typename T>
    SimpleLogger& operator<<(const T& value) {
        oss << value;
        return *this;
    }
};

struct ConfigValue {
    std::vector<std::string> vec = {"test", "vector"};
};

int main() {
    SimpleLogger sl;
    ConfigValue cv;
    
    sl << "vector: [";
    for (size_t i = 0; i < cv.vec.size(); ++i) {
        if (i > 0) sl << ", ";
        sl << cv.vec[i];
    }
    sl << "]";
    
    std::cout << sl.oss.str() << std::endl;
    return 0;
}
