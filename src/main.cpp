#include "train.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

struct ExperimentRow {
    int n;
    double allOff;
    double allOn;
    double randomState;
};

struct Rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class Image {
private:
    int width;
    int height;
    std::vector<unsigned char> pixels;

    static std::uint32_t crc32(const std::vector<unsigned char> &data) {
        std::uint32_t crc = 0xffffffffu;
        for (unsigned char byte : data) {
            crc ^= byte;
            for (int i = 0; i < 8; ++i) {
                crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
            }
        }
        return crc ^ 0xffffffffu;
    }

    static std::uint32_t adler32(const std::vector<unsigned char> &data) {
        std::uint32_t a = 1;
        std::uint32_t b = 0;
        for (unsigned char byte : data) {
            a = (a + byte) % 65521u;
            b = (b + a) % 65521u;
        }
        return (b << 16) | a;
    }

    static void writeUint32(std::vector<unsigned char> &out, std::uint32_t value) {
        out.push_back(static_cast<unsigned char>((value >> 24) & 255u));
        out.push_back(static_cast<unsigned char>((value >> 16) & 255u));
        out.push_back(static_cast<unsigned char>((value >> 8) & 255u));
        out.push_back(static_cast<unsigned char>(value & 255u));
    }

    static void appendChunk(std::vector<unsigned char> &out, const std::string &type, const std::vector<unsigned char> &data) {
        writeUint32(out, static_cast<std::uint32_t>(data.size()));
        std::vector<unsigned char> crcData;
        for (char c : type) {
            out.push_back(static_cast<unsigned char>(c));
            crcData.push_back(static_cast<unsigned char>(c));
        }
        out.insert(out.end(), data.begin(), data.end());
        crcData.insert(crcData.end(), data.begin(), data.end());
        writeUint32(out, crc32(crcData));
    }

    static std::vector<std::string> glyph(char c) {
        static const std::map<char, std::vector<std::string>> font = {
            {' ', {"00000", "00000", "00000", "00000", "00000", "00000", "00000"}},
            {'-', {"00000", "00000", "00000", "11111", "00000", "00000", "00000"}},
            {'.', {"00000", "00000", "00000", "00000", "00000", "01100", "01100"}},
            {'0', {"01110", "10001", "10011", "10101", "11001", "10001", "01110"}},
            {'1', {"00100", "01100", "00100", "00100", "00100", "00100", "01110"}},
            {'2', {"01110", "10001", "00001", "00010", "00100", "01000", "11111"}},
            {'3', {"11110", "00001", "00001", "01110", "00001", "00001", "11110"}},
            {'4', {"00010", "00110", "01010", "10010", "11111", "00010", "00010"}},
            {'5', {"11111", "10000", "10000", "11110", "00001", "00001", "11110"}},
            {'6', {"01110", "10000", "10000", "11110", "10001", "10001", "01110"}},
            {'7', {"11111", "00001", "00010", "00100", "01000", "01000", "01000"}},
            {'8', {"01110", "10001", "10001", "01110", "10001", "10001", "01110"}},
            {'9', {"01110", "10001", "10001", "01111", "00001", "00001", "01110"}},
            {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
            {'B', {"11110", "10001", "10001", "11110", "10001", "10001", "11110"}},
            {'C', {"01111", "10000", "10000", "10000", "10000", "10000", "01111"}},
            {'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
            {'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
            {'F', {"11111", "10000", "10000", "11110", "10000", "10000", "10000"}},
            {'G', {"01111", "10000", "10000", "10011", "10001", "10001", "01111"}},
            {'H', {"10001", "10001", "10001", "11111", "10001", "10001", "10001"}},
            {'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
            {'J', {"00111", "00010", "00010", "00010", "10010", "10010", "01100"}},
            {'K', {"10001", "10010", "10100", "11000", "10100", "10010", "10001"}},
            {'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
            {'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
            {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
            {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
            {'P', {"11110", "10001", "10001", "11110", "10000", "10000", "10000"}},
            {'Q', {"01110", "10001", "10001", "10001", "10101", "10010", "01101"}},
            {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
            {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
            {'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
            {'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
            {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
            {'W', {"10001", "10001", "10001", "10101", "10101", "10101", "01010"}},
            {'X', {"10001", "10001", "01010", "00100", "01010", "10001", "10001"}},
            {'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}},
            {'Z', {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}}
        };
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(c - 'a' + 'A');
        }
        auto it = font.find(c);
        if (it != font.end()) {
            return it->second;
        }
        return font.at(' ');
    }

public:
    Image(int w, int h) : width(w), height(h), pixels(static_cast<std::size_t>(w * h * 3), 255) {}

    void setPixel(int x, int y, Rgb color) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }
        std::size_t index = static_cast<std::size_t>((y * width + x) * 3);
        pixels[index] = color.r;
        pixels[index + 1] = color.g;
        pixels[index + 2] = color.b;
    }

    void line(int x1, int y1, int x2, int y2, Rgb color, int thickness = 1, bool dashed = false) {
        int dx = std::abs(x2 - x1);
        int sx = x1 < x2 ? 1 : -1;
        int dy = -std::abs(y2 - y1);
        int sy = y1 < y2 ? 1 : -1;
        int err = dx + dy;
        int step = 0;
        while (true) {
            bool draw = !dashed || ((step / 10) % 2 == 0);
            if (draw) {
                for (int ox = -thickness; ox <= thickness; ++ox) {
                    for (int oy = -thickness; oy <= thickness; ++oy) {
                        setPixel(x1 + ox, y1 + oy, color);
                    }
                }
            }
            if (x1 == x2 && y1 == y2) {
                break;
            }
            int e2 = 2 * err;
            if (e2 >= dy) {
                err += dy;
                x1 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y1 += sy;
            }
            ++step;
        }
    }

    void rect(int x1, int y1, int x2, int y2, Rgb color) {
        for (int y = y1; y <= y2; ++y) {
            for (int x = x1; x <= x2; ++x) {
                setPixel(x, y, color);
            }
        }
    }

    void text(int x, int y, const std::string &value, Rgb color, int scale = 2) {
        int cursor = x;
        for (char ch : value) {
            std::vector<std::string> data = glyph(ch);
            for (int row = 0; row < 7; ++row) {
                for (int col = 0; col < 5; ++col) {
                    if (data[row][col] == '1') {
                        for (int sy = 0; sy < scale; ++sy) {
                            for (int sx = 0; sx < scale; ++sx) {
                                setPixel(cursor + col * scale + sx, y + row * scale + sy, color);
                            }
                        }
                    }
                }
            }
            cursor += 6 * scale;
        }
    }

    void savePng(const std::string &filename) {
        std::vector<unsigned char> raw;
        raw.reserve(static_cast<std::size_t>((width * 3 + 1) * height));
        for (int y = 0; y < height; ++y) {
            raw.push_back(0);
            for (int x = 0; x < width; ++x) {
                std::size_t index = static_cast<std::size_t>((y * width + x) * 3);
                raw.push_back(pixels[index]);
                raw.push_back(pixels[index + 1]);
                raw.push_back(pixels[index + 2]);
            }
        }

        std::vector<unsigned char> zlib;
        zlib.push_back(0x78);
        zlib.push_back(0x01);
        std::size_t position = 0;
        while (position < raw.size()) {
            std::size_t blockSize = std::min<std::size_t>(65535, raw.size() - position);
            bool finalBlock = position + blockSize == raw.size();
            zlib.push_back(finalBlock ? 1 : 0);
            unsigned int len = static_cast<unsigned int>(blockSize);
            unsigned int nlen = 65535u - len;
            zlib.push_back(static_cast<unsigned char>(len & 255u));
            zlib.push_back(static_cast<unsigned char>((len >> 8) & 255u));
            zlib.push_back(static_cast<unsigned char>(nlen & 255u));
            zlib.push_back(static_cast<unsigned char>((nlen >> 8) & 255u));
            zlib.insert(zlib.end(), raw.begin() + static_cast<std::ptrdiff_t>(position), raw.begin() + static_cast<std::ptrdiff_t>(position + blockSize));
            position += blockSize;
        }
        writeUint32(zlib, adler32(raw));

        std::vector<unsigned char> png = {137, 80, 78, 71, 13, 10, 26, 10};
        std::vector<unsigned char> ihdr;
        writeUint32(ihdr, static_cast<std::uint32_t>(width));
        writeUint32(ihdr, static_cast<std::uint32_t>(height));
        ihdr.push_back(8);
        ihdr.push_back(2);
        ihdr.push_back(0);
        ihdr.push_back(0);
        ihdr.push_back(0);
        appendChunk(png, "IHDR", ihdr);
        appendChunk(png, "IDAT", zlib);
        appendChunk(png, "IEND", {});

        std::ofstream file(filename, std::ios::binary);
        file.write(reinterpret_cast<const char *>(png.data()), static_cast<std::streamsize>(png.size()));
    }
};

int runTrain(int n, int mode, std::mt19937 &generator) {
    Train train;
    std::bernoulli_distribution distribution(0.5);
    for (int i = 0; i < n; ++i) {
        bool light = false;
        if (mode == 1) {
            light = true;
        }
        if (mode == 2) {
            light = distribution(generator);
        }
        train.addCar(light);
    }
    train.getLength();
    return train.getOpCount();
}

std::array<double, 3> fitQuadratic(const std::vector<double> &x, const std::vector<double> &y) {
    double a[3][4] = {};
    for (std::size_t i = 0; i < x.size(); ++i) {
        double x0 = 1.0;
        double x1 = x[i];
        double x2 = x[i] * x[i];
        double p[3] = {x0, x1, x2};
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                a[r][c] += p[r] * p[c];
            }
            a[r][3] += p[r] * y[i];
        }
    }
    for (int col = 0; col < 3; ++col) {
        int pivot = col;
        for (int row = col + 1; row < 3; ++row) {
            if (std::abs(a[row][col]) > std::abs(a[pivot][col])) {
                pivot = row;
            }
        }
        for (int c = col; c < 4; ++c) {
            std::swap(a[col][c], a[pivot][c]);
        }
        double divisor = a[col][col];
        if (std::abs(divisor) < 1e-12) {
            continue;
        }
        for (int c = col; c < 4; ++c) {
            a[col][c] /= divisor;
        }
        for (int row = 0; row < 3; ++row) {
            if (row == col) {
                continue;
            }
            double factor = a[row][col];
            for (int c = col; c < 4; ++c) {
                a[row][c] -= factor * a[col][c];
            }
        }
    }
    return {a[0][3], a[1][3], a[2][3]};
}

std::string numberText(double value) {
    long long rounded = static_cast<long long>(value + 0.5);
    return std::to_string(rounded);
}

void drawPlot(const std::vector<ExperimentRow> &rows, const std::string &filename) {
    const int width = 1200;
    const int height = 800;
    const int left = 90;
    const int top = 70;
    const int right = 50;
    const int bottom = 90;
    const int plotWidth = width - left - right;
    const int plotHeight = height - top - bottom;
    const int minN = rows.front().n;
    const int maxN = rows.back().n;

    std::vector<double> x;
    std::vector<double> yOff;
    std::vector<double> yOn;
    std::vector<double> yRandom;
    for (const ExperimentRow &row : rows) {
        x.push_back(row.n);
        yOff.push_back(row.allOff);
        yOn.push_back(row.allOn);
        yRandom.push_back(row.randomState);
    }
    std::array<double, 3> offTrend = fitQuadratic(x, yOff);
    std::array<double, 3> onTrend = fitQuadratic(x, yOn);
    std::array<double, 3> randomTrend = fitQuadratic(x, yRandom);

    auto eval = [](const std::array<double, 3> &c, double value) {
        return c[0] + c[1] * value + c[2] * value * value;
    };

    double maxY = 0.0;
    for (const ExperimentRow &row : rows) {
        maxY = std::max(maxY, row.allOff);
        maxY = std::max(maxY, row.allOn);
        maxY = std::max(maxY, row.randomState);
        maxY = std::max(maxY, eval(offTrend, row.n));
        maxY = std::max(maxY, eval(onTrend, row.n));
        maxY = std::max(maxY, eval(randomTrend, row.n));
    }
    maxY *= 1.08;

    Image image(width, height);
    Rgb black{25, 25, 25};
    Rgb gray{225, 225, 225};
    Rgb lightGray{242, 242, 242};
    Rgb blue{35, 95, 180};
    Rgb red{190, 55, 55};
    Rgb green{45, 150, 85};

    auto px = [&](double value) {
        return left + static_cast<int>((value - minN) * plotWidth / (maxN - minN) + 0.5);
    };
    auto py = [&](double value) {
        return top + plotHeight - static_cast<int>(value * plotHeight / maxY + 0.5);
    };

    image.rect(left, top, left + plotWidth, top + plotHeight, Rgb{250, 250, 250});

    for (int i = 0; i <= 10; ++i) {
        int xGrid = left + i * plotWidth / 10;
        int yGrid = top + i * plotHeight / 10;
        image.line(xGrid, top, xGrid, top + plotHeight, gray);
        image.line(left, yGrid, left + plotWidth, yGrid, gray);
    }

    image.line(left, top, left, top + plotHeight, black, 1);
    image.line(left, top + plotHeight, left + plotWidth, top + plotHeight, black, 1);

    for (int i = 0; i <= 10; ++i) {
        double nValue = minN + (maxN - minN) * i / 10.0;
        double yValue = maxY * i / 10.0;
        int tickX = px(nValue);
        int tickY = py(yValue);
        image.line(tickX, top + plotHeight, tickX, top + plotHeight + 7, black);
        image.line(left - 7, tickY, left, tickY, black);
        image.text(tickX - 15, top + plotHeight + 18, numberText(nValue), black, 1);
        image.text(8, tickY - 4, numberText(yValue), black, 1);
    }

    auto drawSeries = [&](const std::vector<double> &values, Rgb color, bool dashed) {
        for (std::size_t i = 1; i < rows.size(); ++i) {
            image.line(px(rows[i - 1].n), py(values[i - 1]), px(rows[i].n), py(values[i]), color, dashed ? 0 : 1, dashed);
        }
    };

    auto buildTrend = [&](const std::array<double, 3> &coef) {
        std::vector<double> values;
        for (const ExperimentRow &row : rows) {
            values.push_back(std::max(0.0, eval(coef, row.n)));
        }
        return values;
    };

    drawSeries(yOff, blue, false);
    drawSeries(yOn, red, false);
    drawSeries(yRandom, green, false);
    drawSeries(buildTrend(offTrend), blue, true);
    drawSeries(buildTrend(onTrend), red, true);
    drawSeries(buildTrend(randomTrend), green, true);

    image.text(410, 25, "TRAIN OPERATIONS", black, 3);
    image.text(width / 2 - 5, height - 40, "N", black, 2);
    image.text(8, 32, "OPS", black, 2);

    int lx = 780;
    int ly = 95;
    image.rect(lx - 18, ly - 18, lx + 345, ly + 112, Rgb{255, 255, 255});
    image.line(lx, ly, lx + 55, ly, blue, 2);
    image.text(lx + 70, ly - 7, "ALL OFF", black, 2);
    image.line(lx, ly + 34, lx + 55, ly + 34, red, 2);
    image.text(lx + 70, ly + 27, "ALL ON", black, 2);
    image.line(lx, ly + 68, lx + 55, ly + 68, green, 2);
    image.text(lx + 70, ly + 61, "RANDOM", black, 2);
    image.line(lx + 210, ly + 68, lx + 265, ly + 68, black, 1, true);
    image.text(lx + 275, ly + 61, "TREND", black, 2);

    image.text(110, 740, "SOLID DATA DASHED QUADRATIC TREND", black, 1);
    image.savePng(filename);
}

int main() {
    std::filesystem::create_directories("result");

    const int minN = 2;
    const int maxN = 300;
    const int step = 2;
    const int randomRepeats = 25;

    std::mt19937 generator(123456);
    std::vector<ExperimentRow> rows;

    for (int n = minN; n <= maxN; n += step) {
        double off = runTrain(n, 0, generator);
        double on = runTrain(n, 1, generator);
        double randomTotal = 0.0;
        for (int i = 0; i < randomRepeats; ++i) {
            randomTotal += runTrain(n, 2, generator);
        }
        rows.push_back({n, off, on, randomTotal / randomRepeats});
    }

    std::ofstream data("result/data.csv");
    data << "n,all_off,all_on,random\n";
    for (const ExperimentRow &row : rows) {
        data << row.n << ',' << std::fixed << std::setprecision(2) << row.allOff << ',' << row.allOn << ',' << row.randomState << '\n';
    }

    drawPlot(rows, "result/plot.png");

    ExperimentRow last = rows.back();
    std::cout << "n=" << last.n << " all_off=" << last.allOff << " all_on=" << last.allOn << " random=" << std::fixed << std::setprecision(2) << last.randomState << std::endl;
    std::cout << "result/data.csv" << std::endl;
    std::cout << "result/plot.png" << std::endl;

    return 0;
}
