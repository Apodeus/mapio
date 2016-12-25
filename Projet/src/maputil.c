#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if (argc % 6 != 3 && argc % 6 != 4)
	{
		fprintf(stderr, "%s\n", "Usage : maputil <file> <command> [value] ");
		return 1;
	}

	int getwidth = strcmp(argv[2], "--getwidth");
	int getheight = strcmp(argv[2], "--getheight");
	int getobjects = strcmp(argv[2], "--getobjects");
	int getinfo = strcmp(argv[2], "--getinfo");
	int setwidth = strcmp(argv[2], "--setwidth");
	int setheight = strcmp(argv[2], "--setheight");
	int setobjects = strcmp(argv[2], "--setobjects");

	if (getwidth * getheight * getobjects * getinfo * setwidth * setheight * setobjects)
	{
		fprintf(stderr, "%s\n", "Aivalable commandes : --getwidth, --getheight, --getobjects, --getinfo, --setwidth, --setheight, --setobjects");
		return 1;
	}

	int f = open(argv[1], O_RDONLY);
	if (f == -1)
	{
		fprintf(stderr, "%s %s %s\n", "File", argv[1], "does not exist.");
		return 1;
	}

	if (!getwidth || !getheight)
	{
		char c = 'a';
		int width = 0;
		int height = 0;

		read(f, &c, sizeof(char));

		while (c != ' ')
		{
			width = width * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		read(f, &c, sizeof(char));
		while (c != '\n')
		{
			read(f, &c, sizeof(char));
			height = height * 10 + c - '0';
		}

		if (!getwidth)
			printf("Width : %d\n", width);
		if (!getheight)
			printf("Height : %d\n", height);
			
	}

	close(f);
	return 0;
}