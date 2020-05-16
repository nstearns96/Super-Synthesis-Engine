#include <Include/Super Synthesis.h>

void main()
{
	SSE::initSubSystems();

	SSE::mainLoop();

	SSE::cleanup();
}