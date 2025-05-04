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
  assert(scan_res.has_value() && "All params ok");

  // Incorrect param test
  std::string format = "Hello {%d} {%d} {%s} {%gf} degrees";

  scan_res = stdx::scan<int, int, std::string, float>(input_model, format);
  assert(!scan_res.has_value() && "Incorrect param");

  // Incorrect input test
  std::string input = "Hello 43 55 wewe some degrees";

  scan_res = stdx::scan<int, int, std::string, float>(input, format_model);
  assert(!scan_res.has_value() && "Incorrect input");

  return 0;
}