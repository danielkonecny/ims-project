/**
 * @project			Carbon Footprint in Energetics and Heating Industry
 * @file			model.cpp
 * @version 		1.0
 * @course			IMS - Modelling and Simulation
 * @organisation	Brno University of Technology - Faculty of Information Technology
 * @author			Daniel Konecny (xkonec75), Filip Jerabek (xjerab24)
 * @date			2. 12. 2019
 */

#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#include "House.h"

using namespace std;

const int days_per_year = 365;
const double pi = 3.14159;
const double gas_emissions_constant = 0.2;
const double coal_emissions_constant = 0.36;
const double electricity_emissions_constant = 1.17;
const double nuclear_emissions_constant = 0.00427;

bool print_debug = false;

void generate_houses(int min_area, int max_area, int min_people, int max_people, int number_of_houses,
					 int min_distance, int max_distance, vector<House> *houses) {
	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<int> people(min_people, max_people);
	uniform_int_distribution<int> area(min_area, max_area);
	uniform_int_distribution<int> distance(min_distance, max_distance);

	for (int i = 0; i < number_of_houses; i++) {
		House h{};
		h.number_of_people = people(mt);
		h.area = area(mt);
		h.distance = distance(mt);
		houses->push_back(h);
	}
}

double get_temperature(int day) {
	double temperature;

	random_device rd;
	mt19937 mt(rd());
	normal_distribution<double> deviation(0, 2.5);

	temperature = 10 * sin(2 * pi * (day + 274) / days_per_year) + 7;
	temperature += deviation(mt);

	return temperature;
}

void check_temperature(double today, double yesterday, bool *heating_on) {
	if (today <= 13 && yesterday <= 13) {
		/** Turn on after two consecutive days with temperature under 13 degree Celsius. */
		*heating_on = true;
	} else if (today >= 13 && yesterday >= 13) {
		/** Turn off after two consecutive days with temperature over 13 degree Celsius. */
		*heating_on = false;
	}
}

double get_heating_percentage(double temperature, bool heating_on) {
	const int min_temperature = -15;
	const int max_temperature = 13;

	int range = max_temperature - min_temperature;

	double heating_percentage;
	if (heating_on) {
		if (temperature < min_temperature) {
			heating_percentage = 1;
		} else if (temperature > max_temperature) {
			heating_percentage = 0;
		} else {
			heating_percentage = (max_temperature - temperature) / range;
		}
	} else {
		heating_percentage = 0;
	}

	return heating_percentage * heating_percentage;
}

double
pipeline_output_temperature(double length, double input_temperature, double tube_diameter, double tube_isolation) {
	const double outside_temperature = -15;
	const double alfa_v = 14.65;
	const double isolation_thermal_conductivity = 0.5;
	const double water_flow = 13.1;
	const double specific_heat_capacity = 4479;
	double heat_loss, heat_loss_numerator, heat_loss_denominator;

	heat_loss_numerator = pi * length * (input_temperature - outside_temperature);
	heat_loss_denominator = 1 / (2 * isolation_thermal_conductivity) *
							log((tube_diameter + 2 * tube_isolation) / tube_diameter) +
							1 / (alfa_v * (tube_diameter + 2 * tube_isolation));
	heat_loss = heat_loss_numerator / heat_loss_denominator;

	return input_temperature - heat_loss / (water_flow * specific_heat_capacity);
}

double
station_house_transmission(House house, double heating_percentage, double *heating_liters, double *cooking_liters) {
	const double specific_heat_capacity = 4.18;
	const double temperature_from_station = 60;
	const double temperature_from_house = 40;
	const double temperature_from_water_treatment = 10;
	const double tube_diameter = 0.1;
	const double tube_isolation = 0.1;
	double heat_loss;
	double temperature_in_house, temperature_in_station;
	double house_heating_wh, house_heating_kj, house_cooking_wh, house_cooking_kj;
	double liters_per_heating, liters_per_cooking;
	double heating_loss_to_house_heating, heating_loss_to_house_cooking, heating_loss_to_station;

	temperature_in_house = pipeline_output_temperature(house.distance, temperature_from_station, tube_diameter,
													   tube_isolation);
	temperature_in_station = pipeline_output_temperature(house.distance, temperature_from_house, tube_diameter,
														 tube_isolation);

	house_heating_wh = house.CountHouseHeatLossPerDay(heating_percentage);
	house_heating_kj = house_heating_wh * 3.6;
	house_cooking_wh = house.CountPeopleHeatLossPerDay();
	house_cooking_kj = house_cooking_wh * 3.6;

	liters_per_heating = house_heating_kj / (specific_heat_capacity * (temperature_in_house - temperature_from_house));
	liters_per_cooking =
			house_cooking_kj / (specific_heat_capacity * (temperature_in_house - temperature_from_water_treatment));

	*heating_liters += liters_per_heating;
	*cooking_liters += liters_per_cooking;

	heating_loss_to_house_heating =
			specific_heat_capacity * liters_per_heating * (temperature_from_station - temperature_in_house) / 3.6;
	heating_loss_to_house_cooking =
			specific_heat_capacity * liters_per_cooking * (temperature_from_station - temperature_in_house) / 3.6;
	heating_loss_to_station =
			specific_heat_capacity * liters_per_heating * (temperature_from_house - temperature_in_station) / 3.6;

	heat_loss = house_heating_wh + house_cooking_wh + heating_loss_to_house_heating + heating_loss_to_house_cooking +
				heating_loss_to_station;

	if (print_debug) {
		cout << "CONSUMPTION: " << heat_loss << " Wh" << endl;
		cout << "- Heating: " << house_heating_wh << " Wh" << endl;
		cout << "- Cooking: " << house_cooking_wh << " Wh" << endl;
		cout << "- To House Heating: " << heating_loss_to_house_heating << " Wh" << endl;
		cout << "- To House Cooking: " << heating_loss_to_house_cooking << " Wh" << endl;
		cout << "- To Station Heating: " << heating_loss_to_station << " Wh" << endl;
	}

	return heat_loss;
}

double plant_station_transmission(double station_heating_loss_wh, double *liters) {
	const int distance_from_plant = 5;
	const double specific_heat_capacity = 4.18;
	const double temperature_from_plant = 120;
	const double temperature_from_station = 80;
	const double tube_diameter = 0.15;
	const double tube_isolation = 0.15;
	const double station_loss = 1.01;
	double heat_loss, station_heating_loss_kj;
	double temperature_in_station, temperature_in_plant;
	double heat_loss_to_station, heat_loss_to_plant;

	temperature_in_station = pipeline_output_temperature(distance_from_plant, temperature_from_plant, tube_diameter,
														 tube_isolation);
	temperature_in_plant = pipeline_output_temperature(distance_from_plant, temperature_from_station, tube_diameter,
													   tube_isolation);

	station_heating_loss_kj = station_heating_loss_wh * 3.6;

	*liters = station_heating_loss_kj / (specific_heat_capacity * (temperature_in_station - temperature_from_station));

	heat_loss_to_station = specific_heat_capacity * (*liters) * (temperature_from_plant - temperature_in_station);
	heat_loss_to_plant = specific_heat_capacity * (*liters) * (temperature_from_station - temperature_in_plant);

	heat_loss = station_heating_loss_wh + heat_loss_to_station + heat_loss_to_plant;
	heat_loss *= station_loss;

	return heat_loss;
}

void simulate_one_year(const vector<House> *houses, int *heating_days, double *year_temperature_count,
					   double *gas_emissions, double *coal_emissions, double *electricity_emissions,
					   double *plant_heat_loss, double *year_station_heat_loss,
					   double *year_liters_heating, double *year_liters_cooking, double *year_liters_station,
					   double *max_liters_station, double *max_liters_heating, double *max_liters_cooking) {
	bool heating_on = true;
	double temperature_yesterday = get_temperature(0);
	double month_temperature_count = 0;
	int month_count = 1;

	for (int day = 1; day <= days_per_year; day++) {
		double heating_liters = 0, cooking_liters = 0, station_liters = 0;
		double station_heat_loss = 0;
		double temperature = get_temperature(day);
		check_temperature(temperature, temperature_yesterday, &heating_on);
		double heating_percentage = get_heating_percentage(temperature, heating_on);

		/** Heating Emissions */
		for (auto house : *houses) {
			*gas_emissions += house.CountAreaEmissionsWithPercentage(gas_emissions_constant, heating_percentage);
			*coal_emissions += house.CountAreaEmissionsWithPercentage(coal_emissions_constant, heating_percentage);
			*electricity_emissions += house.CountAreaEmissionsWithPercentage(electricity_emissions_constant,
																			 heating_percentage);
			station_heat_loss += station_house_transmission(house, heating_percentage, &heating_liters,
															&cooking_liters);
		}

		/** Hot Water Emissions (Cooking) */
		for (auto house : *houses) {
			*gas_emissions += house.CountPeopleEmissions(gas_emissions_constant);
			*coal_emissions += house.CountPeopleEmissions(coal_emissions_constant);
			*electricity_emissions += house.CountPeopleEmissions(electricity_emissions_constant);
		}

		*plant_heat_loss += plant_station_transmission(station_heat_loss, &station_liters);

		/** Temperature Statistics */
		if (print_debug) {
			*year_temperature_count += temperature;
			month_temperature_count += temperature;
			if (day % 30 == 0) {
				cout << "^ Month Average - " << month_count << ": " << month_temperature_count / 30 << " °C\n" << endl;
				month_temperature_count = 0;
				month_count++;
			}
			if (heating_percentage > 0) {
				(*heating_days)++;
			}

			cout << "- Day Average: " << temperature << " °C\t"
				 << " - Heating: " << heating_percentage * 100 << " %.\t" << endl;

			if (heating_liters > *max_liters_heating) {
				*max_liters_heating = heating_liters;
			}
			if (cooking_liters > *max_liters_cooking) {
				*max_liters_cooking = cooking_liters;
			}
			if (station_liters > *max_liters_station) {
				*max_liters_station = station_liters;
			}

			cout << "Station Heat Loss: " << station_heat_loss << " Wh" << endl;
		}

		*year_station_heat_loss += station_heat_loss;
		*year_liters_heating += heating_liters;
		*year_liters_cooking += cooking_liters;
		*year_liters_station += station_liters;

		temperature_yesterday = temperature;
	}
}

int main(int argc, char *argv[]) {
	(void) argv;
	vector<House> houses;

	if (argc > 1) {
		print_debug = true;
	}

	generate_houses(30, 120, 1, 6, 5000, 200, 2000, &houses);

	const double water_pump_year_capacity = 567648000;
	double heating_pump_percentage, cooking_pump_percentage, plant_pump_percentage;
	const double year_pump_max_power = 391572000;
	double heating_pump_power, cooking_pump_power, plant_pump_power;

	const double wide_pipeline_length = 5;
	const double narrow_pipeline_length = 700;
	const double construction_emissions_1km_wide_pipeline = 75e6;
	const double construction_emissions_1km_narrow_pipeline = 50e6;
	const double construction_emissions_station = 120e6;
	const double construction_emissions_plant = 100e6;
	double nuclear_construction_emissions;
	double years_to_return_gas, years_to_return_coal, years_to_return_electricity;

	int heating_days = 0;
	double gas_emissions = 0, coal_emissions = 0, electricity_emissions = 0, nuclear_emissions = 0;
	double year_temperature_count = 0, year_station_heat_loss = 0, year_plant_heat_loss = 0;
	double year_liters_heating = 0, year_liters_cooking = 0, year_liters_station = 0;
	double max_liters_station = 0, max_liters_heating = 0, max_liters_cooking = 0;

	simulate_one_year(&houses, &heating_days, &year_temperature_count,
					  &gas_emissions, &coal_emissions, &electricity_emissions,
					  &year_plant_heat_loss, &year_station_heat_loss,
					  &year_liters_heating, &year_liters_cooking, &year_liters_station,
					  &max_liters_station, &max_liters_heating, &max_liters_cooking);

	heating_pump_percentage = year_liters_heating / (water_pump_year_capacity / 100);
	cooking_pump_percentage = year_liters_cooking / (water_pump_year_capacity / 100);
	plant_pump_percentage = year_liters_station / (water_pump_year_capacity / 100);

	heating_pump_power = year_pump_max_power * heating_pump_percentage;
	cooking_pump_power = year_pump_max_power * cooking_pump_percentage;
	plant_pump_power = year_pump_max_power * plant_pump_percentage;

	nuclear_emissions =
			nuclear_emissions_constant *
			(year_plant_heat_loss + heating_pump_power + cooking_pump_power + plant_pump_power);

	nuclear_construction_emissions =
			construction_emissions_1km_wide_pipeline * 2 * wide_pipeline_length +
			construction_emissions_1km_narrow_pipeline * narrow_pipeline_length +
			construction_emissions_station + construction_emissions_plant;

	years_to_return_gas = nuclear_construction_emissions / (gas_emissions - nuclear_emissions);
	years_to_return_coal = nuclear_construction_emissions / (coal_emissions - nuclear_emissions);
	years_to_return_electricity = nuclear_construction_emissions / (electricity_emissions - nuclear_emissions);

	if (print_debug) {
		cout << endl << "Year Average: " << year_temperature_count / days_per_year << " °C" << endl;
		cout << "Heating days: " << heating_days << endl;
		cout << "Year Station Heat Loss: " << year_station_heat_loss / 1e6 << " MWh" << endl;
		cout << "Year Plant Heat Loss: " << year_plant_heat_loss / 1e6 << " MWh" << endl;
		cout << "Max Liters Heating: " << max_liters_heating << " l" << endl;
		cout << "Max Liters Cooking: " << max_liters_cooking << " l" << endl;
		cout << "Max Liters Station: " << max_liters_station << " l" << endl;
		cout << "Year Liters Heating: " << year_liters_heating / 1e3 << " m^3" << endl;
		cout << "Year Liters Cooking: " << year_liters_cooking / 1e3 << " m^3" << endl;
		cout << "Year Liters Station: " << year_liters_station / 1e3 << " m^3" << endl;
		cout << "Heating Pump Percentage: " << heating_pump_percentage << " %" << endl;
		cout << "Cooking Pump Percentage: " << cooking_pump_percentage << " %" << endl;
		cout << "Station Pump Percentage: " << plant_pump_percentage << " %" << endl << endl;
	}

	cout << "Year Gas Emissions: " << gas_emissions / 1e6 << " t" << endl;
	cout << "Year Coal Emissions: " << coal_emissions / 1e6 << " t" << endl;
	cout << "Year Electricity Emissions: " << electricity_emissions / 1e6 << " t" << endl;
	cout << "Year Nuclear Emissions: " << nuclear_emissions / 1e6 << " t" << endl;
	cout << "Nuclear Construction Emissions: " << nuclear_construction_emissions / 1e6 << " t" << endl << endl;

	cout << "Will return in " << years_to_return_gas << " years to gas." << endl;
	cout << "Will return in " << years_to_return_coal << " years to coal." << endl;
	cout << "Will return in " << years_to_return_electricity << " years to electricity." << endl;

	/** Average Data from dodavatelelektriny.cz */
	if (print_debug) {
		gas_emissions = 0;
		coal_emissions = 0;
		electricity_emissions = 0;

		for (auto house : houses) {
			gas_emissions += house.CountEmissions(gas_emissions_constant);
			coal_emissions += house.CountEmissions(coal_emissions_constant);
			electricity_emissions += house.CountEmissions(electricity_emissions_constant);
		}

		cout << endl << "AVERAGE VALUES (dodavatelelektriny.cz)" << endl;
		cout << "Overall Gas Emissions (t): " << gas_emissions / 1e6 << endl;
		cout << "Overall Coal Emissions (t): " << coal_emissions / 1e6 << endl;
		cout << "Overall Electricity Emissions (t): " << electricity_emissions / 1e6 << endl;
	}

	return 0;
}
