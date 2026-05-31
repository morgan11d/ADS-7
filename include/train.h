#ifndef INCLUDE_TRAIN_H_
#define INCLUDE_TRAIN_H_

class Train {
 private:
    struct Car {
        bool light;
        Car *next;
        Car *prev;
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
