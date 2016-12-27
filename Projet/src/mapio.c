#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

struct s_map_case
{
	int x;
	int y;
	int type;

	struct s_map_case* next;
};

typedef struct s_map_case* map_case;


void map_new (unsigned width, unsigned height)
{
	map_allocate (width, height);

	for (int x = 0; x < width; x++)
		map_set (x, height - 1, 0); // Ground

	for (int y = 0; y < height - 1; y++) {
		map_set (0, y, 1); // Wall
		map_set (width - 1, y, 1); // Wall
	}

	map_object_begin (6);

	// Texture pour le sol
	map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
	// Mur
	map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
	// Gazon
	map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
	// Marbre
	map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
	//Fleurs
	map_object_add("images/flower.png", 1, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);
	//Pieces
	map_object_add("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);

	map_object_end ();

}

void map_save (char *filename)
{
	// TODO
	FILE* f = fopen(filename, "w");
	//Nombre d'objet
	int num_items = map_objects();
	//Largeur x Hauteur
	int width = map_width();
	int height = map_height();

	int nb_objects = 0;

	map_case prev = NULL;
	map_case first = NULL;

	fprintf(f, "%d %d\n", width, height);

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			int object = map_get(i, j);
			if (object != MAP_OBJECT_NONE)
			{
				nb_objects++;

				map_case m_case = (map_case)malloc(sizeof(struct s_map_case));
				m_case->x = i;
				m_case->y = j;
				m_case->type = object;
				m_case->next = NULL;

				if (first == NULL)
					first = m_case;
				else
					prev->next = m_case;
				prev = m_case;
			}
		}

	fprintf(f, "%d\n", nb_objects);

	map_case m_case = first;
	while (m_case != NULL)
	{
		first = m_case;
		fprintf(f, "%d %d %d\n", first->x, first->y, first->type);

		m_case = first->next;
		free(first);
	}

	fprintf(f, "%d\n", num_items);
	for (int i = 0; i < num_items; i++)
	{
		char* path = map_get_name(i);
		int len = strlen(path);
		int frames_num = map_get_frames(i);
		int solid = map_get_solidity(i);
		int dest = map_is_destructible(i);
		int coll = map_is_collectible(i);
		int gen = map_is_generator(i);
		fprintf(f, "%d %s %d %d %d %d %d\n", len, path, frames_num, solid, dest, coll, gen);
	}

	fclose(f);
	fprintf (stderr, "Map saved at %s\n", filename);
}

int isNumber(char c){
	return ((c - 48) >= 0 ) && ((c - 48) < 10);
}

int isSolid(int s){
	switch(s){
		case 0:
			return MAP_OBJECT_AIR;
		case 1:
			return MAP_OBJECT_SEMI_SOLID;
		case 2:
			return 	MAP_OBJECT_SOLID;
		default:
			return MAP_OBJECT_AIR;
	}

}

int isDestructible(int d){
	if(d == 0)
		return 0;
	return MAP_OBJECT_DESTRUCTIBLE;

}

int isCollectible(int c){
	if(c == 0)
		return 0;
	return MAP_OBJECT_COLLECTIBLE;
}

int isGenerator(int g){
	if(g == 0)
		return 0;
	return MAP_OBJECT_GENERATOR;
}

void map_load (char *filename)
{
	FILE* file_save = fopen(filename, "r");
	if(file_save == NULL){
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	int buff_size = 128;
	char delim[2] = " ";
	char buffer[buff_size];
	char* tokken;
	int x, y, id;

	int width, height;

	//fread(buffer, buff_size, 1, file_save) > 1
	if(fgets ( buffer, buff_size, file_save) == NULL)
		exit(EXIT_FAILURE);

	//on récupère la longueur
	tokken = strtok(buffer, delim);
	width = atoi(tokken);
	//on recupere la hauteur
	tokken = strtok(NULL, delim);
	height = atoi(tokken);

	map_allocate (width, height);

	if(fgets ( buffer, buff_size, file_save) == NULL)
		exit(EXIT_FAILURE);

	tokken = strtok(buffer, delim);

	int nb_objects = atoi(tokken);

	//On récupère les coordonnées des éléments et leur ID pour les remettre sur la map
	for(int n = 0; n < nb_objects ; n++){

		if(fgets(buffer, buff_size, file_save) == NULL)
			exit(EXIT_FAILURE);

		tokken = strtok(buffer, delim);
		// printf("x : %s ", tokken);
		x = atoi(tokken);

		tokken = strtok(NULL, delim);
		// printf("y : %s ", tokken);
		y = atoi(tokken);

		tokken = strtok(NULL, delim);
		// printf("id : %s\n", tokken);
		id = atoi(tokken);
		
		map_set(x, y, id);

	}

	//On récupère le nombre d'élément différent
	if(fgets(buffer, buff_size, file_save) == NULL )
		exit(EXIT_FAILURE);
	
	int nb_elem = atoi(buffer);

	map_object_begin(nb_elem);

	//Les propriétés des éléments sont stockées de la manière suivante sur une ligne
	// len_chemin chemin_image nb_frames solide destruct coll gener
	while(fgets ( buffer, buff_size, file_save ) != NULL ){
		tokken = strtok(buffer, delim);

		char path[ atoi(tokken) ];
		tokken = strtok(NULL, delim);	
		strcpy(path, tokken);

		tokken = strtok(NULL, delim);
		int nb_frames, solid, destruct, coll, gener;
		nb_frames = atoi(tokken);

		tokken = strtok(NULL, delim);
		solid = atoi(tokken);

		tokken = strtok(NULL, delim);
		destruct = atoi(tokken);

		tokken = strtok(NULL, delim);
		coll = atoi(tokken);

		tokken = strtok(NULL, delim);
		gener = atoi(tokken);

		map_object_add(path, nb_frames, isSolid(solid) | isDestructible(destruct) | isCollectible(coll) | isGenerator(gener));
	}

	map_object_end();

	fclose(file_save);

	//exit_with_error ("Map load is not yet implemented\n");
}

#endif
