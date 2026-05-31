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

    while (current->light == true) {
        count++;
        current = current->next;
        countOp++;
    }

    current->light = true;
    Car *saved = current;

    for (int i = 0; i < count; i++) {
        current = current->prev;
        countOp++;
    }

    if (current->light == false) {
        return count;
    }

    Car *start = current;

    while (true) {
        current = saved->next;
        countOp++;
        int steps = 1;

        while (current->light == true) {
            steps++;
            current = current->next;
            countOp++;
        }

        current->light = true;
        saved = current;
        count += steps;

        for (int i = 0; i < count; i++) {
            current = current->prev;
            countOp++;
        }

        if (current == start && current->light == true) {
            return count;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
