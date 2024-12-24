#define _CRT_SECURE_NO_WARNINGS 1

#include <direct.h>
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

void static createDirectory(const string& path) {
    if (_mkdir(path.c_str()) != 0 && errno != EEXIST) {
        cerr << "Error creating directory: " << path << endl;
    }
}

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
    createDirectory("tests_results");
    ifstream file("tests_results/cumulative_averages.txt");
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
    createDirectory("tests_results");
    ofstream file("tests_results/cumulative_averages.txt");
    if (file.is_open()) {
        for (const auto& pair : averageHistory) {
            file << pair.first << " swaps: " << pair.second.averageTime << " seconds (averaged "
                << pair.second.timesAveraged << " times, total tests: " << pair.second.totalTests << ")\n";
        }
        file.close();
    }
    else {
        cerr << "Error creating cumulative_averages.txt" << endl;
    }
}

void static saveComparisonLog(const string& text, int logNumber) {
    createDirectory("tests_results");
    ofstream file("tests_results/performance_comparison.txt", ios::app);
    if (file.is_open()) {
        if (logNumber == 1) {
            file << "LOG 1 (Base, no comparison)\n";
        } else {
            file << "LOG " << logNumber << " (Compared to LOG " << logNumber - 1 << "):\n";
        }
        file << text;
        file.close();
    } else {
        cerr << "Error writing to performance_comparison.txt" << endl;
    }
}


map<int, double> static readLastRun() {
    map<int, double> lastRun;
    ifstream file("tests_results/last_run.txt");
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
    createDirectory("tests_results");
    ofstream file("tests_results/last_run.txt");
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
    createDirectory("tests_results");
    ofstream file("tests_results/run_history.txt", ios::app);
    if (file.is_open()) {
        file << "Run " << runNumber << ":\n";
        for (const auto& pair : currentRun) {
            file << pair.first << " swaps: " << pair.second << " seconds\n";
        }
        file.close();
    } else {
        cerr << "Error writing to run_history.txt" << endl;
    }
}

void static saveMaxDifference(const map<int, double>& currentRun, const map<int, double>& lastRun, int runNumber) {
    createDirectory("tests_results");
    ofstream file("tests_results/max_difference_log.txt", ios::app);
    if (file.is_open()) {
        file << "Run " << runNumber << ": ";
        if (runNumber == 1) {
            file << "Base (no data to compare)\n";
        } else {
            int maxSwaps = 0;
            double maxDifference = 0.0;
            double timeRun1 = 0.0;
            double timeRun2 = 0.0;

            for (const auto& pair : currentRun) {
                int swaps = pair.first;
                double currentTime = pair.second;
                double lastTime = lastRun.at(swaps);
                double difference = abs(currentTime - lastTime);
                if (difference > maxDifference) {
                    maxDifference = difference;
                    maxSwaps = swaps;
                    timeRun1 = lastTime;
                    timeRun2 = currentTime;
                }
            }
            file << "Swaps " << maxSwaps << " time_run" << runNumber - 1 << " (" << timeRun1 << " s) -> time_run" << runNumber
                 << " (" << timeRun2 << " s) = " << maxDifference << " seconds\n";
        }
        file.close();
    } else {
        cerr << "Error writing to max_difference_log.txt" << endl;
    }
}

void static saveTotalRunTime(const map<int, double>& currentRun, int tests, int runNumber) {
    createDirectory("tests_results");
    ofstream file("tests_results/total_run_time.txt", ios::app);
    if (file.is_open()) {
        double totalRunTime = 0.0;
        double firstTestTime = 0.0;
        bool firstTest = true;

        for (const auto& pair : currentRun) {
            totalRunTime += pair.second * tests;
            if (firstTest) {
                firstTestTime = pair.second * tests;
                firstTest = false;
            }
        }
        file << "Run " << runNumber << ": up to " << currentRun.size() << " swaps and " << tests << " tests per swap -> total time: "
             << totalRunTime << " seconds, first swap test time: " << firstTestTime << " seconds\n";
        file.close();
    } else {
        cerr << "Error writing to total_run_time.txt" << endl;
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

    saveMaxDifference(currentRun, lastRun, runNumber);
    saveTotalRunTime(currentRun, tests, runNumber);

    return 0;
}