#include <iostream>
#include <omp.h>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <list>
using namespace std;

float** global_Z;
float** global_X;
float** global_Y;
int global_n;
struct Task;
void initialize_matrices(int);
void finish_task(Task*,int,int);
vector<list<Task*>> deques;
bool first_task = false;
bool flag = true;

struct Task{
  /*
    trees of tasks. when a task is completed (its pointer is set to 0), decrement the parent's counter. recurse on the parent
    when the root
  */
  vector<int> task; //the task consists of the parameters to PAR_REC_MM (Z,X,Y,size)
  Task* parent; //the task that spawned this task
  int counter; //the number of unfinished tasks spawned by this task which this task is waiting for
  bool first_phase; //is this task in the first phase of the recursion or the second phase?
  Task(vector<int> t, Task* p1, int c, bool p2 = true) : task(t), parent(p1), counter(c), first_phase(p2) {}
};

int main(){
  const int num_cores = 24;
  omp_set_num_threads(num_cores);
  int m = 16;
  int r = 11;
  ofstream out;
  out.open("hw2Cshare.csv");
  for (int i = 1; i < num_cores; i++){
	  omp_set_num_threads(i);
	  cout << "Num cores: " << i << endl;
	  m = 64;
	  if (pow(2,r) < m){
		m = pow(2,r);
	  }else{
		m = 64;
	  }
	  //cout << "m: " << m << endl;
	  out << i << ",";
      initialize_matrices(r);
	  first_task = false;
	  flag = true;
	  double start_time = omp_get_wtime();
	  #pragma omp parallel
	  {
		#pragma omp for
		for (int i = 0; i < omp_get_num_threads(); i++){
			  int thread_ID = omp_get_thread_num();
			  int num_threads = omp_get_num_threads();
			  //printf("num threads:%d\n",num_threads);
			   //setup the first task, by assigning it to a random thread
			  #pragma omp critical
			  {
				  if (!first_task){
					//printf("Assigning first task\n");
					first_task = true;
					Task* n = new Task(vector<int>{0,0,0,global_n},NULL,0,true);
					deques = vector<list<Task*>>(num_threads,list<Task*>{});
					deques[thread_ID].push_back(n);
					//cout << deques[thread_ID].size() << endl;
					//printf("Assigned first task\n");
				  }
			  }
			  while(flag){
				Task* current_task = NULL;
				//printf("Checking if deque is empty\n");
				#pragma omp critical
				{
				  if (deques[thread_ID].size() > 0){
					//printf("Deque is not empty\n");
					current_task = deques[thread_ID].front();
					deques[thread_ID].pop_front();
					//printf("deque size:%d\n",   deques[thread_ID].size());
				  }else{
					//printf("No work!\n");
				  }
				}
				//printf("Checking if there is a task\n");
				if (current_task){ //if task is ready
				  //printf("Executing task: Z:%d X:%d Y:%d size:%d\n", current_task->task[0],current_task->task[1],current_task->task[2],current_task->task[3]);
				  if (current_task->task[3] == m){ //if task is small enough, do it immediately (base case)
					//printf("Small enough size\n");
					int Z_row = current_task->task[0]/global_n;
					int Z_column = current_task->task[0]%global_n;
					int X_row = current_task->task[1]/global_n;
					int X_column = current_task->task[1]%global_n;
					int Y_row = current_task->task[2]/global_n;
					int Y_column = current_task->task[2]%global_n;
					//printf("Before matrix updates\n");
					/*for (int i = 0; i < global_n; i++){
					  for (int j = 0; j < global_n; j++){
						cout << global_Z[i][j] << " ";
					  }
					  cout << endl;
					}*/
					for (int i = 0; i < current_task->task[3]; i++){ //row number
					  for (int k = 0; k < current_task->task[3]; k++){ //iteration
						for (int j = 0; j < current_task->task[3]; j++){  //column number
						  global_Z[Z_row+i][Z_column+j] += (global_X[X_row+i][X_column+k]*global_Y[Y_row+k][Y_column+j]);
						}
					  }
					}
					//printf("After matrix updates\n");
					/*for (int i = 0; i < global_n; i++){
					  for (int j = 0; j < global_n; j++){
						cout << global_Z[i][j] << " ";
					  }
					  cout << endl;
					}*/
					finish_task(current_task, thread_ID, num_threads);
				  }else{ //otherwise, recurse
					int Z = current_task->task[0];
					int X = current_task->task[1];
					int Y = current_task->task[2];
					int size = current_task->task[3];
					Task* task1 = new Task(vector<int>{Z,X,Y,size/2},current_task,0);
					Task* task2 = new Task(vector<int>{Z+size/2,X,Y+size/2,size/2},current_task,0);
					Task* task3 = new Task(vector<int>{Z+global_n*size/2,X+global_n*size/2,Y,size/2},current_task,0);
					Task* task4 = new Task(vector<int>{Z+size/2+global_n*size/2,X+global_n*size/2,Y+size/2,size/2},current_task,0);
					current_task->counter += 4;
					int random_thread1 = rand()%num_threads;
					int random_thread2 = rand()%num_threads;
					int random_thread3 = rand()%num_threads;
					#pragma omp critical
					{
					  deques[random_thread1].push_front(task4);
					  deques[random_thread2].push_front(task3);
					  deques[random_thread3].push_front(task2);
					  deques[thread_ID].push_front(task1);
					}
				  }
				}
				//printf("End of iteration\n");
			  }
		}
	  }
	  double time = omp_get_wtime() - start_time;
	  cout << "\t Time(ms): " << time*1000 << endl;
	  out << time*1000;
	  out << endl;
  }
  /*cout << "done" << endl;
  for (int i = 0; i < global_n; i++){
    for (int j = 0; j < global_n; j++){
      cout << global_Z[i][j] << " ";
    }
    cout << endl;
  }*/
  out.close();
  return 0;
}

void initialize_matrices(int r){
  srand(1);
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
			Z[i][j] = 0;
		}
	}
}

void finish_task(Task* current_task, int thread_ID, int num_threads){
  //printf("Running finish task\n");
  if (current_task->parent != NULL){
    //printf("Parent is not null. Parent's counter:%d\n",current_task->parent->counter);
    #pragma omp atomic
    --current_task->parent->counter;
    if (current_task->parent->counter == 0){
      //printf("Parent's counter was set to 0.\n");
      if (current_task->parent->first_phase){
        //printf("First phase.\n");
        int Z = current_task->parent->task[0];
        int X = current_task->parent->task[1];
        int Y = current_task->parent->task[2];
        int size = current_task->parent->task[3];
        Task* parent_task = current_task->parent;
        parent_task->first_phase = false;
        delete current_task;
        Task* task1 = new Task(vector<int>{Z,X+size/2,Y+global_n*size/2,size/2},parent_task,0);
        Task* task2 = new Task(vector<int>{Z+size/2,X+size/2,Y+size/2+global_n*size/2,size/2},parent_task,0);
        Task* task3 = new Task(vector<int>{Z+global_n*size/2,X+global_n*size/2+size/2,Y+global_n*size/2,size/2},parent_task,0);
        Task* task4 = new Task(vector<int>{Z+size/2+global_n*size/2,X+global_n*size/2+size/2,Y+size/2+global_n*size/2,size/2},parent_task,0);
        parent_task->counter += 4;
        int random_thread1 = rand()%num_threads;
        int random_thread2 = rand()%num_threads;
        int random_thread3 = rand()%num_threads;
        #pragma omp critical
        {
          deques[random_thread1].push_front(task4);
          deques[random_thread2].push_front(task3);
          deques[random_thread3].push_front(task2);
          deques[thread_ID].push_front(task1);
        }
      }else{
          //printf("Second phase.\n");
          //recurse on parent
          Task* parent = current_task->parent;
          delete current_task;
          finish_task(parent, thread_ID, num_threads);
      }
    }else{
      delete current_task;
    }
  }else{
    delete current_task;
    flag = false;
    return;
  }
}
