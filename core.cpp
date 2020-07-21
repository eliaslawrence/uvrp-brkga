#include "core.h"

#define PRINT 1
//#define NDEBUG 1

#ifndef NDEBUG
#define MN_DEBUG 1 //uncomment to activate debug mode
#endif

#ifdef MN_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

Core::Core(int _argc, char ** _argv):
    argc(_argc),
    argv(_argv)
{
}

void Core::run_BRKGA(int argc, char** argv){
    int seed = 0, factor = 4;
    unsigned maximumPopulationSize;
    double top = 0.3, bot = 0.15, rho = 0.7;
    unsigned generations = 50;
    unsigned chromossomeSize = 5;
    double timeLimit = 120.0;

    auto parse_args = [&](int argc, char** argv){
        for(int i = 1; i < argc; ++i){
            std::string arg = argv[i];
            if(arg == "-s" || arg == "-seed")
                seed = atoi(argv[++i]);
            else if(arg == "-t" || arg == "-top")
                top = atof(argv[++i]);
            else if(arg == "-b" || arg == "-bot")
                bot = atof(argv[++i]);
            else if(arg == "-r" || arg == "-rho")
                rho = atof(argv[++i]);
            else if(arg == "-f" || arg == "-factor")
                factor = atoi(argv[++i]);
            else if(arg == "-g" || arg == "-generations")
                generations = atoi(argv[++i]);
            else if(arg == "-h" || arg == "-help"){
                usage(argv[0]);
                exit(0);
            }
            else{
                DEBUG std::cerr << "Unknown param: " << arg << "\n";
                assert(1); //you shouldn't execute this else
                exit(1);
            }
        }
    };

    DEBUG std::cerr << "Parsing args\n";
    parse_args(argc, argv);
    DEBUG std::cerr << "Starting with seed: " << seed << "\n";

    UVRP::ProblemInstance instance(seed, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances/eil51.tsp");
    DEBUG std::cerr << instance << "\n";

    UVRP::Evaluator evaluator(instance);
//    UVRP::DP uvrp(instance);
//    BRKGA::Decoder decoder(instance);

    maximumPopulationSize = factor*chromossomeSize;//factor*instance.c;
    DEBUG std::cerr << "Configuring BRKGA with: " << maximumPopulationSize << " " << chromossomeSize << " " << top << " " << bot << " " << rho << "\n";
//    BRKGA::CL_BRKGA brkga(instance, evaluator, decoder, maximumPopulationSize, chromossomeSize, top, bot, rho);

    DEBUG std::cerr << "Running BRKGA\n";
//    UVRP_Solution solution = brkga(timeLimit);

//	#ifdef PRINT
//    if(!evaluator.is_feasible(solution)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << brkga.getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution.getADS() << std::endl;
//    std::cout << "Solution: " << std::endl << format(solution.getR()) << std::endl;
//    std::cout << "Seed used: " << instance.seed << std::endl;
//	#else
//	std::cout << instance.seed << ' ' << evaluator.is_feasible(solution) << std::endl;
//	std::cout << std::fixed << std::setprecision(3) << brkga.getLastExecutionTime() << std::endl;
//	std::cout << solution << std::endl;
//	#endif

}

void Core::usage(const char* name) const{
    std::cout << "Usage: " << name << " <option(s)> " << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "-h | -help\t\t\tShow this message." << std::endl;
    std::cout << "-s | -seed\t[0, INT_MAX]\tSet  mersenne twister seed. Default: 0" << std::endl;
    std::cout << "-t | -top\t]0, 1[\t\tSet TOP param. Default: 0.3" << std::endl;
    std::cout << "-b | -bot\t[0, 1[\t\tSet BOT param. Default: 0.15" << std::endl;
    std::cout << "-r | -rho\t[0.5, 1[\tSet RHO param. Default: 0.7" << std::endl;
    std::cout << "-f | -factor\t[0, INT_MAX]\tSet population size to #clients*FACTOR param. Default: #clients*4" << std::endl;
    std::cout << "-g | -gen\t[0, UINT_MAX]\tSet maximum number of generations. Default: 200" << std::endl;
    std::cout << "Instance should be passed to stdin." << std::endl;
    std::cout << std::endl;
}

std::string Core::format(const UVRP::Representation rep) {
    std::stringstream ss;

    //    const char * Directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

    //    ss << "Tour 1: ";
    ////    std::list<unsigned>::const_iterator it_d = rep.directions.begin();
    //    std::list<float>::const_iterator    it_v = rep.vels.begin();
    //    std::list<float>::const_iterator    it_r = rep.recharge_rates.begin();

    //    while(it_v != rep.vels.end() && it_r != rep.recharge_rates.end()){
    ////        ss << std::fixed << std::setprecision(3) << Directions[*it_d] << " (" << *it_v << ", " << *it_r << ") ";

    ////        ++it_d;
    //        ++it_v;
    //        ++it_r;
    //    }

    return ss.str();
}

void Core::operator()() {
        return run_BRKGA(argc, argv);
}
