# 📌 Overview

This project implements the K-Means Clustering algorithm in both serial and parallel (using Cilk++) forms. The goal is to evaluate how parallelization impacts performance across varying dataset sizes and cluster counts and finally to apply it on a real-world dataset i.e. image for image compression.

## Key features include:

- Synthetic dataset generation in a 1024x1024 space
- Serial and Cilk++-based parallel implementation
- Early stopping based on centroid convergence
- Experimental setup to evaluate speedup
- Conversion of image into pixels and vice-versa
- Visualization of clustering results
- Visualization of original and compressed images

# 📁 Project Structure

```sh
project-root/
│
├── src/
│   ├── data/                   # Contains generated CSV files
│   ├── images/                 # Contains original & generated images
│       ├── original.jpg        # Original img used for compresion
│   ├── k_means.cpp             # Main implementation
│   ├── image_compression.cpp   # Main implementation
│   └── visualise.ipynb         # Jupyter notebook for plotting results
│
├── bin/                        # Auto-created for compiled binaries
│
├── Makefile                    # Compile, run, clean commands
└── README.md
```

# 🧪 Experimental Setup

## Experiment 1

The algorithm is tested with different combinations of:

- Number of data points: 10,000 to 1,000,000
- Number of clusters k: 5, 10, 50, 100

Each run records:

- Execution time for serial and parallel versions
- Speedup achieved
- Output saved for visualization

## Experiment 2

The algorithm is also used for image compression by

- converting it into RGB pixel data
- inputing the pixel values into both serial and parallel K-means
- saving the output pixel values along with the cluster labels for both serial and parallel versions for visualization.

# 🚀 Usage Instructions

Make sure you have clang++ with OpenCilk support installed.

## 🔧 Compile

```sh
make compile
```

This will:

- Create a `bin/ `directory if it doesn't exist
- Compiles both `src/k_means.cpp` and `src/image_compression.cpp` with optimizations and OpenCilk flags
- Output the binary files to `bin/` directory

## ▶️ Run

### Experiment 1

```sh
make run
```

This runs the compiled binary `bin/k_means`. It executes both serial and parallel K-means for different data sizes and cluster counts, and prints the results in a tabular format like:

| Num Points | K   | Serial Time | Parallel Time | Speedup |
| ---------- | --- | ----------- | ------------- | ------- |
| 10000      | 5   | 0.0105      | 0.0069        | 1.53    |

### Experiment 2

```sh
make run-ic
```

> ⚠️ **IMPORTANT:** Before running this command, convert the image - `src/images/original.jpg` into a CSV data file using the `visualise.ipynb` notebook.

This runs the compiled binary `bin/image_compression`. It reads the `data/image_data.csv` and executes both serial and parallel K-means clustering for the image data and produces two files - `data/compressed_parallel.csv` and `data/compressed_serial.csv` both of which contain the image pixels and cluster label for each pixel. It also prints the time taken by both serial and parallel K-means along with the speed-up.

This data can be used in `visualize.ipynb` to visualise the compressed images.

## 🧹 Clean

```sh
make clean
```

This removes the `bin/` folder with all compiled files along with the generated images and data files.

# 📊 Output and Visualization

After the execution of `bin/k_means`, the console displays a formatted table of timings and speedup. It also produces two CSV files in `src/data` - `serial_clusters.csv` and `parallel_clusters.csv`.

After the execution of `bin/image_compression`, it will generate two CSV files in `src/data` directory - `compression_parallel.csv` and `compression_serial.csv`, both containing compressed image pixels along with cluster labels.

You can use `src/visualise.ipynb` to

- visualize the clustering using the CSV files `serial_clusters.csv` and `parallel_clusters.csv`
- convert compressed image CSV files (`compression_parallel.csv` and `compression_serial.csv`) to images
- view the compressed images side-by-side with the original image

📌 Dependencies

1. C++ Compiler: clang++ with OpenCilk (-fopencilk flag)
2. Python (for visualization): matplotlib, pandas, numpy (install via pip install if needed)

# ✅ Future Work

1. Optimize thread-level parallelism further using fine-grained locks or atomic operations
2. Explore GPU-based parallelism (e.g., using CUDA)
3. Apply to real-world datasets (e.g., customer segmentation, document clustering)
