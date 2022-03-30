#include "KRandomTester.h"

void KRandomTester::test(int n, int m, int j, std::string filename)
{
    // n <- liczba prob dla danej wartosci k
    // m <- liczba rozmiarow wartosci k
    // j <- rozmiar instancji
    Greedy A;
    TwoOpt B;
    std::vector<std::pair<int, double> > kValue;
    kValue.reserve(m);
    int k = 0;
    uint64_t a, b, c, best;
    double prd_c;
    for (int i = 0; i < m; ++i)
    {
        k += 10;
        KRandom C(k);
        prd_c = 0;
        for (int l = 0; l < n; ++l)
        {
            InstancePointer x = TSPInstance::generateAsymetricInstance(j, 42);
            A.solve(x);
            a = x->calculateSolutionDistance();
            B.solve(x);
            b = x->calculateSolutionDistance();
            C.solve(x);
            c = x->calculateSolutionDistance();
            best = std::min(std::min(a, b), c);
            prd_c += (c - best) / best;
        }
        kValue.push_back(std::pair<int, double>(k, prd_c / n));
    }

    std::ofstream writeToFile;
    writeToFile.open(filename);
    for (std::vector<std::pair<int, double> >::iterator itr = kValue.begin(); itr != kValue.end(); ++itr)
        writeToFile << itr->first << " " << itr->second << "\n";
    writeToFile.close();
}
