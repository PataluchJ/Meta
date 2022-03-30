#include "PRDTester.h"

void PRDTester::test(int n, int m, std::string filename)
{
    // n <- liczba prob na danym rozmiarze instancji
    // m <- liczba rozmiarow instancji
    std::vector<std::pair<int, double> > prd_A; // int <- rozmiar instancji, double <- sredni czas dzialania (w sekundach)
    prd_A.reserve(m);
    std::vector<std::pair<int, double> > prd_B;
    prd_B.reserve(m);
    std::vector<std::pair<int, double> > prd_C;
    prd_C.reserve(m);

    Greedy A;
    KRandom B(10);
    TwoOpt C;

    uint64_t a, b, c, best;
    double prd_a, prd_b, prd_c;
    int j = 0; // rozmiar instancji

    for (int i = 0; i < m; ++i)
    {
        j += 100;
        prd_a = 0;
        prd_b = 0;
        prd_c = 0;
        for (int k = 0; k < n; ++k)
        {
            InstancePointer x = TSPInstance::generateAsymetricInstance(j, 42);
            A.solve(x);
            a = x->calculateSolutionDistance();
            B.solve(x);
            b = x->calculateSolutionDistance();
            C.solve(x);
            c = x->calculateSolutionDistance();
            best = std::min(std::min(a, b), c);
            prd_a += (a - best) / best;
            prd_b += (b - best) / best;
            prd_c += (c - best) / best;
        }
        prd_A.push_back(std::pair<int, double>(i, prd_a / n));
        prd_A.push_back(std::pair<int, double>(i, prd_a / n));
        prd_A.push_back(std::pair<int, double>(i, prd_a / n));
    }

    std::ofstream writeToFile;
    writeToFile.open(filename);
    for (std::vector<std::pair<int, double> >::iterator itr = prd_A.begin(); itr != prd_A.end(); ++itr)
        writeToFile << itr->first << " " << itr->second << "\n";
    for (std::vector<std::pair<int, double> >::iterator itr = prd_B.begin(); itr != prd_B.end(); ++itr)
        writeToFile << itr->first << " " << itr->second << "\n";
    for (std::vector<std::pair<int, double> >::iterator itr = prd_C.begin(); itr != prd_C.end(); ++itr)
        writeToFile << itr->first << " " << itr->second << "\n";
    writeToFile.close();
}
