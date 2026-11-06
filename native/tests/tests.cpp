#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_USE_STD_HEADERS
#include <doctest/doctest.h>
#include <tls/renderer.hpp>
#include <tls/shade_ai.hpp>
#include <iostream>
#define require(value, label) REQUIRE_MESSAGE((value), label)
TEST_CASE("Visibility, GLM transforms and native Shade regression cases"){
    using namespace tls;std::vector<Segment> wall={{{5,-10},{5,10}}};
    require(std::abs(Visibility::ray({0,0},{1,0},wall,100)-5)<.001,"ray nearest hit");
    require(!Visibility::line_of_sight({0,0},{10,0},wall),"wall occludes");
    require(Visibility::line_of_sight({0,0},{0,10},wall),"parallel clear");
    require(Visibility::line_of_sight({0,0},{0,0},wall),"zero length");
    std::vector<glm::vec2> polygon;Visibility::polygon({0,0},wall,20,polygon);require(polygon.size()>96,"corner epsilon samples");
    for(auto p:polygon) require(p.x<=5.01f || std::abs(p.y)>10,"visibility does not leak through wall");
    auto model=Camera2D::model({10,20},{2,3},1.5707963f);auto point=model*glm::vec4(1,0,0,1);require(glm::length(glm::vec2(point)-glm::vec2(10,22))<.001,"TRS matrix composition");
    auto vp=Camera2D{{10,20},2}.projection(800,600)*Camera2D{{10,20},2}.view();require(glm::length(glm::vec2(vp*glm::vec4(10,20,0,1)))<.001,"camera center maps to clip origin");
    ShadeAI ai;ai.position={0,0};ai.home={0,0};ai.waypoints={{0,0},{0,100}};
    ai.update(.016,{100,0},true,false,0,{});require(ai.state==ShadeAI::Chase,"light detection chase");
    for(int i=0;i<35;++i) ai.update(.1,{500,0},false,true,0,{});require(ai.state!=ShadeAI::Chase,"hidden player breaks chase");
    ai=ShadeAI();ai.update(.016,{200,0},false,false,1,{});require(ai.state==ShadeAI::Investigate,"noise investigation");
    ai=ShadeAI();ai.update(.016,{100,0},true,false,0,wall);require(ai.state==ShadeAI::Patrol,"no detection through wall");
    ai=ShadeAI();require(ai.update(.016,{0,0},false,false,0,{}),"fresh catch");require(!ai.update(.016,{0,0},false,false,0,{}),"catch cooldown");
    ai=ShadeAI();ai.position={48,48};ai.home={176,48};ai.state=ShadeAI::Return;ai.target=ai.home;
    std::vector<Segment> detour={{{96,0},{96,96}}};
    for(int i=0;i<160;++i)ai.update(.1,{1000,1000},false,true,0,detour);
    require(glm::length(ai.position-ai.home)<10,"native route returns around occluder endpoint");
    std::cout<<"PASS: geometry, corner shadows, TRS/MVP, Shade perception/transitions/cooldown\n";
}
