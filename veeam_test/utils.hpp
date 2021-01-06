#pragma once

#include <boost/algorithm/hex.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/uuid/detail/md5.hpp>

using parts_hashes = std::vector<std::string>;

namespace utils {

namespace bip = boost::interprocess;
namespace fs = std::filesystem;

inline std::optional<bip::file_mapping> open_file(fs::path const &in) {
  if (!fs::exists(in)) {
    return {};
  }

  try {
    bip::file_mapping file(in.string().c_str(), bip::read_only);
    return std::optional(std::move(file));
  } catch (const bip::interprocess_exception &e) {
    std::cerr << e.what() << '\n';
    return {};
  }
}

using boost::uuids::detail::md5;

inline std::string hash_to_str(md5::digest_type const &digest) {

  std::string str_md5;
  auto const char_digest = reinterpret_cast<const char *>(&digest);
  boost::algorithm::hex(
      char_digest, char_digest + sizeof(boost::uuids::detail::md5::digest_type),
      std::back_inserter(str_md5));

  return str_md5;
}

inline bool write_to_file(fs::path const &path, parts_hashes const &hashes) {
  if (fs::exists(path) && !fs::remove(path)) {
    return false;
  }

  std::ofstream out(path);
  for (auto const &hash : hashes) {
    out << hash << "\n";
  }

  return true;
}

} // namespace utils