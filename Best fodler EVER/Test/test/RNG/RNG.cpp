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
//oт тук започва моята история как исках да проверя разликата във всеки рън 
//и ми отиде скрола на мишката

//Сериозен коментар проверката може да я няма нищо се не чупи (освен скрола на мишката)
//, но за да сме сигурни, че аз в бъдеще няма да оплескам нещо по файловете че вече 15 ръна не са малко
//е хубаво да има една проверка дали правилно чете стойностите 
//edit успях да го прецакам дори на правилна проверка avrg time и total tests са десимал

map<int, AverageData> static readAverageHistory() {
    map<int, AverageData> history;
    ifstream file("cumulative_averages.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            int swaps;
            AverageData data;
            if (sscanf(line.c_str(), "%d swaps: %lf seconds (averaged %d times, total tests: %d)",
                &swaps, &data.averageTime, &data.timesAveraged, &data.totalTests) == 4) {
                history[swaps] = data;
            }
        }
        file.close();
    }
    return history;
}

void static saveCumulativeAverage(const string& text) {
    ofstream file("cumulative_averages.txt");
    if (file.is_open()) {
        file << text;
        file.close();
    }
    else {
        cerr << "Error" << endl;
    }
}

void static saveComparisonLog(const string& text) {
    ofstream file("performance_comparison.txt", ios::app);
    if (file.is_open()) {
        file << text;
        file.close();
    }
    else {
        cerr << "Error" << endl;
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

//аз съм идиот, предобрих го в началота 3 часа и една мишка за 10 реда
//човек еби му майката да проверя как се четат и записват данни от последният изпълнен код ми се счупи мишката.
void saveLastRun(const map<int, double>& currentRun) {
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

int main() {
    //трябва ли да се обеснявам защо масива е подреден не искам да реда а да видя скороста на брой размествания и 10 елемента ми харесва с 2 ще е същия кур
    int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int size = sizeof(arr) / sizeof(arr[0]);
    int tests = 1000; //променяай на воля тук 1000 теста са малко и 200 лева са много за една мишка 

    vector<int> swapCounts;
    for (int i = 1; i <= 100; i++) {
        swapCounts.push_back(i);
    }

    // Приемам дарения да си купя нова мишка
    map<int, AverageData> averageHistory = readAverageHistory();
    map<int, double> lastRun = readLastRun();
    map<int, double> currentRun;

    ostringstream cumulativeResult;
    ostringstream comparisonResult;

    bool isFirstRun = lastRun.empty();
    if (isFirstRun) {
        comparisonResult << "LOG 1 (No comparing)\n";
    }
    else {
        comparisonResult << "LOG " << (averageHistory.empty() ? 1 : averageHistory.begin()->second.timesAveraged + 1) << ":\n";
    }
    // коментара е след 10 поредни старта на 1000 теста по 100 swapa 
    //кой може да ми каже как на толкова много теста средната разлика може да е от близо 20%
    //добре знам че 30 микросекудни са малко ама 20% разлика е много
    //аз съм идиот част 2
    for (int swaps : swapCounts) {
        double currentTime = measureSwaps(arr, size, swaps, tests);
        currentRun[swaps] = currentTime;

        //взимам средната стойност и я смятам на ново средно на средното 
        //(знам че е грешно мързи ме да събирам всички данни от миналите изпълнения сучипих си мишката за простотията по-горе 
        // и wtf stackoverflow помощнете на хората като питат с нормални отговори)
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

        // Еби му майката и на проверката човек press f to pay respect for the mouse
        if (!isFirstRun) {
            double diff = currentTime - lastRun[swaps];
            string comparison = diff > 0 ? "+" : "";
            comparisonResult << " swaps: " << swaps << comparison << diff;
        }
    }

    saveCumulativeAverage(cumulativeResult.str());
    saveComparisonLog(comparisonResult.str());
    saveLastRun(currentRun);

    cout << "Open the folder you now have txt files magic";

    return 0;
}

//Бока знам че само ти ще четеш това знаеш ли сайт в който в момента да има намаления на мишки