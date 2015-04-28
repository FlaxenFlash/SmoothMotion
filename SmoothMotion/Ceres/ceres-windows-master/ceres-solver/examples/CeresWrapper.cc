#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen\Dense"
#include <fstream>
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
	BasicCollisionResolutionFunction(vector<Collider> colliders, vector<Vector3d> bonePositions, int frames)
	{
		_colliders = colliders;
		_bonePositions = bonePositions;
		_positions = frames * bonePositions.size();
		_parameters = _positions * 3;
		_frames = frames;
	}

	virtual int NumParameters() const { return _parameters; }

	virtual bool Evaluate(const double* parameters, double* costs, double* gradient) const
	{
		int parameterIndex = 0;

		for (int position = 0; position < _positions; position++)
		{
			Vector3d bonePosition(parameters[parameterIndex++], parameters[parameterIndex++], parameters[parameterIndex++]);			

			costs[position] = 0;
			
			for (int i = 0; i < _colliders.size; i++)
			{
				auto collider = _colliders[i];
				costs[position] += collider.IntersectsPlane(bonePosition) ? 100 : 0;
			}
		}		

		return true;
	}
};

extern"C" __declspec(dllexport) float TestFunction()
{
	return 5;
}

extern"C" __declspec(dllexport) void ReturnTest(int* numbers)
{
	numbers[0] = 5;
}

extern"C" __declspec(dllexport) void BasicCollisionAvoidance(int numParameters, int frames, double* parameters)
{
	ceres::GradientProblem problem(new BasicCollisionResolutionFunction(numParameters, frames));
	ceres::GradientProblemSolver::Options options;
	ceres::GradientProblemSolver::Summary summary;
	ceres::Solve(options, problem, parameters, &summary);
	std::cout << summary.BriefReport() << "\n";
}

int main(int argc, char** argv) 
{
	google::InitGoogleLogging(argv[0]);

	int numParams = 0;
	int numParameters = 0;
	int frames = 0;
	vector<double> parameters;
	vector<Collider> colliders;

	string filename;
	cout << "Enter file to process: ";
	cin >> filename;
	cout << "\n";

	ReadFile(filename, colliders, parameters);

	BasicCollisionAvoidance(numParameters, frames, &parameters[0]);

	ofstream outputStream(filename, ofstream::out | ofstream::trunc);

	if (!outputStream)
	{
		cout << "Failed to reopen file for editing";
		exit(1);
	}

	outputStream << numParameters << " " << frames;

	for (int i = 0; i < parameters.size(); i++)
	{
		outputStream << " " << parameters[i];
	}

	outputStream.close();

	cout << "Success";

	return 0;
}

void ReadFile(string file, vector<Collider> colliders, vector<double> positions)
{
	int numBones, numFrames, numColliders, numParameters, i;

	ifstream inputStream(file, ofstream::in);

	if (!inputStream)
		WriteErrorAndExit("Failed to open file\n");

	ReadInt(inputStream, numBones);
	ReadInt(inputStream, numFrames);
	ReadInt(inputStream, numColliders);
	numParameters = numBones * numFrames * 3;

	i = 0;
	positions.reserve(numParameters);
	while(inputStream && i < numParameters)
	{
		ReadDouble(inputStream, positions[i]);
		i++;
	}

	if (i != numParameters)
		WriteErrorAndExit("File ended before all bone positions were found");
	inputStream.close;

	i = 0;
	colliders.reserve(numColliders);
	while (inputStream && i < numColliders)
	{
		ReadVector3(inputStream, colliders[i].PlanePosition);
		ReadVector3(inputStream, colliders[i].PlaneNormal);

		i++;
	}

	if (i != numParameters)
		WriteErrorAndExit("File ended before all colliders were found");
	inputStream.close;
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
	ReadDouble(stream, value.x);
	ReadDouble(stream, value.y);
	ReadDouble(stream, value.z);
}