//
// Created by danielkonecny on 28.11.19.
//

#include "House.h"

void House::Print() {
    cout << number_of_people << ": " << area << " m3" << endl;
}

double House::CountEmissions(double emissions_constant) {
    return emissions_constant * (number_of_people * 1430e3 + area * 100e3); // constant * Wh
}

double House::CountAreaEmissionsWithPercentage(double emissions_constant, double percentage) {
    return emissions_constant * (number_of_people * 1430e3 + percentage * area * 24 * 14e3) / 365; // constant * Wh
}

double House::CountPeopleEmissions(double emissions_constant) {
    return emissions_constant * number_of_people * 1430e3 / 365; // constant * Wh
}
