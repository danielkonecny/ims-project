/**
 * @project			Carbon Footprint in Energetics and Heating Industry
 * @file			House.h
 * @version 		1.0
 * @course			IMS - Modelling and Simulation
 * @organisation	Brno University of Technology - Faculty of Information Technology
 * @author			Daniel Konecny (xkonec75), Filip Jerabek (xjerab24)
 * @date			2. 12. 2019
 */

#ifndef IMS_HOUSE_H
#define IMS_HOUSE_H

/**
 * Class representing a single house.
 */
class House {
public:
    /**
     * Count emissions from average values from website dodavatelelektriny.cz.
     * @param emissions_constant    Constant of emissions for given heating in g/Wh.
     * @return  Weight of CO2 produced in grams.
     */
    double CountEmissions(double emissions_constant);

    /**
     * Count emissions of the heating running on certain percentage of power.
     * @param emissions_constant    Constant of emissions for given heating in g/Wh.
     * @param heating_percentage    Percentage of power needed for heating.
     * @return  Weight of CO2 produced in grams.
     */
    double CountAreaEmissionsWithPercentage(double emissions_constant, double heating_percentage);

    /**
     * Count emissions of the hot water needed for given amount of people in the house.
     * @param emissions_constant    Constant of emissions for given heating in g/Wh.
     * @return  Weight of CO2 produced in grams.
     */
    double CountPeopleEmissions(double emissions_constant);

    /**
     * Count power needed to heat the house for a day.
     * @param heating_percentage    Percentage of power needed for heating.
     * @return  Power in watt hours.
     */
    double CountHouseHeatLossPerDay(double heating_percentage);

    /**
     * Count power needed for hot water for a day.
     * @return  Power in watt hours.
     */
    double CountPeopleHeatLossPerDay();

    int number_of_people;   /** Number of people living in the house. */
    int area;               /** Area of the whole house. */
    int distance;           /** Distance from the heating station. */
};

#endif //IMS_HOUSE_H
