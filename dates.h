//
// Created by Andrei Gheorghiu 5/24/2022.
//

#ifndef DATABASE_UNIVERSITA2022_DATES_H
#define DATABASE_UNIVERSITA2022_DATES_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <sstream>
#include <regex>
#include <algorithm>
#include <map>
#include <iterator>
#include <sstream>
#include <chrono>
#include "utility.h"
#include "mmath.h"
//January 1 1970 = day 1.
using std::vector;
using std::string;
using std::to_string;
using std::regex;

namespace dates{
    bool IsDateValid(const string& date){
        regex regs("^([0-9]{4})\\-([0-9]{2})\\-([0-9]{2})$");
        //Allowed format: YYYY-MM-DD
        //Epoch starts at 1 January 2000, assuming constant 365 day years, allowed years will be between 2000 and 9999. Months from 01 to 12. Days from 01 to 30.

        if(!regex_match(date,regs)){
            //cout<<"\nDATE FORMAT ERROR: Dates should follow the format YYYY-MM-DD e.g. 2021-05-23\n";
            return false;
        }else{
            string year = date.substr(0,4);
            string month = date.substr(5,2);
            string day = date.substr(8,2);

            if(!((stoi(year)<3000)&&(stoi(year)>1999))){
                return false;
            }else{
                if(!((stoi(month)<13)&&(stoi(month)>0))){
                    return false;
                }else{
                    if(!((stoi(day)<32)&&(stoi(day)>0))){
                        return false;
                    }else{
                        return true;
                    }
                }
            }

        }

    }

    int DateToEpoch(const string& date){
        int epoch = 0;
        if(IsDateValid(date)){
            const int DaysMonths[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
            int year = stoi(date.substr(0,4));
            int month = stoi(date.substr(5,2));
            int day = stoi(date.substr(8,2));

            //year contribution
            epoch += 365*(year-2000);

            //month contribution
            for(int i = 1;i<month;i++){
                epoch += DaysMonths[i];
            }

            //day of month contribution
            epoch += day;

            return epoch;


        }else{return 0;}
    }

    string EpochToDate(const int& epoch){
        const int DaysMonths[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        double epoch_divided = (double)((epoch-1)/365);
        int day_of_year;
        string year = to_string((int)(2000+mmath::floor(epoch_divided)));
        string month;
        string day;

        day_of_year = epoch-(int)(mmath::floor(epoch_divided))*365;


        string zero = "0";
        month = "";
        day = "";
        if((0<day_of_year)&&(day_of_year<(DaysMonths[1]+1))){
            //it's january
            month = "01";
            if(to_string(day_of_year).size()==1){
                day = zero + to_string(day_of_year);
            }else{day = to_string(day_of_year);}

        }else if((DaysMonths[1]<day_of_year)&&(day_of_year<(DaysMonths[2]+DaysMonths[1]+1))){
            //february
            month = "02";
            if(to_string(day_of_year-DaysMonths[1]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]);
            }else{day = to_string(day_of_year-DaysMonths[1]);}

        }else if((DaysMonths[1]+DaysMonths[2]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+1))){
            //march
            month = "03";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]);}

        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+1))){
            //april
            month = "04";

            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]);}


        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+1))){
            //may
            month = "05";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]);}


        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+1))){
            //june
            month = "06";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]);}

        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+1))){
            //july
            month = "07";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]);}

        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+1))){
            //august
            month = "08";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]);}

        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]+1))){
            //september
            month = "09";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]);}


        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]+DaysMonths[10]+1))){
            //october
            month = "10";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]);}


        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]+DaysMonths[10]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]+DaysMonths[10]+DaysMonths[11]+1))){
            //november
            month = "11";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]-DaysMonths[10]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]-DaysMonths[10]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]-DaysMonths[10]);}

        }else if((DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]+DaysMonths[10]+DaysMonths[11]<day_of_year)&&(day_of_year<(DaysMonths[1]+DaysMonths[2]+DaysMonths[3]+DaysMonths[4]+DaysMonths[5]+DaysMonths[6]+DaysMonths[7]+DaysMonths[8]+DaysMonths[9]+DaysMonths[10]+DaysMonths[11]+DaysMonths[12]+1))){
            //december
            month = "12";
            if(to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]-DaysMonths[10]-DaysMonths[11]).size()==1){
                day = zero + to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]-DaysMonths[10]-DaysMonths[11]);
            }else{day = to_string(day_of_year-DaysMonths[1]-DaysMonths[2]-DaysMonths[3]-DaysMonths[4]-DaysMonths[5]-DaysMonths[6]-DaysMonths[7]-DaysMonths[8]-DaysMonths[9]-DaysMonths[10]-DaysMonths[11]);}

        }

        return year.append("-").append(month).append("-").append(day);

    }

}

#endif //DATABASE_UNIVERSITA2022_DATES_H
