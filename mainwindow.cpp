#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

struct Product
{
    int profit;
    int shelf_space_required;
    int transportation_cost;
    int employee_training_cost;
    int employee_training_required;
    bool green_certified;
    double waste_generation;  
    double energy_usage;     
};

struct EnvironmentalData 
{
    double transportation_cost;
    double energy_usage;
    double waste_generation;
};

const double carbonConversionFactor = 1.0;

double retailShelfOptimization(vector<Product>& products, int capacity, int transportationBudget, int trainingBudget, EnvironmentalData& data, vector<int>& selectedProducts)
{
    int n = products.size();
    vector<vector<vector<int>>>
        dp(n + 1, vector<vector<int>>(trainingBudget + 1, vector<int>(transportationBudget + 1, 0)));

    vector<vector<vector<int>>> selected(n + 1, vector<vector<int>>(trainingBudget + 1, vector<int>(transportationBudget + 1, 0)));


    for (int i = 1; i <= n; i++)
    {
        for (int t = 0; t <= trainingBudget; t++)
        {
            for (int w = 0; w <= transportationBudget; w++)
            {
                if (products[i - 1].shelf_space_required <= capacity &&
                    t >= products[i - 1].employee_training_required &&
                    w >= products[i - 1].transportation_cost)
                {
                    double additionalWaste = products[i - 1].waste_generation;
                    double additionalEnergy = products[i - 1].energy_usage;

                    dp[i][t][w] = max(
                        dp[i - 1][t][w],
                        dp[i - 1][t - products[i - 1].employee_training_required][w - products[i - 1].transportation_cost] + products[i - 1].profit
                        );

                    if (dp[i][t][w] > dp[i - 1][t][w])
                    {
                        dp[i][t][w] += additionalWaste;
                        data.energy_usage += additionalEnergy;
                        selected[i][t][w] = true;
                    }
                }
                else
                {
                    dp[i][t][w] = dp[i - 1][t][w];
                }
            }
        }
    }

    int maxProfit = dp[n][trainingBudget][transportationBudget];
    double carbonFootprint = data.transportation_cost * carbonConversionFactor;
    double energyUsage = data.energy_usage;
    double wasteGeneration = data.waste_generation;

    cout << "Maximum Profit: " << maxProfit << endl;
    cout << "Carbon Footprint: " << carbonFootprint << " kg CO2e" << endl;
    cout << "Energy Usage: " << energyUsage << " units" << endl;
    cout << "Waste Generation: " << wasteGeneration << " units" << endl;

    int t = trainingBudget, w = transportationBudget;
    for (int i = n; i >= 1; i--)
    {
        if (selected[i][t][w])
        {
            selectedProducts.push_back(i - 1);
            t -= products[i - 1].employee_training_required;
            w -= products[i - 1].transportation_cost;
        }
    }

    return maxProfit;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->optimizeButton_2, &QPushButton::clicked, this, &MainWindow::performOptimization);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::performOptimization() 
{
    int capacity = ui->capacityInput_2->text().toInt();
    int transportationBudget = ui->transportationBudgetInput_2->text().toInt();
    int trainingBudget = ui->trainingBudgetInput_2->text().toInt();


    EnvironmentalData environmentalData;
    environmentalData.energy_usage = ui->energy->text().toInt();
    environmentalData.transportation_cost = 100;
    environmentalData.waste_generation = 50;
    vector<Product> products = {
        {60, 10, 5, 20, 1, true},//profit,shelfspace,transportation,employee training,training cost,green certified
        {100, 20, 10, 30, 0, false},
        {120, 30, 15, 40, 1, true},
        {80, 15, 8, 25, 0, true},
        {90, 18, 12, 35, 1, true}
    };

    vector<int> selectedProducts;
    int maxProfit = retailShelfOptimization(products, capacity, transportationBudget, trainingBudget, environmentalData, selectedProducts);
    double carbonFootprint = environmentalData.transportation_cost * carbonConversionFactor;
    ui->label_4->setText("Carbon Footprint: " + QString::number(carbonFootprint));
    ui->maxProfitLabel_2->setText("Max Profit: " + QString::number(maxProfit));
    ui->label_6->setText("Energy Usage: " +QString::number(environmentalData.energy_usage));
    ui->label_8->setText("Waste generation: "+QString::number(environmentalData.waste_generation ));
    ui->opt->setText("Selected Products:\n");

    for (int i : selectedProducts) {
        QString productInfo = "Product " + QString::number(i) + ": " + QString::number(products[i].profit) + " profit";
        ui->opt->setText(ui->opt->text() + "\n" + productInfo);
    }

}
