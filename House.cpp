//
// Created by danielkonecny on 28.11.19.
//

#include "House.h"

const int hours_per_day = 24;
const int days_per_year = 365;
const int boiler_power_for_squared_meter = 100;
const int liters_per_person = 96;
const int year_wh_per_squared_meter = 110e3;
const double year_wh_per_people[11] = {
        0e3,
        1430e3,
        2580e3,
        3720e3,
        4590e3,
        5448e3,
        6309e3,
        7169e3,
        8029e3,
        8889e3,
        9750e3
};

void House::Print() {
    cout << number_of_people << ": " << area << " m3 (" << distance << " m)" << endl;
}

//
double House::CountEmissions(double emissions_constant) {
    // g         g/Wh                    Wh             variable      variable            Wh
    return emissions_constant * (year_wh_per_people[number_of_people] + area * year_wh_per_squared_meter);
}

double House::CountAreaEmissionsWithPercentage(double emissions_constant, double heating_percentage) {
    double heat_loss = area * boiler_power_for_squared_meter;
    return emissions_constant * heating_percentage * heat_loss * hours_per_day;
}

double House::CountPeopleEmissions(double emissions_constant) {
    // g         g/Wh         *         Wh             variable      /    constant
    return emissions_constant * year_wh_per_people[number_of_people] / days_per_year;
}

double House::CountHouseHeatLossPerDay(double heating_percentage) {
    double heat_loss = area * boiler_power_for_squared_meter;
    return heating_percentage * heat_loss * hours_per_day;
}

double House::CountPeopleHeatLossPerDay() {
    return year_wh_per_people[number_of_people] / days_per_year;
}

double House::CountPeopleLitersLossPerDay() {
    return number_of_people * liters_per_person;
}
