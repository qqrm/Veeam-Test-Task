#include <algorithm>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "utils.hpp"

uintmax_t const ONE_PART = 1;

constexpr int MB = 1 << 20;

namespace bip = boost::interprocess;
namespace fs = std::filesystem;

using part_range = std::tuple<size_t, size_t>;
using parts_ranges = std::vector<part_range>;
using file_regions = std::vector<bip::mapped_region>;

auto calc_part_ranges(fs::path const in, uintmax_t const size = MB)
    -> std::optional<parts_ranges> {
  auto const file_size = std::filesystem::file_size(in);

  if (0 == file_size) {
    return std::nullopt;
  }

  auto const chunks_count = std::max(file_size / size, ONE_PART);
  parts_ranges ranges;
  ranges.reserve(chunks_count);

  for (size_t i = 0; i < chunks_count; i++) {
    auto const start_pos = MB * i;
    auto const end_pos = std::min(start_pos + MB, file_size);
    ranges.emplace_back(std::tie(start_pos, end_pos));
  }

  return std::optional(ranges);
}

auto split_file_to_regions(bip::file_mapping const &file,
                           parts_ranges const &ranges)
    -> std::optional<parts_hashes> {

  if (ranges.empty()) {
    return std::nullopt;
  }

  std::mutex parts_hashes_m;
  parts_hashes hashes;
  hashes.reserve(ranges.size());

  std::for_each(std::execution::par, ranges.cbegin(), ranges.cend(),
                [&](part_range const &range) {
                  auto const [begin, end] = range;
                  auto const region_size = end - begin;

                  bip::mapped_region mapped_rgn(file, bip::read_only, begin,
                                                region_size);

                  char const *const mmaped_data =
                      static_cast<char *>(mapped_rgn.get_address());

                  auto const mmap_size = mapped_rgn.get_size();

                  using boost::uuids::detail::md5;

                  md5 hash;
                  md5::digest_type digest;

                  hash.process_bytes(mmaped_data, mmap_size);
                  hash.get_digest(digest);

                  const std::lock_guard<std::mutex> lock(parts_hashes_m);
                  hashes.emplace_back(utils::hash_to_str(digest));
                });

  return std::optional(hashes);
}

auto main(int argc, char *argv[]) -> int {
  if (argc != 3) {
    return 1;
  }

  fs::path in_file(argv[1]);
  fs::path out_file(argv[2]);

  auto const file = utils::open_file(in_file);
  if (!file.has_value()) {
    return 1;
  }

  auto const ranges = calc_part_ranges(in_file);
  if (!ranges.has_value()) {
    return 1;
  }

  auto const hashes = split_file_to_regions(file.value(), ranges.value());
  if (!hashes.has_value()) {
    return 1;
  }

  utils::write_to_file(out_file, hashes.value());

  return 0;
}
