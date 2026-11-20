#ifndef DATA_GENERATOR_HPP
#define DATA_GENERATOR_HPP

#include "IList.hpp"
#include "Geometry2D.hpp"
#include <random>
#include <string>

class DataGenerator {
private:
    std::mt19937 rng;

public:
    explicit DataGenerator(unsigned int seed = 42) : rng(seed) {}

    void fillRandomInts(IList<int>& list, size_t count, int minVal = -100, int maxVal = 100) {
        std::uniform_int_distribution<int> dist(minVal, maxVal);
        for (size_t i = 0; i < count; ++i) {
            list.push_back(dist(rng));
        }
    }

    void fillRandomDoubles(IList<double>& list, size_t count, double minVal = -50.0, double maxVal = 50.0) {
        std::uniform_real_distribution<double> dist(minVal, maxVal);
        for (size_t i = 0; i < count; ++i) {
            list.push_back(dist(rng));
        }
    }

    void fillRandomCircles(IList<Geometry::Circle2D>& list, size_t count) {
        std::uniform_real_distribution<double> coordDist(-20.0, 20.0);
        std::uniform_real_distribution<double> radDist(1.0, 15.0);
        for (size_t i = 0; i < count; ++i) {
            list.push_back(Geometry::Circle2D(
                Geometry::Point2D(coordDist(rng), coordDist(rng)),
                radDist(rng)
            ));
        }
    }

    void fillRandomLines(IList<Geometry::Line2D>& list, size_t count) {
        std::uniform_real_distribution<double> coeffDist(-10.0, 10.0);
        for (size_t i = 0; i < count; ++i) {
            double a = coeffDist(rng);
            double b = coeffDist(rng);
            if (Geometry::isZero(a) && Geometry::isZero(b)) a = 1.0;
            list.push_back(Geometry::Line2D(a, b, coeffDist(rng)));
        }
    }
};

#endif 
