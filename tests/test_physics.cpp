#include "Physics.hpp"
#include <iostream>
#include <cstdlib>
void check(bool ok, const char* message) {
    if (!ok) { std::cerr << message << '\n'; std::exit(1); }
}
int main() {
    float x=550, y=400;
    physics::moveShip(x,y,{1,0},0.1f,1100,760);
    check(std::abs(x-586)<0.001f && y==400,"horizontal movement");
    x=550; y=400;
    physics::moveShip(x,y,{1,1},0.1f,1100,760);
    check(std::abs(std::hypot(x-550,y-400)-36)<0.001f,"diagonal normalization");
    x=55; y=135; physics::moveShip(x,y,{-1,-1},1,1100,760);
    check(x==55 && y==135,"ship bounds");
    float cooldown=0.1f; physics::tickCooldown(cooldown,0.2f);
    check(cooldown==0,"cooldown clamp");
    int lives=3, energy=1; float inv=0;
    check(physics::damage(lives,energy,inv) && lives==2 && energy==3,"life loss");
    check(!physics::damage(lives,energy,inv),"invulnerability");
    inv=0; lives=1; energy=1;
    check(physics::damage(lives,energy,inv) && lives==0 && energy==0,"game over");
    x=21; y=0; float rotation=0, drift=-25;
    physics::moveAsteroid(x,y,rotation,drift,100,10,20,1100,0.1f);
    check(x==20 && drift==25 && y==10 && rotation==1,"asteroid bounce");
    y=500; physics::moveLaser(y,0.1f); check(y==415,"laser movement");
    std::cout << "All physics checks passed\n";
}
