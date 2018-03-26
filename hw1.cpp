#include <iostream>
#include <omp.h>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <papi.h>
using namespace std;

float rand_FloatRange(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}


double matrix_multiply(const int r, int type){
	const int n = pow(2,r);
	const float low = -10;
	const float high = 10;
	float** X = new float*[n];
	float** Y = new float*[n];
	float** Z = new float*[n];
	for (int i = 0; i < n; i++){
		X[i] = new float[n];
		Y[i] = new float[n];
		Z[i] = new float[n];
		for (int j = 0; j < n; j++){
			X[i][j] = rand_FloatRange(low,high);
			Y[i][j] = rand_FloatRange(low,high);
			Z[i][j] = 0.0;
		}
	}
	double start_time = omp_get_wtime();
	switch(type){
		case 1:
			for (int i = 0; i < n; i++){
				for (int j = 0; j < n; j++){
					for (int k = 0; k < n; k++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 2:
			for (int i = 0; i < n; i++){
				for (int k = 0; k < n; k++){
					for (int j = 0; j < n; j++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 3:
			for (int j = 0; j < n; j++){
				for (int i = 0; i < n; i++){
					for (int k = 0; k < n; k++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 4:
			for (int j = 0; j < n; j++){
				for (int k = 0; k < n; k++){
					for (int i = 0; i < n; i++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 5:
			for (int k = 0; k < n; k++){
				for (int i = 0; i < n; i++){
					for (int j = 0; j < n; j++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 6:
			for (int k = 0; k < n; k++){
				for (int j = 0; j < n; j++){
					for (int i = 0; i < n; i++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		default:
			cout << "ERROR" << endl;
			exit(EXIT_FAILURE);
      break;
	}
	double time = omp_get_wtime() - start_time;
	for(int i = 0; i < n; i++) {
		delete[] X[i];
		delete[] Y[i];
		delete[] Z[i];
	}
	delete[] X;
	delete[] Y;
	delete[] Z;
	return time;
}


int main(int argc, char *argv[]){
	srand(1);
	bool flag = true;
	int r = 1;
	ofstream out;
	out.open("hw1A.csv");
	while(flag){
		double max_time = 0;
		cout << "Value of r: " << r << endl;
		for (int i = 1; i < 7; i ++){
			cout << "\t Using implementation number: " << i << endl;
			double time = matrix_multiply(r,i);
			cout << "\t Time(ms): " << time*1000 << endl;
			out << time*1000;
			if (i < 6){
				out  << ",";
			}
			if (time == 0 || time > max_time)
				max_time = time;
			if ((max_time > 300)||(r == 11)){
				flag = false;
			}
		}
		out << endl;
		r += 1;
	}
	out.close();
	return 0;
}
