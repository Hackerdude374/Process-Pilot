interface Process {
    id: number;
    arrivalTime: number;
    burstTime: number;
    startTime: number;
    endTime: number;
    turnaroundTime: number;
    waitingTime: number;
    responseTime: number;
    remainingTime: number;
  }
  
  interface SimulationResult {
    totalTime: number;
    avgTurnaroundTime: number;
    avgWaitingTime: number;
    avgResponseTime: number;
    cpuEfficiency: number;
    processes: Process[];
  }
  
  const SWITCH_TIME = 2;
  
  function calculateMetrics(processes: Process[]): SimulationResult {
    let totalExecutionTime = 0;
    let totalTurnaroundTime = 0;
    let totalWaitingTime = 0;
    let totalResponseTime = 0;
  
    processes.forEach(process => {
      process.turnaroundTime = process.endTime - process.arrivalTime;
      process.waitingTime = process.turnaroundTime - process.burstTime;
      process.responseTime = process.startTime - process.arrivalTime;
  
      totalExecutionTime += process.burstTime;
      totalTurnaroundTime += process.turnaroundTime;
      totalWaitingTime += process.waitingTime;
      totalResponseTime += process.responseTime;
    });
  
    const totalTime = processes[processes.length - 1].endTime;
    const totalSwitchTime = SWITCH_TIME * processes.length;
  
    return {
      totalTime,
      avgTurnaroundTime: totalTurnaroundTime / processes.length,
      avgWaitingTime: totalWaitingTime / processes.length,
      avgResponseTime: totalResponseTime / processes.length,
      cpuEfficiency: (totalExecutionTime / (totalTime + totalSwitchTime)) * 100,
      processes
    };
  }
  
  export function runFCFS(inputProcesses: Process[]): SimulationResult {
    let currentTime = 0;
    const processes = inputProcesses.sort((a, b) => a.arrivalTime - b.arrivalTime);
  
    processes.forEach(process => {
      if (currentTime < process.arrivalTime) {
        currentTime = process.arrivalTime;
      }
      process.startTime = currentTime;
      process.endTime = currentTime + process.burstTime;
      currentTime = process.endTime;
    });
  
    return calculateMetrics(processes);
  }
  
  export function runRR(inputProcesses: Process[], timeQuantum: number): SimulationResult {
    let currentTime = 0;
    const processes = inputProcesses.sort((a, b) => a.arrivalTime - b.arrivalTime);
    const queue: Process[] = [];
    let completed = 0;
  
    while (completed < processes.length) {
      processes.forEach(process => {
        if (process.arrivalTime <= currentTime && process.remainingTime > 0 && !queue.includes(process)) {
          queue.push(process);
        }
      });
  
      if (queue.length === 0) {
        currentTime++;
        continue;
      }
  
      const currentProcess = queue.shift()!;
      if (currentProcess.remainingTime === currentProcess.burstTime) {
        currentProcess.startTime = currentTime;
      }
  
      if (currentProcess.remainingTime <= timeQuantum) {
        currentTime += currentProcess.remainingTime;
        currentProcess.remainingTime = 0;
        currentProcess.endTime = currentTime;
        completed++;
      } else {
        currentTime += timeQuantum;
        currentProcess.remainingTime -= timeQuantum;
        queue.push(currentProcess);
      }
    }
  
    return calculateMetrics(processes);
  }