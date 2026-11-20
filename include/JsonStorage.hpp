#ifndef JSON_STORAGE_HPP
#define JSON_STORAGE_HPP

#include "IList.hpp"
#include "Geometry2D.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <stdexcept>


class JsonStorage {
public:
    static nlohmann::json circleToJson(const Geometry::Circle2D& c) {
        return nlohmann::json{
            {"type", "circle"},
            {"center", {{"x", c.getCenter().x}, {"y", c.getCenter().y}}},
            {"radius", c.getRadius()},
            {"area", c.area()},
            {"circumference", c.circumference()}
        };
    }

    static Geometry::Circle2D circleFromJson(const nlohmann::json& j) {
        double x = j.at("center").at("x").get<double>();
        double y = j.at("center").at("y").get<double>();
        double r = j.at("radius").get<double>();
        return Geometry::Circle2D(Geometry::Point2D(x, y), r);
    }

    static nlohmann::json lineToJson(const Geometry::Line2D& l) {
        return nlohmann::json{
            {"type", "line"},
            {"A", l.getA()},
            {"B", l.getB()},
            {"C", l.getC()}
        };
    }

    static Geometry::Line2D lineFromJson(const nlohmann::json& j) {
        return Geometry::Line2D(
            j.at("A").get<double>(),
            j.at("B").get<double>(),
            j.at("C").get<double>()
        );
    }

    static void saveSceneToFile(
        const std::string& filepath,
        const std::string& projectTitle,
        const IList<Geometry::Circle2D>& circles,
        const IList<Geometry::Line2D>& lines
    ) {
        nlohmann::json doc;
        doc["project"] = projectTitle;
        doc["metadata"] = {
            {"circles_count", circles.size()},
            {"lines_count", lines.size()},
            {"library", "nlohmann/json v3.11.3"}
        };

        nlohmann::json circlesArr = nlohmann::json::array();
        for (size_t i = 0; i < circles.size(); ++i) {
            circlesArr.push_back(circleToJson(circles[i]));
        }
        doc["circles"] = circlesArr;

        nlohmann::json linesArr = nlohmann::json::array();
        for (size_t i = 0; i < lines.size(); ++i) {
            linesArr.push_back(lineToJson(lines[i]));
        }
        doc["lines"] = linesArr;

        std::ofstream out(filepath);
        if (!out.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        out << doc.dump(4) << std::endl;
    }

    static std::string loadSceneFromFile(
        const std::string& filepath,
        IList<Geometry::Circle2D>& outCircles,
        IList<Geometry::Line2D>& outLines
    ) {
        std::ifstream in(filepath);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filepath);
        }
        nlohmann::json doc;
        in >> doc;

        outCircles.clear();
        if (doc.contains("circles")) {
            for (const auto& item : doc["circles"]) {
                outCircles.push_back(circleFromJson(item));
            }
        }

        outLines.clear();
        if (doc.contains("lines")) {
            for (const auto& item : doc["lines"]) {
                outLines.push_back(lineFromJson(item));
            }
        }

        return doc.value("project", "Unnamed Scene");
    }
};

#endif 
