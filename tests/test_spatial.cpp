#include "SpatialGrid.hpp"
#include <random>
#include <iostream>
int main(){
 using namespace spatial;
 std::vector<Circle> c{{-64,0,10},{0,0,100},{0,0,100},{200,0,10}};
 std::vector<Point> p{{-65,0},{1,0},{-1,0},{210,0}};
 if(match(c,p).matches != std::vector<Match>{{0,0},{1,1},{2,2}}) return 1;
 if(!match({},p).matches.empty() || !match(c,{}).matches.empty()) return 2;
 std::mt19937 rng(42); std::uniform_real_distribution<float> d(-500,500),r(1,150);
 for(int k=0;k<100;++k){c.clear();p.clear();for(int i=0;i<150;++i){c.push_back({d(rng),d(rng),r(rng)});p.push_back({d(rng),d(rng)});} if(match(c,p).matches!=match(c,p,false).matches) return 3;}
 std::cout<<"Spatial grid tests passed\n";
}
