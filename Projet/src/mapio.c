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

  map_object_begin (4);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);

  map_object_end ();

}

void map_save (char *filename)
{
  // TODO
  FILE* f = fopen(filename, "w");
  int num_items = map_objects();
  fprintf(f, "%d\n", num_items);
  int w = map_width();
  int h = map_height();
  map_case prev = NULL;
  map_case first = prev;
  map_case very_first = first;
  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
    {
      int o = map_get(i, j);
      if (o != MAP_OBJECT_NONE)
      {
        map_case m = (map_case)malloc(sizeof(struct s_map_case));
        m->x = i;
        m->y = j;
        m->type = o;

        if (prev == NULL)
          first = m;
        else
          prev->next = m;
        prev = m;
      }
    }

  while (first->next != NULL)
    {
      fprintf(f, "%d %d %d\n", first->x, first->y, first->type);
      first = first->next;
    }


  fclose(f);
  fprintf (stderr, "Sorry: Map save is not yet implemented\n");
}

void map_load (char *filename)
{
  // TODO
  exit_with_error ("Map load is not yet implemented\n");
}

#endif
