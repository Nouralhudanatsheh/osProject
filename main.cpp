
#include <algorithm>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
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
  float contextSwitch;
  float quantum;
  vector<Process> processes;
  struct compareFCFS {
    bool operator()(Process p1, Process p2) {

      return p1.arrivalTime > p2.arrivalTime;
    }
  };
  struct compareSRTat {
    bool operator()(Process p1, Process p2) {
      if (p1.arrivalTime == p2.arrivalTime)
        return p1.cpuBursts > p2.cpuBursts;
      return p1.arrivalTime > p2.arrivalTime;
    }
  };
  struct compareSRTrt {
    bool operator()(Process p1, Process p2) {
      return p1.remainingBursts > p2.remainingBursts;
    }
  };
  struct findPID {
    int id;
    findPID(int id) : id(id) {}
    bool operator()(const Process &P) const { return P.pid == id; }
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
  }
  void processesDetails(vector<Process> processes, float cpuTime,
                        float idleTime) {
    cout << "\nCPU utilization = ((" << cpuTime << " - " << idleTime << ") / "
         << cpuTime << ") * 100% = " << (cpuTime - idleTime) * 100 / cpuTime
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

  void atSorting() {
    priority_queue<Process, vector<Process>, compareFCFS> FCFS;
    for (int i = 0; i < processes.size(); i++) {
      FCFS.push(processes[i]);
    }
    processes.clear();
    for (int i = 0; !FCFS.empty(); i++) {
      processes.push_back(FCFS.top());
      FCFS.pop();
    }
  }
  void atSortingRT() {
    priority_queue<Process, vector<Process>, compareSRTat> FCFS;
    for (int i = 0; i < processes.size(); i++) {
      FCFS.push(processes[i]);
    }
    processes.clear();
    for (int i = 0; !FCFS.empty(); i++) {
      processes.push_back(FCFS.top());
      FCFS.pop();
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

      atSorting();
      float cpuTime = processes[0].finishTime =
          processes[0].arrivalTime + processes[0].cpuBursts;
      float idleTime = processes[0].arrivalTime;
      processes[0].startTime = processes[0].arrivalTime;
      processes[0].turnanroundTime = cpuTime - processes[0].arrivalTime;

      for (int i = 1; i < processes.size(); i++) {

        if (processes[i].arrivalTime > cpuTime) {
          idleTime += processes[i].arrivalTime - cpuTime;
          cpuTime += processes[i].arrivalTime - cpuTime;
        } else {
          cpuTime += contextSwitch;
          idleTime += contextSwitch;
        }

        processes[i].startTime = cpuTime;
        processes[i].finishTime =
            processes[i].startTime + processes[i].cpuBursts;
        processes[i].turnanroundTime =
            processes[i].finishTime - processes[i].arrivalTime;
        processes[i].waitingTime =
            processes[i].startTime - processes[i].arrivalTime;
        cpuTime += processes[i].cpuBursts;
      }
      cout << "First Come First Served Algorithm\n\n";
      ganttChartandDetails(processes, cpuTime, idleTime);
    } else
      cout << "no processes found";
  }

  void SRT() {
    if (!processes.empty()) {
      atSortingRT();

      for (int i = 0; i < processes.size(); i++)
        processes[i].remainingBursts = processes[i].cpuBursts;

      priority_queue<Process, vector<Process>, compareSRTat> SRTQ;

      int cpuTime = processes[0].arrivalTime;
      int idleTime = processes[0].arrivalTime;
      Process currentProcess = processes[0];
      vector<Process> SRTV;
      SRTV.push_back(processes[0]);
      vector<Process>::iterator it = processes.begin();

      cout << (*(5 + it)).arrivalTime << endl;
      cout << (*it).arrivalTime;
      // while (true) {

      //   if (cpuTime == (*it++).arrivalTime &&
      //       SRTQ.top().remainingBursts < currentProcess.remainingBursts)

      //   {
      //     currentProcess.finishTime = cpuTime;
      //     currentProcess.waitingTime =
      //         currentProcess.startTime - currentProcess.finishTime;
      //     SRTV.push_back(currentProcess);
      //     SRTQ.push(currentProcess);
      //     cpuTime += contextSwitch;
      //     idleTime += contextSwitch;
      //     currentProcess = SRTQ.top();
      //     currentProcess.startTime = cpuTime;
      //     SRTQ.pop();
      //   } else if (currentProcess.remainingBursts) {
      //     currentProcess.remainingBursts--;
      //     cpuTime++;
      //   } else if (!SRTQ.empty())
      //     currentProcess = SRTQ.top();
      //   else
      //     break;
      //   ;
      // }

    }

    else
      cout << "no processes found";
  }
  void RR() {
    if (!processes.empty()) {
      atSorting();
      for (int i = 0; i < processes.size(); i++)
        processes[i].remainingBursts = processes[i].cpuBursts;

      Process *vectorProcess = &processes[0];
      std::vector<Process>::iterator it;
      Process currentProcess = processes[0];
      vector<Process> RR;
      queue<Process> readyQ;
      float cpuTime = processes[0].arrivalTime;
      float idleTime = processes[0].arrivalTime;
      while (true) {
        currentProcess.startTime = cpuTime;
        if (currentProcess.remainingBursts > quantum) {
          cpuTime += quantum;
          currentProcess.finishTime = cpuTime;
          currentProcess.remainingBursts -= quantum;
          readyQ.push(currentProcess);
        } else {
          it = std::find_if(processes.begin(), processes.end(),
                            findPID(currentProcess.pid));
          int id = (*it).pid;
          cpuTime += currentProcess.remainingBursts;
          currentProcess.remainingBursts = processes[id].remainingBursts = 0;
          currentProcess.finishTime = processes[id].finishTime = cpuTime;

          processes[id].turnanroundTime =
              processes[id].finishTime - processes[id].arrivalTime;
        }

        RR.push_back(currentProcess);

        if ((vectorProcess == &processes.back()) && readyQ.empty())
          break;

        if ((vectorProcess != &processes.back()) &&
            (*(1 + vectorProcess)).arrivalTime <= cpuTime) {
          idleTime += contextSwitch;
          cpuTime += contextSwitch;
          currentProcess = *(++vectorProcess);

        } else if (readyQ.empty()) {
          currentProcess = *(++vectorProcess);
          idleTime += currentProcess.arrivalTime - cpuTime;
          cpuTime += currentProcess.arrivalTime - cpuTime;

        } else {
          if (currentProcess.pid != readyQ.front().pid) {
            idleTime += contextSwitch;
            cpuTime += contextSwitch;
          }
          currentProcess = readyQ.front();

          readyQ.pop();
        }
      }

      ganttChartandDetails(RR, cpuTime, idleTime);
    } else
      cout << "no processes found";
  }
}

;

int main() {
  schedulingAlg test;
  test.RR();

  return 0;
}
