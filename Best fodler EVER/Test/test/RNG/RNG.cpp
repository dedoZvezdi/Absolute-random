#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct AverageData {
    double averageTime;
    int timesAveraged;
    int totalTests;
};

void static swapElements(int arr[], int size) {
    int firstElement = arr[0];
    for (int i = 0; i < size - 1; i++) {
        int temp = arr[i];
        arr[i] = arr[i + 1];
        arr[i + 1] = temp;
    }
    arr[size - 1] = firstElement;
}

map<int, AverageData> static readAverageHistory() {
    map<int, AverageData> history;
    ifstream file("cumulative_averages.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            int swaps;
            AverageData data{};
            if (sscanf(line.c_str(), "%d swaps: %lf seconds (averaged %d times, total tests: %d)",
                &swaps, &data.averageTime, &data.timesAveraged, &data.totalTests) == 4) {
                history[swaps] = data;
            }
        }
        file.close();
    }
    return history;
}

void static saveCumulativeAverage(const map<int, AverageData>& averageHistory) {
    ofstream file("cumulative_averages.txt");
    if (file.is_open()) {
        for (const auto& pair : averageHistory) {
            file << pair.first << " swaps: " << pair.second.averageTime << " seconds (averaged "
                << pair.second.timesAveraged << " times, total tests: " << pair.second.totalTests << ")\n";
        }
        file.close();
    }
    else {
        cerr << "Error1" << endl;
    }
}

void static saveComparisonLog(const string& text, int logNumber) {
    ofstream file("performance_comparison.txt", ios::app);
    if (file.is_open()) {
        if (logNumber == 1) {
            file << "LOG 1 (Base, no comparison)\n";
        }
        else {
            file << "LOG " << logNumber << " (Compared to LOG " << logNumber - 1 << "):\n";
        }
        file << text;
        file.close();
    }
    else {
        cerr << "Error2" << endl;
    }
}


map<int, double> static readLastRun() {
    map<int, double> lastRun;
    ifstream file("last_run.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            int swaps;
            double time;
            if (sscanf(line.c_str(), "%d swaps: %lf", &swaps, &time) == 2) {
                lastRun[swaps] = time;
            }
        }
        file.close();
    }
    return lastRun;
}

void static saveLastRun(const map<int, double>& currentRun) {
    ofstream file("last_run.txt");
    if (file.is_open()) {
        for (const auto& pair : currentRun) {
            file << pair.first << " swaps: " << pair.second << endl;
        }
        file.close();
    }
}

double static measureSwaps(int arr[], int size, int swaps, int tests) {
    double totalElapsed = 0;
    for (int t = 0; t < tests; t++) {
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < swaps; i++) {
            swapElements(arr, size);
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        totalElapsed += elapsed.count();
    }
    return totalElapsed / tests;
}

void static saveRunHistory(const map<int, double>& currentRun, int runNumber) {
    ofstream file("run_history.txt", ios::app);
    if (file.is_open()) {
        file << "Run " << runNumber << ":\n";
        for (const auto& pair : currentRun) {
            file << pair.first << " swaps: " << pair.second << " seconds\n";
        }
        file.close();
    }
    else {
        cerr << "Error3" << endl;
    }
}

int main() {
    int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int size = sizeof(arr) / sizeof(arr[0]);
    int tests = 1000;

    vector<int> swapCounts;
    for (int i = 1; i <= 100; i++) {
        swapCounts.push_back(i);
    }

    map<int, AverageData> averageHistory = readAverageHistory();
    map<int, double> lastRun = readLastRun();
    map<int, double> currentRun;

    ostringstream cumulativeResult;
    ostringstream comparisonResult;

    bool isFirstRun = lastRun.empty();
    int runNumber = 1;

    if (!averageHistory.empty()) {
        runNumber = averageHistory.begin()->second.timesAveraged + 1;
    }

    for (int swaps : swapCounts) {
        double currentTime = measureSwaps(arr, size, swaps, tests);
        currentRun[swaps] = currentTime;

        AverageData& data = averageHistory[swaps];
        if (data.timesAveraged == 0) {
            data.averageTime = currentTime;
            data.timesAveraged = 1;
            data.totalTests = tests;
        }
        else {
            data.averageTime = (data.averageTime * data.timesAveraged + currentTime) / (data.timesAveraged + 1);
            data.timesAveraged++;
            data.totalTests += tests;
        }

        cumulativeResult << swaps << " swaps: " << data.averageTime << " seconds (averaged "
            << data.timesAveraged << " times, total tests: " << data.totalTests << ")\n";

        if (!isFirstRun) {
            double diff = currentTime - lastRun[swaps];
            string comparison = diff > 0 ? "+" : "";
            comparisonResult << swaps << " swaps: " << comparison << diff << endl;
        }
    }

    saveCumulativeAverage(averageHistory);
    saveComparisonLog(comparisonResult.str(), runNumber);
    saveLastRun(currentRun);
    saveRunHistory(currentRun, runNumber);

    return 0;
}
