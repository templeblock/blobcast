#include "Level.h"
#include <json.hpp>
#include <fstream>

Level::~Level()
{
	Clear();
}

Level::Level(const Level& other)
{
	*this = other;
}

Level& Level::operator=(const Level& other)
{
	Clear();
	for (RigidBody *r : Objects)
		Objects.push_back(new RigidBody(*r));
	return *this;
}

Level::Level(Level&& other)
{
	*this = std::move(other);
}

Level& Level::operator=(Level&& other)
{
	if (this != &other)
	{
		Clear();
		Objects = std::move(other.Objects);
	}
	return *this;
}

std::size_t Level::AddBox(
		glm::vec3 position,
		glm::quat orientation,
		glm::vec3 dimensions,
		glm::vec4 color,
		float mass)
{
	Mesh *box(Mesh::CreateCubeWithNormals());
	RigidBody *r =
		new RigidBody(box, Shape::Box, position,
			orientation, dimensions, color, mass);
	Objects.push_back(r);
	return Objects.size() - 1;
}

std::size_t Level::AddCylinder(
	glm::vec3 position,
	glm::quat orientation,
	glm::vec3 dimensions,
	glm::vec4 color,
	float mass)
{
	Mesh *cylinder(Mesh::CreateCylinderWithNormals());
	RigidBody *r =
		new RigidBody(cylinder, Shape::Cylinder, position,
			orientation, dimensions, color, mass);
	Objects.push_back(r);
	return Objects.size() - 1;
}

void Level::Delete(std::size_t index)
{
	Objects.erase(Objects.begin() + index);
}

void Level::Clear()
{
	for (RigidBody *r : Objects)
		delete r;
	Objects.clear();
}

int Level::Find(btRigidBody *r)
{
	for (std::size_t i = 0, n = Objects.size(); i < n; i++)
	{
		if (Objects[i]->rigidbody == r)
			return i;
	}
	return -1;
}

void Level::Render(GLuint uMMatrix, GLuint uColor)
{
	for (RigidBody *r : Objects)
	{
		glUniformMatrix4fv(uMMatrix, 1, GL_FALSE, &r->GetModelMatrix()[0][0]);
		glUniform4fv(uColor, 1, &r->color.r);
		r->Render();
	}
}

void Level::Serialize(std::string file)
{
	std::ofstream f(file);
	nlohmann::json objects;
	for (RigidBody *r : Objects)
	{
		nlohmann::json object;

		glm::vec3 translation = r->GetTranslation();
		glm::quat orientation = r->GetOrientation();
		glm::vec3 scale = r->GetScale();

		if(r->shapeType == Shape::Box)
			object["type"] = "box";
		else
			object["type"] = "cylinder";
		object["position"] = {
			translation.x, translation.y,
			translation.z };
		object["orientation"] = {
			orientation.w, orientation.x,
			orientation.y, orientation.z };
		object["dimensions"] = {
			scale.x, scale.y, scale.z };
		object["color"] = {
			r->trueColor.r, r->trueColor.g, r->trueColor.b, r->trueColor.a };
		object["mass"] = r->mass;
		if (!r->motion.Points.empty())
		{
			nlohmann::json path;
			path["speed"] = r->motion.Speed;
			for (auto v = r->motion.Points.begin();
					v != r->motion.Points.end(); ++v)
				path["points"].push_back({ v->x, v->y, v->z });
			object["path"] = path;
		}
		objects.push_back(object);
	}
	nlohmann::json level;
	level["objects"] = objects;
	f << std::setw(2) << level << std::endl;
	f.close();
}

Level *Level::Deserialize(std::string file)
{
	std::ifstream f(file);
	if (!f.is_open())
		return nullptr;
	std::string s(
			(std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
	auto data = nlohmann::json::parse(s);
	Level *level = new Level();
	for (auto object : data["objects"])
	{
		auto j_pos = object["position"];
		glm::vec3 position(j_pos[0], j_pos[1], j_pos[2]);
		auto j_ori = object["orientation"];
		glm::quat orientation(j_ori[0], j_ori[1], j_ori[2], j_ori[3]);
		auto j_dim = object["dimensions"];
		glm::vec3 dimensions(j_dim[0], j_dim[1], j_dim[2]);
		auto j_col = object["color"];
		glm::vec4 color(j_col[0], j_col[1], j_col[2], j_col[3]);
		auto mass = object["mass"];
		auto path = object["path"];
		std::size_t i;
		if (object["type"] == "box")
			i = level->AddBox(position, orientation, dimensions, color, mass);
		else if (object["type"] == "cylinder")
			i = level->AddCylinder(position, orientation, dimensions, 
				color, mass);
		if (!path.is_null())
		{
			RigidBody *r = level->Objects[i];
			auto speed = path["speed"];
			auto points = path["points"];
			r->motion.Speed = speed;
			for (auto point : points)
				r->motion.Points.insert(
						r->motion.Points.end(),
						glm::vec3(point[0], point[1], point[2]));
		}
	}
	f.close();
	return level;
}
