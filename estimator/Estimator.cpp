#include "Estimator.h"

Estimator::Estimator()
{
	measurementList = NULL;
	input = NULL;
	filterManager = NULL;
	reset();
}

Estimator::~Estimator()
{
	destructor();	
}
void Estimator::destructor()
{
	if (measurementList != NULL)
		delete measurementList;
	measurementList = NULL;
	
	if (input != NULL)
		delete input;
	input = NULL;

	if (filterManager != NULL)
		delete filterManager;
	filterManager = NULL;
}

void Estimator::reset()
{
	destructor();

	prevLocation = Vector();
}


void Estimator::setEstimator(EstimatorArgument estimatorArgument)
{
	reset();
	args = estimatorArgument;
	args.setCutThreshold();

	setMeasurementList();
	setSolver();
	setInput();
	setFilterManager();

}


void Estimator::setMeasurementList()
{

	measurementList = new MeasurementList(args.lid, args.beacons, args.planes);


	MeasurementCondition measurementCondition;

	measurementCondition.minSize = args.minBeaconSize;
	measurementCondition.validSize = args.validSize;
	measurementCondition.strictValidSize = args.strictValidSize;
	measurementCondition.timeWindow = args.timeWindow;

	measurementCondition.shortDistanceFirst = (bool)(!args.estimatorMode == EST::TRADITIONAL);
	measurementCondition.smallerNVSSFirst = (bool)(args.optimization & OPT::SELECTION);

	measurementCondition.minValidDistance = args.minValidDistance;

	
	measurementList->setMeasurementCondition(measurementCondition);

	

}

void Estimator::setSolver()
{
	SolverCondition condition;

	condition.solveNaive = (bool)(args.estimatorMode == EST::TRADITIONAL);

	condition.cutBranch1 = (bool)(args.optimization & 4);
	condition.cutBranch2 = (bool)((int)args.optimization & (int)OPT::BRANCHCUT_2);

	condition.maxMeasError = args.maxMeasError;
	
	condition.minBeaconSize = args.minBeaconSize;

	solver.setSolverCondition(condition);
}

void Estimator::setInput()
{
	input = new SolverInput(measurementList);
}

void Estimator::setFilterManager()
{
	KFArgument kfArgs;
	kfArgs.mode = args.kfMode;
	kfArgs.timeSlot = args.timeSlot;
	kfArgs.KFMeasError = args.kfMeasError;
	kfArgs.KFSystemError = args.kfSystemError;
	filterManager = new FilterManager(kfArgs);
}


void Estimator::measure(unsigned long timestamp, int userBid, double distance)
{
	measurementList->measure(userBid, timestamp, distance);
}


EstimatorResult Estimator::solve(long currentTime)
{
	input->setup(currentTime, prevLocation);

	SolverResultList results;
	EstimatorResult prevResult(prevLocation, input->getError(prevLocation));


	
	
	for (size_t i = 0; i < input->measurements.size(); i++)
	{
		printf ("%8ld %d %d\n", input->measurements[i]->getTimestamp(), input->measurements[i]->getUserBid(), (int)input->measurements[i]->getDistance());
	}

	solver.solve(input, &results);

	if (results.isFail())
	{
		// number of beacons are too small
		return prevResult;
	}

	if (args.optimization & OPT::THRESHOLD)
	{
		results.cutThreshold(pow(args.cutThreshold, 2));
	}


	EstimatorResult ret;
	SolverResult result;

	switch(args.estimatorMode)
	{
	case EST::KFONLY:
	case EST::PROPOSED1:

		filterManager->getCorrectedResult(&results);
		result = results.getFilteredResult();
		
		if (!result.isValid())
		{
			// there is no valid result
			return prevResult;
		}
		
		ret.location = result.getCorrectedLocation();
		ret.error = result.getCorrectedError();
		break;

	case EST::TRADITIONAL:

		result = results.getFirstResult();
			
		if (result.isValid())
		{
			ret.location = result.location;
			ret.error = result.error;
		}
		else
		{
			//the first result is not valid
			return prevResult;
		}
		break;
	
	default:
		printf("unknown esimator mode. %d\n", args.estimatorMode);
		exit(20);
		break;
	}

	prevLocation = ret.location;

	return ret;

}









