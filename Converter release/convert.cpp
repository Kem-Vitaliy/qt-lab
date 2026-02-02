#include "convert.h"

convert::convert()
{
    rates["meters (m)"]["kilometers (km)"] = 0.001;
    rates["meters (m)"]["inches (in)"] = 39.3701;
    rates["meters (m)"]["feet (ft)"] = 3.28084;
    rates["meters (m)"]["miles (mi)"] = 0.000621371;
    rates["meters (m)"]["meters (m)"] = 1;

    rates["kilometers (km)"]["meters (m)"] = 1000;
    rates["kilometers (km)"]["inches (in)"] = 39370.0787;
    rates["kilometers (km)"]["feet (ft)"] = 3280.84;
    rates["kilometers (km)"]["miles (mi)"] = 0.621371;
    rates["kilometers (km)"]["kilometers (km)"] = 1;

    rates["inches (in)"]["meters (m)"] =  0.0254;
    rates["inches (in)"]["kilometers (km)"] = 0.0000254;
    rates["inches (in)"]["feet (ft)"] = 0.0833;
    rates["inches (in)"]["miles (mi)"] = 0.0000157828;
    rates["inches (in)"]["inches (in)"] = 1;

    rates["feet (ft)"]["meters (m)"] = 0.3048;
    rates["feet (ft)"]["kilometers (km)"] = 0.0003048;
    rates["feet (ft)"]["inches (in)"] = 12;
    rates["feet (ft)"]["miles (mi)"] = 0.000189394;
    rates["feet (ft)"]["feet (ft)"] = 1;

    rates["miles (mi)"]["meters (m)"] = 1609.34;
    rates["miles (mi)"]["kilometers (km)"] = 1.609344;
    rates["miles (mi)"]["inches (in)"] = 63360;
    rates["miles (mi)"]["feet (ft)"] = 5280;
    rates["miles (mi)"]["miles (mi)"] = 1;

    rates["kilograms (kg)"]["pounds (lb)"] = 2.20462;
    rates["kilograms (kg)"]["ounces (oz)"] = 35.274;
    rates["kilograms (kg)"]["kilograms (kg)"] = 1;

    rates["pounds (lb)"]["kilograms (kg)"] = 0.453592;
    rates["pounds (lb)"]["ounces (oz)"] = 16;
    rates["pounds (lb)"]["pounds (lb)"] = 1;

    rates["ounces (oz)"]["pounds (lb)"] = 0.0625;
    rates["ounces (oz)"]["kilograms (kg)"] = 0.0283495;
    rates["ounces (oz)"]["ounces (oz)"] = 1;

}

double convert::conv(double amount, const QString &from, const QString &to)
{

    if (rates.contains(from) && rates[from].contains(to))
    {
        return amount * rates[from][to];
    }

    if (from == "Celsius (°C)" && to == "Fahrenheit (°F)")
        return (amount * 1.8) + 32;
    if (from == "Celsius (°C)" && to == "Kelvin (K)")
        return amount + 273.15;
    if (from == "Celsius (°C)" && to == "Celsius (°C)")
        return amount;

    if (from == "Fahrenheit (°F)" && to == "Celsius (°C)")
        return (amount - 32) / 1.8;
    if (from == "Fahrenheit (°F)" && to == "Kelvin (K)")
        return (amount - 32) / 1.8 + 273.15;
    if (from == "Fahrenheit (°F)" && to == "Fahrenheit (°F)")
        return amount;

    if (from == "Kelvin (K)" && to == "Celsius (°C)")
        return amount - 273.15;
    if (from == "Kelvin (K)" && to == "Fahrenheit (°F)")
        return (amount - 273.15) * 1.8 + 32;
    if (from == "Kelvin (K)" && to == "Kelvin (K)")
        return amount;


}
