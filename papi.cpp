#include <iostream>
#include <papi.h>
using namespace std;

void handle_error(int err){
    std::cerr << "PAPI error: " << err << std::endl;
}

int main(){
  int numEvents = 3;
  long long values[3];
  int events[3] = {PAPI_L1_TCM,PAPI_L2_TCM,PAPI_L3_TCM};
  if (PAPI_start_counters(events, numEvents) != PAPI_OK){
    handle_error(1);
  }
	cout << "ok" << endl;
  if ( PAPI_stop_counters(values, numEvents) != PAPI_OK){
    handle_error(1);
  }
  cout << "L1 misses: " << values[0] << endl;
  cout << "L2 misses: " << values[1] << endl;
  cout << "L3 misses: " << values[2] << endl;
  return 0;
}
