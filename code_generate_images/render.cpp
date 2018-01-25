#include "render.h"
#include "quaternion.h"
#include "Vector3D.h"
#include "angle.h"
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;
double PI = 3.1415926535898;

void Render::revise()
{
	Eigen::Vector3f point1;
	Eigen::Vector3f point2;
	Eigen::Vector3f point3;
	Eigen::Vector3f point4;

	// 计算平均值
	double sumx = 0;
	double sumy = 0;
	double sumz = 0;
	for (int i = 0; i < points.size();i++)
	{
		sumx += points[i](0);
		sumy += points[i](1);
		sumz += points[i](2);
	}

	// 利用平均值将车拆成四部分，每部分取最底下的那个点，我们认为那个点是车轮的底端
	double avgx = sumx / points.size();
	double avgz = sumz / points.size();
	point1(1) = FLT_MAX;
	point2(1) = FLT_MAX;
	point3(1) = FLT_MAX;
	point4(1) = FLT_MAX;
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i](0) > avgx && points[i](2) > avgz && points[i](1) < point1(1))
			point1 = points[i];
		if (points[i](0) > avgx && points[i](2) < avgz && points[i](1) < point2(1))
			point2 = points[i];
		if (points[i](0) < avgx && points[i](2) < avgz && points[i](1) < point3(1))
			point3 = points[i];
		if (points[i](0) < avgx && points[i](2) > avgz && points[i](1) < point4(1))
			point4 = points[i];
	}

	// 判断这四个点是否属于同一个平面,如果判断为否，说明四个车轮点选择错误，算法失效
	double judge = (point4(0) - point1(0))*((point2(1) - point1(1))*(point3(2) - point1(2)) - (point3(1) - point1(1))*(point2(2) - point1(2))) -
		(point4(1) - point1(1))*((point2(0) - point1(0))*(point3(2) - point1(2)) - (point3(0) - point1(0))*(point2(2) - point1(2))) +
		(point4(2) - point1(2))*((point2(0) - point1(0))*(point3(1) - point1(1)) - (point3(0) - point1(0))*(point2(1) - point1(1)));
	if (judge>0.000001)
		return;   // 不在同一平面

	// 计算平面法线以及与世界坐标系的z轴的夹角
	double plane_normal_x = (point2(1) - point1(1))*(point3(2) - point1(2)) - (point3(1) - point1(1))*(point2(2) - point1(2));
	double plane_normal_y = -((point2(0) - point1(0))*(point3(2) - point1(2)) - (point3(0) - point1(0))*(point2(2) - point1(2)));
	double plane_normal_z = (point2(0) - point1(0))*(point3(1) - point1(1)) - (point3(0) - point1(0))*(point2(1) - point1(1));
	double theta = acos(plane_normal_y / sqrt(plane_normal_x*plane_normal_x + plane_normal_y*plane_normal_y + plane_normal_z*plane_normal_z));
	cout << "theta: "<< theta * 180 / PI << endl; 
	if (theta * 180 / PI < 0.0001)
		return; // 不需要修复

	// 计算旋转轴的方向
	double rorate_axis_x = -plane_normal_z;
	double rorate_axis_y = 0;
	double rorate_axis_z = plane_normal_x;

	// 由于旋转的方向有2个,我们知道夹角和轴后,尝试绕其中一个方向旋转.如果旋转后夹角变小，说明旋转方向正确.
	
	Quaternion q(theta, Vector3D(rorate_axis_x, rorate_axis_y, rorate_axis_z));
	Quaternion q2(-theta, Vector3D(rorate_axis_x, rorate_axis_y, rorate_axis_z));
	Vector3D p1(point1(0), point1(1), point1(2));
	Vector3D p2(point2(0), point2(1), point2(2));
	Vector3D p3(point3(0), point3(1), point3(2));
	p1 = Rotate(p1, q);
	p2 = Rotate(p2, q);
	p3 = Rotate(p3, q);
	plane_normal_x = (p2.y - p1.y)*(p3.z - p1.z) - (p3.y - p1.y)*(p2.z - p1.z);
	plane_normal_y = -((p2.x - p1.x)*(p3.z - p1.z) - (p3.x - p1.x)*(p2.z - p1.z));
	plane_normal_z = (p2.x - p1.x)*(p3.y - p1.y) - (p3.x - p1.x)*(p2.y - p1.y);
	double theta2 = acos(plane_normal_y / sqrt(plane_normal_x*plane_normal_x + plane_normal_y*plane_normal_y + plane_normal_z*plane_normal_z));
	if (theta2 > theta)
		q = q2;
	cout << "theta2: "<<theta2 * 180 / PI << endl;

	
	// 开始旋转修正
	//Quaternion q(PI / 60, Vector3D(0.8, 0 ,1)); 
	for (int i = 0; i < points.size(); i++)
	{
		Vector3D p(points[i](0),points[i](1),points[i](2));
		Vector3D p1 = Rotate(p, q);
		points[i](0) = p1.x;
		points[i](1) = p1.y;
		points[i](2) = p1.z;
	}
	
	
}

void Render::save_obj()
{
	ofstream outfile;
	outfile.open("./model.obj");
	for (int i = 0; i < points.size(); i++)
	{
		Eigen::Vector3f temp_point = points[i];
		outfile << "v " << temp_point(0) << " " << temp_point(1) << " " << temp_point(2) << endl;
	}
	for (int i = 0; i < texture.size(); i++)
	{
		Eigen::Vector2f temp_point = texture[i];
		outfile << "vt " << temp_point(0) << " " << temp_point(1) << endl;
	}

	for (int i = 0; i < normals.size(); i++)
	{
		Eigen::Vector3f temp_point = normals[i];
		outfile << "vn " << temp_point(0) << " " << temp_point(1) << " " << temp_point(2) << endl;
	}

	for (int i = 0; i < faces.size(); i++)
	{
		face temp_face = faces[i];
		outfile << "f " << temp_face.point_index(0) + 1 << "/" << temp_face.texture_index(0) + 1 << "/" << temp_face.normal_index(0) + 1
			<< " " << temp_face.point_index(1) + 1 << "/" << temp_face.texture_index(1) + 1 << "/" << temp_face.normal_index(1) + 1
			<< " " << temp_face.point_index(2) + 1 << "/" << temp_face.texture_index(2) + 1 << "/" << temp_face.normal_index(2) + 1 << endl;
	}
	outfile.close();
}


void Render::load_mtl()
{
	std::fstream file(mtl.c_str(), std::ios::in);
	if (!file.is_open())
		std::cout << "mtl load wrong";
	std::string temp;
	while (getline(file, temp))
	{
		if (temp[0] == 'n')
		{
			mtl_ temp_mtl;
			std::string name, temp1;
			std::istringstream in(temp);
			in >> name >> name;

			getline(file, temp1);
			std::istringstream in1(temp1);
			in1 >> temp_mtl.mtl_name >> temp_mtl.Ka(0) >> temp_mtl.Ka(1) >> temp_mtl.Ka(2);

			getline(file, temp1);
			std::istringstream in2(temp1);
			in2 >> temp_mtl.mtl_name >> temp_mtl.Kd(0) >> temp_mtl.Kd(1) >> temp_mtl.Kd(2);

			getline(file, temp1);
			std::istringstream in3(temp1);
			in3 >> temp_mtl.mtl_name >> temp_mtl.Ks(0) >> temp_mtl.Ks(1) >> temp_mtl.Ks(2);
			temp_mtl.mtl_name = name;

			getline(file, temp1);
			if (temp1[0] == 'm')
			{
				std::istringstream in4(temp1);
				in4 >> temp_mtl.texture_map >> temp_mtl.texture_map;
			}
			mtls.push_back(temp_mtl);
		}
	}
	int id = 1;
	for (int i = 0; i < mtls.size(); i++)
	{
		if (mtls[i].texture_map.size() != 0)
		{
			mtls[i].texture_id = id;
			id++;
		}
	}
}

void Render::load_obj()
{
	std::fstream file(model_name.c_str(), std::ios::in);
	if (!file.is_open())
		std::cout << "mtl load wrong";
	std::string temp;
	std::string mtl_type;
	while (getline(file, temp))
	{
		if (temp[0] == 'u')
		{
			std::istringstream in(temp);
			in >> mtl_type >> mtl_type;
		}
		else if (temp[0] == 'v')
		{
			if (temp[1] == 't') {
				std::string a;
				Eigen::Vector2f temp_point;
				std::istringstream in(temp);
				in >> a >> temp_point(0) >> temp_point(1);
				texture.push_back(temp_point);
			}
			else if (temp[1] == 'n') {
				std::string a;
				Eigen::Vector3f temp_point;
				std::istringstream in(temp);
				in >> a >> temp_point(0) >> temp_point(1) >> temp_point(2);
				normals.push_back(temp_point);
			}
			else {
				std::string a;
				Eigen::Vector3f temp_point;
				std::istringstream in(temp);
				in >> a >> temp_point(0) >> temp_point(1) >> temp_point(2);
				points.push_back(temp_point);

				maxX = std::max<float>(maxX, temp_point(0));
				maxY = std::max<float>(maxY, temp_point(1));
				maxZ = std::max<float>(maxZ, temp_point(2));
				minX = std::min<float>(minX, temp_point(0));
				minY = std::min<float>(minY, temp_point(1));
				minZ = std::min<float>(minZ, temp_point(2));
			}
		}
		else if (temp[0] == 'f')
		{
			face temp_face;
			std::string a;
			for (int i = 0; i < temp.size(); i++)
			{
				if (temp[i] == '/') temp[i] = ' ';
			}
			std::istringstream in(temp);
			in >> a >> temp_face.point_index(0) >> temp_face.texture_index(0) >> temp_face.normal_index(0)
				>> temp_face.point_index(1) >> temp_face.texture_index(1) >> temp_face.normal_index(1)
				>> temp_face.point_index(2) >> temp_face.texture_index(2) >> temp_face.normal_index(2);
			temp_face.point_index(0) = temp_face.point_index(0) - 1; //-1是因为C++里面的索引从0开始,但是在obj里面从1开始
			temp_face.point_index(1) = temp_face.point_index(1) - 1;
			temp_face.point_index(2) = temp_face.point_index(2) - 1;
			temp_face.normal_index(0) -= 1;
			temp_face.normal_index(1) -= 1;
			temp_face.normal_index(2) -= 1;
			temp_face.texture_index(0) -= 1;
			temp_face.texture_index(1) -= 1;
			temp_face.texture_index(2) -= 1;
			//std::cout << temp_face.point_index << "\n";
			//std::cout << mtl_type << "\n";
			if (mtl_type.size() == 0) std::cout << "mtl wrong!" << std::endl;
			temp_face.mtl_index = mtl_type;
			Eigen::Vector3f mm; mm.setZero();
			temp_face.point_color.push_back(mm); temp_face.point_color.push_back(mm); temp_face.point_color.push_back(mm);
			faces.push_back(temp_face);
		}
	}
}



//--------------------------------------------------------------------------
//Useless Function


void Render::render_()
{
	for (int i = 0; i < faces.size(); i++)
	{
		Eigen::Vector3f kd, ks;
		for (int m = 0; m < mtls.size(); m++)
		{
			if (faces[i].mtl_index == mtls[m].mtl_name)
			{
				kd = mtls[m].Kd;
				ks = mtls[m].Ks;
			}
		}
		for (int j = 0; j < 3; j++)
		{
			for (int h = 0; h < light_position.size(); h++)
			{
				Eigen::Vector3f p;
				Eigen::Vector3f N = normals[faces[i].point_index(j)];
				p(0) = points[faces[i].point_index(j)](0);
				p(1) = points[faces[i].point_index(j)](1);
				p(2) = points[faces[i].point_index(j)](2);
				Eigen::Vector3f L = light_position[h] - p;
				L.normalize();
				Eigen::Vector3f V = camera_position - p;
				V.normalize();
				Eigen::Vector3f R = 2 * std::max(float(N.transpose() * L), float(0)) * N - L;
				Eigen::Vector3f diffuse = kd * std::max(float(N.transpose() * L), float(0)) * 1.0;
				Eigen::Vector3f specular = ks * std::max(float(pow(float(R.transpose() * V), 1.0)), float(0)) * 1.0;
				faces[i].point_color[j] += diffuse + specular;
			}

		}
	}
}

void Render::projection()
{
	cv::Mat pic(480, 640, CV_32FC4, cv::Scalar(1, 1, 1, -100));
	for (int t = 0; t < faces.size(); t++)
	{

		Eigen::Vector3f p1 = points[faces[t].point_index(0)];
		Eigen::Vector3f p2 = points[faces[t].point_index(1)];
		Eigen::Vector3f p3 = points[faces[t].point_index(2)];
		p1(0) = p1(0) / focal + 320.0;
		p2(0) = p2(0) / focal + 320.0;
		p3(0) = p3(0) / focal + 320.0;
		p1(1) = p1(1) / focal + 240.0;
		p2(1) = p2(1) / focal + 240.0;
		p3(1) = p3(1) / focal + 240.0;
		float minx = std::min(std::min(p1(0), p2(0)), p3(0));
		float maxx = std::max(std::max(p1(0), p2(0)), p3(0));
		float miny = std::min(std::min(p1(1), p2(1)), p3(1));
		float maxy = std::max(std::max(p1(1), p2(1)), p3(1));
		for (int i = int(minx); i < int(maxx + 1); i++)
		{
			for (int j = int(miny); j < int(maxy + 1); j++)
			{
				float lamda1 = ((p2(1) - p3(1)) * (i - p3(0)) + (p3(0) - p2(0)) * (j - p3(1))) /
					((p2(1) - p3(1)) * (p1(0) - p3(0)) + (p3(0) - p2(0)) * (p1(1) - p3(1)));
				float lamda2 = ((p3(1) - p1(1)) * (i - p3(0)) + (p1(0) - p3(0)) * (j - p3(1))) /
					((p2(1) - p3(1)) * (p1(0) - p3(0)) + (p3(0) - p2(0)) * (p1(1) - p3(1)));
				float lamda3 = 1.0 - lamda1 - lamda2;
				if (lamda1 >= 0 && lamda2 >= 0 && lamda3 >= 0)
				{
					float z = lamda1 * p1(2) + lamda2 * p2(2) + lamda3 * p3(2);
					if (z > pic.at<cv::Vec4f>(j, i)(3))
					{
						pic.at<cv::Vec4f>(j, i)(0) = lamda1 * faces[t].point_color[0](0) +
							lamda2 * faces[t].point_color[1](0) +
							lamda3 * faces[t].point_color[2](0);
						pic.at<cv::Vec4f>(j, i)(1) = lamda1 * faces[t].point_color[0](1) +
							lamda2 * faces[t].point_color[1](1) +
							lamda3 * faces[t].point_color[2](1);
						pic.at<cv::Vec4f>(j, i)(2) = lamda1 * faces[t].point_color[0](2) +
							lamda2 * faces[t].point_color[1](2) +
							lamda3 * faces[t].point_color[2](2);
						pic.at<cv::Vec4f>(j, i)(3) = z;
						//std::cout << j << " " << i << " " << pic.at<cv::Vec4f>(j, i)(0) << "\n";
					}
				}
			}
		}
	}


	cv::Mat a(480, 640, CV_32FC3, cv::Scalar(255, 255, 255));
	for (int i = 0; i < 480; i++)
	{
		for (int j = 0; j < 640; j++)
		{
			a.at<cv::Vec3f>(480 - i, j)(2) = pic.at<cv::Vec4f>(i, j)(0) * 255 * 1.0;
			//std::cout << pic.at<cv::Vec3f>(i, j)(2) << "\n";
			a.at<cv::Vec3f>(480 - i, j)(1) = pic.at<cv::Vec4f>(i, j)(1) * 255 * 1.0;
			a.at<cv::Vec3f>(480 - i, j)(0) = pic.at<cv::Vec4f>(i, j)(2) * 255 * 1.0;
		}
	}
	cv::imwrite("../3.jpg", a);
	std::cout << "writen " << "\n";
}
