#include "WilcoxonTest.h"
#include "TwoOpt.h"
#include "TSPInstance.h"
#include <vector>

void WilcoxonTest::test(int n, int m, std::string filename)
{
    // n <- liczba prob na danym rozmiarze instancji
    // m <- liczba rozmiarow instancji
    std::vector<double> prd_A;
    std::vector<double> prd_B;
    prd_A.reserve(m);
    prd_B.reserve(m);

    Greedy A;
    TwoOpt B;
    uint64_t a, b, best;
    double prd_a, prd_b;

    int j = 0; // rozmiar instancji
    for (int i = 0; i < m; ++i)
    {
        j += 100;  // zwiekszam rozmiar instancji o 100
        prd_a = 0; // suma wartosci prd dla rozmiaru j w algorytmie A
        prd_b = 0; // suma wartosci prd dla rozmiaru j w algorytmie B
        for (int k = 0; k < n; ++k)
        {
            InstancePointer x = TSPInstance::generateAsymetricInstance(j, 42);
            A.solve(x);
            a = x->calculateSolutionDistance();
            B.solve(x);
            b = x->calculateSolutionDistance();
            best = std::min(a, b);
            prd_a += (a - best) / best;
            prd_b += (b - best) / best;
        }
        prd_A.push_back(prd_a / n);
        prd_B.push_back(prd_b / n);
    }

    std::ofstream writeToFile;
    writeToFile.open(filename);
    for (std::vector<double>::iterator itr = prd_A.begin(); itr != prd_A.end(); ++itr)
        writeToFile << (*itr) << " ";
    writeToFile << "\n";
    for (std::vector<double>::iterator itr = prd_B.begin(); itr != prd_B.end(); ++itr)
        writeToFile << (*itr) << " ";
    writeToFile.close();
}
