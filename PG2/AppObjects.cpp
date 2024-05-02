#include <opencv2/opencv.hpp>

#include "App.hpp"

#define print(x) std::cout << x << "\n"

Model* App::CreateModel(std::string name, std::string obj, std::string tex, bool is_opaque, glm::vec3 position, glm::vec3 scale, glm::vec4 rotation, bool collision)
{
	print("Loading " << name << ":");
	std::filesystem::path modelpath("./resources/objects/" + obj);
	std::filesystem::path texturepath("./resources/textures/" + tex);
	auto model = new Model(name, modelpath, texturepath);

	model->position = position;
	model->scale = scale;
	model->init_rotation = rotation;
	model->rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	if (is_opaque) {
		scene_opaque.insert({ name, model});
	}
	else {
		scene_transparent.insert({ name, model });
	}

	if (collision) {
		collisions.push_back(model);
	}

	return model;
}

// Load models, load textures, load shaders, initialize level, etc.
void App::InitAssets()
{
	print("RAM OK\nROM OK");
	// == SHADERS ==
	// Load shaders and create ShaderProgram
	std::filesystem::path VS_path("./resources/shaders/uber.vert");
	std::filesystem::path FS_path("./resources/shaders/uber.frag");
	my_shader = ShaderProgram(VS_path, FS_path);

	// == MODELS ==
	glm::vec3 position{};
	glm::vec3 scale{};
	glm::vec4 rotation{};

	// = OPAQUE MODELS =
	// Jukebox
	position = glm::vec3(4.0f, 0.0f, 8.0f);
	scale = glm::vec3(0.125f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	//CreateModel("obj_jukebox", "jukebox.obj", "jukebox.jpg", true, position, scale, rotation, true);
	obj_jukebox = CreateModel("obj_jukebox", "bunny_tri_vnt.obj", "jukebox.jpg", true, position, scale, rotation, true); // for testing (faster loading)	
	// Table
	position = glm::vec3(1.0f, 0.0f, 6.0f);
	scale = glm::vec3(0.015f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	CreateModel("obj_table", "table.obj", "table.png", true, position, scale, rotation, true);
	// Projectiles
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	scale = glm::vec3(0.125f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	for (int i = 0; i < N_PROJECTILES; i++) {
		auto name = "obj_projectile_" + std::to_string(i);
		auto obj_projectile_x = CreateModel(name, "sphere_tri_vnt.obj", "Red.png", true, position, scale, rotation, false);
		projectiles[i] = obj_projectile_x;
		position.x++;
	}

	// = TRANSPARENT MODELS =
	// Cubes
	scale = glm::vec3(0.5f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	position = glm::vec3(0.2f, 1.0f, 6.0f);
	CreateModel("obj_glass_cube_r", "cube_triangles_normals_tex.obj", "Red.png", false, position, scale, rotation, true);
	position.x += 0.8f;
	CreateModel("obj_glass_cube_g", "cube_triangles_normals_tex.obj", "Green.png", false, position, scale, rotation, true);
	position.x += 0.8f;
	CreateModel("obj_glass_cube_b", "cube_triangles_normals_tex.obj", "Blue.png", false, position, scale, rotation, true);
	// Testing sphere
	/*
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(0.2f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	CreateModel("obj_sphere", "sphere_tri_vnt.obj", "Green.png", false, position, scale, rotation, false);
	/**/

	// == HEIGHTMAP ==
	print("Loading heightmap:");
	std::filesystem::path heightspath("./resources/textures/heights.png");
	std::filesystem::path texturepath("./resources/textures/tex_256.png");
	obj_heightmap = new Model("heightmap", heightspath, texturepath, true);
	obj_heightmap->position = glm::vec3(-HEIGHTMAP_SHIFT, 0.0f, -HEIGHTMAP_SHIFT);
	obj_heightmap->scale = glm::vec3(HEGHTMAP_SCALE);
	obj_heightmap->init_rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	obj_heightmap->rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	scene_opaque.insert({ "obj_heightmap", obj_heightmap });

	// == for TRANSPARENT OBJECTS sorting ==	
	for (auto i = scene_transparent.begin(); i != scene_transparent.end(); i++) {
		scene_transparent_pairs.push_back(&*i);
	}
}

#define JUKEBOX_SPEED 2.0f

void App::UpdateModels(float delta_time)
{
	glm::vec3 position{};
	glm::vec3 scale{};
	glm::vec4 rotation{};

	// Glass cubes rotation
	scene_transparent.find("obj_glass_cube_r")->second->rotation = glm::vec4(0.0f, 1.0f, 0.0f, 23 * glfwGetTime());
	scene_transparent.find("obj_glass_cube_g")->second->rotation = glm::vec4(0.0f, 1.0f, 0.0f, 45 * glfwGetTime());
	scene_transparent.find("obj_glass_cube_b")->second->rotation = glm::vec4(0.0f, 1.0f, 0.0f, 90 * glfwGetTime());

	// JUKEBOX
	// - rotate towards player
	float angles = glm::degrees(atan2(-jukebox_to_player.y, jukebox_to_player.x)) + 90;
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, angles);
	obj_jukebox->rotation = rotation;
	// - move towards player
	if (glm::length(jukebox_to_player) > 2.0f) {
		position = obj_jukebox->position;
		position.x += jukebox_to_player_n.x * delta_time * JUKEBOX_SPEED;
		position.z += jukebox_to_player_n.y * delta_time * JUKEBOX_SPEED;
		position.x = std::clamp(position.x, -3.0f, 7.0f);
		position.z = std::clamp(position.z, 0.0f, 14.0f);
		obj_jukebox->position = position;
	}

	// --------------------
	// Bounding sphere test
	/*
	float sphere_scale = 0.125f;
	//sphere_scale = 0.5f;
	//obj_jukebox = scene_transparent.find("obj_glass_cube_b")->second;
	position = obj_jukebox->position +sphere_scale * obj_jukebox->coll_center;
	scale = glm::vec3(sphere_scale * obj_jukebox->coll_radius);
	scene_transparent.find("obj_sphere")->second->position = position;
	scene_transparent.find("obj_sphere")->second->scale = scale;
	/**/
}
