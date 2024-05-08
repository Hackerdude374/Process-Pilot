# CPU Scheduling Algorithms Comparison Simulator

## Overview
This repository contains a CPU scheduling simulation project implemented in C++. The project aims to compare the performance of two CPU scheduling algorithms: Shortest Job First (SJF) and Shortest Remaining Time Next (SRTN). The simulator executes a set of processes with CPU requirements and collects various statistics to evaluate the efficiency of each algorithm.

## Features

- **FCFS Algorithm**: Simulates the FCFS scheduling algorithm.
- **Round Robin Algorithm**: Simulates the Round Robin scheduling algorithm with a specified time quantum.
- **Performance Metrics**: Calculates and displays average turnaround time, waiting time, response time, and CPU efficiency.
- **Input**: Processes are read from an input file (`input.txt`) with each line containing process ID, arrival time, and burst time.
- **Output**: Provides detailed results for each process and overall performance metrics.
- 
## Getting Started

1. **Clone the Repository**: Clone this repository to your local machine.

   ```bash
   git clone https://github.com/Hackerdude374/ProcessPilot.git

2. **Navigate to Project Directory**: Move into the project directory.

   ```bash
   cd ProcessPilot

3. **Compile the Code /Run the Simulator**: Compile the C++ code using a C++ compiler. For example, using g++:

   ```bash
   g++ -o main main.cpp



