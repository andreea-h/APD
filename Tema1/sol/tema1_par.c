/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define MIN(A, B) (((A) < (B)) ? (A) : (B))

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P; //numarul de thread-uri
int **result_julia;
int width_julia, height_julia;
pthread_barrier_t barrier;

int **result_man;
int width_man, height_man;

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia; //parametrul c al functiei
	int id; //id thread
} params;

params par_julia;
params par_man;

// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 6) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot <nr threads>\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	} //partile reale si imaginare ale parametrului c al functiei

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

void *f(void *arg) {
	long id = *(long *)arg; //id thread
	//indicii de start si end care indica coloanele matricelor care vor fi prelucrate
	int start_man = id * (double) width_man / P;
	int end_man = MIN ((id + 1) * (double) width_man / P, width_man);

	int start_julia = id * (double) width_julia / P;
	int end_julia = MIN ((id + 1) * (double) width_julia / P, width_julia);
	int w, h;

	//calculul multimii Julia
	for (w = start_julia; w < end_julia; w++) {
		for (h = 0; h < height_julia; h++) {
			int step = 0;
			//z - punctul din planul complex in are definim multimea
			complex z = { .a = w * par_julia.resolution + par_julia.x_min,
							.b = h * par_julia.resolution + par_julia.y_min };

			while (sqrt((z.a) * (z.a) + (z.b) * (z.b)) < 2.0 && step < par_julia.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = z_aux.a * z_aux.a - z_aux.b * z_aux.b + par_julia.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par_julia.c_julia.b;
				step++;
			}
			//memoreaza rezultatul direct in coordonate ecran
			result_julia[height_julia - h - 1][w] = step % 256;
		}
	}

	pthread_barrier_wait(&barrier);
	if (id == 0) { //un singur thread se ocupa de scrierea rezultatului
		//rezultatele sunt printate direct in coordonate ecran
		write_output_file(out_filename_julia, result_julia, width_julia, height_julia);
	}
	
	//calculul multimii Mandelbrot
	for (w = start_man; w < end_man; w++) {
		for (h = 0; h < height_man; h++) {
			complex c = { .a = w * par_man.resolution + par_man.x_min,
							.b = h * par_man.resolution + par_man.y_min };
			complex z = { .a = 0.0, .b = 0.0 };
			int step = 0;
			
			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par_man.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };
				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;
				step++;
			}
			//memoreaza rezultatul direct in coordonate ecran
			result_man[height_man - h - 1][w] = step % 256; 
		}
	}
	//inainte de scrierea rezultatului in fisier, toate threadurile trebuie sa finalizeze calculele
	pthread_barrier_wait(&barrier);
	if (id == 0) {
		write_output_file(out_filename_mandelbrot, result_man, width_man, height_man);
	}

	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	get_args(argc, argv);
	long arguments[P];
	pthread_t threads[P];
	
	read_input_file(in_filename_julia, &par_julia);
	read_input_file(in_filename_mandelbrot, &par_man);

	width_julia = (par_julia.x_max - par_julia.x_min) / par_julia.resolution;
	height_julia = (par_julia.y_max - par_julia.y_min) / par_julia.resolution;

	width_man = (par_man.x_max - par_man.x_min) / par_man.resolution;
	height_man = (par_man.y_max - par_man.y_min) / par_man.resolution;

	result_julia = allocate_memory(width_julia, height_julia);
	result_man = allocate_memory(width_man, height_man);

	pthread_barrier_init(&barrier, NULL, P);
	
	long i;
	for (i = 0; i < P; i++) {
		arguments[i] = i; //argumentul trimis este indicele asociat thread-ului
		int r = pthread_create(&threads[i], NULL, f, (void *)&arguments[i]);
		if (r != 0) {
			printf("Eroare la crearea unui thread!\n");
			exit(-1);
		}
	}

	for (i = 0; i < P; i++) {
		int r = pthread_join(threads[i], NULL);
		if (r != 0) {
			printf("Eroare la asteptarea thread-ului!\n");
			exit(-1);
		}
	}

	pthread_barrier_destroy(&barrier);
	pthread_exit(NULL);
	
	return 0;
}
