#include "gtest.h"
#include "constants.h"
#include "plasma_system.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <algorithm>

void RelocateInside(double ax, double ay, double bx, double by, Particle &part);
namespace testConst 
{
	const int MatrixSize = 64; // ���������� ������ ����� 
	const double A = 0.05; // ���������� ��������� ��������� 
	const double L = 1.0; // ���������� ������ ��������� ������� 
	const double NumPerL_Debay = 0.5; // ���������� ���������� ����������� �������� ������ 
	const int NumPerPlasmaPeriod = 256; // ���������� ����� �� ������� �� ������ ��������� 
	const int NumPerCell = 30; // ������� ���������� ���������� �� ������ 
	const double NumPeriods = MatrixSize / (2 * sqrt(2) * constants::pi * NumPerL_Debay); // ���������� �������� ��������� � ������� 
	const double SpaceStep = L / MatrixSize; // ���������� ���� ����� 
	const double L_Debay = SpaceStep * NumPerL_Debay; // ��������������� �������� ��� ����������� ���������� ������ 
	const double Temp = 1e-2 *constants::m * constants::c * constants::c; // ��������� �����������, ���� � ������� ����� ����� 0 
	const double Density = Temp / (8 * constants::pi * constants::q * L_Debay * constants::q * L_Debay); // ������� ��������� ������ 
	const double w_p = sqrt(4 * constants::pi * constants::q * constants::q * Density / constants::m); // ������� ���������

	// ��� ������ ���������� ����� ��������������� ��� ������������� 
	const int MatrixSize_X = MatrixSize; // ���������� ����� �� x (Nx) 
	const int MatrixSize_Y = MatrixSize / 4; // ���������� ����� �� y (Ny) 
	const double X_Min = 0; // ������ ��������� ������� �� x (ax) 
	const double Y_Min = 0; // ������ ��������� ������� �� y (ay) 
	const double X_Max = L; // ����� ��������� ������� �� x (bx) 
	const double Y_Max = MatrixSize_Y * X_Max / MatrixSize_X; // ����� ��������� ������� �� y (by) 
	const double TimeStep = 2 * (constants::pi / w_p) / NumPerPlasmaPeriod; // ��� �� ������� (dt) 
	const int IterationsNumber = NumPeriods * NumPerPlasmaPeriod; // ���������� �������� �� ������� 
	const int IterationsBetweenDumps = NumPerPlasmaPeriod / 16; // ��� � ������� �������� �� ������� ������ ����� 
	const double ParticlesFactor = Density * SpaceStep * SpaceStep / NumPerCell; // ������ (���) ���� ������

	// ��������� ������� ��� ������
	const double Charge = constants::q;
	const double Mass = constants::m;

	// ��������� ������� ��� ����
	const double Amp = 2 * L * Density * constants::q * A;
}
using namespace testConst;
class GeneralTest: public PlasmaSystem, public testing::Test {
public:
	
	// ��������� ������� ��� ����, ��������� ���������� ����� Ex � ������ ������� 
	friend vec3 fE(double x, double y, double z, double t) {
		return vec3(-Amp * cos(2 * constants::pi * x / L), 0, 0);
	}
	
	// ��������� ������� ��� ������, ������� �������������
	friend double fDistr(double x, double y, double z) {
		return Density / ParticlesFactor * (1 + A * sin(2 * constants::pi*x / L));
	}

	GeneralTest() {
		SetGrid(MatrixSize_X, MatrixSize_Y, X_Min, Y_Min, X_Max, Y_Max, fE, NullFunc);
		SetParticles(fDistr, Mass, Charge, ParticlesFactor);
	}

	static double FindS(double x1, double x2, double y1, double y2) {
		double f1 = x1 - A*L / 2 / constants::pi*cos(2 * constants::pi*x1 / L);
		double f2 = x2 - A*L / 2 / constants::pi*cos(2 * constants::pi*x2 / L);
		return Density / ParticlesFactor*(f2 - f1) * (y2-y1);
	}

	double FieldEnergy() {
		double s = 0;
		for (int i = 0; i <= Grid.gnx(); i++)
			for (int j = 0; j <= Grid.gny(); j++) {
				s += Grid(i, j).E.x*Grid(i, j).E.x + Grid(i, j).E.y*Grid(i, j).E.y + Grid(i, j).E.z*Grid(i, j).E.z;
				s += Grid(i, j).B.x*Grid(i, j).B.x + Grid(i, j).B.y*Grid(i, j).B.y + Grid(i, j).B.z*Grid(i, j).B.z;
			}
		s *= Grid.gdy()*Grid.gdx()*10e-7 / (8 * constants::pi);
		return s;
	}
	double ParticlesEnergy() {
		double s = 0; 
		for (int i = 0; i < Particles.size(); i++)
			s += Particles[i].getM()*Particles[i].getV().absSquare() / 2;
		return s;
	}
	double CalcEnergy() {
		return FieldEnergy() + ParticlesEnergy();
	}

protected:
	const int partStep = 1000;
	void RecordParticle(std::ofstream &f, std::string& outDir, 
		const std::string &ident, int numPart, bool createNewFile)
	{
		openStream(f, outDir, "paticle", numPart, ident, createNewFile);
		PrintParticle(f, ident, numPart, true);
		f.close();
	}
	void PrintForEveryParticle(int timeStep, int particleStep, std::string outDir) {
		bool printCoord = false;
		std::ofstream f;
		bool createNewFile = false;
		if (timeStep == 0)
			createNewFile = true;
		for (int i = 0; i < NParticles; i += particleStep)
		{
			RecordParticle(f, outDir, V, i, createNewFile);
			if (printCoord)
				RecordParticle(f, outDir, C, i, createNewFile);
		}
	}
	void PrintDensity(int timeStep, std::string outDir) {
		std::ofstream f;
		openStream(f, outDir, "num_particle", timeStep, "middle", true);
		std::vector<double> v = CalculateNumParticleInRows(0, Grid.gny());
		for (int i = 0; i < Grid.gnx(); i++)
			f << v[i] << ';';
		f.close();
	}
	void PrintEx(int timeStep, std::string outDir) {
		std::ofstream f;
		bool createNewFile = false;
		if (timeStep == 0)
			createNewFile = true;
		openStream(f, outDir, "central_Ex", -1, "", createNewFile);
		for (int i = 0; i <= Grid.gnx(); i++)
			f << Grid(i, Grid.gny() / 2).E.x << ";";
		f << std::endl;
		f.close();
	}
	void PrintEnergy(int timeStep, std::string outDir) {
		std::ofstream f;
		bool createNewFile = false;
		if (timeStep == 0)
			createNewFile = true;
		openStream(f, outDir, "energy", -1, "", createNewFile);
		f << timeStep << ";" << FieldEnergy() << ";" << ParticlesEnergy() << ";" << CalcEnergy() << std::endl;
		f.close();
	}
	void Print(std::string outDir, int timeStep, int particleStep /*��������� � ���� ������ particleStep �������*/) override
	{
		if (timeStep%(IterationsBetweenDumps*32)==0) std::cout << timeStep << std::endl;

		if (timeStep%IterationsBetweenDumps == 0) {
			PrintForEveryParticle(timeStep, particleStep, outDir);
			PrintDensity(timeStep, outDir);
			PrintGrid(outDir, timeStep, B);
			PrintGrid(outDir, timeStep, E);
			PrintEnergy(timeStep, outDir);
		}

		PrintEx(timeStep, outDir);
		
	}
};
class GeneralTestParticles : public GeneralTest {
public:
	
	int** NPartInCell;//��� �������� ����������� ����� ������ � �������

	void DetNPartInCell() {
		//������� ������ � ������
		int ix, iy;
		bool fNPartNotNull = false;
		for (int k = 0; k < NParticles; k++) {
			ix = Grid.getIndex(Particles[k].getC().x, Particles[k].getC().y).x;
			iy = Grid.getIndex(Particles[k].getC().x, Particles[k].getC().y).y;
			if (ix >= MatrixSize_X || iy >= MatrixSize_Y) {
				std::cout << "#########ix=" << ix << " iy=" << iy << std::endl;
				std::cout << Particles[k].getC().x << " " << Particles[k].getC().y << std::endl;
				std::cout << "MaxX=" << X_Max << " MaxY=" << Y_Max << std::endl;
			}

			NPartInCell[ix][iy]++;
		}
	}


	void SetUp() {
		NPartInCell = NULL;
		NPartInCell = new int*[Grid.gnx()];
		for (int i = 0; i < Grid.gnx(); i++) {
			NPartInCell[i] = new int[Grid.gny()];
			for (int j = 0; j < Grid.gny(); j++)
				NPartInCell[i][j] = 0;
		}
		DetNPartInCell();
	}

	void TearDown() {
		if (NPartInCell != NULL) 
		{
			for (int i = 0; i <Grid.gnx(); i++)
				delete[] NPartInCell[i];
			delete[] NPartInCell;
		}
	}
};
class GeneralTestGrid : public GeneralTest {};
class PlasmaSystemTest : public PlasmaSystem, public testing::Test {};

TEST_F(PlasmaSystemTest, find_S_correctly) {
	//double x1 = 0, x2 = Grid.gdx(), y1=0, y2=Grid.gdy();
	double x1 = 0, x2 = X_Max, y1 = 0, y2 = Y_Max;
	ASSERT_NEAR(GeneralTest::FindS(x1, x2,y1,y2), FindSUnderDistr(x1, x2, y1, y2, fDistr),0.1);
}

/*
TEST_F(GeneralTest, file_output) {
	PrintFile("../../files/output/", true);
}

TEST_F(GeneralTest, file_output_upper_funct) {
	PrintGridAndParticles("../../files/output/", 0, 1000, true);
}*/

TEST_F(GeneralTestGrid, grid_created_correctly) {
	ASSERT_EQ(Grid.gdx(), (X_Max - X_Min) / MatrixSize_X);
	ASSERT_EQ(Grid.gdy(), (Y_Max - Y_Min) / MatrixSize_Y);
	for(int i=0; i<=MatrixSize_X;i++)
		for (int j = 0; j <= MatrixSize_Y; j++) {
			ASSERT_EQ(Grid(i, j).E.x,-Amp * cos(2 * constants::pi * i*Grid.gdx() / L));
			ASSERT_EQ(Grid(i, j).E.y, 0);
			ASSERT_EQ(Grid(i, j).E.z, 0);
			ASSERT_EQ(Grid(i, j).B.x, 0);
			ASSERT_EQ(Grid(i, j).B.y, 0);
			ASSERT_EQ(Grid(i, j).B.z, 0);
		}
			
}

TEST_F(GeneralTestParticles, NPart_is_correct) {
	//������� ����� ������
	long long s = 0;
	for (int i = 0; i < MatrixSize_X; i++)
		for (int j = 0; j < MatrixSize_Y; j++)
			s += NPartInCell[i][j];
	ASSERT_EQ(NParticles, s);
}

TEST_F(GeneralTestParticles, Calculete_NumParticleInRows_is_correct) {
	//������� ����� ������
	std::vector<double> v = CalculateNumParticleInRows(0, 1);
	for (int j = 0; j < Grid.gnx(); j++)			
		EXPECT_EQ(NPartInCell[j][0], v[j]);
}

TEST_F(GeneralTestParticles, NPart_in_column_is_equal) {
	//�������� ���������� ����� ������ � ������� �� �������
	for (int i = 0; i < MatrixSize_X; i++) {
		for (int j = 0; j<MatrixSize_Y - 1; j++)
			ASSERT_NEAR(NPartInCell[i][j],NPartInCell[i][j + 1], 5);
	}
}

TEST_F(GeneralTestParticles, particles_created_correctly) {
	//���������� �� ����� ������ � �������
	for (int i = 0; i < MatrixSize_X; i++)
		for (int j = 0; j < MatrixSize_Y; j++) {
			ASSERT_NEAR((int)FindS(DetX(i), DetX(i+1), DetY(j), DetY(j+1)), NPartInCell[i][j], 5);
			ASSERT_NEAR(NPartInCell[i][j], 30, 5);
		}
}

TEST_F(GeneralTest, create_data)
{
	bool isPrint = true, printCoord = false;
	try
	{
		RunModel(TimeStep, IterationsNumber, 1, isPrint, "../../files/output/");
	}
	catch (const char* s)
	{
		std::cout << s;
		throw - 1;
	}
}



