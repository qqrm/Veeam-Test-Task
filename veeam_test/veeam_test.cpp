// Veeam Test Task.cpp : Defines the entry point for the application.
//
#include <algorithm>
#include <execution>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

#include "EasyCRC/Types/crc32_c.h"
#include "veeam_test.h"

constexpr int MB = 1 << 20;

using part_range = std::tuple<size_t, size_t>;
using file_parts = std::vector<part_range>;

auto gen_sig(file_parts const &parts)
    -> std::optional<std::vector<std::string>> {
  if (parts.empty()) {
    return std::nullopt;
  }

  std::vector<std::string> res;
  res.resize(parts.size());

  std::transform(/*std::execution::par,*/ parts.cbegin(), parts.cend(),
                 res.begin(), [&](part_range const range) -> std::string {
                   EasyCRC::Calculator<EasyCRC::Type::CRC32_C> crc32;

                   auto const [begin, end] = range;

                   for (auto i = begin; i < end; i++) {
                     std::cout << i << '\n';

                     crc32 << i;
                   }

                   auto res = crc32.result();
                   std::cout << res << '\n';

                   return std::to_string(res);
                 });

  return std::optional(res);
}

auto split(std::filesystem::path const in, int const size = MB)
    -> std::optional<file_parts> {
  auto const file_size = std::filesystem::file_size(in);
  auto const chunks_count = file_size / size;

  auto res = file_parts();
  res.reserve(chunks_count);

  for (size_t i = 0; i < chunks_count; i++) {
    auto const start_pos = MB * i;
    auto const end_pos = std::min(start_pos + MB, file_size);

    std::cout << "start_pos: " << start_pos << "\n";
    std::cout << "end_pos: " << end_pos << "\n";

    res.emplace_back(std::tie(start_pos, end_pos));
  }

  return std::optional(res);
}

auto main(int argc, char *argv[]) -> int {
  std::cout << "Hello CMake." << std::endl;

  std::string in = argv[1];

  std::cout << "in: " << in << '\n';

  std::filesystem::path path(in);

  if (!std::filesystem::exists(path)) {
    return 1;
  }
  auto res = split(path);

  if (!res.has_value()) {
    return 1;
  }

  auto const parts = res.value();

  auto const gens_res = gen_sig(parts);

  for (auto s : std::views::all(gens_res.value())) {
    std::cout << s << '\n';
  }

  return 0;
}
