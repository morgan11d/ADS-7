#include "train.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>

struct ExperimentResult {
    int trainLength;
    int operations;
};

ExperimentResult runExperiment(int length, const std::vector<bool>& lightStates) {
    Train train;
    
    for (int i = 0; i < length; i++) {
        train.addCar(lightStates[i]);
    }
    
    int calculatedLength = train.getLength();
    int operations = train.getOpCount();
    
    return {calculatedLength, operations};
}

void drawCircle(std::vector<std::vector<int>>& image, int x, int y, int r, int color) {
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            if (dx*dx + dy*dy <= r*r) {
                int px = x + dx;
                int py = y + dy;
                if (px >= 0 && px < (int)image[0].size() && py >= 0 && py < (int)image.size()) {
                    image[py][px] = color;
                }
            }
        }
    }
}

void drawLine(std::vector<std::vector<int>>& image, int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        if (x1 >= 0 && x1 < (int)image[0].size() && y1 >= 0 && y1 < (int)image.size()) {
            image[y1][x1] = color;
        }
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void createPlot(const std::vector<int>& lengths, 
                const std::vector<double>& allOffOps,
                const std::vector<double>& allOnOps, 
                const std::vector<double>& randomOps) {
    
    const int width = 1200;
    const int height = 800;
    const int margin = 100;
    const int colorWhite = 0;
    const int colorBlack = 1;
    const int colorRed = 2;
    const int colorBlue = 3;
    const int colorGreen = 4;
    const int colorGray = 5;
    const int colorLightRed = 6;
    const int colorLightBlue = 7;
    const int colorLightGreen = 8;
    
    std::vector<std::vector<int>> image(height, std::vector<int>(width, colorWhite));
    
    double maxOps = 0;
    for (size_t i = 0; i < lengths.size(); i++) {
        maxOps = std::max(maxOps, allOffOps[i]);
        maxOps = std::max(maxOps, allOnOps[i]);
        maxOps = std::max(maxOps, randomOps[i]);
    }
    maxOps *= 1.1;
    
    int maxLength = lengths.back();
    int minLength = lengths.front();
    
    int plotX1 = margin + 50;
    int plotY1 = margin;
    int plotX2 = width - margin;
    int plotY2 = height - margin - 50;
    
    drawLine(image, plotX1, plotY1, plotX1, plotY2, colorBlack);
    drawLine(image, plotX1, plotY2, plotX2, plotY2, colorBlack);
    
    for (int i = 0; i <= 10; i++) {
        int x = plotX1 + i * (plotX2 - plotX1) / 10;
        drawLine(image, x, plotY2 - 5, x, plotY2 + 5, colorBlack);
    }
    
    for (int i = 0; i <= 10; i++) {
        int y = plotY2 - i * (plotY2 - plotY1) / 10;
        drawLine(image, plotX1 - 5, y, plotX1 + 5, y, colorBlack);
    }
    
    for (int i = 0; i <= 10; i++) {
        int x = plotX1 + i * (plotX2 - plotX1) / 10;
        int y = plotY2 - i * (plotY2 - plotY1) / 10;
        if (i > 0) {
            drawLine(image, plotX1, y, plotX2, y, colorGray);
        }
        if (i < 10) {
            drawLine(image, x, plotY1, x, plotY2, colorGray);
        }
    }
    
    for (size_t i = 0; i < lengths.size(); i++) {
        int x = plotX1 + (lengths[i] - minLength) * (plotX2 - plotX1) / (maxLength - minLength);
        int yOff = plotY2 - allOffOps[i] * (plotY2 - plotY1) / maxOps;
        int yOn = plotY2 - allOnOps[i] * (plotY2 - plotY1) / maxOps;
        int yRand = plotY2 - randomOps[i] * (plotY2 - plotY1) / maxOps;
        
        drawCircle(image, x, yOff, 4, colorRed);
        drawCircle(image, x, yOn, 4, colorBlue);
        drawCircle(image, x, yRand, 4, colorGreen);
        
        if (i > 0) {
            int prevX = plotX1 + (lengths[i-1] - minLength) * (plotX2 - plotX1) / (maxLength - minLength);
            int prevYOff = plotY2 - allOffOps[i-1] * (plotY2 - plotY1) / maxOps;
            int prevYOn = plotY2 - allOnOps[i-1] * (plotY2 - plotY1) / maxOps;
            int prevYRand = plotY2 - randomOps[i-1] * (plotY2 - plotY1) / maxOps;
            
            drawLine(image, prevX, prevYOff, x, yOff, colorLightRed);
            drawLine(image, prevX, prevYOn, x, yOn, colorLightBlue);
            drawLine(image, prevX, prevYRand, x, yRand, colorLightGreen);
        }
    }
    
    drawCircle(image, width - 250, 50, 6, colorRed);
    drawCircle(image, width - 250, 80, 6, colorBlue);
    drawCircle(image, width - 250, 110, 6, colorGreen);
    
    std::ofstream ppm("result/plot.ppm");
    ppm << "P3\n" << width << " " << height << "\n255\n";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            switch(image[y][x]) {
                case colorBlack:
                    ppm << "0 0 0 ";
                    break;
                case colorRed:
                    ppm << "255 0 0 ";
                    break;
                case colorBlue:
                    ppm << "0 0 255 ";
                    break;
                case colorGreen:
                    ppm << "0 180 0 ";
                    break;
                case colorGray:
                    ppm << "220 220 220 ";
                    break;
                case colorLightRed:
                    ppm << "255 150 150 ";
                    break;
                case colorLightBlue:
                    ppm << "150 150 255 ";
                    break;
                case colorLightGreen:
                    ppm << "150 220 150 ";
                    break;
                default:
                    ppm << "255 255 255 ";
                    break;
            }
        }
        ppm << "\n";
    }
    
    ppm.close();
    
    system("ffmpeg -y -i result/plot.ppm result/plot.png 2>/dev/null");
}

int main() {
    srand(time(nullptr));
    
    const int MAX_TRAIN_LENGTH = 50;
    const int EXPERIMENTS_PER_LENGTH = 5;
    
    std::vector<int> lengths;
    std::vector<double> allOffOps;
    std::vector<double> allOnOps;
    std::vector<double> randomOps;
    
    for (int length = 2; length <= MAX_TRAIN_LENGTH; length += 2) {
        std::vector<int> offResults;
        std::vector<int> onResults;
        std::vector<int> randResults;
        
        for (int exp = 0; exp < EXPERIMENTS_PER_LENGTH; exp++) {
            std::vector<bool> allOff(length, false);
            auto result1 = runExperiment(length, allOff);
            offResults.push_back(result1.operations);
            
            std::vector<bool> allOn(length, true);
            auto result2 = runExperiment(length, allOn);
            onResults.push_back(result2.operations);
            
            std::vector<bool> random(length);
            for (int i = 0; i < length; i++) {
                random[i] = (rand() % 2 == 0);
            }
            auto result3 = runExperiment(length, random);
            randResults.push_back(result3.operations);
        }
        
        double avgOff = std::accumulate(offResults.begin(), offResults.end(), 0.0) / offResults.size();
        double avgOn = std::accumulate(onResults.begin(), onResults.end(), 0.0) / onResults.size();
        double avgRand = std::accumulate(randResults.begin(), randResults.end(), 0.0) / randResults.size();
        
        lengths.push_back(length);
        allOffOps.push_back(avgOff);
        allOnOps.push_back(avgOn);
        randomOps.push_back(avgRand);
    }
    
    createPlot(lengths, allOffOps, allOnOps, randomOps);
    
    return 0;
}
