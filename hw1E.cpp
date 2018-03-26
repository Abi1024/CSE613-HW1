#include <iostream>
#include <omp.h>
#include <math.h>
#include <cstdlib>
#include <fstream>
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
			X[i][j] = 2.;
			Y[i][j] = 3.;
			Z[i][j] = 0.0;
		}
	}
	double start_time = omp_get_wtime();
	switch(type){
      case 21: //(ikj) Parallelize: i
          #pragma omp parallel for 
            for (int i = 0; i < n; i++){
                  for (int k = 0; k < n; k++){
                        for (int j = 0; j < n; j++){
                          Z[i][j] += (X[i][k]*Y[k][j]);
                        }
                  }
			}
			break;
      case 22: //(ikj) Parallelize: i,j
          #pragma omp parallel for 
            for (int i = 0; i < n; i++){
                  for (int k = 0; k < n; k++){
						#pragma omp parallel for 
						for (int j = 0; j < n; j++){
                            Z[i][j] += (X[i][k]*Y[k][j]);
                          }
 
                  }
            }
          break;
        case 23: //(ikj) Parallelize: j
            for (int i = 0; i < n; i++){
                  for (int k = 0; k < n; k++){
                        #pragma omp parallel for 
                          for (int j = 0; j < n; j++){
                            Z[i][j] += (X[i][k]*Y[k][j]);
                          }                       
                  }
            }
            break;
		case 51: //(kij) Parallelize: i
			for (int k = 0; k < n; k++){ 
				#pragma omp parallel for
				for (int i = 0; i < n; i++){
					for (int j = 0; j < n; j++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 52: //(kij) Parallelize: i,j
			for (int k = 0; k < n; k++){ 
				#pragma omp parallel for
				for (int i = 0; i < n; i++){
					#pragma omp parallel for
					for (int j = 0; j < n; j++){
						Z[i][j] += (X[i][k]*Y[k][j]);
					}
				}
			}
			break;
		case 53: //(kij) Parallelize: j
			for (int k = 0; k < n; k++){ 
				for (int i = 0; i < n; i++){
					#pragma omp parallel for
					for (int j = 0; j < n; j++){
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
  /*for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (Z[i][j] != 768.){
        cout << Z[i][j] << endl;
        cout << "INCORRECT!" << endl;
        exit(EXIT_FAILURE);
      }
    }
  }
  cout << endl;
  */
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


int main(){
	srand(1);
	int max_cores = 24;
	bool flag = true;
	//int r = 1;
	ofstream out;
	int r = 11;
	out.open("hw1E.csv");
	for (int i = 1; i < max_cores; i++){
		omp_set_num_threads(i);
		double time;
		
		cout << "Number of cores: " << i << endl;
		time = matrix_multiply(r,21);
		cout << "\t Time(ms): " << time*1000 << endl;
		out << i << ",";
		out << time*1000 << ",";
		
		time = matrix_multiply(r,51);
		cout << "\t Time(ms): " << time*1000 << endl;
		out << time*1000;
		out << endl;
	}

	out.close();

	return 0;
}
