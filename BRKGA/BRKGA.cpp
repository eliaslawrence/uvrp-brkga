#include <iostream>
#include <cassert>
#include <chrono>
#include <limits>
#include <algorithm>

#include "BRKGA.h"

#ifndef NDEBUG
#define BR_DEBUG 1 //coment this to avoid debugging this code
#endif

#ifdef BR_DEBUG
#define DEBUG

std::ostream& operator<<(std::ostream& os, const BRKGA_RandomKeys& rk){
	for(const double& x : rk)
		std::cerr << x << " ";
	return os;
}


#else
#define DEBUG while(false)
#endif

using namespace BRKGA;

CL_BRKGA::CL_BRKGA(UVRP::ProblemInstance& _pUVRP, const UVRP::Evaluator& _evaluator, const Decoder& _decoder, const UVRP::AStar& _astar,
             int _maximumPopulationSize, int _chromossomeSize, double _top, double _bot, double _rho)
             : pUVRP(_pUVRP), evaluator(_evaluator), decoder(_decoder), aStar(_astar) {

    phase                 = 1;
    improveSize           = 60;
    maxGensWoImprov       = 1;
    stageSize             = _chromossomeSize;
    chromossomeSize       = 3 * pUVRP.v *_chromossomeSize;
    sameDirectionSize     = 1;
	maximumPopulationSize = _maximumPopulationSize;
	bot = _bot;
    top = _top;
	rho = _rho;
	assert(maximumPopulationSize >= 2);
	assert(bot <= 0.5 && bot >= 0.0);
	assert(top <= 0.5 && top >= 0.0);
	assert(rho >= 0.5 && rho <= 1.0);
	assert(bot*maximumPopulationSize + top*maximumPopulationSize < maximumPopulationSize);
	lastExecutionTime = 0.0;
    lastExecutionPopulation.clear();
}

//It is expected that every individual was decoded and evaluated
void CL_BRKGA::rank(BRKGA_Population& population) const {
    DEBUG{
        for(const auto& x : population){
			assert(getInd(x) != nullptr);
            assert(getFit(x) != nullptr);
		}
	}

    auto compare = [&](std::tuple<BRKGA_RandomKeys, std::unique_ptr<BRKGA_Individual> >& a, std::tuple<BRKGA_RandomKeys, std::unique_ptr<BRKGA_Individual> >& b)->bool{
        return UVRP::betterThan(*getFit(a), *getFit(b));
    };

    std::sort(std::begin(population), std::end(population), compare);

//    for(int i = eliteSize; i < maximumPopulationSize; ++i){
//        for(int j = 0; j < eliteSize; ++j){
//            if(UVRP::betterThan(*getFit(population[i]), *getFit(population[j]))){
//                std::swap(population[i],population[j]);
//                break;
//            }
//        }
//    }

	#ifdef BR_DEBUG
		std::cerr << "Population ranked: \n";
		int i = 0;
		for(const auto& x : population){
			assert(getInd(x) != nullptr);
			assert(getFit(x) != nullptr);
			std::cerr << ++i << ": " << std::setprecision(2) << getRK(x) << "\t" << *getFit(x) << "\n";
		}
	#endif
}

BRKGA_Population CL_BRKGA::generateInitialPopulation() const {

	BRKGA_Population init_pop(maximumPopulationSize);
	for(int i = 0; i < maximumPopulationSize; ++i){
        getRK(init_pop[i]) = generateInitialRandomKeys();
        getInd(init_pop[i]) = std::make_unique<BRKGA_Individual>(decoder.decode(getRK(init_pop[i]), phase));
		if(getFit(init_pop[i]) == nullptr) //the decoder may or may not fill the evaluation
			evaluator.evaluate(*getInd(init_pop[i]));

//        singleMutation(init_pop[i], 0);

        // While not valid
//        while(getOutOfBoundsCount(*getFit(init_pop[i])) > 0){
//            singleMutation(init_pop[i], 0);
//            getInd(init_pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(init_pop[i]))));
//            if(getFit(init_pop[i]) == nullptr) //the decoder may or may not fill the evaluation
//                evaluator.evaluate(*getInd(init_pop[i]));
//        }
	}
	assert(init_pop.size() == maximumPopulationSize);

	DEBUG{
		std::cerr << "Initial population generated. Population size: " << init_pop.size() << "\n";
		for(auto& x : init_pop){
			assert(getInd(x) != nullptr);
			assert(getFit(x) != nullptr);
		}
	}

	return init_pop;
}

void CL_BRKGA::updatePopulation(BRKGA_Population& pop) const {

    for(int i = 0; i < pop.size(); ++i){
//        getRK(pop[i]) = increaseRandomKeys(getRK(pop[i]));
        getInd(pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(pop[i]), phase)));
        if(getFit(pop[i]) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(pop[i]));
    }

    DEBUG{
        std::cerr << "Population of new phase updated. Population size: " << pop.size() << "\n";
        for(auto& x : pop){
            assert(getInd(x) != nullptr);
            assert(getFit(x) != nullptr);
        }
    }
}

void CL_BRKGA::generateMutats(BRKGA_Population& pop) const {
	assert(pop.size() == maximumPopulationSize);
	assert(mutantEndIndex == maximumPopulationSize);
	assert(mutantStartIndex + mutantSize == mutantEndIndex);

	for(int i = mutantStartIndex; i < mutantEndIndex; ++i){
		getRK(pop[i]) = generateRandomKeys();
        getInd(pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(pop[i]), phase)));
		if(getFit(pop[i]) == nullptr) //the decoder may or may not fill the evaluation
			evaluator.evaluate(*getInd(pop[i]));

//        singleMutation(pop[i], 0);

        // While not valid
//        while(getOutOfBoundsCount(*getFit(pop[i])) > 0){
//            singleMutation(pop[i], 0);
//            getInd(pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(pop[i]))));
//            if(getFit(pop[i]) == nullptr) //the decoder may or may not fill the evaluation
//                evaluator.evaluate(*getInd(pop[i]));
//        }
	}

	DEBUG{
		std::cerr << "Mutated from " << mutantStartIndex << " to " << mutantEndIndex << "\n";
		for(auto it = std::begin(pop) + mutantStartIndex; it != std::begin(pop) + mutantEndIndex; ++it){
			assert(getInd(*it) != nullptr);
			assert(getFit(*it) != nullptr);
		}
	}
}

void CL_BRKGA::generateMutats1(BRKGA_Population& pop) {
    assert(pop.size() == maximumPopulationSize);
    assert(mutantEndIndex == maximumPopulationSize);
    assert(mutantStartIndex + mutantSize == mutantEndIndex);

    for(int i = mutantStartIndex; i < mutantEndIndex; ++i){
       BRKGA_RandomKeys const * parent = electParent(pop);
        getRK(pop[i]) = mutate(parent);
        getInd(pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(pop[i]), phase)));
        if(getFit(pop[i]) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(pop[i]));
    }

    DEBUG{
        std::cerr << "Mutated from " << mutantStartIndex << " to " << mutantEndIndex << "\n";
        for(auto it = std::begin(pop) + mutantStartIndex; it != std::begin(pop) + mutantEndIndex; ++it){
            assert(getInd(*it) != nullptr);
            assert(getFit(*it) != nullptr);
        }
    }
}

void CL_BRKGA::generateMutats2(BRKGA_Population& pop) {
    assert(pop.size() == maximumPopulationSize);
    assert(mutantEndIndex == maximumPopulationSize);
    assert(mutantStartIndex + mutantSize == mutantEndIndex);

    for(int i = mutantStartIndex; i < mutantEndIndex; ++i){
//        getRK(pop[i]) = aStar.generateIndividual();
        getInd(pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(pop[i]), phase)));
        if(getFit(pop[i]) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(pop[i]));
    }

    DEBUG{
        std::cerr << "Mutated from " << mutantStartIndex << " to " << mutantEndIndex << "\n";
        for(auto it = std::begin(pop) + mutantStartIndex; it != std::begin(pop) + mutantEndIndex; ++it){
            assert(getInd(*it) != nullptr);
            assert(getFit(*it) != nullptr);
        }
    }
}

void CL_BRKGA::generateMutats3(BRKGA_Population& pop) {
    assert(pop.size() == maximumPopulationSize);
    assert(mutantEndIndex == maximumPopulationSize);
    assert(mutantStartIndex + mutantSize == mutantEndIndex);

    for(int i = mutantStartIndex; i < mutantEndIndex; ++i){
        BRKGA_RandomKeys const * parent = electParent(pop);
        getRK(pop[i]) = generateRandomKeysByStage(parent);
        getInd(pop[i]).reset(new BRKGA_Individual(decoder.decode(getRK(pop[i]), phase)));
        if(getFit(pop[i]) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(pop[i]));
    }

    DEBUG{
        std::cerr << "Mutated from " << mutantStartIndex << " to " << mutantEndIndex << "\n";
        for(auto it = std::begin(pop) + mutantStartIndex; it != std::begin(pop) + mutantEndIndex; ++it){
            assert(getInd(*it) != nullptr);
            assert(getFit(*it) != nullptr);
        }
    }
}

void CL_BRKGA::generateOffspring(BRKGA_Population& population) {
	assert(population.size() == maximumPopulationSize);
	assert(offspringStartIndex + offspringSize == offspringEndIndex);

	BRKGA_Population offspring(offspringSize);
	for(int i = 0; i < offspringSize; ++i){
		std::pair<BRKGA_RandomKeys const *, BRKGA_RandomKeys const *> parents = electParents(population);
        getRK(offspring[i]) = crossoverByStage(parents);
        getInd(offspring[i]) = std::make_unique<BRKGA_Individual>(decoder.decode(getRK(offspring[i]), phase));
		if(getFit(offspring[i]) == nullptr) //the decoder may or may not fill the evaluation
			evaluator.evaluate(*getInd(offspring[i]));

//        singleMutation(offspring[i], 0);

        // While not valid
//        while(getOutOfBoundsCount(*getFit(offspring[i])) > 0){
//            singleMutation(offspring[i], 0);
//            getInd(offspring[i]).reset(new BRKGA_Individual(decoder.decode(getRK(offspring[i]))));
//            if(getFit(offspring[i]) == nullptr) //the decoder may or may not fill the evaluation
//                evaluator.evaluate(*getInd(offspring[i]));
//        }
	}
	DEBUG std::cerr << "Generated " << offspring.size() << " offsprings\n";
	assert(offspring.size() == offspringSize);
	
	std::move(offspring.begin(), offspring.end(), population.begin() + offspringStartIndex);
	DEBUG{
		for(auto it = std::begin(population) + offspringStartIndex; it != std::begin(population) + offspringEndIndex; ++it){
			assert(getInd(*it) != nullptr);
			assert(getFit(*it) != nullptr);
		}
	}
}

void CL_BRKGA::increaseSize(BRKGA_Population& population) {    
    assert(population.size() == maximumPopulationSize);

    chromossomeSize       += improveSize;
    maximumPopulationSize += eliteSize;

    BRKGA_Population newPopulation(maximumPopulationSize);
    for(int i = 0; i < eliteSize; ++i){
        getRK(newPopulation[i]) = getRK(population[i]);
//        std::copy(getRK(newPopulation[i]).begin(), getRK(newPopulation[i]).end(), getRK(population[i]).begin());
        getInd(newPopulation[i]).reset(new BRKGA_Individual(*getInd(population[i])));
        if(getFit(newPopulation[i]) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(newPopulation[i]));
    }

    for(int i = 0; i < population.size(); ++i){
        getRK(newPopulation[i + eliteSize]) = increaseRandomKeys(getRK(population[i]));
        getInd(newPopulation[i + eliteSize]) = std::make_unique<BRKGA_Individual>(decoder.decode(getRK(newPopulation[i + eliteSize]), phase));
        if(getFit(newPopulation[i + eliteSize]) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(newPopulation[i + eliteSize]));

//        singleMutation(newPopulation[i + eliteSize], getRK(population[i]).size());

        // While not valid
//        while(getOutOfBoundsCount(*getFit(newPopulation[i + eliteSize])) > 0){
//            singleMutation(newPopulation[i + eliteSize], getRK(population[i]).size());
//            getInd(newPopulation[i + eliteSize]).reset(new BRKGA_Individual(decoder.decode(getRK(newPopulation[i + eliteSize]))));
//            if(getFit(newPopulation[i + eliteSize]) == nullptr) //the decoder may or may not fill the evaluation
//                evaluator.evaluate(*getInd(newPopulation[i + eliteSize]));
//        }
    }
    DEBUG std::cerr << "Generated " << newPopulation.size() << " offsprings\n";
    assert(newPopulation.size() == maximumPopulationSize);

    population.resize(maximumPopulationSize);
    std::move(newPopulation.begin(), newPopulation.end(), population.begin());
    DEBUG{
        for(auto it = std::begin(population); it != std::end(population); ++it){
            assert(getInd(*it) != nullptr);
            assert(getFit(*it) != nullptr);
        }
    }
}

void CL_BRKGA::singleMutation(std::tuple<BRKGA_RandomKeys, std::unique_ptr<BRKGA_Individual> >& solution, int start) const{
    static std::uniform_real_distribution<double> dice_01(0.0, 1.0);

    unsigned outOfBounds      = getOutOfBoundsCount(*getFit(solution));
    unsigned prohibitedPoints = getProhibitedCount(*getFit(solution));

    double tempKey;
    int randomPos = start;
    double direction = 0;

    while(randomPos % 3 != 0){
        randomPos++;
    }

//    while(getOutOfBoundsCount(*getFit(solution)) >= outOfBounds &&
//          getProhibitedCount(*getFit(solution)) >= prohibitedPoints){

//        if(randomPos != -1){
//            getRK(solution)[randomPos] = tempKey;
//        }

//        do{
//            randomPos = start + (int) (dice_01(pUVRP.pseudorandom_engine) * (getRK(solution).size() - start));
//        }while(randomPos % 3 != 0);

//        tempKey = getRK(solution)[randomPos];

//        getRK(solution)[randomPos] = dice_01(pUVRP.pseudorandom_engine);
//        getInd(solution).reset(new BRKGA_Individual(decoder.decode(getRK(solution))));
//        if(getFit(solution) == nullptr) //the decoder may or may not fill the evaluation
//            evaluator.evaluate(*getInd(solution));
//    }

    while(getOutOfBoundsCount(*getFit(solution)) > 0
          ||
          getProhibitedCount(*getFit(solution))  > 0){

        do{
            randomPos = start + (int) (dice_01(pUVRP.pseudorandom_engine) * (getRK(solution).size() - start));
        }while(randomPos % 3 != 0);
        if(randomPos == getRK(solution).size()){
            int w = 0;
        }

        tempKey = getRK(solution)[randomPos];

        getRK(solution)[randomPos] = dice_01(pUVRP.pseudorandom_engine);//direction / 8.0 + .01;

//        getRK(solution)[randomPos] = dice_01(pUVRP.pseudorandom_engine);
        getInd(solution).reset(new BRKGA_Individual(decoder.decode(getRK(solution), phase)));
        if(getFit(solution) == nullptr) //the decoder may or may not fill the evaluation
            evaluator.evaluate(*getInd(solution));

        //Not improved
        if(getOutOfBoundsCount(*getFit(solution)) >= outOfBounds
                &&
           getProhibitedCount(*getFit(solution))  >= prohibitedPoints){


            getRK(solution)[randomPos] = tempKey;
//            direction++;
        }
        else {
            outOfBounds      = getOutOfBoundsCount(*getFit(solution));
            prohibitedPoints = getProhibitedCount(*getFit(solution));

//            randomPos += 3;
//            direction = 0;
        }
    }
}

BRKGA_RandomKeys CL_BRKGA::generateRandomKeys() const {
	static std::uniform_real_distribution<double> dice_01(0.0, 1.0);
//    double randomKey;
    BRKGA_RandomKeys individual(chromossomeSize);
    for(int i = 0; i < chromossomeSize; ++i){
        individual[i] = dice_01(pUVRP.pseudorandom_engine);
//        if(i % sameDirectionSize == 0){
//            randomKey = dice_01(pUVRP.pseudorandom_engine);
//        }

//        if(i % 3 == 0){
//            individual[i] = randomKey;
//        } else {
//            individual[i] = dice_01(pUVRP.pseudorandom_engine);
//        }
    }
	return individual;
}

BRKGA_RandomKeys CL_BRKGA::generateInitialRandomKeys() const {
    static std::uniform_real_distribution<double> dice_01(0.0, 1.0);
    BRKGA_RandomKeys individual(chromossomeSize);

    for(int i = 0; i < chromossomeSize / 3; ++i){
        individual[i] = dice_01(pUVRP.pseudorandom_engine);
    }

    for(int i = chromossomeSize / 3; i < 2 * chromossomeSize / 3; ++i){
        individual[i] = 1;
    }

    for(int i = 2 * chromossomeSize / 3; i < chromossomeSize; ++i){
        individual[i] = 1;
    }

//    for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//        individual[i] = dice_01(pUVRP.pseudorandom_engine);
//    }

//    for(int i = stageSize + (pUVRP.v - 1); i < 2 * stageSize + (pUVRP.v - 1); ++i){
//        individual[i] = 1;
//    }

//    for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//        individual[i] = 1;
//    }

    return individual;
}

BRKGA_RandomKeys CL_BRKGA::generateRandomKeysByStage(BRKGA_RandomKeys const* parent) const {
    static std::uniform_real_distribution<double> dice_01(0.0, 1.0);

    const BRKGA_RandomKeys& elite_parent = *parent;

    BRKGA_RandomKeys offspring(chromossomeSize);

    for(int i = 0; i < stage * chromossomeSize / 3; ++i){
        offspring[i] = elite_parent[i];
    }

    for(int i = stage * chromossomeSize / 3; i < (stage + 1) * chromossomeSize / 3; ++i){
        offspring[i] = dice_01(pUVRP.pseudorandom_engine);
    }

    for(int i = (stage + 1) * chromossomeSize / 3; i < chromossomeSize; ++i){
        offspring[i] = elite_parent[i];
    }


//    switch (stage) {
//    case 0:
//        for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = dice_01(pUVRP.pseudorandom_engine);
//        }

//        for(int i = stageSize + (pUVRP.v - 1); i < 2 * stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//            offspring[i] = elite_parent[i];
//        }
//        break;
//    case 1:
//        for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = stageSize + (pUVRP.v - 1); i < 2 * stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = dice_01(pUVRP.pseudorandom_engine);
//        }

//        for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//            offspring[i] = elite_parent[i];
//        }
//        break;
//    case 2:
//        for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = stageSize + (pUVRP.v - 1); i < 2 * stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//            offspring[i] = dice_01(pUVRP.pseudorandom_engine);
//        }
//        break;
//    }



    return offspring;
}

std::pair<BRKGA_RandomKeys const *, BRKGA_RandomKeys const *> CL_BRKGA::electParents(const BRKGA_Population& population) {
	assert(population.size() == maximumPopulationSize);
    int roll1 = eliteSizeDice(pUVRP.pseudorandom_engine);
    int roll2 = nonEliteSizeDice(pUVRP.pseudorandom_engine);
	std::pair<BRKGA_RandomKeys const *, BRKGA_RandomKeys const *> parents = {&getRK(population[roll1]), &getRK(population[roll2])};
	return parents;
}

BRKGA_RandomKeys const * CL_BRKGA::electParent(const BRKGA_Population& population) {
    assert(population.size() == maximumPopulationSize);
    int roll = eliteSizeDice(pUVRP.pseudorandom_engine);

    return &getRK(population[roll]);
}

BRKGA_RandomKeys CL_BRKGA::crossover(std::pair<BRKGA_RandomKeys const *, BRKGA_RandomKeys const *>& parents) const {
	static std::uniform_real_distribution<double> dice_01(0.0, 1.0);
	assert(parents.first);
	assert(parents.second);

	const BRKGA_RandomKeys& elite_parent = *parents.first;
	const BRKGA_RandomKeys& non_elite_parent = *parents.second;

//    int smallerSize = elite_parent.size() > non_elite_parent.size() ? non_elite_parent.size() : elite_parent.size();
//    int biggerSize  = elite_parent.size() < non_elite_parent.size() ? non_elite_parent.size() : elite_parent.size();

    BRKGA_RandomKeys offspring(chromossomeSize);

    for(int i = 0; i < chromossomeSize; ++i){
        double roll = dice_01(pUVRP.pseudorandom_engine);
		offspring[i] = roll <= rho ? elite_parent[i] : non_elite_parent[i];
	}

//    if(elite_parent.size() == biggerSize){
//        for(int i = smallerSize; i < biggerSize; ++i){
//            offspring[i] = elite_parent[i];
//        }
//    } else {
//        for(int i = smallerSize; i < biggerSize; ++i){
//            offspring[i] = non_elite_parent[i];
//        }
//    }


	return offspring;
}

BRKGA_RandomKeys CL_BRKGA::crossoverByStage(std::pair<BRKGA_RandomKeys const *, BRKGA_RandomKeys const *>& parents) const {
    static std::uniform_real_distribution<double> dice_01(0.0, 1.0);
    assert(parents.first);
    assert(parents.second);

    const BRKGA_RandomKeys& elite_parent = *parents.first;
    const BRKGA_RandomKeys& non_elite_parent = *parents.second;

    BRKGA_RandomKeys offspring(chromossomeSize);

//    switch (stage) {
//    case 0:
//        for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//            double roll = dice_01(pUVRP.pseudorandom_engine);
//            offspring[i] = roll <= rho ? elite_parent[i] : non_elite_parent[i];
//        }

//        for(int i = stageSize; i < 2 * stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//            offspring[i] = elite_parent[i];
//        }
//        break;
//    case 1:
//        for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = stageSize; i < 2 * stageSize + (pUVRP.v - 1); ++i){
//            double roll = dice_01(pUVRP.pseudorandom_engine);
//            offspring[i] = roll <= rho ? elite_parent[i] : non_elite_parent[i];
//        }

//        for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//            offspring[i] = elite_parent[i];
//        }
//        break;
//    case 2:
//        for(int i = 0; i < stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = stageSize; i < 2 * stageSize + (pUVRP.v - 1); ++i){
//            offspring[i] = elite_parent[i];
//        }

//        for(int i = 2 * stageSize + (pUVRP.v - 1); i < chromossomeSize; ++i){
//            double roll = dice_01(pUVRP.pseudorandom_engine);
//            offspring[i] = roll <= rho ? elite_parent[i] : non_elite_parent[i];
//        }
//        break;
//    }


    for(int i = 0; i < stage * chromossomeSize / 3; ++i){
        offspring[i] = elite_parent[i];
    }

    for(int i = stage * chromossomeSize / 3; i < (stage + 1) * chromossomeSize / 3; ++i){
        double roll = dice_01(pUVRP.pseudorandom_engine);
        offspring[i] = roll <= rho ? elite_parent[i] : non_elite_parent[i];
    }

    for(int i = (stage + 1) * chromossomeSize / 3; i < offspring.size(); ++i){
        offspring[i] = elite_parent[i];
    }


    return offspring;
}

BRKGA_RandomKeys CL_BRKGA::mutate(BRKGA_RandomKeys const * parent) const {
    static std::uniform_real_distribution<double> dice_01(0.0, 1.0);
    assert(parent);

    const BRKGA_RandomKeys& elite_parent = *parent;
    BRKGA_RandomKeys mutant(elite_parent.size());

    int roll   = dice_01(pUVRP.pseudorandom_engine);
    int origin = roll > rho ? 0 : -1 * sameDirectionSize;
    double randomKey;

    for(int i = 0; i < mutant.size(); ++i){
        if(i < origin + sameDirectionSize){
            if(i % 3 == 0){
                mutant[i] = randomKey;
            } else {
                mutant[i] = dice_01(pUVRP.pseudorandom_engine);
            }
        } else {
            mutant[i] = elite_parent[i];

            roll = dice_01(pUVRP.pseudorandom_engine);
            if(roll > rho){
                origin = i;
                randomKey = dice_01(pUVRP.pseudorandom_engine);
            }
        }
    }

    return mutant;
}

BRKGA_RandomKeys CL_BRKGA::increaseRandomKeys(BRKGA_RandomKeys const& parent) const {
    static std::uniform_real_distribution<double> dice_01(0.0, 1.0);
    double randomKey;

    BRKGA_RandomKeys offspring(chromossomeSize);
//    const BRKGA_RandomKeys& actual_parent = *parent;
//	const BRKGA_RandomKeys& non_elite_parent = *parents.second;

    for(int i = 0; i < parent.size(); ++i){
        offspring[i] = parent[i];
    }

    for(int i = parent.size(); i < offspring.size(); ++i){
        if((i - parent.size()) % sameDirectionSize == 0){
            randomKey = dice_01(pUVRP.pseudorandom_engine);
        }

        if(i % 3 == 0){
            offspring[i] = randomKey;
        } else {
            offspring[i] = dice_01(pUVRP.pseudorandom_engine);
        }
    }

    return offspring;
}

UVRP_Solution CL_BRKGA::operator()(const unsigned numGeneration){
	return exec(numGeneration, std::numeric_limits<double>::max());
}

UVRP_Solution CL_BRKGA::operator()(const double timelimit){
	return exec(std::numeric_limits<int>::max(), timelimit);
}

UVRP_Solution CL_BRKGA::operator()(const unsigned numGeneration, const double timelimit){
	return exec(numGeneration, timelimit);
}

void CL_BRKGA::resetParameters(){

    eliteSize = top*maximumPopulationSize;
    mutantSize = bot*maximumPopulationSize;
    offspringSize = maximumPopulationSize - eliteSize - mutantSize;

    eliteStartIndex = 0; eliteEndIndex = eliteSize;
    offspringStartIndex = eliteEndIndex; offspringEndIndex = eliteEndIndex + offspringSize;
    mutantStartIndex = offspringEndIndex; mutantEndIndex = offspringEndIndex + mutantSize;

    eliteSizeDice    = std::uniform_int_distribution<int>(eliteStartIndex, eliteEndIndex-1);
    nonEliteSizeDice = std::uniform_int_distribution<int>(eliteEndIndex, maximumPopulationSize-1);

    DEBUG{
        std::cerr << "Calculated sizes (elite/offspring/mutant): " << eliteSize << " " << offspringSize << " " << mutantSize << "\n";
        std::cerr << "Calculated Indexes (elite/offspring/mutant): " << eliteStartIndex << " " << eliteEndIndex << " / " << offspringStartIndex << " " << offspringEndIndex << " / " << mutantStartIndex << " " << mutantEndIndex << "\n";
    }
    assert(eliteSize > 1 && eliteSize < maximumPopulationSize/2);
    assert(mutantSize > 1 && mutantSize < maximumPopulationSize/2);
    assert(offspringSize > 0);
    assert(eliteSize + mutantSize + offspringSize == maximumPopulationSize);

}

UVRP_Solution CL_BRKGA::exec(const unsigned numGeneration, const double timelimit){
	DEBUG std::cerr << "Starting BRKGA with " << numGeneration << " max generations and " << timelimit << " max seconds\n";
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::duration<double> execution_duration(end - start);

    resetParameters();

	DEBUG std::cerr << "Starting generation 0. Generating initial population\n";

	BRKGA_Population current_population = generateInitialPopulation();
	DEBUG std::cerr << "Initial population size: " << current_population.size() << "\n";
	assert(current_population.size() == maximumPopulationSize);

	DEBUG std::cerr << "Ranking population\n";
	rank(current_population);

    DEBUG std::cerr << "Generation 0 finished. Most fit: " << (*getFit(current_population[0])) << "\n";

    UVRP_Solution best_solution(*getInd(current_population[0])); //deep copy best solution

    unsigned bigger_gens_wo_improv = 0, gens_wo_improv = 0, last_gen_improv = 0;

    stage = 0;
    std::chrono::steady_clock::time_point stage_start = start;
    std::chrono::duration<double> stage_duration(end - start);
    int genPerStage = 20;
    int stageInit = 1;

    int g1;
    for(g1 = 1; g1 < numGeneration; ++g1){
//        stage = (numGeneration - 1) % 3;
//        if(g1 - stageInit > genPerStage){
//            stage = stage + 1 ? stage < 2 : 0;
//        }


        DEBUG std::cerr << "Starting generation " << g1 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

		DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats3(current_population);
		assert(current_population.size() == maximumPopulationSize);
		
		DEBUG std::cerr << "Ranking population\n";
		rank(current_population);		

        DEBUG std::cerr << "Generation " << g1 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g1 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g1;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

//        if(gens_wo_improv > maxGensWoImprov){
//            gens_wo_improv = 0;

//            DEBUG std::cerr << "Increase size of population\n";
//            increaseSize(current_population);

//            resetParameters();

//            DEBUG std::cerr << "Ranking population\n";
//            rank(current_population);

//            best_solution = *getInd(current_population[0]); //deep copy best solution
//        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;        

        if(execution_duration.count() > 5 * timelimit / 8){
            break;
        }

//        stage_duration = end - stage_start;
//        if(stage_duration.count() > timelimit / 3){
////            break;
//            stage_start = std::chrono::steady_clock::now();
//            stage++;
//        }
	}

    stage = 1;
    for(g1; g1 < numGeneration; ++g1){

        DEBUG std::cerr << "Starting generation " << g1 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats3(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Ranking population\n";
        rank(current_population);

        DEBUG std::cerr << "Generation " << g1 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g1 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g1;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;

        if(execution_duration.count() > 3 * timelimit / 8){
            break;
        }
    }

    stage = 2;
    for(g1; g1 < numGeneration; ++g1){

        DEBUG std::cerr << "Starting generation " << g1 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats3(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Ranking population\n";
        rank(current_population);

        DEBUG std::cerr << "Generation " << g1 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g1 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g1;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;

        if(execution_duration.count() > 4 * timelimit / 8){
            break;
        }
    }

    phase = 2;
//    bigger_gens_wo_improv = 0, gens_wo_improv = 0, last_gen_improv = 0;

    DEBUG std::cerr << "Updating population for phase 2\n";
    updatePopulation(current_population);

    DEBUG std::cerr << "Ranking population\n";
    rank(current_population);

    best_solution = *getInd(current_population[0]); //deep copy best solution

    start = std::chrono::steady_clock::now();
    end = std::chrono::steady_clock::now();
    execution_duration = end - start;

    stage = 0;
    for(g1 = 1; g1 < 0; ++g1){
        DEBUG std::cerr << "Starting generation " << g1 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats3(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Ranking population\n";
        rank(current_population);

        DEBUG std::cerr << "Generation " << g1 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g1 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g1;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;

        if(execution_duration.count() > 6 * timelimit / 8){
            break;
        }
    }

    stage = 1;
    for(g1; g1 < numGeneration; ++g1){

        DEBUG std::cerr << "Starting generation " << g1 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats3(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Ranking population\n";
        rank(current_population);

        DEBUG std::cerr << "Generation " << g1 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g1 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g1;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;

        if(execution_duration.count() > 6 * timelimit / 8){//7 * timelimit / 8){
            break;
        }
    }

    stage = 2;
    stage = 0;
    for(g1; g1 < numGeneration; ++g1){
        stage = numGeneration % 2 + 1;

        DEBUG std::cerr << "Starting generation " << g1 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats3(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Ranking population\n";
        rank(current_population);

        DEBUG std::cerr << "Generation " << g1 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g1 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g1;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;

        if(execution_duration.count() > timelimit){
            break;
        }
    }

    int g2;
    for(g2 = 1; g2 < 0; ++g2){
        DEBUG std::cerr << "Starting generation " << g2 << "\n";

        DEBUG std::cerr << "Generating offspring\n";
        generateOffspring(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Introducing mutants to current population\n";
        generateMutats(current_population);
        assert(current_population.size() == maximumPopulationSize);

        DEBUG std::cerr << "Ranking population\n";
        rank(current_population);

        DEBUG std::cerr << "Generation " << g2 << "end. Most fit: " << (*getFit(current_population[0])) << "\n";

        std::cout << "Generation " << g2 << "end. Most fit: " << (*getInd(current_population[0])).getR() << "\n";

        if(UVRP::betterThan(*getFit(current_population[0]), *best_solution.getADS().evaluation)){
            if(gens_wo_improv > bigger_gens_wo_improv){
                bigger_gens_wo_improv = gens_wo_improv;
            }
            gens_wo_improv = 0;
            last_gen_improv = g2;
            best_solution = *getInd(current_population[0]); //deep copy best solution
        }else{
            gens_wo_improv++;
        }

        end = std::chrono::steady_clock::now();
        execution_duration = end - start;

        if(execution_duration.count() > timelimit){
            break;
        }
    }

    std::cout << "Last Generation PHASE 1: " << g1 << '\n';
    std::cout << "Last Generation PHASE 2: " << g2 << '\n';
    std::cout << "Generation best solution: " << last_gen_improv << '\n';
    std::cout << "Generation without improvement: " << bigger_gens_wo_improv << '\n';

    end = std::chrono::steady_clock::now();
    execution_duration = end - start;
	lastExecutionTime = execution_duration.count();
	DEBUG{
        std::cerr << "BRKGA ended in " << lastExecutionTime << " seconds with " << (*getFit(current_population[0])) << " most fit individual\n";
		std::cerr << "Keeping population and copying best solution\n";
	}

//    UVRP_Solution best_solution(*getInd(current_population[0])); //deep copy best solution
    best_solution = *getInd(current_population[0]);
	lastExecutionPopulation.clear();
    std::move(current_population.begin(), current_population.end(), std::back_inserter(lastExecutionPopulation)); //fill the last execution population

	return best_solution;
}

double CL_BRKGA::getLastExecutionTime() const {
	return lastExecutionTime;
}

const BRKGA_Population& CL_BRKGA::getLastExecutionPopulation() const {
	return lastExecutionPopulation;
}
