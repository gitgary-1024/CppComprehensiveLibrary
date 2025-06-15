#pragma once
#include<bits/stdc++.h>
class PointMass;
class RigidBody;
#include"../Objects/RigidBody/RigidBody.h"
class Rope;
#include"../Objects/Rope/Rope.h"
class Spring;
#include"../Objects/Spring/Spring.h"
#include"../Constants.h"
#include"../../Geometry/CoordinateSystem/CoordinateSystem.h"

class PhysicsWorld {
		std::unordered_map<std::string, RigidBody> RigidBodies;
		std::unordered_map<std::string, Rope> Ropes;
		std::unordered_map<std::string, Spring> Springs;
	public:
		PhysicsWorld() {}
		
		~PhysicsWorld() {}
};
