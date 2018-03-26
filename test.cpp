#include <iostream>
int g_seed = 0;
using namespace std;

inline int fastrand() { 
  g_seed = (214013*g_seed+2531011); 
  return (g_seed>>16)&0x7FFF; 
} 

int main(){
  for (int i = 0; i < 100; i++){
	  cout << fastrand() << endl;
  }
  return 0;
}
