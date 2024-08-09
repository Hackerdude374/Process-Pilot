import { NextApiRequest, NextApiResponse } from 'next';
import { runFCFS, runRR } from '../../lib/scheduler';

export default function handler(req: NextApiRequest, res: NextApiResponse) {
  if (req.method === 'POST') {
    const { algorithm, processes, timeQuantum } = req.body;

    const parsedProcesses = processes.trim().split('\n').map((line: string) => {
      const [id, arrivalTime, burstTime] = line.trim().split(/\s+/).map(Number);
      return { 
        id, 
        arrivalTime, 
        burstTime, 
        remainingTime: burstTime, 
        startTime: 0, 
        endTime: 0, 
        turnaroundTime: 0, 
        waitingTime: 0, 
        responseTime: 0 
      };
    });

    let result;
    if (algorithm === 'FCFS') {
      result = runFCFS(parsedProcesses);
    } else if (algorithm === 'RR') {
      result = runRR(parsedProcesses, timeQuantum);
    } else {
      return res.status(400).json({ error: 'Invalid algorithm' });
    }

    res.status(200).json(result);
  } else {
    res.setHeader('Allow', ['POST']);
    res.status(405).end(`Method ${req.method} Not Allowed`);
  }
}