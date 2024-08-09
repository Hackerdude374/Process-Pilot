import { useState } from 'react';
import { useRouter } from 'next/router';
import { TextField, MenuItem, Button, Container, Typography, Box, IconButton } from '@mui/material';
import { motion } from 'framer-motion';
import { FaSpinner, FaMoon, FaSun } from 'react-icons/fa';

export default function Home() {
  const [algorithm, setAlgorithm] = useState('FCFS');
  const [processes, setProcesses] = useState(`1 0 80
2 0 17
// (truncated for brevity)
50 258 37`);
  const [timeQuantum, setTimeQuantum] = useState('2');
  const [loading, setLoading] = useState(false);
  const [darkMode, setDarkMode] = useState(false); // Dark mode state
  const router = useRouter();

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    const response = await fetch('/api/simulate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ algorithm, processes, timeQuantum: parseInt(timeQuantum) }),
    });
    const results = await response.json();
    setLoading(false);
    router.push({ pathname: '/results', query: { results: JSON.stringify(results) } });
  };

  const generateAIInput = async () => {
    setLoading(true);
    const response = await fetch('/api/generate-input');
    const data = await response.json();
    setProcesses(data.processes);
    setLoading(false);
  };

  return (
    <div className={`${darkMode ? 'dark' : ''}`}>
      {/* Navbar */}
      <div className="bg-retroPurple p-4">
        <div className="container mx-auto flex justify-between items-center">
          <Typography variant="h4" className="text-white font-bold">
            ProcessPilot
          </Typography>
          <IconButton onClick={() => setDarkMode(!darkMode)} className="text-white">
            {darkMode ? <FaSun /> : <FaMoon />}
          </IconButton>
        </div>
      </div>

      <Container maxWidth="md" className="p-8 mt-12">
        <motion.div
          initial={{ opacity: 0, y: -20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, ease: 'easeOut' }}
          className="text-center"
        >
          <Typography variant="h3" className="font-bold mb-6 text-gray-200">
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
              className="bg-gray-800 text-gray-200"
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
                className="bg-gray-800 text-gray-200"
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
              className="bg-gray-800 text-gray-200"
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
    </div>
  );
}
