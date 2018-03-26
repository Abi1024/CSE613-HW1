First, load papi as follows:
module load papi

To run, compile any file <filename.cpp> as follows:
g++ -fopenmp -lpapi -I /opt/papi/intel/include -L /opt/papi/intel/lib  -std=gnu++11 <filename.cpp>

Then run on Comet using:
srun --nodes=1 --time=20:00 ./a.out

The following cpp files are used to generate the output in the PDF:
hw1.cpp
hw1B.cpp
hw1D.cpp
hw1D2.cpp
hw1E.cpp
hw1G.cpp
hw2centshare.cpp (part A task 2)
hw2Cshare.cpp (part C task 2)
hw2Cstealc.pp  (part C task 2)
hw2share.cpp  (part A task2)
hw2steal.cpp (part A task 2)
