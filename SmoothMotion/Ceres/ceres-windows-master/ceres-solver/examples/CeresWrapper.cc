#include "ceres/ceres.h"
#include "glog/logging.h"
#include <fstream>
#include <windows.h>
#include <iostream>
#include "Collider.h"
#include "../../win/examples/MaintainBoneLengthFunction.h"
#include "../../win/examples/BasicCollisionResolutionFunction.h"
#include "../../win/examples/MaintainBoneLengthChildren.h"

using ceres::NumericDiffCostFunction;
using ceres::CENTRAL;
using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;
using Eigen::Vector3d;
using namespace std;

void ReadFile(string file, vector<Collider> &colliders, vector<double> &positions, int &frames, int &numBones, vector<int> &parents);
void SaveFile(string filename, vector<double> parameters, int frames);
void WriteErrorAndExit(string error);
void ReadInt(ifstream &stream, int &value);
void ReadDouble(ifstream &stream, double &value);
void ReadVector3(ifstream &stream, Vector3d &value);

void BasicCollisionAvoidance(vector<Collider> colliders, int frames, int bones, double* parameters)
{
	Solver::Options options;
	options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
	options.minimizer_progress_to_stdout = true;
	Solver::Summary summary;

	Problem problem;

	CostFunction *cost_function = new NumericDiffCostFunction<BasicCollisionResolutionFunction, CENTRAL, 1, 3>(
		new BasicCollisionResolutionFunction(colliders));

	for (int position = 0; position < bones * frames; position++)
	{
		problem.AddResidualBlock(cost_function, NULL, &parameters[position * 3]);
	}
		
	Solve(options, &problem, &summary);
	cout << summary.FullReport() << "\n";
}

void MaintainBoneLength(vector<Collider> colliders, int frames, int bones, double* parameters, vector<int> parents)
{
	Solver::Options options;
	options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
	options.minimizer_progress_to_stdout = true;
	Solver::Summary summary;

	Problem problem;

	for (int bone = 0; bone < bones; bone++)
	{
		for (int frame = 0; frame < frames; frame++)
		{
			auto position = (bone * frames + frame)*3;
			auto parentPosition = (parents[bone] * frames + frame)*3;
			Vector3d bonePosition(parameters[position], parameters[position + 1], parameters[position + 2]);
			Vector3d parentBonePosition(parameters[parentPosition], parameters[parentPosition + 1], parameters[parentPosition + 2]);
			auto boneLength = (parentBonePosition - bonePosition).norm();

			CostFunction *cost_function = new NumericDiffCostFunction<MaintainBoneLengthFunction, CENTRAL, 2, 3>(
				new MaintainBoneLengthFunction(colliders, &parameters[parentPosition], boneLength));

			problem.AddResidualBlock(cost_function, NULL, &parameters[position]);
		}

	}

	Solve(options, &problem, &summary);
	cout << summary.FullReport() << "\n";
}

void MaintainBoneLengthChildrenFunction(vector<Collider> colliders, int frames, int bones, double* parameters, vector<int> parents)
{
	Solver::Options options;
	options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
	options.minimizer_progress_to_stdout = true;
	Solver::Summary summary;

	Problem problem;
	vector<int> boneDepths;

	for (int bone = 0; bone < bones; bone++)
	{		
		auto parent = parents[bone];
		boneDepths.push_back(boneDepths.size() <= parent ? 1 : boneDepths[parent] + 1);
		for (int frame = 0; frame < frames; frame++)
		{
			auto position = (bone * frames + frame) * 3;
			Vector3d bonePosition(parameters[position], parameters[position + 1], parameters[position + 2]);

			vector<double*> childrenPositions;
			vector<double> boneLengths;			
			for (auto child = 0; child < bones; child++)
			{
				if (parents[child] == bone)
				{
					auto childPosition = (child*frames + frame) * 3;
					childrenPositions.push_back(&parameters[childPosition]);
					Vector3d childBone(parameters[childPosition], parameters[childPosition + 1], parameters[childPosition + 2]);
					boneLengths.push_back((childBone - bonePosition).norm());
				}
			}

			CostFunction *cost_function = new NumericDiffCostFunction<MaintainBoneLengthChildren, CENTRAL, 2, 3>(
				new MaintainBoneLengthChildren(colliders, childrenPositions, boneLengths, boneDepths[bone]));

			problem.AddResidualBlock(cost_function, NULL, &parameters[position]);
		}

	}

	Solve(options, &problem, &summary);
	cout << summary.FullReport() << "\n";
}

int main(int argc, char** argv) 
{
	google::InitGoogleLogging(argv[0]);

	int bones = 0;
	int frames = 0;
	vector<double> parameters;
	vector<Collider> colliders;
	vector<int> parents;

	string filename;
	cout << "Enter file to process: ";
	cin >> filename;
	cout << "\n";

	ReadFile(filename, colliders, parameters, frames, bones, parents);

	MaintainBoneLengthChildrenFunction(colliders, frames, bones, &parameters[0], parents);

	SaveFile(filename, parameters, frames);

	cout << "Success";
	cin >> filename;

	return 0;
}

void ReadFile(string file, vector<Collider> &colliders, vector<double> &positions, int &frames, int &numBones, vector<int> &parents)
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

	i = 0;
	parents = vector<int>(numBones);
	while (inputStream && i < numBones)
	{
		ReadInt(inputStream, parents[i]);
		i++;
	}
	if (i != numBones)
		WriteErrorAndExit("File ended before all bone parents were found");

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