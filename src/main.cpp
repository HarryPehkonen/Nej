#include <CLI/CLI.hpp>  // Include CLI11 header
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core.h"  // Include core functions

namespace fs = std::filesystem;

auto main(int argc, char** argv) noexcept -> int {
    CLI::App app{"Nej - No EmoJis: A command-line tool for removing emojis from text files"};

    std::vector<std::string> input_files_str;
    app.add_option("files", input_files_str, "Input text files to process")
        ->required()
        ->check(CLI::ExistingFile);

    std::string backup_extension = ".bak";
    bool in_place = false;
    bool dry_run = false;
    auto *in_place_option =
        app.add_option(
               "-i,--in-place",
               "Perform in-place editing with backup. Specify backup extension (e.g., -i.bak)")
            ->option_text("EXT");

    app.add_flag("--dry-run", dry_run, "Report changes without modifying files.");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    if (in_place_option->count() > 0) {
        in_place = true;
        backup_extension = in_place_option->as<std::string>();
    }

    for (const auto& file_path_str : input_files_str) {
        fs::path file_path(file_path_str);

        if (!fs::exists(file_path)) {
            std::cerr << "Error: File not found: " << file_path << '\n';
            continue;
        }

        if (isBinary(file_path)) {
            std::cerr << "Warning: Skipping binary file: " << file_path << '\n';
            continue;
        }

        std::ifstream infile(file_path);
        if (!infile.is_open()) {
            std::cerr << "Error: Could not open file for reading: " << file_path << '\n';
            continue;
        }

        std::string content((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
        infile.close();

        auto [processed_content, removed_emoji_count] = removeEmojis(content);

        if (dry_run) {
            if (removed_emoji_count > 0) {
                std::cout << "File: " << file_path << ", Emojis removed: " << removed_emoji_count << "\n";
            } else {
                std::cout << "File: " << file_path << ", No emojis found.\n";
            }
        } else if (in_place) {
            fs::path backup_path = file_path;
            backup_path += backup_extension;

            // Generate a unique temporary file path
            fs::path temp_dir = fs::temp_directory_path();
            std::string temp_filename = file_path.filename().string() + ".nej_tmp";
            fs::path temp_file_path = temp_dir / temp_filename;

            // Ensure the temporary filename is unique
            int counter = 0;
            while (fs::exists(temp_file_path)) {
                counter++;
                temp_file_path = temp_dir / (temp_filename + std::to_string(counter));
            }

            // Write processed content to the temporary file
            std::ofstream temp_outfile(temp_file_path);
            if (!temp_outfile.is_open()) {
                std::cerr << "Error: Could not open temporary file for writing: " << temp_file_path << "\n";
                continue;
            }
            temp_outfile << processed_content;
            temp_outfile.close();

            // Check if the temporary file write was successful
            if (!fs::exists(temp_file_path) || fs::file_size(temp_file_path) != processed_content.length()) {
                std::cerr << "Error: Failed to write complete content to temporary file: " << temp_file_path << "\n";
                fs::remove(temp_file_path); // Clean up incomplete temp file
                continue;
            }

            std::error_code ec;

            // Rename original file to backup
            fs::rename(file_path, backup_path, ec);
            if (ec) {
                std::cerr << "Error: Could not create backup file for " << file_path << ": "
                          << ec.message() << "\n";
                fs::remove(temp_file_path); // Clean up temp file
                continue;
            }

            // Rename temporary file to original file path
            fs::rename(temp_file_path, file_path, ec);
            if (ec) {
                std::cerr << "Error: Could not rename temporary file to original: " << file_path << ": "
                          << ec.message() << "\n";
                // Attempt to restore original file from backup if renaming fails
                fs::rename(backup_path, file_path, ec);
                if (ec) {
                    std::cerr << "Error: Could not restore original file from backup " << backup_path << ": "
                              << ec.message() << "\n";
                }
                fs::remove(temp_file_path); // Clean up temp file
                continue;
            }
        } else {
            std::cout << processed_content;
        }
    }

    return 0;
}
