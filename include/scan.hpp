#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <tuple>

namespace stdx {

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input,
                                                                     std::string_view format) {

    std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>,
                  details::scan_error>
        parse_results = details::parse_sources<Ts...>(input, format);

    if (parse_results.has_value() == false) {
        return std::unexpected{details::scan_error{parse_results.error().message}};
    }

    if (sizeof...(Ts) != parse_results.value().first.size() ||
        sizeof...(Ts) != parse_results.value().second.size()) {
        return std::unexpected{details::scan_error{"The number of parameters does not match"}};
    }

    int i = 0;
    int j = 0;

    auto parse_with_format_results_func =
        [&]<std::size_t... I>(
            std::index_sequence<I...>) -> std::tuple<std::expected<Ts, details::scan_error>...> {
        return std::make_tuple(details::parse_value_with_format<Ts>(
            parse_results.value().second.at(I), parse_results.value().first.at(I))...);
    };

    std::tuple<std::expected<Ts, details::scan_error>...> parse_with_format_results =
        parse_with_format_results_func(std::index_sequence_for<Ts...>{});

    bool has_error = false;
    details::scan_error error;

    auto extract_values =
        [&]<std::size_t... I>(
            std::index_sequence<I...>) -> std::expected<std::tuple<Ts...>, std::string> {
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

        return std::make_tuple(std::get<I>(parse_with_format_results).value()...);
    };

    auto values_opt = extract_values(std::index_sequence_for<Ts...>{});

    if (!values_opt.has_value()) {
        return std::unexpected(details::scan_error{error});
    }

    return details::scan_result<Ts...>{std::move(values_opt.value())};
}

}  // namespace stdx
