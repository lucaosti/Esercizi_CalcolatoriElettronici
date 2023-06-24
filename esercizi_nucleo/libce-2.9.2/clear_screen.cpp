#include "internal.h"
#include "vid.h"

using namespace vid;

void clear_screen(natb a) 
{
	attr = static_cast<natw>(a) << 8;
	for (unsigned int i = 0; i < VIDEO_SIZE; i++) 
		video[i] = ' ' | attr;
	cursore();
}
