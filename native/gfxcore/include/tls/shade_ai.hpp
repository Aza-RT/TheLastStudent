#pragma once
#include <tls/visibility.hpp>
namespace tls {
/// Stateful enemy controller; all perception and movement decisions happen in C++.
class ShadeAI {
public:
    enum State { Patrol, Investigate, Chase, Return };
    glm::vec2 position{0}, target{0}, home{0};
    std::vector<glm::vec2> waypoints;
    State state=Patrol;
    float memory=0, caught_cooldown=0;
    float route_timer=0;
    glm::vec2 route_goal{0};
    std::vector<glm::vec2> route;
    size_t waypoint=0;
    /// Advance perception and movement, returning true only on a fresh capture.
    bool update(float dt, glm::vec2 player, bool flashlight, bool hidden, float noise, const std::vector<Segment>& walls);
};
}
