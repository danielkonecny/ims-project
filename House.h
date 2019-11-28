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
    double CountAreaEmissionsWithPercentage(double emissions_constant, double percentage);
    double CountPeopleEmissions(double emissions_constant);
    int number_of_people;
    int area;
};

#endif //IMS_HOUSE_H
