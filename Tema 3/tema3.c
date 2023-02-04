#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define ROOT 3

static int num_neigh;
static int *neigh;
int *vect_parinti;

/**
 * @brief Functie care citeste datele de intrare din fisierele de intrare
 * 
 * @param rank 
 */
void read_neighbours(int rank) {
    FILE *fp;
    char file_name[50];
    sprintf(file_name, "cluster%d.txt", rank);

    fp = fopen(file_name, "r");
	fscanf(fp, "%d", &num_neigh);

	neigh = malloc(sizeof(int) * num_neigh);
 
	for (size_t i = 0; i < num_neigh; i++)
		fscanf(fp, "%d", &neigh[i]);

}

/**
 * @brief Functe care afisaza topologia finala pentru fiecare proces din topologie
 * 
 * @param vect_parinti 
 * @param rank 
 * @param numtasks 
 */
void print_final_topology (int *vect_parinti, int rank, int numtasks){
    int before  = 0;
    printf("%d -> ", rank);
        for (int i = 0; i < 4; i++){
            printf("%d:", i);
            before = 0;
            for(int j = 0; j < numtasks; j++){
                if(vect_parinti[j] == i){
                    if (before == 1){
                        printf(",");
                    }
                    printf("%d", j);
                    before = 1;
                }
            }
            printf(" ");
        }
        printf("\n");
}

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int before = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status status;

    vect_parinti = malloc(sizeof(int) * numtasks);
    for(int i = 0; i < numtasks; i++){
        vect_parinti[i] = -1;
    }

    /**
     * @brief Exercitiile 1 si 2 unde stabilim topologia si realizam calculele
     * Se parcurge topologiei in ordinea 0 -> 1 -> 2 -> 3 -> 0
     * 
     */
    if (atoi(argv[2]) == 0){
        /**
         * @brief Popularea vectorului de parinti si stabilirea topologiei.
         * Se apeleaza functia de citire din fisiere "read_neighbours"
         * Se populeaza vectorul de parinti si procesul 0 afla topologia finala
         * 
         */
        if (rank == 0) {
            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }

            MPI_Send(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            print_final_topology(vect_parinti, rank, numtasks);
        } else if (rank == 1) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }

            MPI_Send(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        } else if (rank == 2) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }

            MPI_Send(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        } else if(rank == 3){
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }
        
            MPI_Send(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
        }

        /**
         * @brief Trimiterea topologiel finale tuturor proceselor
         * 
         */
        if(rank == 0){
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
            MPI_Send(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
        } else if (rank == 1) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
            MPI_Send(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        } else if (rank == 2) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
            MPI_Send(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        } else if (rank == 3) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
        } else {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
        }

        /**
         * @brief Exercitiul 2 unde realizam calculele
         * 
         */
        int n = atoi(argv[1]);
        int *v;
        v = malloc(sizeof(int) * n);
        if(rank == 0) {
            for(int i = 0 ; i < n ; i++){
                v[i] = n - i - 1;
            }
            
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);

            MPI_Recv(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
            printf("Rezultat: ");
            for(int i = 0 ; i < n ; i++){
                printf("%d ",v[i]);
            }
            printf("\n");
        } else if (rank == 1){
            MPI_Recv(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        } else if (rank ==2){
            MPI_Recv(v, n, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        } else if (rank == 3){
            MPI_Recv(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
        } else {
            MPI_Recv(v, n, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int rank_worker = rank - 4;
            int start_index = rank_worker * (double) n / (numtasks - 4);
            int end_index = fmin(n, (rank_worker + 1) * (double) n / (numtasks - 4));
            for(int i = start_index ; i < end_index ; i++){
                v[i] *= 5;
            }
            MPI_Send(v, n, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, status.MPI_SOURCE);
        }
    }

    /**
     * @brief Exercitiul 3 unde rupem legatura intre 0 si 1
     * Se parcurge topologiei in ordinea 0 -> 3 -> 2 -> 1 -> 2 -> 3 -> 0
     * 
     */
    if( atoi(argv[2]) == 1){
         /**
         * @brief Popularea vectorului de parinti si stabilirea topologiei.
         * Se apeleaza functia de citire din fisiere "read_neighbours"
         * Se populeaza vectorul de parinti si procesul 0 afla topologia finala
         * 
         */
        if (rank == 0) {
            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }

            MPI_Send(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            print_final_topology(vect_parinti, rank, numtasks);
        } else if (rank == 3) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }

            MPI_Send(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);

            MPI_Recv(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
        } else if (rank == 2) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }

            MPI_Send(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);

            MPI_Recv(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        } else if(rank == 1){
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            read_neighbours(rank);
            for(int i = 0; i < num_neigh; i++){
                vect_parinti[neigh[i]] = rank;
            }
        
            MPI_Send(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        }

        /**
         * @brief Trimiterea topologiel finale tuturor proceselor
         * 
         */
        if(rank == 0){
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
            MPI_Send(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        } else if (rank == 3) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
            MPI_Send(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        } else if (rank == 2) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
            MPI_Send(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
        } else if (rank == 1) {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(vect_parinti, numtasks, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                }
            }
        } else {
            MPI_Recv(vect_parinti, numtasks, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_final_topology(vect_parinti, rank, numtasks);
        }

         /**
         * @brief Exercitiul 2 unde realizam calculele
         * 
         */
        int n = atoi(argv[1]);
        int *v;
        v = malloc(sizeof(int) * n);
        if(rank == 0) {
            for(int i = 0 ; i < n ; i++){
                v[i] = n - i - 1;
            }
            
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);

            MPI_Recv(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
            printf("Rezultat: ");
            for(int i = 0 ; i < n ; i++){
                printf("%d ",v[i]);
            }
            printf("\n");
        } else if (rank == 3){
            MPI_Recv(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);

            MPI_Recv(vect_parinti, numtasks, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            MPI_Send(vect_parinti, numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
        } else if (rank == 2){
            MPI_Recv(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);

            MPI_Recv(vect_parinti, numtasks, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(vect_parinti, numtasks, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        } else if (rank == 1){
            MPI_Recv(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for( int i = 0; i < numtasks; i++){
                if (vect_parinti[i] == rank ){
                    MPI_Send(v, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Recv(v, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
            }
            MPI_Send(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        } else {
            MPI_Recv(v, n, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int rank_worker = rank - 4;
            int start_index = rank_worker * (double) n / (numtasks - 4);
            int end_index = fmin(n, (rank_worker + 1) * (double) n / (numtasks - 4));
            for(int i = start_index ; i < end_index ; i++){
                v[i] *= 5;
            }
            MPI_Send(v, n, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, status.MPI_SOURCE);
        }
    }

    MPI_Finalize();

}