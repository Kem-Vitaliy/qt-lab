#include "converter.h"

double Converter::convert(Mode mode, int fromIndex, int toIndex, double value) const
{
    if (fromIndex == toIndex) {
        return value;
    }

    switch (mode) {
    case Mode::Length:
        return convertLength(value,
                             static_cast<LengthUnit>(fromIndex),
                             static_cast<LengthUnit>(toIndex));
    case Mode::Mass:
        return convertMass(value,
                           static_cast<MassUnit>(fromIndex),
                           static_cast<MassUnit>(toIndex));
    case Mode::Temperature:
        return convertTemperature(value,
                                  static_cast<TemperatureUnit>(fromIndex),
                                  static_cast<TemperatureUnit>(toIndex));
    }

    return value;
}

double Converter::convertLength(double value, LengthUnit from, LengthUnit to) const
{
    // convert to base unit: meter
    double inMeters = value;
    switch (from) {
    case LengthUnit::Meter:      inMeters = value; break;
    case LengthUnit::Kilometer:  inMeters = value * 1000.0; break;
    case LengthUnit::Inch:       inMeters = value * 0.0254; break;
    case LengthUnit::Foot:       inMeters = value * 0.3048; break;
    case LengthUnit::Mile:       inMeters = value * 1609.344; break;
    }

    // from meters to target unit
    switch (to) {
    case LengthUnit::Meter:      return inMeters;
    case LengthUnit::Kilometer:  return inMeters / 1000.0;
    case LengthUnit::Inch:       return inMeters / 0.0254;
    case LengthUnit::Foot:       return inMeters / 0.3048;
    case LengthUnit::Mile:       return inMeters / 1609.344;
    }

    return value;
}

double Converter::convertMass(double value, MassUnit from, MassUnit to) const
{
    // convert to base unit: kilogram
    double inKilograms = value;
    switch (from) {
    case MassUnit::Kilogram: inKilograms = value; break;
    case MassUnit::Pound:    inKilograms = value * 0.45359237; break;
    case MassUnit::Ounce:    inKilograms = value * 0.028349523125; break;
    }

    switch (to) {
    case MassUnit::Kilogram: return inKilograms;
    case MassUnit::Pound:    return inKilograms / 0.45359237;
    case MassUnit::Ounce:    return inKilograms / 0.028349523125;
    }

    return value;
}

double Converter::convertTemperature(double value, TemperatureUnit from, TemperatureUnit to) const
{
    // convert to base unit: Kelvin
    double inKelvin = value;
    switch (from) {
    case TemperatureUnit::Celsius:
        inKelvin = value + 273.15;
        break;
    case TemperatureUnit::Fahrenheit:
        inKelvin = (value - 32.0) * 5.0 / 9.0 + 273.15;
        break;
    case TemperatureUnit::Kelvin:
        inKelvin = value;
        break;
    }

    switch (to) {
    case TemperatureUnit::Celsius:
        return inKelvin - 273.15;
    case TemperatureUnit::Fahrenheit:
        return (inKelvin - 273.15) * 9.0 / 5.0 + 32.0;
    case TemperatureUnit::Kelvin:
        return inKelvin;
    }

    return value;
}

