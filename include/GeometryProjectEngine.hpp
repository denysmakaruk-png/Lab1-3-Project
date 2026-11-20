#ifndef GEOMETRY_PROJECT_ENGINE_HPP
#define GEOMETRY_PROJECT_ENGINE_HPP

#include "IList.hpp"
#include "ArrayList.hpp"
#include "LinkedList.hpp"
#include "SortAlgorithms.hpp"
#include "Geometry2D.hpp"
#include "JsonStorage.hpp"
#include <iostream>
#include <string>
#include <memory>

class GeometryProjectEngine {
private:
    std::unique_ptr<IList<Geometry::Circle2D>> circles;
    std::unique_ptr<IList<Geometry::Line2D>> lines;
    std::string containerType;

public:
    explicit GeometryProjectEngine(bool useLinkedList = false) {
        switchContainerImplementation(useLinkedList);
    }

    void switchContainerImplementation(bool useLinkedList) {
        auto newCircles = useLinkedList
            ? std::unique_ptr<IList<Geometry::Circle2D>>(std::make_unique<LinkedList<Geometry::Circle2D>>())
            : std::unique_ptr<IList<Geometry::Circle2D>>(std::make_unique<ArrayList<Geometry::Circle2D>>());

        auto newLines = useLinkedList
            ? std::unique_ptr<IList<Geometry::Line2D>>(std::make_unique<LinkedList<Geometry::Line2D>>())
            : std::unique_ptr<IList<Geometry::Line2D>>(std::make_unique<ArrayList<Geometry::Line2D>>());

        if (circles) {
            for (size_t i = 0; i < circles->size(); ++i) newCircles->push_back((*circles)[i]);
        }
        if (lines) {
            for (size_t i = 0; i < lines->size(); ++i) newLines->push_back((*lines)[i]);
        }

        circles = std::move(newCircles);
        lines = std::move(newLines);
        containerType = useLinkedList ? "LinkedList" : "ArrayList";
    }

    void addCircle(const Geometry::Circle2D& c) { circles->push_back(c); }
    void addLine(const Geometry::Line2D& l) { lines->push_back(l); }

    IList<Geometry::Circle2D>& getCircles() { return *circles; }
    IList<Geometry::Line2D>& getLines() { return *lines; }
    const std::string& getContainerType() const { return containerType; }

    void sortCirclesByRadiusDesc() {
        auto radiusDesc = SortUtils::makeKeyComparator(
            [](const Geometry::Circle2D& c) { return c.getRadius(); },
            std::greater<double>()
        );
        QuickSort<IList<Geometry::Circle2D>, decltype(radiusDesc)> sorter;
        sorter.sort(*circles, radiusDesc);
    }

    void printSceneReport() const {
        std::cout << "=== Project Scene Report [Container: " << containerType << "] ===\n";
        std::cout << "Circles (" << circles->size() << "): " << circles->toString() << "\n";
        std::cout << "Lines   (" << lines->size() << "): " << lines->toString() << "\n";

        for (size_t i = 0; i < circles->size(); ++i) {
            for (size_t j = 0; j < lines->size(); ++j) {
                auto pts = (*circles)[i].intersect((*lines)[j]);
                std::cout << "  Intersection Circle[" << i << "] & Line[" << j << "]: ";
                if (pts.empty()) {
                    std::cout << "none\n";
                } else {
                    for (const auto& p : pts) std::cout << p << " ";
                    std::cout << "(angle = " << ((*circles)[i].angleWith((*lines)[j]) * 180.0 / Geometry::PI) << " deg)\n";
                }
            }
        }
    }

    void exportJson(const std::string& filename) const {
        JsonStorage::saveSceneToFile(filename, "2D Geometry & Inversion Analyzer", *circles, *lines);
    }

    void importJson(const std::string& filename) {
        JsonStorage::loadSceneFromFile(filename, *circles, *lines);
    }
};

#endif 
