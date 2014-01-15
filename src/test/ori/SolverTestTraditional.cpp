#include "Solver.h"


Solver solver;











void initSolver()
{
	SolverCondition condition;
	condition.gatherData = false;
	condition.cutBranch1 = false;
	condition.cutBranch2 = false;
	solveNaive = true;
	maxMeasError = 50.0;
	solver.setSolverCondition(condition);

}






int main()
{
	initSolver();
	initInput();



	return 0;
}
