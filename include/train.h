// Copyright 2024 <Copyright Owner>

#ifndef INCLUDE_TRAIN_H_
#define INCLUDE_TRAIN_H_

class Train {
 private:
    struct Car {
        bool light;
        Car *next;
        Car *prev;
        explicit Car(bool value);
    };

    int countOp;
    Car *first;

 public:
    Train();
    ~Train();
    void addCar(bool light);
    int getLength();
    int getOpCount();
};

#endif  // INCLUDE_TRAIN_H_
