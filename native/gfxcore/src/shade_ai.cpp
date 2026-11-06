#include <tls/shade_ai.hpp>
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>
namespace tls {
/// Grid BFS respects the same segment geometry used for shadows. Replan only when blocked.
static void find_route(glm::vec2 start,glm::vec2 goal,const std::vector<Segment>& walls,std::vector<glm::vec2>& route){
    route.clear();if(walls.empty())return;
    glm::vec2 lo=glm::min(start,goal),hi=glm::max(start,goal);for(auto wall:walls){lo=glm::min(lo,glm::min(wall.a,wall.b));hi=glm::max(hi,glm::max(wall.a,wall.b));}
    const int min_x=int(std::floor(lo.x/32))-1,min_y=int(std::floor(lo.y/32))-1;
    const int width=std::min(160,int(std::ceil(hi.x/32))-min_x+2),height=std::min(160,int(std::ceil(hi.y/32))-min_y+2);
    auto cell=[&](glm::vec2 p){int x=int(std::floor(p.x/32))-min_x,y=int(std::floor(p.y/32))-min_y;return x>=0&&x<width&&y>=0&&y<height?y*width+x:-1;};
    auto point=[&](int i){return glm::vec2((i%width+min_x)*32.f+16,(i/width+min_y)*32.f+16);};
    int source=cell(start),dest=cell(goal);if(source<0||dest<0)return;
    std::vector<int> parent(width*height,-1),queue;queue.reserve(width*height);queue.push_back(source);parent[source]=source;
    size_t cursor=0;
    while(cursor<queue.size()&&parent[dest]<0){int at=queue[cursor++],x=at%width,y=at/width;
        for(auto step:{glm::ivec2(1,0),glm::ivec2(-1,0),glm::ivec2(0,1),glm::ivec2(0,-1)}){
            int nx=x+step.x,ny=y+step.y;if(nx<0||nx>=width||ny<0||ny>=height)continue;int next=ny*width+nx;
            if(parent[next]>=0||!Visibility::line_of_sight(point(at),point(next),walls))continue;
            parent[next]=at;queue.push_back(next);
        }
    }
    if(parent[dest]<0)return;
    for(int at=dest;at!=source;at=parent[at])route.push_back(point(at));
    std::reverse(route.begin(),route.end());route.push_back(goal);
}
bool ShadeAI::update(float dt,glm::vec2 p,bool light,bool hidden,float noise,const std::vector<Segment>& walls) {
    dt=std::clamp(dt,0.f,0.1f); caught_cooldown=std::max(0.f,caught_cooldown-dt);
    route_timer-=dt;
    float distance=glm::length(p-position);
    bool visible=!hidden && distance<(light?270.f:145.f) && Visibility::line_of_sight(position,p,walls);
    bool heard=!hidden && noise>0 && distance<noise*240.f;
    if(visible && caught_cooldown<=0) { state=Chase; target=p; memory=2.3f; }
    else if(state==Chase) { memory-=dt; if(memory<=0) { state=Return; target=home; } }
    else if(heard && caught_cooldown<=0) { state=Investigate; target=p; memory=3.f; }
    if(state==Patrol && !waypoints.empty()) target=waypoints[waypoint%waypoints.size()];
    glm::vec2 movement_target=target;
    if(!Visibility::line_of_sight(position,target,walls)){
        if(route.empty()||route_timer<=0||glm::length(route_goal-target)>64){find_route(position,target,walls,route);route_goal=target;route_timer=1.2f;}
        while(!route.empty()&&glm::length(route.front()-position)<8)route.erase(route.begin());
        if(!route.empty())movement_target=route.front();
    }else route.clear();
    glm::vec2 delta=movement_target-position;
    float len=glm::length(delta);
    if(len<8) {
        if(state==Patrol) ++waypoint;
        else if(state==Return) state=Patrol;
        else if(state==Investigate) { memory-=dt; if(memory<=0) { state=Return; target=home; } }
    } else {
        float speed=state==Chase?155.f:state==Investigate?80.f:58.f;
        auto step=delta/len*std::min(speed*dt,len);
        // Sweep ahead and slide along walls, so an investigation cannot tunnel through a room.
        if(Visibility::line_of_sight(position,position+step+delta/len*8.f,walls)) position+=step;
        else {
            auto x=glm::vec2(step.x,0), y=glm::vec2(0,step.y);
            if(Visibility::line_of_sight(position,position+x*3.f,walls)) position+=x;
            if(Visibility::line_of_sight(position,position+y*3.f,walls)) position+=y;
            route_timer=0;
        }
    }
    if(!hidden && distance<19 && caught_cooldown<=0) { caught_cooldown=5; state=Return; target=home; return true; }
    return false;
}
}
