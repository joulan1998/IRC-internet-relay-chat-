#include "inlcudes.hpp"

int string_to_int(const std::string &str)
{
    std::istringstream iss(str);
    int value;

    // Try to read an int from the stream
    if (!(iss >> value))
    {
        throw std::invalid_argument("Invalid integer string: " + str);
    }

    return value;
}