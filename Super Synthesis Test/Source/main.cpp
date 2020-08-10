#include <Super Synthesis.h>

int main()
{
	SSE::initSubSystems();

	SSE::mainLoop();

	SSE::cleanup();

	return 0;
}