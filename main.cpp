#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <cstring>
#include <map>
#include <iomanip>

using namespace std;

// event types for the CPU scheduling simulation
enum class EventType{ARRIVAL, COMPLETION, TIME_SLICE}; //completion for FCFS, time slice for RR

// Event class for managing simulation events.
class Event{
public:
    int event_time; // time when the event occurs
    EventType type; // type of event (arrival, completion, or time slice)
    int process_id; // ID of the process associated with the event

    Event(int time, EventType et, int pid) : event_time(time), type(et), process_id(pid){}

    // comparator for priority queue to order events based on event time (good for FCFS since process order matters)
    bool operator<(const Event& other) const{
        return event_time > other.event_time;
    }

};

// Process class representing a process in the simulation.
class Process{
public:
    int p_id; //unique process IDs for incomming processes
    int arrival_time, burst_time; // Arrival time and total CPU burst time of the process.
    int start_time, end_time; // Start and end times of process execution.
    int turnaround_time, waiting_time, response_time; // Performance metrics.
    int remaining_time; // Remaining burst time for Round Robin scheduling. (no need in FCFS since processes will be started then finished undisturbed)

    Process(int id, int arrival, int burst): 
      p_id(id), arrival_time(arrival), burst_time(burst), start_time(0), end_time(0),
      turnaround_time(0), waiting_time(0), response_time(0), remaining_time(burst) {}
};

// Sorting functions for processes.
bool sortByArrivalTime(const Process& p1, const Process& p2){
    return p1.arrival_time < p2.arrival_time;
}

bool sortByProcessID(const Process& p1, const Process& p2){
    return p1.p_id < p2.p_id; 
}

const int SWITCH_TIME = 2; //context switch time between processes, because naturally it takes time to save and load new process.

// calculate data of the processes
void calculateMetrics(vector<Process>& processes, float& avg_rt, float& avg_tat, float& avg_wt, float& cpu_efficiency, int total_execution_time, int total_switch_time){
    int total_tat = 0, total_wt = 0, total_rt = 0;

    for(auto& process : processes){
      // calculate times
      process.turnaround_time = process.end_time - process.arrival_time; //TAT = waiting time + service time, such as in this case the TAT is the processes's time of completion the moment it arrives in the ready queue unti it completes its burst time plus additional waiting times from stuff like context switching, convoy effect, etc.
      process.waiting_time = process.turnaround_time - process.burst_time; //Waiting time = TAT- Service Time
      process.response_time = process.start_time - process.arrival_time;

      total_tat += process.turnaround_time;
      total_wt += process.waiting_time;
      total_rt += process.response_time;

    }

    // Calculating average metrics and CPU efficiency.
    avg_tat = static_cast<float>(total_tat) / processes.size();
    avg_wt = static_cast<float>(total_wt) / processes.size();
    avg_rt = static_cast<float>(total_rt) / processes.size();
    cpu_efficiency = (static_cast<float>(total_execution_time) / (total_execution_time + total_switch_time)) * 100.0f; //efficient cpu utilization would have low execution times and low context swiitch times
}
//-------------------------------------------------------------------DISPLAY------------------------------------
// display the calculated results
void displayResults(const vector<Process>& processes, const float avg_rt, const float avg_tat, const float avg_wt, const float cpu_efficiency){
    cout << "Total Time required is " << processes.back().end_time << " time units\n";
    cout << "Average Turn Around Time: " << avg_tat << " time units\n";
    cout << "Average Waiting Time: " << avg_wt << " time units\n";
    cout << "Average Response Time: " << avg_rt << " time units\n";
    cout << "CPU Efficiency: " << cpu_efficiency << "%\n\n";

    // Displaying details for each process.
    for(const auto& process : processes){
      cout << "Process " << process.p_id << ":\n";
      cout << "Service time = " << process.burst_time << " time units\n";
      cout << "Turnaround time = " << process.turnaround_time << " time units\n";
      cout << "Waiting time = " << process.waiting_time << " time units\n";
      cout << "Response time = " << process.response_time << " time units\n\n";
    }
}

// Main function for running the CPU scheduling simulation.
void runSimulation(vector<Process>& processes,int time_quantum,bool isRR){
    priority_queue<Event> event_queue; // queue for process arrival order
    queue<int> ready_queue; // ready queue for processes ready to run
    map<int, Process*> process_map; // access process by ID On their map 
    int current_time = 0; // simulation current time
    Process* current_process = nullptr; // pointer to the currently waiting process in RQ
    int total_execution_time = 0; // total execution time for CPU efficiency calculation

    // Initialize the event queue with the arrival of all processes.
    for(auto& process:processes){
        event_queue.push(Event(process.arrival_time, EventType::ARRIVAL, process.p_id));
        process_map[process.p_id] = &process;
    }

    // Simulation loop processing each event in chronological order.
    while(!event_queue.empty()){
        Event event = event_queue.top();
        event_queue.pop();

        current_time = event.event_time; // Updating current time to the time of the event.

        switch(event.type){
            // Arrival event handling.
            case EventType::ARRIVAL:{
                auto& proc = process_map[event.process_id];
                if (!current_process){
                    current_process = proc;
                    current_process->start_time = current_time;
                    if(isRR){
                        // Schedule a time slice event for Round Robin (REMEMBER RR IS BASED ON A TIME SLICE SO EVERY PROCESS GET ITS FAIR SHARE.)
                        event_queue.push(Event(current_time + time_quantum, EventType::TIME_SLICE, proc->p_id));
                    }else{
                        // Schedule a completion event for FCFS (FCFS IS BASED ON COMPLETION OF PROCESSES IN ARRIVAL ORDER)
                        event_queue.push(Event(current_time + proc->burst_time, EventType::COMPLETION, proc->p_id));
                    }
                }else{
                    // If there is a current process, add this process to the ready queue.
                  ready_queue.push(proc->p_id);
                }
                break;
            }

          //---------------------------------------------------------------------------------
            // Completion event handling for FCFS
            case EventType::COMPLETION:{
              //RETRIEve process that just finished execitopm
                auto& proc = process_map[event.process_id];
                proc->end_time =current_time; // mark the completion time of the process. //this is the gap start
              // update the total execution time with the burst time (aka srrvcie time) of the completed process
              // This represents the cumulative time the CPU has been active

                total_execution_time+=proc->burst_time; // Add to total execution time
                current_process =nullptr; // Current process is now complete. reset it to null we dont need it anymore

                // Check if there's a next process in the ready queue.
                if (!ready_queue.empty()) { // there are processes waiting to be executed
                    int next_pid = ready_queue.front(); //arrival order matters in FCFS, get the one in front
                    ready_queue.pop(); //pop the selected process from the RQ to start its exeuction.
                    current_process = process_map[next_pid];
                    current_process->start_time = current_time; //this is the gap end. the current processes start time is now the current time.
                    // Schedule the next completion event for the new process.
                    event_queue.push(Event(current_time + current_process->burst_time, EventType::COMPLETION, next_pid));
                }
                break;
            }

//---------------------------------------------------------------------------------------------------
            // Time slice event handling for Round Robin.
            case EventType::TIME_SLICE:{
                if(current_process && current_process->p_id == event.process_id){
                    current_process->remaining_time -= time_quantum; // Decrease remaining time of current processs by time quantum (its their turn to execute in the given amount of time)
                    if(current_process->remaining_time <= 0){ //process COMPLETE! It finished in its TQ
                    
                        current_process->end_time = current_time; // mark completion time of the process
                        total_execution_time += (current_process->burst_time - current_process->remaining_time); //update exeuction time, account for the total time it ran for ^
                        current_process = nullptr; // reset current process since its already finished
                    }else{
                        // If process didnt finish in its TQ, enqueue it again back in the ready queue.
                        ready_queue.push(current_process->p_id);
                    }

                    // Scheduling processes from ready queue
                    if(!ready_queue.empty()){ //more processes waiting for its time share
                        int next_pid = ready_queue.front(); //pick the one in the front
                        ready_queue.pop(); // pop the selected process and start its execution
                        current_process = process_map[next_pid];
                        current_process->start_time = current_time;
                        // Schedule the next time slice event with time Quantum 
                        int next_time_slice = min(time_quantum, current_process->remaining_time);
                        event_queue.push(Event(current_time + next_time_slice, EventType::TIME_SLICE, next_pid));
                    }
                }
                break;
            }
        }
    }

    //display simulation results
    float avg_rt, avg_tat, avg_wt, cpu_efficiency;
    calculateMetrics(processes, avg_rt, avg_tat, avg_wt, cpu_efficiency, total_execution_time, SWITCH_TIME * processes.size());
    displayResults(processes, avg_rt, avg_tat, avg_wt, cpu_efficiency);
}

// read input.txt
vector<Process> readProcesses(const string& filename, int num_processes){
    ifstream infile(filename); // Open input.txt
    vector<Process> processes; // Vector to store processes.
    int id, arrival, burst;

    //read each process
    for(int i = 0; i < num_processes; ++i){
        if(infile >> id >> arrival >> burst){
            processes.emplace_back(id, arrival, burst);
        }
    }

    return processes; //return list of process
}

int main(){
    int num_processes, choice, time_quantum;
    vector<Process> processes;

    //user prompt
    cout << "Choose the Scheduling Algorithm:\n";
    cout << "1. First-Come, First-Served (FCFS)\n";
    cout << "2. Round Robin (RR)\n";
    cout << "Enter your choice (1 or 2): ";
    cin>>choice; 

    
    cout<<"Enter number of processes: ";
    cin>>num_processes; 
    // Check if the number of processes exceeds the limit of 50.
    if(num_processes>50){ //exit if limit exceeded
        cout << "Error: Exceeds maximum process limit of 50.\n";
        return 1; 
    }

    // read processes from input file 
    processes = readProcesses("input.txt", num_processes);

    // switch statements to execute algorithms
    switch(choice){
        case 1:
            // If choice is 1, run FCFS
            runSimulation(processes, -1, false); // FCFS
            break;
        case 2:
            // If choice is 2, run RR
            cout << "Enter the time quantum for RR: ";
            cin >> time_quantum; // TQ input
            runSimulation(processes, time_quantum, true); 
            break;
        default:
            // If an invalid choice is entered, show an error message.
            cout << "Invalid choice!\n";
            return 1; // Exit the program.
    }
    return 0; 
}

//
// Choose the Scheduling Algorithm:
// 1. First-Come, First-Served (FCFS)
// 2. Round Robin (RR)
// Enter your choice (1 or 2): 1
// Enter number of processes: 50
// Total Time required is 2321 time units
// Average Turn Around Time: 1172.98 time units
// Average Waiting Time: 1126.56 time units
// Average Response Time: 1126.56 time units
// CPU Efficiency: 95.8695%

// Process 1:
// Service time = 80 time units
// Turnaround time = 80 time units
// Waiting time = 0 time units
// Response time = 0 time units

// Process 2:
// Service time = 17 time units
// Turnaround time = 97 time units
// Waiting time = 80 time units
// Response time = 80 time units

// Process 3:
// Service time = 40 time units
// Turnaround time = 136 time units
// Waiting time = 96 time units
// Response time = 96 time units

// Process 4:
// Service time = 29 time units
// Turnaround time = 163 time units
// Waiting time = 134 time units
// Response time = 134 time units

// Process 5:
// Service time = 95 time units
// Turnaround time = 327 time units
// Waiting time = 232 time units
// Response time = 232 time units

// Process 6:
// Service time = 48 time units
// Turnaround time = 232 time units
// Waiting time = 184 time units
// Response time = 184 time units

// Process 7:
// Service time = 22 time units
// Turnaround time = 184 time units
// Waiting time = 162 time units
// Response time = 162 time units

// Process 8:
// Service time = 24 time units
// Turnaround time = 350 time units
// Waiting time = 326 time units
// Response time = 326 time units

// Process 9:
// Service time = 20 time units
// Turnaround time = 369 time units
// Waiting time = 349 time units
// Response time = 349 time units

// Process 10:
// Service time = 60 time units
// Turnaround time = 428 time units
// Waiting time = 368 time units
// Response time = 368 time units

// Process 11:
// Service time = 42 time units
// Turnaround time = 492 time units
// Waiting time = 450 time units
// Response time = 450 time units

// Process 12:
// Service time = 23 time units
// Turnaround time = 450 time units
// Waiting time = 427 time units
// Response time = 427 time units

// Process 13:
// Service time = 103 time units
// Turnaround time = 594 time units
// Waiting time = 491 time units
// Response time = 491 time units

// Process 14:
// Service time = 23 time units
// Turnaround time = 616 time units
// Waiting time = 593 time units
// Response time = 593 time units

// Process 15:
// Service time = 95 time units
// Turnaround time = 705 time units
// Waiting time = 610 time units
// Response time = 610 time units

// Process 16:
// Service time = 105 time units
// Turnaround time = 809 time units
// Waiting time = 704 time units
// Response time = 704 time units

// Process 17:
// Service time = 41 time units
// Turnaround time = 848 time units
// Waiting time = 807 time units
// Response time = 807 time units

// Process 18:
// Service time = 35 time units
// Turnaround time = 919 time units
// Waiting time = 884 time units
// Response time = 884 time units

// Process 19:
// Service time = 37 time units
// Turnaround time = 884 time units
// Waiting time = 847 time units
// Response time = 847 time units

// Process 20:
// Service time = 5 time units
// Turnaround time = 983 time units
// Waiting time = 978 time units
// Response time = 978 time units

// Process 21:
// Service time = 50 time units
// Turnaround time = 978 time units
// Waiting time = 928 time units
// Response time = 928 time units

// Process 22:
// Service time = 12 time units
// Turnaround time = 928 time units
// Waiting time = 916 time units
// Response time = 916 time units

// Process 23:
// Service time = 47 time units
// Turnaround time = 1020 time units
// Waiting time = 973 time units
// Response time = 973 time units

// Process 24:
// Service time = 6 time units
// Turnaround time = 1025 time units
// Waiting time = 1019 time units
// Response time = 1019 time units

// Process 25:
// Service time = 91 time units
// Turnaround time = 1115 time units
// Waiting time = 1024 time units
// Response time = 1024 time units

// Process 26:
// Service time = 68 time units
// Turnaround time = 1180 time units
// Waiting time = 1112 time units
// Response time = 1112 time units

// Process 27:
// Service time = 4 time units
// Turnaround time = 1407 time units
// Waiting time = 1403 time units
// Response time = 1403 time units

// Process 28:
// Service time = 17 time units
// Turnaround time = 1194 time units
// Waiting time = 1177 time units
// Response time = 1177 time units

// Process 29:
// Service time = 152 time units
// Turnaround time = 1406 time units
// Waiting time = 1254 time units
// Response time = 1254 time units

// Process 30:
// Service time = 63 time units
// Turnaround time = 1254 time units
// Waiting time = 1191 time units
// Response time = 1191 time units

// Process 31:
// Service time = 77 time units
// Turnaround time = 1484 time units
// Waiting time = 1407 time units
// Response time = 1407 time units

// Process 32:
// Service time = 14 time units
// Turnaround time = 1497 time units
// Waiting time = 1483 time units
// Response time = 1483 time units

// Process 33:
// Service time = 79 time units
// Turnaround time = 1573 time units
// Waiting time = 1494 time units
// Response time = 1494 time units

// Process 34:
// Service time = 43 time units
// Turnaround time = 1614 time units
// Waiting time = 1571 time units
// Response time = 1571 time units

// Process 35:
// Service time = 13 time units
// Turnaround time = 1627 time units
// Waiting time = 1614 time units
// Response time = 1614 time units

// Process 36:
// Service time = 97 time units
// Turnaround time = 1720 time units
// Waiting time = 1623 time units
// Response time = 1623 time units

// Process 37:
// Service time = 44 time units
// Turnaround time = 1760 time units
// Waiting time = 1716 time units
// Response time = 1716 time units

// Process 38:
// Service time = 76 time units
// Turnaround time = 1833 time units
// Waiting time = 1757 time units
// Response time = 1757 time units

// Process 39:
// Service time = 37 time units
// Turnaround time = 1866 time units
// Waiting time = 1829 time units
// Response time = 1829 time units

// Process 40:
// Service time = 87 time units
// Turnaround time = 1953 time units
// Waiting time = 1866 time units
// Response time = 1866 time units

// Process 41:
// Service time = 18 time units
// Turnaround time = 1968 time units
// Waiting time = 1950 time units
// Response time = 1950 time units

// Process 42:
// Service time = 21 time units
// Turnaround time = 1987 time units
// Waiting time = 1966 time units
// Response time = 1966 time units

// Process 43:
// Service time = 42 time units
// Turnaround time = 2027 time units
// Waiting time = 1985 time units
// Response time = 1985 time units

// Process 44:
// Service time = 11 time units
// Turnaround time = 2035 time units
// Waiting time = 2024 time units
// Response time = 2024 time units

// Process 45:
// Service time = 31 time units
// Turnaround time = 2044 time units
// Waiting time = 2013 time units
// Response time = 2013 time units

// Process 46:
// Service time = 55 time units
// Turnaround time = 2092 time units
// Waiting time = 2037 time units
// Response time = 2037 time units

// Process 47:
// Service time = 41 time units
// Turnaround time = 2124 time units
// Waiting time = 2083 time units
// Response time = 2083 time units

// Process 48:
// Service time = 18 time units
// Turnaround time = 2141 time units
// Waiting time = 2123 time units
// Response time = 2123 time units

// Process 49:
// Service time = 26 time units
// Turnaround time = 2068 time units
// Waiting time = 2042 time units
// Response time = 2042 time units

// Process 50:
// Service time = 37 time units
// Turnaround time = 2063 time units
// Waiting time = 2026 time units
// Response time = 2026 time units

// Choose the Scheduling Algorithm:
// 1. First-Come, First-Served (FCFS)
// 2. Round Robin (RR)
// Enter your choice (1 or 2): 2
// Enter number of processes: 50
// Enter the time quantum for RR: 2
// Total Time required is 1613 time units
// Average Turn Around Time: 1386.6 time units
// Average Waiting Time: 1340.18 time units
// Average Response Time: 1385.18 time units
// CPU Efficiency: 95.9184%

// Process 1:
// Service time = 80 time units
// Turnaround time = 2066 time units
// Waiting time = 1986 time units
// Response time = 2064 time units

// Process 2:
// Service time = 17 time units
// Turnaround time = 570 time units
// Waiting time = 553 time units
// Response time = 569 time units

// Process 3:
// Service time = 40 time units
// Turnaround time = 1388 time units
// Waiting time = 1348 time units
// Response time = 1386 time units

// Process 4:
// Service time = 29 time units
// Turnaround time = 1100 time units
// Waiting time = 1071 time units
// Response time = 1099 time units

// Process 5:
// Service time = 95 time units
// Turnaround time = 2229 time units
// Waiting time = 2134 time units
// Response time = 2228 time units

// Process 6:
// Service time = 48 time units
// Turnaround time = 1619 time units
// Waiting time = 1571 time units
// Response time = 1617 time units

// Process 7:
// Service time = 22 time units
// Turnaround time = 814 time units
// Waiting time = 792 time units
// Response time = 812 time units

// Process 8:
// Service time = 24 time units
// Turnaround time = 930 time units
// Waiting time = 906 time units
// Response time = 928 time units

// Process 9:
// Service time = 20 time units
// Turnaround time = 780 time units
// Waiting time = 760 time units
// Response time = 778 time units

// Process 10:
// Service time = 60 time units
// Turnaround time = 1845 time units
// Waiting time = 1785 time units
// Response time = 1843 time units

// Process 11:
// Service time = 42 time units
// Turnaround time = 1503 time units
// Waiting time = 1461 time units
// Response time = 1501 time units

// Process 12:
// Service time = 23 time units
// Turnaround time = 942 time units
// Waiting time = 919 time units
// Response time = 941 time units

// Process 13:
// Service time = 103 time units
// Turnaround time = 2257 time units
// Waiting time = 2154 time units
// Response time = 2256 time units

// Process 14:
// Service time = 23 time units
// Turnaround time = 957 time units
// Waiting time = 934 time units
// Response time = 956 time units

// Process 15:
// Service time = 95 time units
// Turnaround time = 2224 time units
// Waiting time = 2129 time units
// Response time = 2223 time units

// Process 16:
// Service time = 105 time units
// Turnaround time = 2256 time units
// Waiting time = 2151 time units
// Response time = 2255 time units

// Process 17:
// Service time = 41 time units
// Turnaround time = 1527 time units
// Waiting time = 1486 time units
// Response time = 1526 time units

// Process 18:
// Service time = 35 time units
// Turnaround time = 1365 time units
// Waiting time = 1330 time units
// Response time = 1364 time units

// Process 19:
// Service time = 37 time units
// Turnaround time = 1422 time units
// Waiting time = 1385 time units
// Response time = 1421 time units

// Process 20:
// Service time = 5 time units
// Turnaround time = 212 time units
// Waiting time = 207 time units
// Response time = 211 time units

// Process 21:
// Service time = 50 time units
// Turnaround time = 1697 time units
// Waiting time = 1647 time units
// Response time = 1695 time units

// Process 22:
// Service time = 12 time units
// Turnaround time = 492 time units
// Waiting time = 480 time units
// Response time = 490 time units

// Process 23:
// Service time = 47 time units
// Turnaround time = 1659 time units
// Waiting time = 1612 time units
// Response time = 1658 time units

// Process 24:
// Service time = 6 time units
// Turnaround time = 229 time units
// Waiting time = 223 time units
// Response time = 227 time units

// Process 25:
// Service time = 91 time units
// Turnaround time = 2191 time units
// Waiting time = 2100 time units
// Response time = 2190 time units

// Process 26:
// Service time = 68 time units
// Turnaround time = 1960 time units
// Waiting time = 1892 time units
// Response time = 1958 time units

// Process 27:
// Service time = 4 time units
// Turnaround time = 149 time units
// Waiting time = 145 time units
// Response time = 147 time units

// Process 28:
// Service time = 17 time units
// Turnaround time = 778 time units
// Waiting time = 761 time units
// Response time = 777 time units

// Process 29:
// Service time = 152 time units
// Turnaround time = 2277 time units
// Waiting time = 2125 time units
// Response time = 2275 time units

// Process 30:
// Service time = 63 time units
// Turnaround time = 1904 time units
// Waiting time = 1841 time units
// Response time = 1903 time units

// Process 31:
// Service time = 77 time units
// Turnaround time = 2072 time units
// Waiting time = 1995 time units
// Response time = 2071 time units

// Process 32:
// Service time = 14 time units
// Turnaround time = 619 time units
// Waiting time = 605 time units
// Response time = 617 time units

// Process 33:
// Service time = 79 time units
// Turnaround time = 2091 time units
// Waiting time = 2012 time units
// Response time = 2090 time units

// Process 34:
// Service time = 43 time units
// Turnaround time = 1588 time units
// Waiting time = 1545 time units
// Response time = 1587 time units

// Process 35:
// Service time = 13 time units
// Turnaround time = 627 time units
// Waiting time = 614 time units
// Response time = 626 time units

// Process 36:
// Service time = 97 time units
// Turnaround time = 2202 time units
// Waiting time = 2105 time units
// Response time = 2201 time units

// Process 37:
// Service time = 44 time units
// Turnaround time = 1584 time units
// Waiting time = 1540 time units
// Response time = 1582 time units

// Process 38:
// Service time = 76 time units
// Turnaround time = 2044 time units
// Waiting time = 1968 time units
// Response time = 2042 time units

// Process 39:
// Service time = 37 time units
// Turnaround time = 1437 time units
// Waiting time = 1400 time units
// Response time = 1436 time units

// Process 40:
// Service time = 87 time units
// Turnaround time = 2141 time units
// Waiting time = 2054 time units
// Response time = 2140 time units

// Process 41:
// Service time = 18 time units
// Turnaround time = 802 time units
// Waiting time = 784 time units
// Response time = 800 time units

// Process 42:
// Service time = 21 time units
// Turnaround time = 947 time units
// Waiting time = 926 time units
// Response time = 946 time units

// Process 43:
// Service time = 42 time units
// Turnaround time = 1535 time units
// Waiting time = 1493 time units
// Response time = 1533 time units

// Process 44:
// Service time = 11 time units
// Turnaround time = 555 time units
// Waiting time = 544 time units
// Response time = 554 time units

// Process 45:
// Service time = 31 time units
// Turnaround time = 1262 time units
// Waiting time = 1231 time units
// Response time = 1261 time units

// Process 46:
// Service time = 55 time units
// Turnaround time = 1760 time units
// Waiting time = 1705 time units
// Response time = 1759 time units

// Process 47:
// Service time = 41 time units
// Turnaround time = 1522 time units
// Waiting time = 1481 time units
// Response time = 1521 time units

// Process 48:
// Service time = 18 time units
// Turnaround time = 804 time units
// Waiting time = 786 time units
// Response time = 802 time units

// Process 49:
// Service time = 26 time units
// Turnaround time = 1041 time units
// Waiting time = 1015 time units
// Response time = 1039 time units

// Process 50:
// Service time = 37 time units
// Turnaround time = 1355 time units
// Waiting time = 1318 time units
// Response time = 1354 time units


