#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct s_map_object
{
	int x;
	int y;
	int type;
	int active;

	struct s_map_object* next;
};

struct s_object_property
{
	int lenght_path;
	char* path;
	int frames;
	int solidity;
	int destructible;
	int collectible;
	int generator;
	int active;

	struct s_object_property* next;
};

struct s_map_info
{
	char* filename;
	int width;
	int height;
	int nb_element;
	int nb_object;

	struct s_map_object* first_case;
	struct s_object_property* first_property;
};

typedef struct s_map_object* map_object;
typedef struct s_object_property* object_property;
typedef struct s_map_info* map_info;

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

	printf("\n");
}

int read_digit(int file_id)
{
	char c = '0';
	read(file_id, &c, sizeof(char));
	return c - '0';
}

map_info load_map_file(char* filename)
{
	//Initialisation des trois structures
	map_info loading_map = (map_info)malloc(sizeof(struct s_map_info));
	//On recopie le nom du fichier dans la map entrain de charger
	loading_map->filename = (char*)malloc(sizeof(char) * strlen(filename));
	strcpy(loading_map->filename, filename);

	//On ouvre le fichier de sauvegarde en lecture
	FILE* file_save = fopen(filename, "r");

	int buff_size = 256;
	char buffer[buff_size];
	char* token;
	char delim[2] = " ";

	//On recupere la largeur et la hauteur
	fgets(buffer, buff_size, file_save);
	token = strtok(buffer, delim);
	loading_map->width = atoi(token);
	token = strtok(NULL, delim);
	loading_map->height = atoi(token);

	//On recupere le nombre d'élement, et on initialise la struture du premier element à NULL
	fgets(buffer, buff_size, file_save);
	token = strtok(buffer, delim);
	loading_map->nb_element = atoi(token);
	loading_map->first_case	= NULL;
	map_object prev_object = NULL;

	for(int i = 0; i < loading_map->nb_element; i++)
	{
		fgets(buffer, buff_size, file_save);

		map_object new_object = (map_object)malloc(sizeof(struct s_map_object));

		token = strtok(buffer, delim);
		new_object->x = atoi(token);
		token = strtok(NULL, delim);
		new_object->y = atoi(token);
		token = strtok(NULL, delim);
		new_object->type = atoi(token);
		new_object->active = 1;
		new_object->next = NULL;

		if (prev_object != NULL)
			prev_object->next = new_object;
		else
			loading_map->first_case = new_object;

		prev_object = new_object;
	}

	//On recupere le nombre d'objet
	fgets(buffer, buff_size, file_save);
	token = strtok(buffer, delim);
	loading_map->nb_object = atoi(token);
	loading_map->first_property = NULL;
	object_property prev_property = NULL;
	//On recupere les informations des objets
	for(int i = 0; i < loading_map->nb_object; i++)
	{
		fgets(buffer, buff_size, file_save);
		object_property new_property = (object_property)malloc(sizeof(struct s_object_property));

		token = strtok(buffer, delim);
		new_property->lenght_path = atoi(token);

		token = strtok(NULL, delim);
		new_property->path = (char*) malloc(new_property->lenght_path * sizeof(char));
		strcpy(new_property->path, token);

		token = strtok(NULL, delim);
		new_property->frames = atoi(token);

		token = strtok(NULL, delim);
		new_property->solidity = atoi(token);

		token = strtok(NULL, delim);
		new_property->destructible = atoi(token);

		token = strtok(NULL, delim);
		new_property->collectible = atoi(token);

		token = strtok(NULL, delim);
		new_property->generator = atoi(token);

		new_property->active = 1;

		new_property->next = NULL;

		if (prev_property != NULL)
			prev_property->next = new_property;
		else
			loading_map->first_property = new_property;

		prev_property = new_property;
	}

	return loading_map;
}

//Permet de determiner les nouvelles ID des elements sur la map (pour pruneobjects)
int* find_new_id(int* check_tab, int nb_object)
{
	int* tab2 = (int*)malloc(sizeof(int) * nb_object);

	int debug = 0;

	for(int i = 0; i < nb_object; i++)
		tab2[i] = 0;
	
	for(int i = 0; i < nb_object; i++)
	{
		int nb_zero = 0;
		for(int j = 0; j < i; j++)
		{
			if(check_tab[j] == 0)
				nb_zero += 1;
		}

		if(check_tab[i] != 0)
			tab2[i] = i - nb_zero;
	}

	return tab2;
}

//Enleve les objets non utilisés
void command_pruneobjects(map_info saved_map){
	int nb_element = saved_map->nb_element;
	int nb_object = saved_map->nb_object;
	int check_object[nb_object];

	for(int i = 0; i < nb_object; i++)
		check_object[i] = 0;

	map_object actual_case = saved_map->first_case;
	while(actual_case != NULL)
	{
		check_object[actual_case->type] = 1;
		actual_case = actual_case->next;
	}

	int* new_ids = find_new_id(check_object, nb_object);

	actual_case = saved_map->first_case;

	while(actual_case != NULL)
	{
		actual_case->type = new_ids[actual_case->type];
		actual_case = actual_case->next;
	}

	object_property actual_object = saved_map->first_property;

	int i = 0;
	int new_nb_object = 0;
	while(actual_object != NULL)
	{
		if(check_object[i] == 0)
			actual_object->active = 0;

		new_nb_object += check_object[i];
		i++;
		actual_object = actual_object->next;
	}

	saved_map->nb_object = new_nb_object;
	free(new_ids);
}

//Convertit la structure map en un fichier de sauvegarde 
void save_map_file(map_info new_map, char* filename)
{
	FILE* new_file = fopen("tmp", "w");
	char buffer[256];

	sprintf(buffer, "%d %d\n", new_map->width, new_map->height);
	fputs(buffer, new_file);

	int i = 0;
	map_object o = new_map->first_case;
	sprintf(buffer, "%d\n", new_map->nb_element);
	fputs(buffer, new_file);

	map_object current_object = new_map->first_case;
	while(current_object != NULL)
	{
		if (current_object->active)
		{
			sprintf(buffer, "%d %d %d\n", current_object->x, current_object->y, current_object->type);
			fputs(buffer, new_file);
		}

		current_object = current_object->next;
	}

	sprintf(buffer, "%d\n", new_map->nb_object);
	fputs(buffer, new_file);

	object_property current_property = new_map->first_property;
	while(current_property != NULL)
	{
		if(current_property->active)
		{
			sprintf(buffer, "%d %s %d %d %d %d %d\n", 
				current_property->lenght_path, current_property->path, current_property->frames, current_property->solidity,
				current_property->destructible, current_property->collectible, current_property->generator);
			fputs(buffer, new_file);
		}

		current_property = current_property->next;
	}

	fclose(new_file);
	remove(filename);
	rename("tmp", filename);
}


int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "%s\n", "Usage : maputil <file> <command> [value] ");
		return 1;
	}

	int f = open(argv[1], O_RDONLY);
	map_info actual_map = load_map_file(argv[1]);
	if (f == -1)
	{
		fprintf(stderr, "%s %s %s\n", "File", argv[1], "does not exist.");
		return 1;
	}

	int getwidth = 1;
	int getheight = 1;
	int getobjects = 1;
	int getinfo = 1;
	int setwidth = 1;
	int setheight = 1;
	int setobjects = 1;
	int pruneobjects = 1;

	for (int i = 2; i < argc; i++)
	{
		if (!strcmp(argv[i], "--getwidth"))
			getwidth = 0;

		if (!strcmp(argv[i], "--getheight"))
			getheight = 0;

		if (!strcmp(argv[i], "--getobjects"))
			getobjects = 0;

		if (!strcmp(argv[i], "--getinfo"))
			getinfo = 0;

		if (!(strcmp(argv[i], "--setwidth") * strcmp(argv[i], "--setheight")))
		{
			i++;
			if (i == argc)
			{
				fprintf(stderr, "Missing argument 'value' for %s.\n", argv[i - 1]);
				return 1;
			}

			if (!atoi(argv[i]))
			{
				fprintf(stderr, "Value for %s must be an integer greather or equal to 1.\n", argv[i - 1]);
				return 1;
			}

			if (!strcmp(argv[i - 1], "--setwidth"))
				setwidth = 0;
			else
				setheight = 0;
		}

		if (!strcmp(argv[i], "--setobjects"))
		{
			//...
		}

		if (!strcmp(argv[i], "--pruneobjects"))
			pruneobjects = 0;

	}

	if (getwidth * getheight * getobjects * getinfo * setwidth * setheight * setobjects * pruneobjects)
	{
		fprintf(stderr, "%s\n", "Aivalable commandes : --getwidth, --getheight, --getobjects, --getinfo, --setwidth, --setheight, --setobjects, --pruneobjects");
		return 1;
	}


	if (!getinfo)
	{
		getwidth = 0;
		getheight = 0;
		getobjects = 0;
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

		int value_width = actual_map->width;
		int value_height = actual_map->height;
		

		if (!setwidth)
		{
			int i = 2;
			while (strcmp(argv[i], "--setwidth") != 0)
				i++;
			value_width = atoi(argv[i + 1]);
		}

		if (!setheight)
		{
			int i = 2;
			while (strcmp(argv[i], "--setheight") != 0)
				i++;
			value_height = atoi(argv[i + 1]);
		}

		map_object current_object = actual_map->first_case;
		while (current_object != NULL)
		{
			if (current_object->x < value_width && current_object->y + (value_height - actual_map->height) > 0)
				current_object->y = current_object->y + (value_height - actual_map->height);
			else
				current_object->active = 0;
			current_object = current_object->next;
		}

		actual_map->width = value_width;
		actual_map->height = value_height;
	}

	if (!setobjects)
	{
		//...
	}

	if(!pruneobjects)
		command_pruneobjects(actual_map);

	close(f);

	if (!(setwidth * setheight * setobjects * pruneobjects))
		save_map_file(actual_map, argv[1]);

	return 0;
}