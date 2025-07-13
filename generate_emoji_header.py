import re

def generate_emoji_header(input_file, output_file):
    """
    Parses the emoji-test.txt file and generates a C++ header file
    with a set of emoji code points.
    """
    emoji_sequences = set()

    with open(input_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or line.startswith("\n") or line.startswith("\t") or "Emoji_Component" in line:
                continue

            # Example line format:
            # 1F600                                      ; fully-qualified # 😀 E1.0 grinning face
            # 1F468 200D 200D 1F467                      ; fully-qualified # 👨‍👩‍👧 E4.0 family: man, woman, girl

            # Extract the code points and status
            match = re.match(r"([0-9A-F\s]+);\s*(fully-qualified|minimally-qualified)", line)
            if match:
                code_points_str = match.group(1).strip()
                status = match.group(2)

                if status in ["fully-qualified", "minimally-qualified"]:
                    # Split by space and convert to hex integers
                    code_points = tuple(int(cp, 16) for cp in code_points_str.split())
                    emoji_sequences.add(code_points)

    with open(output_file, "w") as f:
        f.write("#ifndef NEJ_EMOJI_DATA_H\n")
        f.write("#define NEJ_EMOJI_DATA_H\n\n")
        f.write("#include <cstdint>\n")
        f.write("#include <set>\n")
        f.write("#include <vector>\n\n")
        f.write("const std::set<std::vector<uint32_t>> EMOJI_SEQUENCES = {\n")
        for i, emoji_sequence in enumerate(sorted(list(emoji_sequences))):
            f.write("    {")
            f.write(", ".join(f"0x{cp:04X}" for cp in emoji_sequence))
            f.write("}")
            if i < len(emoji_sequences) - 1:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")
        f.write("#endif  // NEJ_EMOJI_DATA_H\n")

if __name__ == "__main__":
    generate_emoji_header("emoji-test.txt", "src/emoji_data.h")