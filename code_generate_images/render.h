#pragma once

#include <fstream>
#include <opencv2/opencv.hpp>
//#include <eigen3/Eigen/Dense>
#include <Eigen\Dense>
#include <sstream>

struct mtl_ {
	std::string mtl_name;
	Eigen::Vector3f Ka;
	Eigen::Vector3f Kd;
	Eigen::Vector3f Ks;
	std::string texture_map;
	int texture_id = -1;
};


struct face {
	Eigen::Vector3i point_index;
	Eigen::Vector3i normal_index;
	Eigen::Vector3i texture_index;
	std::vector<Eigen::Vector3f> point_color;
	std::string mtl_index;
};

class Render
{
public:
	Render(std::string name1, std::string name2) {
		mtl = name1;
		model_name = name2;
		camera_position(0) = 0.0;
		camera_position(1) = 0.5;
		camera_position(2) = 1.0;
		Eigen::Vector3f temp_light;
		temp_light(0) = 0.4;
		temp_light(1) = 1.0;
		temp_light(2) = 1.0;
		light_position.push_back(temp_light);
		temp_light(0) = -0.4;
		temp_light(1) = 1.0;
		temp_light(2) = 1.0;
		light_position.push_back(temp_light);
		/*temp_light(0) = -0.4;
		temp_light(1) = 1.0;
		temp_light(2) = -1.0;
		light_position.push_back(temp_light);
		temp_light(0) = 0.4;
		temp_light(1) = 1.0;
		temp_light(2) = -1.0;
		light_position.push_back(temp_light);*/
		focal = 0.0019;
	}
	void load_mtl();
	void load_obj();
	void render_();
	void projection();
	void revise();
	void save_obj();

public:
	float theta1;
	float theta2;
	float theta3;
	float focal;
	std::string mtl, model_name;
	std::vector<mtl_> mtls;
	std::vector<Eigen::Vector3f> points;
	std::vector<Eigen::Vector3f> normals;
	std::vector<Eigen::Vector2f> texture;
	std::vector<face> faces;
	Eigen::Vector3f camera_position;
	std::vector<Eigen::Vector3f>  light_position;

	float minX = FLT_MAX, maxX = FLT_MIN;
	float minY = FLT_MAX, maxY = FLT_MIN;
	float minZ = FLT_MAX, maxZ = FLT_MIN;



};
