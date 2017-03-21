#include "XTimeUtil.h"
#include <time.h>
using namespace std;

namespace extension
{

double XTimeUtil::getSystemTimeForSecond()
{
	struct tm *tm;
	time_t timep;
	time(&timep);
	tm = localtime(&timep);
	int year = tm->tm_year/* + 1900*/;
	int month = tm->tm_mon + 1;
	int day = tm->tm_mday;
	int hour = tm->tm_hour;
	int minute = tm->tm_min;
	int second = tm->tm_sec;
	double num1 = 365 * 24 * 60 * 60;
	double yearToSecond = year * num1;
	double monthToSecond = month * 30 * 24 * 60 * 60;
	double dayToSecond = day * 24 * 60 * 60;
	double hourToSecong = hour * 60 * 60;
	double minuteToSecond = minute * 60;
	double result = yearToSecond + monthToSecond + dayToSecond + hourToSecong + minuteToSecond + second;
	return result;
};

}