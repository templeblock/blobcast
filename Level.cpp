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
		new RigidBody(box, position, orientation, dimensions, color, mass);
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

		object["type"] = "box";
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
		if (!r->path_points.empty())
		{
			nlohmann::json path;
			path["speed"] = r->path_speed;
			for (glm::vec3 v : r->path_points)
				path["points"].push_back({ v.x, v.y, v.z });
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
		std::size_t i =
			level->AddBox(position, orientation, dimensions, color, mass);
		if (!path.is_null())
		{
			RigidBody *r = level->Objects[i];
			auto speed = path["speed"];
			auto points = path["points"];
			r->path_speed = speed;
			for (auto point : points)
				r->path_points.push_back(
						glm::vec3(point[0], point[1], point[2]));
			r->path_tangents = CatmullRomTangents(r->path_points);
		}
	}
	f.close();
	return level;
}

std::vector<glm::vec3> Level::CatmullRomTangents(
		const std::vector<glm::vec3>& points)
{
	int num_points = points.size();
	std::vector<glm::vec3> tangents(num_points);
	if (!points.empty())
	{
		if (num_points == 2)
		{
			tangents[0] = points[1] - points[0];
			tangents[1] = tangents[0];
		}
		else if (num_points >= 3)
		{
			for (int i = 1; i < num_points - 1; i++)
			{
				tangents.push_back(points[i + 1] - points[i - 1]);
			}
			tangents[0] = tangents[1];
			tangents[num_points - 1] = tangents[num_points - 2];
		}
	}
	return tangents;
}
