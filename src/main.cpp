#include "scan.hpp"
#include <cassert>
#include <iostream>
#include <string_view>

int main() {
  // All params ok test
  const std::string input_model = "Hello 43 55 wewe 5.67 degrees";
  const std::string format_model = "Hello {%d} {%d} {%s} {%f} degrees";

  auto scan_res =
      stdx::scan<int, int, std::string, float>(input_model, format_model);
  assert(scan_res.has_value() && "All params ok 1");

  // Incorrect param test
  std::string format = "Hello {%d} {%d} {%s} {%gf} degrees";

  scan_res = stdx::scan<int, int, std::string, float>(input_model, format);
  assert(!scan_res.has_value() && "Incorrect param 1");

  // Incorrect input test
  std::string input = "Hello 43 55 wewe some degrees";

  scan_res = stdx::scan<int, int, std::string, float>(input, format_model);
  assert(!scan_res.has_value() && "Incorrect input 1");

  // Incorrect input test
  input = "Hello some 55 wewe 5.67 degrees";

  scan_res = stdx::scan<int, int, std::string, float>(input, format_model);
  assert(!scan_res.has_value() && "Incorrect input 2");

  // Correct input test
  input = "123";
  format = "{%d}";

  auto scan_res_2 = stdx::scan<int>(input, format);
  assert(scan_res_2.has_value() && "All params ok 2");

  // Incorrect input test
  input = "123 123.56 456";
  format = "{%d} {%d} {%d}";

  auto scan_res_3 = stdx::scan<int, int, int>(input, format);
  assert(!scan_res_3.has_value() && "Incorrect param 2");

  // Incorrect input test
  input = "123 123.56 456";
  format = "{%d} {%s} {%d}";

  auto scan_res_4 = stdx::scan<int, int, int>(input, format);
  assert(!scan_res_4.has_value() && "Incorrect param 3");

  // Incomplete input test
  input = "Hello world";
  format = "{%s} {%s} {%d}";

  auto scan_res_5 = stdx::scan<std::string, std::string, int>(input, format);
  assert(!scan_res_5.has_value() && "Incomplete input 3");

  // Incomplete input test
  input = "Hello world !!!";
  format = "{%s} {%s} {%d}";

  auto scan_res_6 = stdx::scan<std::string, std::string, int>(input, format);
  assert(!scan_res_6.has_value() && "Incorrect param 4");

  input = "-123 123";
  format = "{%d} {%u}";

  auto scan_res_7 = stdx::scan<int, uint8_t>(input, format);
  assert(scan_res_7.has_value() && "All params ok 3");

  return 0;
}