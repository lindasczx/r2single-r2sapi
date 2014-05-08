#include "soundwrapper.h"
#include <stdio.h>
#include <stdlib.h>

BOOL APIENTRY DllMain(HINSTANCE,DWORD,LPVOID);
int main(int argc, char *argv[])
{
	DllMain(0,DLL_PROCESS_ATTACH,0);

	AddHandle(1,111);
	AddHandle(2,222);
	AddHandle(3,333);
	AddHandle(4,444);
	AddHandle(5,555);
	AddHandle(6,666);
	AddHandle(7,777);

	int a=FindHandle(5);
	printf("%d\n",a);

	DelHandle(0);
	DelHandle(5);
	DelHandle(1);
	DelHandle(4);
	DelHandle(2);
	DelHandle(3);
	DelHandle(6);

	system("PAUSE");

	DllMain(0,DLL_PROCESS_DETACH,0);

	return 0;
}
