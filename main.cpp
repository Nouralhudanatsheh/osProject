
#include <algorithm>
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

  void ganttChartandDetails(vector<Process> processes) {

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
      if (i > 0 && processes[i].startTime != processes[i - 1].finishTime) {
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
  void processesDetails(float cpuTime, float idleTime) {

    cout << "\npid \t finish time \t waiting time \t turnaround time \t"
         << endl;
    for (int i = 0; i < processes.size(); i++) {
      cout << " " << processes[i].pid

           << "\t \t \t" << processes[i].finishTime << " \t \t \t \t"

           << processes[i].waitingTime <<

          " \t\t\t \t \t " << processes[i].turnanroundTime << "\t " << endl;
    }
    cout << "\nCPU utilization = ((" << cpuTime << " - " << idleTime << ") / "
         << cpuTime << ") * 100% = " << (cpuTime - idleTime) * 100 / cpuTime
         << "%\n"
         << endl;
  }
  void processesClear() {

    for (int i = 0; i < processes.size(); i++) {
      processes[i].finishTime = processes[i].turnanroundTime =
          processes[i].waitingTime = 0;
      processes[i].remainingBursts = processes[i].cpuBursts;
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
  int findCPid(Process currentProcess) {
    int index;
    for (index = 0; index < processes.size(); index++)
      if (processes[index].pid == currentProcess.pid)
        break;

    return index;
  }

public:
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
        p.cpuBursts = p.remainingBursts = cpuBursts;
        processes.push_back(p);
      }
      fin.close();
    }
  }

  void FCFS() {
    if (!processes.empty()) {
      processesClear();
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
        processes[i].remainingBursts = 0;
        cpuTime += processes[i].cpuBursts;
      }
      cout << "First Come First Served Algorithm\n";
      ganttChartandDetails(processes);
      processesDetails(cpuTime, idleTime);

    } else
      cout << "no processes found";
  }

  void SRT() {
    if (!processes.empty()) {
      processesClear();
      atSortingRT();

      float cpuTime = processes[0].startTime = processes[0].arrivalTime;
      float idleTime = processes[0].arrivalTime;
      Process currentProcess = processes[0];
      vector<Process> SRT;
      priority_queue<Process, vector<Process>, compareSRTrt> readyQ;
      int j = 1;
      int i = 0;
      int index = 0;
      while (true) {
        i = index;
        index = findCPid(currentProcess);
        if (i != index)
          currentProcess.startTime = cpuTime;

        if (j < processes.size()) {
          while (processes[j].arrivalTime != cpuTime) {
            if (currentProcess.remainingBursts > 0) {
              cpuTime++;
              currentProcess.remainingBursts--;
            } else
              break;
          }

          if (currentProcess.remainingBursts == 0) {
            currentProcess.remainingBursts = processes[index].remainingBursts =
                0;
            currentProcess.finishTime = processes[index].finishTime = cpuTime;
            processes[index].turnanroundTime =
                processes[index].finishTime - processes[index].arrivalTime;
            processes[index].waitingTime =
                processes[index].turnanroundTime - processes[index].cpuBursts;
            SRT.push_back(currentProcess);
          } else if (currentProcess.remainingBursts <=
                     processes[j].remainingBursts) {
            readyQ.push(processes[j++]);

            continue;
          } else {

            currentProcess.finishTime = cpuTime;
            SRT.push_back(currentProcess);
            readyQ.push(currentProcess);
            readyQ.push(processes[j++]);
          }

        } else {

          cpuTime += currentProcess.remainingBursts;
          currentProcess.remainingBursts = processes[index].remainingBursts = 0;
          currentProcess.finishTime = processes[index].finishTime = cpuTime;
          processes[index].turnanroundTime =
              processes[index].finishTime - processes[index].arrivalTime;
          processes[index].waitingTime =
              processes[index].turnanroundTime - processes[index].cpuBursts;
          SRT.push_back(currentProcess);
        }

        if (j == processes.size() && readyQ.empty())
          break;

        if (readyQ.empty() && (j < processes.size())) {
          currentProcess = processes[j++];
          idleTime += currentProcess.arrivalTime - cpuTime;
          cpuTime += currentProcess.arrivalTime - cpuTime;
        } else if (!readyQ.empty()) {
          if (currentProcess.pid != readyQ.top().pid) {
            if ((j < processes.size()) &&
                (cpuTime + contextSwitch) > processes[j].arrivalTime)
              readyQ.push(processes[j++]);
            idleTime += contextSwitch;
            cpuTime += contextSwitch;
          }
          currentProcess = readyQ.top();
          readyQ.pop();
        }
      }

      cout << "\nShortest Remaining Time Served Algorithm\n";
      ganttChartandDetails(SRT);
      processesDetails(cpuTime, idleTime);
      SRT.clear();

    }

    else
      cout << "no processes found";
  }

  void RR() {
    if (!processes.empty()) {
      processesClear();
      atSorting();
      int i = 0;
      Process currentProcess = processes[0];
      vector<Process> RR;
      queue<Process> readyQ;
      float cpuTime = processes[0].arrivalTime;
      float idleTime = processes[0].arrivalTime;
      while (true) {
        currentProcess.startTime = cpuTime;
        int index = findCPid(currentProcess);
        if (currentProcess.remainingBursts > quantum) {
          cpuTime += quantum;
          currentProcess.finishTime = cpuTime;
          currentProcess.remainingBursts -= quantum;
          readyQ.push(currentProcess);
        } else {

          cpuTime += currentProcess.remainingBursts;
          currentProcess.remainingBursts = processes[index].remainingBursts = 0;
          currentProcess.finishTime = processes[index].finishTime = cpuTime;

          processes[index].turnanroundTime =
              processes[index].finishTime - processes[index].arrivalTime;
          processes[index].waitingTime =
              processes[index].turnanroundTime - processes[index].cpuBursts;
        }

        RR.push_back(currentProcess);

        if ((i == processes.size() - 1) && readyQ.empty())
          break;

        if ((i + 1 < processes.size()) &&
            processes[i + 1].arrivalTime <= cpuTime) {
          currentProcess = processes[++i];
          idleTime += contextSwitch;
          cpuTime += contextSwitch;

        } else if (readyQ.empty()) {
          currentProcess = processes[++i];
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
      cout << "\nRound Robin Algorithm\n";

      ganttChartandDetails(RR);
      processesDetails(cpuTime, idleTime);
      RR.clear();

    } else
      cout << "no processes found";
  }
};

int main() {
  schedulingAlg test;
  test.FCFS();
  test.SRT();
  test.RR();
  return 0;
}
