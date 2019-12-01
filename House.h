//
// Created by danielkonecny on 28.11.19.
//

#ifndef IMS_HOUSE_H
#define IMS_HOUSE_H

#include <iostream>

using namespace std;

class House {
public:
    void Print();
    double CountEmissions(double emissions_constant);
    double CountAreaEmissionsWithPercentage(double emissions_constant, double heating_percentage);
    double CountPeopleEmissions(double emissions_constant);
    double CountHouseHeatLossPerDay(double heating_percentage);
    double CountPeopleHeatLossPerDay();
    double CountPeopleLitersLossPerDay();
    int number_of_people;
    int area;
    int distance;
};

#endif //IMS_HOUSE_H
