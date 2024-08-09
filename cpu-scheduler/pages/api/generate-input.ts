import { NextApiRequest, NextApiResponse } from 'next';
import { GoogleGenerativeAI } from "@google/generative-ai";
import { PromptTemplate } from "@langchain/core/prompts";
const genAI = new GoogleGenerativeAI(process.env.GOOGLE_API_KEY as string);

export default async function handler(req: NextApiRequest, res: NextApiResponse) {
  try {
    const model = genAI.getGenerativeModel({ model: "gemini-pro" });

    const prompt = new PromptTemplate({
      template: "Generate {count} processes for CPU scheduling simulation. Each line should contain exactly 3 space-separated integers: Process ID, Arrival Time, and Burst Time. Process IDs should be sequential starting from 1. Arrival times should be non-decreasing. Burst times should be realistic for CPU processes (between 1 and 100 time units). The output should be exactly {count} lines, each in the format 'ID ArrivalTime BurstTime'.",
      inputVariables: ["count"],
    });

    const input = await prompt.format({ count: 50 });
    const result = await model.generateContent(input);
    const text = result.response.text();

    res.status(200).json({ processes: text.trim() });
  } catch (error) {
    console.error('Error generating input:', error);
    res.status(500).json({ error: 'Failed to generate input' });
  }
}