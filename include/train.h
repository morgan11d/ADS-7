#ifndef TRAIN_H
#define TRAIN_H

class Train {
private:
    struct Car {
        bool light;
        Car *next;
        Car *prev;
        Car(bool value);
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

#endif
