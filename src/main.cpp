#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "IList.hpp"
#include "LinkedList.hpp"
#include "ArrayList.hpp"
#include "StdListAdapter.hpp"
#include "SortAlgorithms.hpp"
#include "Geometry2D.hpp"
#include "DataGenerator.hpp"
#include "JsonStorage.hpp"
#include "GeometryProjectEngine.hpp"

using namespace Geometry;

struct Student {
    std::string name;
    int age;
    double gpa;

    bool operator==(const Student& o) const {
        return name == o.name && age == o.age && std::fabs(gpa - o.gpa) < 1e-6;
    }

    friend std::ostream& operator<<(std::ostream& os, const Student& s) {
        return os << s.name << "(age=" << s.age << ", gpa=" << s.gpa << ")";
    }
};

void demonstrateLab1AndLab2() {
    std::cout << "====================================================================\n";
    std::cout << "=== PART 1: DATA STRUCTURES, SORTING & POLYMORPHIC IList<T>      ===\n";
    std::cout << "====================================================================\n";

    ArrayList<int> intArr;
    LinkedList<double> doubleList;
    StdListAdapter<std::string> strAdapter = {"Zebra", "Alpha", "Omega", "Delta"};

    DataGenerator gen(2026);
    gen.fillRandomInts(intArr, 8, 1, 99);
    gen.fillRandomDoubles(doubleList, 5, -10.0, 10.0);

    std::cout << "Random ArrayList<int>:      " << intArr.toString() << "\n";
    QuickSort<IList<int>> qsInt;
    qsInt.sort(intArr);
    std::cout << "Sorted ArrayList<int>:      " << intArr.toString() << "\n";

    std::cout << "Random LinkedList<double>:  " << doubleList.toString() << "\n";
    MergeSort<IList<double>> msDouble;
    msDouble.sort(doubleList);
    std::cout << "Sorted LinkedList<double>:  " << doubleList.toString() << "\n";

    std::cout << "StdListAdapter<string>:     " << strAdapter.toString() << "\n";
    InsertionSort<IList<std::string>> isStr;
    isStr.sort(strAdapter);
    std::cout << "Sorted StdListAdapter<str>: " << strAdapter.toString() << "\n";

    const int* foundEven = intArr.findFirstIf([](int x) { return x % 2 == 0; });
    std::cout << "First even int in ArrayList: " << (foundEven ? std::to_string(*foundEven) : "none") << "\n\n";

    std::cout << "====================================================================\n";
    std::cout << "=== PART 2: 2D GEOMETRY (LINES & CIRCLES, REFLECTION, INVERSION) ===\n";
    std::cout << "====================================================================\n";

    Circle2D invCircle(Point2D(0.0, 0.0), 4.0);
    Circle2D c2(Point2D(5.0, 0.0), 3.0);
    Line2D line = Line2D::fromPoints(Point2D(0.0, 2.0), Point2D(4.0, 2.0));

    std::cout << "Circle 1 (Inversion): " << invCircle << "\n";
    std::cout << "Circle 2:             " << c2 << "\n";
    std::cout << "Line 1:               " << line << "\n";

    auto ccPts = invCircle.intersect(c2);
    std::cout << "Intersection Circle 1 & Circle 2 (" << ccPts.size() << " pts): ";
    for (const auto& p : ccPts) std::cout << p << " ";
    std::cout << "\n";

    auto clPts = invCircle.intersect(line);
    std::cout << "Intersection Circle 1 & Line 1 (" << clPts.size() << " pts): ";
    for (const auto& p : clPts) std::cout << p << " ";
    std::cout << "\n";

    Point2D testPt(1.0, 5.0);
    std::cout << "Reflection of " << testPt << " across Line 1: " << line.reflect(testPt) << "\n";
    std::cout << "Reflection of Circle 2 across Line 1:   " << c2.reflectAcross(line) << "\n";

    Point2D pOut(8.0, 0.0);
    std::cout << "Inversion of point (8, 0) wrt Circle 1: " << invCircle.invert(pOut) << "\n";

    auto invLineRes = invCircle.invert(line);
    if (std::holds_alternative<Circle2D>(invLineRes)) {
        std::cout << "Inversion of Line (y=2) wrt Circle 1 -> " << std::get<Circle2D>(invLineRes) << "\n";
    }

    double angleBefore = invCircle.angleWith(c2);
    Circle2D interC2(Point2D(3.0, 0.0), 3.5);
    Circle2D interC3(Point2D(0.0, 3.0), 3.5);
    double angleC2C3 = interC2.angleWith(interC3);

    auto invC2 = invCircle.invert(interC2);
    auto invC3 = invCircle.invert(interC3);
    if (std::holds_alternative<Circle2D>(invC2) && std::holds_alternative<Circle2D>(invC3)) {
        double angleAfter = std::get<Circle2D>(invC2).angleWith(std::get<Circle2D>(invC3));
        std::cout << "Conformality check (angle C2 & C3): before = "
                  << (angleC2C3 * 180.0 / PI) << " deg, after inversion = "
                  << (angleAfter * 180.0 / PI) << " deg\n";
    }
    (void)angleBefore;
}

void demonstrateLab3AndProject() {
    std::cout << "\n====================================================================\n";
    std::cout << "=== PART 3: LAB 3 (nlohmann/json) & PROJECT ENGINE WORKFLOW        ===\n";
    std::cout << "====================================================================\n";

    GeometryProjectEngine engine(false);
    engine.addCircle(Circle2D(Point2D(0, 0), 5.0));
    engine.addCircle(Circle2D(Point2D(3, 4), 2.5));
    engine.addLine(Line2D::fromPoints(Point2D(-5, 0), Point2D(5, 0)));
    engine.addLine(Line2D::fromPoints(Point2D(0, -5), Point2D(0, 5)));

    engine.sortCirclesByRadiusDesc();
    engine.printSceneReport();

    const std::string jsonPath = "geometry_scene.json";
    engine.exportJson(jsonPath);
    std::cout << "\n[Lab 3] Scene exported successfully to '" << jsonPath << "' using nlohmann/json.\n";

    GeometryProjectEngine restoredEngine(true);
    restoredEngine.importJson(jsonPath);
    std::cout << "[Lab 3] Scene restored from JSON into LinkedList-backed engine:\n";
    restoredEngine.printSceneReport();
}

int main() {
    demonstrateLab1AndLab2();
    demonstrateLab3AndProject();
    return 0;
}
