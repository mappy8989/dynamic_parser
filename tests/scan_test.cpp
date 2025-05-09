#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, All_params_ok_1) {
    const std::string input_model = "Hello 43 55 wewe 5.67 degrees";
    const std::string format_model = "Hello {%d} {%d} {%s} {%f} degrees";

    auto scan_res = stdx::scan<int, int, std::string_view, float>(input_model, format_model);
    ASSERT_TRUE(scan_res.has_value() && (std::get<0>(scan_res.value().result) == 43) &&
                (std::get<1>(scan_res.value().result) == 55 &&
                 (std::get<2>(scan_res.value().result) == "wewe") &&
                 (std::get<3>(scan_res.value().result) == 5.67f) &&
                 (std::tuple_size_v<decltype(scan_res.value().result)> == 4)));
}

TEST(ScanTest, Incorrect_param_1) {
    const std::string input_model = "Hello 43 55 wewe 5.67 degrees";
    const std::string format = "Hello {%d} {%d} {%s} {%gf} degrees";

    auto scan_res = stdx::scan<int, int, std::string_view, float>(input_model, format);
    ASSERT_TRUE(scan_res.error().message == "Unexpected type format %gf");
}

TEST(ScanTest, Incorrect_input_1) {
    const std::string input = "Hello 43 55 wewe some degrees";
    const std::string format_model = "Hello {%d} {%d} {%s} {%f} degrees";

    auto scan_res = stdx::scan<int, int, std::string_view, float>(input, format_model);
    ASSERT_TRUE(scan_res.error().message == "Invalid argument");
}

TEST(ScanTest, Incorrect_input_2) {
    auto input = "Hello some 55 wewe 5.67 degrees";
    const std::string format_model = "Hello {%d} {%d} {%s} {%f} degrees";

    auto scan_res = stdx::scan<int, int, std::string_view, float>(input, format_model);
    ASSERT_TRUE(scan_res.error().message == "Invalid argument");
}

TEST(ScanTest, All_params_ok_2) {
    auto input = "123";
    auto format = "{%d}";

    auto scan_res = stdx::scan<int>(input, format);
    ASSERT_TRUE(scan_res.has_value());
}

TEST(ScanTest, Incorrect_param_2) {
    auto input = "123 123.56 456";
    auto format = "{%d} {%d} {%d}";

    auto scan_res = stdx::scan<int, int, int>(input, format);
    ASSERT_TRUE(scan_res.has_value());
}

TEST(ScanTest, Incorrect_input_3) {
    auto input = "123 123.56 456";
    auto format = "{%d} {%s} {%d}";

    auto scan_res = stdx::scan<int, int, int>(input, format);
    ASSERT_FALSE(scan_res.has_value());
}

TEST(ScanTest, Too_few_template_parameters) {
    auto input = "Hello world";
    auto format = "{%s} {%s}";

    ASSERT_FALSE((stdx::scan<std::string_view, std::string_view, int>(input, format)));
}

TEST(ScanTest, Too_much_template_parameters) {
    auto input = "Hello world !!!";
    auto format = "{%s} {%s}";

    ASSERT_FALSE((stdx::scan<std::string_view, std::string_view, int>(input, format)));
}

TEST(ScanTest, All_params_ok_3) {
    auto input = "-123 123";
    auto format = "{%d} {%u}";

    auto scan_res = stdx::scan<int, uint8_t>(input, format);
    ASSERT_TRUE(scan_res.has_value());
}
