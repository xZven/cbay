#include "../headers/*.h"

#include "../define.h"

void welcome_message(int argc, char * argv[])
{
	int index = 0;

	printf("\nBienvenue sur Cbay, Application serveur!\n");
	if(argc > 1)
		{
			fprintf(stdout, "[WAR]: Arguments in the launching command [IGNORING SOME]\n");
			while(index != argc)
			{
				if(strcmp(argv[index], "-debug") == 0)
					debug = DEBUG_ON; // MODE DEBUG ON
				else
					debug = DEBUG_OFF; // MODE DEBUG OFF
				fprintf(stdout, "ARGUMENT %d: %s \n", index, argv[index]);
				index++;
			}
			fprintf(stdout, "[WAR]: MODE DEBUG ON\n");
			index = 0;
		}
}

void clean_b(char * buffer)
{
	buffer[0]='\0';
}

