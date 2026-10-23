#include "NPC.hpp"

// NPC is defined entirely in the header: it is an Entity plus a public sprite,
// with no out-of-line members. This translation unit exists so the build picks
// up one .cpp per class and to verify that the header compiles on its own.
