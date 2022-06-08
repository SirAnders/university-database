//
// Created by Andrei Gheorghiu on 5/29/2022.
//

#ifndef DATABASE_UNIVERSITA2022_UNIVERSITY_H
#define DATABASE_UNIVERSITA2022_UNIVERSITY_H

#include <iostream>
#include <vector>
#include <string>
#include "uni_database.h"
#include "utility.h"
#include "mmath.h"
#include "dates.h"

using std::vector;
using std::string;
using std::pair;
using std::map;
using std::cout;
using utility::ExtractInstanceDetails;
using utility::IntToCourseID;
using utility::TokenizeString;
using utility::isYear;
using utility::ExtractCorsiSpentiMajors;
using utility::ExtractInstanceDetails;
using utility::ExtractVersionDetails;
using utility::ExtractVersionsFromCourses;
using dates::DateToEpoch;
using dates::EpochToDate;
using dates::IsDateValid;

struct c_prof{
    int ore_lez;
    int ore_lab;
    int ore_es; //minutes
    int id;
    string s_id;
    bool titolare;
};
struct versione{
    int titolare;
    string anno;
    string codice_versione;
    vector<c_prof> lista_prof;
};
struct instance{
    string anno;
    bool attivo;
    int nr_versioni;
    vector<string> raggrupamento;
    vector<string> codici_versioni;
    int durata_esame; //minuti
    int t_ingresso;
    int t_uscita;
    string modalita; //progetto,scritto,progetto e scritto,orale
    string luogo; // A|L   aula o lab
    int nr_iscritti;
    vector<versione> lista_versioni;
};


class Student{
private:
    int _matricola;
    string _email;
    string _nome;
    string _cognome;
public:

    Student(int mat = 0,const string& em = "",const string& nom = "",const string& cogn = ""){
        _matricola = mat;
        _email = em;
        _nome = nom;
        _cognome = cogn;
    }

    int matricola(){
        if(_matricola>0){
            return _matricola;
        }
        return 0;
    }

    string email(){
        return _email;
    }

    string nome(){
        return _nome;
    }
    string cognome(){
        return _cognome;
    }


};

class Prof{
private:
    int m_matricola;
    string m_email;
    string m_nome;
    string m_cognome;
    map<int,vector<pair<int,int>>> profAV;
public:


    Prof(int mat = 0,const string& em = "",const string& nom = "",const string& cogn = "",const map<int,vector<pair<int,int>>>& av = map<int,vector<pair<int,int>>>()){
        m_matricola = mat;
        m_email = em;
        m_nome = nom;
        m_cognome = cogn;
        profAV = av;
    }

    bool isAvailable(const string& date){
        //outputs true if prof X is available on date YYYY-MM-DD  of the exam sessions.
        //profAV.find(m_matricola);
        int data = DateToEpoch(date);
        auto it = profAV.find(m_matricola);
        if(it!=profAV.end()){
            //prof found

            for(auto x : (*it).second){
                //loop through vector of pairs
                if( (data<=x.second)  && (data>=x.first) ){
                    //not available
                  //  cout<<data<<" is between: "<<x.first<<" and "<<x.second<<endl;/////////////
                  //  cout<<date<<" is between: "<<EpochToDate(x.first)<<" and "<<EpochToDate(x.second)<<endl;/////////////
                    return false;
                }
            }

            return true;
        }else{
            //prof is not present in availability file -> available
            return true;
        }
    }

    int matricola(){
        if(m_matricola > 0){
            return m_matricola;
        }
        return 0;
    }

    string email(){
        return m_email;
    }

    string nome(){
        return m_nome;
    }
    string cognome(){
        return m_cognome;
    }


};

class Major{
private:
    string m_degree; //MS or BS
    int m_codice;
    vector<string> m_spenti;
    vector<string> m_first_sem;
    vector<string> m_second_sem;
    vector<string> m_third_sem;
    vector<string> m_fourth_sem;
    vector<string> m_fifth_sem;
    vector<string> m_sixth_sem;

    friend class Course;
public:

    Major(int id,const string& type,const string& courses,const string& scourses)
    {
        m_codice = id;

        m_degree = type;
        //convert courses string to semester vectors

        vector<vector<string>> temp = utility::ExtractCoursesFromMajors(courses);
        for(int i = 0;i<temp.size();i++){
            switch(i){
                case 0:
                    m_first_sem = temp[i];
                    break;
                case 1:
                    m_second_sem = temp[i];
                    break;
                case 2:
                    m_third_sem = temp[i];
                    break;
                case 3:
                    m_fourth_sem = temp[i];
                    break;
                case 4:
                    m_fifth_sem = temp[i];
                    break;
                case 5:
                    m_sixth_sem = temp[i];
                    break;
                default:
                    cout<<"\nNo major can have more than 6 semesters.\n";
                    break;
            }
        }

        //convert scourses string to corsi spenti vector

        vector<string> spt = ExtractCorsiSpentiMajors(scourses);
        m_spenti = spt;
    }

    string degree(){
        if((m_degree == "MS") || (m_degree == "BS")){
            return m_degree;
        }else{
            return "";
        }
    }

    vector<string> spenti(){
        return m_spenti;
    }
    vector<string> first_sem(){
        return m_first_sem;
    }
    vector<string> second_sem(){
        return m_second_sem;
    }
    vector<string> third_sem(){
        return m_third_sem;
    }
    vector<string> fourth_sem(){
        return m_fourth_sem;
    }
    vector<string> fifth_sem(){
        return m_fifth_sem;
    }
    vector<string> sixth_sem(){
        return m_sixth_sem;
    }
};

class Course{
private:

    int m_cid;
    int m_nr_versione;
    int m_ore_es;
    int m_ore_lab;
    int m_cfu;
    int m_majorid;
    int m_semester;
    int m_ore_lez;
    string m_titolo;

    map<string,instance> m_instances; //course instances are searched by year. YYYY-YYYY

    friend class Major;
public:
    //CONSTRUCTOR

    Course() = default;

    Course(int key,const std::vector<string> &content,const std::map<int,Major> &m)
    {

        m_cid = key;
        m_titolo = content[0];
        m_cfu = std::stoi(content[1]);
        m_ore_lez = std::stoi(content[2]);
        m_ore_es = std::stoi(content[3]);
        m_ore_lab = std::stoi(content[4]);
        int enumber = content.size() - 5;

        if(enumber % 6 == 0){
            int number_courses = enumber/6;

            for(int i = 0;i < number_courses; i++){

                instance temp;
                temp.anno = content[5+6*i];
                temp.nr_versioni = std::stoi(content[5+6*i+2]);

                if(content[5+6*i+1] == "attivo"){
                    temp.attivo = true;
                }else if(content[5+6*i+1] == "non-attivo"){
                    temp.attivo = false;
                }
                //Call functions to extract data from strings. Defined in "utility.h"
                std::vector<string> versions_temp =  ExtractVersionsFromCourses(content[5+6*i+3]);
                std::pair<string,vector<vector<string>>> details_temp;//first is codice versione, second is d123456 0 50 20 etc.
                std::vector<pair<string,vector<vector<string>>>> versions_per_instance;
                for(const auto& j : versions_temp){
                    details_temp = ExtractVersionDetails(j);
                    versions_per_instance.push_back(details_temp);
                }

                for(int k = 0;k<versions_per_instance.size();k++){
                    //for every version, define version and add c_prof structures to version
                    versione versione_temp;
                    versione_temp.codice_versione = versions_per_instance[k].first;
                    for(int j = 0;j<(versions_per_instance[k].second).size();j++){
                        c_prof prof_temp;
                        prof_temp.s_id = (versions_per_instance[k].second)[j][0]; // gets m'th prof's ID
                        prof_temp.id = std::stoi(prof_temp.s_id.substr(1,6));
                        prof_temp.ore_lez = stoi((versions_per_instance[k].second)[j][1]);
                        prof_temp.ore_es = stoi((versions_per_instance[k].second)[j][2]);
                        if((versions_per_instance[k].second).size() > 3){
                            prof_temp.ore_lab = stoi((versions_per_instance[k].second)[j][3]);
                        }



                        (versione_temp.lista_prof).push_back(prof_temp);
                    }

                    (temp.lista_versioni).push_back(versione_temp);
                }

                //extract exam details
                std::vector<string> instance_details = ExtractInstanceDetails(content[5+6*i+4]);// e.g. {12,12,99,S,A,15}
                        temp.durata_esame = stoi(instance_details[0]);
                        temp.t_ingresso = stoi(instance_details[1]);
                        temp.t_uscita = stoi(instance_details[2]);
                        temp.modalita = instance_details[3];
                        temp.luogo = instance_details[4];
                            if(instance_details.size() == 5){
                                temp.nr_iscritti = 0;
                            }else if(instance_details.size() == 6){
                                temp.nr_iscritti = stoi(instance_details[5]);
                            }

                //extract raggruppamenti
                    //same function can be used as for the exam details

                    instance_details = ExtractInstanceDetails(content[5+6*i+5]);

                            if(!instance_details.empty()){
                                for(const auto& it : instance_details){
                                    (temp.raggrupamento).push_back(it);
                                }
                            }

                m_instances.insert(std::pair<string,instance>(temp.anno, temp));


                            //search for major
                            bool found_flag = false;
                            for(auto it = m.begin(); (it!=m.end()) && (!found_flag); it++){
                                auto k = *it;
                                std::string cid = IntToCourseID(m_cid);
                                std::vector<string> f1 = (k.second).m_first_sem;
                                std::vector<string> f2 = (k.second).m_second_sem;
                                std::vector<string> f3 = (k.second).m_third_sem;
                                std::vector<string> f4 = (k.second).m_fourth_sem;
                                std::vector<string> f5 = (k.second).m_fifth_sem;
                                std::vector<string> f6 = (k.second).m_sixth_sem;

                                if(std::find(f1.begin(), f1.end(), cid) != f1.end()){
                                    found_flag = true;
                                    m_semester = 1;

                                }else if(std::find(f2.begin(), f2.end(), cid) != f2.end()){
                                    found_flag = true;
                                    m_semester = 2;

                                }else if(std::find(f3.begin(), f3.end(), cid) != f3.end()){
                                    found_flag = true;
                                    m_semester = 1;

                                }else if(std::find(f4.begin(), f4.end(), cid) != f4.end()){
                                    found_flag = true;
                                    m_semester = 2;

                                }else if(std::find(f5.begin(), f5.end(), cid) != f5.end()){
                                    found_flag = true;
                                    m_semester = 1;

                                }else if(std::find(f6.begin(), f6.end(), cid) != f6.end()){
                                    found_flag = true;
                                    m_semester = 2;

                                }

                                if(found_flag){
                                    m_majorid = k.first;
                                }

                            }
                            if(!found_flag){
                                m_semester = 0;//error, major not found
                            }

            }


        }else{
            cout<<"\nCould not construct majors\n";
        }

    }

    std::map<string,instance> GetInstanceList() const {
        return m_instances;
    }

    std::pair<bool,instance> Instance (const string& year) const{
        if(isYear(year)){
            auto it = m_instances.find(year);
            if(it != m_instances.end()){
                return std::pair<bool,instance>(true,(*it).second);
            }else{
                instance random;
                cout<<"\nERROR in university::Course::Instance() : Could not find year "<<year<<" in the course database, please use a valid academic year.\n";
                return std::pair<bool,instance>(false,random);
            }

        }else{
            instance random;
            cout<<"\nERROR in university::Course::Instance() : the input provided is not a valid academic year! (e.g. 2020-2021,2015-2016,etc.)\n";
            return std::pair<bool,instance>(false,random);
        }
    }

//per instance!

    bool active (const instance& in) const{
        return in.attivo;
    }

    int nr_versions (const instance& in) const{
        return in.nr_versioni;
    }

    vector<string> group (const instance& in) const{
        return in.raggrupamento;
    }

    int prof_titolare (const instance& in) const{
       return stoi((((in.lista_versioni[0]).lista_prof)[0]).s_id);
    }

    vector<string> lista_codici_vers (const instance& in) const{
        return in.codici_versioni;
    }

    int exam_duration (const instance& in) const{
        return in.durata_esame;
    }

    int entrance_time (const instance& in) const{
        return in.t_ingresso;
    }

    int exit_time (const instance& in) const{
        return in.t_uscita;
    }

    string mode (const instance& in) const{
        return in.modalita;
    }

    string place (const instance& in) const{
        return in.luogo;
    }

    int nr_booked (const instance& in) const{
        return in.nr_iscritti;
    }

    string version_code (const instance& in,int vers) const{
        if(vers < in.nr_versioni){

            return (in.lista_versioni[vers]).codice_versione;

        }else{
            cout<<"\nERROR in university::Course::version_code() : int vers argument must match the number of versions of the course instance!\n";
            return "";
        }
    }


    //

    int id () const{
        return m_cid;
    }
    int nr_versioni () const{
        return m_nr_versione;
    }
    int ore_esercitazione () const{
        return m_ore_es;
    }
    int ore_lab () const{
        return m_ore_lab;
    }
    int ore_lezione () const{
        return m_ore_lez;
    }
    string titolo () const{
        return m_titolo;
    }
    int cfu () const{
        return m_cfu;
    }
    int course_semester () const{
        return m_semester;
    }

    int majorid() const{
        return m_majorid;
    }
};

class Aula{
private:
    int m_capienza;
    int m_capienza_exam;
    int m_id;
    string m_nome;
    string m_tipo;

public:

    Aula(int id = 0,const string& cap = "", const string& cap_exam = "",const string& nm = "",const string& tp = "")
    {
        m_id = id;
        m_nome = nm;
        m_tipo = tp;
        m_capienza = stoi(cap);
        m_capienza_exam = stoi(cap_exam);

    }

    [[nodiscard]] int capienza() const{
        return m_capienza;
    }
    [[nodiscard]] int capienza_exam() const{
        return m_capienza_exam;
    }
    [[nodiscard]] int codice() const{
        return m_id;
    }
    [[nodiscard]] string nome() const{
        return m_nome;
    }
    [[nodiscard]] string tipo() const{
        if((m_tipo == "A") || (m_tipo == "L")){
            return m_tipo;
        }else{
            return "";
        }
    }
};

#endif //DATABASE_UNIVERSITA2022_UNIVERSITY_H
