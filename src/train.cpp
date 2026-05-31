#include "train.h"

Train::Car::Car(bool value) : light(value), next(this), prev(this) {}

Train::Train() : countOp(0), first(nullptr) {}

Train::~Train() {
    if (first == nullptr) {
        return;
    }
    Car *current = first->next;
    while (current != first) {
        Car *next = current->next;
        delete current;
        current = next;
    }
    delete first;
}

void Train::addCar(bool light) {
    Car *car = new Car(light);
    if (first == nullptr) {
        first = car;
        return;
    }
    Car *last = first->prev;
    last->next = car;
    car->prev = last;
    car->next = first;
    first->prev = car;
}

int Train::getLength() {
    countOp = 0;
    if (first == nullptr) {
        return 0;
    }
    Car *current = first;
    current->light = true;
    int distance = 0;
    while (true) {
        ++distance;
        current = current->next;
        ++countOp;
        if (current->light) {
            current->light = false;
            for (int i = 0; i < distance; ++i) {
                current = current->prev;
                ++countOp;
            }
            if (!current->light) {
                return distance;
            }
            for (int i = 0; i < distance; ++i) {
                current = current->next;
                ++countOp;
            }
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
