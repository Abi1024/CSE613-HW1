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

void handle_error(int err){
    std::cerr << "PAPI error: " << err << std::endl;
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
	int r = 10;
	ofstream out;
	out.open("hw1B.csv");
	for (int i = 1; i < 7; i ++){
		int numEvents = 3;
	  long long values[3];
	  int events[3] = {PAPI_L1_TCM,PAPI_L2_TCM,PAPI_L3_TCM};
	  if (PAPI_start_counters(events, numEvents) != PAPI_OK){
		handle_error(1);
	  }
			cout << i << ",";
			out << i << ",";
			double time = matrix_multiply(r,i);
	 if (PAPI_stop_counters(values, numEvents) != PAPI_OK){
		handle_error(1);
	  }
			//cout << "\t Time(ms): " << time*1000 << endl;
		  //cout<<"L3 accesses: "<<valuesL3[0]<<endl;
		  //cout<<"L3 misses: "<<valuesL3[1]<<endl;
		  //cout<<"L3 miss/access ratio: "<<(double)valuesL3[1]/valuesL3[0]<<endl;
		  //cout<<"L1 accesses: "<<valuesL1[0]<<endl;
		  //cout<<"L1 misses: "<<valuesL1[1]<<endl;
		  //cout<<"L1 miss/access ratio: "<<(double)valuesL1[1]/valuesL1[0]<<endl;
		  //cout<<"L2 accesses: "<<valuesL2[0]<<endl;
		  //cout<<"L2 misses: "<<valuesL2[1]<<endl;
		  //cout<<"L2 miss/access ratio: "<<(double)valuesL2[1]/valuesL2[0]<<endl;
		  cout << values[0] << "," << values[1] << "," << values[2] << endl;
		  out << values[0] << "," << values[1] << "," << values[2] << endl;
	}
	out.close();
	return 0;
}
