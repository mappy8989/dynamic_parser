#pragma once

#include <string>
#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    // здесь ваш код
    std::tuple<Ts...> result;
    const std::tuple<Ts...> &values(void) { return result; }
};

}  // namespace stdx::details
