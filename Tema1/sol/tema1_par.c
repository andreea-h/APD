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
int **result;
int width, height;
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

params par;

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

	long id = *(long *)arg;
	//printf("Hello din thread-ul %ld!\n", id);

	int start = id * (double) width / P;
	int end = MIN ((id + 1) * (double) width / P, width);
	int w, h, i;

	for (w = start; w < end; w++) {
		for (h = 0; h < height; h++) {
			int step = 0;
			complex z = { .a = w * par.resolution + par.x_min,
							.b = h * par.resolution + par.y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par.c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	//bariera
	pthread_barrier_wait(&barrier);
	// transforma rezultatul din coordonate matematice in coordonate ecran (logice)
	if (id == P - 1) {
		for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height- i - 1] = aux;
		}
	}
	
	pthread_barrier_wait(&barrier);
	pthread_exit(NULL);
}

// ruleaza algoritmul Julia
void run_julia(params *par, int **result, int width, int height)
{
	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			int step = 0;
			complex z = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par->c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran (logice)
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
}

// ruleaza algoritmul Mandelbrot
void run_mandelbrot(params *par, int **result, int width, int height)
{
	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			complex c = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
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
	read_input_file(in_filename_julia, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;

	result = allocate_memory(width, height);

	pthread_barrier_init(&barrier, NULL, P);
	long i;
	for (i = 0; i < P; i++) {
		arguments[i] = i;
		//printf("(main)%ld\n", arguments[i]);
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
	
	//trebuie paralelizate
	//run_julia(&par, result, width, height);
	pthread_barrier_destroy(&barrier);
	write_output_file(out_filename_julia, result, width, height);
	free_memory(result, height);

	pthread_exit(NULL);
	// Mandelbrot:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	/*read_input_file(in_filename_mandelbrot, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;

	result = allocate_memory(width, height);
	run_mandelbrot(&par, result, width, height);
	write_output_file(out_filename_mandelbrot, result, width, height);
	free_memory(result, height);
*/
	return 0;
}
