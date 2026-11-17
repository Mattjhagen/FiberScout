#include <unity.h>
#include <cstring>
#include "portal_assets.h"

void test_bundle_has_required_resources() {
  const char *required[] = {"/", "/style.css", "/avatar.JPG", "/favicon.png", "/manifest.json", "/Matthew_Hagen_TMobile_Fiber.vcf", "/icons.css", "/fa-solid-900.woff2"};
  TEST_ASSERT_EQUAL_UINT(8, sizeof(portalAssets) / sizeof(portalAssets[0]));
  for (const char *path : required) {
    bool found = false;
    for (const auto &asset : portalAssets) {
      if (strcmp(path, asset.path) == 0) {
        found = true;
        TEST_ASSERT_GREATER_THAN(20, asset.size);
        TEST_ASSERT_EQUAL_HEX8(0x1f, asset.data[0]);
        TEST_ASSERT_EQUAL_HEX8(0x8b, asset.data[1]);
        TEST_ASSERT_EQUAL_HEX8(8, asset.data[2]);
      }
    }
    TEST_ASSERT_TRUE_MESSAGE(found, path);
  }
}
int main() { UNITY_BEGIN(); RUN_TEST(test_bundle_has_required_resources); return UNITY_END(); }
