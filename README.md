## This is the readme file for project icsFormer, written by Danny Wenjue Zhang on May 17,2024.
### Overview:
icsFormer is a formatter that explains a plain-text course description and translate it into .ics form events, a form that all electronic calendar can recognize. After import the ics file, the course would be represented by a sequence of events on the calendar.
### Compile
Open a terminal in the repo folder and use the line
```
g++ ./icsformer.cpp -o icsformer -std=c++11
```
to compile in Linux.
For Windows users, use
```
g++ ./icsformer.cpp -o icsformer.exe -std=c++11
```
### Usage
To use the project, simply create a Calendar.icsconfig file (**notice the capitalized letter!**) in the same folder as the executeable file and write in the course.
The files start with a date of the beginning day of the semester (**Must be Monday!**) in *YYYYMMDD* format. It is followed by the course 
Each of the course should be written in a line, with the following fields provided.
#### Name of the course
#### Place of the course
#### Duration week
Start from 1, following the format xx-xx.  
#### Weekday of the course:
Follows a traditional ics file format. Please write weekdays in **Capitalized** two letter abbreviations, like MO for Mondays, TU for Tuesdays and so on. If you have multiple days, please use comma to seperate them, **DO NOT** use spaces. You can add ^ or * after a weekday to represent they are biweekly and are on even weeks and odd weeks respectively.
For example, If the course is on Mondays and Thursdays, and is held on Fridays on odd weeks, then you should write MO,TH,FR*.  
#### Exact index of the time for the course.
 They are written in xx-xx forms and follows a SJTU course timetable (start from 08:00 every day, each 2-hour period contains 45min class, 10 min break then 45min class followed by 20min course intervals. The first class starts at 8:00 and the second 8:55(45min+10min)).
 There's another twist for the course that was held on 18:00-19:40 (class 11-12). Some teachers prefer to hold it 20mins later to give the students enough time to have supper. In the case, the course is held at 18:20 to 20:00. To represent this situation, add a astroid to the common 11-12 index.  
 Note that the project **DO NOT** provide support for a course with different class time or duration week. In this situation, you have to seperate it into 2 classes notes and use two lines to describe it. The author decided not to support these situations because they will mess up the record and make it hard to write. Besides, you can always use the copy-paste-alter tactics.  
 After you have written the Calendar.ics file, import it to your calendar app. You can find a import event option inside most vcalendar apps.
 ### Example
 This is an example line of the record.
 ```
 German Online 7-12 TU,TH,FR* 11-12*
 ```
 This example shows that a course named German, will be held online, is to be held on week 7 to week 12 at 18:20-20:00(altered 18:00 class), on Tuesdays, Thursdays and odd Fridays. 
 You can find a useable complete Calendar.icsconfig file in this repo.
 ### Feedbacks
 The author's email is DannyWenjueZhang@gmail.com. Feel free to report any issues, advice or bug feedbacks to it. :\)

