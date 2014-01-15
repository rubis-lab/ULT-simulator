#include "PlaneDetector_old.h"

class DetectionData
{
public:
	DetectionData(Vector vListener, Vector vB, double fR)
	{
		this->vB = vB;
		this->vC = (vB + vListener) * 0.5;
		this->fCP = fR / 2;
		this->vL = vC - vB;
		this->fcM = pow(this->fCP, 2) - vL.getSqrSize();
	}
	DetectionData(){}
	~DetectionData(){}

	void SetBaseData(DetectionData &baseData)
	{
		this->vK = this->vC - baseData.vC;
		this->fcM1 = baseData.fcM;
		this->vL1 = baseData.vL;
	}
	
	double GetF(Vector vN)
	{
		double sqrtI = fcM + pow(vL & vN, 2);
		double sqrt1 = fcM1 + pow(vL1 & vN, 2);
		if (sqrtI < 0 || sqrt1 < 0) 
		{
			printf("PlaneDetector.cpp : DetectionData.GetF, negative value in sqrtI = %f\t sqrt1 = %f\n", sqrtI, sqrt1);
			getchar();
			exit(90);
		}
		double ret = (vK & vN) + sqrt(sqrtI) - sqrt(sqrt1);
		return ret;
	}
	
	Vector GetDF(Vector vN)
	{
		double fLNi = vL & vN;
		double fLN1 = vL1 & vN;
		double sqrtI = fcM + pow(fLNi, 2);
		double sqrt1 = fcM1 + pow(fLN1, 2);
		if (sqrtI < 0 || sqrt1 < 0) 
		{
			printf("PlaneDetector.cpp : DetectionData.GetDF, negative value in sqrtI = %f\t sqrt1 = %f\n", sqrtI, sqrt1);
			getchar();
			exit(91);
		}

		double invSqrtI = 1.0 / sqrt(sqrtI);
		double invSqrt1 = 1.0 / sqrt(sqrt1);

		Vector ret;
		ret.x = vK.x + (invSqrtI * fLNi * vL.x) - (invSqrt1 * fLN1 * vL1.x);
		ret.y = vK.y + (invSqrtI * fLNi * vL.y) - (invSqrt1 * fLN1 * vL1.y);
		ret.z = vK.z + (invSqrtI * fLNi * vL.z) - (invSqrt1 * fLN1 * vL1.z);

		return ret;
	}
	
	Vector GetP(Vector vN)
	{
		double fLNi = vL & vN;
		double sqrtI = fcM + pow(fLNi, 2);
		double fT = fLNi + sqrt(sqrtI);
		Vector vP = vB + (vN * fT);

		return vP;

	}
	void Print()
	{
		printf("vC = ");
		vC.Print();
		printf("\tvB = ");
		vB.Print();
		printf("\tfCP = %f", fCP);
		printf("\tfcM = %f", fcM);
		printf("\tvL = ");
		vL.Print();
		printf("\tvK = ");
		vK.Print();

/*
		printf("\tfcM1 = %f", fcM1);
		printf("\tvL1 = ");
		vL1.Print();
*/

		printf("\n");
	}


public:
	Vector vC;
	Vector vB;
	double fCP;
	double fcM;
	Vector vL;
	Vector vK;
	double fcM1;
	Vector vL1;
};

PlaneDetector::PlaneDetector()
{
	fpA = fopen("data.txt", "w");
	fpD = fopen("PD_distance.txt", "w");

}

PlaneDetector::~PlaneDetector()
{
	fclose(fpA);
	fclose(fpD);
}
// plane normal fitting
int pnf_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	Vector vN = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2)).getUnitVector();

	for (size_t i = 0; i < data->size(); i++)
	{
		gsl_vector_set(f, i, (*data)[i].GetF(vN));
	}
	return GSL_SUCCESS;
}
int pnf_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	Vector vN = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2)).getUnitVector();

	for (size_t i = 0; i < data->size(); i++)
	{
		Vector vJi = (*data)[i].GetDF(vN);
		gsl_matrix_set(J, i, 0, vJi.x);
		gsl_matrix_set(J, i, 1, vJi.y);
		gsl_matrix_set(J, i, 2, vJi.z);
	}

	return GSL_SUCCESS;
}
int pnf_func_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	pnf_func_f(x, data, f);
	pnf_func_df(x, data, J);
	return GSL_SUCCESS;
}

// plane offset fitting
int pof_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	// the first element of ptrData is vN
	std::vector<Vector> *data = (std::vector<Vector>*) ptrData;
	Vector vN = (*data)[0];
	Vector vP0 = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2));

	for (size_t i = 0; i < data->size()-1; i++)
	{
		gsl_vector_set(f, i, vN & ((*data)[i+1] - vP0));
	}
	return GSL_SUCCESS;
}

int pof_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	// the first element of ptrData is vN
	std::vector<Vector> *data = (std::vector<Vector>*) ptrData;
	Vector vN = (*data)[0];

	for (size_t i = 0; i < data->size()-1; i++)
	{
		gsl_matrix_set(J, i, 0, -vN.x);
		gsl_matrix_set(J, i, 1, -vN.y);
		gsl_matrix_set(J, i, 2, -vN.z);
	}
	return GSL_SUCCESS;
}
int pof_func_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	pof_func_f(x, data, f);
	pof_func_df(x, data, J);
	return GSL_SUCCESS;
}


Vector PlaneDetector::getPlaneNormal(HeardList& heardList, Vector vListener)
{
	// prepare calculation data
	std::vector<DetectionData> data;

	DetectionData baseData;

	for (size_t i = 0; i < heardList.heardInfo.size(); i++)
	{
		DetectionData curData = 
				DetectionData(vListener, heardList.heardInfo[i].location, heardList.heardInfo[i].distance);
		if (i == 0) 
			baseData = curData;
		else
		{
			curData.SetBaseData(baseData);
			data.push_back(curData);
		}
	}

	// prepare gsl variables
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	int n = (int)data.size();
	int p = 3;

	gsl_multifit_function_fdf f;
	f.f = &pnf_func_f;
	f.df = &pnf_func_df;
	f.fdf = &pnf_func_fdf;
	f.n = n;
	f.p = p;
	f.params = &data;

	double x_init[3] = {0.0, 0.0, 0.0};
	gsl_vector_view x = gsl_vector_view_array(x_init, 3);

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, n, p);
	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	gsl_vector *gradt = gsl_vector_alloc(p);

	int iter = 0;
	int status;
	do
	{
		iter ++;
		status = gsl_multifit_fdfsolver_iterate(s);
		if (status)
		{
//			printf ("error: %s\n", gsl_strerror (status));
			break;
		}
		gsl_multifit_gradient(s->J, s->f, gradt);
		status = gsl_multifit_test_gradient(gradt, 1e-9);
	} while (status == GSL_CONTINUE && iter < 50000);

	vN = Vector(
			gsl_vector_get(s->x, 0),
			gsl_vector_get(s->x, 1),
			gsl_vector_get(s->x, 2)).getUnitVector();

//	for (size_t i = 0; i < data.size(); i++)
//		printf ("F_%d = %f\n", i, data[i].GetF(vN));


	gsl_vector_free(gradt);
	gsl_multifit_fdfsolver_free(s);

	vP.clear();
	vP.push_back(vN);
	vP.push_back(baseData.GetP(vN));
	for (size_t i = 0; i < data.size(); i++)
	{
		vP.push_back(data[i].GetP(vN));
	}

	return vN;
}

Vector PlaneDetector::getPlaneOffset()
{
	// plane offset fitting
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	int n = (int)vP.size() - 1;
	int p = 3;

	gsl_multifit_function_fdf f;
	f.f = &pof_func_f;
	f.df = &pof_func_df;
	f.fdf = &pof_func_fdf;
	f.n = n;
	f.p = p;
	f.params = &vP;

	double x_init[3] = {0.0, 0.0, 0.0};
	gsl_vector_view x = gsl_vector_view_array(x_init, 3);

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, n, p);
	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	gsl_vector *gradt = gsl_vector_alloc(p);

	int iter = 0;
	int status;
	do
	{
		iter ++;
		status = gsl_multifit_fdfsolver_iterate(s);
		if (status) break;
		gsl_multifit_gradient(s->J, s->f, gradt);
		status = gsl_multifit_test_gradient(gradt, 1e-6);
	} while (status == GSL_CONTINUE && iter < 50000);

	vP0 = Vector(
			gsl_vector_get(s->x, 0),
			gsl_vector_get(s->x, 1),
			gsl_vector_get(s->x, 2));

	gsl_vector_free(gradt);
	gsl_multifit_fdfsolver_free(s);

	return vP0;
}

Vector PlaneDetector::GetReflectedPoint(Vector vPoint)
{
	/* this part is as same as GetReflectedPoint function in Plane class*/
	Vector propNormal = vN * ((vP0 -  vPoint) & vN);		
//	Vector propNormal = vN * ((vP[1] -  vPoint) & vN);		
	return vPoint + (propNormal * 2);
}

double PlaneDetector::GetAngleError(Plane &realPlane)
{	
	double ret = acos(realPlane.vNormal & vN) * 180 / M_PI;
	fprintf(fpA, "%f\t", ret);
	return ret;
}

double PlaneDetector::GetDistanceError(HeardList &heardList, Plane &realPlane)
{
	int nValidBeacon = 0;
	double distErrorSum = 0;

	for (size_t i = 0; i < heardList.heardInfo.size(); i++)
	{
		Vector vBeacon = heardList.heardInfo[i].location;
		Vector vRealRefBeacon = realPlane.GetReflectedPoint(vBeacon);
		Vector vEstRefBeacon = GetReflectedPoint(vBeacon);

		nValidBeacon++;
		double distError = vRealRefBeacon.getDistance(vEstRefBeacon);
		distErrorSum += distError;
	}

	double ret = distErrorSum / nValidBeacon;
	fprintf(fpA, "%f\n", ret);
	return ret;
}


void PlaneDetector::GetPlane(HeardList &heardList, Vector vListener)
{
	fprintf(fpA, "%f\t%f\t%f\t", vListener.x, vListener.y, vListener.z);
	fprintf(fpD, "%f\t%f\t%f\t", vListener.x, vListener.y, vListener.z);
	getPlaneNormal(heardList, vListener);
	getPlaneOffset();
	for (size_t i = 1; i < vP.size(); i++)
	{
//		vP[i].Print();
		vP[i] = vP[i] + (vN * ((vP0 - vP[i]) & vN));
//		printf(" -> ");
//		vP[i].Println();

	}
}
