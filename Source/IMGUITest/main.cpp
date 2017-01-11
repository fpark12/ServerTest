
#include <thread>

BEFORE_MAIN()
{
	GCore.Init();
}
int main(int, char**)
{

	

	while (1)
	{
		Sleep(100);
	}
	
	GGUI.Main();
    
	return 0;
}
