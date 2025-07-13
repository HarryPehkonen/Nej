#ifndef NEJ_CORE_H
#define NEJ_CORE_H

#include <cstdint>
#include <filesystem>
#include <set>
#include <vector>
#include <string>

namespace fs = std::filesystem;

// Set of Unicode code points for emojis
extern const std::set<std::vector<uint32_t>> EMOJI_SEQUENCES;

// Function to check if a file is likely binary
auto isBinary(const fs::path& file_path) -> bool;

// Function to remove emojis from a UTF-8 string and return the count of removed emojis
auto removeEmojis(const std::string& text) -> std::pair<std::string, int>;

#endif  // NEJ_CORE_H
