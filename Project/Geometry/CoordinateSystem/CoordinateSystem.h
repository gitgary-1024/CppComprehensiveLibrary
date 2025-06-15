/*
    Copyright (c) June 9, 2025 Gitgary-1024

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include <iostream>
#include <cmath>
#include "../../Configuration.h"
#include "../Funtion/Funtion.h"
#include "../../EquationAndFunction/EquationClasses.h"

class Dot {
		double x, y;
	public:
		Dot() {
			x = 0;
			y = 0;
		}
		
		Dot(double x, double y) {
			this->x = x;
			this->y = y;
		}
		
		~Dot() {
		}
		
		double getX() const {
			return x;
		}
		
		double getY() const {
			return y;
		}
		
		void setX(double x) {
			this->x = x;
		}
		
		void setY(double y) {
			this->y = y;
		}
		
		double distanceToOther(const Dot& other) const {
			double dx = x - other.x;
			double dy = y - other.y;
			return std::sqrt(dx * dx + dy * dy);
		}
		
		Dot operator+(const Dot& other) const {
			return Dot(x + other.x, y + other.y);
		}
		
		Dot operator-(const Dot& other) const {
			return Dot(x - other.x, y - other.y);
		}
		
		Dot operator*(double scalar) const {
			return Dot(x * scalar, y * scalar);
		}
		
		void print() const {
			std::cout << "(" << x << ", " << y << ")" << std::endl;
		}
};

Dot operator*(double scalar, const Dot& dot) {
	return dot * scalar;
}

class Line {
		Function func; // func=0
		std::unordered_map<std::string, double> coefficient; // 系数
		double a, b, c; // 形如:a*x+b*y+c
	public:
		Line() {}
		
		Line(const std::string& analytical)
			: func(analytical) {
			coefficient = func.getExpression().printPublicTerms();
			a = coefficient["x"];
			b = coefficient["y"];
			c = coefficient["constant"];
		}
		
		Line(Dot a, Dot b) {
			double x_1 = a.getX();
			double y_1 = a.getY();
			double x_2 = b.getX();
			double y_2 = b.getY();
			// 计算直线的一般式方程 ax + by + c = 0
			double a_1 = y_2 - y_1;
			double b_1 = x_1 - x_2;
			double c_1 = x_2 * y_1 - x_1 * y_2;
			// 使用 std::ostringstream 拼接字符串
			std::ostringstream oss;
			
			if (c_1 >= 0) {
				oss << "(" << a_1 << ")*x + (" << b_1 << ")*y + " << c_1;
			}
			else {
				oss << "(" << a_1 << ")*x + (" << b_1 << ")*y - " << -c_1;
			}
			
			std::string analytical = oss.str();
			Line l(analytical);
			func = l.func;
			coefficient = l.coefficient;
		}
		
		~Line() {}
		
		bool isIntersect(Line& other) const { // 相交
			try {
				Dot d = intersectionPoints(other);
				return 1;
			}
			catch (const std::invalid_argument& e) {
				return 0;
			}
		}
		
		Dot intersectionPoints(const Line& other) const { // 交点
			try {
				LinearEquationSystem system({func.getAnalytical() + "=0", other.func.getAnalytical() + "=0"}, {"x", "y"});
				std::vector<double> result = system.solve();
				return Dot(result[0], result[1]);
			}
			catch (const std::exception& e) {
				throw std::invalid_argument("Two straight lines seem parallel");
			}
		}
		
		bool isParallel(Line& other) const { // 平行
			return !isIntersect(other);
		}
		
		bool isVertical(Line& other) const { // 垂直
			return a * other.a + b * other.b == 0;
		}
		
		double disToLine(const Dot& dot) const {
			double x_0 = dot.getX();
			double y_0 = dot.getY();
			double d = std::abs(a * x_0 + b * y_0 + c) / std::sqrt(std::pow(a, 2) + std::pow(b, 2));
			return d;
		}
		
		double disToLine(const Line& line) const {
			if (!isParallel(const_cast<Line&>(line))) {
				throw std::invalid_argument("The two given lines are not parallel");
			}
			
			double d = std::abs(c - line.c) / std::sqrt(std::pow(a, 2) + std::pow(b, 2));
			return d;
		}
		
		double angle(Line& line) const { // 求角度
			if (isParallel(line)) {
				throw std::invalid_argument("The two given lines are parallel");
			}
			
			double numerator = std::abs(line.a * b - a * line.b);
			double denominator = a * line.a + b * line.b;
			return std::atan2(numerator, denominator);
		}
		
		ExpressionEvaluator getExpression() {
			return func.getExpression();
		}
};

class Segment {
		Dot vertex_a, vertex_b; //顶点 a < b
		Line func; //解析式
		double length;
	public:
		Segment() {
		}
		
		Segment(Dot a, Dot b) { //从端点构造
			vertex_a = a;
			vertex_b = b;
			func = Line(a, b);
			length = a.distanceToOther(b);
			
			if (a.getX() > b.getX() || a.getY() > b.getY()) {
				std::swap(a, b);
			}
		}
		
		Segment(Line l) { //两端点无线远
			func = l;
			length = DBL_MAX;
			vertex_a = {DBL_MIN, DBL_MIN};
			vertex_b = {DBL_MAX, DBL_MAX};
		}
		
		// Dot 交点
		Dot intersection(const Segment& other) const {
			try {
				Dot intersection = func.intersectionPoints(other.func);
				double ix = intersection.getX(), iy = intersection.getY();
				
				if (ix < vertex_a.getX() || ix > vertex_b.getX() || iy < vertex_a.getY() || iy > vertex_b.getY()) {
					throw std::invalid_argument("Two line segments do not intersect");
				}
				
				return intersection;
			}
			catch (const std::invalid_argument& e) {
				throw std::invalid_argument("The two line segments appear to be parallel");
			}
		}
		
		// double 距离
		double dis(Segment other) const {
			try {
				double dis = func.disToLine(other.func);
				return dis;
			}
			catch (const std::invalid_argument& e) {
				throw std::invalid_argument("The two given segment are not parallel");
			}
		}
		
		bool isPointOnSegment(const Dot& point) const {
			double lineDist = func.disToLine(point);
			
			if (lineDist > microError)
				return false;
				
			double minX = std::min(vertex_a.getX(), vertex_b.getX());
			double maxX = std::max(vertex_a.getX(), vertex_b.getX());
			double minY = std::min(vertex_a.getY(), vertex_b.getY());
			double maxY = std::max(vertex_a.getY(), vertex_b.getY());
			return point.getX() >= minX && point.getX() <= maxX &&
			                       point.getY() >= minY && point.getY() <= maxY;
		}
		
		Dot getMidpoint() const {
			double midX = (vertex_a.getX() + vertex_b.getX()) / 2.0;
			double midY = (vertex_a.getY() + vertex_b.getY()) / 2.0;
			return Dot(midX, midY);
		}
		
		~Segment() {
		}
};

class Polygon {
		std::vector<Dot> vertexes;
		std::vector<Segment> edges;
		
		// 顶点排序方法（这里使用简单的排序，实际应用中可以使用更复杂的算法）
		void sortVertexes() {
			// 找到最左下角的点
			auto pivot = *std::min_element(vertexes.begin(), vertexes.end(), [](const Dot & a, const Dot & b) {
				if (a.getY() != b.getY()) {
					return a.getY() < b.getY();
				}
				
				return a.getX() < b.getX();
			});
			
			// 按极角排序
			std::sort(vertexes.begin(), vertexes.end(), [&pivot](const Dot & a, const Dot & b) {
				double angleA = std::atan2(a.getY() - pivot.getY(), a.getX() - pivot.getX());
				double angleB = std::atan2(b.getY() - pivot.getY(), b.getX() - pivot.getX());
				return angleA < angleB;
			});
		}
		
		// 过滤顶点的通用方法
		std::vector<Dot> filterPoints(const std::vector<Dot>& allPoints, const Polygon& other, bool insideBoth) const {
			std::vector<Dot> resultPoints;
			
			for (const auto& point : allPoints) {
				bool inThis = this->isPointInside(point) || isPointOnPolygonEdge(point);
				bool inOther = other.isPointInside(point) || other.isPointOnPolygonEdge(point);
				
				if ((insideBoth && inThis && inOther) || (!insideBoth && (inThis || inOther))) {
					resultPoints.push_back(point);
				}
			}
			
			return resultPoints;
		}
		
	public:
		Polygon() {
		}
		
		Polygon(std::vector<Dot> dots) { //从顶点构造(按顺序输入："ABCD"!="BDAC")
			vertexes = dots;
			size_t cnt = dots.size();
			
			for (size_t i = 0; i < cnt; i++) {
				edges.push_back({dots[i], dots[(i + 1) % cnt]});
			}
		}
		
		Polygon(std::vector<Segment> segment) { //从边构造
			edges = segment;
			size_t cnt = segment.size();
			
			for (size_t i = 0; i < cnt; i++) {
				try {
					vertexes.push_back(segment[i].intersection(segment[(i + 1) % cnt]));
				}
				catch (const std::invalid_argument& e) {
					throw std::invalid_argument("The given segments cannot form a polygon");
				}
			}
		}
		
		double getArea() const {
			double area = 0.0;
			size_t n = vertexes.size();
			
			for (size_t i = 0; i < n; ++i) {
				size_t j = (i + 1) % n;
				area += vertexes[i].getX() * vertexes[j].getY() - vertexes[j].getX() * vertexes[i].getY();
			}
			
			return std::abs(area) / 2.0;
		}
		
		std::vector<Dot> getVertexes() {
			return vertexes;
		}
		
		bool isPointInside(const Dot& point) const {
			bool inside = false;
			size_t n = vertexes.size();
			
			for (size_t i = 0, j = n - 1; i < n; j = i++) {
				if (((vertexes[i].getY() > point.getY()) != (vertexes[j].getY() > point.getY())) &&
				    (point.getX() < (vertexes[j].getX() - vertexes[i].getX()) * (point.getY() - vertexes[i].getY()) / (vertexes[j].getY() - vertexes[i].getY()) + vertexes[i].getX())) {
					inside = !inside;
				}
			}
			
			return inside;
		}
		
		// 周长
		double getPerimeter() const {
			double perimeter = 0.0;
			size_t n = vertexes.size();
			
			for (size_t i = 0; i < n; ++i) {
				size_t j = (i + 1) % n;
				perimeter += vertexes[i].distanceToOther(vertexes[j]);
			}
			
			return perimeter;
		}
		
		// 螺旋卡壳算法求多边形直径
		double rotatingCalipers() const {
			size_t n = vertexes.size();
			
			if (n < 2)
				return 0;
				
			if (n == 2)
				return vertexes[0].distanceToOther(vertexes[1]);
				
			double maxDist = 0;
			size_t j = 1;
			
			for (size_t i = 0; i < n; ++i) {
				while (true) {
					double dist1 = vertexes[i].distanceToOther(vertexes[j]);
					double dist2 = vertexes[i].distanceToOther(vertexes[(j + 1) % n]);
					
					if (dist2 >= dist1) {
						j = (j + 1) % n;
					}
					else {
						break;
					}
				}
				
				double dist = vertexes[i].distanceToOther(vertexes[j]);
				
				if (dist > maxDist) {
					maxDist = dist;
				}
			}
			
			return maxDist;
		}
		
		// 平移操作
		void translate(const Dot& translationVector) {
			for (auto& vertex : vertexes) {
				vertex.setX(vertex.getX() + translationVector.getX());
				vertex.setY(vertex.getY() + translationVector.getY());
			}
		}
		
		// 旋转操作，绕原点旋转
		void rotate(double angle) {
			double cosAngle = std::cos(angle);
			double sinAngle = std::sin(angle);
			
			for (auto& vertex : vertexes) {
				double newX = vertex.getX() * cosAngle - vertex.getY() * sinAngle;
				double newY = vertex.getX() * sinAngle + vertex.getY() * cosAngle;
				vertex.setX(newX);
				vertex.setY(newY);
			}
		}
		
		// 旋转操作，绕指定点旋转
		void rotate(double angle, const Dot& center) {
			// 先将中心点平移到原点
			translate(Dot(-center.getX(), -center.getY()));
			// 绕原点旋转
			rotate(angle);
			// 再将中心点平移回去
			translate(center);
		}
		
		// 辅助函数：检查点是否在多边形的边上
		bool isPointOnPolygonEdge(const Dot& point) const {
			const auto& edges = this->edges;
			
			for (const auto& edge : edges) {
				if (edge.isPointOnSegment(const_cast<Dot&>(point))) {
					return true;
				}
			}
			
			return false;
		}
		
		// 辅助函数：获取两个多边形所有边之间的交点
		std::vector<Dot> getIntersectionPoints(const Polygon& other) const {
			std::vector<Dot> intersectionPoints;
			const auto& edges1 = this->edges;
			const auto& edges2 = other.edges;
			
			for (const auto& edge1 : edges1) {
				for (const auto& edge2 : edges2) {
					try {
						Dot intersection = edge1.intersection(const_cast<Segment&>(edge2));
						intersectionPoints.push_back(intersection);
					}
					catch (const std::invalid_argument& e) {
						// 没有交点，继续下一组边
						continue;
					}
				}
			}
			
			return intersectionPoints;
		}
		
		// 多边形交集计算
		Polygon intersection(const Polygon& other) {
			std::vector<Dot> allPoints;
			const auto& vertexes1 = this->vertexes;
			const auto& vertexes2 = other.vertexes;
			auto intersectionPoints = getIntersectionPoints(other);
			// 添加原多边形顶点
			allPoints.insert(allPoints.end(), vertexes1.begin(), vertexes1.end());
			allPoints.insert(allPoints.end(), vertexes2.begin(), vertexes2.end());
			// 添加交点
			allPoints.insert(allPoints.end(), intersectionPoints.begin(), intersectionPoints.end());
			// 过滤出同时在两个多边形内部或边上的点
			std::vector<Dot> resultPoints = filterPoints(allPoints, other, true);
			// 顶点排序
			sortVertexes();
			return Polygon(resultPoints);
		}
		
		// 多边形并集计算
		Polygon polygonUnion(const Polygon& other) {
			std::vector<Dot> allPoints;
			const auto& vertexes1 = this->vertexes;
			const auto& vertexes2 = other.vertexes;
			auto intersectionPoints = getIntersectionPoints(other);
			// 添加原多边形顶点
			allPoints.insert(allPoints.end(), vertexes1.begin(), vertexes1.end());
			allPoints.insert(allPoints.end(), vertexes2.begin(), vertexes2.end());
			// 添加交点
			allPoints.insert(allPoints.end(), intersectionPoints.begin(), intersectionPoints.end());
			// 过滤出至少在一个多边形内部或边上的点
			std::vector<Dot> resultPoints = filterPoints(allPoints, other, false);
			// 顶点排序
			sortVertexes();
			return Polygon(resultPoints);
		}
		
		// 多边形差集计算
		Polygon difference(const Polygon& other) {
			std::vector<Dot> allPoints;
			const auto& vertexes1 = this->vertexes;
			auto intersectionPoints = getIntersectionPoints(other);
			// 添加原多边形顶点
			allPoints.insert(allPoints.end(), vertexes1.begin(), vertexes1.end());
			// 添加交点
			allPoints.insert(allPoints.end(), intersectionPoints.begin(), intersectionPoints.end());
			// 过滤出在当前多边形内部或边上，但不在另一个多边形内部或边上的点
			std::vector<Dot> resultPoints;
			
			for (const auto& point : allPoints) {
				if ((this->isPointInside(point) || isPointOnPolygonEdge(point)) &&
				    !(other.isPointInside(point) || other.isPointOnPolygonEdge(point))) {
					resultPoints.push_back(point);
				}
			}
			
			return Polygon(resultPoints);
		}
		
		~Polygon() {
		}
};

class rectangularCoordinateSystem {
		std::unordered_map<char, Dot> dots;
		std::unordered_map<std::string, Line> lines;
		std::unordered_map<std::string, Segment> segments;
		std::unordered_map<std::string, Polygon> polygons;
		
		void update() {
			// 更新 dots，把之前没有统计出的 lines 中线的交点放进来
			for (auto it1 = lines.begin(); it1 != lines.end(); ++it1) {
				for (auto it2 = std::next(it1); it2 != lines.end(); ++it2) {
					Line& line1 = it1->second;
					Line& line2 = it2->second;
					
					if (line1.isIntersect(line2)) {
						try {
							Dot intersection = line1.intersectionPoints(line2);
							bool exists = false;
							
							for (const auto& dot : dots) {
								if (dot.second.distanceToOther(intersection) < microError) { // 考虑浮点数误差
									exists = true;
									break;
								}
							}
							
							if (!exists) {
								// 生成一个新的点名称
								char newName = 'A';
								
								while (dots.find(newName) != dots.end()) {
									newName++;
								}
								
								dots[newName] = intersection;
								std::cout << "New intersection point added: " << newName << std::endl;
							}
						}
						catch (const std::invalid_argument& e) {
							// 处理平行直线的情况
							continue;
						}
					}
				}
			}
		}
		
	public:
		rectangularCoordinateSystem() {
			dots.clear();
			lines.clear();
			segments.clear();
			polygons.clear();
		}
		
		~rectangularCoordinateSystem() {}
		
		Dot getDot(char name) {
			return dots[name];
		}
		
		void addSegment(std::string name, Dot a, Dot b) {
			if (segments.find(name) != segments.end()) {
				// throw std::domain_error("your segment already exists");
				return ;
			}
			
			segments[name] = Segment(a, b);
		}
		
		void addPolygon(std::string name, std::vector<Dot> dots) {
			if (polygons.find(name) != polygons.end()) {
				// throw std::domain_error("your Polygon already exists");
				return ;
			}
			
			polygons[name] = Polygon(dots);
		}
		
		void addPolygon(std::string name, std::vector<Segment> lines) {
			if (polygons.find(name) != polygons.end()) {
				// throw std::domain_error("your Polygon already exists");
				return ;
			}
			
			polygons[name] = Polygon(lines);
		}
		
		void addDot(char name, std::pair<double, double> position) {
			if (dots.find(name) != dots.end()) {
				// throw std::domain_error("your point already exists");
				return ;
			}
			
			dots[name] = Dot(position.first, position.second);
			std::cout << "new dot:" << name << " ok." << std::endl;
		}
		
		void addLine(std::string name) {
			if (name.size() != 2 || !std::isupper(name[0]) || !std::isupper(name[1])) {
				// throw std::domain_error("your input format is a bit off");
				return ;
			}
			
			if (name[0] > name[1]) {
				std::swap(name[0], name[1]);
			}
			
			if (dots.find(name[0]) == dots.end() || dots.find(name[1]) == dots.end()) {
				throw std::domain_error("the point on which the line you entered depends does not exist");
			}
			
			if (lines.find(name) != lines.end()) {
				throw std::domain_error("the line you entered already exists");
			}
			
			lines[name] = Line(dots[name[0]], dots[name[1]]);
			std::cout << "new line: " << name << " ok." << std::endl;
		}
		
		double distanceOfDot(char a, char b) {
			if (dots.find(a) == dots.end() || dots.find(b) == dots.end()) {
				throw std::domain_error("one or both of your points do not exist");
			}
			
			double distance = dots[a].distanceToOther(dots[b]);
			return distance;
		}
		
		double distanceOfLine(std::string a, std::string b) {
			if (lines.find(a) == lines.end() || lines.find(b) == lines.end()) {
				throw std::domain_error("one or both of your lines do not exist");
			}
			
			double distance = lines[a].disToLine(lines[b]);
			return distance;
		}
		
		double getAngle(std::string a, std::string b) {
			if (lines.find(a) == lines.end() || lines.find(b) == lines.end()) {
				throw std::domain_error("one or both of your lines do not exist");
			}
			
			double angle = lines[a].angle(lines[b]);
			return angle;
		}
		
		char intersectionPoints(std::string a, std::string b) {
			if (lines.find(a) == lines.end() || lines.find(b) == lines.end()) {
				throw std::domain_error("one or both of your lines do not exist");
			}
			
			Dot point = lines[a].intersectionPoints(lines[b]);
			
			for (const auto& dot : dots) {
				if (dot.second.distanceToOther(point) < microError) {
					return dot.first;
				}
			}
			
			throw std::domain_error("Intersection point not found in existing dots");
		}
		
		void removeDot(char name) {
			auto it = dots.find(name);
			
			if (it != dots.end()) {
				dots.erase(it);
				std::cout << "Dot " << name << " removed." << std::endl;
			}
			else {
				throw std::domain_error("The dot you want to remove does not exist.");
			}
		}
		
		void removeLine(const std::string& name) {
			auto it = lines.find(name);
			
			if (it != lines.end()) {
				lines.erase(it);
				std::cout << "Line " << name << " removed." << std::endl;
			}
			else {
				throw std::domain_error("The line you want to remove does not exist.");
			}
		}
		
		void removeSegment(const std::string& name) {
			auto it = segments.find(name);
			
			if (it != segments.end()) {
				segments.erase(it);
				std::cout << "Segment " << name << " removed." << std::endl;
			}
			else {
				throw std::domain_error("The segment you want to remove does not exist.");
			}
		}
		
		void removePolygon(const std::string& name) {
			auto it = polygons.find(name);
			
			if (it != polygons.end()) {
				polygons.erase(it);
				std::cout << "Polygon " << name << " removed." << std::endl;
			}
			else {
				throw std::domain_error("The polygon you want to remove does not exist.");
			}
		}
		
		void modifyDot(char name, std::pair<double, double> newPosition) {
			auto it = dots.find(name);
			
			if (it != dots.end()) {
				it->second.setX(newPosition.first);
				it->second.setY(newPosition.second);
				std::cout << "Dot " << name << " modified." << std::endl;
			}
			else {
				throw std::domain_error("The dot you want to modify does not exist.");
			}
		}
		
		void modifyLine(const std::string& name, const Dot& point1, const Dot& point2) {
			auto it = lines.find(name);
			
			if (it != lines.end()) {
				it->second = Line(point1, point2);
				std::cout << "Line " << name << " modified." << std::endl;
			}
			else {
				throw std::domain_error("The line you want to modify does not exist.");
			}
		}
		
		void modifySegment(const std::string& name, const Dot& newVertexA, const Dot& newVertexB) {
			auto it = segments.find(name);
			
			if (it != segments.end()) {
				it->second = Segment(newVertexA, newVertexB);
				std::cout << "Segment " << name << " modified." << std::endl;
			}
			else {
				throw std::domain_error("The segment you want to modify does not exist.");
			}
		}
		
		void modifyPolygon(const std::string& name, const std::vector<Dot>& newVertexes) {
			auto it = polygons.find(name);
			
			if (it != polygons.end()) {
				it->second = Polygon(newVertexes);
				std::cout << "Polygon " << name << " modified." << std::endl;
			}
			else {
				throw std::domain_error("The polygon you want to modify does not exist.");
			}
		}
		
		double getPolygonArea(const std::string& name) {
			auto it = polygons.find(name);
			
			if (it != polygons.end()) {
				return it->second.getArea();
			}
			else {
				throw std::domain_error("The polygon you want to query does not exist.");
			}
		}
		
		double getPolygonPerimeter(const std::string& name) {
			auto it = polygons.find(name);
			
			if (it != polygons.end()) {
				return it->second.getPerimeter();
			}
			else {
				throw std::domain_error("The polygon you want to query does not exist.");
			}
		}
		
		void translatePolygon(const std::string& name, const Dot& translationVector) {
			auto it = polygons.find(name);
			
			if (it != polygons.end()) {
				it->second.translate(translationVector);
				std::cout << "Polygon " << name << " translated." << std::endl;
			}
			else {
				throw std::domain_error("The polygon you want to translate does not exist.");
			}
		}
		
		void rotatePolygon(const std::string& name, double angle, const Dot& center) {
			auto it = polygons.find(name);
			
			if (it != polygons.end()) {
				it->second.rotate(angle, center);
				std::cout << "Polygon " << name << " rotated." << std::endl;
			}
			else {
				throw std::domain_error("The polygon you want to rotate does not exist.");
			}
		}
		
		Polygon differentPolygon(std::string a_name, std::string b_name) {
			auto A = polygons.find(a_name);
			auto B = polygons.find(b_name);
			
			if (A != polygons.end() && B != polygons.end()) {
				return A->second.difference(B->second);
			}
			else {
				throw std::domain_error("The polygon you want to rotate does not exist.");
			}
		}
		
		Polygon intersectionPolygon(std::string a_name, std::string b_name) {
			auto A = polygons.find(a_name);
			auto B = polygons.find(b_name);
			
			if (A != polygons.end() && B != polygons.end()) {
				return A->second.intersection(B->second);
			}
			else {
				throw std::domain_error("The polygon you want to rotate does not exist.");
			}
		}
		
		Polygon polygonUnionPolygon(std::string a_name, std::string b_name) {
			auto A = polygons.find(a_name);
			auto B = polygons.find(b_name);
			
			if (A != polygons.end() && B != polygons.end()) {
				return A->second.polygonUnion(B->second);
			}
			else {
				throw std::domain_error("The polygon you want to rotate does not exist.");
			}
		}
		
		void printAllObject() {
			for (auto& dot : dots) {
				std::cout << "Dot: " << dot.first << " :(" << dot.second.getX() << "," << dot.second.getY() << ")" << std::endl;
			}
			
			for (auto& line : lines) {
				std::cout << "Line: " << line.first << " :";
				line.second.getExpression().printExpression();
				std::cout << std::endl;
			}
		}
};
