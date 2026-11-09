#define DOCTEST_CONFIG_USE_STD_HEADERS
#include <doctest/doctest.h>
#include <tls/visibility.hpp>

TEST_CASE("Ray in an empty scene reaches its distance limit") {
    const std::vector<tls::Segment> walls;
    const float distance = tls::Visibility::ray({0.f, 0.f}, {1.f, 0.f}, walls, 12.f);
    CHECK(distance == doctest::Approx(12.f));
}

TEST_CASE("Ray ignores a wall behind its origin") {
    const std::vector<tls::Segment> walls = {{{-3.f, -2.f}, {-3.f, 2.f}}};
    const float distance = tls::Visibility::ray({0.f, 0.f}, {1.f, 0.f}, walls, 12.f);
    CHECK(distance == doctest::Approx(12.f));
}

TEST_CASE("Ray ignores a wall beyond its distance limit") {
    const std::vector<tls::Segment> walls = {{{15.f, -2.f}, {15.f, 2.f}}};
    const float distance = tls::Visibility::ray({0.f, 0.f}, {1.f, 0.f}, walls, 12.f);
    CHECK(distance == doctest::Approx(12.f));
}

TEST_CASE("Ray returns the nearer hit when the farther wall comes first") {
    const std::vector<tls::Segment> walls = {
        {{9.f, -2.f}, {9.f, 2.f}},
        {{4.f, -2.f}, {4.f, 2.f}},
    };
    const float distance = tls::Visibility::ray({0.f, 0.f}, {1.f, 0.f}, walls, 12.f);
    CHECK(distance == doctest::Approx(4.f));
}
