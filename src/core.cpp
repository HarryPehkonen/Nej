#include "core.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <utility> // For std::pair

#include "emoji_data.h"

// Constants for file processing
const size_t FILE_BUFFER_SIZE = 4096; // Check first 4KB

// Helper function to decode a single UTF-8 character into a Unicode code point
// and return its length in bytes.
// Returns 0 if the character is malformed or incomplete.
auto decode_utf8_char(const std::string& text, size_t offset, uint32_t& out_code_point) -> size_t {
    if (offset >= text.length()) return 0;

    unsigned char byte1 = static_cast<unsigned char>(text[offset]);

    if ((byte1 & 0x80) == 0x00) { // 1-byte character (0xxxxxxx)
        out_code_point = byte1;
        return 1;
    } else if ((byte1 & 0xE0) == 0xC0) { // 2-byte character (110xxxxx 10xxxxxx)
        if (offset + 1 >= text.length()) return 0; // Incomplete
        unsigned char byte2 = static_cast<unsigned char>(text[offset + 1]);
        if ((byte2 & 0xC0) != 0x80) return 0; // Malformed
        out_code_point = ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
        return 2;
    } else if ((byte1 & 0xF0) == 0xE0) { // 3-byte character (1110xxxx 10xxxxxx 10xxxxxx)
        if (offset + 2 >= text.length()) return 0; // Incomplete
        unsigned char byte2 = static_cast<unsigned char>(text[offset + 1]);
        unsigned char byte3 = static_cast<unsigned char>(text[offset + 2]);
        if (((byte2 & 0xC0) != 0x80) || ((byte3 & 0xC0) != 0x80)) return 0; // Malformed
        out_code_point = ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
        return 3;
    } else if ((byte1 & 0xF8) == 0xF0) { // 4-byte character (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        if (offset + 3 >= text.length()) return 0; // Incomplete
        unsigned char byte2 = static_cast<unsigned char>(text[offset + 1]);
        unsigned char byte3 = static_cast<unsigned char>(text[offset + 2]);
        unsigned char byte4 = static_cast<unsigned char>(text[offset + 3]);
        if (((byte2 & 0xC0) != 0x80) || ((byte3 & 0xC0) != 0x80) || ((byte4 & 0xC0) != 0x80)) return 0; // Malformed
        out_code_point = ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
        return 4;
    }
    return 0; // Invalid UTF-8 start byte
}

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
    size_t i = 0;
    while (i < text.length()) {
        std::vector<uint32_t> current_sequence;
        size_t current_char_byte_len = 0;
        size_t matched_emoji_len = 0;

        // Try to match the longest possible emoji sequence
        for (size_t j = i; j < text.length();) {
            uint32_t code_point;
            size_t char_len = decode_utf8_char(text, j, code_point);
            if (char_len == 0) { // Malformed or incomplete character
                break;
            }
            current_sequence.push_back(code_point);

            // Check if the current sequence is an emoji
            if (EMOJI_SEQUENCES.count(current_sequence) != 0) {
                matched_emoji_len = (j - i) + char_len;
            }
            j += char_len;
        }

        if (matched_emoji_len > 0) {
            // An emoji sequence was found, replace it with a single space
            result += ' ';
            removed_emoji_count++;
            i += matched_emoji_len;
        } else {
            // No emoji sequence found, append the current character
            uint32_t code_point;
            size_t char_len = decode_utf8_char(text, i, code_point);
            if (char_len == 0) { // Should not happen if text is valid UTF-8
                // Append a replacement character or handle error
                result += '?'; 
                i++;
            } else {
                result.append(text.substr(i, char_len));
                i += char_len;
            }
        }
    }
    return std::make_pair(result, removed_emoji_count);
}