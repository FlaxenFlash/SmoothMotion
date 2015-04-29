#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen\Dense"
#include <fstream>
#include <windows.h>
#include <iostream>
#include "Collider.h"

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;
using Eigen::Vector3d;
using namespace std;


struct CostFunctor 
{
	template <typename T> bool operator()(const T* const x, T* residual) const {
		residual[0] = T(10.0) - x[0];
		return true;
	}
};

class BasicCollisionResolutionFunction : public ceres::FirstOrderFunction
{
private:
	int _parameters;
	int _positions;
	int _frames;

	vector<Collider> _colliders;
	//ToDo: decide wheter bone positions need to be stored in here as vector3ds, finish calling read file, fix write file, test
public:
	BasicCollisionResolutionFunction(vector<Collider> colliders, int frames, int bones)
	{
		_colliders = colliders;
		_positions = frames * bones;
		_parameters = _positions * 3;
		_frames = frames;
	}

	virtual int NumParameters() const { return _parameters; }

	virtual bool Evaluate(const double* parameters, double* cost, double* gradient) const
	{
		int parameterIndex = 0;
		*cost = 0;

		for (int position = 0; position < _positions; position++)
		{
			Vector3d bonePosition(parameters[parameterIndex++], parameters[parameterIndex++], parameters[parameterIndex++]);			
			
			for (int i = 0; i < _colliders.size(); i++)
			{
				auto collider = _colliders[i];
				auto overlap = collider.IntersectsPlane(bonePosition);
				*cost += (overlap < 0) ? 0 : overlap;
			}
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
	ceres::GradientProblemSolver::Options options;
	options.minimizer_progress_to_stdout = true;
	ceres::GradientProblemSolver::Summary summary;

	ceres::GradientProblem problem(new BasicCollisionResolutionFunction(colliders, frames, bones));
		
	ceres::Solve(options, problem, parameters, &summary);
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