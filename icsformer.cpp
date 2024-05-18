#include <algorithm>
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
  oss << std::put_time(&time, "%Y%m%dT%H%M%S");
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
     << "DTSTART;TZID=Asia/Shanghai:" << formatTimeUTC(myevent.Start) << "\n"
     << "DTEND;TZID=Asia/Shanghai:" << formatTimeUTC(myevent.End) << "\n"
     << "RRULE:" << "FREQ=WEEKLY;" << (myevent.isBiweekly ? "INTERVAL=2;" : "")
     << "BYDAY=" << weekdayRule(myevent.WeekdayCode) << ";UNTIL="
     << formatTimeUTC(
            addDaysToTm(myevent.End, 7 * (myevent.EndWeek - myevent.StartWeek)))
     << "\n"
     << "END:VEVENT" << "\n";
  return os;
}
vector<Course> ReadRecord(stringstream& fin, int startdate, char& weeklyMap) {
  Course weekly, Evenweekly, Oddweekly;
  string coursename, courseplace;
  fin >> coursename >> courseplace;
  string weekDuration;
  getline(fin, weekDuration, '-');
  // cout << weekDuration << endl;
  int startweek = atoi(weekDuration.c_str());
  int endweek;
  fin >> endweek;
  startweek--;
  endweek--;  // to turn 1-start weeks into 0-start swifting.
  // cout << ">>>DEBUG: Startweek=" << startweek << " EndWeek=" << endweek <<
  // endl;
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
    vector<string> weekDayNames = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
    weekday = std::find(weekDayNames.begin(), weekDayNames.end(),
                        currentWeekday.substr(0, 2)) -
              weekDayNames.begin();
    /*if (currentWeekday.find("MO") != -1) {
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
    }*/
    if (weekday == 7) {
      cerr << "Invalid input. Program Terminated." << endl;
      exit(1);
    }
    weekday++;
    if (currentWeekday.find('^') != -1) {
      EvenDayMap |= 1 << (weekday - 1);
    } else if (currentWeekday.find('*') != -1) {
      OddDayMap |= 1 << (weekday - 1);
    } else
      WeeklyDayMap |= 1 << (weekday - 1);
  }
  int startclass = 0, endclass = 0;
  string coursehour = "";
  fin >> coursehour;
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
    int startTimeHourArray[15] = {0,  8,  8,  10, 10, 12, 12, 14,
                                  14, 16, 16, 18, 18, 20, 20};
    int startTimeMinuteArray[15] = {0, 0,  55, 0,  55, 0,  55,
                                    0, 55, 0,  55, 0,  55, 0};
    int endTimeHourArray[15] = {0,  8,  9,  10, 11, 12, 13, 14,
                                15, 16, 17, 18, 19, 20, 21};
    int endTimeMinuteArray[15] = {0,  45, 40, 45, 40, 45, 40, 45,
                                  40, 45, 40, 45, 40, 45, 40};
    courseStartTime.tm_hour = startTimeHourArray[startclass];
    courseStartTime.tm_min = startTimeMinuteArray[startclass];
    courseEndTime.tm_hour = endTimeHourArray[endclass];
    courseEndTime.tm_min = endTimeMinuteArray[endclass];
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
  courseStartTime.tm_year = courseEndTime.tm_year = (startdate / 10000) - 1900;
  courseEndTime.tm_year = courseEndTime.tm_year = (startdate / 10000) - 1900;
  courseStartTime.tm_mon = ((startdate / 100) % 100 - 1);
  courseEndTime.tm_mon = ((startdate / 100) % 100 - 1);
  courseStartTime.tm_mday = startdate % 100;
  courseEndTime.tm_mday = startdate % 100;
  tm multiStartTime[3] = {courseStartTime, courseStartTime, courseStartTime};
  tm multiEndTime[3] = {courseEndTime, courseEndTime, courseEndTime};
  for (int i = 0; i < 8; i++) {
    if (WeeklyDayMap & 1 << i) {
      multiStartTime[0] = addDaysToTm(courseStartTime, startweek * 7 + i);
      multiEndTime[0] = addDaysToTm(courseEndTime, startweek * 7 + i);
      // It stops when it finds the first day that have classes.
    }
  }
  for (int i = 0; i < 8; i++) {
    if (EvenDayMap & 1 << i) {
      if (startweek % 2 == 0) startweek++;
      multiStartTime[1] = addDaysToTm(courseStartTime, startweek * 7 + i);
      multiEndTime[1] = addDaysToTm(courseEndTime, startweek * 7 + i);
      if (startweek % 2 == 1) startweek--;
      break;  // It stops when it finds the first day that have classes.
    }
  }
  for (int i = 0; i < 8; i++) {
    if (OddDayMap & 1 << i) {
      if (startweek % 2 == 1) startweek++;
      multiStartTime[2] = addDaysToTm(courseStartTime, startweek * 7 + i);
      multiEndTime[2] = addDaysToTm(courseEndTime, startweek * 7 + i);
      if (startweek % 2 == 0) startweek--;
      break;
    }
  }
  vector<Course> ny;
  Course curCourse;
  if (WeeklyDayMap) {
    curCourse.Start = multiStartTime[0];
    curCourse.EndWeek = endweek;
    curCourse.End = multiEndTime[0];
    curCourse.StartWeek = startweek;
    curCourse.isBiweekly = false;
    curCourse.Name = coursename;
    curCourse.Place = courseplace;
    curCourse.WeekdayCode = WeeklyDayMap;
    // cout << ">>>DEBUG: WeeklyDayMap=" << (int)WeeklyDayMap << endl;
    ny.push_back(curCourse);
    weeklyMap |= 1;
  }
  if (EvenDayMap) {
    curCourse.Start = multiStartTime[1];
    curCourse.EndWeek = endweek;
    curCourse.End = multiEndTime[1];
    curCourse.StartWeek = startweek;
    curCourse.isBiweekly = true;
    curCourse.Name = coursename;
    curCourse.Place = courseplace;
    curCourse.WeekdayCode = EvenDayMap;
    ny.push_back(curCourse);
    weeklyMap |= 2;
  }
  if (OddDayMap) {
    curCourse.Start = multiStartTime[2];
    curCourse.EndWeek = endweek;
    curCourse.End = multiEndTime[2];
    curCourse.StartWeek = startweek;
    curCourse.isBiweekly = true;
    curCourse.Name = coursename;
    curCourse.Place = courseplace;
    curCourse.WeekdayCode = OddDayMap;
    ny.push_back(curCourse);
    weeklyMap |= 4;
  }
  return ny;
}
int main() {
  int startdate;
  ifstream fin("Calendar.icsconfig");
  ofstream fout("Calendar.ics");
  fout << "BEGIN:VCALENDAR" << endl;
  fout << "BEGIN:VTIMEZONE\n\
TZID:Asia/Shanghai\n\
BEGIN:STANDARD\n\
DTSTART:19700101T000000\n\
TZOFFSETFROM:+0800\n\
TZOFFSETTO:+0800\n\
TZNAME:CST\n\
END:STANDARD\n\
END:VTIMEZONE\n\
";

  fin >> startdate;
  fin.ignore();
  char weeklyCode = 0;
  string inputString;
  while (1) {
    if (!getline(fin, inputString)) break;
    // cout << ">>>DEBUG: inputstring= " << inputString << endl;
    stringstream strin(inputString);
    vector<Course> courses = ReadRecord(strin, startdate, weeklyCode);
    for (auto i : courses) {
      fout << i;
    }
  }
  fout << "END:VCALENDAR";
  fin.close();
  fout.close();
}
