#include <Super Synthesis.h>

#undef main
int main()
{
	SSE::initSubSystems();

	SSE::mainLoop();

	SSE::cleanup();

	return 0;
}