//
// Created by Andrei Gheorghiu on 5/18/2022.
//

#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <set>
#include <string>
#include <sstream>
#include <regex>
#include <random>
#include <algorithm>
#include <map>
#include <iterator>
#include <sstream>
#include "mmath.h"


using std::string;
using std::vector;
using std::cout;
using std::regex;
using std::to_string;
using std::pair;
using std::stringstream;


struct slot{
    vector<int> indexes = vector<int>();//groups indexes
    vector<int> aind = vector<int>(); // aule indexes
    bool empty = true;
    bool sunday = false;
};


//Course stuff
namespace utility{

    int CourseIDtoInt(const string& id){
        regex regs(R"(^[0-9][1-9][A-Z]{5}$)");

        if (!regex_match(id, regs)) {
            cout <<"ERROR: String Course ID format is not correct. Must be of type 01AAAAA\n";
            return -1;
        }else{
            //matched
            return ((10*(id[0]-'0')+(id[1]-'0'-1))*11881376 + (26*26*26*26*(id[2]-'A')+26*26*26*(id[3]-'A')+26*26*(id[4]-'A')+26*(id[5]-'A')+(id[6]-'A')));
        }
    }

    string IntToCourseID(int in){
        string temp = "AAAAA";
        bool found = false;
        string id = "01AAAAA";
        int cnt = 1;
        if(CourseIDtoInt(id)==in){
            return id;
        }else{
            //increment
            while(!found){
                if(temp[4]!='Z'){
                    temp[4] += 1;
                }else{
                    temp[4] = 'A';
                    if(temp[3]!='Z'){
                        temp[3] += 1;
                    }else{
                        temp[3] = 'A';
                        if(temp[2]!='Z'){
                            temp[2] += 1;
                        }else{
                            temp[2] = 'A';
                            if(temp[1]!='Z'){
                                temp[1]+=1;
                            }else{
                                temp[1] = 'A';
                                if(temp[0]!='Z'){
                                    temp[0] += 1;
                                }else{
                                    temp[0] = 'A';
                                }
                            }
                        }
                    }

                }


                if(temp == "ZZZZZ"){
                    temp = "AAAAA";
                    cnt++;
                    if(cnt>9){
                        id = to_string(cnt) + temp;
                    }else{
                        id = "0"+to_string(cnt)+temp;
                    }
                }else{
                    if(cnt>9){
                        id = to_string(cnt) + temp;
                    }else{
                        id = "0"+to_string(cnt)+temp;
                    }
                }

                if(CourseIDtoInt(id)==in){
                    found = true;
                }

            }

        }
        return id;
    }


    vector<vector<string>> ExtractCoursesFromMajors(const string& in){
        vector<string> sem;
        vector<vector<string>> out;
        string str = in;
        std::regex rgx("\\{([^}]+)\\}");
        std::smatch match;
        while(std::regex_search(str,match,rgx)){
            //temp.push_back(match[1]);
            sem.push_back(match[1]);
            str = match.suffix();
        }

        for(const auto& i : sem){
            vector<string> tempp;
            string tmp = i;
            std::regex rg("[0-9A-Z]+");
            std::smatch m;
            while(std::regex_search(tmp,m,rg)){
                //temp.push_back(match[1]);
                tempp.push_back(m[0]);
                tmp = m.suffix();
            }
            out.push_back(tempp);
        }
        return out;
    }
//TO CONSTRUCT MAJOR OBJECTS

    vector<string> ExtractCorsiSpentiMajors(const string& in){
        vector<string> temp;
        string tmp = in;
        std::regex rg("[0-9A-Z]+");
        std::smatch m;
        while(std::regex_search(tmp,m,rg)){
            //temp.push_back(match[1]);
            temp.push_back(m[0]);
            tmp = m.suffix();
        }

        return temp;
    }

//TO CONSTRUCT COURSE OBJECTS. Must be called in order!

    vector<string> ExtractVersionsFromCourses(const string& in){
        //input Cbase.second[anno.slot + 3]
        vector<string> temp;
        string tmp = in;
        std::regex rg("\\{[\\S ]+?\\]\\}");
        std::smatch m;
        while(std::regex_search(tmp,m,rg)){
            //temp.push_back(match[1]);
            temp.push_back(m[0]);
            tmp = m.suffix();
        }

        return temp;
    }


////////


    void printVectorString(const vector<string>& in){
        cout<<"\n[";
        for(const auto& x : in){
            cout<<" "<<x<<" ";
        }
        cout<<"]\n";
    }

    string IntToID(const int& id){
//converts integer to 6 digit number string

        string s = to_string(id);
        switch(s.length()){
            case 1:
                s = "00000"+s;
                return s;

            case 2:
                s = "0000"+s;
                return s;

            case 3:
                s = "000"+s;
                return s;

            case 4:
                s = "00"+s;
                return s;

            case 5:
                s = "0"+s;
                return s;

            case 6:
                return s;

            default:
                return "000000";


        }
    }

    string IntToAulaID(const int& id){
        if(id<=25974){
            double division = ((double)id)/999;
            int letter_code = mmath::floor(division)+'A';
            char letter = letter_code;
            int rest = id - mmath::floor(division)*999;
            string temp = "";
            temp += letter;
            string s = to_string(rest);
            switch(s.length()){
                case 1:
                    s = "00"+s;
                    return (temp+s);

                case 2:
                    s = "0"+s;
                    return (temp+s);

                case 3:
                    return (temp+s);

                default:
                    return "A001";


            }
        }else{
            cout<<"\nERROR: Could not convert integer ID to Aula string ID\n";
            return "";
        }
    }

    int AulaIDtoInt(const string& id){
        return ((id[0]-'A')*999 + 100*(id[1]-'0')+10*(id[2]-'0')+(id[3]-'0'));

    }

    vector<string> LoadCMD(int argc,char *argv[]){
        vector <string> cmd;
        string temp;
        for (int i = 1; i < argc; i++){
            temp = "";
            for(int j = 0;j < strlen(argv[i]);j++){
                temp += argv[i][j];
            }

            cmd.push_back(temp);
        }
        return cmd;
    }

    pair<string,vector<vector<string>>> ExtractVersionDetails(const string& in){
        pair<string,vector<vector<string>>> output;
        string codice_versione;
        vector<vector<string>> prof_details;
        vector<string> temp_vector;
        vector<string> second_temp_vector;
        string text = in.substr(1, in.size() - 2);//removed first and last { } characters to facilitate matching
        string temp = text;
        string titolare;

        std::regex profsrgx(R"(\{[\S ]+?\})");// groups {d3333...}
        std::regex titolarergx("^[dD][0-9]{6}");
        std::regex codicergx("[A-Z][0-9]{3}");

        std::smatch a,b,c;
//match profs details
        while(std::regex_search(temp,a,profsrgx)){
            //temp.push_back(match[1]);
            temp_vector.push_back(a[0]);
            temp = a.suffix();
        }
        std::smatch d;
        std::regex per_prof("[dD 0-9]+");
        for(const auto& x: temp_vector){
            string tempos = x;
            while(std::regex_search(tempos,d,per_prof)){
                //temp.push_back(match[1]);
                second_temp_vector.push_back(d[0]);
                tempos = d.suffix();
            }
            prof_details.push_back(second_temp_vector);
            second_temp_vector.clear();
        }

        //match titolare
        if(std::regex_search(text,b,titolarergx)){
            titolare = b[0];
        }
        //match codice
        if(std::regex_search(text,c,codicergx)){
            codice_versione = c[0];
        }

        output = pair<string,vector<vector<string>>>(codice_versione,prof_details);
        return output;
    }

    vector<string> ExtractInstanceDetails(const string& in){
        vector<string> temp;
        string tmp = in;
        std::regex rg("[0-9A-Z]+");
        std::smatch m;
        while(std::regex_search(tmp,m,rg)){
            //temp.push_back(match[1]);
            temp.push_back(m[0]);
            tmp = m.suffix();
        }

        return temp;
    }

    bool isYear(const string& in){
        if(in.size()!=9){
            return false;
        }
        string tempone;
        string temptwo;
        tempone = in.substr(0,4);
        temptwo = in.substr(5,4);
        if((stoi(temptwo)-stoi(tempone))!=1){
            return false;
        }
        return true;
    }

    vector<string> TokenizeString(const string& in){
        //by ;  delimiter
        vector<string> tempo;
        stringstream ss;
        string temp = "";
        ss << "";
        ss<< in;
        while(ss.good()){
            temp = "";
            getline(ss,temp,';');
            tempo.push_back(temp);
        }
        return tempo;

    }

//main functions

    int CMDSyntaxCheck(vector<string> cmd){
        if(cmd[0] == "-a:s"){
            //add students
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -a:s ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 1;
            }else
            {return 0;}
        } else if(cmd[0] == "-a:d"){
            //add docenti ./eseguibile -a:d ../../cartella_a_caso/inputs/teacherslist.txt
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -a:d ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 2;
            }else
            {return 0;}
        } else if(cmd[0] == "-a:a"){
            //add aule
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -a:a ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 3;
            }else
            {return 0;}
        } else if(cmd[0] == "-a:c"){
            // add corsi
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -a:c ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 4;
            }else
            {return 0;}
        } else if(cmd[0] == "-a:f"){
            // add corsi studio majors
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -a:f ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 5;
            }else
            {return 0;}
        } else if(cmd[0]== "-u:s"){
            //update studenti
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -u:s ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 6;
            }else
            {return 0;}
        }else if(cmd[0]== "-u:d"){
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -u:d ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 7;
            }else
            {return 0;}
        }else if(cmd[0]== "-u:a"){
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -u:a ../../cartella_a_caso/inputs/nome_file.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 8;
            }else
            {return 0;}
        }else if(cmd[0]== "-i:c"){
            regex regs("^([\\S]+)?((([a-zA-Z0-9_.-]+\\/)+)?)([a-zA-Z0-9_.-]+.txt)$");

            if (!regex_match(cmd[1], regs)) {
                cout<<"ERROR: Invalid command format. Use format: -i:c ../../cartella_a_caso/inputs/file_esami.txt\n";
                return 0;
            } else if(cmd.size() == 2){
                return 9;
            }else
            {return 0;}
        }else if (cmd[0]=="-s"){
            if(cmd.size()>1){

                if(cmd[1]=="current_a"){
                    if(cmd.size()!=6) return 0;
                    string temp;
                    temp.append(cmd[0]);temp.append(" ");temp.append(cmd[1]);temp.append(" ");temp.append(cmd[2]);temp.append(" ");
                    temp.append(cmd[3]);temp.append(" ");temp.append(cmd[4]);temp.append(" ");temp.append(cmd[5]);

                    //set sessions for current year
                    regex regs(R"(^\-s current_a [0-9]{4}\-[0-9]{4}( [0-9]{4}\-[0-9]{2}\-[0-9]{2}\_[0-9]{4}\-[0-9]{2}\-[0-9]{2}){3}$)");
                    if (!regex_match(temp, regs)) {
                        cout<<"ERROR: Invalid command format. Use format: -s current_a <anno_accademico> <data_inizio>_<data_fine> "
                              "<data_inizio>_<data_fine> <data_inizio>_<data_fine>\n";
                        return 0;
                    }else{
                        //valid
                        return 10;

                    }


                }else if(cmd[1]=="set_availability"){
                    if(cmd.size()!=4) return 0;
                    string temp;
                    temp.append(cmd[0]);temp.append(" ");temp.append(cmd[1]);temp.append(" ");temp.append(cmd[2]);temp.append(" ");temp.append(cmd[3]);

                    //set sessions for current year
                    regex regs(R"(^\-s set_availability [0-9]{4}\-[0-9]{4} ([\S]+)?((([a-zA-Z0-9_.-]+\/)+)?)([a-zA-Z0-9_.-]+.txt)$)");
                    if (!regex_match(temp, regs)) {
                        cout<<"ERROR: Invalid command format. Use format: -s set_availability <anno_accademico> <file_indisponibilita>\n";
                        return 0;
                    }else{
                        //valid
                        return 11;

                    }


                }else{
                    return 0;
                }
            }else{return 0;}

        }else if(cmd[0]=="-g") {
            //scheduling request
            if(cmd.size()==3){

                string temp;
                temp.append(cmd[0]);temp.append(" ");temp.append(cmd[1]);temp.append(" ");temp.append(cmd[2]);

                //set sessions for current year
                regex regs(R"(^\-g [0-9]{4}\-[0-9]{4} ([\S]+)?((([a-zA-Z0-9_.-]+\/)+)?)([a-zA-Z0-9_.-]+.txt)$)");
                if (!regex_match(temp, regs)) {
                    cout<<"ERROR: Invalid command format. Use format: -g <anno_accademico> <output_file>\n";
                    return 0;
                }else{
                    //valid
                    return 12;

                }


            }else{
                return 0;
            }

        }else{
            //error message
            return 0;
        }

    }


    int ExitProgramError(){
        // printf("\n\tType and enter anything to exit...\n");
        // cin.get();
        return -1;
    }
    int ExitProgram(){
        // printf("\n\tType and enter anything to exit...\n");
        // cin.get();
        return 0;
    }

//Scheduling

    bool isSlotEmpty(const slot& slt){
        if(slt.empty){
            return true;
        }else{
            return false;
        }
    }

    int GetRandom(int first,int last){
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> distr(first, last); // define the range

        return distr(gen);
    }

    string IntToTimeOfDay(int time){
        if((time<0)||(time > 5)){
            return "";
        }else{

            switch(time){
                case 0:
                    return "8:00-10:00";
                    break;
                case 1:
                    return "10:00-12:00";
                    break;
                case 2:
                    return "12:00-14:00";
                    break;
                case 3:
                    return "14:00-16:00";
                    break;
                case 4:
                    return "16:00-18:00";
                    break;
                case 5:
                    return "18:00-20:00";
                    break;
                default:
                    return "";
                    break;
            }

        }
    }

}

//Debugging

std::chrono::time_point<std::chrono::system_clock> TimerStart(){
    auto const t0 = std::chrono::high_resolution_clock::now();
    return t0;
}

void TimerRead(const std::chrono::time_point<std::chrono::system_clock> t0){
    auto const t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = t1 - t0;
    std::cout << "implementation duration: " << duration.count() << "s\n";
}


//
/* PRINTS COURSES PROFS BY EACH VERSION
 for(const auto &x : courses){
         cout<<"\n____________________"<<utility::IntToCourseID(x.second.id())<<"____________________\n";
     for(const auto &y : x.second.Instance(anno).second.lista_versioni){
         for(const auto &p : y.lista_prof){
             cout<<p.id<<" | ";
         }
         cout<<"\n";
     }
     cout<<"\n________________________________________________________________________________________\n";
 }
 */

/*PRINT GROUPS
                                  for(const auto &i : winter_groups){
                                    cout<<i[0]->appello()<<"  <- APPELLO  : ";
                                    for(const auto &k : i){
                                        for(const auto & z: k->profList()){
                                            cout<<z<<" | ";
                                        }
                                    }
                                    cout<<"\n";
                                }
 */


#endif //UTILITY_H
