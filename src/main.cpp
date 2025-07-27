#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "core.h"

namespace fs = std::filesystem;

void show_help(const char* program_name) {
    std::cout << "Nej - No EmoJis: A command-line tool for removing emojis from text files\n\n";
    std::cout << "Usage: " << program_name << " [OPTIONS] FILES...\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  FILES                    Input text files to process\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --in-place          Perform in-place editing with no backup\n";
    std::cout << "  --backup-ext EXT        Backup extension for in-place editing (e.g., .bak)\n";
    std::cout << "  --dry-run               Report changes without modifying files\n";
    std::cout << "  -h, --help              Show this help message and exit\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " file.txt                    # Output to stdout\n";
    std::cout << "  " << program_name << " -i file.txt                 # In-place, no backup\n";
    std::cout << "  " << program_name << " -i --backup-ext .bak file.txt  # In-place with backup\n";
    std::cout << "  " << program_name << " --dry-run *.txt             # Preview changes\n";
}

struct Arguments {
    std::vector<std::string> files;
    bool in_place = false;
    std::string backup_extension;
    bool dry_run = false;
    bool help = false;
};

Arguments parse_arguments(int argc, char** argv) {
    Arguments args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            args.help = true;
        } else if (arg == "-i" || arg == "--in-place") {
            args.in_place = true;
        } else if (arg.length() > 2 && arg.substr(0, 2) == "-i") {
            // Handle -i.bak syntax for backward compatibility
            args.in_place = true;
            args.backup_extension = arg.substr(2); // Extract everything after "-i"
            if (!args.backup_extension.empty() && args.backup_extension[0] != '.') {
                std::cerr << "Error: Backup extension must start with '.' (e.g., .bak)\n";
                exit(1);
            }
        } else if (arg == "--backup-ext") {
            if (i + 1 < argc) {
                args.backup_extension = argv[++i];
                if (!args.backup_extension.empty() && args.backup_extension[0] != '.') {
                    std::cerr << "Error: Backup extension must start with '.' (e.g., .bak)\n";
                    exit(1);
                }
            } else {
                std::cerr << "Error: --backup-ext requires an argument\n";
                exit(1);
            }
        } else if (arg == "--dry-run") {
            args.dry_run = true;
        } else if (!arg.empty() && arg[0] == '-') {
            std::cerr << "Error: Unknown option '" << arg << "'\n";
            std::cerr << "Try '" << argv[0] << " --help' for more information.\n";
            exit(1);
        } else {
            args.files.push_back(arg);
        }
    }
    
    return args;
}

auto main(int argc, char** argv) noexcept -> int {
    Arguments args = parse_arguments(argc, argv);
    
    if (args.help) {
        show_help(argv[0]);
        return 0;
    }
    
    if (args.files.empty()) {
        std::cerr << "Error: No input files specified\n";
        std::cerr << "Try '" << argv[0] << " --help' for more information.\n";
        return 1;
    }

    for (const auto& file_path_str : args.files) {
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

        infile.close();  // Close input file after isBinary check

        std::string line;
        int total_removed_emoji_count = 0;
        std::ofstream temp_outfile;
        fs::path temp_file_path;  // Declare here for broader scope

        if (args.in_place) {
            // Generate unique temporary filename with PID, timestamp and counter in same directory as
            // original file
            static int file_counter = 0;
            auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
#ifdef _WIN32
            int pid = _getpid();
#else
            pid_t pid = getpid();
#endif
            std::string temp_filename = file_path.filename().string() + "." + std::to_string(pid) +
                                        "." + std::to_string(timestamp) + "." + std::to_string(++file_counter) + ".nej_tmp";
            temp_file_path = file_path.parent_path() / temp_filename;

            temp_outfile.open(temp_file_path);
            if (!temp_outfile.is_open()) {
                std::cerr << "Error: Could not open temporary file for writing: " << temp_file_path
                          << "\n";
                continue;
            }
        }

        // Reopen infile for line-by-line reading
        infile.open(file_path);
        if (!infile.is_open()) {
            std::cerr << "Error: Could not re-open file for reading: " << file_path << "\n";
            if (args.in_place) {
                fs::remove(temp_file_path);  // Clean up temp file if created
            }
            continue;
        }

        while (std::getline(infile, line)) {
            auto [processed_line, removed_emoji_count] = removeEmojis(line);
            total_removed_emoji_count += removed_emoji_count;

            if (!args.dry_run) {
                if (args.in_place) {
                    temp_outfile << processed_line << '\n';
                } else {
                    std::cout << processed_line << '\n';
                }
            }
        }
        infile.close();  // Close input file after processing

        if (args.in_place) {
            temp_outfile.close();  // Close temp output file

            if (!fs::exists(temp_file_path)) {
                std::cerr << "Error: Temporary file was not created or is empty: " << temp_file_path
                          << "\n";
                continue;
            }

            std::error_code ec;
            fs::path backup_path;

            // Create backup only if backup extension is provided
            if (!args.backup_extension.empty()) {
                auto find_unique_backup_path = [](const fs::path& base_path,
                                                  const std::string& extension) -> fs::path {
                    std::function<fs::path(const fs::path&, const std::string&, int)> find_path =
                        [&find_path](const fs::path& base, const std::string& ext,
                                     int counter) -> fs::path {
                        auto candidate = base;
                        candidate += ext + (counter == 0 ? "" : std::to_string(counter));

                        if (!fs::exists(candidate)) {
                            return candidate;  // Base case
                        }

                        // Tail call - nothing happens after this return
                        return find_path(base, ext, counter + 1);
                    };

                    return find_path(base_path, extension, 0);
                };

                backup_path = find_unique_backup_path(file_path, args.backup_extension);

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
                std::cerr << "Error: Could not rename temporary file to original: " << file_path
                          << ": " << ec.message() << "\n";
                if (!args.backup_extension.empty()) {
                    fs::rename(backup_path, file_path, ec);
                    if (ec) {
                        std::cerr << "Error: Could not restore original file from backup "
                                  << backup_path << ": " << ec.message() << "\n";
                    }
                }
                fs::remove(temp_file_path);
                continue;
            }
        }

        if (args.dry_run) {
            if (total_removed_emoji_count > 0) {
                std::cout << "File: " << file_path
                          << ", Emojis removed: " << total_removed_emoji_count << "\n";
            } else {
                std::cout << "File: " << file_path << ", No emojis found.\n";
            }
        }
    }

    return 0;
}