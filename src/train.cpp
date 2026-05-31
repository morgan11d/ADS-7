// Copyright 2024 <Copyright Owner>

#include "train.h"

Train::Train() : countOp(0), first(nullptr) {}

Train::~Train() {
    if (first == nullptr) return;

    first->prev->next = nullptr;

    Car *current = first;
    Car *temp = nullptr;

    while (current != nullptr) {
        temp = current;
        current = current->next;
        delete temp;
    }
}

void Train::addCar(bool light) {
    Car *newCar = new Car();
    newCar->light = light;
    newCar->next = nullptr;
    newCar->prev = nullptr;

    if (first == nullptr) {
        first = newCar;
        newCar->next = newCar;
        newCar->prev = newCar;
    } else {
        newCar->next = first;
        newCar->prev = first->prev;
        first->prev->next = newCar;
        first->prev = newCar;
    }
}

int Train::getLength() {
    if (first == nullptr) return 0;
    if (first->next == first) return 1;

    countOp = 0;

    first->light = false;
    Car *car = first->next;
    int count = 1;
    countOp++;

    while (car->light == true) {
        car = car->next;
        count++;
        countOp++;
    }

    car->light = true;
    countOp++;

    int toGo = count;
    while (toGo > 0) {
        car = car->prev;
        countOp++;
        toGo--;
    }

    if (car->light == false) {
        return count;
    }

    Car *lastFound = car;
    for (int i = 0; i < count; i++) {
        lastFound = lastFound->next;
        countOp++;
    }

    int total = count;

    while (true) {
        car = lastFound->next;
        countOp++;
        int steps = 1;

        while (car->light == true) {
            car = car->next;
            steps++;
            countOp++;
        }

        car->light = true;
        countOp++;
        lastFound = car;
        total += steps;

        toGo = total;
        while (toGo > 0) {
            car = car->prev;
            countOp++;
            toGo--;
        }

        if (car->light == false) {
            return total;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
