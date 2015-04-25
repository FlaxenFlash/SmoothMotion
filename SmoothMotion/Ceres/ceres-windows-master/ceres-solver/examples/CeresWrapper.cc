#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen\Dense"

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;
using std::vector;
using Eigen::Vector3d;


struct CostFunctor {
	template <typename T> bool operator()(const T* const x, T* residual) const {
		residual[0] = T(10.0) - x[0];
		return true;
	}
};

class BasicCollisionResolutionFunction : public ceres::FirstOrderFunction
{
private:
	int _parameters;
	int _frames;
public:
	BasicCollisionResolutionFunction(int parameters, int frames)
	{
		/*_parameters = parameters;
		_frames = frames;*/
	}

	virtual int NumParameters() const { return _parameters; }

	virtual bool Evaluate(const double* parameters, double* costs, double* gradient) const
	{
		/*int parameterIndex = 0;

		for (int frame = 0; frame < _frames; frame++)
		{
			Vector3d colliderPosition(parameters[parameterIndex++], parameters[parameterIndex++], parameters[parameterIndex++]);
			Vector3d planePosition(parameters[parameterIndex++], parameters[parameterIndex++], parameters[parameterIndex++]);
			Vector3d planeNormal(parameters[parameterIndex++], parameters[parameterIndex++], parameters[parameterIndex++]);

			Vector3d intersectionVector = colliderPosition - planePosition;
			auto intersection = intersectionVector.dot(planeNormal);

			costs[frame] = intersection < 0 ? 100 : 0;
		}		
*/
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
	//ceres::GradientProblemSolver::Options options;
	//ceres::GradientProblemSolver::Summary summary;
	//ceres::Solve(options, problem, parameters, &summary);
}

//int main(int argc, char** argv) {
//	google::InitGoogleLogging(argv[0]);
//
//	// The variable to solve for with its initial value. It will be
//	// mutated in place by the solver.
//	double x = 0.5;
//	const double initial_x = x;
//
//	// Build the problem.
//	Problem problem;
//
//	// Set up the only cost function (also known as residual). This uses
//	// auto-differentiation to obtain the derivative (jacobian).
//	CostFunction* cost_function =
//		new AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
//	problem.AddResidualBlock(cost_function, NULL, &x);
//
//	// Run the solver!
//	Solver::Options options;
//	options.minimizer_progress_to_stdout = true;
//	Solver::Summary summary;
//	Solve(options, &problem, &summary);
//
//	std::cout << summary.BriefReport() << "\n";
//	std::cout << "x : " << initial_x
//		<< " -> " << x << "\n";
//	return 0;
//}