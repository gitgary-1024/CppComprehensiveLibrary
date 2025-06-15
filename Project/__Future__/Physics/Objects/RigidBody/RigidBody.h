#pragma once
#include"../../Constants.h"
#include"../../../Geometry/CoordinateSystem/CoordinateSystem.h"
#include"../../../Geometry/Vector/Vector.h"
#include"../FixedPoint/FixedPoint.h"
#include"../Rope/Rope.h"
#include"../Spring/Spring.h"

class PointMass;
class RigidBody;

class PointMass {
		Dot location;
		Vector speed;
		double mass;
	public:
		PointMass() {
		}
		
		PointMass(Dot location, double mass = 1, Vector speed = Vector({0, 0})) {
			this->location = location;
			this->mass = mass;
			this->speed = speed;
		}
		
		~PointMass() {}
		
		bool update(std::vector<PointMass> PointMasses, std::vector<RigidBody> RigidBodies, std::vector<Rope> Ropes, std::vector<Spring> Springs, double dt) {
			double Vx = speed.getVector()[0];
			double Vy = speed.getVector()[1];
			location = {Vx*dt + location.getX(), Vy*dt + location.getY()};
			
			for (const PointMass& other : PointMasses) {
				if (isCollision(other)) {
					CollisionWith(other);
				}
			}
			
			for (const RigidBody& other : RigidBodies) {
				if (isCollision(other)) {
					CollisionWith(other);
				}
			}
			
			for (const Rope& other : Ropes) {
				if (isCollision(other)) {
					CollisionWith(other);
				}
			}
			
			for (const Spring& other : Springs) {
				if (isCollision(other)) {
					CollisionWith(other);
				}
			}
			
			for (const PointMass& other : PointMasses) {
				if (isCollision(other)) {
					return 1;
				}
			}
			
			for (const RigidBody& other : RigidBodies) {
				if (isCollision(other)) {
					return 1;
				}
			}
			
			for (const Rope& other : Ropes) {
				if (isCollision(other)) {
					return 1;
				}
			}
			
			for (const Spring& other : Springs) {
				if (isCollision(other)) {
					return 1;
				}
			}
			
			return 0;
		}
		
		bool isCollision(const PointMass& other) {
		}
		
		bool isCollision(const RigidBody& other) {
		}
		
		bool isCollision(const Rope& other) {
		}
		
		bool isCollision(const Spring& other) {
		}
		
		void CollisionWith(const PointMass& other) {
		}
		
		void CollisionWith(const RigidBody& other) {
		}
		
		void CollisionWith(const Rope& other) {
		}
		
		void CollisionWith(const Spring& other) {
		}
};

class RigidBody {
		std::unordered_map<std::string, PointMass> vertex;
		Vector speed;
		double mass;
	public:
		RigidBody() {
		}
		
		~RigidBody() {
		}
		
		bool update(std::vector<PointMass> PointMasses, std::vector<RigidBody> RigidBodies, std::vector<Rope> Ropes, std::vector<Spring> Springs, double dt) {
		}
		
		bool isCollision(const PointMass& other) {
		}
		
		bool isCollision(const RigidBody& other) {
		}
		
		bool isCollision(const Rope& other) {
		}
		
		bool isCollision(const Spring& other) {
		}
		
		void CollisionWith(const PointMass& other) {
		}
		
		void CollisionWith(const RigidBody& other) {
		}
		
		void CollisionWith(const Rope& other) {
		}
		
		void CollisionWith(const Spring& other) {
		}
};

