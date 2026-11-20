#include <cassert>
#include <iostream>
#include <chrono>
#include <cmath>
#include "ArrayList.hpp"
#include "LinkedList.hpp"
#include "StdListAdapter.hpp"
#include "SortAlgorithms.hpp"
#include "Geometry2D.hpp"
#include "JsonStorage.hpp"
#include "DataGenerator.hpp"

using namespace Geometry;

void testListAndSorting() {
    ArrayList<int> arr = { 9, 3, 7, 1, 5 };
    LinkedList<int> lst = { 9, 3, 7, 1, 5 };
    StdListAdapter<int> vec = { 9, 3, 7, 1, 5 };

    QuickSort<IList<int>> qs;
    MergeSort<IList<int>> ms;
    InsertionSort<IList<int>> is;

    qs.sort(arr);
    ms.sort(lst);
    is.sort(vec);

    for (size_t i = 0; i < 5; ++i) {
        int expected = static_cast<int>(2 * i + 1);
        assert(arr[i] == expected);
        assert(lst[i] == expected);
        assert(vec[i] == expected);
    }

    assert(arr.findByValue(7) == 3);
    assert(lst.findByValue(99) == -1);
    const int* gt5 = arr.findFirstIf([](int v) { return v > 5; });
    assert(gt5 != nullptr && *gt5 == 7);
}

void testGeometryOperations() {
    Circle2D unitCircle(Point2D(0, 0), 1.0);
    Line2D xAxis = Line2D::fromPoints(Point2D(-1, 0), Point2D(1, 0));

    auto pts = unitCircle.intersect(xAxis);
    assert(pts.size() == 2);

    Point2D p(3.0, 4.0);
    Point2D pRef = xAxis.reflect(p);
    assert(pRef == Point2D(3.0, -4.0));

    Point2D pInv = unitCircle.invert(Point2D(2.0, 0.0));
    assert(pInv == Point2D(0.5, 0.0));

    Line2D vertLine = Line2D::fromPoints(Point2D(2.0, -1.0), Point2D(2.0, 1.0));
    auto invShape = unitCircle.invert(vertLine);
    assert(std::holds_alternative<Circle2D>(invShape));
    Circle2D invC = std::get<Circle2D>(invShape);
    assert(invC.getCenter() == Point2D(0.25, 0.0));
    assert(areEqual(invC.getRadius(), 0.25));

    Circle2D cA(Point2D(2.0, 0.0), 1.5);
    Circle2D cB(Point2D(2.0, 2.0), 1.5);
    double angleBefore = cA.angleWith(cB);

    auto invA = unitCircle.invert(cA);
    auto invB = unitCircle.invert(cB);
    assert(std::holds_alternative<Circle2D>(invA) && std::holds_alternative<Circle2D>(invB));
    double angleAfter = std::get<Circle2D>(invA).angleWith(std::get<Circle2D>(invB));
    assert(areEqual(angleBefore, angleAfter));
}

void testJsonSerialization() {
    ArrayList<Circle2D> circles = { Circle2D(Point2D(1, 2), 3.5) };
    LinkedList<Line2D> lines = { Line2D(1, 1, -4) };

    JsonStorage::saveSceneToFile("test_scene.json", "UnitTestScene", circles, lines);

    LinkedList<Circle2D> loadedCircles;
    ArrayList<Line2D> loadedLines;
    std::string title = JsonStorage::loadSceneFromFile("test_scene.json", loadedCircles, loadedLines);

    assert(title == "UnitTestScene");
    assert(loadedCircles.size() == 1 && loadedCircles[0] == circles[0]);
    assert(loadedLines.size() == 1 && loadedLines[0] == lines[0]);
}

void runMicroBenchmark() {
    const size_t N = 2000;
    DataGenerator gen(123);
    ArrayList<int> arr1, arr2;
    gen.fillRandomInts(arr1, N, 0, 100000);
    arr2 = arr1;

    auto t1 = std::chrono::high_resolution_clock::now();
    InsertionSort<ArrayList<int>> ins;
    ins.sort(arr1);
    auto t2 = std::chrono::high_resolution_clock::now();
    QuickSort<ArrayList<int>> qs;
    qs.sort(arr2);
    auto t3 = std::chrono::high_resolution_clock::now();

    auto insUs = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto qsUs = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "[Benchmark N=" << N << "] InsertionSort: " << insUs
        << " us | QuickSort: " << qsUs << " us\n";
}

int main() {
    testListAndSorting();
    testGeometryOperations();
    testJsonSerialization();
    runMicroBenchmark();
    std::cout << "ALL UNIT TESTS PASSED SUCCESSFULLY!\n";
    return 0;
}