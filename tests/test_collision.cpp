#include "Collision.hpp"
static_assert(collision::overlaps(0,0,0,0,1));
static_assert(collision::overlaps(0,0,3,4,6));
static_assert(!collision::overlaps(0,0,3,4,5));
static_assert(!collision::overlaps(0,0,3,4,4));
static_assert(!collision::overlaps(0,0,0,0,0));
int main() { return 0; }
