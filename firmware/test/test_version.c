#include <stdio.h>

#include "unity.h"

#include "version.h"

void setUp(void) {}
void tearDown(void) {}

/* The strings must spell out exactly the numbers ci/version.sh writes, in the form x.y.z.b. */
static void test_fw_version_string_matches_the_numbers(void)
{
  char expected[48];
  snprintf(expected, sizeof expected, "%d.%d.%d.%d",
           FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_REVISION, FW_VERSION_BUILD);
  TEST_ASSERT_EQUAL_STRING(expected, FW_VERSION);
}

static void test_ui_version_string_matches_the_numbers(void)
{
  char expected[48];
  snprintf(expected, sizeof expected, "%d.%d.%d.%d",
           UI_VERSION_MAJOR, UI_VERSION_MINOR, UI_VERSION_REVISION, UI_VERSION_BUILD);
  TEST_ASSERT_EQUAL_STRING(expected, UI_VERSION);
}

static void test_build_numbers_are_set(void)
{
  TEST_ASSERT_TRUE(FW_VERSION_BUILD >= 1);
  TEST_ASSERT_TRUE(UI_VERSION_BUILD >= 1);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_fw_version_string_matches_the_numbers);
  RUN_TEST(test_ui_version_string_matches_the_numbers);
  RUN_TEST(test_build_numbers_are_set);
  return UNITY_END();
}
