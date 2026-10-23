#include "Entity.hpp"

// Entity is defined entirely in the header: it is a plain struct whose members
// are public data and whose only function (bounds) is inline. This translation
// unit exists so the build picks up one .cpp per class and to verify that the
// header compiles on its own.
