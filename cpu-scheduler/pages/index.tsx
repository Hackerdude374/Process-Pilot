import { useState } from 'react';
import { useRouter } from 'next/router';

export default function Home() {
  const [algorithm, setAlgorithm] = useState('FCFS');
  const [processes, setProcesses] = useState(`1 0 80
2 0 17
3 1 40
4 3 29
5 4 95
6 4 48
7 4 22
8 5 24
9 6 20
10 7 60
11 8 42
12 8 23
13 9 103
14 10 23
15 16 95
16 17 105
17 19 41
18 20 35
19 20 37
20 23 5
21 23 50
22 23 12
23 33 47
24 34 6
25 35 91
26 38 68
27 47 4
28 41 17
29 44 152
30 44 63
31 47 77
32 48 14
33 51 79
34 53 43
35 53 13
36 57 97
37 61 44
38 64 76
39 68 37
40 68 87
41 71 18
42 73 21
43 75 42
44 78 11
45 100 31
46 107 55
47 116 41
48 117 18
49 216 26
50 258 37`);
  const [timeQuantum, setTimeQuantum] = useState('2');
  const router = useRouter();

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    const response = await fetch('/api/simulate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ algorithm, processes, timeQuantum: parseInt(timeQuantum) }),
    });
    const results = await response.json();
    router.push({ pathname: '/results', query: { results: JSON.stringify(results) } });
  };

  const generateAIInput = async () => {
    const response = await fetch('/api/generate-input');
    const data = await response.json();
    setProcesses(data.processes);
  };

  return (
    <div className="container mx-auto p-4">
      <h1 className="text-2xl font-bold mb-4">CPU Scheduler Simulator</h1>
      <form onSubmit={handleSubmit} className="space-y-4">
        <div>
          <label className="block mb-2">Algorithm</label>
          <select
            value={algorithm}
            onChange={(e) => setAlgorithm(e.target.value)}
            className="w-full p-2 border rounded"
          >
            <option value="FCFS">First-Come, First-Served (FCFS)</option>
            <option value="RR">Round Robin (RR)</option>
          </select>
        </div>
        {algorithm === 'RR' && (
          <div>
            <label className="block mb-2">Time Quantum</label>
            <input
              type="number"
              value={timeQuantum}
              onChange={(e) => setTimeQuantum(e.target.value)}
              className="w-full p-2 border rounded"
            />
          </div>
        )}
        <div>
          <label className="block mb-2">Processes</label>
          <textarea
            value={processes}
            onChange={(e) => setProcesses(e.target.value)}
            className="w-full p-2 border rounded"
            rows={10}
            placeholder="Enter processes in format: ID ArrivalTime BurstTime"
          />
        </div>
        <button
          type="button"
          onClick={generateAIInput}
          className="bg-green-500 text-white px-4 py-2 rounded"
        >
          Generate AI Input
        </button>
        <button
          type="submit"
          className="bg-blue-500 text-white px-4 py-2 rounded"
        >
          Run Simulation
        </button>
      </form>
    </div>
  );
}