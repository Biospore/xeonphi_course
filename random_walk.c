#include <stdio.h>
#include <omp.h>
#include <mkl_vsl_functions.h>
#include <mkl_vsl.h>
#include <mkl.h>
//#include <offload.h>

#define PI 3.14159265358979323846
#define radToDeg( radians ) ( ( radians ) * ( 180.0 / PI ) )

//Структура для хранения координат
struct position {
	double x;
	double y;
};

int main (void){
    //Поток, необходим для vdRngUniform и vdRngGaussian
	VSLStreamStatePtr stream;
	vslNewStream( &stream, VSL_BRNG_MT19937, 1);
    //Начало[a] и конец[sigma] отрезка
	double a = 0.0;
	double da = 0.0;
	double sigma = 300.3;
	double dsigma = 2*PI;
	int en = 10000;
	double seed[10000];
    //Создание семян генерации
	vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD, stream, en, seed, 1, 2000);
	vslDeleteStream(&stream);
	FILE * output;
	double start = omp_get_wtime();
//	#pragma omp parallel for
    //Основной цикл - N (10^6) экспериментов
	for (int exp = 0; exp <= en; exp+=1){
	    vslNewStream( &stream, VSL_BRNG_MT19937, seed[exp]);
		struct position walk[101];
        //Начальные координаты
		walk[0].x = 0;
		walk[0].y = 0;
		int n = 100;
		double directions[100];
		double lengths[100];
        //Генерация длин и углов шагов
		vdRngGaussian(VSL_RNG_METHOD_GAUSSIAN_ICDF, stream, n, lengths, a, sigma);
		vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD, stream, n, directions, da, dsigma);
		double coss[100];
		double sins[100];
		double iv[100];
        //Преобразование радиан в градусы
		for (int i=0; i<100; i+=1){
			iv[i] = radToDeg(directions[i]);
		}
        //Поиск косинусов и синусов углов
		vdCos(n, iv, coss);
		vdSin(n, iv, sins);
//		#pragma offload target(mic)
//		#pragma omp parallel for shared(walk, coss, sins, lengths)
        //Генерация 100 шагов
		for (int step = 1; step <= 100; step += 1){
			//calculate step as (length*cos(angle)+x;length*sin(angle)+y);
			double old_x = walk[step-1].x;
			double old_y = walk[step-1].y;
			walk[step].x = lengths[step-1]*coss[step-1]+old_x;
			walk[step].y = lengths[step-1]*sins[step-1]+old_y;
//			printf("nx:%f\tny:%f\n", walk[step].x, walk[step].y);
		}
		char filename[80];
		sprintf(filename, "samples/sample%d", exp);
		output = fopen(filename, "w+");
		for (int step=0; step<=100; step+=1){
			fprintf(output, "%f %f\n", walk[step].x, walk[step].y);
		}
		fclose(output);
		vslDeleteStream(&stream);
	}
//	vslDeleteStream( &stream );
//	printf("Hi!\n");
	double end = omp_get_wtime();
	printf("%f\n", (double)end-start);
	return 0;
}
