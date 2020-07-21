#ifndef UVRP_BRKGA_H_
#define UVRP_BRKGA_H_

#include <vector>
#include <memory>
#include <utility>
#include <random>

#include "../UVRP/ProblemInstance.h"
#include "../UVRP/Representation.h"
#include "../UVRP/ADS.h"
#include "../UVRP/Evaluation.hpp"
#include "../UVRP/Solution.hpp"
#include "../UVRP/Evaluator.h"
#include "../UVRP/AStar.h"
#include "Decoder.h"
#include "Common.h"

namespace BRKGA
{

class CL_BRKGA{
protected:

	//components
    UVRP::ProblemInstance& pUVRP;
    const UVRP::Evaluator& evaluator;
    const Decoder& decoder; //when decoding, fitness may be calculated or not. In UVRP with DP case, fitness will exist at deconding step;
    const UVRP::AStar& aStar;

    //internal variables
    int phase;
    int stage;
    int stageSize;
	int eliteSize;
	int mutantSize;    
    int improveSize;
	int offspringSize;    
    int chromossomeSize;    
    int sameDirectionSize;
	int eliteStartIndex, eliteEndIndex;
	int mutantStartIndex, mutantEndIndex;
	int offspringStartIndex, offspringEndIndex;
    std::uniform_int_distribution<int> eliteSizeDice, nonEliteSizeDice;

    unsigned maxGensWoImprov;

	//user support variables
	BRKGA_Population lastExecutionPopulation;
	double lastExecutionTime;

    virtual void singleMutation(std::tuple<BRKGA_RandomKeys, std::unique_ptr<BRKGA_Individual> >& solution, int start) const;
    virtual void resetParameters();
	virtual void rank(BRKGA_Population& population) const; //Rank the population and return the best.It is expected that every individual was decoded and evaluated
	virtual BRKGA_Population generateInitialPopulation() const; 
    virtual void increaseSize(BRKGA_Population& population);
	virtual void generateOffspring(BRKGA_Population& population); //fill the current population with offspring
	virtual void generateMutats(BRKGA_Population& population) const; //fill the current population with mutants
    virtual void updatePopulation(BRKGA_Population& population) const; //fill the current population with mutants
    virtual BRKGA_RandomKeys generateRandomKeys() const;    
	virtual std::pair<BRKGA_RandomKeys const*, BRKGA_RandomKeys const* > electParents(const BRKGA_Population& population);
	virtual BRKGA_RandomKeys crossover(std::pair<BRKGA_RandomKeys const*, BRKGA_RandomKeys const* >& parents) const;    
    virtual BRKGA_RandomKeys increaseRandomKeys(BRKGA_RandomKeys const& parent) const;

    virtual void generateMutats1(BRKGA_Population& population);
    virtual void generateMutats2(BRKGA_Population& population);
    virtual BRKGA_RandomKeys const * electParent(const BRKGA_Population& population);
    virtual BRKGA_RandomKeys mutate(BRKGA_RandomKeys const * parent) const;

    virtual BRKGA_RandomKeys generateInitialRandomKeys() const;
    virtual void generateMutats3(BRKGA_Population& population);
    virtual BRKGA_RandomKeys generateRandomKeysByStage(BRKGA_RandomKeys const * parent) const;
    virtual BRKGA_RandomKeys crossoverByStage(std::pair<BRKGA_RandomKeys const*, BRKGA_RandomKeys const* >& parents) const;

public:
	//Meta-heuristic parameters
	int maximumPopulationSize; //parameter to be calibrated that determines the amount of individuals that the current generation will have at maximum
	double top; //parameter to be calibrated that determines the amout o individuals that will pass selection 
	double bot; //parameter to be calibrated that determines the amout o individuals that will be introduced as mutants
	double rho; //parameter to be calibrated that will dictate elitism rate in the crossover

    CL_BRKGA(UVRP::ProblemInstance& _pUVRP, const UVRP::Evaluator& _evaluator, const Decoder& _decoder, const UVRP::AStar& _astar, int _maximumPopulationSize, int _chromossomeSize, double _top, double _bot, double _rho);
	virtual ~CL_BRKGA() = default;

    virtual UVRP_Solution exec(const unsigned numGeneration, const double timelimit);

    virtual UVRP_Solution operator()(const unsigned numGeneration);
    virtual UVRP_Solution operator()(const double timelimit);
    virtual UVRP_Solution operator()(const unsigned numGeneration, const double timelimit);

	virtual double getLastExecutionTime() const;
	virtual const BRKGA_Population& getLastExecutionPopulation() const;
};

}


#endif
