
#include <algorithm>
#include <cstddef>
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

        if (processes[i].arrivalTime > cpuTime &&
            processes[i].arrivalTime >= cpuTime + contextSwitch) {
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
      const int n = processes.size();
      int arr[n];
      for (int i = 0; i < n; i++)
        arr[i] = ceil(processes[i].cpuBursts / quantum);

      for (int i = 0; i < processes.size(); i++)
        processes[i].remainingBursts = processes[i].cpuBursts;
      // Process *currentProcess = &processes[0];
      float cpuTime = processes[0].arrivalTime;
      float idleTime = processes[0].arrivalTime;
      vector<Process> RR;

      Process currentProcess = processes[0];
      int tempST;
      int flag = 0;
      int j;
      for (int i = 0; flag < n; i++) {
        currentProcess.startTime = cpuTime;

        if (currentProcess.arrivalTime > cpuTime &&
            currentProcess.arrivalTime >= cpuTime + contextSwitch) {
          idleTime += currentProcess.arrivalTime - cpuTime;
          cpuTime += currentProcess.arrivalTime - cpuTime;
        } else {
          cpuTime += contextSwitch;
          idleTime += contextSwitch;
        }

        if (currentProcess.remainingBursts < quantum)
          cpuTime += currentProcess.remainingBursts;
        else
          cpuTime += quantum;
        currentProcess.finishTime = cpuTime;

        if (arr[i] == ceil(processes[i].cpuBursts / quantum)) {
          processes[i].startTime = currentProcess.startTime;
        }
        if (arr[i] == 1) {
          processes[i].finishTime = currentProcess.finishTime;
          processes[i].turnanroundTime =
              processes[i].finishTime - processes[i].startTime;
          processes[i].remainingBursts = 0;
        } else
          processes[i].remainingBursts -= quantum;

        RR.push_back(currentProcess);
        arr[i]--;

        i = (i + 1) % n;
        currentProcess = processes[i];
        for (j = 0; j < n; j++) {
          if (arr[j] == 0)
            flag++;
        }
      }

      // for (int i = 0; flag != n - 1;) {
      //   flag = 0;
      //   if (arr[i] > 0) {
      //     if (ceil(processes[i].cpuBursts / quantum) == arr[i]) {
      //       (*currentProcess).startTime = tempST = cpuTime;
      //       (*currentProcess).waitingTime +=
      //           (*currentProcess).startTime - (*currentProcess).arrivalTime;
      //     } else {
      //       tempST = cpuTime;
      //       (*currentProcess).waitingTime +=
      //           (*currentProcess).startTime - (*currentProcess).finishTime;
      //     }

      //     if ((*currentProcess).remainingBursts < quantum)
      //       cpuTime += (*currentProcess).remainingBursts;
      //     else
      //       cpuTime += quantum;
      //     (*currentProcess).finishTime = cpuTime;
      //     if (arr[i] == 1)
      //       (*currentProcess).turnanroundTime +=
      //           (*currentProcess).finishTime - (*currentProcess).arrivalTime;
      //     if ((*currentProcess).remainingBursts >= quantum)
      //       (*currentProcess).remainingBursts -= quantum;
      //     else
      //       (*currentProcess).remainingBursts = 0;
      //     RR.push_back(*currentProcess);
      //     arr[i]--;
      //   }
      //   if (1 + currentProcess != NULL)
      //     ++currentProcess;
      //   else
      //     currentProcess = &processes[0];
      //   if (i < n)
      //     i++;
      //   else
      //     i = 0;

      //   for (int j = 0; j < n; j++) {
      //     if (arr[j] == 0)
      //       flag++;
      //   }
      // }

      ganttChartandDetails(RR, cpuTime, idleTime);
      processesDetails(processes, cpuTime, idleTime);

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
