#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>



typedef struct {
    int x;
    int y;
} Coordinate;


Coordinate norm_compare(Coordinate coord,Coordinate coord_new) {
    float curr_norm = coord.x * coord.x + coord.y * coord.y;
    float new_norm = coord_new.x * coord_new.x + coord_new.y * coord_new.y;

    return (new_norm>curr_norm) ? coord_new : coord ;
}


int main() {

int N = 64;
Coordinate coord[N];

srand(1);

printf("The random coordinates are - \n");
#pragma omp parallel for 
for (int i = 0; i <N; i++) {
    coord[i].x = 1 + (rand() % 100);
    coord[i].y = 1 + (rand() % 100);  
    printf(" (%d, %d)\n", coord[i].x, coord[i].y);
}

Coordinate temp = {0, 0};

#pragma omp declare reduction \
 (nc:Coordinate:omp_out=norm_compare(omp_out,omp_in)) \
 initializer(omp_priv={0,0})
 

#pragma omp parallel for reduction(nc:temp)
  for(int i=0; i<N; i++) {
   temp = norm_compare(temp,coord[i]);
  }

  printf("Coordinate with highest norm: (%d, %d)\n", temp.x, temp.y);


return 0;
}
