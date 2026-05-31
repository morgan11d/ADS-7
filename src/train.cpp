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
    countOp++;

    Car *current = first->next;
    int count = 1;
    countOp++;

    while (current != first && current->light == true) {
        count++;
        current = current->next;
        countOp++;
    }

    if (current == first) {
        return count;
    }

    current->light = true;
    countOp++;

    Car *checkpoint = current;

    for (int i = 0; i < count; i++) {
        current = current->prev;
        countOp++;
    }

    if (current->light == false) {
        return count;
    }

    while (true) {
        current = checkpoint->next;
        countOp++;
        int steps = 1;

        while (current != checkpoint && current->light == true) {
            steps++;
            current = current->next;
            countOp++;
        }

        if (current == checkpoint) {
            return count;
        }

        count += steps;

        current->light = true;
        countOp++;
        checkpoint = current;

        for (int i = 0; i < steps; i++) {
            current = current->prev;
            countOp++;
        }

        for (int i = 0; i < count - steps; i++) {
            current = current->prev;
            countOp++;
        }

        if (current->light == false) {
            return count;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
