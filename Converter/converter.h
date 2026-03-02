#ifndef CONVERTER_H
#define CONVERTER_H

class Converter
{
public:
    enum class Mode {
        Length,
        Mass,
        Temperature
    };

    enum class LengthUnit {
        Meter = 0,
        Kilometer,
        Inch,
        Foot,
        Mile
    };

    enum class MassUnit {
        Kilogram = 0,
        Pound,
        Ounce
    };

    enum class TemperatureUnit {
        Celsius = 0,
        Fahrenheit,
        Kelvin
    };

    double convert(Mode mode, int fromIndex, int toIndex, double value) const;

private:
    double convertLength(double value, LengthUnit from, LengthUnit to) const;
    double convertMass(double value, MassUnit from, MassUnit to) const;
    double convertTemperature(double value, TemperatureUnit from, TemperatureUnit to) const;
};

#endif // CONVERTER_H

