//
//  timeConversionPriv.hpp
//  timeConversion
//
//  Created by Clifford Campo on 11/2/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

/* The classes below are not exported */
#ifndef __timeConversionBaseClass__
#define __timeConversionBaseClass__
#include <type_traits>
#include <iostream>
#include <string>
#include <regex>
#include <cmath> //Used for generating Not-A-Number (NaN)constant
#include <ctgmath>
#include <iomanip>
using namespace std;
// #pragma GCC visibility push(hidden)
//
template <class Tgj = const char *, class Tjg = double * >
class timeConversionBaseClass {
private:
    bool debug_mode;      // Indicate debug mode
    const int PASS = 0;
    const int INITIALIZE = 3;
    const int FAIL = 1;
    const int DAMNEDIFIKNOW = 2;
    const int LENGTHSOK = 1;
    const int MONTHSANDYEARSOK = 2;
    const int MONTHSANDDAYSOK31 = 3;
    const int MONTHSANDDAYSOK30 = 4;
    const int MONTHSANDDAYSOK29 = 5;
    const int MONTHSANDDAYSOK28 = 6;
    const int HOURSPERDAY =   24;
    const int MINUTESPERHOUR = 60;
    const int SECONDSPERMINUTE = 60;
    const int SECONDSPERHOUR = 3600;
    const int UNKNOWN = 7;
    const int ASTROTIME  =  1;
    const int CIVILTIME  =  0;
    const int TOGREGORIAN  =  1;      // Do a Julian to Gregorian Conversion
    const int TOJULIAN  =  2;      // Do a Gregorian to Julian Conversion
    const int NONE    =    0;      // No conversion
    const int MINUTESPERDAY =  HOURSPERDAY *  MINUTESPERHOUR;
    const int SECONDSPERDAY = HOURSPERDAY * SECONDSPERHOUR;
    const int YEAROUTOFRANGE =  1; /* Year is > 9999 */
    const int MONTHOUTOFRANGE = 2; /* Month is <1 or >12 */
    int A = 0;
    int IP = 0;
    int B = 0;
    int C = 0;
    int D = 0;
    int E = 0;
    int G = 0;
    int m = 0;
    int mn = 0; //months
    int rc=0;
    int day = 0;
    int y = 0;
    int hr = 0;
    int min = 0;
    int sec = 0;
    int intJD = 0;
    double  subsec = 0;
    Tjg jd = 0 ; //Tjg could be a double, or a double *
    double FP = 0;
    double d = 0;
    double dblD = 0;
    double dblSec = 0;
    double dblJD = 0;
    double myJD = 0;
    size_t myJDi = 0;
    const char* ISO8601_copy; //Copied from input parameter
    string ISO8601_madeIntoString;
    
public:
    string workingString;
    string datePart;
    string timePart;
    regex regSpace;
    regex regISOT;
    regex splitApartDate;
    regex splitApartTime;
    
    smatch matched;
    smatch matchSpace;
    smatch matchISOT;
    
    smatch matchDate;
    smatch matchTime;
    int numberOfSubStringsTime;
    int numberOfSubStringsDate;
    bool found = false;
    ~timeConversionBaseClass(void);
    timeConversionBaseClass(bool = false);  //bool = false means to set debug_mode to false.
    //    int julianToGregorian (double *, char *, bool=false); //bool = false means day starts at mid-night.
    //    template < class T > // T is either `const string` or `const char *`
 //   template <typename Tgj>
    double gregorianToJulian(Tgj, bool=false); //bool=false means that new day does NOT start at noon.
    //    double gregorianToJulian(const string ymd);
    
    //    template < class T > // T is either `const string` or `const char *`
//    template <typename Tjg>
    const char  *julianToGregorian(Tjg, bool=false); //bool=false means that new day does NOT start at noon.
    //    string julianToGregorian(double);
    //    int julianToGregorian (double *JD,  char *ISO8601, bool dayStartsAtNoon)
    void setDebugMode(bool);
};
// #pragma GCC visibility pop

//Constructor definition
template < class Tgj, class Tjg >
timeConversionBaseClass<Tgj, Tjg >::timeConversionBaseClass(bool debug)
{
    debug_mode = debug;
    regSpace.assign("(.*) (.*)");
    regISOT.assign("(.*)T(.*)");
    //    surprisingly, the next two instructions work!
    splitApartDate.assign("([[:digit:]]{4})-([[:digit:]]{2})-([[:digit:]]{2})");
    splitApartTime.assign("([[:d:]]{2}):([[:d:]]{2}):([[:d:]]{2})");
}

//Destructor definiton.
template < class Tgj, class Tjg >
timeConversionBaseClass<Tgj, Tjg >::~timeConversionBaseClass() {
//This destructor is here as a place-holder for when, and if, at some future time we decide to have the member fucntions \
    dynamically acquire memory. At that time we will add the appropriate delete code here so we do not incur any memory leaks.
    ;
}

/*
template < class Tgj, class Tjg >
void timeConversionBaseClass<Tgj, Tjg >::setDebugMode(bool dm) {
    debug_mode = dm; //Use this function if you want to reset the value set by the constructor, \
    named timeConversionBaseClass::timeConversionBaseClass
}
 */
// template < typename T > // T is either `const string` or `const char *`

template < class Tgj, class Tjg >  //Define the default types for input parameters.
double timeConversionBaseClass<Tgj, Tjg >::gregorianToJulian(Tgj inputOfSomeType, bool bbb) { //Declare function gregorianToJulian. ***Note*** that Julian \
    is spelled with a CAPITAL "J" !!! This function prototype takes just one input argument, a pointer to the structure \
    YearMonthDay, and returns a double.
    workingString = string(inputOfSomeType); //Taking the string of type string should cause a problem?¿
    found = regex_search(workingString, matched, regSpace);
    if (!found) {
        regISOT.assign("(.*)T(.*)");
        found = regex_search(workingString, matched, regISOT);
        if (!found) return ((double)NAN); //Exit by returning a null
    }
    found = false;
    if (debug_mode) {
        cout << "matched.length()=" << matched.length() << "\n";
        cout << "matched.size()=" << matched.size() << endl;
    }
    if (matched.size() != 3) {
        return ((double)-1); //Exit by returning a minus 1 because we did not get two substrings;
    }
    //So at this point our input string (or array of characters, known as a c-strig) should have been split apart into 2 pieces:\
    a date part and a time part.
    datePart = matched[1].str();
    timePart = matched[2].str();
    found = false;
    found = regex_search(datePart, matchDate, splitApartDate);
    if (debug_mode) {
        cout << "matcheDate.length()=" << matchDate.length() << "\n";
        cout << "matcheDate.size()=" << matchDate.size() << endl;
    }
    if ( (!found) && (matchDate.size() != 4) ) {
        return ((double)-2); //Return with -2 error code indicating that we failed to split the date apart into three pieces.
    }
    y = stoi(matchDate[1].str() );
    mn = stoi(matchDate[2].str() );
    dblD = stod(matchDate[3].str() );
    found = regex_search(timePart, matchTime, splitApartTime);
    if (debug_mode) {
        cout << "matcheTime.length()=" << matchTime.length() << "\n";
        cout << "matcheTime.size()=" << matchTime.size() << endl;
    }
    if (found && (matchTime.size() == 2) ) {
        hr = stoi(matchTime[1].str() );
        // The following were already set to zero by the constructor\
         min = 0;\
         dblSec = 0;\
 
    } else if (found && (matchTime.size() == 3) ) {
        hr = stoi(matchTime[1].str() );
        min = stoi(matchTime[2].str() );
        // The constructor already set the dblSec to zero\
         dblSec = 0;
 
    } else if (found && (matchTime.size() == 4) ) {
        hr = stoi(matchTime[1].str() );
        min = stoi(matchTime[2].str() );
        dblSec = stod(matchTime[3].str() );
    } else {
        return ((double)-3); //Return indicating strange results when parsing the time part.
    }
    //note: 1461 divided by 4 is 365.25
    //First, assume new-day-starts-at-mid-night. (i.e., bbb=false)
    dblJD = (1461*(y+4800+(mn-14)/12))/4+(367*(mn-2-12*((mn-14)/12)))/12-(3*((y+4900+(mn-14)/12)/100))/4 + dblD-32075 ;
    //    ymd->dblJD = (1461*(ymd->y+4800+(ymd->mn-14)/12))/4+(367*(ymd->mn-2-12*((ymd->mn-14)/12)))/12-(3*((ymd->y+4900+(ymd->mn-14)/12)/100))/4+ymd->dblD-32075 ;
    // ymd->intJD = (1461*(ymd->y+4800+(ymd->mn-14)/12))/4+(367*(ymd->mn-2-12*(ymd->mn-14)/12))/12-(3*((ymd->y+4900+(ymd->mn-14)/12)/100))/4+ymd->dblD-32075;
    if (bbb) {
        //IF new day starts at noon add a half-a-day to new-day-starts-at-mid-night.
        dblJD += 0.5;
    }
    intJD = (int)(dblJD);  //Recast the double as an integer and save this integer version of the Julian Day (sans hh:mm:ss) in a separate location
    this->intJD = (int)(this->dblJD);  //Recast the double as an integer and save this integer version of the Julian Day (sans hh:mm:ss) in a separate location
    return(this->dblJD + ((double)this->hr/HOURSPERDAY) + ((double)this->min/(HOURSPERDAY*MINUTESPERHOUR)) + this->dblSec/(HOURSPERDAY*SECONDSPERHOUR));
    
}

string zeroPadIfNecessary(int someInteger);
template < typename Tgj, typename Tjg >
const char  *timeConversionBaseClass<Tgj, Tjg >::julianToGregorian (Tjg JD,   bool dayStartsAtNoon) {
    bool bl = false;
    bl = std::is_pointer<Tjg>::value;
    if ( bl == true ) {
        cout << "Input parameter  `JD` is of type " << "pointer to double." << endl;;
// If Tjg is a pointer, is it a pointer to a floating point?
        jd = JD; // dereference the pointer to a floating pointer; Assume day starts at mid-night rather than noon.
    } else {
        cout << "Input parameter  `JD` is of type " << "double." << endl;;
        // If Tjg is a pointer, is it a pointer to a floating point?
        jd = JD; // dereference the pointer to a floating pointer; Assume day starts at mid-night rather than noon.
    }
    if (dayStartsAtNoon) { /* When variable dayStartsAtNoon=0 means day starts at midnight */
        jd =  jd + 0.5;  // This could be a problem. Civilians think the day starts at midnight,  \
        while astronomers define the new day as starting at noon, with noon defined as when the zenith distance \
        is at a minimum (there's some logic to that way of thinking).
    }
    IP=(int)jd; /* separate the integer part from the fractional part, integer part contains the YYYY-MM-DD information */
    FP=jd-(double)IP; /* separate out the fractional part, contains hh:mm:ss information */
    if(IP>2299160) { //The Gregorian Calendar equivalent of jd = 2299160 is 1582-10-04 00:00:00.
        A=(int)(((double)IP-1867216.25)/(36524.25)); //The Gregorian Calendar equivalent of jd = 1867216.25 is 0400-02-28 06:00:00.
        B=IP+1+A-(int)(A/4);
    } else {
        B=IP;
    }
    C=B+1524;
    D=(int)(((double)C-122.1)/365.25);
    E=(int)((double)(D)*365.25);
    G=(int)((  ((double)(C)-(double)(E))/30.6001   ));
    d=(double)(C-E+FP-(int)(30.6001*G));
    if ((double)G<13.5) {
        m=G-1;
    }else {
        m=G-13;
    }
    if((double)m>2.5) {
        y=D-4716;
    } else {
        y=D-4715;
    }
    mn = (int)m;
    day = (int)d;
    
    hr = (int)(HOURSPERDAY*FP); /* extract hours from the fractional part */
    FP = HOURSPERDAY*FP-(double)hr; /* remove hours from the fractional part so FP contains information on just minutes and seconds */
    min = (int)(MINUTESPERHOUR*FP); /* extract minutes from the fractional part */
    FP  = MINUTESPERHOUR*FP -(double)min; /* remove minutes from the fractional part so FP contains information on just seconds */
    sec = (int)(SECONDSPERMINUTE)*FP; // extract seconds from the fractional part so FP contains information on just subseconds, \
    which for now, we won't worry about.
    FP  = SECONDSPERMINUTE*FP-(double)sec; // remove seconds from the fractional part so FP contains information about subseconds.
    subsec = (int)1000000*FP; /* let subsec represent the microseconds that remain */
    if ( (subsec/1000000) >= 0.5 ) sec++;
    
    workingString = to_string(y) + "-" + zeroPadIfNecessary(mn) + "-" + zeroPadIfNecessary(day) + " " + zeroPadIfNecessary(hr) + ":" + zeroPadIfNecessary(min) + ":" + zeroPadIfNecessary(sec);
    
    return(workingString.c_str());
}
/*
string zeroPadIfNecessary(int someInteger) {
    if (someInteger < 10) {
        return "0" + to_string(someInteger);
    } else {
        return to_string(someInteger);
    }
}
 */
#endif


