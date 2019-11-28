/**
 * @project			Carbon Footprint in Energetics and Heating Industry
 * @file			model.cpp
 * @version 		0.1
 * @course			IMS - Modelling and Simulation
 * @organisation	Brno University of Technology - Faculty of Information Technology
 * @author			Daniel Konecny (xkonec75), Filip Jerabek (xjerab24)
 * @date			25. 11. 2019
 */

#include <iostream>
#include <vector>
#include <random>

#include "simlib.h"

#include "House.h"

using namespace std;

#define PI 3.14159

void generate_houses(int min_area, int max_area, int min_people, int max_people, int number_of_houses,
                     vector<House> *houses) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> people(min_people, max_people);
    uniform_int_distribution<int> area(min_area, max_area);

    for (int i = 0; i < number_of_houses; i++) {
        House h{};
        h.number_of_people = people(mt);
        h.area = area(mt);
        houses->push_back(h);
    }
}

double get_temperature(int day) {
    double temperature;

    random_device rd;
    mt19937 mt(rd());
    normal_distribution<double> deviation(0, 2.5);

    temperature = 11 * sin(2 * PI * day / 365) + 9;
    temperature += deviation(mt);

    return temperature;
}

int main(int argc, char *argv[]) {
    vector<House> houses;

    double gas_emissions = 0;
    const double gas_emissions_constant = 0.2;

    generate_houses(30, 120, 1, 6, 50, &houses);

    bool heating_on = true;
    double temperature_yesterday = get_temperature(0);

    for (int i = 1; i <= 365; i++) {
        double heating_percentage = 0;
        double temperature = get_temperature(i);

        if (temperature <= 13 && temperature_yesterday <= 13) {
            heating_on = true;
        } else if (temperature >= 13 && temperature_yesterday >= 13) {
            heating_on = false;
        }

        if (heating_on) {
            if (temperature < -7) {
                heating_percentage = 1;
            } else if (temperature > 13) {
                heating_percentage = 0;
            } else {
                heating_percentage = (13 - temperature) / 20;
            }
        }

        if (heating_on) {
            cout << "Yesterday: " << temperature_yesterday << " °C\t"
                 << " - Today: " << temperature << " °C\t"
                 << " - Heating: " << heating_percentage << " %.\t"
                 << " - Emissions: "<< gas_emissions << endl;

            for (vector<House>::iterator it = houses.begin(); it != houses.end(); it++) {
                gas_emissions += it->CountAreaEmissionsWithPercentage(gas_emissions_constant, heating_percentage);
            }


        } else {
            for (vector<House>::iterator it = houses.begin(); it != houses.end(); it++) {
                gas_emissions += it->CountPeopleEmissions(gas_emissions_constant);
            }
            cout << "Yesterday: " << temperature_yesterday << " °C\t"
                 << " - Today: " << temperature << " °C\t"
                 << " - Heating Off.\t"
                 << " - Emissions: "<< gas_emissions << endl;
        }

        temperature_yesterday = temperature;
    }

    cout << "Overal gas emissions (t): " << gas_emissions / 1e6 << endl;

    return 0;
}
