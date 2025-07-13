#include "gtest/gtest.h"
#include "../src/core.h" // Include core functions

// Test fixture for removeEmojis function
class RemoveEmojisTest : public ::testing::Test {
protected:
    // You can set up common test data here if needed
};

TEST_F(RemoveEmojisTest, HandlesEmptyString) {
    ASSERT_EQ(removeEmojis(" ").first, " ");
}

TEST_F(RemoveEmojisTest, RemovesSingleEmoji) {
    ASSERT_EQ(removeEmojis("Hello 👋 World!").first, "Hello   World!");
}

TEST_F(RemoveEmojisTest, RemovesMultipleEmojis) {
    ASSERT_EQ(removeEmojis("✨🐛📝").first, "   ");
}

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
    std::string non_emojis = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:'\",./<>?`~\n\t\r ";
    ASSERT_EQ(removeEmojis(non_emojis).first, non_emojis);
}

auto main(int argc, char **argv) -> int {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}