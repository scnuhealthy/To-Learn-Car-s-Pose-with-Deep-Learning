#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <chrono>
#include "render.h"
#include "time.h"

using namespace std;

#define _P_I_ 3.1415926
std::default_random_engine gen;
std::normal_distribution<double> distribution(0, 22);  //框中心偏移量服从的正态分布

GLfloat elevation;
GLfloat azimuth;
GLfloat rotation;
//The anchor point which we define
float x1;
float x2;
float x3;
//相机在世界坐标的位置
GLfloat camera_x;
GLfloat camera_y;
GLfloat camera_z;
//相机镜头对准的物体在世界坐标的位置
GLfloat cam_refx;
GLfloat cam_refy;
GLfloat cam_refz;
//相机向上的方向在世界坐标中的方向
GLfloat upx;
GLfloat upy;
GLfloat upz;
//你把相机想象成为你自己的脑袋：
//第一组数据就是脑袋的位置
//第二组数据就是眼睛看的物体的位置
//第三组就是头顶朝向的方向（因为你可以歪着头看同一个物体）


inline void loadCarList(const std::string file, vector<pair<string, double>>& car)
{
	char fields[2][256];

	ifstream fin(file);
	if (!fin.is_open())
	{
		std::cout << "Unable to open file: " << file << endl;
		//std::exit(-1);
	}

	char line[256];
	while (!fin.eof())
	{
		if (fin.getline(line, 256).good())
		{
			char tmp[256];
			int pos = 0;
			int indicator = 0;
			for (int i = 0; i < strlen(line) + 1; i++)
			{
				if (line[i] != ',' && line[i] != '\0')
				{
					tmp[pos++] = line[i];
				}
				else
				{
					tmp[pos] = '\0';
					strcpy(fields[indicator], tmp);
					indicator++;
					pos = 0;
				}
			}

			string name = fields[0];
			double scale = atof(fields[1]);
			car.push_back(make_pair(name, scale));
		}
		else
			break;
	}
	fin.close();
}

inline vector<pair<string, double>> ReadCarList(string filename)
{
	vector<pair<string, double>> names;
	ifstream fin(filename.c_str());
	if (!fin.is_open())
	{
		std::cout << "Unable to open file: " << filename << endl;
		//exit(-1);
	}
	while (!fin.eof())
	{
		string name;
		double scale;
		fin >> name >> scale;
		if (name.compare("") == 0)
		{
			break;
		}
		names.push_back(pair<string, double>(name, scale));
	}
	fin.close();
	return names;
}

//-----------------------------Update the camera parameters-----------------------------------//
inline void update_azimuth()
{
	float x = camera_x, z = camera_z;
	camera_z = z * std::cos(azimuth) - x * std::sin(azimuth);
	camera_x = z * std::sin(azimuth) + x * std::cos(azimuth);

	float x1 = upx, z1 = upz;
	upz = z1 * std::cos(azimuth) - x1 * std::sin(azimuth);
	upx = z1 * std::sin(azimuth) + x1 * std::cos(azimuth);
}

inline void update_rotation()
{
	float y = upy;
	float x = upx;
	upx = x * std::cos(-rotation) - y * std::sin(-rotation);
	upy = x * std::sin(-rotation) + y * std::cos(-rotation);
}

inline void update_elevation()
{
	float y1 = camera_y - cam_refy;
	float z1 = camera_z - cam_refz;
	camera_z = z1 * std::cos(elevation) - y1 * std::sin(elevation);
	camera_y = z1 * std::sin(elevation) + y1 * std::cos(elevation);
	float y = upy; float z = upz;
	upz = -(z * std::cos(-elevation) - y * std::sin(-elevation));
	upy = (z * std::sin(-elevation) + y * std::cos(-elevation));
	//std::cout << camera_z << " " << camera_y << " " << upz << " " << upy << "\n";
}

inline void UpdateCamera()
{
	update_rotation();
	update_elevation();
	update_azimuth();
	gluLookAt(camera_x, camera_y, camera_z, cam_refx, cam_refy, cam_refz, upx, upy, upz);
}

inline void update_points(Render& a)
{
	for (int i = 0; i < a.points.size(); i++)
	{
		float x = a.points[i](0);
		float z = a.points[i](2);
		a.points[i](0) = x * std::cos(rotation) - z * std::sin(rotation);
		a.points[i](2) = x * std::sin(rotation) + z * std::cos(rotation);
	}
	x1 = x1 * std::cos(rotation) - x3 * std::sin(rotation);
	x3 = x1 * std::sin(rotation) + x3 * std::cos(rotation);
}

//------------------------------Load models and set the environment----------------------------------//
inline void init()
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	glClearColor(255.0, 255.0, 255.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//inline void LoadTexture(Render& a)
//{
//	for (int i = 0; i < a.mtls.size(); i++)
//	{
//		if (a.mtls[i].texture_map.size() != 0)
//		{
//			std::string name = a.mtls[i].texture_map;
//			name = "../model/" + name;
//			//std::cout << name << "\n";
//			cv::Mat I = cv::imread(name);
//			int pixellength = I.cols * I.rows * 3;
//			GLubyte* pixels = new GLubyte[pixellength];
//			memcpy(pixels, I.data, pixellength * sizeof(char));
//			GLuint texture_ID;
//			glGenTextures(1, &texture_ID);
//			glBindTexture(GL_TEXTURE_2D, texture_ID);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, I.cols, I.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
//			free(pixels);
//		}
//	}
//}

void LoadModel(Render& a)
{
	for (int i = 0; i < a.faces.size(); i++)
	{
		mtl_ temp_mtl;
		for (int m = 0; m < a.mtls.size(); m++)
		{
			//std::cout << a.mtls[m].texture_id <<"\n";
			if (a.faces[i].mtl_index == a.mtls[m].mtl_name)
			{
				temp_mtl = a.mtls[m];
			}
		}
		glBegin(GL_TRIANGLES);
		GLfloat mat_specular[] = { temp_mtl.Ks(0), temp_mtl.Ks(1), temp_mtl.Ks(2), 1.0 };
		GLfloat mat_diffuse[] = { temp_mtl.Kd(0), temp_mtl.Kd(1), temp_mtl.Kd(2), 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		for (int j = 0; j < 3; j++)
		{
			if (temp_mtl.texture_id == -1) 
			{
				glNormal3f(GLfloat(a.normals[a.faces[i].normal_index[j]](0)),
					GLfloat(a.normals[a.faces[i].normal_index[j]](1)),
					GLfloat(a.normals[a.faces[i].normal_index[j]](2)));
				glVertex3f(GLfloat(a.points[a.faces[i].point_index[j]](0)),
					GLfloat(a.points[a.faces[i].point_index[j]](1)),
					GLfloat(a.points[a.faces[i].point_index[j]](2)));
			}
		}
	}
	glEnd();

	//for (int i = 0; i < a.faces.size(); i++)
	//{
	//	mtl_ temp_mtl;
	//	for (int m = 0; m < a.mtls.size(); m++)
	//	{
	//		//std::cout << a.mtls[m].texture_id <<"\n";
	//		if (a.faces[i].mtl_index == a.mtls[m].mtl_name)
	//		{
	//			temp_mtl = a.mtls[m];
	//		}
	//	}
	//	glEnable(GL_TEXTURE_2D);
	//	if (temp_mtl.texture_id != -1)
	//	{
	//		glBindTexture(GL_TEXTURE_2D, GLuint(temp_mtl.texture_id));
	//	}
	//	glBegin(GL_TRIANGLES);
	//	GLfloat mat_specular[] = { temp_mtl.Ks(0), temp_mtl.Ks(1), temp_mtl.Ks(2), 1.0 };
	//	GLfloat mat_diffuse[] = { temp_mtl.Kd(0), temp_mtl.Kd(1), temp_mtl.Kd(2), 1.0 };
	//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//	for (int j = 0; j < 3; j++)
	//	{
	//		if (temp_mtl.texture_id != -1)
	//		{
	//			glBindTexture(GL_TEXTURE_2D, temp_mtl.texture_id);
	//			glTexCoord2f(GLfloat(a.texture[a.faces[i].texture_index[j]](0)),
	//				GLfloat(-a.texture[a.faces[i].texture_index[j]](1)));
	//			glNormal3f(GLfloat(a.normals[a.faces[i].normal_index[j]](0)),
	//				GLfloat(a.normals[a.faces[i].normal_index[j]](1)),
	//				GLfloat(a.normals[a.faces[i].normal_index[j]](2)));
	//			glVertex3f(GLfloat(a.points[a.faces[i].point_index[j]](0)),
	//				GLfloat(a.points[a.faces[i].point_index[j]](1)),
	//				GLfloat(a.points[a.faces[i].point_index[j]](2)));
	//		}
	//	}
	//	glDisable(GL_TEXTURE_2D);
	//	glEnd();
	//}
	glFlush();
}

//------------------------------Main functions of image processing----------------------------------//
// mt19937 is a standard mersenne_twister_engine
// obtain a seed from the system clock to initialize random number generator
std::mt19937 mt_rand(std::chrono::system_clock::now().time_since_epoch().count());
//Produce a random integer value, which uniformly distributed on the closed interval [a, b]
auto generator = std::bind(std::uniform_int_distribution<int>(0, 16800 - 1), mt_rand);
//auto generator = std::bind(std::uniform_int_distribution<int>(0, 10 - 1), mt_rand);

inline string GetBackgroundImage(const string& backgroundImgPath)
{
	int fileNum = generator();
	stringstream ss;
	ss << fileNum;
	string fileName = backgroundImgPath + ss.str() + ".jpg";
	cout << fileName << endl;
	bool flag = true;
	cv::Mat img;
	while (flag)
	{
		img = cv::imread(fileName, -1);
		if (img.cols < 500 || img.rows < 500)
		{
			stringstream sss;
			fileNum = generator();
			sss << fileNum;
			fileName = backgroundImgPath + sss.str() + ".jpg";
		}
		else
			flag = false;
	}

	return fileName;
}

inline void ChangeBackground(cv::Mat& img, const string& backgroundImgPath)
{
	string backImgFilename = GetBackgroundImage(backgroundImgPath);
	cv::Mat backImg = cv::imread(backImgFilename.c_str(), -1);
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
		{
			cv::Vec3b color = img.at<cv::Vec3b>(i, j);
			if (color.val[0] > 255 - 15 && color.val[1] > 255 - 15 && color.val[2] > 255 - 15)
			{
				cv::Vec3b color = backImg.at<cv::Vec3b>(i, j);
				img.at<cv::Vec3b>(i, j) = color;
			}
		}
}

cv::Mat ImageResize(const cv::Mat& srcImg, float u, float v, int minx, int miny, int maxx, int maxy, int height, int width, float &newU, float &newV)
{
	

	//u = u - miny;
	//v = v - minx;
	float boxW = maxy - miny;
	float boxH = maxx - minx;

	srand((unsigned)time(0) + 512);
	double offsetX = distribution(gen);  //X方向的偏移
	double offsetY = distribution(gen);  //Y方向的偏移
	//cout << offsetX << ends << offsetY << endl;
	float centerX = (maxx + minx) / 2;
	float centerY = (maxy + miny) / 2;
	
	centerX += offsetX;
	centerY += offsetY;

	float length = 0.0;
	if (boxW > boxH)
		length = boxW;
	else
		length = boxH;

	int gap = rand() % 40+30;  //框大小的随机变化
	length += gap;
	float leftupX = centerX - length / 2;
	float leftupY = centerY - length / 2;

	if (leftupX + length > srcImg.cols)
	{
		leftupX = srcImg.cols - length;
	}
	if (leftupY + length > srcImg.rows)
	{
		leftupY = srcImg.rows - length;
	}
	if (leftupX < 0)
	{
		leftupX = 0;
	}
	if (leftupY < 0)
	{
		leftupY = 0;
	}
	newU = u - leftupY;
	newV = v - leftupX;
	newV = newV * (height / length);
	newU = newU * (width / length);
	//newV = v / length * width;
	//newU = u / length * height;
	newV < 0 ? newV = 0 : newV;
	newU < 0 ? newU = 0 : newU;

	cv::Mat dstImg(length, length, CV_8UC3);

	for (int i = 0; i < length; i++)
		for (int j = 0; j < length; j++)
		{
			int px = int(i + leftupX);
			int py = int(j + leftupY);

			cv::Vec3b color = srcImg.at<cv::Vec3b>(px, py);
			dstImg.at<cv::Vec3b>(i, j) = color;
		}

	cv::Size ResImgSiz = cv::Size(height, width);
	cv::Mat ResImg = cv::Mat(ResImgSiz, CV_8UC3);
	cv::resize(dstImg, ResImg, ResImgSiz, 0, 0, CV_INTER_CUBIC);

	return ResImg;
}

void GenerateImage(const string& saveImgPath, const string& saveImgName, const string& backgroundImgPath)
{
	//-----------------------------------------------------------------------//
	//Generate the image
	cv::Mat img(500, 500, CV_8UC4);
	void *data = malloc(sizeof(unsigned char) * 4 * 500 * 500);
	glReadPixels(0, 0, 500, 500, GL_BGRA, GL_UNSIGNED_BYTE, data);
	for (int row = 0; row < 500; row++)
	{
		memcpy(img.ptr(500 - 1 - row), (unsigned char*)data + row * 500 * 4, sizeof(unsigned char) * 4 * 500);
	}
	free(data);

	float minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			if (img.at<cv::Vec4b>(i, j)(0) != 255)  //不是白色的像素,即属于车的像素
			{
				minx = std::min(minx, float(i));
				maxx = std::max(maxx, float(i));
				miny = std::min(miny, float(j));
				maxy = std::max(maxy, float(j));
			}
		}
	}
	//cv::rectangle(img, cv::Point(miny, minx), cv::Point(maxy, maxx), {255.0, 0.0, 0.0}, 1);

	//-----------------------------------------------------------------------//
	//Project the 3D anchor point to image
	Eigen::Vector3f y_axis, z_axis, x_axis, cord;

	cord(0) = x1 - camera_x; 
	cord(1) = x2 - camera_y; 
	cord(2) = x3 - camera_z;

	y_axis(0) = upx;
	y_axis(1) = upy;
	y_axis(2) = upz;
	y_axis.normalize();

	z_axis(0) = cam_refx - camera_x;
	z_axis(1) = cam_refy - camera_y;
	z_axis(2) = cam_refz - camera_z;
	z_axis.normalize();

	x_axis(0) = -(y_axis(1) * z_axis(2) - y_axis(2) * z_axis(1));
	x_axis(1) = -(-y_axis(0) * z_axis(2) + y_axis(2) * z_axis(0));
	x_axis(2) = -(y_axis(0) * z_axis(1) - y_axis(1) * z_axis(0));
	float temp_x = cord.transpose() * x_axis;
	float temp_y = cord.transpose() * y_axis;
	float temp_z = cord.transpose() * z_axis;
	float u = temp_x * 600 / temp_z + 250;
	float v = -temp_y * 600 / temp_z + 250;
	//cv::circle(img, cv::Point(u, v), 4, { 0.0, 0.0, 255.0 }, 2);  //红圈

	//-----------------------------------------------------------------------//
	//Change the background
	cv::Mat temp;
	cv::cvtColor(img, temp, CV_BGRA2BGR); // 颜色空间转换
	ChangeBackground(temp, backgroundImgPath);

	//-----------------------------------------------------------------------//
	//Resize the output image
	float newU = 0;
	float newV = 0;
	cv::Mat cropImg = ImageResize(temp, u, v, minx, miny, maxx, maxy, 224, 224, newU, newV);
	//cv::circle(cropImg, cv::Point(newU, newV), 2, { 255.0, 0.0, 0.0 }, 2); // 蓝圈

	cv::imwrite(saveImgPath + saveImgName, cropImg);

	//-----------------------------------------------------------------------//
	//Save information to txt file
	std::fstream fileOut(saveImgPath + "main.txt", ios_base::out | ios_base::app);
	fileOut << saveImgName << " "
		<< elevation * 180.0 / _P_I_ << " "
		<< newU << " " << newV << " "
		<< azimuth * 180.0 / _P_I_ << " "
		<< rotation * 180.0 / _P_I_ << " "
		<< camera_z << std::endl;
	fileOut.close();
}

void HZJLoop()
{
	string carListPath = "./data/Car_obj/list_car.csv";
	string car_model_path = "./data/Car_obj/";
	string backgroundImgPath = "./data/background_image/";
	string saveImgPath = "./data/renderedImages/";

	vector<pair<string, double>> carNameAndScale;
	loadCarList(carListPath, carNameAndScale);
	std::cout << "Number of Cars: " << carNameAndScale.size() << endl;
	for (int i = 0; i < 1; i++)
	{
		string car_name = carNameAndScale[i].first;
		//double car_Scale = carNameScale[i].second;
		string car_mtl_path = car_model_path + carNameAndScale[i].first + "/model.mtl";
		string car_obj_path = car_model_path + carNameAndScale[i].first + "/model.obj";
		Render r(car_mtl_path, car_obj_path);
		r.load_mtl();
		r.load_obj();
		r.revise();
		r.save_obj();
	}
	std::cout << std::endl << "Over!" << std::endl;
	std::system("pause");
}

void MainLoop()
{
	string carListPath    = "./data/Car_obj/list_car.csv";
	string car_model_path = "./data/Car_obj/";
	string backgroundImgPath = "./data/background_image/";
	string saveImgPath    = "./data/renderedImages/";

	vector<pair<string, double>> carNameAndScale;
	loadCarList(carListPath, carNameAndScale);
	std::cout << "Number of Cars: " << carNameAndScale.size() << endl;

	for (int i = 0; i < carNameAndScale.size(); i++)
	{
		string car_name = carNameAndScale[i].first;
		//double car_Scale = carNameScale[i].second;
		string car_mtl_path = car_model_path + carNameAndScale[i].first + "/model.mtl";
		string car_obj_path = car_model_path + carNameAndScale[i].first + "/model.obj";
		Render r(car_mtl_path, car_obj_path);
		r.load_mtl();
		r.load_obj();
		r.revise();
		init();
		float el = 0.0, az = 0.0, ro = 0.0;
		//float x1_ = 0.255, x2_ = -0.057, x3_ = 0.16;
		float x1_ = r.minX + (r.maxX - r.minX) / 2;
		float x2_ = r.minY;    //Before 20171123 we use X - 0.03
		float x3_ = r.minZ + (r.maxZ - r.minZ) / 2;
		GLfloat camera_x_ = 0.0;
		GLfloat camera_y_ = 0.0;
		GLfloat camera_z_ = 2.0;
		GLfloat cam_refx_ = 0.0;
		GLfloat cam_refy_ = 0.0;
		GLfloat cam_refz_ = 0.0;
		GLfloat upx_ = 0.0;
		GLfloat upy_ = 1.0;
		GLfloat upz_ = 0.0;

		elevation = el;
		azimuth = az;
		rotation = ro;
		x1 = x1_;
		x2 = x2_;
		x3 = x3_;
		camera_x = camera_x_;
		camera_y = camera_y_;
		camera_z = camera_z_;
		cam_refx = cam_refx_;
		cam_refy = cam_refy_;
		cam_refz = cam_refz_;
		upx = upx_;
		upy = upy_;
		upz = upz_;

		init();
		//--------------------------------------------------//
		int index = 0;
		for (int i = 0; i < 50;i++)
		//for (el = 0; el < 180; el += 90)
		{
			el = 5;
			for (az = 0; az < 360; az += 20)
			{
				glLoadIdentity();
				double el1, az1, ro1;
				el1 = el / 180.0 * _P_I_;
				ro1 = ro / 180.0 * _P_I_;
				az1 = az / 180.0 * _P_I_;
				elevation = el1;
				azimuth = az1;
				rotation = ro1;
				x1 = x1_;
				x2 = x2_;
				x3 = x3_;
				camera_x = camera_x_;
				camera_y = camera_y_;
				camera_z = camera_z_;
				cam_refx = cam_refx_;
				cam_refy = cam_refy_;
				cam_refz = cam_refz_;
				upx = upx_;
				upy = upy_;
				upz = upz_;

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				UpdateCamera();
				LoadModel(r);
				char str[10];
				sprintf(str, "%d", index);
				string saveImgName = car_name + "_" + str + ".jpg";
				GenerateImage(saveImgPath, saveImgName, backgroundImgPath);
				std::cout << i + 1 << "  " 
					<< "Car:" << car_name << "  "
					<< "Elevation: " << el << "  "
					<< "Azimuth: " << az << std::endl;
				index += 1;
			}
			
		}
	}

	std::cout << std::endl << "Over!" << std::endl;
	std::system("pause");
	//std::exit(0);
}

//-------------------------------OpenGL functions---------------------------------//
void GLDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 3.0, 1.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	std::cout << "Beginning" << endl;
	//HZJLoop();
	MainLoop();
}

void GLReshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//if(w < h)
	//	glOrtho(-1.5, 1.5, -1.5 * (GLfloat)h/(GLfloat)w, 1.5 * (GLfloat)h/(GLfloat)w, -10.0, 10.0);
	//else
	//	glOrtho(-1.5 * (GLfloat)w/(GLfloat)h, 1.5 * (GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
	if (w < h) 
		gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 50);
	else 
		gluPerspective(45.0, (GLfloat)h / (GLfloat)w, 0.1, 50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	UpdateCamera();
	gluLookAt(camera_x, camera_y, camera_z, cam_refx, cam_refy, cam_refz, upx, upy, upz);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL Image Render");

	glutDisplayFunc(GLDisplay);
	glutReshapeFunc(GLReshape);
	glutMainLoop();

	return 0;
}
