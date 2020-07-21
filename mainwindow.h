#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <memory>
#include <utility>
#include <time.h>
#include <stdlib.h>
#include <cmath>

#include "UVRP/ProblemInstance.h"
#include "BRKGA/BRKGA.h"
#include "UVRP/Solution.hpp"
#include "BRKGA/Common.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    MainWindow(QWidget *parent, int argc, char *argv[]);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene * scene;

    int argc;
    char **argv;

    struct Point{
        int x, y, type;
    };

    const double POINT_SIZE  =  4;
    const int LINE_SIZE   =  2;
    const double MARGIN_SIZE = 2;

    int QTY_COLUMNS = 20;
    int QTY_LINES   = 20;
    double interval_horizontal_size;
    double interval_vertical_size;

//    UVRP::ProblemInstance * ptrInstance;
    std::unique_ptr<UVRP::ProblemInstance> ptrInstance;
    std::unique_ptr<BRKGA_Population>      ptrPopulation;
//    std::unique_ptr<BRKGA_Population>      ptrPopulation;
    std::unique_ptr<BRKGA::CL_BRKGA>       ptrBRKGA;

    void update_interval(const UVRP::ProblemInstance& instance);
    void print_point    (Point point);
    void print_map      (const UVRP::ProblemInstance& instance);
    void print_solution (const UVRP::ProblemInstance& instance, const UVRP::Representation& rep);
    void reset_canvas   ();
    void set_actual_play(int currentRow);

    void saveToFile          (unsigned maximumPopulationSize, double top, double bot, double rho, double timeLimit, bool preProcessing, string directory);
    void UAV_to_file_solution(const UVRP::ProblemInstance& instance, UVRP_Solution sol, bool valid, string directory_name);

protected:
    void showEvent(QShowEvent *ev);
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_actionLoad_triggered();
    void on_actionAMOVND_triggered();
    void on_actionEvaluate_triggered();
    void on_actionNeural_Network_triggered();
    void on_actionBRKGA_triggered();
    void on_listPlays_currentRowChanged(int currentRow);
    void on_listPlays_itemClicked(QListWidgetItem *item);
    void on_actionTest_triggered();
};

#endif // MAINWINDOW_H
