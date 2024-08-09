import { useState } from 'react';
import { useRouter } from 'next/router';
import { TextField, MenuItem, Button, Container, Typography, Box, CircularProgress } from '@mui/material';
import { motion } from 'framer-motion';
import { FaSpinner } from 'react-icons/fa';

export default function Home() {
  const [algorithm, setAlgorithm] = useState('FCFS');
  const [processes, setProcesses] = useState(`1 0 80
2 0 17
// (truncated for brevity)
50 258 37`);
  const [timeQuantum, setTimeQuantum] = useState('2');
  const [loading, setLoading] = useState(false); // Add loading state
  const router = useRouter();

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true); // Start loading
    const response = await fetch('/api/simulate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ algorithm, processes, timeQuantum: parseInt(timeQuantum) }),
    });
    const results = await response.json();
    setLoading(false); // End loading
    router.push({ pathname: '/results', query: { results: JSON.stringify(results) } });
  };

  const generateAIInput = async () => {
    setLoading(true); // Start loading
    const response = await fetch('/api/generate-input');
    const data = await response.json();
    setProcesses(data.processes);
    setLoading(false); // End loading
  };

  return (
    <Container maxWidth="md" className="p-8 mt-12">
      <motion.div
        initial={{ opacity: 0, y: -20 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ duration: 0.8, ease: 'easeOut' }}
        className="text-center"
      >
        <Typography variant="h3" className="font-bold mb-6 text-gray-800">
          CPU Scheduler Simulator
        </Typography>
      </motion.div>

      <motion.div
        initial={{ opacity: 0, y: -20 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ duration: 0.8, delay: 0.2, ease: 'easeOut' }}
        className="space-y-6"
      >
        <Box component="form" onSubmit={handleSubmit} className="space-y-6">
          <TextField
            select
            label="Algorithm"
            value={algorithm}
            onChange={(e) => setAlgorithm(e.target.value)}
            fullWidth
            variant="outlined"
            className="text-gray-800"
          >
            <MenuItem value="FCFS">First-Come, First-Served (FCFS)</MenuItem>
            <MenuItem value="RR">Round Robin (RR)</MenuItem>
          </TextField>

          {algorithm === 'RR' && (
            <TextField
              label="Time Quantum"
              type="number"
              value={timeQuantum}
              onChange={(e) => setTimeQuantum(e.target.value)}
              fullWidth
              variant="outlined"
              className="text-gray-800"
            />
          )}

          <TextField
            label="Processes"
            value={processes}
            onChange={(e) => setProcesses(e.target.value)}
            fullWidth
            multiline
            rows={10}
            variant="outlined"
            placeholder="Enter processes in format: ID ArrivalTime BurstTime"
            className="text-gray-800"
          />

          <motion.div whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }}>
            <Button
              variant="contained"
              onClick={generateAIInput}
              fullWidth
              className="bg-green-600 text-white hover:bg-green-500 transition-all"
            >
              Generate AI Input
            </Button>
          </motion.div>

          <motion.div whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }}>
            <Button
              type="submit"
              variant="contained"
              fullWidth
              className="bg-blue-600 text-white hover:bg-blue-500 transition-all"
            >
              {loading ? (
                <div className="flex items-center justify-center">
                  <FaSpinner className="animate-spin mr-2" />
                  Loading...
                </div>
              ) : (
                'Run Simulation'
              )}
            </Button>
          </motion.div>
        </Box>
      </motion.div>
    </Container>
  );
}
