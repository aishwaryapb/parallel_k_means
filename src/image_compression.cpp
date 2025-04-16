#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <cilk/cilk.h>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;

struct Pixel
{
    double r, g, b;
};

// Read image pixel data from CSV
vector<Pixel> readImageCSV(const string &filename)
{
    vector<Pixel> data;
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header
    while (getline(file, line))
    {
        stringstream ss(line);
        string token;
        Pixel p;
        getline(ss, token, ',');
        p.r = stod(token);
        getline(ss, token, ',');
        p.g = stod(token);
        getline(ss, token, ',');
        p.b = stod(token);
        data.push_back(p);
    }
    return data;
}

// Save clustered pixels and centroids to CSV
void saveCompressedCSV(const vector<Pixel> &pixels, const vector<int> &labels, const vector<Pixel> &centroids, const string &filename)
{
    ofstream file(filename);
    file << "r,g,b,label\n";
    for (size_t i = 0; i < pixels.size(); ++i)
    {
        Pixel c = centroids[labels[i]];
        file << c.r << "," << c.g << "," << c.b << "," << labels[i] << "\n";
    }
    file.close();
}

// Euclidean distance
double distance(Pixel a, Pixel b)
{
    return sqrt((a.r - b.r) * (a.r - b.r) + (a.g - b.g) * (a.g - b.g) + (a.b - b.b) * (a.b - b.b));
}

// Serial K-means
vector<int> kmeansSerial(vector<Pixel> &data, vector<Pixel> &centroids, int k, int max_iters = 100, double tolerance = 1e-4)
{
    int n = data.size();
    vector<int> labels(n, 0);

    for (int iter = 0; iter < max_iters; ++iter)
    {
        vector<int> counts(k, 0);
        vector<Pixel> newCentroids(k, {0, 0, 0});

        // Assign to nearest centroid
        for (int i = 0; i < n; ++i)
        {
            double minDist = numeric_limits<double>::max();
            for (int j = 0; j < k; ++j)
            {
                double dist = distance(data[i], centroids[j]);
                if (dist < minDist)
                {
                    minDist = dist;
                    labels[i] = j;
                }
            }
            counts[labels[i]]++;
            newCentroids[labels[i]].r += data[i].r;
            newCentroids[labels[i]].g += data[i].g;
            newCentroids[labels[i]].b += data[i].b;
        }

        // Update centroids
        double maxChange = 0;
        for (int j = 0; j < k; ++j)
        {
            if (counts[j] > 0)
            {
                newCentroids[j].r /= counts[j];
                newCentroids[j].g /= counts[j];
                newCentroids[j].b /= counts[j];
            }
            double change = distance(centroids[j], newCentroids[j]);
            maxChange = max(maxChange, change);
            centroids[j] = newCentroids[j];
        }

        if (maxChange < tolerance)
            break;
    }
    return labels;
}

// Parallel K-means using Cilk
vector<int> kmeansParallel(vector<Pixel> &data, vector<Pixel> &centroids, int k, int max_iters = 100, double tolerance = 1e-4)
{
    int n = data.size();
    vector<int> labels(n, 0);

    for (int iter = 0; iter < max_iters; ++iter)
    {
        vector<int> counts(k, 0);
        vector<Pixel> newCentroids(k, {0, 0, 0});

        // Assign to nearest centroid
        cilk_for(int i = 0; i < n; ++i)
        {
            double minDist = numeric_limits<double>::max();
            for (int j = 0; j < k; ++j)
            {
                double dist = distance(data[i], centroids[j]);
                if (dist < minDist)
                {
                    minDist = dist;
                    labels[i] = j;
                }
            }
        }

        // Recalculate centroids
        cilk_for(int j = 0; j < k; ++j)
        {
            double sumR = 0, sumG = 0, sumB = 0;
            int count = 0;
            for (int i = 0; i < n; ++i)
            {
                if (labels[i] == j)
                {
                    sumR += data[i].r;
                    sumG += data[i].g;
                    sumB += data[i].b;
                    count++;
                }
            }
            if (count > 0)
            {
                newCentroids[j].r = sumR / count;
                newCentroids[j].g = sumG / count;
                newCentroids[j].b = sumB / count;
            }
        }

        // Check convergence
        double maxChange = 0;
        cilk_for(int j = 0; j < k; ++j)
        {
            double change = distance(centroids[j], newCentroids[j]);
            if (change > maxChange)
                maxChange = change;
            centroids[j] = newCentroids[j];
        }

        if (maxChange < tolerance)
            break;
    }
    return labels;
}

int main()
{
    srand(time(0));
    string inputCSV = "src/data/image_data.csv";
    string outputCSVSerial = "src/data/compressed_serial.csv";
    string outputCSVParallel = "src/data/compressed_parallel.csv";
    int k = 16; // Number of colors

    cout << "Reading image data.." << "\n";
    vector<Pixel> data = readImageCSV(inputCSV);
    cout << "Successfully read image data!" << "\n\n";

    // Serial
    vector<Pixel> centroidsSerial(k);
    for (int i = 0; i < k; ++i)
        centroidsSerial[i] = data[rand() % data.size()];
    auto start = chrono::high_resolution_clock::now();
    cout << "Starting Serial K-means.. " << "\n";
    vector<int> labelsSerial = kmeansSerial(data, centroidsSerial, k);
    auto end = chrono::high_resolution_clock::now();
    double serialTime = chrono::duration<double>(end - start).count();
    cout << "Serial Time: " << serialTime << "s\n";
    cout << "Saving compressed data into CSV file..." << "\n\n";
    saveCompressedCSV(data, labelsSerial, centroidsSerial, outputCSVSerial);

    // Parallel
    vector<Pixel> centroidsParallel(k);
    for (int i = 0; i < k; ++i)
        centroidsParallel[i] = data[rand() % data.size()];
    start = chrono::high_resolution_clock::now();
    cout << "Starting Parallel K-means.. " << "\n";
    vector<int> labelsParallel = kmeansParallel(data, centroidsParallel, k);
    end = chrono::high_resolution_clock::now();
    double parallelTime = chrono::duration<double>(end - start).count();
    cout << "Parallel Time: " << parallelTime << "s\n";
    cout << "Saving compressed data into CSV file..." << "\n\n";
    saveCompressedCSV(data, labelsParallel, centroidsParallel, outputCSVParallel);

    double speedup = serialTime / parallelTime;
    cout << "Speed-up: " << speedup << "\n";

    return 0;
}
