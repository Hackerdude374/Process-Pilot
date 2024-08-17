import { useRouter } from 'next/router';
import { useEffect, useState } from 'react';

interface Process {
  id: number;
  arrivalTime: number;
  burstTime: number;
  startTime: number;
  endTime: number;
  turnaroundTime: number;
  waitingTime: number;
  responseTime: number;
}

interface SimulationResult {
  totalTime: number;
  avgTurnaroundTime: number;
  avgWaitingTime: number;
  avgResponseTime: number;
  cpuEfficiency: number;
  processes: Process[];
}

export default function Results() {
  const router = useRouter();
  const [results, setResults] = useState<SimulationResult | null>(null);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (router.query.results) {
      try {
        const parsedResults = JSON.parse(router.query.results as string);
        setResults(parsedResults);
      } catch (error) {
        console.error("Error parsing results:", error);
        setError("Failed to load simulation results. Please try again.");
      }
    }
  }, [router.query]);

  if (error) return <div className="text-red-500">{error}</div>;
  if (!results) return <div>Loading...</div>;

  const formatNumber = (value: number | undefined | null): string => 
    value !== undefined && value !== null ? value.toFixed(2) : 'N/A';

  return (
    <div className="container mx-auto p-4">
      <h1 className="text-2xl font-bold mb-4">Simulation Results</h1>
      <div className="mb-4 grid grid-cols-2 gap-4">
        <div className="bg-gray-100 p-4 rounded">
          <h2 className="font-semibold mb-2">Overall Statistics</h2>
          <p>Total Time: {results.totalTime ?? 'N/A'} time units</p>
          <p>CPU Efficiency: {formatNumber(results.cpuEfficiency)}%</p>
        </div>
        <div className="bg-gray-100 p-4 rounded">
          <h2 className="font-semibold mb-2">Average Times</h2>
          <p>Turnaround Time: {formatNumber(results.avgTurnaroundTime)} units</p>
          <p>Waiting Time: {formatNumber(results.avgWaitingTime)} units</p>
          <p>Response Time: {formatNumber(results.avgResponseTime)} units</p>
        </div>
      </div>
      <div className="overflow-x-auto">
        <table className="w-full border-collapse border">
          <thead>
            <tr className="bg-gray-200">
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
              <tr key={process.id} className="hover:bg-gray-50">
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
    </div>
  );
}