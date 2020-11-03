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
pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_barrier_t barrier3;
pthread_barrier_t barrier4;

int **result_man;
int width_man, height_man;
pthread_barrier_t barrier;

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
			fprintf(file, "%d ", result[height - i - 1][j]);
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

	long id = *(long *)arg;

	int i;
	int start_man = id * (double) width_man / P;
	int end_man = MIN ((id + 1) * (double) width_man / P, width_man);

	int start_julia = id * (double) width_julia / P;
	int end_julia = MIN ((id + 1) * (double) width_julia / P, width_julia);
	int w, h;

	for (i = start_julia; i < height_julia; i++) {
		result_julia[i] = malloc(width_julia * sizeof(int));
		if (result_julia[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	for (i = start_man; i < height_man; i++) {
		result_man[i] = malloc(width_man * sizeof(int));
		if (result_man[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}
	//inainte de a trece mai departe, trebuie sa se termine alocarile
	pthread_barrier_wait(&barrier);

	for (w = start_julia; w < end_julia; w++) {
		for (h = 0; h < height_julia; h++) {
			int step = 0;
			//z - punctul din planul complex in are definim multimea
			complex z = { .a = w * par_julia.resolution + par_julia.x_min,
							.b = h * par_julia.resolution + par_julia.y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par_julia.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par_julia.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par_julia.c_julia.b;

				step++;
			}

			result_julia[h][w] = step % 256;
		}
	}

	pthread_barrier_wait(&barrier1);
	if (id == 0) {
		write_output_file(out_filename_julia, result_julia, width_julia, height_julia);
	}
	pthread_barrier_wait(&barrier1);

	for (w = start_man; w < end_man; w++) {
		for (h = 0; h < height_man; h++) {
			complex c = { .a = w * par_man.resolution + par_man.x_min,
							.b = h * par_man.resolution + par_man.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par_man.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result_man[h][w] = step % 256;
		}
	}

	//bariera
	pthread_barrier_wait(&barrier2);
	// transforma rezultatul din coordonate matematice in coordonate ecran (logice)
	if (id == 0) {
		write_output_file(out_filename_mandelbrot, result_man, width_man, height_man);
	}
	pthread_barrier_wait(&barrier2);

	pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
	
	// se citesc argumentele programului
	get_args(argc, argv);
	long arguments[P];
	pthread_t threads[P];
	// Julia:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	read_input_file(in_filename_julia, &par_julia);
	read_input_file(in_filename_mandelbrot, &par_man);

	width_julia = (par_julia.x_max - par_julia.x_min) / par_julia.resolution;
	height_julia = (par_julia.y_max - par_julia.y_min) / par_julia.resolution;

	width_man = (par_man.x_max - par_man.x_min) / par_man.resolution;
	height_man = (par_man.y_max - par_man.y_min) / par_man.resolution;

	result_julia = (int **)malloc(height_julia * sizeof(int *));
	result_man = (int **)malloc(height_man * sizeof(int *));

	pthread_barrier_init(&barrier, NULL, P);
	pthread_barrier_init(&barrier1, NULL, P);
	pthread_barrier_init(&barrier2, NULL, P);


	long i;
	for (i = 0; i < P; i++) {
		arguments[i] = i;
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
	pthread_barrier_destroy(&barrier1);
	pthread_barrier_destroy(&barrier2);
	
	//free_memory(result_julia, height_julia);
	//free_memory(result_man, height_man);

	pthread_exit(NULL);
	
	return 0;
}
