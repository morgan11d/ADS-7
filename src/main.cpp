// Copyright 2024 <Copyright Owner>

#include "train.h"

#include <cstdlib>
#include <ctime>

#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

struct ExperimentResult {
    int operations;
};

ExperimentResult runExperiment(int length,
                                const std::vector<bool>& lightStates) {
    Train train;

    for (int i = 0; i < length; i++) {
        train.addCar(lightStates[i]);
    }

    train.getLength();
    int operations = train.getOpCount();

    return {operations};
}

int main() {
    srand(time(nullptr));

    const int MAX_TRAIN_LENGTH = 50;
    const int EXPERIMENTS_PER_LENGTH = 5;

    std::vector<int> lengths;
    std::vector<double> allOffOps;
    std::vector<double> allOnOps;
    std::vector<double> randomOps;

    for (int length = 2; length <= MAX_TRAIN_LENGTH; length += 2) {
        std::vector<int> offResults;
        std::vector<int> onResults;
        std::vector<int> randResults;

        for (int exp = 0; exp < EXPERIMENTS_PER_LENGTH; exp++) {
            std::vector<bool> allOff(length, false);
            auto result1 = runExperiment(length, allOff);
            offResults.push_back(result1.operations);

            std::vector<bool> allOn(length, true);
            auto result2 = runExperiment(length, allOn);
            onResults.push_back(result2.operations);

            std::vector<bool> random(length);
            for (int i = 0; i < length; i++) {
                random[i] = (rand() % 2 == 0);
            }
            auto result3 = runExperiment(length, random);
            randResults.push_back(result3.operations);
        }

        double avgOff = std::accumulate(offResults.begin(),
                                         offResults.end(), 0.0) /
                        offResults.size();
        double avgOn = std::accumulate(onResults.begin(),
                                        onResults.end(), 0.0) /
                       onResults.size();
        double avgRand = std::accumulate(randResults.begin(),
                                          randResults.end(), 0.0) /
                         randResults.size();

        lengths.push_back(length);
        allOffOps.push_back(avgOff);
        allOnOps.push_back(avgOn);
        randomOps.push_back(avgRand);
    }

    std::ofstream data("result/data.csv");
    data << "Length,AllOff,AllOn,Random\n";
    for (size_t i = 0; i < lengths.size(); i++) {
        data << lengths[i] << ","
             << allOffOps[i] << ","
             << allOnOps[i] << ","
             << randomOps[i] << "\n";
    }
    data.close();

    return 0;
}
