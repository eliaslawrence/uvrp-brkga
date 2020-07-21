#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core.h"

#include <iostream>
#include <ostream>

// MKDIR
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
}

MainWindow::MainWindow(QWidget *parent, int _argc, char ** _argv) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    argc(_argc),
    argv(_argv)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset_canvas()
{
    scene->clear();
    scene->setSceneRect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
}

void MainWindow::update_interval(const UVRP::ProblemInstance& instance)
{
    interval_horizontal_size = (ui->graphicsView->viewport()->width()  - 2 * MARGIN_SIZE - instance.col   * POINT_SIZE)/(instance.col   - 1);
    interval_vertical_size   = (ui->graphicsView->viewport()->height() - 2 * MARGIN_SIZE - instance.lines * POINT_SIZE)/(instance.lines - 1);
}

void MainWindow::showEvent( QShowEvent* event ) {
    QWidget::showEvent( event );
    reset_canvas();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   reset_canvas();
   if(ptrInstance){
       update_interval(*ptrInstance);
       print_solution(*ptrInstance, (*getInd((*ptrBRKGA).getLastExecutionPopulation()[0])).getR());
       print_map(*ptrInstance);
   }
}

void MainWindow::print_point(Point point)
{
    QBrush gray_brush(Qt::gray);
    QPen gray_pen(Qt::gray);

    QBrush red_brush(Qt::red);
    QPen red_pen(Qt::red);

    QBrush blue_brush(Qt::blue);
    QPen blue_pen(Qt::blue);

    QBrush green_brush(Qt::green);
    QPen green_pen(Qt::green);

    QBrush actual_brush;
    QPen actual_pen;

    switch (point.type) {
    case UVRP::NOTHING:
        actual_brush = gray_brush;
        actual_pen = gray_pen;
        break;
    case UVRP::COLLECT:
        actual_brush = red_brush;
        actual_pen = red_pen;
        break;
    case UVRP::DELIVER:
        actual_brush = blue_brush;
        actual_pen = blue_pen;
        break;
    case UVRP::RECHARGE:
        actual_brush = green_brush;
        actual_pen = green_pen;
        break;
    case UVRP::PROHIBITED:
        actual_brush = red_brush;
        actual_pen = red_pen;
        break;
    }

    scene->addEllipse(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE) * (double) point.x,
                      MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE) * (double) point.y,
                      POINT_SIZE, POINT_SIZE, actual_pen, actual_brush);
}

void MainWindow::print_map(const UVRP::ProblemInstance& instance)
{
//    ui->graphicsView->fitInView();

    QBrush blackBrush(Qt::black);
    QPen blackpen(Qt::black);


    QBrush redBrush(Qt::red);
    QPen redpen(Qt::red);

    QBrush blueBrush(Qt::blue);
    QPen bluepen(Qt::blue);

    Point pnt;
    for (int i = 0; i < instance.grid.size(); ++i) {
        for (int j = 0; j < instance.grid[i].size(); ++j) {
            pnt.x = j;
            pnt.y = i;
            pnt.type = instance.grid[i][j];
            print_point(pnt);
        }
    }
}

void MainWindow::print_solution(const UVRP::ProblemInstance& instance, const UVRP::Representation& rep)
{    

    auto update_x = [&](const unsigned& dir, const int& actual_x)->int{
        int x = actual_x;
        switch (dir) {
        case UVRP::DIRECTIONS::NE:
            x++;
            break;
        case UVRP::DIRECTIONS::E:
            x++;
            break;
        case UVRP::DIRECTIONS::SE:
            x++;
            break;
        case UVRP::DIRECTIONS::SW:
            x--;
            break;
        case UVRP::DIRECTIONS::W:
            x--;
            break;
        case UVRP::DIRECTIONS::NW:
            x--;
            break;
        }

        return x;
    };

    auto update_y = [&](const unsigned& dir, const int& actual_y)->int{
        int y = actual_y;
        switch (dir) {
        case UVRP::DIRECTIONS::N:
            y--;
            break;
        case UVRP::DIRECTIONS::NE:
            y--;
            break;
        case UVRP::DIRECTIONS::SE:
            y++;
            break;
        case UVRP::DIRECTIONS::S:
            y++;
            break;
        case UVRP::DIRECTIONS::SW:
            y++;
            break;
        case UVRP::DIRECTIONS::NW:
            y--;
            break;
        }

        return y;
    };

    srand(time(NULL));
    for (int curr_vehicle = 0; curr_vehicle < instance.v; ++curr_vehicle) {
        int x0 = instance.origin_x;
        int y0 = instance.origin_y;

        int x1, y1;

        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;
        QColor color(r, g, b, 1);

        QPen black_pen(Qt::black);
        black_pen.setWidth(LINE_SIZE);
        color.setAlpha(255);
        black_pen.setColor(color);

        int ind = 1;
        for(const std::pair<int, int>& pair : rep.pos[curr_vehicle]) {
            x1 = getX(pair);
            y1 = getY(pair);

    //        black_pen.setColor(color);
            scene->addLine(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE) * x0 + POINT_SIZE/2,
                           MARGIN_SIZE + (  interval_vertical_size + POINT_SIZE) * y0 + POINT_SIZE/2,
                           MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE) * x1 + POINT_SIZE/2,
                           MARGIN_SIZE + (  interval_vertical_size + POINT_SIZE) * y1 + POINT_SIZE/2,
                           black_pen);

            x0 = x1;
            y0 = y1;

            if(ind > rep.finalPos[curr_vehicle]){
                break;
            }

            ++ind;
        }
    }

//    std::cout << "Size: " << rep.pos.size() << std::endl;
//    std::cout << "Final Pos: " << rep.finalPos << std::endl;
}

void MainWindow::on_actionLoad_triggered()
{

}

void MainWindow::on_actionAMOVND_triggered()
{

}

void MainWindow::on_actionEvaluate_triggered()
{

}

void MainWindow::on_actionNeural_Network_triggered()
{

}

void MainWindow::set_actual_play(int index) // pointer
{
//    Solution * play = a_solutions[currentRow];

    string s_info_play;
//    s_info_play.append("Play #");
//    s_info_play.append(std::to_string(currentRow));


//    s_info_play.append("\nSize : ");

//    for (int pos_route = 0; pos_route < play->get_size(); pos_route++) {
//        s_info_play.append(std::to_string(play->uavs[pos_route]->get_size()));
//        s_info_play.append(" ");
//    }

//    s_info_play.append("\nFinal Charge : ");
//    s_info_play.append(std::to_string(play->objectives[1]));

//    s_info_play.append("\nTime : ");
//    s_info_play.append(std::to_string((-1) * play->objectives[2]));

//    s_info_play.append("\nConsumption : ");
//    s_info_play.append(std::to_string((-1) * play->objectives[3]));

//    s_info_play.append("\nIteration : ");
//    s_info_play.append(std::to_string(play->iteration));

//    s_info_play.append("\nValid : ");
//    s_info_play.append(play->valid ? "true" : "false");

//    s_info_play.append("\n\nVelocity : \n\n");
//    for (int pos_route = 0; pos_route < play->get_size(); pos_route++) {
//        for (int i = 0; i < play->uavs[pos_route]->get_size(); i++) {
//            s_info_play.append(std::to_string(play->uavs[pos_route]->get_movement(i)->vel));
//            s_info_play.append(" | ");
//        }

//        s_info_play.append("\n\n");
//    }

//    s_info_play.append("\n\nCharge Rate : \n\n");
//    Matrix * m_map = core.get_map();
//    for (int pos_route = 0; pos_route < play->get_size(); pos_route++) {
//        for (int i = 0; i < play->uavs[pos_route]->get_size(); i++) {
//            int type = m_map->get_stop_point(play->uavs[pos_route]->get_movement(i)->get_x(), play->uavs[pos_route]->get_movement(i)->get_y()).get_type();
//            if(type == STOPS::CHARGE){
//                s_info_play.append(std::to_string(play->uavs[pos_route]->get_movement(i)->charge_rate));
//                s_info_play.append(" | ");
//            }
//        }
//        s_info_play.append("\n\n");
//    }

    const UVRP_Evaluation& evaluation = *((*getInd((*ptrBRKGA).getLastExecutionPopulation()[index])).getADS().evaluation);

    s_info_play.append("Clients : ");
    s_info_play.append(std::to_string(getClientsCount(evaluation)));
    s_info_play.append("\n");
    s_info_play.append("TotalTime : ");
    s_info_play.append(std::to_string(getTime(evaluation)));
    s_info_play.append("\n");
    s_info_play.append("Consumption : ");
    s_info_play.append(std::to_string(getConsumption(evaluation)));
    s_info_play.append("\n");
    s_info_play.append("FinalCharge : ");
    s_info_play.append(std::to_string(getFinalCharge(evaluation)));
    s_info_play.append("\n");
    s_info_play.append("InvalidCharge : ");
    s_info_play.append(std::to_string(getInvChargeCount(evaluation)));
    s_info_play.append("\n");
    s_info_play.append("OutOfBounds : ");
    s_info_play.append(std::to_string(getOutOfBoundsCount(evaluation)));
    s_info_play.append("\n");
    s_info_play.append("ProhibitedPoints : ");
    s_info_play.append(std::to_string(getProhibitedCount(evaluation)));

    s_info_play.append("\n");
    s_info_play.append("Final Pos : ");
    for (int var = 0; var < ptrInstance->v; ++var) {
        s_info_play.append(std::to_string((*getInd((*ptrBRKGA).getLastExecutionPopulation()[index])).getR().finalPos[var]));
        s_info_play.append(", ");
    }

    s_info_play.append("\n");

    ui->teSelectedPlay->setText(QString::fromStdString(s_info_play));

    reset_canvas();
    print_solution(*ptrInstance, (*getInd((*ptrBRKGA).getLastExecutionPopulation()[index])).getR());
    print_map(*ptrInstance);
}

void MainWindow::on_actionBRKGA_triggered()
{
    int seed = 0, factor = 4;
    unsigned maximumPopulationSize;
    double top = 0.15, bot = 0.15, rho = 0.7;//0.3 0.15 0.7    2 0.25 0.15 0.5
    unsigned generations = 1000;
    unsigned chromossomeSize = 5;
    double timeLimit = 60.0;
    bool preProcessing = true;

//    UVRP::ProblemInstance instance(seed, "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances/eil51.tsp");
    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/A/origin_1/"));
    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

    update_interval(*ptrInstance);

    UVRP::Evaluator evaluator(*ptrInstance);
    UVRP::AStar astar(*ptrInstance, preProcessing);
    BRKGA::Decoder decoder(*ptrInstance, astar);

    maximumPopulationSize = 50;//factor*chromossomeSize;
//    BRKGA::CL_BRKGA brkga(*ptrInstance, evaluator, decoder, maximumPopulationSize, chromossomeSize, top, bot, rho);
    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator, decoder, astar, maximumPopulationSize, chromossomeSize, top, bot, rho));


    UVRP_Solution solution = (*ptrBRKGA)(timeLimit);

//	#ifdef PRINT
    if(!evaluator.is_feasible(solution)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
        std::cout << "Is infeasible" << std::endl;
    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
    std::cout << "Evaluation: " << solution.getADS() << std::endl;
//    std::cout << "Solution: " << std::endl << format(solution.getR()) << std::endl;
//    std::cout << "Seed used: " << instance.seed << std::endl;
//	#else
//	std::cout << instance.seed << ' ' << evaluator.is_feasible(solution) << std::endl;
//	std::cout << std::fixed << std::setprecision(3) << brkga.getLastExecutionTime() << std::endl;
//	std::cout << solution << std::endl;
//	#endif
//    ptrInstance = &instance;
//    ptrBRKGA    = &brkga;

//    BRKGA_Population population = brkga.getLastExecutionPopulation();
//    ptrPopulation = std::unique_ptr<BRKGA_Population> (new BRKGA_Population(population.size()));
//    ptrPopulation = std::move(population);
//    ptrPopulation = &population;

//    reset_canvas();
//    print_solution(*ptrInstance, (*getInd((*ptrBRKGA).getLastExecutionPopulation()[0])).getR());
//    print_map(*ptrInstance);

    string s_item;

    for (int i = 0; i < (*ptrBRKGA).getLastExecutionPopulation().size(); i++) {
        s_item = std::to_string(i);
        s_item.append(". ");

        std::ostringstream stream;
        stream << (*getInd((*ptrBRKGA).getLastExecutionPopulation()[i])).getR();
        s_item.append(stream.str());
        ui->listPlays->addItem(QString::fromStdString(s_item));
    }

//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing);

    print_map(*ptrInstance);



//    print_map(pntInstance);

}

void MainWindow::on_listPlays_currentRowChanged(int currentRow)
{
    set_actual_play(currentRow);
}

void MainWindow::on_listPlays_itemClicked(QListWidgetItem *item)
{
    set_actual_play(ui->listPlays->row(item));
}

void MainWindow::on_actionTest_triggered()
{
    int seed = 0, factor = 4;
    unsigned maximumPopulationSize;
    double top = 0.15, bot = 0.15, rho = 0.7;//0.3 0.15 0.7    2 0.25 0.15 0.5
    unsigned generations = 1000;
    unsigned chromossomeSize = 5;
    double timeLimit = 60.0;
    bool preProcessing = true;
    bool feasible = true;
    std::string directory_name;


    // Test 1

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/A/origin_1/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator(*ptrInstance);
//    UVRP::AStar astar(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder(*ptrInstance, astar);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator, decoder, astar, maximumPopulationSize, chromossomeSize, top, bot, rho));

//    UVRP_Solution solution = (*ptrBRKGA)(timeLimit);

//    if(!evaluator.is_feasible(solution)) {//if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//        feasible = false;
//    }
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

//    std::string directory_name = (*ptrInstance).directory_name + "brkga";

//    //Create directory to store solutions
//    mkdir(directory_name.c_str(), 0777);

//    UAV_to_file_solution(*ptrInstance, solution, feasible, directory_name);


//    ptrInstance.reset();
//    ptrBRKGA.reset();

//    // Test 2

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/A/origin_2/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator2(*ptrInstance);
//    UVRP::AStar astar2(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder2(*ptrInstance, astar2);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator2, decoder2, astar2, maximumPopulationSize, chromossomeSize, top, bot, rho));


//    UVRP_Solution solution2 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator2.is_feasible(solution2)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution2.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

//    ptrInstance.reset();
//    ptrBRKGA.reset();

//    // Test 3

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/B/origin_1/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator3(*ptrInstance);
//    UVRP::AStar astar3(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder3(*ptrInstance, astar3);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator3, decoder3, astar3, maximumPopulationSize, chromossomeSize, top, bot, rho));


//    UVRP_Solution solution3 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator3.is_feasible(solution3)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution3.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);


//    ptrInstance.reset();
//    ptrBRKGA.reset();

//    // Test 4

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/B/origin_2/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator4(*ptrInstance);
//    UVRP::AStar astar4(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder4(*ptrInstance, astar4);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator4, decoder4, astar4, maximumPopulationSize, chromossomeSize, top, bot, rho));


//    UVRP_Solution solution4 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator4.is_feasible(solution4)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution4.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);


//    ptrInstance.reset();
//    ptrBRKGA.reset();


    // ----------- EIL 101 ----------- //

    timeLimit = 10;

    // Test 1

    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/A/origin_1/"));
    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

    update_interval(*ptrInstance);

    UVRP::Evaluator evaluator5(*ptrInstance);
    UVRP::AStar astar5(*ptrInstance, preProcessing);
    BRKGA::Decoder decoder5(*ptrInstance, astar5);

    maximumPopulationSize = 50;//factor*chromossomeSize;
    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator5, decoder5, astar5, maximumPopulationSize, chromossomeSize, top, bot, rho));

    UVRP_Solution solution5 = (*ptrBRKGA)(timeLimit);

    feasible = true;
    if(!evaluator5.is_feasible(solution5)) {//if the solution isn't feasible then there's a mistake in the instance or the code itself
        std::cout << "Is infeasible" << std::endl;
        feasible = false;
    }
    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
    std::cout << "Evaluation: " << solution5.getADS() << std::endl;


    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

    directory_name = (*ptrInstance).directory_name + "brkga";

    //Create directory to store solutions
    mkdir(directory_name.c_str(), 0777);

    UAV_to_file_solution(*ptrInstance, solution5, feasible, directory_name);


    ptrInstance.reset();
    ptrBRKGA.reset();

    // Test 2

    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/A/origin_2/"));
    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

    update_interval(*ptrInstance);

    UVRP::Evaluator evaluator6(*ptrInstance);
    UVRP::AStar astar6(*ptrInstance, preProcessing);
    BRKGA::Decoder decoder6(*ptrInstance, astar6);

    maximumPopulationSize = 50;//factor*chromossomeSize;
    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator6, decoder6, astar6, maximumPopulationSize, chromossomeSize, top, bot, rho));

    UVRP_Solution solution6 = (*ptrBRKGA)(timeLimit);

    feasible = true;
    if(!evaluator6.is_feasible(solution6)) {//if the solution isn't feasible then there's a mistake in the instance or the code itself
        std::cout << "Is infeasible" << std::endl;
        feasible = false;
    }

    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
    std::cout << "Evaluation: " << solution6.getADS() << std::endl;


    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

    directory_name = (*ptrInstance).directory_name + "brkga";

    //Create directory to store solutions
    mkdir(directory_name.c_str(), 0777);

    UAV_to_file_solution(*ptrInstance, solution6, feasible, directory_name);

    ptrInstance.reset();
    ptrBRKGA.reset();

    // Test 3

    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/B/origin_1/"));
    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

    update_interval(*ptrInstance);

    UVRP::Evaluator evaluator7(*ptrInstance);
    UVRP::AStar astar7(*ptrInstance, preProcessing);
    BRKGA::Decoder decoder7(*ptrInstance, astar7);

    maximumPopulationSize = 50;//factor*chromossomeSize;
    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator7, decoder7, astar7, maximumPopulationSize, chromossomeSize, top, bot, rho));

    UVRP_Solution solution7 = (*ptrBRKGA)(timeLimit);

    feasible = true;
    if(!evaluator7.is_feasible(solution7)) {//if the solution isn't feasible then there's a mistake in the instance or the code itself
        std::cout << "Is infeasible" << std::endl;
        feasible = false;
    }

    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(7) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
    std::cout << "Evaluation: " << solution7.getADS() << std::endl;

    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

    directory_name = (*ptrInstance).directory_name + "brkga";

    //Create directory to store solutions
    mkdir(directory_name.c_str(), 0777);

    UAV_to_file_solution(*ptrInstance, solution7, feasible, directory_name);

    ptrInstance.reset();
    ptrBRKGA.reset();

    // Test 4

    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/B/origin_2/"));
    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

    update_interval(*ptrInstance);

    UVRP::Evaluator evaluator8(*ptrInstance);
    UVRP::AStar astar8(*ptrInstance, preProcessing);
    BRKGA::Decoder decoder8(*ptrInstance, astar8);

    maximumPopulationSize = 50;//factor*chromossomeSize;
    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator8, decoder8, astar8, maximumPopulationSize, chromossomeSize, top, bot, rho));

    UVRP_Solution solution8 = (*ptrBRKGA)(timeLimit);

    feasible = true;
    if(!evaluator8.is_feasible(solution8)) {//if the solution isn't feasible then there's a mistake in the instance or the code itself
        std::cout << "Is infeasible" << std::endl;
        feasible = false;
    }
    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
    std::cout << "Evaluation: " << solution8.getADS() << std::endl;

    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

    directory_name = (*ptrInstance).directory_name + "brkga";

    //Create directory to store solutions
    mkdir(directory_name.c_str(), 0777);

    UAV_to_file_solution(*ptrInstance, solution8, feasible, directory_name);

    ptrInstance.reset();
    ptrBRKGA.reset();

    // ----------- RAT 195 ----------- //

//    timeLimit = 900;

//    // Test 1

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/A/origin_1/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator9(*ptrInstance);
//    UVRP::AStar astar9(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder9(*ptrInstance, astar9);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator9, decoder9, astar9, maximumPopulationSize, chromossomeSize, top, bot, rho));

//    UVRP_Solution solution9 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator9.is_feasible(solution9)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution9.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);


//    ptrInstance.reset();
//    ptrBRKGA.reset();

//    // Test 2

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/A/origin_2/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator10(*ptrInstance);
//    UVRP::AStar astar10(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder10(*ptrInstance, astar10);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator10, decoder10, astar10, maximumPopulationSize, chromossomeSize, top, bot, rho));


//    UVRP_Solution solution10 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator10.is_feasible(solution10)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution10.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);

//    ptrInstance.reset();
//    ptrBRKGA.reset();

//    // Test 3

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/B/origin_1/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator11(*ptrInstance);
//    UVRP::AStar astar11(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder11(*ptrInstance, astar11);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator11, decoder11, astar11, maximumPopulationSize, chromossomeSize, top, bot, rho));


//    UVRP_Solution solution11 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator11.is_feasible(solution11)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(11) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution11.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);


//    ptrInstance.reset();
//    ptrBRKGA.reset();

//    // Test 4

//    ptrInstance = std::unique_ptr<UVRP::ProblemInstance>(new UVRP::ProblemInstance(seed, "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/B/origin_2/"));
//    chromossomeSize = (unsigned) (*ptrInstance).col * (*ptrInstance).lines / 2;

//    chromossomeSize = (*ptrInstance).specialPoints.size();//specialPoints

//    update_interval(*ptrInstance);

//    UVRP::Evaluator evaluator12(*ptrInstance);
//    UVRP::AStar astar12(*ptrInstance, preProcessing);
//    BRKGA::Decoder decoder12(*ptrInstance, astar12);

//    maximumPopulationSize = 50;//factor*chromossomeSize;
//    ptrBRKGA = std::unique_ptr<BRKGA::CL_BRKGA>(new BRKGA::CL_BRKGA(*ptrInstance, evaluator12, decoder12, astar12, maximumPopulationSize, chromossomeSize, top, bot, rho));


//    UVRP_Solution solution12 = (*ptrBRKGA)(timeLimit);

//    if(!evaluator12.is_feasible(solution12)) //if the solution isn't feasible then there's a mistake in the instance or the code itself
//        std::cout << "Is infeasible" << std::endl;
//    std::cout << "Run time in seconds: " << std::fixed << std::setprecision(3) << (*ptrBRKGA).getLastExecutionTime() << std::endl;
//    std::cout << "Evaluation: " << solution12.getADS() << std::endl;


//    saveToFile(maximumPopulationSize, top, bot, rho, timeLimit, preProcessing, (*ptrInstance).directory_name);


//    ptrInstance.reset();
//    ptrBRKGA.reset();
}

void MainWindow::saveToFile(unsigned maximumPopulationSize, double top, double bot, double rho, double timeLimit, bool preProcessing, string directory){
    time_t timer;
    struct tm y2k = {0};

    y2k.tm_hour = 0;
    y2k.tm_min  = 0;
    y2k.tm_sec  = 0;
    y2k.tm_year = 0;
    y2k.tm_mon  = 0;
    y2k.tm_mday = 1;

    time(&timer);

    //START: Info file
    ofstream info_file;
    info_file.open (directory + "info.txt");

    info_file << "# INSTANCE" << "\n" <<
                 ">> Clients: "              << (*ptrInstance).c                                                          << "\n" <<
                 ">> Recharge Points: "      << (*ptrInstance).r   << "(" << (*ptrInstance).pRecharge   << ")"            << "\n" <<
                 ">> Prohibited Points: "    << (*ptrInstance).p   << "(" << (*ptrInstance).pProhibited << ")"            << "\n" <<
                 ">> Vehicles: "             << (*ptrInstance).v                                                          << "\n" <<
                 ">> Origin: "               << "(" << (*ptrInstance).origin_x  << ", " << (*ptrInstance).origin_y << ")" << "\n" <<
                 ">> Discretization: "       << (*ptrInstance).discretization                                             << "\n" <<
                 ">> Initial battery: "      << (*ptrInstance).initial_battery_charge                                     << "\n" <<
                 ">> Fixed Consumption: "    << (*ptrInstance).fixed_consumption                                          << "\n" <<
                 ">> Variable Consumption: " << (*ptrInstance).variable_consumption                                       << "\n" <<
                 ">> Recharge time: "        << (*ptrInstance).recharge_time                                              << "\n" << "\n";

    info_file << "# SETTINGS" << "\n" <<
                 ">> Population Size: " << maximumPopulationSize << "\n" <<
                 ">> Top: "             << top                   << "\n" <<
                 ">> Bot: "             << bot                   << "\n" <<
                 ">> Rho: "             << rho                   << "\n" <<
                 ">> Time Limit: "      << timeLimit             << "\n" <<
                 ">> Pre Processing: "  << preProcessing         << "\n" << "\n";

    const UVRP_Evaluation& evaluation = *((*getInd((*ptrBRKGA).getLastExecutionPopulation()[0])).getADS().evaluation);

    info_file << ">> Clients: "           << std::to_string(getClientsCount(evaluation))     << "\n";
    info_file << ">> Invalid Charge: "    << std::to_string(getInvChargeCount(evaluation))   << "\n";
    info_file << ">> Out Of Bounds: "     << std::to_string(getOutOfBoundsCount(evaluation)) << "\n";
    info_file << ">> Prohibited Points: " << std::to_string(getProhibitedCount(evaluation))  << "\n";
    info_file << ">> Final Charge: "      << std::to_string(getFinalCharge(evaluation))      << "\n";
    info_file << ">> Time: "              << std::to_string(getTime(evaluation))             << "\n";
    info_file << ">> Consumption: "       << std::to_string(getConsumption(evaluation))      << "\n" << "\n";

    info_file << std::to_string(getFinalCharge(evaluation)) << " " <<
                 std::to_string(getTime(evaluation))        << " " <<
                 std::to_string(getConsumption(evaluation)) << "\n";

    info_file.close();
    //END: Info file

    //START: Aggregation file
    ofstream aggregation_file;
    aggregation_file.open (directory + "aggregation.txt");

    aggregation_file << "1 3" << "\n";

    aggregation_file << std::to_string((int) std::round(getFinalCharge(evaluation)))      << " " <<
                        std::to_string((int) std::round(-1 * getTime(evaluation)))        << " " <<
                        std::to_string((int) std::round(-1 * getConsumption(evaluation))) << "\n";

    aggregation_file.close();
    //END: Aggregation file
}

/* FILE */

void MainWindow::UAV_to_file_solution(const UVRP::ProblemInstance& instance, UVRP_Solution sol, bool valid, string directory_name)
{

    UVRP::Representation& representation = sol.getR();
    UVRP::ADS& ads = sol.getADS();

    double&   time              = getTime(*ads.evaluation);
    double&   consumption       = getConsumption(*ads.evaluation);
    double&   final_charge      = getFinalCharge(*ads.evaluation);
    unsigned& num_clients       = getClientsCount(*ads.evaluation);

    //Create directory to store solutions
    string solutions_directory_name = directory_name + "/solutions";
    mkdir(solutions_directory_name.c_str(), 0777);

    string routes_directory_name = solutions_directory_name + "/routes";
    mkdir(routes_directory_name.c_str(), 0777);

    string file_name = solutions_directory_name + "/solution_0";

    ofstream array_file;
    array_file.open (file_name);

    array_file << instance.v        << "\n";
    array_file << 4                 << "\n";
    array_file << 0                 << "\n";
    array_file << valid             << "\n";

    array_file << num_clients  << " ";
    array_file << final_charge << " ";
    array_file << time         << " ";
    array_file << consumption  << " ";

    array_file << "\n";

    array_file.close();

//    size_t found = file_name.find_last_of("/");
//    string solution_name = file_name.substr(found, file_name.size() - 1);
//    file_name.erase (found, file_name.size() - 1);
//    string routes_file_name = file_name;
//    routes_file_name.append("/routes");

    for (int pos_route = 0; pos_route < instance.v; pos_route++) {
        string route_file_name = routes_directory_name + "/solution_0_route_" + std::to_string(pos_route);

        ofstream route_file;
        route_file.open (route_file_name);

        route_file << (representation.finalPos[pos_route] + 1) << "\n";
        route_file << 4 << "\n";

        route_file << valid << "\n";

        route_file << num_clients  << " ";
        route_file << final_charge << " ";
        route_file << time         << " ";
        route_file << consumption  << " ";
        route_file << "\n";

        std::list<float>::const_iterator it_vel      = representation.vels[pos_route].begin();
        std::list<float>::const_iterator it_recharge = representation.recharge_rates[pos_route].begin();

        int it_pos = 0;
        while (it_pos <= representation.finalPos[pos_route]) {
            route_file << getX(representation.pos[pos_route][it_pos])            << " " <<
                          getY(representation.pos[pos_route][it_pos])            << " " <<
                          (*it_vel)                                              << " " <<
                          std::round((*it_recharge)*instance.max_battery_charge) << " " <<
                          100                                                    << "\n";

            it_pos++;
            it_vel++;
            it_recharge++;
        }

        route_file.close();
    }
}

