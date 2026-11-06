#define DOCTEST_CONFIG_USE_STD_HEADERS
#include <doctest/doctest.h>
#include <tls/visibility.hpp>

TEST_CASE("Ray in an empty scene reaches its distance limit") {
    const std::vector<tls::Segment> walls;
    const float distance = tls::Visibility::ray({0.f, 0.f}, {1.f, 0.f}, walls, 12.f);
    CHECK(distance == doctest::Approx(12.f));
}

// TODO(AZAT-01): Add the three edge cases from the task card.
// tests.cpp already provides the doctest main function.
