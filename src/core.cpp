#include "core.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <utility>  // For std::pair
#include <vector>

#include "emoji_data.h"

#include <unordered_map>
#include <functional>

// Custom hash function for std::vector<uint32_t>
struct VectorHash {
    auto operator()(const std::vector<uint32_t>& v) const -> std::size_t {
        std::size_t seed = v.size();
        for (auto& i : v) {
            seed ^= std::hash<uint32_t>{}(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

// Constants for file processing
const size_t FILE_BUFFER_SIZE = 4096;  // Check first 4KB

// Function to check if a file is likely binary
auto isBinary(const fs::path& file_path) -> bool {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return false;  // Cannot open, assume not binary for this check
    }

    std::vector<char> buffer(FILE_BUFFER_SIZE);  // Check first 4KB
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    std::streamsize bytes_read = file.gcount();

    for (std::streamsize i = 0; i < bytes_read; ++i) {
        if (buffer[i] == 0) {  // Null byte found
            return true;
        }
    }
    return false;
}

// Function to remove emojis from a UTF-8 string
auto removeEmojis(const std::string& text) -> std::pair<std::string, int> {
    std::string result;
    int removed_emoji_count = 0;

    auto it = text.begin();
    auto end = text.end();

    while (it != end) {
        auto start_of_sequence = it;
        std::vector<uint32_t> current_sequence;
        auto lookahead_it = it;
        auto matched_end_it = it;
        bool found_emoji = false;

        // Try to match the longest possible emoji sequence
        while (lookahead_it != end) {
            try {
                uint32_t code_point = utf8::next(lookahead_it, end);
                current_sequence.push_back(code_point);

                // Check if the current sequence is an emoji
                if (EMOJI_SEQUENCES.count(current_sequence) != 0) {
                    matched_end_it = lookahead_it;
                    found_emoji = true;
                }
            } catch (const utf8::exception&) {
                // Invalid UTF-8 sequence, stop lookahead
                break;
            }
        }

        if (found_emoji) {
            // An emoji sequence was found, replace it with a single space
            result += ' ';
            removed_emoji_count++;
            it = matched_end_it;
        } else {
            // No emoji sequence found, append the current character
            try {
                uint32_t code_point = utf8::next(it, end);
                utf8::append(code_point, std::back_inserter(result));
            } catch (const utf8::exception&) {
                // Invalid UTF-8 character, append replacement character
                result += '?';
                ++it;
            }
        }
    }
    return {result, removed_emoji_count};
}

// Mapping from common emojis to professional Unicode equivalents
const std::unordered_map<std::vector<uint32_t>, std::string, VectorHash> EMOJI_REPLACEMENTS = {
    // Checkmarks and crosses
    {{0x2705}, "✓"},  // ✅ → ✓ (Green checkmark → Check mark)
    {{0x274C}, "✗"},  // ❌ → ✗ (Cross mark → Ballot X)
    {{0x2714, 0xFE0F}, "✓"},  // ✔️ → ✓ (Check mark with variation selector)
    {{0x2714}, "✓"},  // ✔ → ✓ (Check mark)
    {{0x2716, 0xFE0F}, "✗"},  // ✖️ → ✗ (Multiply with variation selector)
    {{0x2716}, "✗"},  // ✖ → ✗ (Multiply)
    
    // Warning signs
    {{0x26A0, 0xFE0F}, "⚠"},  // ⚠️ → ⚠ (Warning with variation selector)
    {{0x26A0}, "⚠"},  // ⚠ → ⚠ (Already non-emoji, but included for completeness)
    
    // Common technical symbols
    {{0x1F680}, "→"},  // 🚀 → → (Rocket → Rightward arrow)
    {{0x1F4DD}, "※"},  // 📝 → ※ (Memo → Reference mark)
    {{0x1F4A1}, "◆"},  // 💡 → ◆ (Light bulb → Black diamond)
    {{0x1F50D}, "⚙"},  // 🔍 → ⚙ (Magnifying glass → Gear)
    {{0x1F3AF}, "★"},  // 🎯 → ★ (Bullseye → Black star)
    
    // Status and progress indicators
    {{0x2728}, "★"},  // ✨ → ★ (Sparkles → Black star)
    {{0x1F41B}, "▶"},  // 🐛 → ▶ (Bug → Black right-pointing triangle)
    {{0x1F4E6}, "▶"},  // 📦 → ▶ (Package → Black right-pointing triangle)
    {{0x1F44B}, "→"},  // 👋 → → (Waving hand → Rightward arrow)
};

// Function to replace emojis with Unicode equivalents
auto replaceEmojis(const std::string& text) -> std::pair<std::string, int> {
    std::string result;
    int replaced_emoji_count = 0;

    auto it = text.begin();
    auto end = text.end();

    while (it != end) {
        auto start_of_sequence = it;
        std::vector<uint32_t> current_sequence;
        auto lookahead_it = it;
        auto matched_end_it = it;
        bool found_emoji = false;
        std::string replacement;

        // Try to match the longest possible emoji sequence
        while (lookahead_it != end) {
            try {
                uint32_t code_point = utf8::next(lookahead_it, end);
                current_sequence.push_back(code_point);

                // Check if the current sequence is an emoji
                if (EMOJI_SEQUENCES.count(current_sequence) != 0) {
                    matched_end_it = lookahead_it;
                    found_emoji = true;
                    
                    // Check if we have a replacement for this emoji
                    auto replacement_it = EMOJI_REPLACEMENTS.find(current_sequence);
                    if (replacement_it != EMOJI_REPLACEMENTS.end()) {
                        replacement = replacement_it->second;
                    } else {
                        replacement = " ";  // Default replacement for unmapped emojis
                    }
                }
            } catch (const utf8::exception&) {
                // Invalid UTF-8 sequence, stop lookahead
                break;
            }
        }

        if (found_emoji) {
            // An emoji sequence was found, replace it with the appropriate Unicode
            result += replacement;
            replaced_emoji_count++;
            it = matched_end_it;
        } else {
            // No emoji sequence found, append the current character
            try {
                uint32_t code_point = utf8::next(it, end);
                utf8::append(code_point, std::back_inserter(result));
            } catch (const utf8::exception&) {
                // Invalid UTF-8 character, append replacement character
                result += '?';
                ++it;
            }
        }
    }
    return {result, replaced_emoji_count};
}