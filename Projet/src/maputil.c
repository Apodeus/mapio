#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct s_map_case
{
	int x;
	int y;
	int type;

	struct s_map_case* next;
};

typedef struct s_map_case* map_case;

void skip_line(int file_id)
{
	char c = 'a';
	while (c != '\n')
		read(file_id, &c, sizeof(char));
}

void skip_word(int file_id)
{
	char c = 'a';
	while (c != ' ')
		read(file_id, &c, sizeof(char));
}

void read_word(int file_id, char label[])
{
	printf("%s", label);
	char c = 62;
	while (c !=' ')
	{
		read(file_id, &c, sizeof(char));
		printf("%c", c);
	}
}

int read_digit(int file_id)
{
	char c = '0';
	read(file_id, &c, sizeof(char));
	return c - '0';
}

void pruneobjects(char* filename){
	FILE* file = fopen(filename, "r+");

	int buff_size = 128;
	char buffer[buff_size];

	char* tokken;
	char delim[2] = " ";

	char tmp_filename[4] = "tmp";
	//Le nombre de ligne à sauter pour acceder aux elements voulu..
	int header = 1;

	//On recupere le nombre d'element sur la map
	for(int i = 0; i < header + 1; i++)
		fgets(buffer, buff_size, file);

	int nb_elem = atoi(buffer);

	//On Saut les N element de la map pour acceder au nombre d'objet initialisé
	for(int i = 0; i <= nb_elem; i ++)
		fgets(buffer, buff_size, file);

	int nb_object = atoi(buffer);
	// printf("nb:%d|buf:%s\n", nb_object, buffer);

	//On crée un tableau de taile N objet initialisé rempli par défaut avec des 0
	int checkingObjects[nb_object];

	for(int i = 0; i < nb_object; i++)
		checkingObjects[i] = 0;

	//DEBUT REECRITURE
	FILE* new_file = fopen("tmp", "w");
	//On se replace au début du fichier puis à la position ou se situe les coordonnées
	fseek( file, 0, SEEK_SET );

	for(int i = 0; i < header + 1; i++)
	{
		fgets(buffer, buff_size, file);
		fputs(buffer, new_file);
	}

	//On recupere les ID des elements présents, et on met à 1 quand il est présent.
	for(int i = 0; i < nb_elem; i++){
		fgets(buffer, buff_size, file);
		fputs(buffer, new_file);
		tokken = strtok(buffer, delim);
		tokken = strtok(NULL, delim);
		tokken = strtok(NULL, delim);

		int id = atoi(tokken);

		checkingObjects[id] = 1;
	}

	fgets(buffer, buff_size, file);

	int new_nb_object = 0;

	for(int i = 0; i < nb_object; i++){
		// printf("tab[%d] = %d|nb_obj:%d\n", i, checkingObjects[i], nb_object);
		new_nb_object += checkingObjects[i];
	}

	sprintf(buffer, "%d\n", new_nb_object);

	fputs(buffer, new_file);

	for(int i = 0; i < nb_object; i++){
		fgets(buffer, buff_size, file);
		if(checkingObjects[i] == 1)
			fputs(buffer, new_file);
	}

	fclose(file);
	fclose(new_file);

	remove(filename);
	rename(tmp_filename, filename);

}

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
	int pruneobject = strcmp(argv[2], "--pruneobjects");

	if (getwidth * getheight * getobjects * getinfo * setwidth * setheight * setobjects * pruneobject)
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

	if (!getinfo)
	{
		getwidth = 0;
		getheight = 0;
		getobjects = 0;
	}

	if(!pruneobject)
	{
		pruneobjects(argv[1]);
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
			height = height * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		if (!getwidth)
			printf("Width : %d\n", width);
		
		if (!getheight)
			printf("Height : %d\n", height);	
	}

	if (!getobjects)
	{
		if (getinfo)
			skip_line(f);

		int num_object = 0;
		char c = '0';
		while (c != '\n')
		{
			num_object = num_object * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		for (int i = 0; i < num_object; i++)
			skip_line(f);

		num_object = 0;
		c = '0';
		while (c != '\n')
		{
			num_object = num_object * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		for(int i = 0; i < num_object; i++)
		{
			skip_word(f);
			read_word(f, "\nPath : ");
			read_word(f, ", frames : ");
			for (int i = 0; i < 4; i++)
			{
				int v = read_digit(f);
				read_digit(f);
				switch(i)
				{
					case 0:
					if (!v)
						printf(", not solid");
					else if (v == 1)
						printf(", semi-solid");
					else if (v == 2)
						printf(", solid");
					break;

					case 1:
					if (v)
						printf(", destructible");
					else
						printf(", not destructible");
					break;

					case 2:
					if (v)
						printf(", collectible");
					else
						printf(", not collectible");
					break;

					case 3:
					if (v)
						printf(", is generator.");
					else
						printf(", is not generator.");
					break;
				}
			}
		}

		printf("\n");
	}

	if (!setwidth || !setheight)
	{
		int value_width = atoi(argv[3]);
		int value_height = 12; //TODO
		if (argc < 4 || value_width < 1 || value_height < 1)
		{
			fprintf(stderr, "%s\n", "--setwidth and --setheight need a int value superior to zero.");
		}

		char c = 'a';
		int width = 0;
		int height = 0;
		int num_object = 0;
		read(f, &c, sizeof(char));

		while (c != ' ')
		{
			width = width * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		read(f, &c, sizeof(char));
		while (c != '\n')
		{
			height = height * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		read(f, &c, sizeof(char));
		while (c != '\n')
		{
			num_object = num_object * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		map_case prev = NULL;
		map_case first = NULL;
		int nb_objects = 0;
		for (int i = 0; i < num_object; i++)
		{
			int pos_x = 0;
			int pos_y = 0;
			int type = 0;
			read(f, &c, sizeof(char));
			while (c != ' ')
			{
				pos_x = pos_x * 10 + c - '0';
				read(f, &c, sizeof(char));
			}

			read(f, &c, sizeof(char));
			while (c != ' ')
			{
				pos_y = pos_y * 10 + c - '0';
				read(f, &c, sizeof(char));
			}

			read(f, &c, sizeof(char));
			while (c != '\n')
			{
				type = type * 10 + c - '0';
				read(f, &c, sizeof(char));
			}

			if (pos_x < value_width && pos_y < value_height)
			{
				nb_objects++;

				map_case m_case = (map_case)malloc(sizeof(struct s_map_case));
				m_case->x = pos_x;
				m_case->y = pos_y + (value_height - height);
				m_case->type = type;
				m_case->next = NULL;

				if (first == NULL)
					first = m_case;
				else
					prev->next = m_case;
				prev = m_case;
			}
		}

		//int new_file = open("tmp", O_WRONLY);
		FILE* new_file = fopen("tmp", "w");
		fprintf(new_file, "%d %d\n%d\n", value_width, value_height, nb_objects);
		map_case m_case = first;
		while (m_case != NULL)
		{
			first = m_case;
			fprintf(new_file, "%d %d %d\n", first->x, first->y, first->type);

			m_case = first->next;
			free(first);
		}

		int remaining = read(f, &c, sizeof(char));
		while (remaining > 0)
		{
			fprintf(new_file, "%c", c);
			remaining = read(f, &c, sizeof(char));
		}

		fclose(new_file);
		close(f);
		int nf = open("tmp", O_RDONLY);
		new_file = fopen(argv[1], "w");
		int l = read(nf, &c, sizeof(char));
		while (l > 0)
		{
			fprintf(new_file, "%c", c);
			l = read(nf, &c, sizeof(char));
		}
		fclose(new_file);
		close(nf);
		//remove("tmp");
	}

	close(f);
	return 0;
}