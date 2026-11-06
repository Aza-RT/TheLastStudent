#include <tls/visibility.hpp>
#include <algorithm>
#include <cmath>
namespace tls {
static float cross(glm::vec2 a, glm::vec2 b) { return a.x*b.y-a.y*b.x; }
float Visibility::ray(glm::vec2 o, glm::vec2 d, const std::vector<Segment>& walls, float limit) {
    for (const auto& w : walls) {
        const auto e=w.b-w.a; const float denom=cross(d,e);
        if (std::abs(denom)<1e-7f) continue;
        const auto q=w.a-o;
        const float t=cross(q,e)/denom, u=cross(q,d)/denom;
        if (t>=0 && u>=0 && u<=1 && t<limit) limit=t;
    }
    return limit;
}
bool Visibility::line_of_sight(glm::vec2 a, glm::vec2 b, const std::vector<Segment>& walls) {
    float len=glm::length(b-a);
    return len<0.001f || ray(a,(b-a)/len,walls,len)>=len-0.05f;
}
void Visibility::polygon(glm::vec2 o, const std::vector<Segment>& walls, float r, std::vector<glm::vec2>& out) {
    // Uniform rays bound curvature; +/- epsilon rays capture either side of each wall corner.
    out.clear(); out.reserve(96+walls.size()*6);
    for(int i=0;i<96;++i) { float a=i*6.283185307f/96; out.emplace_back(std::cos(a),std::sin(a)); }
    for(const auto& w:walls) for(auto p:{w.a,w.b}) {
        if(glm::length(p-o)>r*1.5f) continue;
        float a=std::atan2(p.y-o.y,p.x-o.x);
        for(float eps:{-0.0001f,0.f,0.0001f}) out.emplace_back(std::cos(a+eps),std::sin(a+eps));
    }
    std::sort(out.begin(),out.end(),[](auto a,auto b){return std::atan2(a.y,a.x)<std::atan2(b.y,b.x);});
    for(auto& d:out) d=o+d*ray(o,d,walls,r);
}
}
