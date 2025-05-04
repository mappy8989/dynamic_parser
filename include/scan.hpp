#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <tuple>

namespace stdx {

// замените болванку функции scan на рабочую версию
template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error>
scan(std::string_view input, std::string_view format) {
  std::expected<
      std::pair<std::vector<std::string_view>, std::vector<std::string_view>>,
      stdx::details::scan_error>
      parse_results = stdx::details::parse_sources<Ts...>(input, format);

  if (parse_results.has_value() == false) {
    return std::unexpected{details::scan_error{parse_results.error().message}};
  }

  int i = 0;
  int j = 0;
  std::tuple<std::expected<Ts, stdx::details::scan_error>...>
      parse_with_format_results{stdx::details::parse_value_with_format<Ts>(
          parse_results.value().second.at(j++),
          parse_results.value().first.at(i++))...};

  bool has_error = false;
  details::scan_error error;

  auto extract_values = [&]<std::size_t... I>(std::index_sequence<I...>)
      -> std::expected<std::tuple<Ts...>, std::string> {
    // Check errors
    (([&] {
       if (!std::get<I>(parse_with_format_results)) {
         has_error = true;
         error = std::get<I>(parse_with_format_results).error();
       }
     }()),
     ...);

    if (has_error) {
      return std::unexpected{error.message};
    }

    return std::tuple<Ts...>{
        std::make_tuple(std::get<I>(parse_with_format_results).value()...)};
  };

  auto values_opt = extract_values(std::index_sequence_for<Ts...>{});

  if (!values_opt.has_value()) {
    return std::unexpected(error);
  }

  return details::scan_result<Ts...>{std::move(values_opt.value())};
}

} // namespace stdx
