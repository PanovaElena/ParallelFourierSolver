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
	const int MatrixSize = 64; // Определяет размер сетки 
	const double A = 0.05; // Определяет амплитуду колебаний 
	const double L = 1.0; // Определяет размер расчетной области 
	const double NumPerL_Debay = 0.5; // Определяет разрешение физического масштаба сеткой 
	const int NumPerPlasmaPeriod = 256; // Количество шагов по времени на период колебаний 
	const int NumPerCell = 30; // Среднее количество электронов на ячейку 
	const double NumPeriods = MatrixSize / (2 * sqrt(2) * constants::pi * NumPerL_Debay); // Количество периодов колебаний в расчете 
	const double SpaceStep = L / MatrixSize; // Определяет шаги сетки 
	const double L_Debay = SpaceStep * NumPerL_Debay; // Вспомогательная величина для определения разрешения сеткой 
	const double Temp = 1e-2 *constants::m * constants::c * constants::c; // Начальная температура, пока в расчете можно взять 0 
	const double Density = Temp / (8 * constants::pi * constants::q * L_Debay * constants::q * L_Debay); // Средняя плотность частиц 
	const double w_p = sqrt(4 * constants::pi * constants::q * constants::q * Density / constants::m); // Частота колебаний

	// Эта группа параметров нужна непосредственно для моделирования 
	const int MatrixSize_X = MatrixSize; // Количество ячеек по x (Nx) 
	const int MatrixSize_Y = MatrixSize / 4; // Количество ячеек по y (Ny) 
	const double X_Min = 0; // Начало расчетной области по x (ax) 
	const double Y_Min = 0; // Начало расчетной области по y (ay) 
	const double X_Max = L; // Конец расчетной области по x (bx) 
	const double Y_Max = MatrixSize_Y * X_Max / MatrixSize_X; // Конец расчетной области по y (by) 
	const double TimeStep = 2 * (constants::pi / w_p) / NumPerPlasmaPeriod; // Шаг по времени (dt) 
	const int IterationsNumber = NumPeriods * NumPerPlasmaPeriod; // Количество итераций по времени 
	const int IterationsBetweenDumps = NumPerPlasmaPeriod / 16; // Раз в сколько итераций по времени делать вывод 
	const double ParticlesFactor = Density * SpaceStep * SpaceStep / NumPerCell; // Фактор (вес) всех частиц

	// Начальные условия для частиц
	const double Charge = constants::q;
	const double Mass = constants::m;

	// Начальные условия для поля
	const double Amp = 2 * L * Density * constants::q * A;
}
using namespace testConst;
class GeneralTest: public PlasmaSystem, public testing::Test {
public:
	
	// Начальные условия для поля, остальные компоненты кроме Ex в начале нулевые 
	friend vec3 fE(double x, double y, double z, double t) {
		return vec3(-Amp * cos(2 * constants::pi * x / L), 0, 0);
	}
	
	// Начальные условия для частиц, функция распределения
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
	void Print(std::string outDir, int timeStep, int particleStep /*выводится в файл каждая particleStep частица*/) override
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
	
	int** NPartInCell;//для подсчета полученного числа частиц в ячейках

	void DetNPartInCell() {
		//подсчет частиц в ячейке
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
	//сколько всего частиц
	long long s = 0;
	for (int i = 0; i < MatrixSize_X; i++)
		for (int j = 0; j < MatrixSize_Y; j++)
			s += NPartInCell[i][j];
	ASSERT_EQ(NParticles, s);
}

TEST_F(GeneralTestParticles, Calculete_NumParticleInRows_is_correct) {
	//сколько всего частиц
	std::vector<double> v = CalculateNumParticleInRows(0, 1);
	for (int j = 0; j < Grid.gnx(); j++)			
		EXPECT_EQ(NPartInCell[j][0], v[j]);
}

TEST_F(GeneralTestParticles, NPart_in_column_is_equal) {
	//примерно одинаковое число частиц в ячейках по столбцу
	for (int i = 0; i < MatrixSize_X; i++) {
		for (int j = 0; j<MatrixSize_Y - 1; j++)
			ASSERT_NEAR(NPartInCell[i][j],NPartInCell[i][j + 1], 5);
	}
}

TEST_F(GeneralTestParticles, particles_created_correctly) {
	//правильное ли число частиц в ячейках
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



