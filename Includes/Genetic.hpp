#ifndef META_GENETIC_H_
#define META_GENETIC_H_

#include <Solver.hpp>
#include <Logger.hpp>
#include <vector>
#include <chrono>
#include <memory>

/*
* TODO:
* [X] Getting initial population
* [X] Selection (random, roulet)
* [~] Crossover (multiple)
* [] Choosing next generation (if parents can survive)
* [] Multithreading and islands
* [] Mutations
* [] Params. modyfication by algo.
* [] Islands population crossover
*
*/

namespace GA {

    struct Specimen {
        Solution solution;
        uint64_t cost;
    };

    struct Population {
        std::vector<Specimen> specimens;
        uint32_t nextGenerationOffset;
        uint32_t populationSize;
        uint32_t parentPopulation;
        Specimen& operator[](size_t i) { return specimens[i]; }
        uint32_t size() { return populationSize; }
    };

    struct Pair {
        Specimen* a, * b;
    };

    /* Base classes for functions */
    class ParentSelector {
    public:
        virtual std::vector<Pair> select(Population& population, uint32_t noPairs) = 0;
    };

    class Crossover {
    public:
        virtual void crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation) = 0;
        virtual void injectInstance(InstancePointer) = 0;
    protected:
        void mutate(Specimen* spec);
    };

    class Selection {
    public:
        virtual void select(Population& population, double parentElitism, double kidElitism, bool parentsIncluded) = 0;
    protected:
        uint32_t applyElitism(Population& pop, double parentElitism, double kidElitism, bool parentsIncluded);
    };

    /* Parent selection */
    class RouletteParentSelector : public ParentSelector {
    public:
        std::vector<Pair> select(Population& population, uint32_t noPairs) override;
    private:
        uint32_t findOnWheel(std::vector<double>& partialSums, double value);
    };

    class RandomParentSelector : public ParentSelector {
    public:
        std::vector<Pair> select(Population& population, uint32_t noPairs) override;
    };

    /* Crossovers */
    void pmxSwap(const Specimen* parent, Specimen* kid, uint32_t pivot);
    class PMXCrossover : public Crossover {
    public:
        PMXCrossover() {}
        void crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation) override;
        void injectInstance(InstancePointer newInst) override { ins = newInst; }
    private:
        InstancePointer ins;
    };

    class PMXLocalOptCrossover : public Crossover {
    public:
        PMXLocalOptCrossover(uint32_t iterations) :iters(iterations) {}
        void crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation) override;
        void injectInstance(InstancePointer newInst) override { ins = newInst; }
    private:
        void optimize(Solution& spec);
        InstancePointer ins;
        uint32_t iters;
    };

    /* Selection */
    class RouletteSelector : public Selection {
    public:
        void select(Population& population, double parentElitism, double kidElitism, bool includeParents) override;
    private:
        uint32_t spinTheWheel(std::vector<double>& wheel, double sum);
    };

    class RandomSelector : public Selection {
    public:
        void select(Population& population, double parentElitism, double kidElitism, bool includeParents) override;
    };

    /* Genetic MH implementation */
    class Genetic : public Solver {
    public:
        struct Parameters {
            uint32_t parentsPopulation;
            uint32_t kidsPopulation;

            double mutation;
            double parentElitism;
            double kidElitism;

            bool includeParents;
            uint64_t timeLimit;

            std::shared_ptr<ParentSelector> parentSelector;
            std::shared_ptr<Crossover> crossover;
            std::shared_ptr<Selection> selection;
            
            static Parameters getStandard();
        };

        struct RunStatistic {
            std::chrono::high_resolution_clock::time_point start;
            uint64_t iteration;
            uint64_t stagnation;
        };

        Genetic(Parameters p) : parameters(p) {}
        ~Genetic() {}

        Solution solve(InstancePointer instance) override;
        std::string getName() override { return std::string("Genetic"); };
    private:
        Parameters parameters;
        void generateRandomPopulation(InstancePointer instance, Population& population, uint32_t size);
    };

}

#endif