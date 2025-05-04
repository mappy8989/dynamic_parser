#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input,
                                                     std::string_view fmt) {
  if (input.empty()) {
    return std::unexpected(scan_error{"Input is empty"});
  }

  T ret_val;
  std::size_t pos = 0;
  std::string type_name;

  try {
    if (fmt == "%d") {
      ret_val = std::stoi(input.data(), &pos);
      type_name = "int";
    } else if (fmt == "%f") {
      ret_val = std::stod(input.data(), &pos);
      type_name = "float";
    } else if (fmt == "%u") {
      ret_val = std::stoul(input.data(), &pos);
      type_name = "unsigned int";
    } else {
      return std::unexpected(
          scan_error{"Unexpected type format " + std::string(fmt)});
    }

    if (pos != input.size()) {
      return std::unexpected(scan_error{
          "Unable to format " + std::string(input) + " to " + type_name});
    }
  } catch (...) {
    return std::unexpected(
        scan_error{"Unable to format " + std::string(input) + " to int"});
  }

  return ret_val;
}

template <>
inline std::expected<std::string, scan_error>
parse_value_with_format(std::string_view input, std::string_view fmt) {
  if (input.empty()) {
    return std::unexpected(scan_error{"Input is empty"});
  }

  if (fmt != "%s") {
    return std::unexpected(
        scan_error{"Unexpected type format" + std::string(fmt)});
  }
  return std::string(input);
}

// Функция для проверки корректности входных данных и выделения из обеих строк
// интересующих данных для парсинга
template <typename... Ts>
std::expected<
    std::pair<std::vector<std::string_view>, std::vector<std::string_view>>,
    scan_error>
parse_sources(std::string_view input, std::string_view format) {
  std::vector<std::string_view> format_parts; // Части формата между {}
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

    // Если между предыдущей } и текущей { есть текст,
    // проверяем его наличие во входной строке
    if (open > start) {
      std::string_view between = format.substr(start, open - start);
      auto pos = input.find(between);
      if (input.size() < between.size() || pos == std::string_view::npos) {
        return std::unexpected(scan_error{
            "Unformatted text in input and format string are different"});
      }
      if (start != 0) {
        input_parts.emplace_back(input.substr(0, pos));
      }

      input = input.substr(pos + between.size());
    }

    // Сохраняем спецификатор формата (то, что между {})
    format_parts.push_back(format.substr(open + 1, close - open - 1));
    start = close + 1;
  }

  // Проверяем оставшийся текст после последней }
  if (start < format.size()) {
    std::string_view remaining_format = format.substr(start);
    auto pos = input.find(remaining_format);
    if (input.size() < remaining_format.size() ||
        pos == std::string_view::npos) {
      return std::unexpected(scan_error{
          "Unformatted text in input and format string are different"});
    }
    input_parts.emplace_back(input.substr(0, pos));
    input = input.substr(pos + remaining_format.size());
  } else {
    input_parts.emplace_back(input);
  }
  return std::pair{format_parts, input_parts};
}

} // namespace stdx::details