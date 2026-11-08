#include "unity.h"

#include "blink.h"

void setUp(void) {}
void tearDown(void) {}

static void test_startup_phase_is_fast_for_two_seconds(void)
{
  uint32_t total_ms = 0U;
  for (uint32_t t = 0U; t < BLINK_STARTUP_TOGGLES; t++)
  {
    TEST_ASSERT_EQUAL_UINT32(100U, blink_delay_ms(t));
    total_ms += blink_delay_ms(t);
  }
  TEST_ASSERT_EQUAL_UINT32(2000U, total_ms);
}

static void test_then_one_blink_every_500_ms(void)
{
  TEST_ASSERT_EQUAL_UINT32(250U, blink_delay_ms(BLINK_STARTUP_TOGGLES));
  TEST_ASSERT_EQUAL_UINT32(500U, blink_delay_ms(BLINK_STARTUP_TOGGLES) + blink_delay_ms(BLINK_STARTUP_TOGGLES + 1U));
  TEST_ASSERT_EQUAL_UINT32(250U, blink_delay_ms(UINT32_MAX));
}

static void test_startup_ends_with_the_led_off(void)
{
  /* The LED starts off and every toggle inverts it: an even number of start-up toggles leaves it off. */
  TEST_ASSERT_EQUAL_UINT32(0U, BLINK_STARTUP_TOGGLES % 2U);
}

static void test_counter_stops_after_startup_so_it_never_repeats(void)
{
  TEST_ASSERT_EQUAL_UINT32(1U, blink_next_toggle(0U));
  TEST_ASSERT_EQUAL_UINT32(BLINK_STARTUP_TOGGLES, blink_next_toggle(BLINK_STARTUP_TOGGLES - 1U));
  TEST_ASSERT_EQUAL_UINT32(BLINK_STARTUP_TOGGLES, blink_next_toggle(BLINK_STARTUP_TOGGLES));
  TEST_ASSERT_EQUAL_UINT32(BLINK_STARTUP_TOGGLES, blink_next_toggle(UINT32_MAX));
}

static void test_counter_walks_through_startup_into_run(void)
{
  uint32_t toggle = 0U;
  uint32_t fast = 0U;
  for (int i = 0; i < 100; i++)
  {
    if (blink_delay_ms(toggle) == BLINK_STARTUP_DELAY_MS)
    {
      fast++;
    }
    toggle = blink_next_toggle(toggle);
  }
  TEST_ASSERT_EQUAL_UINT32(BLINK_STARTUP_TOGGLES, fast);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_startup_phase_is_fast_for_two_seconds);
  RUN_TEST(test_then_one_blink_every_500_ms);
  RUN_TEST(test_startup_ends_with_the_led_off);
  RUN_TEST(test_counter_stops_after_startup_so_it_never_repeats);
  RUN_TEST(test_counter_walks_through_startup_into_run);
  return UNITY_END();
}
