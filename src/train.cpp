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

    countOp = 0;

    first->light = false;
    Car *current = first->next;
    int count = 1;
    countOp++;

    while (current != first) {
        if (current->light == false) {
            current->light = true;
            int steps = count;
            while (steps > 0) {
                current = current->prev;
                countOp++;
                steps--;
            }
            if (current->light == false) {
                return count;
            }
            steps = count;
            while (steps > 0) {
                current = current->next;
                countOp++;
                steps--;
            }
        }
        count++;
        current = current->next;
        countOp++;
    }

    return count;
}

int Train::getOpCount() {
    return countOp;
}
