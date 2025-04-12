# 📌 Overview

This project implements the K-Means Clustering algorithm in both serial and parallel (using Cilk++) forms. The goal is to evaluate how parallelization impacts performance across varying dataset sizes and cluster counts.

## Key features include:

- Synthetic dataset generation in a 1024x1024 space
- Serial and Cilk++-based parallel implementation
- Early stopping based on centroid convergence
- Experimental setup to evaluate speedup
- Python-based visualization of clustering results

# 📁 Project Structure

```sh
project-root/
│
├── src/
│   ├── k_means.cpp          # Main implementation
│   └── visualise.ipynb      # Jupyter notebook for plotting results
│
├── bin/                     # Auto-created for compiled binaries
│
├── Makefile                 # Compile, run, clean commands
└── README.md
```

# 🧪 Experimental Setup

The algorithm is tested with different combinations of:

- Number of data points: 10,000 to 1,000,000
- Number of clusters k: 5, 10, 50, 100

Each run records:

- Execution time for serial and parallel versions
- Speedup achieved
- Output saved for visualization

# 🚀 Usage Instructions

Make sure you have clang++ with OpenCilk support installed.

## 🔧 Compile

```sh
make compile
```

This will:

- Create a bin/ directory if it doesn't exist
- Compile the code with optimizations and OpenCilk flags
- Output the binary to bin/k_means

## ▶️ Run

```sh
make run
```

This runs the compiled binary. It executes both serial and parallel K-means for different data sizes and cluster counts, and prints the results in a tabular format like:

| Num Points | K   | Serial Time | Parallel Time | Speedup |
| ---------- | --- | ----------- | ------------- | ------- |
| 10000      | 5   | 0.0105      | 0.0069        | 1.53    |

## 🧹 Clean

```sh
make clean
```

This removes the bin/ folder and all compiled files.

# 📊 Output and Visualization

After execution, the console displays a formatted table of timings and speedup. You can use `src/visualise.ipynb` to visualize the clustering.

📌 Dependencies

1. C++ Compiler: clang++ with OpenCilk (-fopencilk flag)
2. Python (for visualization): matplotlib, pandas, numpy (install via pip install if needed)

# ✅ Future Work

1. Optimize thread-level parallelism further using fine-grained locks or atomic operations
2. Explore GPU-based parallelism (e.g., using CUDA)
3. Apply to real-world datasets (e.g., image compression, customer segmentation)
