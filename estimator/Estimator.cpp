#include "Esttimator.h"

void Estimator::Estimator()
{
	reset();
}

void Estimator::~Estimator()
{
	if (measurementList != NULL)
	{
		delete measurementList;
		measurementList = NULL;
	}
	if (input != NULL)
	{
		delete input;
		input = NULL;
	}

}

void Estimator::reset()
{
	if (measurementList != NULL)
		delete measurementList;
	measurementList = NULL;

	if (input != NULL)
		delete input;

	prevLocation = Vector();
}


void Estimator::setEstimator(EstimatorArguement estimatorArgument)
{
	reset();
	args = estimatorArguement;
	args.setCutThreshold();

	setMeasurementList();
	setSolver();
	setInput();
	setCutThreshold();
	setFilterManager();

}


void Estimator::setMeasurementList()
{

	measurementList = new MeasurementList(args.lid, args.beacons., args.beacons, args.planes);


	MeasurementCondition measurementCondition;

	measurementCondition.minSize = args.minBeaconSize;
	measurementCondition.validSize = args.validSize;
	measurementCondition.strictValidSize = args.strictValidSize;
	measurementCondition.timeWindow = timeWindow;

	measurementCondition.shortDistanceFirst = (bool)(!args.estimatorMode == EST::TRADITIONAL);
	measurementCondition.smallerNVSSFirst = (bool)(args.optimizastion & OPT::SELECTION);

	
	measurementList.setMeasurementCondition(measurementCondition);

	

}

void Estimator::setSolver()
{
	SolverCondition condition;

	condition.solveNaive = (bool)(args.estimatorMode & EST::TRADITIONAL);

	condition.cutBranch1 = (bool)(args.optimization & OPT::BRANCHCUT);
	condition.cutBranch2 = (bool)(args.optimization & OPT::BRANCHCUT_2);

	condition.maxMeasError = args.maxMeasError;


	solver.setSolverCondition(condition);
}

void Estimator::setInput()
{
	input = new SolverInput(measurementList, args.timeWindow);
}

void Estimator::setFilterManager()
{

}


void Estimator::measure(int userBid, unsigned long timestamp, double distance)
{
	measurementList->measure(userBid, timestamp, distance);
}


EstimatorResult Estimator::solve(long currentTime)
{
	measurementList->makeSnapshot(currentTime, args.timeWindow);
	input->setup(currentTime, prevLocation);

	SolverResult result;
	
	solver.solve(input, &result);

	if (args.optimization & OPT::THRESHOLD)
	{
		result->cutThreshold(pow(args.cutThreshold, 2));
	}

}









