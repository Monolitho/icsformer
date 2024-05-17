#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
struct event {};
struct Course {
  tm Start;
  tm End;
  string Name;
  string Place;
  // int SemesterStartDate;//for output only.
  int StartWeek;     // The start week of the course
  int EndWeek;       // The end week of the course
  bool isBiweekly;   // Whether the course is held biweekly
  char WeekdayCode;  // digit 1~7 is 1 -> that day have course;
};
std::string formatTimeUTC(const std::tm& time) {
  std::ostringstream oss;
  oss << std::put_time(&time, "%Y%m%dT%H%M%SZ");
  return oss.str();
}
string weekdayRule(char weekdayMap) {
  string weekdaySymbol[7] = {"MO,", "TU,", "WE,", "TH,", "FR,", "SA,", "SU,"};
  string res = "";
  if (!weekdayMap)
    NULL;
  else {
    for (int i = 0; i < 8; i++) {
      if (weekdayMap & 1 << i) {
        res += weekdaySymbol[i];
      }
    }
    res = res.substr(0, res.length() - 1);
  }
  return res;
}
tm addDaysToTm(std::tm date, int days) {
  // Convert tm to time_t
  std::time_t time = std::mktime(&date);

  // Add the desired number of days, converted to seconds
  time += days * 24 * 60 * 60;

  // Convert time_t back to tm
  tm date1 = *std::localtime(&time);
  return date1;
}
std::ostream& operator<<(std::ostream& os, const Course& myevent) {
  os << "BEGIN:VEVENT" << "\n"
     << "SUMMARY:" << myevent.Name << "\n"
     << "LOCATION:" << myevent.Place << "\n"
     << "DTSTART:" << formatTimeUTC(myevent.Start) << "\n"
     << "DTEND:" << formatTimeUTC(myevent.End) << "\n"
     << "RRULE: " << "FREQ=WEEKLY;" << (myevent.isBiweekly ? "INTERVAL=2;" : "")
     << "BYDAY=" << weekdayRule(myevent.WeekdayCode) << ";UNTIL="
     << formatTimeUTC(
            addDaysToTm(myevent.End, 7 * (myevent.EndWeek - myevent.StartWeek)))
     << "END:VEVENT" << "\n";
  return os;
}
vector<Course> ReadRecord(ifstream& fin, int startdate, char& weeklyMap) {
  Course weekly, Evenweekly, Oddweekly;
  string coursename, courseplace;
  fin >> coursename >> courseplace;
  string weekDuration;
  getline(fin, weekDuration, '-');
  cout << weekDuration << endl;
  int startweek = atoi(weekDuration.c_str());
  int endweek;
  fin >> endweek;
  string DaySetting;
  fin >> DaySetting;
  while (DaySetting.find(',') != -1) {
    DaySetting.replace(DaySetting.find(','), 1, " ");
  }  // Discrete the DayTimes;
  char WeeklyDayMap = 0, EvenDayMap = 0, OddDayMap = 0;
  stringstream strin(DaySetting);
  string currentWeekday = "";
  while (strin >> currentWeekday) {
    int weekday = 0;
    if (currentWeekday.find("MO") != -1) {
      weekday = 1;
    }
    if (currentWeekday.find("TU") != -1) {
      weekday = 2;
    }
    if (currentWeekday.find("WE") != -1) {
      weekday = 3;
    }
    if (currentWeekday.find("TH") != -1) {
      weekday = 4;
    }
    if (currentWeekday.find("FR") != -1) {
      weekday = 5;
    }
    if (currentWeekday.find("SA") != -1) {
      weekday = 6;
    }
    if (currentWeekday.find("SU") != -1) {
      weekday = 7;
    }
    if (currentWeekday.find('^') != -1) {
      EvenDayMap |= 1 << (weekday - 1);
    } else if (currentWeekday.find('*') != -1) {
      OddDayMap |= 1 << (weekday - 1);
    } else
      WeeklyDayMap |= 1 << (weekday - 1);
  }
  int startclass = 0, endclass = 0;
  string coursehour = "";
  cin >> coursehour;
  tm courseStartTime = {};
  tm courseEndTime = {};
  if (coursehour == "11-12*") {
    // alternative 11 - 12 course(18 : 20 instead of 18 : 00)
    courseStartTime.tm_hour = 18;
    courseStartTime.tm_min = 20;
    courseEndTime.tm_hour = 20;
    courseEndTime.tm_min = 0;
  } else {
    sscanf(coursehour.c_str(), "%d-%d", &startclass, &endclass);
    switch (startclass) {
      case 1:
        courseStartTime.tm_hour = 8;
        courseStartTime.tm_min = 0;
        break;
      case 2:
        courseStartTime.tm_hour = 8;
        courseStartTime.tm_min = 55;
        break;
      case 3:
        courseStartTime.tm_hour = 10;
        courseStartTime.tm_min = 0;
        break;
      case 4:
        courseStartTime.tm_hour = 10;
        courseStartTime.tm_min = 55;
        break;
      case 5:
        courseStartTime.tm_hour = 12;
        courseStartTime.tm_min = 0;
        break;
      case 6:
        courseStartTime.tm_hour = 12;
        courseStartTime.tm_min = 55;
        break;
      case 7:
        courseStartTime.tm_hour = 14;
        courseStartTime.tm_min = 00;
        break;
      case 8:
        courseStartTime.tm_hour = 14;
        courseStartTime.tm_min = 55;
        break;
      case 9:
        courseStartTime.tm_hour = 16;
        courseStartTime.tm_min = 0;
        break;
      case 10:
        courseStartTime.tm_hour = 16;
        courseStartTime.tm_min = 55;
        break;
      case 11:
        courseStartTime.tm_hour = 18;
        courseStartTime.tm_min = 0;
        break;
      case 12:
        courseStartTime.tm_hour = 18;
        courseStartTime.tm_min = 55;
        break;
      case 13:
        courseStartTime.tm_hour = 20;
        courseStartTime.tm_min = 0;
        break;
      case 14:
        courseStartTime.tm_hour = 20;
        courseStartTime.tm_min = 55;
        break;
      default:
        cout << "Input Error." << endl;
        exit(1);
    }
    switch (endclass) {
      case 1:
        courseStartTime.tm_hour = 8;
        courseStartTime.tm_min = 45;
        break;
      case 2:
        courseStartTime.tm_hour = 9;
        courseStartTime.tm_min = 40;
        break;
      case 3:
        courseStartTime.tm_hour = 10;
        courseStartTime.tm_min = 45;
        break;
      case 4:
        courseStartTime.tm_hour = 11;
        courseStartTime.tm_min = 40;
        break;
      case 5:
        courseStartTime.tm_hour = 12;
        courseStartTime.tm_min = 45;
        break;
      case 6:
        courseStartTime.tm_hour = 13;
        courseStartTime.tm_min = 40;
        break;
      case 7:
        courseStartTime.tm_hour = 14;
        courseStartTime.tm_min = 45;
        break;
      case 8:
        courseStartTime.tm_hour = 15;
        courseStartTime.tm_min = 40;
        break;
      case 9:
        courseStartTime.tm_hour = 16;
        courseStartTime.tm_min = 45;
        break;
      case 10:
        courseStartTime.tm_hour = 17;
        courseStartTime.tm_min = 40;
        break;
      case 11:
        courseStartTime.tm_hour = 18;
        courseStartTime.tm_min = 45;
        break;
      case 12:
        courseStartTime.tm_hour = 19;
        courseStartTime.tm_min = 40;
        break;
      case 13:
        courseStartTime.tm_hour = 20;
        courseStartTime.tm_min = 45;
        break;
      case 14:
        courseStartTime.tm_hour = 21;
        courseStartTime.tm_min = 40;
        break;
      default:
        cout << "Input Error." << endl;
        exit(1);
    }
  }
  if (WeeklyDayMap) {
    weeklyMap |= 1;
  }
  if (OddDayMap) {
    weeklyMap |= 2;
  }
  if (EvenDayMap) {
    weeklyMap |= 4;
  }

  vector<Course> ny;
  return ny;
}
int main() {
  int startdate;
  ifstream fin("version.icsconfig");
  fin >> startdate;
  char weeklyCode = 0;
  ReadRecord(fin, startdate, weeklyCode);
}
