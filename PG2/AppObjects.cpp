#include <opencv2/opencv.hpp>

#include "App.hpp"

#define print(x) std::cout << x << "\n"

void App::CreateModel(std::string name, std::string obj, std::string tex, bool is_opaque, glm::vec3 position, glm::vec3 scale, glm::vec4 rotation)
{
	print("Loading " << name << ":");
	std::filesystem::path modelpath("./resources/objects/" + obj);
	std::filesystem::path texturepath("./resources/textures/" + tex);
	auto model = Model(modelpath, texturepath);

	model.position = position;
	model.scale = scale;
	model.init_rotation = rotation;
	model.rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	if (is_opaque) {
		scene_opaque.insert({ name, model});
	}
	else {
		scene_transparent.insert({ name, model });
	}
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

	// BUNNY
	position = glm::vec3(1.0f, 1.0f, 1.0f);
	scale = glm::vec3(0.5f, 0.5f, 0.5f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	CreateModel("obj_bunny", "bunny_tri_vnt.obj", "box_rgb888.png", true, position, scale, rotation);
	
	// TEAPOT
	position = glm::vec3(2.0f, 2.0f, 2.0f);
	scale = glm::vec3(0.2f, 0.2f, 0.2f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	CreateModel("obj_teapot", "teapot_tri_vnt.obj", "Glass.png", false, position, scale, rotation);
	
	// GREEN BALL
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(0.2f, 0.2f, 0.2f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	CreateModel("obj_sphere_0", "sphere_tri_vnt.obj", "Green.png", true, position, scale, rotation);

	// GLASS PANE
	position = glm::vec3(0.0f, 3.0f, 4.0f);
	rotation = glm::vec4(1.0f, 0.0f, 0.0f, 90.0f);
	CreateModel("obj_glass", "plane_tri_vnt.obj", "Glass.png", false, position, scale, rotation);
	position = glm::vec3(1.0f, 4.0f, 3.0f);
	rotation = glm::vec4(1.0f, 0.0f, 0.0f, 90.0f);
	CreateModel("obj_glass2", "plane_tri_vnt.obj", "Glass.png", false, position, scale, rotation);

	// JUKEBOX
	position = glm::vec3(4.0f, 0.0f, 8.0f);
	scale = glm::vec3(0.125f, 0.125f, 0.125f);
	rotation = glm::vec4(1.0f, 0.0f, 0.0f, -90.0f);
	CreateModel("obj_jukebox", "jukebox.obj", "jukebox.jpg", true, position, scale, rotation);

	// == HEIGHTMAP ==
	print("Loading heightmap:");
	std::filesystem::path heightspath("./resources/textures/heights.png");
	std::filesystem::path texturepath("./resources/textures/tex_256.png");
	auto model = Model(heightspath, texturepath, true);
	model.position = glm::vec3(-HEIGHTMAP_SHIFT, 0.0f, -HEIGHTMAP_SHIFT);
	model.scale = glm::vec3(HEGHTMAP_SCALE, HEGHTMAP_SCALE, HEGHTMAP_SCALE);
	model.init_rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	model.rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	scene_opaque.insert({ "heightmap", model });

	// == for TRANSPARENT OBJECTS sorting ==	
	for (auto i = scene_transparent.begin(); i != scene_transparent.end(); i++) {
		scene_transparent_pairs.push_back(&*i);
	}
}

void App::UpdateModels()
{
	glm::vec3 position{};
	glm::vec3 scale{};
	glm::vec4 rotation{};

	// rotate the teapot
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 45 * glfwGetTime());
	scene_transparent.find("obj_teapot")->second.rotation = rotation;
	
	// rotate the jukebox
	/*
	rotation = glm::vec4(0.0f, 0.0f, 1.0f, 45 * glfwGetTime());
	scene_opaque.find("obj_jukebox")->second.rotation = rotation;
	/**/

	// move green ball
	scene_opaque.find("obj_sphere_0")->second.position = ball_position;
}
