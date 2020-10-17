#include <stdio.h>
#include <cstring>
#include <string>
#include <time.h>
#include <ctime>

using namespace std;

/*Accounting for leap years, give us the index of where the calendar day starts given the date*/
int dayIndex(int day, int month, int year){

	static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};

	year -= month < 3;
	return (year + year/4 - year/100 + year/400 + t[month] + day + 1) % 7;
}

/*Convert a month number to a month string, 0-11*/
string getMonthLabel(int month){
	string months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

	return (months[month]);
}

/**/
int getDaysInMonth (int month, int year){
	switch(month){
		case 0:
			return 30;
			break;
		case 1:
			if ( year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
				return 29;
			else
				return 28;
			break;
		case 2:
			return 31;
			break;
		case 3:
			return 30;
			break;
		case 4:
			return 31;
			break;
		case 5:
			return 30;
			break;
		case 6:
			return 31;
			break;
		case 7:
			return 31;
			break;
		case 8:
			return 30;
			break;
		case 9:
			return 31;
			break;
		case 10:
			return 30;
			break;
		case 11:
			return 31;
			break;
		default:
			printf("Error in getting month label:\nWas given: %d\n", month);
			return -1;
			break;

	}
}

/*Even spacing for month printing*/
void printLabel(int month, int year){
	
	string dayLabel = " Sun Mon Tue Wed Thu Fri Sat\n";

	static int width = strlen(dayLabel.c_str());

	int len = strlen(getMonthLabel(month).c_str()) + 5; 

	//get the margine of one side
	int margin = (width - len) / 2;  
	
	printf("\n");
	for (int i = 0; i < margin; i++)
		printf(" ");

	printf("%s %d\n%s\n", getMonthLabel(month).c_str(), year, dayLabel.c_str());

	return;
}

/*Print the calendar for this month only*/
void printCalendar(int day, int month, int year){
	
	int index = dayIndex( day, month, year);
	int days = getDaysInMonth(month, year);

	//print labels
	printLabel(month, year);

	//print spaces
	int i; 
	for(i = 0; i < index; i++)
		printf("    ");

	//add days, but keep track of index
	int j; 
	for(j = 1; j <= days; j++){
		printf("%4d", j);

		//increments i for us and checks it
		if(++i > 6)
		{
			i = 0;
			printf("\n");
		}
	}
	
	if(i)
		printf("\n");
	index = i;

	return;
}

/*Process with no arguments*/
void processToday(){
	
	time_t snapshot;
	time(&snapshot); //pull snapshot data into snapshot
	auto date = localtime(&snapshot);
	
	int day = date -> tm_mday;
	int month = date -> tm_mon;
	int year = 1900 + (date -> tm_year);
	
	//print yyyy-mm first
	if(month+1 < 10)
		printf("Today's Date: %d-0%d\n\n", year, month+1);
	else
		printf("Today's Date: %d-%d\n\n", year, month+1);

	printCalendar(day, month, year);
}

/*Process with month and year arguments*/
void processAnotherDay(int month, int year){
	int day = 1;
	if(month+1 < 10)
		printf("Requested Date: %d-0%d\n\n", year, month+1);
	else
		printf("Requested Date: %d-%d\n\n", year, month+1);

	printCalendar(day, month, year);
}

/*Validate arguments*/
int validate(int month, int year){
	if(month < 0 || month > 11 || year < 1901)
		return -1;
	else
		return 0;
}


/*Process user arguments and run program*/
int main(int argc, char* argv[]) {
	if(argc == 3){
		int month = atoi(argv[1]) - 1;
		int year = atoi(argv[2]);
		if(validate(month, year) == 0){
			processAnotherDay(month, year);
			return 0;
		}
		else{
			printf("Failed validating arguments. Please enter valid MM YYYY");	
			return -1;
		}
	}
	else if (argc == 1){
		processToday();
		return 0;
	}
	else {
		printf("Error processing arguments. Please use: \n program [month] [year]\nor: program\n");
		return -1;
	}
}
