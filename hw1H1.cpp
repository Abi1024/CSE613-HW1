#include <iostream>
#include <omp.h>
#include <math.h>
#include <cstdlib>
#include <fstream>
using namespace std;
float** global_Z;
float** global_X;
float** global_Y;
int global_n;

float rand_FloatRange(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

void PAR_REC_MM(int Z, int X, int Y, int size, int m){
  //printf("Before computation: Z:%d,X:%d,Y:%d,size:%d \n",Z,X,Y,size);
  /*for (int i = 0; i < global_n; i++){
    for (int j = 0; j < global_n; j++){
      printf("%d ",(int)global_Z[i][j]);
    }
      printf("\n");
  }*/
  if (size == m){
    int Z_row = Z/global_n;
    int Z_column = Z%global_n;
    int X_row = X/global_n;
    int X_column = X%global_n;
    int Y_row = Y/global_n;
    int Y_column = Y%global_n;
    for (int i = 0; i < size; i++){ //row number
      for (int k = 0; k < size; k++){ //iteration
        for (int j = 0; j < size; j++){  //column number
          global_Z[Z_row+i][Z_column+j] += (global_X[X_row+i][X_column+k]*global_Y[Y_row+k][Y_column+j]);
        }
      }
    }
  }else{
      #pragma omp parallel
      {
          #pragma omp single
          PAR_REC_MM(Z,X,Y,size/2,m);
          #pragma omp single
          PAR_REC_MM(Z+size/2,X,Y+size/2,size/2,m);
          #pragma omp single
          PAR_REC_MM(Z+global_n*size/2,X+global_n*size/2,Y,size/2,m);
          #pragma omp single
          PAR_REC_MM(Z+size/2+global_n*size/2,X+global_n*size/2,Y+size/2,size/2,m);
      }
      #pragma omp parallel
      {
          #pragma omp single
          PAR_REC_MM(Z,X+size/2,Y+global_n*size/2,size/2,m);
          #pragma omp single
          PAR_REC_MM(Z+size/2,X+size/2,Y+size/2+global_n*size/2,size/2,m);
          #pragma omp single
          PAR_REC_MM(Z+global_n*size/2,X+global_n*size/2+size/2,Y+global_n*size/2,size/2,m);
          #pragma omp single
          PAR_REC_MM(Z+size/2+global_n*size/2,X+global_n*size/2+size/2,Y+size/2+global_n*size/2,size/2,m);
      }
  }
  /*printf("After computation: Z:%d,X:%d,Y:%d,size:%d \n",Z,X,Y,size);
  for (int i = 0; i < global_n; i++){
    for (int j = 0; j < global_n; j++){
      printf("%d ",(int)global_Z[i][j]);
    }
      printf("\n");
  }
  printf("Leaving recursive call\n");
  */
}


double matrix_multiply(const int r, int m){
	const int n = pow(2,r);
  global_n = n;
	const float low = -10;
	const float high = 10;
	float** X = new float*[n];
	float** Y = new float*[n];
	float** Z = new float*[n];
  global_X = X;
  global_Y = Y;
  global_Z = Z;
	for (int i = 0; i < n; i++){
		X[i] = new float[n];
		Y[i] = new float[n];
		Z[i] = new float[n];
		for (int j = 0; j < n; j++){
			X[i][j] = 3.;
			Y[i][j] = 2.;
      //X[i][j] = rand_FloatRange(low,high);
			//Y[i][j] = rand_FloatRange(low,high);
			Z[i][j] = 0;
		}
	}
  double start_time = omp_get_wtime();
  PAR_REC_MM(0,0,0,n,m);
	double time = omp_get_wtime() - start_time;
  /*for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      cout << global_Z[i][j] << " ";
    }
      cout << endl;
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
  omp_set_num_threads(24);
	bool flag = true;
	//int r = 1;
	ofstream out;
  int m = 64;
  //cout << "Value of r: " << r << endl;
  //double time = matrix_multiply(r,m);
  //cout << "\t Time(ms): " << time*1000 << endl;
	out.open("hw1H1.csv");
	out << "r,Time(ms)" << endl;
	for (int r = 4; r < 12; r++){
		cout << "Value of r: " << r << endl;
		m = 64;
		if (pow(2,r) < m){
		m = pow(2,r);
		}else{
		m = 64;
		}
		double time = matrix_multiply(r,m);
		cout << "\t Time(ms): " << time*1000 << endl;
		out << r << "," << time*1000;
		out << endl;
	}
	out.close();

	return 0;
}
