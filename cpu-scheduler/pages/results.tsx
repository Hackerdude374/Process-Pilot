import { useRouter } from 'next/router';
import { useEffect, useState } from 'react';

interface SimulationResult {
  totalTime: number;
  avgTurnaroundTime: number;
  avgWaitingTime: number;
  avgResponseTime: number;
  cpuEfficiency: number;
  processes: Array<{
    id: number;
    arrivalTime: number;
    burstTime: number;
    startTime: number;
    endTime: number;
    turnaroundTime: number;
    waitingTime: number;
    responseTime: number;
  }>;
}

export default function Results() {
  const router = useRouter();
  const [results, setResults] = useState<SimulationResult | null>(null);

  useEffect(() => {
    if (router.query.results) {
      setResults(JSON.parse(router.query.results as string));
    }
  }, [router.query]);

  if (!results) return <div>Loading...</div>;

  return (
    <div className="container mx-auto p-4">
      <h1 className="text-2xl font-bold mb-4">Simulation Results</h1>
      <div className="mb-4">
        <p>Total Time: {results.totalTime} time units</p>
        <p>Average Turnaround Time: {results.avgTurnaroundTime.toFixed(2)} time units</p>
        <p>Average Waiting Time: {results.avgWaitingTime.toFixed(2)} time units</p>
        <p>Average Response Time: {results.avgResponseTime.toFixed(2)} time units</p>
        <p>CPU Efficiency: {results.cpuEfficiency.toFixed(2)}%</p>
      </div>
      <table className="w-full border-collapse border">
        <thead>
          <tr>
            <th className="border p-2">Process ID</th>
            <th className="border p-2">Arrival Time</th>
            <th className="border p-2">Burst Time</th>
            <th className="border p-2">Start Time</th>
            <th className="border p-2">End Time</th>
            <th className="border p-2">Turnaround Time</th>
            <th className="border p-2">Waiting Time</th>
            <th className="border p-2">Response Time</th>
          </tr>
        </thead>
        <tbody>
          {results.processes.map((process) => (
            <tr key={process.id}>
              <td className="border p-2">{process.id}</td>
              <td className="border p-2">{process.arrivalTime}</td>
              <td className="border p-2">{process.burstTime}</td>
              <td className="border p-2">{process.startTime}</td>
              <td className="border p-2">{process.endTime}</td>
              <td className="border p-2">{process.turnaroundTime}</td>
              <td className="border p-2">{process.waitingTime}</td>
              <td className="border p-2">{process.responseTime}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}