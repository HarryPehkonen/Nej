#include "../src/core.h"  // Include core functions
#include "gtest/gtest.h"

// Test fixture for removeEmojis function
class RemoveEmojisTest : public ::testing::Test {
   protected:
    // You can set up common test data here if needed
};

// Test fixture for replaceEmojis function
class ReplaceEmojisTest : public ::testing::Test {
   protected:
    // You can set up common test data here if needed
};

TEST_F(RemoveEmojisTest, HandlesEmptyString) { ASSERT_EQ(removeEmojis(" ").first, " "); }

TEST_F(RemoveEmojisTest, RemovesSingleEmoji) {
    ASSERT_EQ(removeEmojis("Hello 👋 World!").first, "Hello   World!");
}

TEST_F(RemoveEmojisTest, RemovesMultipleEmojis) { ASSERT_EQ(removeEmojis("✨🐛📝").first, "   "); }

TEST_F(RemoveEmojisTest, HandlesTextWithoutEmojis) {
    ASSERT_EQ(removeEmojis("This is a plain text string.").first, "This is a plain text string.");
}

TEST_F(RemoveEmojisTest, HandlesMixedContent) {
    ASSERT_EQ(removeEmojis("Text with ✨ and 🐛 emojis.").first, "Text with   and   emojis.");
}

TEST_F(RemoveEmojisTest, HandlesEmojisAtBeginningAndEnd) {
    ASSERT_EQ(removeEmojis("✨Text🐛").first, " Text ");
}

TEST_F(RemoveEmojisTest, HandlesEmojisAdjacentToEachOther) {
    ASSERT_EQ(removeEmojis("Hello👋🐛World!").first, "Hello  World!");
}

TEST_F(RemoveEmojisTest, HandlesEmojisWithDifferentByteLengths) {
    // Test with a 4-byte emoji (e.g., 🚀) and a 3-byte emoji (e.g., ✅)
    ASSERT_EQ(removeEmojis("🚀Test✅").first, " Test ");
}

TEST_F(RemoveEmojisTest, DoesNotRemoveNonEmojis) {
    // Test with various non-emoji characters (ASCII range 0-255)
    std::string non_emojis =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:'\",./"
        "<>?`~\n\t\r ";
    ASSERT_EQ(removeEmojis(non_emojis).first, non_emojis);
}

// Tests for replaceEmojis function
TEST_F(ReplaceEmojisTest, HandlesEmptyString) { 
    ASSERT_EQ(replaceEmojis(" ").first, " "); 
}

TEST_F(ReplaceEmojisTest, ReplacesMappedEmojis) {
    // Test checkmark replacement
    ASSERT_EQ(replaceEmojis("✅ Done").first, "✓ Done");
    // Test cross mark replacement  
    ASSERT_EQ(replaceEmojis("❌ Failed").first, "✗ Failed");
    // Test warning replacement
    ASSERT_EQ(replaceEmojis("⚠️ Warning").first, "⚠ Warning");
}

TEST_F(ReplaceEmojisTest, ReplacesRocketWithArrow) {
    ASSERT_EQ(replaceEmojis("Launch 🚀 now").first, "Launch → now");
}

TEST_F(ReplaceEmojisTest, ReplacesUnmappedEmojisWithSpace) {
    // Emojis not in the replacement map should become spaces (using a truly unmapped emoji)
    ASSERT_EQ(replaceEmojis("Hello 😊 World!").first, "Hello   World!");  // 😊 is smiley face, not mapped
}

TEST_F(ReplaceEmojisTest, ReplacesMixedMappedAndUnmapped) {
    ASSERT_EQ(replaceEmojis("✅ Success 😊 Done").first, "✓ Success   Done");  // 😊 is unmapped
}

TEST_F(ReplaceEmojisTest, HandlesTextWithoutEmojis) {
    ASSERT_EQ(replaceEmojis("This is plain text.").first, "This is plain text.");
}

TEST_F(ReplaceEmojisTest, HandlesMultipleMappedEmojis) {
    ASSERT_EQ(replaceEmojis("✅❌⚠️").first, "✓✗⚠");
}

TEST_F(ReplaceEmojisTest, CountsReplacedEmojis) {
    auto [result, count] = replaceEmojis("✅ Done ❌ Failed");
    ASSERT_EQ(count, 2);
    ASSERT_EQ(result, "✓ Done ✗ Failed");
}

auto main(int argc, char **argv) -> int {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}