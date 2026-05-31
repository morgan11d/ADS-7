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
    int length = 1;
    Car *cur = first;

    cur->light = false;

    cur = cur->next;
    countOp++;

    while (cur->light == true) {
        cur = cur->next;
        countOp++;
        length++;
    }

    cur->light = true;
    countOp++;

    for (int i = 0; i < length; i++) {
        cur = cur->prev;
        countOp++;
    }

    if (cur == first) {
        return length;
    }

    int known = length;
    Car *anchor = cur;
    for (int i = 0; i < length; i++) {
        anchor = anchor->next;
        countOp++;
    }

    while (true) {
        cur = anchor;
        int steps = 0;

        do {
            cur = cur->next;
            countOp++;
            steps++;
        } while (cur->light == true);

        cur->light = true;
        countOp++;
        anchor = cur;
        known += steps;

        for (int i = 0; i < known; i++) {
            cur = cur->prev;
            countOp++;
        }

        if (cur == first) {
            return known;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
