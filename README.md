# 📦 Smart Warehouse Inbound Processing System

A command-line data processing pipeline built in C for the TU Dublin Algorithms & Design module (CMPU1001). The system ingests raw warehouse inbound records from four logistics teams, sorts and merges them into a single time-ordered dispatch list, and provides search and reporting tools — all meeting strict Big-O complexity requirements.

---

## 🧩 Problem Overview

A warehouse receives daily container deliveries from four logistics partners, each processed by a separate team using handheld scanners. The resulting data files are unsorted. This tool preprocesses all four files into a single organised dispatch list ready for querying and reporting.

Each record contains:
- Container ID
- Timestamp (HH:MM:SS)
- Weight (kg)
- Product code
- Temperature at arrival
- Courier ID

---

## ⚙️ Tasks & Algorithms

| Task | Goal | Algorithm | Complexity |
|---|---|---|---|
| 1 | Sort each team's file by timestamp | Merge Sort | O(N log N) |
| 2 | Merge all 4 sorted files into one dispatch list | K-Way Heap Merge | O(N) |
| 3 | Search dispatch list by time and product code | Binary Search | O(log N) |
| 4 | Count products per container with averages | Single-pass HashMap | O(N) |

---

## 🗂️ How It Works

**Task 1 — Merge Sort**
Timestamps are converted to seconds (`HH×3600 + MM×60 + SS`) for easy integer comparison. Each of the four container files is independently sorted using recursive merge sort with a temporary auxiliary array.

**Task 2 — K-Way Heap Merge**
A min-heap of size 4 holds the current front record from each sorted file. The smallest is popped and written to the dispatch list, then the next record from that file is pushed in. This produces a fully merged time-ordered list in a single pass without re-sorting.

**Task 3 — Binary Search**
A parallel key array of integer timestamps is built from the dispatch list. A left-boundary binary search finds the first record at or after the target time, then scans forward for a matching product code if one is specified.

**Task 4 — Container Report**
A single pass through the dispatch list accumulates count, total weight, total temperature, and a bitmask of courier IDs per container. Averages are calculated at the end. O(1) per record × N records = O(N).

---

## 🚀 Running the Project

```bash
gcc -o warehouse demoAlg.c
./warehouse
```

Place `Testdata Algorithms.csv` in the same directory before running. The program steps through each task interactively, pausing between stages.

---

## 📊 Test Data

Test data was generated in Excel using `RAND()` and `RANDBETWEEN()` with a formula to produce random timestamps between 08:00 and 17:00. Each container has over 10 records with varied weights, temperatures, product codes, and courier IDs.

---

## 📄 Data Structure

```c
typedef struct {
    int   container_id;
    int   HH, MM, SS;
    float weight;
    int   product_code;
    int   temperature;
    int   courier_id;
} Record;
```
