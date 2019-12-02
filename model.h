/**
 * @project			Carbon Footprint in Energetics and Heating Industry
 * @file			model.h
 * @version 		1.0
 * @course			IMS - Modelling and Simulation
 * @organisation	Brno University of Technology - Faculty of Information Technology
 * @author			Daniel Konecny (xkonec75), Filip Jerabek (xjerab24)
 * @date			2. 12. 2019
 */

#ifndef IMS_MODEL_H
#define IMS_MODEL_H

#include <vector>

#include "House.h"

/**
 * Generate requested number of houses with requested parameters.
 */
void generate_houses(int min_area, int max_area, int min_people, int max_people, int number_of_houses,
                     int min_distance, int max_distance, std::vector<House> *houses);

/**
 * Compute average day temperature from sinus function according to values from Dukovany region.
 * @param day   Day of the year requested.
 * @return      Average temperature that day.
 */
double get_temperature(int day);

/**
 * Set heating on or off according to values from Ministry of the Environment of the Czech Republic.
 * @param today         Temperature today.
 * @param yesterday     Temperature yesterday.
 * @param heating_on    Heating to be set.
 */
void check_temperature(double today, double yesterday, bool *heating_on);

/**
 * What is the percentage of heating for given temperature.
 * @param temperature
 * @param heating_on
 * @return  Value between 0 and 1.
 */
double get_heating_percentage(double temperature, bool heating_on);

/**
 * Computation of output temperature of water from pipeline of given parameters.
 * @param length                Length of the pipeline.
 * @param input_temperature     Input temperature of the water.
 * @return  Output temperature of the water.
 */
double pipeline_output_temperature(double length, double input_temperature);

/**
 * Computation of the power needed in station for a specific day and house.
 * @param house
 * @param heating_percentage
 * @param heating_liters        Liters needed to heat the house.
 * @param cooking_liters        Volume of hot water needed.
 * @return  Power in watt hours.
 */
double
station_house_transmission(House house, double heating_percentage, double *heating_liters, double *cooking_liters);

/**
 * Computation of the power needed from plant for a specific day.
 * @param station_heating_loss_wh   Power needed by the station.
 * @param liters                    Volume of the water needed.
 * @return  Power in watt hours.
 */
double plant_station_transmission(double station_heating_loss_wh, double *liters);

/**
 * Simulation of one year with all the needed computation.
 */
void simulate_one_year(const std::vector<House> *houses, int *heating_days, double *year_temperature_count,
                       double *gas_emissions, double *coal_emissions, double *electricity_emissions,
                       double *plant_heat_loss, double *year_station_heat_loss,
                       double *year_liters_heating, double *year_liters_cooking, double *year_liters_station,
                       double *max_liters_station, double *max_liters_heating, double *max_liters_cooking);

#endif //IMS_MODEL_H
