// Copyright 2024 <Copyright Owner>

#include <ctime>

#include <cstdint>
#include <cstdlib>
#include <fstream>

#include "train.h"

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int kMaxLength = 50;
    const int kExperiments = 20;

    std::ofstream csv("result/data.csv");
    csv << "n,all_off,all_on,random\n";

    for (int n = 2; n <= kMaxLength; n += 2) {
        int64_t sumOff = 0;
        int64_t sumOn = 0;
        int64_t sumRand = 0;

        for (int e = 0; e < kExperiments; e++) {
            Train trainOff;
            for (int i = 0; i < n; i++) {
                trainOff.addCar(false);
            }
            trainOff.getLength();
            sumOff += trainOff.getOpCount();

            Train trainOn;
            for (int i = 0; i < n; i++) {
                trainOn.addCar(true);
            }
            trainOn.getLength();
            sumOn += trainOn.getOpCount();

            Train trainRand;
            for (int i = 0; i < n; i++) {
                trainRand.addCar((std::rand() % 2) == 1);
            }
            trainRand.getLength();
            sumRand += trainRand.getOpCount();
        }

        csv << n << ","
            << static_cast<double>(sumOff) / kExperiments << ","
            << static_cast<double>(sumOn) / kExperiments << ","
            << static_cast<double>(sumRand) / kExperiments << "\n";
    }

    csv.close();

    return 0;
}
