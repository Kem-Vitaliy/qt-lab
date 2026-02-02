#ifndef CONVERT_H
#define CONVERT_H
#include <QString>
#include <QMap>

class convert
{

public:
    convert();
    double conv(double amount, const QString &from, const QString &to);

private:
    QMap<QString, QMap<QString, double>> rates;

};

#endif // CONVERT_H
