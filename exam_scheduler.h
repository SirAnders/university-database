//
// Created by Andrei Gheorghiu on 5/27/2022.
//

#ifndef DATABASE_UNIVERSITA2022_EXAM_SCHEDULER_H
#define DATABASE_UNIVERSITA2022_EXAM_SCHEDULER_H

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <array>
#include <sstream>
#include <chrono>
#include <regex>
#include <algorithm>
#include <map>
#include <iterator>
#include <sstream>
#include "utility.h"
#include "dates.h"
#include "mmath.h"
#include "uni_database.h"
#include "university.h"

using std::vector;
using std::string;
using std::pair;
using std::cout;
using std::map;
using std::unordered_map;
using std::stringstream;
using utility::isYear;
using utility::printVectorString;
using utility::TokenizeString;
using utility::IntToCourseID;
using dates::DateToEpoch;
using dates::EpochToDate;
using dates::IsDateValid;
using std::array;

/* EXAM SCHEDULER HEURISTIC: I take exams in random order then schedule each in the period with least penalty available, if at the end
 *                           the solution is unfit (violates some constraints) take a new random order and repeat the process. Keep admissible solutions in memory
 *                            (reject those which violate unrelaxable constraints). Stop the search instantly if any non-violating solution is found, if after N cycles no perfect solution is found
 *                            stop the search and select the one with the best fitness score to do the scheduling and generate warning files.
 */
struct date{
    int day;
    int month;
    int year;
    std::string literal;
};
struct Session{
    int start_date=1;
    int end_date=1;
};
enum semester {Winter = 1,Summer = 2,Autumn = 3};



class Exam_db{
private:

    Session winter,summer,autumn;

    vector<string> commands;
    vector<string> SessionFileContent;
////Availability Related
    vector<string> AvailabilityContent;
    vector<string> AvailabilityAddContent;
    map<string,vector<vector<string>>> AvailabilityBase; // .first = anno accademico YYYY-YYYY.
    vector<pair<int,int>> periodi_unav;
    map<int,vector<pair<int,int>>> ProfAvb; // to be sent to university.h Prof constructor
    vector<vector<string>> AVData;

////
    friend class Schedule;
public:

    Exam_db() = default;

    bool CheckSessions(const vector<string>& cmd){
        //checks that the sessions are in order, first two 6 weeks and the last 4 weeks.
        winter.start_date = DateToEpoch(cmd[3].substr(0,10));
        winter.end_date = DateToEpoch(cmd[3].substr(11,10));

        summer.start_date = DateToEpoch(cmd[4].substr(0,10));
        summer.end_date = DateToEpoch(cmd[4].substr(11,10));

        autumn.start_date = DateToEpoch(cmd[5].substr(0,10));
        autumn.end_date = DateToEpoch(cmd[5].substr(11,10));

        //check correct sizes       6 weeks = 42 days, 4 weeks = 28 days
        if(winter.end_date-winter.start_date+1 != 42){
            cout<<"\nSESSIONS GENERATION ERROR: Winter session is "<<winter.end_date-winter.start_date+1<<" days long and not 42 days as required (6 weeks).\n";
            return false;
        }
        if(summer.end_date-summer.start_date+1 != 42){
            cout<<"\nSESSIONS GENERATION ERROR: Summer session is "<<summer.end_date-summer.start_date+1<<" days long and not 42 days as required (6 weeks).\n";
            return false;
        }
        if(autumn.end_date-autumn.start_date+1 != 28){
            cout<<"\nSESSIONS GENERATION ERROR: Autumn session is "<<autumn.end_date-autumn.start_date+1<<" days long and not 28 days as required (4 weeks).\n";
            return false;
        }

        if(!((winter.end_date<summer.end_date)&&(summer.end_date<autumn.end_date))){
            cout<<"\nSESSIONS GENERATION ERROR: Sessions are not placed in the correct order. [Winter (6 weeks) -> Summer (6 weeks) -> Autumn (4 weeks)]\n";
            return false;
        }
        commands = cmd;
        return true;
    }

    bool LoadSessionFile(){
        string temp;
        string txts;
        std::fstream sfile;
        sfile.open("db_periodi_esami.txt", std::ios::out | std::ios::app);
        sfile.close();
        sfile.open("db_periodi_esami.txt", std::ios::in | std::ios::out | std::ios::app);
        if (sfile.is_open()) {

            while (sfile.good()) {
                temp = "";
                getline(sfile, temp);
                temp += "\n";
                txts += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txts);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) SessionFileContent.push_back(tmp);
            }



        } else {
            cout << "\nERROR : " <<"db_periodi_esami.txt"<< " failed to open\n";
            return false;
        }

        return true;
    }

    Session FileToSession(const string &year,semester sem) {
        if (sem == Winter) {
            Session s;
            for (const auto &i: SessionFileContent) {
                if (i.substr(0, 9) == year) {
                    vector<string> temp = TokenizeString(i);
                    s.start_date = DateToEpoch(temp[1].substr(0, 10));
                    s.end_date = DateToEpoch(temp[1].substr(11, 10));
                    return s;
                    break;
                }
            }
        } else if (sem == Summer) {
            Session s;
            for (const auto &i: SessionFileContent) {
                if (i.substr(0, 9) == year) {
                    vector<string> temp = TokenizeString(i);
                    s.start_date = DateToEpoch(temp[2].substr(0, 10));
                    s.end_date = DateToEpoch(temp[2].substr(11, 10));
                    return s;
                    break;
                }
            }

        }else if(sem == Autumn){
            Session s;
            for (const auto &i: SessionFileContent) {
                if (i.substr(0, 9) == year) {
                    vector<string> temp = TokenizeString(i);
                    s.start_date = DateToEpoch(temp[3].substr(0, 10));
                    s.end_date = DateToEpoch(temp[3].substr(11, 10));
                    return s;
                    break;
                }
            }
        }
        Session s;
        return s;
    }

    void AddNewSession(){
        bool year_flag = false;
        string session;

        for(const auto& i : SessionFileContent){
            if(i.substr(0,9) == commands[2]) year_flag = true;
            session = i;
        }

        if(!year_flag){
            string temp;
            temp.append(commands[2]);
            temp.append(";");
            temp.append(commands[3]);
            temp.append(";");
            temp.append(commands[4]);
            temp.append(";");
            temp.append(commands[5]);

            SessionFileContent.push_back(temp);
            temp = "";
        }else{
           //year already present, modify it.
          auto it = find(SessionFileContent.begin(),SessionFileContent.end(),session);
            (*it) = "";
            (*it).append(commands[2]).append(";").append(commands[3]).append(";").append(commands[4]).append(";").append(commands[5]);
        }

    }

    bool WriteSessionFile(){
        std::fstream ssfile;
        ssfile.open("db_periodi_esami.txt",std::ios::out);
        if (ssfile.is_open()) {
            for (const string& n : SessionFileContent) {
                ssfile << n << "\n";
            }

            return true;


        } else {
            cout << "\nERROR: Could not edit db_periodi_esami.txt\n";
            return false;
        }
    }

    bool LoadAvailabilityFile(){
        string temp;
        string txts;
        std::fstream sfile;
        sfile.open("db_indisponibilita.txt", std::ios::out | std::ios::app);
        sfile.close();
        sfile.open("db_indisponibilita.txt", std::ios::in | std::ios::out | std::ios::app);
        if (sfile.is_open()) {

            while (sfile.good()) {
                temp = "";
                getline(sfile, temp);
                temp += "\n";
                txts += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txts);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) AvailabilityContent.push_back(tmp);
            }



        } else {
            cout << "\nERROR : " <<"db_indisponibilita.txt"<< " failed to open\n";
            return false;
        }

        return true;
    }

    bool LoadAvailabilityAddFile(const string& name){
        string temp;
        string txts;
        std::fstream sfile;
        sfile.open(name, std::ios::out | std::ios::app);
        sfile.close();
        sfile.open(name, std::ios::in | std::ios::out | std::ios::app);
        if (sfile.is_open()) {

            while (sfile.good()) {
                temp = "";
                getline(sfile, temp);
                temp += "\n";
                txts += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txts);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) AvailabilityAddContent.push_back(tmp);
            }



        } else {
            cout << "\nERROR : " <<name<< " failed to open\n";
            return false;
        }

        return true;
    }

    void printData(){
        for (const auto& i:AVData) {
            printVectorString(i);
        }
    }

    void TokenizeAvailability(){
        if(!AvailabilityContent.empty()){
            vector<vector<string>> output;
            vector<string> temp;
            bool flag = true;
            for(auto it = AvailabilityContent.begin();it!=AvailabilityContent.end();it++){
                if(isYear((*it))){
                    flag = not flag;
                }

                if(flag){
                    flag = not flag;
                    output.push_back(temp);
                    temp.clear();
                    temp.push_back(*it);
                }else{
                    temp.push_back(*it);
                }
            }
            output.push_back(temp);
            AVData = output;
        }

    }

    void MapAvailability(){
        //map by year
        string tempkey;
        vector<string> tempo;
        vector<vector<string>> temp;
        if(!AvailabilityContent.empty()){
            for(auto n : AVData){
                tempkey = n[0];
                for(auto it = n.begin()+1;it!=n.end();it++){
                    tempo = TokenizeString(*it);
                    temp.push_back(tempo);
                }

                AvailabilityBase.insert(pair<string,vector<vector<string>>>(tempkey,temp));
                temp.clear();

            }
        }
    }

    void printAVBase(){
        for(const auto& x:AvailabilityBase){
            cout<<"\n__________________________"<<x.first<<"__________________________\n";

            for(const auto& i : x.second){
                printVectorString(i);
            }

            cout<<"\n_________________________________________________________________\n";
        }
    }

    bool SyntaxAvailabilityAdd(){

        int cnt = 0;
        int flag = 0;
        for (const string& i : AvailabilityAddContent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^d[0-9]{6}\;(([0-9]{4}\-[0-9]{2}\-[0-9]{2}\|[0-9]{4}\-[0-9]{2}\-[0-9]{2}\;)+)?[0-9]{4}\-[0-9]{2}\-[0-9]{2}\|[0-9]{4}\-[0-9]{2}\-[0-9]{2}$)");

            if (!regex_match(i, regs)) {
                cout << "\nERROR: Line number " << cnt
                     << " does not follow the standard format of the professor availability file\n";
                flag = 1;
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            return true;
        }
    }

    bool AddAvailability(const string& anno){
        vector<string> temp;
        bool exists = false;
        bool flag_found = false;
        if(!AvailabilityAddContent.empty()){
            if(!AvailabilityContent.empty()){
                //check if the year already exists
                for (auto it = AVData.begin();it!=AVData.end(); it++)
                {
                    if((*it)[0]==anno){
                        exists = true;
                        auto ptr = AvailabilityBase.find(anno);
                        for(auto x : AvailabilityAddContent){
                            for(auto m = ((*ptr).second).begin();m!=((*ptr).second).end();m++){
                                if((*m)[0]==x.substr(0,7)){
                                    //found same prof, replace
                                    flag_found = true;
                                    vector<string> tempx = TokenizeString(x);
                                   // printVectorString(*m);//////
                                    *m = tempx;
                                   // printVectorString(*m);//////
                                }
                            }
                            if(flag_found==false){
                                ((*ptr).second).push_back(TokenizeString(x));
                            }
                        }
                        flag_found = false;
                    }
                    exists = false;
                }
                if(!exists){
                    vector<vector<string>> temppp;
                    for(const auto& k : AvailabilityAddContent){
                        temppp.push_back(TokenizeString(k));
                    }
                    AvailabilityBase.insert(pair<string,vector<vector<string>>>(anno,temppp));
                }
            }else{
                AvailabilityContent.push_back(anno);
                for(auto x : AvailabilityAddContent){
                    AvailabilityContent.push_back(x);
                }
            }


        }else{
            cout<<"\nERROR in exam_scheduler::Exam_db::AddAvailability() : The new availability file is empty\n";
            return false;
        }

        return true;

    }

    bool CheckIntegrity(const map<int,vector<string>>& Pbase,bool in_scheduler = false){
        //before writing the file, check if the new database makes sense, e.g. no academic year duplicates
        for(auto it = AvailabilityContent.begin();it!=AvailabilityContent.end();it++){
            if(isYear(*it))
             if(std::count(AvailabilityContent.begin(), AvailabilityContent.end(), *it) > 1){
                 //counts how many duplicates are of *it
                 return false;
             }

        }
        //check that the dates are part of the academic year

        for(const auto& x : AvailabilityBase){
            string year = x.first;
            //build 1st january of academic year and 31 December
            string inizio_ac_year = year.substr(0,4) + "-" + "01" + "-" + "01";
            string fine_ac_year = year.substr(5,4) + "-" + "12" + "-" + "31";
            int inizio_ref = DateToEpoch(inizio_ac_year);
            int fine_ref = DateToEpoch(fine_ac_year);

            for(const auto& i : x.second){
                for(auto it = i.begin()+1;it!=i.end();it++){
                    int inizio = DateToEpoch((*it).substr(0,10));
                    int fine = DateToEpoch((*it).substr(11,10));

                    if(!((inizio<=fine_ref)&&(inizio>=inizio_ref))){
                        cout<<"\nERROR in exam_scheduler::Exam_db:CheckIntegrity(): Date "<<(*it).substr(0,10)<<" is not part of the academic year "<<x.first<<"\n";
                        return false;
                    }
                    if(!((fine<=fine_ref)&&(fine>=inizio_ref))){
                        cout<<"\nERROR in exam_scheduler::Exam_db:CheckIntegrity(): Date "<<(*it).substr(11,10)<<" is not part of the academic year "<<x.first<<"\n";
                        return false;
                    }
                    if(fine < inizio){
                        cout<<"\nERROR in exam_scheduler::Exam_db:CheckIntegrity(): Unavailability start date must come before the end date ("<<EpochToDate(inizio)<<" --> "<<EpochToDate(fine)<<")\n";
                        return false;
                    }

                }
            }
        }

        //check profs are present in database
        if(in_scheduler){
            for(const auto& x : AvailabilityBase){
                for(const auto& i : x.second){
                    if(i[0].size() == 7){

                        auto it = Pbase.find(stoi(i[0].substr(1,6)));
                        if(it==Pbase.end()){
                            //couldn't find, send error.
                            cout<<"\nERROR in exam_scheduler::Exam_db:CheckIntegrity(): Couldn't find professor "<<i[0]<<" in the database.\n";
                            return false;
                        }

                    }else{
                        cout<<"\nERROR in exam_scheduler::Exam_db:CheckIntegrity(): First entries must be professor ID's\n";
                        return false;
                    }

                }
            }
        }


        return true;
    }

    bool areProfsInDatabase(){

        return true;
    }

    void MapToContent(){
        if(!AvailabilityBase.empty()){
            string temp;
            AvailabilityContent.clear();
            for(auto x : AvailabilityBase){
                AvailabilityContent.push_back(x.first);
                for(auto y : x.second){

                    for(auto z: y){
                        temp.append(z);
                        temp.append(";");
                    }
                    temp.pop_back();
                    AvailabilityContent.push_back(temp);
                    temp.clear();
                }
            }
        }

    }

    bool WriteAvailability(){
        std::fstream ssfile;
        ssfile.open("db_indisponibilita.txt",std::ios::out);
        if (ssfile.is_open()) {
            for (const string& n : AvailabilityContent) {
                ssfile << n << "\n";
            }

            return true;


        } else {
            cout << "\nERROR: Could not edit db_indisponibilita.txt\n";
            return false;
        }
    }

    vector<string> GetAvailabilities(){
        return AvailabilityContent;
    }

    map<int,vector<pair<int,int>>>GetProfAV(){
        return ProfAvb;
    }

    void MapProfAV(){
        if(!AvailabilityBase.empty()){
            vector<pair<int,int>> tempv;
            for(auto& i : AvailabilityBase){
                for(auto& j : i.second){
                    int tempkey = stoi(j[0].substr(1,6));
                    for(auto it = j.begin()+1;it!=j.end();it++){
                        //insert periodi unavailable
                        int inizio = DateToEpoch((*it).substr(0,10));
                        int fine = DateToEpoch((*it).substr(11,10));

                        pair<int,int> temp = pair<int,int>(inizio,fine);
                        tempv.push_back(temp);

                    }
                    auto ptr = ProfAvb.find(tempkey);
                    if(ptr!=ProfAvb.end()){
                        ((*ptr).second).insert( ((*ptr).second).end(), tempv.begin(), tempv.end() );
                    }else{
                        ProfAvb.insert(pair<int,vector<pair<int,int>>>(tempkey,tempv));
                    }
                    tempv.clear();
                }


            }
        }

    }

    void printProfAV(){
        for(const auto& x:ProfAvb){
            cout<<"\n__________________________"<<x.first<<"__________________________\n";

            for(const auto& i : x.second){

                     cout<<"\n";
                     cout<<EpochToDate(i.first)<<" ---> "<< EpochToDate(i.second);
                     cout<<"\n";

            }

            cout<<"\n_________________________________________________________________\n";
        }
    }




};

class Exam{
private:

    struct relaxable_constraints{
        bool at_least_one = false;
        bool mindist_intermajor = false;
        bool mindist_exam = false;
        bool unavailability = false;
        bool classrooms = false;
    };

    bool m_scheduled;
    vector<bool> m_constraints;

    int m_course_id;
    string m_year;
    int m_version;
    int m_major_id;
   // int m_grouping_id;
   int m_nr_vers;
    int m_nr_appelli;
    int m_appello;  // int = 1 or 2.
    Course m_corso;
    string m_codice_vers;
    int m_duration;
    int m_slot_duration;
    vector<int> m_profs_list;

    friend class Schedule;
    friend class Course;
    friend class Exam_db;
public:

   // Exam() = default;

    Exam(int aps,const Course &corso,int vers,const string &year,int appl,const instance &in){
        m_nr_vers = aps;
        m_year = year;
        m_corso = corso;
        m_course_id = m_corso.id();
        m_major_id = m_corso.majorid();
        m_version = m_corso.nr_versioni();
        m_appello = appl;
       // m_codice_vers = m_corso.version_code();
        auto it = m_corso.GetInstanceList().find(year);
        m_codice_vers = m_corso.version_code((*it).second,vers);
        m_duration = m_corso.exam_duration(in) + m_corso.entrance_time(in) + m_corso.exit_time(in);
        m_slot_duration = mmath::ceil((float)((float)m_duration/120));
        m_scheduled = false;
        m_constraints.push_back(false); m_constraints.push_back(false); m_constraints.push_back(false); m_constraints.push_back(false);
        m_version = vers;
        for(auto const &x : (m_corso.Instance(year).second).lista_versioni[m_version].lista_prof){
            m_profs_list.push_back(x.id);
        }


    }




    [[nodiscard]] Course course_exam() const{
       return m_corso;
   }

   [[nodiscard]] int getVersion() const{
       return m_version;
   }

   [[nodiscard]] string codice_vers() const{
       return m_codice_vers;
   }

   [[nodiscard]] int appello() const{
       return m_appello;
   }

    [[nodiscard]] int nr_app() const{
        return m_nr_appelli;
    }

    [[nodiscard]] int duration() const{
        return m_duration;
    }

    [[nodiscard]] int slot_duration() const{
       return m_slot_duration;
   }

    [[nodiscard]] vector<bool> constraints() const{
       return m_constraints;
   }

   [[nodiscard]] vector<int> profList() const{
       return m_profs_list;
   }

};

class Schedule{
private:
    semester m_semester;
    vector<vector<slot>> group_schedule;
   // vector<vector<slot>> aule_schedule;
    vector<vector<std::shared_ptr<Exam>>> groups;
public:
     Schedule(semester sem,const vector<vector<std::shared_ptr<Exam>>> &g){
         groups = g;
        m_semester = sem;
        vector<slot> ttemp;

        if((m_semester == Winter)||(m_semester == Summer)){
            auto temp = vector<slot>(6);
          //  aule_schedule = vector<vector<slot>>(42,temp);
            for(int i = 0;i<42;i++){
                for(int j = 0;j<6;j++){
                    slot tempp;
                    // stemp.slot_exam = exam;
                    if((i == 6)||(i == 13)||(i == 20)||(i == 27)||(i == 34)||(i == 41)){
                        //domenica
                        tempp.sunday = true;
                        ttemp.push_back(tempp);
                    }else{
                        ttemp.push_back(tempp);
                    }

                }
                group_schedule.push_back(ttemp);
                ttemp.clear();

            }

        }else if(m_semester == Autumn){
          //  auto www = vector<slot>(6);
            //aule_schedule = vector<vector<slot>>(28,www);
            for(int i = 0;i<28;i++){
                for(int j = 0;j<6;j++){
                    slot temp;
                    // stemp.slot_exam = exam;
                    if((i == 6)||(i == 13)||(i == 20)||(i == 27)){
                        //domenica
                        temp.sunday = true;
                        ttemp.push_back(temp);
                    }else{
                        ttemp.push_back(temp);
                    }

                }
                group_schedule.push_back(ttemp);
                ttemp.clear();

            }

        }

            }

    vector<vector<slot>> time_calendar() {
        return group_schedule;
    }

    void setTSchedule(const vector<vector<slot>> &in)
    {
         group_schedule = in;
     }


    [[nodiscard]] semester sem() const{
        return m_semester;
    }

    void clearSchedule(){
        if((m_semester == Winter)||(m_semester == Summer)){
            auto temp = vector<slot>(6);
          //  aule_schedule = vector<vector<slot>>(42,temp);
            group_schedule =  vector<vector<slot>>(42,temp);

        }else if(m_semester == Autumn){
            auto temp = vector<slot>(6);
          //  aule_schedule = vector<vector<slot>>(28,temp);
            group_schedule =  vector<vector<slot>>(28,temp);
        }
     }

    void printCalendar(){
        int cnt = 1;
        for(const auto &x : group_schedule){
            cout<<"\n__________________DAY "<<cnt<<"__________________\n";
            for(const auto &y : x){
                for(const auto &z : y.indexes){
                    //print course ID's
                    for(const auto &w : groups[z]){
                        cout<<utility::IntToCourseID(w->m_corso.id())<<" | ";
                    }

                }
                cout<<"\n";
            }
            cnt++;
            cout<<"\n_______________________________________________\n";
        }
    }

    void printClassroomCalendar(){
        int cnt = 1;
        for(const auto &x : group_schedule){
            cout<<"\n__________________DAY "<<cnt<<"__________________\n";
            for(const auto &y : x){
                for(const auto &z : y.aind){
                    //print course ID's
                    cout<<utility::IntToAulaID(z)<<" | ";

                }
                cout<<"\n";
            }
            cnt++;
            cout<<"\n_______________________________________________\n";
        }
     }
};

bool checkProfAvailability(Prof &p,int day,Session &s){
    if(p.isAvailable(EpochToDate(s.start_date+day))){
        return true;
    }else{
        return false;
    }
}

bool checkNoSameProfsSameTime(const vector<std::shared_ptr<Exam>> &new_group, const vector<vector<slot>> &cal, int day, int time,const vector<vector<std::shared_ptr<Exam>>> &groups,const map<int,Prof> &p){
    vector<int> pkey_compare_list;//check for doubles here.
   // vector<int> new_list;//


    for(const auto &x : cal[day][time].indexes){

        for(const auto & k : groups[x]){
            //check every exam in group x
            for(const auto &m : k->profList()){
                auto it = p.find(m);
                pkey_compare_list.push_back((*it).first);
            }
        }

    }

    for(const auto & k : new_group){
        //check every exam in group x
        for(const auto &m : k->profList()){
            auto it = p.find(m);
            pkey_compare_list.push_back((*it).first);
        }
    }
    std::sort(pkey_compare_list.begin(), pkey_compare_list.end());
    const bool hasDuplicates = std::adjacent_find(pkey_compare_list.begin(), pkey_compare_list.end()) != pkey_compare_list.end();
 ///////////////////////////////////////
 /*
    cout<<"\n___________"<<day<<" | "<<time<<"__________\n";

    cout<<"TO BE ADDED GROUP: ";
    for(const auto &k : new_group){
        for(const auto &m : k->profList()){
            cout<<m<<" | ";
        }
    }
    cout<<"\nCURRENT GROUPS: ";
    for(const auto &k : cal[day][time].indexes){
        for(const auto & j : groups[k]){
            //check every exam in group x
            for(const auto &m : j->profList()){
                cout<<m<<" | ";
            }
        }
    }

    cout<<"\n_____________\n";
    */
//////////////////////////////////////////
    if(hasDuplicates){
        return false;
    }else{
        return true;
    }

}

bool MinDistTwoDays(int day,int time, const vector<std::shared_ptr<Exam>> &new_group,Schedule &sc,const vector<vector<slot>> &cal,semester sem,const vector<vector<std::shared_ptr<Exam>>> &groups){
    //2 giorni between same major courses
    vector<int> major_id_list;
    for(const auto &i : new_group){
        int temp = i->course_exam().majorid();
        if(find(major_id_list.begin(),major_id_list.end(),temp) == major_id_list.end()){
            major_id_list.push_back(temp);
        }
    }

    if((sc.sem() == Winter) || (sc.sem() == Summer)){
        if((day>0) && (day < 41)){
            for(int i = -1;i<2;i++){
                    for(auto it = cal[day+i].begin();(it!=cal[day+i].end())&&(it - cal[day+i].begin() != time);it++){
                        auto k = *it;
                        for(const auto &n : k.indexes){
                            for(const auto &v : groups[n]){
                                if(std::find(major_id_list.begin(),major_id_list.end(),v->course_exam().majorid()) != major_id_list.end()){
                                    return false; // found same major course within 2 days of current group to be scheduled
                                }
                            }
                        }
                    }
            }
        }else if(day == 0){
            for(int i = 0;i<2;i++){
                for(auto it = cal[day+i].begin();(it!=cal[day+i].end())&&(it - cal[day+i].begin() != time);it++){
                    auto k = *it;
                    for(const auto &n : k.indexes){
                        for(const auto &v : groups[n]){
                            if(std::find(major_id_list.begin(),major_id_list.end(),v->course_exam().majorid()) != major_id_list.end()){
                                return false; // found same major course within 2 days of current group to be scheduled
                            }
                        }
                    }
                }
            }
        }else if(day == 41){
            for(int i = -1;i<1;i++){
                for(auto it = cal[day+i].begin();(it!=cal[day+i].end())&&(it - cal[day+i].begin() != time);it++){
                    auto k = *it;
                    for(const auto &n : k.indexes){
                        for(const auto &v : groups[n]){
                            if(std::find(major_id_list.begin(),major_id_list.end(),v->course_exam().majorid()) != major_id_list.end()){
                                return false; // found same major course within 2 days of current group to be scheduled
                            }
                        }
                    }
                }
            }
        }
    }else{
        if((day>0) && (day < 27)){
            for(int i = -1;i<2;i++){
                for(auto it = cal[day+i].begin();(it!=cal[day+i].end())&&(it - cal[day+i].begin() != time);it++){
                    auto k = *it;
                    for(const auto &n : k.indexes){
                        for(const auto &v : groups[n]){
                            if(std::find(major_id_list.begin(),major_id_list.end(),v->course_exam().majorid()) != major_id_list.end()){
                                return false; // found same major course within 2 days of current group to be scheduled
                            }
                        }
                    }
                }
            }
        }else if(day == 0){
            for(int i = 0;i<2;i++){
                for(auto it = cal[day+i].begin();(it!=cal[day+i].end())&&(it - cal[day+i].begin() != time);it++){
                    auto k = *it;
                    for(const auto &n : k.indexes){
                        for(const auto &v : groups[n]){
                            if(std::find(major_id_list.begin(),major_id_list.end(),v->course_exam().majorid()) != major_id_list.end()){
                                return false; // found same major course within 2 days of current group to be scheduled
                            }
                        }
                    }
                }
            }
        }else if(day == 27){
            for(int i = -1;i<1;i++){
                for(auto it = cal[day+i].begin();(it!=cal[day+i].end())&&(it - cal[day+i].begin() != time);it++){
                    auto k = *it;
                    for(const auto &n : k.indexes){
                        for(const auto &v : groups[n]){
                            if(std::find(major_id_list.begin(),major_id_list.end(),v->course_exam().majorid()) != major_id_list.end()){
                                return false; // found same major course within 2 days of current group to be scheduled
                            }
                        }
                    }
                }
            }
        }
    }



    return true;
}

bool MinDistAppelli(int day,int time, const vector<std::shared_ptr<Exam>> &new_group,Schedule &sc,const vector<vector<slot>> &cal,semester sem,const vector<vector<std::shared_ptr<Exam>>> &groups){
    //14 giorni
 if(new_group[0]->nr_app() == 2){
     //loop through schedule and if any distance between the different appeals of the same course are at less than 14 days apart, return false.
     for(const auto &i : new_group){
         for(auto it = cal.begin();it!=cal.end();){

             for(auto ptr = (*it).begin();ptr!=(*it).end();ptr++){
                 auto k = *ptr;
                 for(const auto &n : k.indexes){
                     for(const auto &v : groups[n]){
                         if(v->course_exam().id() == i->course_exam().id()){
                            if(v->appello() != i->appello()){
                                if(mmath::abs( (int)(it-cal.begin()) - (day) ) < 14){
                                    return false;
                                }
                            }
                         }
                     }
                 }
             }

         }
     }
 }

    return true;
}

bool isClassroomAvailableForExamDuration(int day, int time,int slots_duration,vector<vector<slot>> &cl,int aula_index){

    for(int i =0;i<slots_duration;i++){
        if(find(cl[day][time+i].aind.begin(),cl[day][time+i].aind.end(),aula_index) != cl[day][time+i].aind.end()){
            return false;
        }
    }

    return true;
}

bool checkFreeClassrooms(int day,int time,const vector<std::shared_ptr<Exam>> &new_group,Schedule &sc,vector<vector<slot>> &cl,semester sem,const map<int,Aula> &a,const string &y){
  //  vector<vector<slot>> acal = sc.aule_calendar();
    int added_aule_cnt = 0;
bool flag = false;
    for(const auto &k : new_group){
      int iscritti =   k->course_exam().nr_booked(k->course_exam().Instance(y).second);
      for(const auto &i : a){
          if(((i.second).capienza_exam()) >= iscritti){
              //enough space
              if(i.second.tipo() == k->course_exam().Instance(y).second.luogo){
                  //check available classroooms for all slots to be occupied!
                  if(isClassroomAvailableForExamDuration(day,time,k->slot_duration(),cl,i.first)){
                      flag = true;
                      added_aule_cnt++;
                      for(int o =0;o<k->slot_duration();o++){
                          cl[day][time+o].aind.push_back(i.first);
                      }
                      break;
                  }
              }
          }
      }
      if(!flag){
          //pop back all classrooms added for this slot for this specific group.
          for(int j = 0;j<added_aule_cnt;j++)
              cl[day][time].aind.pop_back();
          return false;
      }
      flag = false;
    }


    return true;
}


bool checkOkTime(int day,int time, const vector<std::shared_ptr<Exam>> &new_group,Schedule &sc,semester sem){
   // if(utility::IntToCourseID(new_group[0]->course_exam().id()) == "01AAAAA"){cout<<new_group[0]->slot_duration()+time<<endl;}

        if(sem == Summer){
            if(new_group[0]->course_exam().course_semester() == 2){
                if(new_group[0]->appello() == 1){
                    if(day < 14){
                        //first two weeks
                        if((day != 13) && (day!= 6)){
                            //check space
                            auto cal = sc.time_calendar();

                            if(new_group[0]->slot_duration()+time>5){
                                //no time today!
                            }else{
                                //if(utility::IntToCourseID(new_group[0]->course_exam().id()) == "01AAAAA"){cout<<"PASSED!\n";}
                                return true;
                            }
                        }else{   }
                    }else{   }
                }else if(new_group[0]->appello() == 2){
                    if(day > 14-1){
                        //last 4 weeks
                        if((day != 20) && (day!= 27) && (day != 34) && (day != 41)){
                            auto cal = sc.time_calendar();
                            if(new_group[0]->slot_duration()+time>5){
                                //no time today!
                            }else{
                                return true;
                            }
                        }else{

                        }
                    }else{}
                }

            }else if(new_group[0]->course_exam().course_semester() == 1){
                if(day > 14-1){
                    if((day != 20) && (day!= 27) && (day != 34) && (day != 41)){
                        //check space
                        auto cal = sc.time_calendar();

                        if(new_group[0]->slot_duration()+time>5){
                            //no time today!
                        }else{
                            return true;
                        }
                    }
                }


            }
        }else if(sem == Winter){

            if(new_group[0]->course_exam().course_semester() == 1){

                if(new_group[0]->appello() == 1){

                    if(day < 14){

                        //first two weeks

                        if((day != 13) && (day!= 6)){
                            //check space
                            auto cal = sc.time_calendar();

                            if(new_group[0]->slot_duration()+time>5){
                                //no time today!


                            }else{
                                //if(utility::IntToCourseID(new_group[0].course_exam().id()) == "01AAAAA"){cout<<"PASSED!\n";}
                                return true;
                            }
                        }else{

                        }
                    }else{

                    }

                }else if(new_group[0]->appello() == 2){

                    if(day > 14-1){
                        //last 4 weeks
                        if((day != 20) && (day!= 27) && (day != 34) && (day != 41)){
                            auto cal = sc.time_calendar();

                            if(new_group[0]->slot_duration()+time>5){
                                //no time today!

                            }else{
                                return true;
                            }
                        }else{

                        }
                    }else{

                    }

                }



            }else if(new_group[0]->course_exam().course_semester() == 2){
                if(day > 14-1){
                    if((day != 20) && (day!= 27) && (day != 34) && (day != 41)){
                        //check space
                        auto cal = sc.time_calendar();

                        if(new_group[0]->slot_duration()+time>5){
                            //no time today!
                        }else{
                            return true;
                        }
                    }
                }




            }
        }else if(sem == Autumn){


            if((day != 13) && (day!= 6) && (day != 20) && (day!= 27)){
                //check space
                auto cal = sc.time_calendar();

                if(new_group[0]->slot_duration()+time>5){
                    //no time today!
                }else{
                    return true;
                }
            }




        }



///////////////OKTIME FAILED GROUPS///////////
/*
    if(new_group[0]->course_exam().id() == 0){
        cout<<new_group[0]->appello()<<" | "<<utility::IntToCourseID(new_group[0]->course_exam().id())<<endl;////////////////////DEBUG
        cout<<day<<" "<<time<<" | semester: "<<new_group[0]->course_exam().course_semester()<<endl;
    }
*/
///////////////////////////////////////////////////////////////////

    return false;
}

pair<bool,int> ConstraintsAreRespected(int relax_level, const vector<std::shared_ptr<Exam>> &group, Schedule &sc, int day, int time, const string &year, Session &s, const map<int,Prof> &p,const vector<vector<std::shared_ptr<Exam>>> &groups,const map<int,Aula> &a){
    auto calendar = sc.time_calendar();
    if(relax_level == 0){

        //check that there are enough not_full slots with available classrooms for the group to be placed at this time



        if(!MinDistAppelli(day,time,group,sc,sc.time_calendar(),sc.sem(),groups)){

            return {false,relax_level};
        }
        if(!MinDistTwoDays(day,time,group,sc,sc.time_calendar(),sc.sem(),groups)){

            return {false,relax_level};
        }


        if(!checkOkTime(day,time,group,sc,sc.sem())){
            return {false,relax_level};
        }

        if(!checkNoSameProfsSameTime(group,sc.time_calendar(),day,time,groups,p)){

            return {false,relax_level};
        }

        //check prof availability
        bool flag_nav = false;
        for(const auto &x : group){

            for(const auto &m : ((((x->course_exam()).Instance(year)).second).lista_versioni[0].lista_prof)){

                auto it = p.find(m.id);
                Prof ptemp = (*it).second;

                if(!(checkProfAvailability(ptemp,day,s))){
                    flag_nav = true;
                }
            }
        }
        if(flag_nav){

            return {false,relax_level};
        }


    }




    return {true,0};
}


bool ScheduleExams(const vector<vector<std::shared_ptr<Exam>>> &groups, Schedule &sc, int relax_level, const map<int,Prof> &p, Session &s, const string &y, const map<int,Aula> &a){
    int max_days;
    if(sc.sem() == Autumn){max_days = 28;}
    if((sc.sem() == Summer) || (sc.sem() == Winter)){max_days = 42;}
    vector<vector<slot>> cal = sc.time_calendar();
  //  vector<vector<slot>> acal = sc.aule_calendar();
        vector<int> indx;
        bool done = false;
       // vector<vector<slot>> cal = sc.calendar();
        indx.reserve(max_days);
        for(int i =0;i<max_days;i++){indx.push_back(i);}
        std::shuffle ( indx.begin(), indx.end() , std::mt19937(std::random_device()()));

        //standard exam scheduling process, no constraints are allowed to be violated
        //random sample from group vector
        for(int i = 0;i<indx.size();i++){
            //loop through groups
            //for(auto it = cal.begin();(it!=cal.end()) && (!done);it++)
            bool flag_assigned = false;
            //loop through slots

            for(auto it = cal.begin();(it!=cal.end()) &&(!flag_assigned);it++){
                auto sl=*it;
                for(auto ptr = sl.begin();(ptr != sl.end()) && (!flag_assigned) ;ptr++){
                    //for each slot if no constraints are violated, stop looping through calendar and assign the group to the slot, assign a free classroom for each exam in the group.

                    pair<bool,int> temp = ConstraintsAreRespected(relax_level, groups[indx[i]], sc, (int) (it - cal.begin()),
                                                                  (int) (ptr - sl.begin()), y, s, p,groups,a);

                    if(temp.first){

                        if(checkFreeClassrooms((int) (it - cal.begin()),(int) (ptr - sl.begin()),groups[indx[i]],sc,cal,sc.sem(),a,y)){

                            flag_assigned = true;
                            cal[(int)(it-cal.begin())][(int)(ptr-sl.begin())].empty = false;
                            cal[(int)(it-cal.begin())][(int)(ptr-sl.begin())].indexes.push_back(indx[i]);
                        }else{

                        }

                    }else{

                        //constraints not respected

                    }
                }


            }


           if(!flag_assigned){
               /*
               if(sc.sem() == Winter){
                   cout<< utility::IntToCourseID(groups[indx[i]][0]->course_exam().id())<<endl;///////
               }
                */
               return false;
           }


        }

        //load new schedule into Schedule class.
        sc.setTSchedule(cal);
        //if it passed through all the first loop it means each group was assigned somewhere
        return true;

}

vector<string> genSessionFileContent(Session &s,Schedule &sc,const string &year, const vector<vector<std::shared_ptr<Exam>>> &groups,const map<int,Aula> &a){
    std::vector<string> buff;
    std::string temp;
    int start_date = s.start_date;
    int end_date = s.end_date;
    auto cal = sc.time_calendar();
    for(auto it = cal.begin();it!=cal.end();it++){
        int day = (int)(it-cal.begin());
        if(!std::all_of(cal[day].begin(),cal[day].end(),utility::isSlotEmpty)){
            temp.append(dates::EpochToDate(start_date+day));
            temp.append("\n");

            for(auto ptr = (*it).begin();ptr!=(*it).end();ptr++){
                int time = (int)(ptr-(*it).begin());
                if(!utility::isSlotEmpty(cal[day][time])){
                    temp.append(utility::IntToTimeOfDay(time));
                    temp.append(";");
                    int scnt = 0;
                    for(int i = 0;i<cal[day][time].indexes.size();i++){
                        //for each exam in the groups, add it
                        for(const auto &m : groups[cal[day][time].indexes[i]]){
                            temp.append(utility::IntToCourseID(m->course_exam().id()));
                            temp.append("[");
                            temp.append(m->codice_vers());
                            temp.append("]");
                            temp.append("(");
                            temp.append(utility::IntToAulaID(m->course_exam().majorid()));
                            temp.append(")|");
                            auto fnd = a.find(cal[day][time].aind[scnt]);
                            if(fnd != a.end())
                                temp.append((*fnd).second.nome());

                            temp.append(";");


                            scnt++;
                        }
                    }

                    temp.append("\n");


                    scnt = 0;
                }

            }
            buff.push_back(temp);
            temp.clear();
        }

    }

    return buff;
}

 bool genSessionFile(string nome_file,Session &s,Schedule &sc,const string &year,const vector<string> &content){
     std::fstream ssfile;

     for(int i = 0;i<4;i++){
         nome_file.pop_back();
     }

     nome_file.append("_s");
     if(sc.sem() == Winter){ nome_file.append("1.txt");}
     if(sc.sem() == Summer){ nome_file.append("2.txt");}
     if(sc.sem() == Autumn){ nome_file.append("3.txt");}
     ssfile.open( nome_file,std::ios::out);
     if (ssfile.is_open()) {
         for (const string& n : content) {
             ssfile << n;
         }

         return true;


     } else {
         cout << "\nERROR: Could not edit "<<nome_file<<"\n";
         return false;
     }
}


#endif //DATABASE_UNIVERSITA2022_EXAM_SCHEDULER_H
