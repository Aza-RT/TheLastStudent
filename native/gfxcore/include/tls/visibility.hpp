#pragma once
#include <glm/glm.hpp>
#include <vector>
namespace tls {
/// A solid wall edge in world coordinates.
struct Segment { glm::vec2 a, b; };
/// Robust segment geometry, independent of Godot and OpenGL context.
class Visibility {
public:
    /// Return nearest intersection distance (or max_distance for a miss).
    static float ray(glm::vec2 origin, glm::vec2 direction, const std::vector<Segment>& walls, float max_distance);
    /// Whether the open segment joining a and b is unobstructed.
    static bool line_of_sight(glm::vec2 a, glm::vec2 b, const std::vector<Segment>& walls);
    /// Fill a sorted radial polygon, with endpoint epsilon rays to resolve corners.
    static void polygon(glm::vec2 origin, const std::vector<Segment>& walls, float radius, std::vector<glm::vec2>& result);
};
}
