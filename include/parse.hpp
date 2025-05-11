#pragma once

#include <charconv>
#include <concepts>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {

template <typename T>
inline std::expected<T, scan_error> parse_value_with_format(std::string_view input,
                                                            std::string_view fmt) {
    return std::unexpected(scan_error{"Unsupported type"});
}

template <typename T>
concept IntegrTypes = std::same_as<T, int8_t> || std::same_as<T, int16_t> ||
                      std::same_as<T, int32_t> || std::same_as<T, int64_t>;
template <typename T>
concept UnsignedIntegrTypes = std::same_as<T, uint8_t> || std::same_as<T, uint16_t> ||
                              std::same_as<T, uint32_t> || std::same_as<T, uint64_t>;
template <typename T>
concept FloatingTypes = std::same_as<T, float> || std::same_as<T, double>;

template <typename T>
    requires IntegrTypes<T> || UnsignedIntegrTypes<T> || FloatingTypes<T>
inline std::expected<T, scan_error> parse_value_with_format(std::string_view input,
                                                            std::string_view fmt) {
    if (input.empty()) {
        return std::unexpected(scan_error{"Input is empty"});
    }

    auto unexpected_type_error = [](std::string_view format) -> std::expected<T, scan_error> {
        return std::unexpected(scan_error{"Unexpected type "
                                          "format " +
                                          std::string(format)});
    };

    if constexpr (IntegrTypes<T>) {
        if (!(fmt == "%d" || fmt.empty())) {
            return unexpected_type_error(fmt);
        }
    } else if constexpr (UnsignedIntegrTypes<T>) {
        if ((fmt != "%u" || fmt.empty())) {
            return unexpected_type_error(fmt);
        }
    } else if constexpr (FloatingTypes<T>) {
        if ((fmt != "%f" || fmt.empty())) {
            return unexpected_type_error(fmt);
        }
    }

    T value;
    auto result = std::from_chars(input.data(), input.data() + input.size(), value);

    if (result.ec == std::errc()) {
        return value;
    } else if (result.ec == std::errc::invalid_argument) {
        return std::unexpected(scan_error{"Invalid argument"});
    } else if (result.ec == std::errc::result_out_of_range) {
        return std::unexpected(scan_error{"Out of range"});
    }

    return std::unexpected(scan_error{"Unspecified error"});
}

template <>
inline std::expected<std::string_view, scan_error> parse_value_with_format(std::string_view input,
                                                                           std::string_view fmt) {
    if (input.empty()) {
        return std::unexpected(scan_error{"Input is empty"});
    }

    if (!(fmt == "%s" || fmt.empty())) {
        return std::unexpected(scan_error{"Unexpected type "
                                          "format " +
                                          std::string(fmt)});
    }
    return input;
}

// Функция для проверки корректности
// входных данных и выделения из обеих
// строк интересующих данных для
// парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата
                                                 // между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и
        // текущей { есть текст,
        // проверяем его наличие во
        // входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted "
                                                  "text in input "
                                                  "and format "
                                                  "string are "
                                                  "different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор
        // формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после
    // последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text "
                                              "in input and "
                                              "format string are "
                                              "different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

}  // namespace stdx::details