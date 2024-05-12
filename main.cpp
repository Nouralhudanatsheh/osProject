
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

struct Process {
  int pid;
  float arrivalTime;
  float finishTime;
  float startTime;
  float turnanroundTime;
  float waitingTime;
  int cpuBursts;
  int remainingBursts;
};

class schedulingAlg {
private:
  int contextSwitch;
  int quantum;
  vector<Process> processes;
  struct com {
    bool operator()(Process p1, Process p2) {
      return p1.arrivalTime > p2.arrivalTime;
    }
  };
  void ganttChartandDetails(vector<Process> processes, float cpuTime,
                            float idleTime) {

    // lines for loop
    if (processes[0].arrivalTime != 0) {
      cout << "|";
      for (int k = 0; k < processes[0].arrivalTime; k++)
        cout << "-";
    }
    for (int i = 0; i < processes.size(); i++) {
      cout << "|";

      if (i > 0 && processes[i].startTime > processes[i - 1].finishTime) {
        for (int k = 0;
             k < processes[i].startTime - processes[i - 1].finishTime; k++)
          cout << "-";
        cout << "|";
      }
      for (int j = 0; j < processes[i].cpuBursts; j++) {
        if (j == processes[i].cpuBursts / 2)
          cout << "P" << processes[i].pid;
        else
          cout << " ";
      }
    }
    cout << "|\n";

    // numbers for loop
    if (processes[0].arrivalTime != 0) {
      cout << 0;
      for (int k = 0; k < processes[0].arrivalTime; k++)
        cout << " ";
    }
    for (int i = 0; i < processes.size(); i++) {
      if (i > 0 && processes[i].finishTime > processes[i - 1].finishTime) {
        cout << processes[i - 1].finishTime;
        for (int k = 0;
             k < processes[i].startTime - processes[i - 1].finishTime * 1.05;
             k++)
          cout << " ";
      }

      cout << processes[i].startTime;
      for (int j = 0; j < processes[i].cpuBursts; j++) {
        cout << " ";
      }
    }
    cout << processes[processes.size() - 1].finishTime << endl;
    cout << "\nCPU utilization = (" << cpuTime << " - " << idleTime << ") / "
         << cpuTime << " * 100% = " << (cpuTime - idleTime) * 100 / cpuTime
         << "%" << endl;
    cout << "\nDetails of each process:\n";
    for (int i = 0; i < processes.size(); i++) {
      cout << ".process " << processes[i].pid

           << " \n finish time: " << processes[i].finishTime

           << " \n turnaround time:" << processes[i].turnanroundTime

           << " \n waiting time: " << processes[i].waitingTime << endl
           << endl;
    }
  }

public:
  // constructor to read data from file
  schedulingAlg() {
    ifstream fin;
    fin.open("/Users/nouralhudanatsheh/Documents/ppu/CSE 3rd year/2nd "
             "semester/os/project/osProject.txt");
    if (fin.fail()) {
      cout << "couldn't open file";
    } else {
      fin >> contextSwitch >> quantum;
      float arrivalTime;
      float cpuBursts;
      for (int i = 0; fin >> arrivalTime >> cpuBursts; i++) {
        Process p;
        p.pid = i;
        p.arrivalTime = arrivalTime;
        p.cpuBursts = cpuBursts;
        processes.push_back(p);
      }
      fin.close();
    }
  }

  void FCFS() {
    if (!processes.empty()) {
      priority_queue<Process, vector<Process>, com> FCFS;
      for (int i = 0; i < processes.size(); i++) {
        FCFS.push(processes[i]);
      }

      vector<Process> sortedProcesses;
      for (int i = 0; !FCFS.empty(); i++) {
        sortedProcesses.push_back(FCFS.top());
        FCFS.pop();
      }
      float cpuTime = sortedProcesses[0].finishTime =
          sortedProcesses[0].arrivalTime + sortedProcesses[0].cpuBursts;
      float idleTime = sortedProcesses[0].arrivalTime;
      sortedProcesses[0].startTime = sortedProcesses[0].arrivalTime;
      sortedProcesses[0].turnanroundTime =
          cpuTime - sortedProcesses[0].arrivalTime;

      for (int i = 1; i < processes.size(); i++) {

        if (sortedProcesses[i].arrivalTime > cpuTime) {
          idleTime += sortedProcesses[i].arrivalTime - cpuTime;
          cpuTime += sortedProcesses[i].arrivalTime - cpuTime;
        }
        cpuTime += contextSwitch;
        idleTime += contextSwitch;
        sortedProcesses[i].startTime = cpuTime;
        sortedProcesses[i].finishTime =
            sortedProcesses[i].startTime + sortedProcesses[i].cpuBursts;
        sortedProcesses[i].turnanroundTime =
            sortedProcesses[i].finishTime - sortedProcesses[i].arrivalTime;
        sortedProcesses[i].waitingTime =
            sortedProcesses[i].startTime - sortedProcesses[i].arrivalTime;
        cpuTime += sortedProcesses[i].cpuBursts;
      }
      cout << "First Come First Served Algorithm\n\n";
      ganttChartandDetails(sortedProcesses, cpuTime, idleTime);
      // cout << " cpu time = " << cpuTime << " idle time = " << idleTime
      //      << " cpu utilization time =  " << (cpuTime - idleTime) / cpuTime;
    } else {
      cout << "no processes found";
    }
  }

  void SRT() {}
};

int main() {
  schedulingAlg test;
  test.FCFS();
  return 0;
}
