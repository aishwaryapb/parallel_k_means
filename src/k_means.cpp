#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <chrono>
#include <iomanip>
#include <tuple>
#include <fstream>

using namespace std;

struct Point
{
    double x, y;
};

// Generate synthetic dataset
vector<Point> generateData(int numPoints)
{
    vector<Point> data(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        data[i].x = rand() % 1024;
        data[i].y = rand() % 1024;
    }
    return data;
}

// Euclidean distance
double distance(Point a, Point b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Save results to a CSV file
void saveResults(vector<Point> &data, vector<int> &labels, string filename)
{
    ofstream file(filename);
    for (size_t i = 0; i < data.size(); ++i)
    {
        file << data[i].x << "," << data[i].y << "," << labels[i] << "\n";
    }
    file.close();
}

// Serial K-means clustering with early stopping
vector<int> kmeansSerial(vector<Point> &data, int k, int max_iters = 100, double tolerance = 1e-4)
{
    int n = data.size();
    vector<Point> centroids(k);
    vector<int> labels(n, 0);

    // Initialize centroids
    for (int i = 0; i < k; ++i)
    {
        centroids[i] = data[rand() % n];
    }

    for (int iter = 0; iter < max_iters; ++iter)
    {
        vector<int> counts(k, 0);
        vector<Point> newCentroids(k, {0, 0});

        // Assign points to the nearest centroid
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
            newCentroids[labels[i]].x += data[i].x;
            newCentroids[labels[i]].y += data[i].y;
        }

        // Update centroids and check for convergence
        double maxChange = 0;
        for (int j = 0; j < k; ++j)
        {
            if (counts[j] > 0)
            {
                newCentroids[j].x /= counts[j];
                newCentroids[j].y /= counts[j];
            }
            double change = distance(centroids[j], newCentroids[j]);
            maxChange = max(maxChange, change);
            centroids[j] = newCentroids[j];
        }

        if (maxChange < tolerance)
            break; // Stop if centroids do not change significantly
    }
    return labels;
}

// Parallel K-means using Cilk++ with early stopping
vector<int> kmeansParallel(vector<Point> &data, int k, int max_iters = 100, double tolerance = 1e-4)
{
    int n = data.size();
    vector<Point> centroids(k);
    vector<int> labels(n, 0);

    // Initialize centroids
    for (int i = 0; i < k; ++i)
    {
        centroids[i] = data[rand() % n];
    }

    for (int iter = 0; iter < max_iters; ++iter)
    {
        vector<int> counts(k, 0);
        vector<Point> newCentroids(k, {0, 0});

        // Assign points to nearest centroid (Parallelized)
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

        // Compute new centroids (Parallelized reduction)
        cilk_for(int j = 0; j < k; ++j)
        {
            double sumX = 0, sumY = 0;
            int count = 0;
            for (int i = 0; i < n; ++i)
            {
                if (labels[i] == j)
                {
                    sumX += data[i].x;
                    sumY += data[i].y;
                    count++;
                }
            }
            if (count > 0)
            {
                newCentroids[j].x = sumX / count;
                newCentroids[j].y = sumY / count;
            }
        }

        // Check for convergence
        double maxChange = 0;
        cilk_for(int j = 0; j < k; ++j)
        {
            double change = distance(centroids[j], newCentroids[j]);
            if (change > maxChange)
            {
                maxChange = change;
            }
            centroids[j] = newCentroids[j];
        }

        if (maxChange < tolerance)
            break; // Stop if centroids do not change significantly
    }
    return labels;
}

int main()
{
    srand(time(0));
    vector<int> numPointsArr = {10000, 50000, 100000, 500000, 1000000};
    vector<int> kArr = {5, 10, 50, 100};
    vector<tuple<int, int, double, double, double>> results;

    for (int numPoints : numPointsArr)
    {
        cout << "Generating dataset with " << numPoints << " points..." << endl;
        vector<Point> data = generateData(numPoints);
        for (int k : kArr)
        {
            cout << "Running serial K-means with k = " << k << "..." << endl;
            auto start = chrono::high_resolution_clock::now();
            vector<int> labelsSerial = kmeansSerial(data, k);
            auto end = chrono::high_resolution_clock::now();
            double serialTime = chrono::duration<double>(end - start).count();
            cout << "Serial execution time: " << serialTime << " seconds." << endl;

            // Save results only for the first run for visualization
            if (numPoints == 10000 && k == 5)
            {
                saveResults(data, labelsSerial, "src/serial_clusters.csv");
            }

            cout << "Running parallel K-means with k = " << k << "..." << endl;
            start = chrono::high_resolution_clock::now();
            vector<int> labelsParallel = kmeansParallel(data, k);
            end = chrono::high_resolution_clock::now();
            double parallelTime = chrono::duration<double>(end - start).count();
            cout << "Parallel execution time: " << parallelTime << " seconds." << endl;

            // Save results only for the first run for visualization
            if (numPoints == 10000 && k == 5)
            {
                saveResults(data, labelsParallel, "src/parallel_clusters.csv");
            }

            double speedup = serialTime / parallelTime;
            results.emplace_back(numPoints, k, serialTime, parallelTime, speedup);
        }
    }

    cout << endl
         << "Final Results:" << endl;
    cout << left << setw(15) << "Num Points" << setw(10) << "K" << setw(15) << "Serial Time" << setw(15) << "Parallel Time" << setw(15) << "Speedup" << endl;
    cout << string(70, '-') << endl;
    for (const auto &res : results)
    {
        cout << left << setw(15) << get<0>(res) << setw(10) << get<1>(res) << setw(15) << get<2>(res) << setw(15) << get<3>(res) << setw(15) << get<4>(res) << endl;
    }

    return 0;
}
