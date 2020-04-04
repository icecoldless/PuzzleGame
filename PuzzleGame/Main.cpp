#include "Game.h"
#include<ctime>
int main(int argc, char** argv)
{
	srand(time(0));
	Game g;
	bool success =  g.Initialize();
	if (success)
	{
		g.RunLoop();
	}
	g.ShutDown();
	return 1;
}