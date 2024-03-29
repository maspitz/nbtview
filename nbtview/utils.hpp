/**
 * @file utils.hpp
 * @brief Fundamental conversion routines for NBT data
 * @author Michael Spitznagel
 * @copyright Copyright 2023 Michael Spitznagel. Released under the Boost
 * Software License 1.0
 *
 * https://github.com/maspitz/nbtview
 */

#ifndef NBT_UTILS_H_
#define NBT_UTILS_H_

#include <regex>
#include <string>
#include <vector>

namespace nbtview {

inline bool snbt_requires_quoting(const std::string &str) {
    std::regex pattern("^[a-zA-Z0-9_\\-\\.\\+]*$");
    return std::regex_match(str, pattern) == false;
}

inline std::string quoted_string(const std::string &str) {
    std::regex pattern("\"");
    // std::regex replacement("\\\"");
    return "\"" + std::regex_replace(str, pattern, "\\\"") + "\"";
}

/**
 * @brief Parameter object which provides options for formatting arrays.
 */
struct array_format {
    const std::string
        array_prefix; /**< The prefix to be added before the array. */
    const std::string
        array_suffix; /**< The suffix to be added after the array. */
    const std::string
        elt_suffix; /**< The suffix to be added after each array element. */
};

/**
 * @brief Generates a comma-delimited string representation of an array.
 *
 * @tparam Type of elements in the vector.
 * @param vec The input array.
 * @param fmt The formatting configuration.
 * @return The output string representating the vector.
 */
template <typename T>
std::string comma_delimited_array(const std::vector<T> &vec,
                                  const array_format &fmt) {
    std::string output_string;
    for (size_t i = 0; i != vec.size(); ++i) {
        if (output_string.empty()) {
            output_string = fmt.array_prefix;
        } else {
            output_string += ",";
        }
        output_string += std::to_string(vec[i]) + fmt.elt_suffix;
    }
    output_string += fmt.array_suffix;
    return output_string;
}

} // namespace nbtview

#endif // NBT_UTILS_H_
