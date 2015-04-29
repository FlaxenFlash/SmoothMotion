#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen\Dense"
#include <fstream>
#include <windows.h>
#include <iostream>
#include "Collider.h"

using ceres::NumericDiffCostFunction;
using ceres::CENTRAL;
using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;
using Eigen::Vector3d;
using namespace std;

class BasicCollisionResolutionFunction
{
private:
	vector<Collider> _colliders;

public:
	BasicCollisionResolutionFunction(vector<Collider> colliders)
	{
		_colliders = colliders;
	}

	bool operator()(const double* const x, double* residual) const
	{
		int parameterIndex = 0;

		Vector3d bonePosition(x[0], x[1], x[2]);
			
		for (int i = 0; i < _colliders.size(); i++)
		{
			auto collider = _colliders[i];
			auto overlap = collider.IntersectsPlane(bonePosition);
			*residual = (overlap < 0) ? 0 : overlap;
		}	

		return true;
	}
};

void ReadFile(string file, vector<Collider> &colliders, vector<double> &positions, int &frames, int &numBones);
void SaveFile(string filename, vector<double> parameters, int frames);
void WriteErrorAndExit(string error);
void ReadInt(ifstream &stream, int &value);
void ReadDouble(ifstream &stream, double &value);
void ReadVector3(ifstream &stream, Vector3d &value);

extern"C" __declspec(dllexport) float TestFunction()
{
	return 5;
}

extern"C" __declspec(dllexport) void ReturnTest(int* numbers)
{
	numbers[0] = 5;
}

extern"C" __declspec(dllexport) void BasicCollisionAvoidance(vector<Collider> colliders, int frames, int bones, double* parameters)
{
	ceres::Solver::Options options;
	options.minimizer_progress_to_stdout = true;
	ceres::Solver::Summary summary;

	ceres::Problem problem;

	CostFunction *cost_function = new NumericDiffCostFunction<BasicCollisionResolutionFunction, CENTRAL, 1, 3>(
		new BasicCollisionResolutionFunction(colliders));

	for (int position = 47 * frames; position < 48 * frames; position++)
	{
		problem.AddResidualBlock(cost_function, NULL, &parameters[position * 3]);
	}
		
	ceres::Solve(options, &problem, &summary);
	std::cout << summary.BriefReport() << "\n";
}

int main(int argc, char** argv) 
{
	google::InitGoogleLogging(argv[0]);

	int bones = 0;
	int frames = 0;
	vector<double> parameters;
	vector<Collider> colliders;

	string filename;
	cout << "Enter file to process: ";
	cin >> filename;
	cout << "\n";

	ReadFile(filename, colliders, parameters, frames, bones);

	BasicCollisionAvoidance(colliders, frames, bones, &parameters[0]);

	SaveFile(filename, parameters, frames);

	cout << "Success";
	cin >> filename;

	return 0;
}

void ReadFile(string file, vector<Collider> &colliders, vector<double> &positions, int &frames, int &numBones)
{
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);

	int numColliders, numParameters, i;

	ifstream inputStream(file+".SMInput", ofstream::in);

	if (!inputStream)
		WriteErrorAndExit("Failed to open file\n");

	numBones = 0;
	ReadInt(inputStream, numBones);
	ReadInt(inputStream, frames);
	ReadInt(inputStream, numColliders);
	numParameters = numBones * frames * 3;

	i = 0;
	positions = vector<double>(numParameters);
	while(inputStream && i < numParameters)
	{
		ReadDouble(inputStream, positions[i]);
		i++;
	}

	if (i != numParameters)
		WriteErrorAndExit("File ended before all bone positions were found");

	i = 0;
	colliders = vector<Collider>(numColliders);
	while (inputStream && i < numColliders)
	{
		ReadVector3(inputStream, colliders[i].PlanePosition);
		ReadVector3(inputStream, colliders[i].PlaneNormal);

		i++;
	}

	if (i != numColliders)
		WriteErrorAndExit("File ended before all colliders were found");
	inputStream.close();
}

void SaveFile(string filename, vector<double> parameters, int frames)
{
	ofstream outputStream(filename+".SMOutput", ofstream::out | ofstream::trunc);

	if (!outputStream)
	{
		cout << "Failed to open file for saving " + filename + ".SMOutput";
		exit(1);
	}

	outputStream << frames;

	for (int i = 0; i < parameters.size(); i++)
	{
		outputStream << " " << parameters[i];
	}

	outputStream.close();
}

void WriteErrorAndExit(string error)
{
	int temp;
	cout << error;
	cin >> temp;
	exit(1);
}

void ReadInt(ifstream &stream, int &value)
{
	if (!stream)
		WriteErrorAndExit("Unexpected end of file\n");

	stream >> value;
}

void ReadDouble(ifstream &stream, double &value)
{
	if (!stream)
		WriteErrorAndExit("Unexpected end of file\n");

	stream >> value;	
}

void ReadVector3(ifstream &stream, Vector3d &value)
{
	ReadDouble(stream, value.x());
	ReadDouble(stream, value.y());
	ReadDouble(stream, value.z());
}