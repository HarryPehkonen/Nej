#include <CLI/CLI.hpp>  // Include CLI11 header
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "core.h"  // Include core functions

namespace fs = std::filesystem;

auto main(int argc, char** argv) noexcept -> int {
    CLI::App app{"Nej - No EmoJis: A command-line tool for removing emojis from text files"};

    std::vector<std::string> input_files_str;
    app.add_option("files", input_files_str, "Input text files to process")
        ->required()
        ->check(CLI::ExistingFile);

    std::string backup_extension;
    bool in_place = false;
    bool dry_run = false;
    auto *in_place_option =
        app.add_option(
               "-i,--in-place",
               backup_extension,
               "Perform in-place editing. Optionally specify backup extension (e.g., -i.bak)")
            ->option_text("EXT")
            ->expected(0, 1);

    app.add_flag("--dry-run", dry_run, "Report changes without modifying files.");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    if (in_place_option->count() > 0) {
        in_place = true;
        // backup_extension is already set by CLI11 if provided, empty if not
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

        infile.close(); // Close input file after isBinary check

        std::string line;
        int total_removed_emoji_count = 0;
        std::ofstream temp_outfile;
        fs::path temp_file_path; // Declare here for broader scope

        if (in_place) {
            // Generate unique temporary filename with PID and timestamp in same directory as original file
            auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
#ifdef _WIN32
            int pid = _getpid();
#else
            pid_t pid = getpid();
#endif
            std::string temp_filename = file_path.filename().string() + "." + std::to_string(pid) + "." + std::to_string(timestamp) + ".nej_tmp";
            temp_file_path = file_path.parent_path() / temp_filename;

            temp_outfile.open(temp_file_path);
            if (!temp_outfile.is_open()) {
                std::cerr << "Error: Could not open temporary file for writing: " << temp_file_path << "\n";
                continue;
            }
        }

        // Reopen infile for line-by-line reading
        infile.open(file_path);
        if (!infile.is_open()) {
            std::cerr << "Error: Could not re-open file for reading: " << file_path << "\n";
            if (in_place) {
                fs::remove(temp_file_path); // Clean up temp file if created
            }
            continue;
        }

        while (std::getline(infile, line)) {
            auto [processed_line, removed_emoji_count] = removeEmojis(line);
            total_removed_emoji_count += removed_emoji_count;

            if (!dry_run) {
                if (in_place) {
                    temp_outfile << processed_line << '\n';
                } else {
                    std::cout << processed_line << '\n';
                }
            }
        }
        infile.close(); // Close input file after processing

        if (in_place) {
            temp_outfile.close(); // Close temp output file

            if (!fs::exists(temp_file_path)) {
                std::cerr << "Error: Temporary file was not created or is empty: " << temp_file_path << "\n";
                continue;
            }

            std::error_code ec;
            fs::path backup_path;

            // Create backup only if backup extension is provided
            if (!backup_extension.empty()) {
                backup_path = file_path;
                backup_path += backup_extension;
                int counter = 1;
                while (fs::exists(backup_path)) {
                    backup_path = file_path.string() + backup_extension + std::to_string(counter++);
                }

                fs::rename(file_path, backup_path, ec);
                if (ec) {
                    std::cerr << "Error: Could not create backup file for " << file_path << ": "
                              << ec.message() << "\n";
                    fs::remove(temp_file_path);
                    continue;
                }
            } else {
                // No backup - just remove the original file
                fs::remove(file_path, ec);
                if (ec) {
                    std::cerr << "Error: Could not remove original file " << file_path << ": "
                              << ec.message() << "\n";
                    fs::remove(temp_file_path);
                    continue;
                }
            }

            fs::rename(temp_file_path, file_path, ec);
            if (ec) {
                std::cerr << "Error: Could not rename temporary file to original: " << file_path << ": "
                          << ec.message() << "\n";
                if (!backup_extension.empty()) {
                    fs::rename(backup_path, file_path, ec);
                    if (ec) {
                        std::cerr << "Error: Could not restore original file from backup " << backup_path << ": "
                                  << ec.message() << "\n";
                    }
                }
                fs::remove(temp_file_path);
                continue;
            }
        }

        if (dry_run) {
            if (total_removed_emoji_count > 0) {
                std::cout << "File: " << file_path << ", Emojis removed: " << total_removed_emoji_count << "\n";
            } else {
                std::cout << "File: " << file_path << ", No emojis found.\n";
            }
        }
    }

    return 0;
}
