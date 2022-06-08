//
// Created by Andrei Gheorghiu on 5/16/2022.
//

#ifndef UNI_DATABASE_H
#define UNI_DATABASE_H
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
#include "utility.h"
#include "dates.h"

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::fstream;
using std::iterator;
using std::endl;
using std::unordered_map;
using utility::ExtractInstanceDetails;
using utility::IntToCourseID;
using utility::TokenizeString;
using utility::isYear;
using utility::ExtractCorsiSpentiMajors;
using utility::ExtractInstanceDetails;
using utility::ExtractVersionDetails;
using utility::ExtractVersionsFromCourses;
using utility::AulaIDtoInt;
using utility::IntToAulaID;
using utility::IntToID;
using utility::printVectorString;
using utility::CourseIDtoInt;
using utility::IntToCourseID;

class Database {
// Functions executed correctly return TRUE.  FALSE if errors were found.

protected:
    string filecopy;
    bool DisarmEditing;

public:


    Database(){
        DisarmEditing = true;
    }

    virtual bool SyntaxChecker(const int &f,const string &cmd) = 0; // checks correct syntax of the main database files

    virtual bool did_it_Fail() = 0; // gives 'true' if a database file failed to be opened or created

    virtual bool a_load_File(const string &name) = 0; // loads add file into memory

    virtual void Mapper() = 0;

    virtual void Rebuild() = 0;

    virtual bool Update() = 0;

    virtual void u_Tokenizer() = 0;

    virtual bool u_load_File(const string &n) = 0; // loads update file into memory

    virtual bool a_SyntaxChecker() = 0;  //checks syntax format for the file to be added

    virtual bool u_SyntaxChecker() = 0; //checks syntax format for the update file

    virtual bool LoadChanges() = 0; //loads the database file with the current content of the database vector

   // virtual void add_File(int last_id) = 0; // appends the students/profs/etc. to be added over the database vector

    virtual void Tokenizer() = 0; // tokenizes the vector into a map where the keys are the ID integers.

};

class Students_db : public Database {

private:
    bool failflag_students;
    string txts; //string where the database file is copied
    string txtsa;//string where file to be added is copied
    string temp;
    vector<string> SDBcontent;
    vector<vector<string>> SDBData;
    vector<string> SAcontent;
    vector <string> SUcontent;
    vector<vector<string>> SUData;
    map <int,vector<string>> Sbase;
    vector <int> SID;
    int add_index; // position of the last student before add_files adds the new students in SDBcontent

    friend class Profs_db;

public:

    explicit Students_db(const string &dbst) { //CONSTRUCTOR
        DisarmEditing = true;
        std::fstream sfile;
        sfile.open(dbst, std::ios::out | std::ios::app);
        sfile.close();
        sfile.open(dbst, std::ios::in | std::ios::out | std::ios::app);
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
                if(!tmp.empty()) SDBcontent.push_back(tmp);
            }

            failflag_students = false;

        } else {
            cout << "\nERROR : " << dbst << " failed to open\n";
            failflag_students = true;
        }
        add_index=SDBcontent.size(); // important for SPI::AssignID to work!
    }

    map<int,vector<string>> OutputMap(){
        return Sbase;
    }


    bool did_it_Fail() override {
        return failflag_students;
    }

    vector<string> GiveContent(){
        return SDBcontent;
    }

    void debug_Print_DB(){
        cout<<"\n######## SDBcontent ########\n";
        for(const auto &n : SDBcontent){
            cout<<n<<"\n";
        }
        cout<<"\n############################\n";
    }

    bool SyntaxChecker(const int &f,const string &cmd) override { //return -1 if error found

        int cnt = 0;
        int flag = 0;
        for (const string& i : SDBcontent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^[sS]([0-9]{6})\;([\S ]+)\;([\S ]+)\;(\S+)@(\S+)\.(\S+)$)");

            if (!regex_match(i, regs)) {
                if(f==0){
                    cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the db_studenti.txt file\n";
                    flag = 1;
                }else if(f==1){
                    cout << "\nERROR: Adding the file " << cmd << " introduces errors in the database system\n";
                    return false;
                }
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            DisarmEditing = false;
            return true;
        }
    }

    bool a_load_File(const string &name) override { // returns false if failed

        std::fstream ssfile;
        ssfile.open(name, std::ios::in | std::ios::out);
        if (ssfile.is_open()) {

            while (ssfile.good()) {
                temp = "";
                getline(ssfile, temp);
                temp += "\n";
                txtsa += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtsa);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) SAcontent.push_back(tmp);
            }

            return true;

        } else {
            cout << "\nERROR : " << name << " failed to open\n";
            return false;
        }

    }

    bool u_load_File(const string &n) override{

        std::fstream ssfile;
        ssfile.open(n, std::ios::in | std::ios::out);
        if (ssfile.is_open()) {

            while (ssfile.good()) {
                temp = "";
                getline(ssfile, temp);
                temp += "\n";
                txtsa += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtsa);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) SUcontent.push_back(tmp);
            }

            return true;

        } else {
            cout << "\nERROR : " << n << " failed to open\n";
            return false;
        }
    }

    bool a_SyntaxChecker() override {

        int cnt = 0;
        int flag = 0;
        for (const string& i : SAcontent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^([\S ]+)\;([\S ]+)\;(\S+)@(\S+)\.(\S+)$)");

            if (!regex_match(i, regs)) {
                cout << "\nERROR: Line number " << cnt
                     << " does not follow the standard format of the students add file\n";
                flag = 1;
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            return true;
        }
    }

    bool u_SyntaxChecker() override{

        int cnt = 0;
        int flag = 0;
        for (const string& i : SUcontent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(
                    R"(^[sS][0-9]{6};([\S ]+)?;(([\S ]+)?)?;((\S+)@(\S+)\.(\S+)?)?$)");

            if (!regex_match(i, regs)) {
                cout << "\nERROR: Line number " << cnt
                     << " does not follow the standard format of the students update file\n";
                flag = 1;
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            return true;
        }
    }

    bool LoadChanges() override{
        if (!DisarmEditing) {
            std::fstream ssfile;
            ssfile.open("db_studenti.txt",std::ios::out);
            if (ssfile.is_open()) {
                for (const string& n : SDBcontent) {
                    ssfile << n << "\n";
                }

                return true;


            } else {
                cout << "\nERROR 1: Could not edit db_studenti.txt\n";
                DisarmEditing = true;
                return false;
            }

        }else{
            cout << "\nERROR 2: Could not edit db_studenti.txt\n";
            DisarmEditing = true;
            return false;
        }

    }

    void add_File(int last_id) {
       /* int last_id = 0;
        if(!SDBcontent.empty()){
            last_id = stoi(SDBcontent.back().substr(1,6));
           // cout<<"\nLAST STUDENT ID: "<<SDBcontent.back().substr(1,6)<<"\n";////////////
        }
        */
        int cnt = 1;
        string temp="";
        add_index = SDBcontent.size();
        for(string n : SAcontent){
            temp = "s"+IntToID(last_id+cnt)+";";
            cnt++;
            //n = temp + n;
            n = temp.append(n);
            SDBcontent.push_back(n);
        }


    }

    void Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(string n : SDBcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            SDBData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }

        /*  for(auto k : SDBData){
              for (string n : k) {
                  cout << n  << ' ';
              }
              cout<<"\n";
          }*/


    }

    void u_Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(string n : SUcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            SUData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }
    }

    void Mapper() override{
        int tempkey;
        vector<string> tempo;
        temp ="";
        if(!SDBData.empty()){
            for(auto n : SDBData){
                tempkey = 100000*(n[0][1]-48) + 10000*(n[0][2]-48) + 1000*(n[0][3]-48) + 100*(n[0][4]-48) + 10*(n[0][5]-48) + (n[0][6]-48); //converts matricola to integer
                tempo.push_back(n[1]);
                tempo.push_back(n[2]);
                tempo.push_back(n[3]);

                Sbase.insert(pair<int,vector<string>>(tempkey,tempo));
                tempo.clear();
            }
        }

    }

    bool Update() override{
        map<int,vector<string>>::iterator it;
        string temp1;
        string temp2;
        string temp3;
        int key;
        for(auto n : SUData){
            key = 100000*(n[0][1]-48) + 10000*(n[0][2]-48) + 1000*(n[0][3]-48) + 100*(n[0][4]-48) + 10*(n[0][5]-48) + (n[0][6]-48);
            it = Sbase.find(key);
            if(it != Sbase.end()){
                temp1 = n[1]; // Name
                temp2 = n[2]; // Surname
                temp3 = n[3]; // Email
                temp1.erase(remove(temp1.begin(), temp1.end(), ' '), temp1.end());
                temp2.erase(remove(temp2.begin(), temp2.end(), ' '), temp2.end());
                temp3.erase(remove(temp3.begin(), temp3.end(), ' '), temp3.end());
                if(!temp1.empty()){
                    (it->second)[0] = temp1;
                }
                if(!temp2.empty()){
                    (it->second)[1] = temp2;
                }
                if(!temp3.empty()){
                    (it->second)[2] = temp3;
                }
            }else{
                return false;
            }

        }
        return true;
        ////debug
        /*   for(it=Sbase.begin();it!=Sbase.end();++it){
               for(auto i : it->second){
                   cout << i << ";";
               }
               cout<<"\n";
           }*/
    }

    void Rebuild() override{
        temp="";
        stringstream ss;
        map<int,vector<string>>::iterator it;
        SDBcontent.clear();
        for(it=Sbase.begin();it!=Sbase.end();++it){
            ss << (it->first);
            temp+="s";
            switch((ss.str()).size()){
                case 1:
                    temp+="00000";
                    temp+=ss.str();
                    break;
                case 2:
                    temp+="0000";
                    temp+=ss.str();
                    break;
                case 3:
                    temp+="000";
                    temp+=ss.str();
                    break;
                case 4:
                    temp+="00";
                    temp+=ss.str();
                    break;
                case 5:
                    temp+="0";
                    temp+=ss.str();
                    break;
                case 6:
                    temp+=ss.str();
                    break;
                default:
                    break;

            }
            temp+= ";";
            temp += (it->second)[0];
            temp+= ";";
            temp += (it->second)[1];
            temp+= ";";
            temp += (it->second)[2];
            SDBcontent.push_back(temp);
            ss.str("");
            temp="";
        }
/*cout<<"\n\n";
        //debug
        for(auto k : SDBcontent){
            cout << k <<"\n";
        }*/

    }


    bool checkStudentEmails(){ // true if ok, false if problems.
        vector<vector<string>>::iterator ptr;
        vector<vector<string>>::iterator it;

        for(it = SDBData.begin();it != SDBData.end();it++){

            for(ptr = SDBData.begin(); ptr != SDBData.end();ptr++) {
                if(ptr != it) {
                    if ((*it)[3] == (*ptr)[3]) {
                        cout << "\nERROR : Two or more email addresses are identical in the db_studenti.txt file\n         starting from: "<<(*it)[3]<<"\n";
                        return false;
                    }

                }

            }
        }
        return true;
    }

    bool checkStudentEmails_u(){
        map<int,vector<string>>::iterator ptr;
        map<int,vector<string>>::iterator it;

        for(it = Sbase.begin();it != Sbase.end();it++){

            for(ptr = Sbase.begin(); ptr != Sbase.end();ptr++) {
                if(ptr != it) {
                    if ((it->second)[2] == (ptr->second)[2]) {
                        cout << "\nERROR : Two or more email addresses are identical in the db_studenti.txt file\n";
                        return false;
                    }

                }

            }
        }
        return true;
    }



};

class Aule_db : public Database {
private:
    map <int,vector<string>> Abase; //ID's are converted to integers and used as keys mapping to a vector of strings
    string txta;
    string txtaa;
    string temp;
    bool failflag_aule;
    vector <string> ADBcontent;
    vector <string> AAcontent;
    vector<vector<string>> AUData;
    vector <string> AUcontent;
    vector<vector<string>> ADBData; //tokenized form of ADBcontent
    int aindex;
    friend class Exam;
public:

    explicit Aule_db(const string &dbst){ //CONSTRUCTOR
        aindex = 0;
        DisarmEditing = true;
        std::fstream sfile;
        sfile.open(dbst, std::ios::out | std::ios::app);
        sfile.close();
        sfile.open(dbst, std::ios::in | std::ios::out | std::ios::app);
        if(sfile.is_open()){

            while(sfile.good()){
                temp = "";
                getline(sfile,temp);
                temp+="\n";
                txta += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txta);

            while(getline(ss,tmp)){ // translates the file string into a vector of strings
                if(!tmp.empty()) ADBcontent.push_back(tmp);
            }

            failflag_aule = false;

        }else{
            cout << "\nERROR : " << dbst << " failed to open\n";
            failflag_aule = true;
        }
    }

    map<int,vector<string>> OutputMap(){
        return Abase;
    }

    void printContent(){
        for(const string& n : ADBcontent){
            cout << n << "\n";
        }
    }

    bool did_it_Fail() override{
        return failflag_aule;
    }


    void Rebuild() override{
        temp="";
        map<int,vector<string>>::iterator it;
        ADBcontent.clear();
        for(it=Abase.begin();it!=Abase.end();++it){
            temp = IntToAulaID(it->first);
            temp+= ";";
            temp += (it->second)[0];
            temp+= ";";
            temp += (it->second)[1];
            temp+= ";";
            temp += (it->second)[2];
            temp+= ";";
            temp += (it->second)[3];
            ADBcontent.push_back(temp);

            temp="";
        }
/* cout<<"\n\n";
        //debug
        for(auto k : ADBcontent){
            cout << k <<"\n";
        }*/
    }

    bool Update() override{
        map<int,vector<string>>::iterator it;
        string temp1;
        string temp2;
        string temp3;
        string temp4;
        int key;
        for(auto n : AUData){
            //key = 1000*(n[0][0]-48) + 100*(n[0][1]-65) + 10*(n[0][2]-65) + (n[0][3]-48);
            key = AulaIDtoInt(n[0].substr(0,4));
            it = Abase.find(key);
            if(it != Abase.end()){
                temp1 = n[1];
                temp2 = n[2];
                temp3 = n[3];
                temp4 = n[4];
                temp1.erase(remove(temp1.begin(), temp1.end(), ' '), temp1.end());
                temp2.erase(remove(temp2.begin(), temp2.end(), ' '), temp2.end());
                temp3.erase(remove(temp3.begin(), temp3.end(), ' '), temp3.end());
                if(!temp1.empty()){
                    (it->second)[0] = temp1;
                }
                if(!temp2.empty()){
                    (it->second)[1] = temp2;
                }
                if(!temp3.empty()){
                    (it->second)[2] = temp3;
                }
                if(!temp4.empty()){
                    (it->second)[3] = temp4;
                }
            }else{
                return false;
            }

        }
        return true;
        ////debug
        /*   for(it=Sbase.begin();it!=Sbase.end();++it){
               for(auto i : it->second){
                   cout << i << ";";
               }
               cout<<"\n";
           }*/
    }

    void u_Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(const string& n : AUcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            AUData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }
    }

    bool SyntaxChecker(const int &f,const string &cmd) override{ //return true if error found

        int cnt =0;
        int flag =0;
        for(const string& i : ADBcontent){cnt++; //REGEX

            regex regs("^([A-Z][0-9]{3});([AL]);([a-zA-Z0-9 ]+);([0-9]+);([0-9]+)$");

            if(!regex_match(i,regs)){
                if(f==0){
                    cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the db_aule.txt file\n";
                    flag = 1;
                }else if(f==1){
                    cout << "\nERROR: Adding the file " << cmd << " introduces errors in the database system\n";
                    return false;
                }
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            DisarmEditing = false;
            return true;
        }
    }

    bool a_load_File(const string &name) override{

        std::fstream ssfile;
        ssfile.open(name, std::ios::in | std::ios::out);
        if(ssfile.is_open()){

            while(ssfile.good()){
                temp = "";
                getline(ssfile,temp);
                temp+="\n";
                txtaa += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtaa);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) AAcontent.push_back(tmp);
            }

            return true;

        }else{
            cout << "\nERROR : " << name << " failed to open\n";
            return false;
        }

    }

    bool u_load_File(const string &n) override{

        std::fstream ssfile;
        ssfile.open(n, std::ios::in | std::ios::out);
        if (ssfile.is_open()) {

            while (ssfile.good()) {
                temp = "";
                getline(ssfile, temp);
                temp += "\n";
                txtaa += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtaa);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) AUcontent.push_back(tmp);
            }

            return true;

        } else {
            cout << "\nERROR : " << n << " failed to open\n";
            return false;
        }
    }

    bool a_SyntaxChecker() override{

        int cnt =0;
        int flag =0;
        for(const string& i : AAcontent){cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs("^([AL]);([a-zA-Z0-9 ]+);([0-9]+);([0-9]+)$");

            if(!regex_match(i,regs)){
                cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the classrooms to add file\n";
                flag = 1;
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            return true;
        }
    }

    bool u_SyntaxChecker() override{

        int cnt = 0;
        int flag = 0;
        for (const string& i : AUcontent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(
                    "^([A-Z][0-9]{3});((A|L))?;(([\\S ]+))?;([0-9]+)?;([0-9]+)?$");

            if (!regex_match(i, regs)) {
                cout << "\nERROR: Line number " << cnt
                     << " does not follow the standard format of the students update file\n";
                flag = 1;
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            return true;
        }
    }

    bool LoadChanges() override{
        if (!DisarmEditing) {
            std::fstream ssfile;
            ssfile.open("db_aule.txt", std::ios::in | std::ios::out);
            if (ssfile.is_open()) {
                ssfile << "";
                for (const auto& n : ADBcontent) {
                    ssfile << n << "\n";
                }
                DisarmEditing = true;
                return true;

            } else {
                cout << "\nERROR : Could not edit db_aule.txt\n";
                DisarmEditing = true;
                return false;
            }

        }else{
            cout << "\nERROR : Could not edit db_aule.txt\n";
            DisarmEditing = true;
            return false;
        }

    }

    void add_File() {
        //temp="A001";
        int last_id = 0;
        if(!ADBcontent.empty()){
            last_id = AulaIDtoInt(ADBcontent.back().substr(0,4));
        }
        int cnt = 1;
        string temp="";

        for(string n : AAcontent){
            temp = IntToAulaID(last_id+cnt)+";";
            cnt++;
            n = temp.append(n);
            ADBcontent.push_back(n);
        }
    }

    void assignCodes() {
        int tmp;
        int cnt = 0;
        int flg = 0;
        int steag = 0;
        stringstream ss;
        string tmpp;
        temp="";

        for (auto n = (ADBcontent.begin()+aindex);n!=ADBcontent.end();++n) {

            while (flg == 0) {

                ss << cnt;
                temp = ss.str();
                for (auto k = ADBcontent.begin();k != (ADBcontent.end() - (ADBcontent.size() - aindex)); ++k) {
                    if (!k->empty()) {
                        tmpp = (*k).substr(0, 4);
                        tmp = 1000 * (tmpp[0] - 48) + 100 * (tmpp[1] - 65) + 10 * (tmpp[2] - 65) + (tmpp[3] - 48);
                        if (tmp == cnt) {
                            steag = 1;
                        }
                    }
                }
                ss << "";
                if (steag == 0) {
                    int fD;
                    int sD;
                    int tD;
                    int foD;
                    switch (temp.size()) {
                        case 1 :
                            fD = temp[0] - 48;
                            temp = "";
                            temp += "0AA";
                            temp.push_back(fD + 48);
                            (*n) = (*n).replace(0, 4, temp);
                            break;
                        case 2 :
                            sD = temp[0] - 48;
                            fD = temp[1] - 48;
                            temp = "";
                            temp += "0A";
                            temp.push_back(sD + 65);
                            temp.push_back(fD + 48);

                            (*n) = (*n).replace(0, 4, temp);
                            break;
                        case 3 :
                            tD = temp[0] - 48;
                            sD = temp[1] - 48;
                            fD = temp[2] - 48;
                            temp = "";
                            temp += "0";
                            temp.push_back(tD + 65);
                            temp.push_back(sD + 65);
                            temp.push_back(fD + 48);
                            (*n) = (*n).replace(0, 4, temp);
                            break;
                        case 4 :
                            foD = temp[0] - 48;
                            tD = temp[1] - 48;
                            sD = temp[2] - 48;
                            fD = temp[3] - 48;
                            temp = "";

                            temp.push_back(foD + 48);
                            temp.push_back(tD + 65);
                            temp.push_back(sD + 65);
                            temp.push_back(fD + 48);

                            (*n) = (*n).replace(0, 4, temp);
                            break;

                        default :

                            break;


                    }

                    ADBcontent[n-ADBcontent.begin()] = (*n);
                    ss.str("");
                    cnt++;
                    temp = "";
                    flg = 1;
                } else {
                    ss.str("");
                    cnt++;
                    temp = "";
                    steag = 0;
                }
            }
            flg=0;
        }


    }

    bool checkTitles(){
        vector<vector<string>>::iterator ptr;
        vector<vector<string>>::iterator it;
        string comp1;
        string comp2;

        for(it = ADBData.begin();it != ADBData.end();it++){

            for(ptr = ADBData.begin(); ptr != ADBData.end();ptr++) {
                comp1 = "";
                comp2 = "";
                if(ptr != it) {
                    comp1= (*ptr)[2];
                    comp2= (*it)[2];
                    comp1.erase(remove(comp1.begin(), comp1.end(), ' '), comp1.end());
                    comp2.erase(remove(comp1.begin(), comp1.end(), ' '), comp1.end());
                    if (comp1 == comp2) {
                        cout << "\nERROR : Two or more classroom names are identical in the db_aule.txt file\n";
                        return false;
                    }

                }

            }
        }
        return true;
    }

    bool checkCodes(){
        vector<vector<string>>::iterator ptr;
        vector<vector<string>>::iterator it;
        string comp1;
        string comp2;

        for(it = ADBData.begin();it != ADBData.end();it++){

            for(ptr = ADBData.begin(); ptr != ADBData.end();ptr++) {
                comp1 = "";
                comp2 = "";
                if(ptr != it) {
                    comp1= (*ptr)[0];
                    comp2= (*it)[0];
                    if (comp1 == comp2) {
                        cout << "\nERROR : Two or more classroom ID's are identical in the db_aule.txt file (ID:"<<(*ptr)[0]<<")\n";
                        return false;
                    }

                }

            }
        }
        return true;
    }

    bool checkTitles_u(){
        map<int,vector<string>>::iterator ptr;
        map<int,vector<string>>::iterator it;
        string comp1;
        string comp2;

        for(it = Abase.begin();it != Abase.end();it++){

            for(ptr = Abase.begin(); ptr != Abase.end();ptr++) {
                comp1 = "";
                comp2 = "";
                if(ptr != it) {
                    comp1= (ptr->second)[1];
                    comp2= (it->second)[1];
                    comp1.erase(remove(comp1.begin(), comp1.end(), ' '), comp1.end());
                    comp2.erase(remove(comp1.begin(), comp1.end(), ' '), comp1.end());
                    if (comp1 == comp2) {
                        cout << "\nERROR : Two or more classroom names are identical in the db_aule.txt file\n";
                        return false;
                    }

                }

            }
        }
        return true;
    }

    void Mapper() override{
        int tempkey;
        vector<string> tempo;
        temp ="";
        if(!ADBData.empty()){
            for(auto n : ADBData){
                //tempkey = 1000*(n[0][0]-48) + 100*(n[0][1]-65) + 10*(n[0][2]-65) + (n[0][3]-48); //converts matricola to integer
                tempkey = AulaIDtoInt(n[0].substr(0,4));
                tempo.push_back(n[1]);
                tempo.push_back(n[2]);
                tempo.push_back(n[3]);
                tempo.push_back(n[4]);

                Abase.insert(pair<int,vector<string>>(tempkey,tempo));
                tempo.clear();
            }
        }

    }



    void Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        string temp = "";
        ss << "";
        for(string n : ADBcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            ADBData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }
    }


};



class Profs_db : public Database {
private:
    string temp;

    map <int,vector<string>> Pbase;
    bool failflag_profs;
    string txtp; //string where the database file will be copied
    string txtpa;
    vector <string> PDBcontent;
    vector <string> PAcontent;
    vector <string> PUcontent;
    vector<vector<string>> PDBData;
    vector<vector<string>> PUData;
    vector <int> PID;
    int padd_index;

    friend class Exam;
    friend class Students_db;
public:

    void printpID(){
        for(int n : PID){
            cout << n << "\n";
        }
    }

    vector<string> GiveContent(){
        return PDBcontent;
    }

    map<int,vector<string>> OutputMap(){
        return Pbase;
    }

    void print2(){
        for(const string& n : PDBcontent){
            cout << n << "\n";
        }
        cout << "\n----\n";
        for(const string& n : PAcontent){
            cout << n << "\n";
        }
    }

    void printContent(){
        for(const string& n : PDBcontent){
            cout << n << "\n";
        }
    }

    explicit Profs_db(const string &dbst){ //CONSTRUCTOR
        DisarmEditing = true;
        std::fstream sfile;
        sfile.open(dbst, std::ios::out | std::ios::app);
        sfile.close();
        sfile.open(dbst, std::ios::in | std::ios::out | std::ios::app);
        if(sfile.is_open()){

            while(sfile.good()){
                temp = "";
                getline(sfile,temp);
                temp+="\n";
                txtp += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtp);

            while(getline(ss,tmp)){ // translates the file string into a vector of strings
                if(!tmp.empty()) PDBcontent.push_back(tmp);
            }

            failflag_profs = false;

        }else{
            cout << "\nERROR : " << dbst << " failed to open\n";
            failflag_profs = true;
        }
        padd_index=PDBcontent.size(); // important for SPI::AssignID to work!
    }


    bool Update() override{
        map<int,vector<string>>::iterator it;
        string temp1;
        string temp2;
        string temp3;
        int key;
        for(auto n : PUData){
            key = 100000*(n[0][1]-48) + 10000*(n[0][2]-48) + 1000*(n[0][3]-48) + 100*(n[0][4]-48) + 10*(n[0][5]-48) + (n[0][6]-48);
            it = Pbase.find(key);
            if(it != Pbase.end()){
                temp1 = n[1]; // Name
                temp2 = n[2]; // Surname
                temp3 = n[3]; // Email
                temp1.erase(remove(temp1.begin(), temp1.end(), ' '), temp1.end());
                temp2.erase(remove(temp2.begin(), temp2.end(), ' '), temp2.end());
                temp3.erase(remove(temp3.begin(), temp3.end(), ' '), temp3.end());
                if(!temp1.empty()){
                    (it->second)[0] = temp1;
                }
                if(!temp2.empty()){
                    (it->second)[1] = temp2;
                }
                if(!temp3.empty()){
                    (it->second)[2] = temp3;
                }
            }else{
                return false;
            }

        }
        return true;
        ////debug
        /*   for(it=Sbase.begin();it!=Sbase.end();++it){
               for(auto i : it->second){
                   cout << i << ";";
               }
               cout<<"\n";
           }*/
    }

    void Rebuild() override{
        temp="";
        stringstream ss;
        map<int,vector<string>>::iterator it;
        PDBcontent.clear();
        for(it=Pbase.begin();it!=Pbase.end();++it){
            ss << (it->first);
            temp+="d";
            switch((ss.str()).size()){
                case 1:
                    temp+="00000";
                    temp+=ss.str();
                    break;
                case 2:
                    temp+="0000";
                    temp+=ss.str();
                    break;
                case 3:
                    temp+="000";
                    temp+=ss.str();
                    break;
                case 4:
                    temp+="00";
                    temp+=ss.str();
                    break;
                case 5:
                    temp+="0";
                    temp+=ss.str();
                    break;
                case 6:
                    temp+=ss.str();
                    break;
                default:
                    break;

            }
            temp+= ";";
            temp += (it->second)[0];
            temp+= ";";
            temp += (it->second)[1];
            temp+= ";";
            temp += (it->second)[2];
            PDBcontent.push_back(temp);
            ss.str("");
            temp="";
        }
/*cout<<"\n\n";
        //debug
        for(auto k : SDBcontent){
            cout << k <<"\n";
        }*/
    }

    void u_Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(const string& n : PUcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            PUData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }
    }

    bool did_it_Fail() override{
        return failflag_profs;
    }

    bool SyntaxChecker(const int &f,const string &cmd) override{ //return -1 if error found

        int cnt =0;
        int flag =0;
        for(const string& i : PDBcontent){cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^[dD]([0-9]{6})\;([\S ]+)\;([\S ]+)\;(\S+)@(\S+)\.(\S+)$)");

            if(!regex_match(i,regs)){
                if(f==0){
                    cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the db_professori.txt file\n";
                    flag = 1;
                }else if(f==1){
                    cout << "\nERROR: Adding the file " << cmd << " introduces errors in the database system\n";
                    return false;
                }
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            DisarmEditing = false;
            return true;
        }
    }

    bool a_load_File(const string &name) override{

        std::fstream ssfile;
        ssfile.open(name, std::ios::in | std::ios::out);
        if(ssfile.is_open()){

            while(ssfile.good()){
                temp = "";
                getline(ssfile,temp);
                temp+="\n";
                txtpa += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtpa);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) PAcontent.push_back(tmp);
            }

            return true;

        }else{
            cout << "\nERROR : " << name << " failed to open\n";
            return false;
        }

    }

    bool u_load_File(const string &n) override{

        std::fstream ssfile;
        ssfile.open(n, std::ios::in | std::ios::out);
        if (ssfile.is_open()) {

            while (ssfile.good()) {
                temp = "";
                getline(ssfile, temp);
                temp += "\n";
                txtpa += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtpa);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) PUcontent.push_back(tmp);
            }

            return true;

        } else {
            cout << "\nERROR : " << n << " failed to open\n";
            return false;
        }
    }

    bool a_SyntaxChecker() override{

        int cnt =0;
        int flag =0;
        for(const string& i : PAcontent){cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^([\S ]+)\;([\S ]+)\;(\S+)@(\S+)\.(\S+)$)");

            if(!regex_match(i,regs)){
                cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the professors add file\n";
                flag = 1;
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            return true;
        }
    }

    bool u_SyntaxChecker() override{

        int cnt = 0;
        int flag = 0;
        for (const string& i : PUcontent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(
                    R"(^[dD][0-9]{6};([\S ]+)?;(([\S ]+)?)?;((\S+)@(\S+)\.(\S+)?)?$)");

            if (!regex_match(i, regs)) {
                cout << "\nERROR: Line number " << cnt
                     << " does not follow the standard format of the students update file\n";
                flag = 1;
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            return true;
        }
    }

    bool LoadChanges() override{
        if (!DisarmEditing) {
            std::fstream ssfile;
            ssfile.open("db_professori.txt",std::ios::out);
            if (ssfile.is_open()) {
                ssfile << "";
                for (const auto& n : PDBcontent) {
                    ssfile << n << "\n";
                }
                DisarmEditing = true;
                return true;

            } else {
                cout << "\nERROR : Could not edit db_professori.txt\n";
                DisarmEditing = true;
                return false;
            }

        }else{
            cout << "\nERROR : Could not edit db_professori.txt\n";
            DisarmEditing = true;
            return false;
        }

    }

    void add_File(int last_id) {
        /*int last_id = 0;
        if(!PDBcontent.empty()){
             last_id = stoi(PDBcontent.back().substr(1,6));
            //cout<<"\nLAST PROFESSOR ID: "<<PDBcontent.back().substr(1,6)<<"\n";////////////
        }
      */
        int cnt = 1;
        string temp="";
        padd_index = PDBcontent.size();
        for(string n : PAcontent){
            temp = "d"+IntToID(last_id+cnt)+";";
            cnt++;
            //n = temp + n;
            n = temp.append(n);
            PDBcontent.push_back(n);
        }



    }

    void Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(string n : PDBcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            PDBData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }

        /*  for(auto k : SDBData){
              for (string n : k) {
                  cout << n  << ' ';
              }
              cout<<"\n";
          }*/

    }

    bool checkProfEmails(){
        vector<vector<string>>::iterator ptr;
        vector<vector<string>>::iterator it;

        for(it = PDBData.begin();it != PDBData.end();it++){

            for(ptr = PDBData.begin(); ptr != PDBData.end();ptr++) {
                if(ptr != it) {
                    if ((*it)[3] == (*ptr)[3]) {
                        int line_email1 = distance(PDBData.begin(),it);
                        int line_email2 = distance(PDBData.begin(),ptr);

                        cout << "\nERROR : Two or more email addresses are identical in the db_professori.txt file (line "<<line_email1<<" and line "<<line_email2<<")"<<endl;
                        return false;
                    }

                }

            }
        }
        return true;
    }

    bool checkProfEmails_u(){
        map<int,vector<string>>::iterator ptr;
        map<int,vector<string>>::iterator it;

        for(it = Pbase.begin();it != Pbase.end();it++){

            for(ptr = Pbase.begin(); ptr != Pbase.end();ptr++) {
                if(ptr != it) {
                    if ((it->second)[2] == (ptr->second)[2]) {
                        int line_email1 = distance(Pbase.begin(),it);
                        int line_email2 = distance(Pbase.begin(),ptr);

                        cout << "\nERROR : Two or more email addresses are identical in the db_professori.txt file (line "<<line_email1<<" and line "<<line_email2<<")"<<endl;
                        return false;
                    }

                }

            }
        }
        return true;
    }

    void Mapper() override{
        int tempkey;
        vector<string> tempo;
        temp ="";
        if(!PDBData.empty()){
            for(auto n : PDBData){
                tempkey = 100000*(n[0][1]-48) + 10000*(n[0][2]-48) + 1000*(n[0][3]-48) + 100*(n[0][4]-48) + 10*(n[0][5]-48) + (n[0][6]-48); //converts matricola to integer
                tempo.push_back(n[1]);
                tempo.push_back(n[2]);
                tempo.push_back(n[3]);

                Pbase.insert(pair<int,vector<string>>(tempkey,tempo));
                tempo.clear();
            }
        }

    }

};



class Courses_db : public Database{

private:
    string temp;
    string txtc;
    string txtca;
    bool failflag_courses;
    map <int,vector<string>> Cbase;
    vector <string> CDBcontent;
    vector <string> CAcontent;
    vector <string> CUcontent;
    vector<vector<string>> CDBData;
    vector<vector<string>> CAData;//tokenized add file
    vector<vector<string>> CUData;
    int cindex;
    //friend class Exam;
public:

    explicit Courses_db(const string &dbst){ //CONSTRUCTOR
        cindex = 0;
        DisarmEditing = true;
        std::fstream sfile;
        sfile.open(dbst, std::ios::out | std::ios::app);
        sfile.close();
        sfile.open(dbst, std::ios::in | std::ios::out | std::ios::app);
        if(sfile.is_open()){

            while(sfile.good()){
                temp = "";
                getline(sfile,temp);
                temp+="\n";
                txtc += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtc);

            while(getline(ss,tmp)){ // translates the file string into a vector of strings
                if(!tmp.empty()) CDBcontent.push_back(tmp);
            }

            failflag_courses = false;

        }else{
            cout << "\nERROR : " << dbst << " failed to open\n";
            failflag_courses = true;
        }
    }

    map<int,vector<string>> OutputMap(){
        return Cbase;
    }

    void printCBase(){
        cout<<"\n=================COURSE DATABASE=================\n";
        for(const auto& i : Cbase){
            printVectorString(i.second);
        }
        cout<<"\n==================================================\n";
    }

    void printCourseFile(){
        printVectorString(CDBcontent);
    }

    void Mapper() override{
        int tempkey;
        vector<string> tempo;
        temp ="";
        if(!CDBData.empty()){
            for(auto n : CDBData){
                if(n[0][0]=='c'){
                    if(!tempo.empty()){
                        Cbase.insert(pair<int,vector<string>>(tempkey,tempo));
                    }
                    tempo.clear();
                    //tempkey = 100000*(n[1][0]-65) + 10000*(n[1][1]-65) + 1000*(n[1][2]-65) + 100*(n[1][3]-48) + 10*(n[1][4]-48) + (n[1][5]-48); //converts course ID to integer key
                    tempkey = CourseIDtoInt(n[1].substr(0,7));

                    tempo.push_back(n[2]);
                    tempo.push_back(n[3]);
                    tempo.push_back(n[4]);
                    tempo.push_back(n[5]);
                    tempo.push_back(n[6]);
                    // tempo.push_back("|");
                }else if(n[0][0]=='a'){
                    tempo.push_back(n[1]);
                    tempo.push_back(n[2]);
                    tempo.push_back(n[3]);
                    tempo.push_back(n[4]);
                    tempo.push_back(n[5]);
                    if(n.size()==7){
                        tempo.push_back(n[6]);
                    }
                    // tempo.push_back("|");
                }

            }
            Cbase.insert(pair<int,vector<string>>(tempkey,tempo));
        }


/*
         for(const auto& k : Cbase){
    cout<<k.first<<"\n";
         for(const auto& n : k.second){

         cout << n << ";";

         }
         cout<<"\n";
         }

*/



    }

    void u_Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(const string& n : CUcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            CUData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }
        //for(const auto& n : CUData[1])
        // cout<<n;///////////////

    }

    void Rebuild() override{
        temp="";
        map<int,vector<string>>::iterator it;
        vector<string>::iterator ptr;
        CDBcontent.clear();
        for(it=Cbase.begin();it!=Cbase.end();++it){

            temp = IntToCourseID(it->first);

            temp = "c;"+temp;
            for(int i=0;i<5;i++){
                temp+= ";";
                temp += (it->second)[i];
            }
            CDBcontent.push_back(temp);
            temp = "";
            if((it->second).size()>5){
                ptr = (it->second).begin() + 5;
                for(int j = 1;j<(((it->second).size()-5)/6)+1;j++){
                    temp+="a";
                    for(int k = 0;k<6;k++){
                        temp+=";";
                        temp+= (*ptr);
                        ptr++;
                    }
                    CDBcontent.push_back(temp);
                    temp = "";
                }
            }

            temp="";
        }

        ////
        /*  for(const auto& n : CDBcontent){
              cout<<n<<"\n";
          }  */

    }

    bool Update() override{
        map<int,vector<string>>::iterator it;
        vector<string>::iterator tp;
        vector<string>::iterator ptr;
        int flag =0;
        int key;
        bool empty_flag = false;
        string year;
        int location = 5;
        for(auto n : CUData){
            vector<string> tmp;//contains updater vector
            vector<string> course_instance;
            //key = 100000*(n[0][0]-65) + 10000*(n[0][1]-65) + 1000*(n[0][2]-65) + 100*(n[0][3]-48) + 10*(n[0][4]-48) + (n[0][5]-48);
            key = CourseIDtoInt(n[0].substr(0,7));

            it = Cbase.find(key);
            if(it != Cbase.end()){
                //vector<string> tmp((it->second).begin()+location,(it->second).begin()+location+6);
                tmp.push_back(n[2]);
                tmp.push_back(n[3]);
                tmp.push_back(n[4]);
                tmp.push_back(n[5]);
                tmp.push_back(n[6]);
                year = n[1];

                tmp[0].erase(remove(tmp[0].begin(), tmp[0].end(), ' '), tmp[0].end());
                tmp[1].erase(remove(tmp[1].begin(), tmp[1].end(), ' '), tmp[1].end());
                tmp[2].erase(remove(tmp[2].begin(), tmp[2].end(), ' '), tmp[2].end());
                tmp[3].erase(remove(tmp[3].begin(), tmp[3].end(), ' '), tmp[3].end());
                tmp[4].erase(remove(tmp[4].begin(), tmp[4].end(), ' '), tmp[4].end());

                //check if any entry is empty in course update request
                for(int i=0;i<5;i++){
                    if(tmp[i].empty()){
                        empty_flag = true;
                        break;
                    }
                }

                //
                    //string F= "";///////
                   for(tp=(it->second).begin();tp != (it->second).end();++tp){
                       if((*tp)==n[1]){
                           ptr = tp;
                          //F = *tp;////////////
                           location = ptr - (it->second).begin();
                           flag=1;
                       }
                   }
                    if(flag==1){//found academic year in course *key
                        //cout<<F<<"\n"<<location<<"\n";
                        //cout<<(it->second)[location]<<"\n";
                        //create subvector of current  year info from Cbase
                                vector<string> course_instance((it->second).begin()+location,(it->second).begin()+location+5);
                       // printVectorString(course_instance);/////////////////

                       for(int i = 1;i<5;i++){
                           if(!tmp[i-1].empty()){
                               //modifies Cbase where there is a nonempty entry in the new courses file
                               (it->second)[location+i] = tmp[i-1];
                           }
                       }
                        temp = tmp[4].substr(1,tmp[4].length()-2); //removes {  and  }
                       if(!temp.empty()){
                           (it->second)[location+5] = tmp[4];
                       }


                    }else{

                        if(!empty_flag){
                            (it->second).push_back(year);
                            for(int i = 0;i<5;i++){
                                (it->second).push_back(tmp[i]);
                            }
                        }else{
                            cout<<"\nERROR: The new instance of course "<<IntToCourseID(key)<<" for the year "<<year<<" cannot contain empty entries as there is no present course to inherit them from.\n";
                            return false;
                        }

                        /*
                        for(int i=1;i<(((it->second).size()-5)/6)+1;i++){
                            if((it->second)[location]==n[1]){
                                for(int j =1;j<6;j++){
                                    if(!tmp[j-1].empty()){
                                        (it->second)[location+j] = n[j+1];

                                    }
                                }
                            }else{
                                location += 6;
                            }
                        }
                        location =5;



                             */
                    }


            }else{
                cout<<"\nERROR: Could not find course ID "<<IntToCourseID(key)<<" inside the database. New courses insertion cannot proceed.\n";
                return false;
            }
            tmp.clear();
            empty_flag = false;
        }

        ////////
        /* for(const auto& n : Cbase){
             for(const auto& k : n.second){
                 cout<<k<<" ; ";
             }
             cout<<"\n";
         }

 */





        return true;

    }

    bool did_it_Fail() override{
        return  failflag_courses;
    }

    bool SyntaxChecker(const int &f,const string &cmd) override{ //return true if error found

        int cnt =0;
        int flag =0;
        for(const string& i : CDBcontent){cnt++; //REGEX
            regex regc(R"(^c\;([A-Z0-9]{7})\;([\S ]+)\;([0-9]+)\;([0-9]+)\;([0-9]+)\;([0-9]+)$)");
// ^ REGEX for the c;  type
            regex rega(R"(^a\;([0-9]{4}\-[0-9]{4})\;(attivo|non-attivo)\;([0-9]+)\;\[\{[dD]([0-9]{6})\,([A-Z][0-9]{3})\,\[\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\}((\,\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\})+)?\]\}((\,\{[dD]([0-9]{6})\,([A-Z][0-9]{3})\,\[\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\}((\,\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\})+)?\]\})+)?\]\;\{([0-9]+)\,([0-9]+)\,([0-9]+)\,(SO|P||S|O)\,(A|L)(\,([0-9]+))?\}\;\{(([ ]+)?)([A-Z0-9 ]{7}((\,(([ ]+)?)([A-Z0-9]{7}))+)?)?\}$)");
            // ^ REGEX for the a; type
            if(!(regex_match(i,regc)||regex_match(i,rega))){
                if(f==0){
                    cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the db_corsi.txt file\n";
                    //cout<<i<<endl;//
                    flag = 1;
                }else if(f==1){
                    cout << "\nERROR: Adding the file " << cmd << " introduces errors in the database system\n";
                    return false;
                }
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            return true;
            DisarmEditing = false;
        }
    }

    bool a_load_File(const string &name) override{


        std::fstream ssfile;
        ssfile.open(name, std::ios::in | std::ios::out);
        if(ssfile.is_open()){

            while(ssfile.good()){
                temp = "";
                getline(ssfile,temp);
                temp+="\n";
                txtca += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtca);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) CAcontent.push_back(tmp);
            }

            return true;

        }else{
            cout << "\nERROR : " << name << " failed to open\n";
            return false;
        }

    }

    bool u_load_File(const string &n) override{

        std::fstream ssfile;
        ssfile.open(n, std::ios::in | std::ios::out);
        if (ssfile.is_open()) {

            while (ssfile.good()) {
                temp = "";
                getline(ssfile, temp);
                temp += "\n";
                txtca += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtca);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) CUcontent.push_back(tmp);
            }

            return true;

        } else {
            cout << "\nERROR : " << n << " failed to open\n";
            return false;
        }
    }

    bool a_SyntaxChecker() override{

        int cnt =0;
        int flag =0;
        for(const string& i : CAcontent){cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^([0-9]{4}\-[0-9]{4})\;([\S ]+)\;([0-9]+)\;([0-9]+)\;([0-9]+)\;([0-9]+);([0-9]+)\;\[\{[dD]([0-9]{6})\,([A-Z][0-9]{3})\,\[\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\}((\,\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\})+)?\]\}((\,\{[dD]([0-9]{6})\,([A-Z][0-9]{3})\,\[\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\}((\,\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\})+)?\]\})+)?\]\;\{([0-9]+)\,([0-9]+)\,([0-9]+)\,(SO|P||S|O)\,(A|L)(\,([0-9]+))?\}\;\{(([ ]+)?)([A-Z0-9 ]{7}((\,(([ ]+)?)([A-Z0-9]{7}))+)?)?\}$)");

            if(!regex_match(i,regs)){
                cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the courses add file\n";
                flag = 1;
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            return true;
        }
    }

    bool u_SyntaxChecker() override{

        int cnt = 0;
        int flag = 0;
        for (const string& i : CUcontent) {
            cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^[A-Z0-9]{7}\;([0-9]{4}\-[0-9]{4})\;(attivo|non-attivo)?\;([0-9]+)?\;(\[\{[dD]([0-9]{6})\,([A-Z][0-9]{3})\,\[\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\}((\,\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\})+)?\]\}((\,\{[dD]([0-9]{6})\,([A-Z][0-9]{3})\,\[\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\}((\,\{[dD]([0-9]{6})\,([0-9]+)(\,([0-9]+))?(\,([0-9]+))?\})+)?\]\})+)?\])?\;(\{([0-9]+)\,([0-9]+)\,([0-9]+)\,(SO|P||S|O)\,(A|L)(\,([0-9]+))?\})?\;\{(([A-Z0-9]{7}((\,([A-Z0-9]{7}))+)?)?)?\}$)");

            if (!regex_match(i, regs)) {
                cout << "\nERROR: Line number " << cnt
                     << " does not follow the standard format of the courses insert file\n";
                flag = 1;
            }

        }

        if (flag == 1) {
            return false; // ERROR FOUND
        } else {
            return true;
        }
    }

    bool LoadChanges() override{
        std::fstream ssfile;
        ssfile.open("db_corsi.txt", std::ios::in | std::ios::out);
        if (ssfile.is_open()) {
            ssfile << "";
            for (const auto& n : CDBcontent) {
                ssfile << n << "\n";
            }
            DisarmEditing = true;
            return true;

        } else {
            cout << "\nERROR : Could not edit db_corsi.txt\n";
            DisarmEditing = true;
            return false;
        }


    }

    void add_File() { // call this function only after proper syntax tests!
        int last_id = 0;
        int cnt;
        if(!CDBcontent.empty()){
            auto it = Cbase.end();
            it--;
            last_id = (it->first);
            cnt = 1;
            //cout<<"\nLAST PROFESSOR ID: "<<PDBcontent.back().substr(1,6)<<"\n";////////////
        }else{
            cnt = 0;
        }
        temp="";
        vector<string>::iterator ptr;
        cindex = CDBcontent.size();
        for(auto n : CAData){
            temp.append("c;");
            temp.append(IntToCourseID(last_id+cnt));
            temp.append(";");
            for(int i = 1;i<5;i++){
                temp.append(n[i]);
                temp.append(";");
            }
            temp.append(n[5]);
            CDBcontent.push_back(temp);
            temp = "";
            temp.append("a;");
            temp.append(n[0]);
            temp.append(";attivo;");

            for(ptr = n.begin();ptr != n.end();ptr++){
                if((ptr - n.begin())>5){
                    if((n.end()-ptr)==1){

                        temp += *ptr;

                    }else{temp += *ptr;
                        temp += ";";}

                }
            }
            CDBcontent.push_back(temp);
            temp = "";
            cnt++;
        }




    }

    void Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(const string& n : CDBcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            CDBData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }

    }

    void a_Tokenizer(){

        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(const string& n : CAcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            CAData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }

    }

    void assignCCodes(){
        int cnt = 0;
        int count = 0;
        string tmpp;
        int tmp;
        int steag =0;
        int flg = 0;
        stringstream ss;
        temp="";

        for (auto n = (CDBcontent.begin()+cindex);n!=CDBcontent.end();++n) {

            while(flg == 0){
                ss << count;
                temp = ss.str();


                for (auto k = CDBcontent.begin();k != CDBcontent.end() - (CDBcontent.size() - cindex); ++k) {

                    if ((!(*k).empty())&&((*k)[0] == 'c')&&((*n)[0] == 'c')) {
                        tmpp = (*k).substr(2, 6);

                        tmp = 100000 * (tmpp[0] - 65) + 10000 * (tmpp[1] - 65) + 1000 * (tmpp[2] - 65) + 100*(tmpp[3] - 48)+ 10*(tmpp[4] - 48)+ (tmpp[5] - 48);

                        if (tmp == count) {
                            steag = 1;
                        }
                    }
                }


                ss <<"";
                int indx[6] = {0,0,0,0,0,0};
                if(steag == 0){
                    if((*n)[0] == 'c'){

                        switch(temp.size()){
                            case 1 :
                                indx[0] = temp[0]-48;
                                temp = "";
                                temp += "AAA00";
                                temp.push_back(indx[0]+48);
                                (*n) = (*n).replace(2,6,temp);
                                break;
                            case 2 :
                                indx[1] = temp[0] -48;
                                indx[0] = temp[1] -48;
                                temp = "";
                                temp += "AAA0";
                                temp.push_back(indx[1]+48);
                                temp.push_back(indx[0]+48);

                                (*n) = (*n).replace(2,6,temp);
                                break;
                            case 3 :
                                indx[2] = temp[0] -48;
                                indx[1] = temp[1] -48;
                                indx[0] = temp[2] -48;
                                temp = "";
                                temp+="AAA";
                                temp.push_back(indx[2]+48);
                                temp.push_back(indx[1]+48);
                                temp.push_back(indx[0]+48);
                                (*n) = (*n).replace(2,6,temp);
                                break;
                            case 4 :
                                indx[3] = temp[0] -48;
                                indx[2]= temp[1] -48;
                                indx[1] = temp[2] -48;
                                indx[0] = temp[3] -48;
                                temp = "";
                                temp += "AA";
                                temp.push_back(indx[3]+65);
                                temp.push_back(indx[2]+48);
                                temp.push_back(indx[1]+48);
                                temp.push_back(indx[0]+48);

                                (*n) = (*n).replace(2,6,temp);
                                break;

                            case 5 :
                                indx[4] = temp[0] -48;
                                indx[3] = temp[1] -48;
                                indx[2]= temp[2] -48;
                                indx[1] = temp[3] -48;
                                indx[0] = temp[4] -48;
                                temp = "";
                                temp += "A";
                                temp.push_back(indx[4]+65);
                                temp.push_back(indx[3]+65);
                                temp.push_back(indx[2]+48);
                                temp.push_back(indx[1]+48);
                                temp.push_back(indx[0]+48);

                                (*n) = (*n).replace(2,6,temp);
                                break;

                            case 6:
                                indx[5] = temp[0] -48;
                                indx[4] = temp[1] -48;
                                indx[3] = temp[2] -48;
                                indx[2]= temp[3] -48;
                                indx[1] = temp[4] -48;
                                indx[0] = temp[5] -48;
                                temp = "";
                                temp.push_back(indx[5]+65);
                                temp.push_back(indx[4]+65);
                                temp.push_back(indx[3]+65);
                                temp.push_back(indx[2]+48);
                                temp.push_back(indx[1]+48);
                                temp.push_back(indx[0]+48);

                                (*n) = (*n).replace(2,6,temp);

                                break;
                            default :

                                break;


                        }
                        // CDBcontent[n-CDBcontent.begin()] = (*n);
                        ss.str("");
                        count++;
                        cnt++;
                        flg = 1;
                        temp = "";
                    }else{
                        flg =1;
                        cnt++;
                        ss.str("");
                        temp = "";
                    }
                }else{
                    cnt++;
                    count++;
                    ss.str("");
                    temp = "";
                    steag = 0;
                }
            }
            flg = 0;


        }

    }

    bool checkErrors(){
        vector<vector<string>>::iterator ptr;
        vector<vector<string>>::iterator it;
        string comp1;
        string comp2;

        for(it = CDBData.begin();it != CDBData.end();it++){

            for(ptr = CDBData.begin(); ptr != CDBData.end();ptr++) {
                comp1 = "";
                comp2 = "";
                if(ptr != it) {
                    if((((*ptr)[0])=="c") && (((*it)[0])=="c")){
                        comp1= (*ptr)[2];
                        comp2= (*it)[2];
                        comp1.erase(remove(comp1.begin(), comp1.end(), ' '), comp1.end());
                        comp2.erase(remove(comp2.begin(), comp2.end(), ' '), comp2.end());
                        if (comp1 == comp2) {
                            cout << "\nERROR : Two or more course names are identical in the db_courses.txt file\n";
                            return false;
                        }
                    }

                }

            }
        }
        return true;
    }

    bool checkErrors_u() {
        map<int,vector<string>>::iterator ptr;
        map<int,vector<string>>::iterator it;
        string comp1;
        string comp2;

        for(it = Cbase.begin();it != Cbase.end();it++){

            for(ptr = Cbase.begin(); ptr != Cbase.end();ptr++) {
                comp1 = "";
                comp2 = "";
                if(ptr != it) {
                    if((((ptr->second)[0])=="c") && (((it->second)[0])=="c")){
                        comp1= (ptr->second)[2];
                        comp2= (it->second)[2];
                        comp1.erase(remove(comp1.begin(), comp1.end(), ' '), comp1.end());
                        comp2.erase(remove(comp2.begin(), comp2.end(), ' '), comp2.end());
                        if (comp1 == comp2) {
                            cout << "\nERROR : Two or more course names are identical in the db_courses.txt file\n";
                            return false;
                        }
                    }

                }

            }
        }
        return true;
    }

};



class Majors_db : public Database{

private:
    string txtm;
    string txtma;
    map<int,vector<string>> Mbase;
    string temp;
    bool failflag_majors;
    unordered_map <int,vector<string>> mbase;
    vector <string> MDBcontent;
    vector <string> MAcontent;
    vector <vector<string>> MDBData;
    friend class Exam;
public:


    explicit Majors_db(const string &dbst){ //CONSTRUCTOR
        DisarmEditing = true;
        std::fstream sfile;
        sfile.open(dbst, std::ios::out | std::ios::app);
        sfile.close();
        sfile.open(dbst, std::ios::in | std::ios::out | std::ios::app);
        if(sfile.is_open()){

            while(sfile.good()){
                temp = "";
                getline(sfile,temp);
                temp+="\n";
                txtm += temp;
            }

            sfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtm);

            while(getline(ss,tmp)){ // translates the file string into a vector of strings
                if(!tmp.empty())  MDBcontent.push_back(tmp);
            }

            failflag_majors = false;

        }else{
            cout << "\nERROR : " << dbst << " failed to open\n";
            failflag_majors = true;
        }
    }

    map<int,vector<string>> OutputMap(){
        return Mbase;
    }

    void Mapper() override{
        int tempkey;
        vector<string> tempm;
        temp ="";
        if(!MDBData.empty()){
            for(auto n : MDBData){
                //tempkey = 1000*(n[0][0]-48) + 100*(n[0][1]-65) + 10*(n[0][2]-65) + (n[0][3]-48); //converts matricola to integer
                tempkey = AulaIDtoInt(n[0]);
                tempm.push_back(n[1]);
                tempm.push_back(n[2]);
                tempm.push_back(n[3]);

                Mbase.insert(pair<int,vector<string>>(tempkey,tempm));
                tempm.clear();
            }
        }

    }

    bool Update() override{
        return false;
    }

    void printMData(){
        for(const auto& x: MDBData){
            cout<<"\n_________________\n";
            printVectorString(x);
            cout<<"\n_________________\n";
        }
    }

    void Rebuild() override{

    }

    void u_Tokenizer() override{


    }

    bool did_it_Fail() override{
        return  failflag_majors;
    }

    bool SyntaxChecker(const int &f,const string &cmd) override{ //return -1 if error found /// if f = 0 give database error , if f=1 give add error
        int cnt =0;
        int flag =0;
        for(const string& i : MDBcontent){cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^([A-Z][0-9]{3})\;(BS|MS)\;\[\{([ ]+)?([A-Z0-9]{7}([ ]+)?)((\,([ ]+)?[A-Z0-9]{7})+)?([ ]+)?\}((\,\{([ ]+)?([A-Z0-9]{7})((([ ]+)?\,([ ]+)?[A-Z0-9]{7})+([ ]+)?)?\})+)?\](\;\[((([ ]+)?[A-Z0-9]{7})((\,[A-Z0-9]{7})+)?([ ]+)?)?\])$)");

            if(!regex_match(i,regs)){
                if(f==0){
                    cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the db_corsi_studio.txt file\n";
                    cout<<i<<endl;//
                    flag = 1;
                }else if(f==1){
                    cout << "\nERROR: Adding the file " << cmd << " introduces errors in the database system\n";
                    return false;
                }
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            return true;
            DisarmEditing = false;
        }
    }

    bool a_load_File(const string &name) override{

        std::fstream ssfile;
        ssfile.open(name, std::ios::in | std::ios::out);
        if(ssfile.is_open()){

            while(ssfile.good()){
                temp = "";
                getline(ssfile,temp);
                temp+="\n";
                txtma += temp;
            }

            ssfile.close();

            string tmp;
            stringstream ss;
            ss.str(txtma);

            while (getline(ss, tmp)) { // translates the file string into a vector of strings
                if(!tmp.empty()) MAcontent.push_back(tmp);
            }

            return true;

        }else{
            cout << "\nERROR : " << name << " failed to open\n";
            return false;
        }

    }

    bool u_load_File(const string &n) override{
        return false;
    }

    bool a_SyntaxChecker() override{

        int cnt =0;
        int flag =0;
        for(const string& i : MAcontent){cnt++; //REGEX
//^(( +)?(s[\d]+)( +)?;( +)?([\w]+)( +)?;(( +)?([\w]+)( +)?);( +)?([a-zA-Z0-9.\-%]+)@([a-zA-Z0-9.\-%]+)\.([a-z]{2,3})( +)?)$

            regex regs(R"(^(BS|MS)\;\[\{([ ]+)?([A-Z0-9]{7}([ ]+)?)((\,([ ]+)?[A-Z0-9]{7})+)?([ ]+)?\}((\,\{([ ]+)?([A-Z0-9]{7})((([ ]+)?\,([ ]+)?[A-Z0-9]{7})+([ ]+)?)?\})+)?\]?$)");

            if(!regex_match(i,regs)){
                cout << "\nERROR: Line number " << cnt << " does not follow the standard format of the add file\n";
                flag = 1;
            }

        }

        if(flag ==1){
            return false; // ERROR FOUND
        }else{
            return true;
        }
    }

    bool u_SyntaxChecker() override{
        return false;
    }

    bool LoadChanges() override{
        std::fstream ssfile;
        ssfile.open("db_corsi_studio.txt", std::ios::in | std::ios::out);
        if (ssfile.is_open()) {
            ssfile << "";
            for (const auto& n : MDBcontent) {
                ssfile << n << "\n";
            }
            DisarmEditing = true;
            return true;

        } else {
            cout << "\nERROR : Could not edit db_corsi_studio.txt\n";
            DisarmEditing = true;
            return false;
        }


    }

    void add_File() {
        //temp="A001";
        int last_id = AulaIDtoInt("C001");
        if(!MDBcontent.empty()){
            last_id = AulaIDtoInt(MDBcontent.back().substr(0,4));
        }
        int cnt = 0;
        string temp="";

        for(string n : MAcontent){
            temp = IntToAulaID(last_id+cnt)+";";
            cnt++;
            n = temp.append(n);
            n.append(";[]");///
            MDBcontent.push_back(n);
        }



    }

    void Tokenizer() override{
        vector<string> tempo;
        stringstream ss;
        temp = "";
        ss << "";
        for(string n : MDBcontent){
            ss<< n;
            while(ss.good()){
                temp = "";
                getline(ss,temp,';');
                tempo.push_back(temp);
            }

            MDBData.push_back(tempo);
            tempo.clear();
            ss.clear();
            ss << "";
        }
    }

    void assignMCodes(){
        int cnt = 0;
        stringstream ss;
        temp="";

        for (string n : MDBcontent) {
            ss << cnt;
            temp = ss.str();
            ss <<"";
            int fD;
            int sD;
            int tD;
            int foD;
            switch(temp.size()){
                case 1 :
                    fD = temp[0]-48;
                    temp = "";
                    temp += "A00";
                    temp.push_back(fD+48);
                    n = n.replace(0,4,temp);
                    break;
                case 2 :
                    sD = temp[0] -48;
                    fD = temp[1] -48;
                    temp = "";
                    temp += "A0";
                    temp.push_back(sD+48);
                    temp.push_back(fD+48);

                    n = n.replace(0,4,temp);
                    break;
                case 3 :
                    tD = temp[0] -48;
                    sD = temp[1] -48;
                    fD = temp[2] -48;
                    temp = "";
                    temp+="A";
                    temp.push_back(tD+48);
                    temp.push_back(sD+48);
                    temp.push_back(fD+48);
                    n = n.replace(0,4,temp);
                    break;
                case 4 :
                    foD = temp[0] -48;
                    tD = temp[1] -48;
                    sD = temp[2] -48;
                    fD = temp[3] -48;
                    temp = "";

                    temp.push_back(foD+65);
                    temp.push_back(tD+48);
                    temp.push_back(sD+48);
                    temp.push_back(fD+48);

                    n = n.replace(0,4,temp);
                    break;

                default :

                    break;


            }

            MDBcontent[cnt] = n;
            ss.str("");
            cnt++;
            temp = "";
        }
    }

    bool ErrorCheck(){
        return true;
    }


};



int GetLastID(vector<string> s,vector<string> p){
    if(s.empty() && !p.empty()){
        return stoi(p.back().substr(1,6));
    }

    if(!s.empty() && p.empty()){
        return stoi(s.back().substr(1,6));
    }

    if(s.empty() && p.empty()){
        return 0;
    }

    if(!s.empty() && !p.empty()){
        int last_s = stoi(s.back().substr(1,6));
        int last_p = stoi(p.back().substr(1,6));

        if(last_s > last_p){
            return last_s;
        }
        if(last_p > last_s){
            return last_p;
        }
        cout<<"\nERROR: Profs and Students cannot share ID's!\n";
        return 0;
    }
  return 0;
}

#endif //UNI_DATABASE_H
