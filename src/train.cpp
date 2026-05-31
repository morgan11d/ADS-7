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

    Car *cur = first;
    int len = 0;

    do {
        cur = cur->next;
        countOp++;
        len++;
    } while (cur->light == true);

    cur->light = true;

    for (int i = 0; i < len; i++) {
        cur = cur->prev;
        countOp++;
    }

    if (cur->light == false) {
        return len;
    }

    int pos = 0;

    while (true) {
        int steps = 0;
        do {
            cur = cur->next;
            countOp++;
            steps++;
        } while (cur->light == true);

        cur->light = true;
        pos += steps;
        len += steps;

        for (int i = 0; i < len; i++) {
            cur = cur->prev;
            countOp++;
        }

        if (cur->light == false) {
            return len;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
