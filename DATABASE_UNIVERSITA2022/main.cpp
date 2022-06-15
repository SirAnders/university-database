#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <string>
#include "uni_database.h"
#include "utility.h"
#include "exam_scheduler.h"
#include "university.h"

using utility::ExitProgramError;
using utility::ExitProgram;

using namespace std;

bool InitializeDatabase(Students_db &sdb,Profs_db &pdb,Aule_db &adb,Courses_db &cdb,Majors_db &mdb);
pair<bool,Exam_db> InitializeExam_db(Exam_db exam, const map<int,vector<string>>& Pbase);
map<int,Major> InitializeMajors(const map<int,vector<string>>& Mbase);
map<int,Student> InitializeStudents(const map<int,vector<string>>& Sbase);
map<int,Prof> InitializeProfs(const map<int,vector<string>>& Pbase,const map<int,vector<pair<int,int>>>& av);
map<int,Course> InitializeCourses(const map<int,vector<string>>& Cbase,const map<int,Major> &m);
map<int,Aula> InitializeAule(const map<int,vector<string>>& Abase);
vector<vector<std::shared_ptr<Exam>>> InitializeExams(const map<int,Course> &courses,const string &anno,semester sem);

int main(int argc, char *argv[]) {    // [Made by Andrei Gheorghiu @ s258922@studenti.polito.it]
   // auto t0 = TimerStart();
    map<int,vector<string>> Sbase,Cbase,Pbase,Mbase,Abase;
    vector<Exam> winter_exams,summer_exams,autumn_exams;
    map<int,Student> students;map<int,Prof> profs;map<int,Major> majors;map<int,Course> courses;map<int,Aula> aule;
    map <int,vector<string>> dummy;
    bool flags,flagp,flagc,flagm,flaga;
    cout << "_____________________________________________________________________________________________\n";
    cout<<"\n                         University Database & Exam Scheduling Program      v2022                   \n";
    cout << "_____________________________________________________________________________________________\n";

    if(argc > 1){ // generates a string array containing all the command line arguments

        //convert char arrays of the commands to string type
        vector <string> cmd = utility::LoadCMD(argc,argv); // vector of command arguments, cmd[0] is the first argument after the .exe name
        string temp;
        vector <string> temporar;

        int request = utility::CMDSyntaxCheck(cmd);
        //cout<<"\n"<<cmd[0]<<"\n";
        //cout<<"Syntax output code is: "<<request<<"\n";       ////////

        Students_db sdb("db_studenti.txt");
        Profs_db pdb("db_professori.txt");
        Aule_db adb("db_aule.txt");
        Courses_db cdb("db_corsi.txt");
        Majors_db mdb("db_corsi_studio.txt");
        Exam_db exam;
        //request = 12;///////////////////////FORCED
        switch(request){

            case 1 : // -a:s

                flags = sdb.did_it_Fail();
                if(!flags){
                    sdb.a_load_File(cmd[1]);
                    bool fs1 = sdb.SyntaxChecker(0, cmd[1]);
                    bool fs2 = sdb.a_SyntaxChecker();
                    bool fs3 = pdb.SyntaxChecker(0,cmd[1]);
                    if(fs1 && fs2 && fs3) {
                       // SPI::assign_ID(&sdb, &pdb);
                        pdb.Tokenizer();
                        sdb.Tokenizer();
                        sdb.add_File(GetLastID(sdb.GiveContent(),pdb.GiveContent()));
                        if (sdb.checkStudentEmails() && sdb.SyntaxChecker(1, cmd[1])){
                            if (sdb.LoadChanges()) {
                                cout << "\nSUCCESSFUL OPERATION: Student list added to the database\n";
                                return 0;
                            } else {
                                cout << "\nERROR: Failed to edit db_studenti.txt\n";
                                return -1;
                            }
                        }else{
                            cout << "\nERROR: Errors have been found in the db_studenti.txt and/or "<<cmd[1]<<" files\n";
                            return -1;
                        }

                    }else{
                        cout << "\nERROR: Errors have been found in the db_studenti.txt and/or "<<cmd[1]<<" files\n";
                        return -1;
                    }



                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create student database file\n";
                    return -1;
                }

                //break;

            case 2 : // -a:d


                flagp = pdb.did_it_Fail();
                if(!flagp){
                    pdb.a_load_File(cmd[1]);
                    bool f1 = pdb.SyntaxChecker(0, cmd[1]);
                    bool f2 =  pdb.a_SyntaxChecker();
                    bool f3 = sdb.SyntaxChecker(0, cmd[1]);
                    if (f1 && f2 && f3){

                        sdb.Tokenizer();
                        pdb.Tokenizer();
                        pdb.add_File(GetLastID(sdb.GiveContent(),pdb.GiveContent()));
                        if(pdb.checkProfEmails() && pdb.SyntaxChecker(1, cmd[1])) {
                            if (pdb.LoadChanges()) {
                                cout << "\nSUCCESSFUL OPERATION: Professors list added to the database\n";
                                return 0;
                            } else {
                                cout << "\nERROR: Failed to edit db_professori.txt\n";
                                return -1;
                            }
                        }else{
                            //ERROR
                            cout << "\nERROR : Failed to add new professors list";
                            return -1;
                        }

                    }else{
                        //ERROR
                        cout << "\nERROR : Failed to add new professors list";
                        return -1;
                    }


                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create professor database file\n";
                    return -1;
                }
                //break;

            case 3 : // -a:a


                flaga = adb.did_it_Fail();
                if(!flaga) {
                    adb.a_load_File(cmd[1]);
                    bool fa1 = adb.SyntaxChecker(0, cmd[1]);
                    bool fa2 = adb.a_SyntaxChecker();
                    bool fa3 = adb.checkTitles();
                    if (fa1 && fa2 && fa3) {
                        adb.add_File();
                       // adb.assignCodes();
                        adb.Tokenizer();
                        if(adb.SyntaxChecker(0, cmd[1]) && adb.checkTitles()) {
                            if (adb.LoadChanges()) {
                                cout << "\nSUCCESSFUL OPERATION: Classroom list added to the database\n";
                                return 0;
                            } else {
                                cout << "\nERROR: Failed to edit db_aule.txt\n";
                                return -1;
                            }
                        }else{
                            //ERROR
                            cout << "\nERROR : Syntax errors found in db_aule.txt\n";
                            return -1;
                        }
                    } else {
                        //ERROR
                        cout << "\nERROR : Syntax errors found in db_aule.txt\n";
                        return -1;
                    }
                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create classroom database file\n";
                    return -1;
                }
                //break;

            case 4 : // -a:c


                flagc = cdb.did_it_Fail();
                if(!flagc) {
                    cdb.a_load_File(cmd[1]);
                    bool fa11 = cdb.SyntaxChecker(0, cmd[1]);
                    bool fa22 = cdb.a_SyntaxChecker();
                    cdb.Tokenizer();
                    if (fa11 && fa22) {
                        cdb.a_Tokenizer();
                        cdb.Mapper();
                        cdb.add_File();
                       // cdb.printCourseFile();///////////////
                      //  cdb.assignCCodes();
                      //cdb.printCourseFile();/////////////
                        if (cdb.SyntaxChecker(0, cmd[1]) && cdb.checkErrors()){
                            if (cdb.LoadChanges()) {
                                cout << "\nSUCCESSFUL OPERATION: Course list added to the database\n";
                                return 0;
                            } else {
                                return -1;
                            }
                        }else{
                            //ERROR
                            cout << "\nERROR : Syntax errors found in db_corsi.txt\n";
                            return -1;
                        }
                    } else {
                        //ERROR
                        cout << "\nERROR : Syntax errors found in db_corsi.txt\n";
                        return -1;
                    }
                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create cdb database file\n";
                    return -1;
                }
                //break;

            case 5 : // -a:f

                flagm = mdb.did_it_Fail();
                if(!flagm) {
                    mdb.a_load_File(cmd[1]);
                    bool fma1 = mdb.SyntaxChecker(0, cmd[1]);
                    bool fma2 = mdb.a_SyntaxChecker();
                    if (fma1 && fma2) {
                        mdb.add_File();
                        //mdb.assignMCodes();
                        mdb.Tokenizer();
                        mdb.printMData();
                        if(mdb.SyntaxChecker(1, cmd[1])) {
                            if (mdb.LoadChanges()) {
                                cout << "\nSUCCESSFUL OPERATION: Majors list added to the database\n";
                                return 0;
                            } else {
                                return -1;
                            }
                        }else{
                            //ERROR
                            cout << "\nERROR : Syntax errors found in db_corsi_studio.txt\n";
                            return -1;
                        }
                    } else {
                        //ERROR
                        cout << "\nERROR : Syntax errors found in db_corsi_studio.txt\n";
                        return -1;
                    }
                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create major database file\n";
                    return -1;
                }

                //break;

            case 6 : // update students

                if(!sdb.did_it_Fail()){
                    sdb.u_load_File(cmd[1]);
                    if(sdb.SyntaxChecker(0, cmd[1]) && sdb.u_SyntaxChecker()) {
                       // SPI::assign_ID(&sdb,&pdb);
                        sdb.Tokenizer();
                        pdb.Tokenizer();
                        sdb.u_Tokenizer();
                        sdb.Mapper();
                        if(!sdb.Update()){
                            cout<<"\nERROR: Requested update for students not present in the database\n";
                            return -1;
                        }else{
                            sdb.Rebuild();
                            if(sdb.SyntaxChecker(1, cmd[1]) && sdb.checkStudentEmails_u()){
                                if(sdb.LoadChanges()){
                                    cout<<"\nSUCCESSFUL OPERATION: Updated student database\n";
                                    return 0;
                                }else{
                                    return -1;
                                }
                            }else{
                                cout<<"\nERROR: Syntax errors found in db_studenti.txt or in "<<cmd[1]<<"\n";
                                return -1;
                            }
                        }

                    }else{
                        cout<<"\nERROR: Syntax errors found in db_students.txt or in "<<cmd[1]<<"\n";
                        return -1;
                    }


                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create students database file\n";
                    return -1;
                }

                //break;

            case 7 :
                if(!pdb.did_it_Fail()){
                    pdb.u_load_File(cmd[1]);
                    if(pdb.SyntaxChecker(0, cmd[1]) && pdb.u_SyntaxChecker()) {
                        //SPI::assign_ID(&sdb,&pdb);
                        pdb.Tokenizer();
                        sdb.Tokenizer();
                        pdb.u_Tokenizer();
                        pdb.Mapper();
                        if(!pdb.Update()) {
                            cout << "\nERROR: Requested update for professors not present in the database\n";
                            return -1;
                        }else{
                            pdb.Rebuild();
                            if(pdb.SyntaxChecker(1, cmd[1]) && pdb.checkProfEmails_u()){
                                if(pdb.LoadChanges()){
                                    cout<<"\nSUCCESSFUL OPERATION: Updated professors database\n";
                                    return 0;
                                }else{
                                    return -1;
                                }
                            }else{
                                cout<<"\nERROR: Syntax errors found in db_corsi.txt or in "<<cmd[1]<<"\n";
                                return -1;
                            }
                        }
                    }else{
                        cout<<"\nERROR: Syntax errors found in db_professori.txt or in "<<cmd[1]<<"\n";
                        return -1;
                    }


                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create professors database file\n";
                    return -1;
                }

                //break;

            case 8 :
                if(!adb.did_it_Fail()){
                    adb.u_load_File(cmd[1]);
                    if(adb.SyntaxChecker(0, cmd[1]) && adb.u_SyntaxChecker()) {
                        //adb.assignCodes();
                        adb.Tokenizer();
                        adb.u_Tokenizer();
                        adb.Mapper();

                        if(!adb.Update()){
                            cout<<"\nERROR: Requested update for classrooms not present in the database\n";
                            return -1;
                        }
                        adb.Rebuild();
                        if(adb.SyntaxChecker(0, cmd[1]) && adb.checkTitles_u()){
                            if(adb.LoadChanges()){
                                cout<<"\nSUCCESSFUL OPERATION: Updated classroom database\n";
                                return 0;
                            }else{
                                return -1;
                            }
                        }else{
                            cout<<"\nERROR: Syntax errors found in db_aule.txt or in "<<cmd[1]<<"\n";
                            return -1;
                        }
                    }else{
                        cout<<"\nERROR: Syntax errors found in db_aule.txt or in "<<cmd[1]<<"\n";
                        return -1;
                    }


                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create classroom database file\n";
                    return -1;
                }
                //break;

            case 9 : // insert new course organization
                if(!cdb.did_it_Fail()){
                    cdb.u_load_File(cmd[1]);
                    if(cdb.SyntaxChecker(0, cmd[1]) && cdb.u_SyntaxChecker()) {
                       // cdb.assignCCodes();
                        cdb.Tokenizer();
                        cdb.u_Tokenizer();
                        cdb.Mapper();
                        //cdb.printCBase();///////////////
                        if(!cdb.Update()){
                            return -1;
                        }else{
                            //cdb.printCBase();////////////
                            cdb.Rebuild();
                            if(cdb.SyntaxChecker(1, cmd[1]) && cdb.checkErrors_u()){
                                if(cdb.LoadChanges()){
                                    cout<<"\nSUCCESSFUL OPERATION: Insertion of new course organization\n";
                                    return 0;
                                }else{
                                    cout<<"\nERROR: Failed to load update changes into the db_corsi.txt file\n";
                                    return -1;
                                }
                            }else{
                                cout<<"\nERROR: Syntax errors found in db_corsi.txt or in "<<cmd[1]<<"\n";
                                return -1;
                            }
                        }

                    }else{
                        cout<<"\nERROR: Syntax errors found in db_corsi.txt or in "<<cmd[1]<<"\n";
                        return -1;
                    }


                }else{
                    //ERROR
                    cout << "\nERROR : Failed to load/create course database file\n";
                    return -1;
                }

                //break;

            case 10:
                //SET SESSIONS

                if(exam.CheckSessions(cmd)){
                    if(exam.LoadSessionFile()){

                        exam.AddNewSession();

                        if(exam.WriteSessionFile()){
                            //OK
                            cout<<"\nNew sessions for the year "<<cmd[2]<<" have been saved successfully!\n";
                            return 0;
                        }else{
                            return -1;

                        }




                    }else{
                        return -1;
                    }
                }else{
                    return -1;
                }

                //break;

            case 11:
                //SET INDISPONIBILITA PROFS
                if(isYear(cmd[2])){
                    if(exam.LoadAvailabilityFile() && exam.LoadAvailabilityAddFile(cmd[3])){
                        //if not empty, check for correct syntax of the add availability file.
                        exam.TokenizeAvailability();
                        exam.MapAvailability();

                        if(exam.SyntaxAvailabilityAdd()){

                            if(exam.AddAvailability(cmd[2])){

                                if(exam.CheckIntegrity(dummy)){
                                    exam.MapToContent();
                                    if(exam.WriteAvailability()){
                                        cout<<"\nProfessor availability list updated successfully for the year "<<cmd[2]<<"\n";
                                        return 0;
                                    }else{
                                        return -1;
                                    }

                                }else{
                                    cout<<"\nERROR in exam_scheduler::Exam_db::CheckIntegrity() : Conflicts in availability schedule.\n";
                                    return -1;
                                }

                            }else{
                                cout<<"\nERROR in exam_scheduler::Exam_db::AddAvailability() : Cannot proceed with availability update.\n";
                                return -1;
                            }
                        }else{
                            return -1;
                        }


                    }else{
                        return -1;
                    }
                }else{
                    cout<<"\nERROR in utility::isYear() : "<<cmd[2]<<" is not a valid academic year. An example of a valid year is 2020-2021 or 2009-2010.\n";
                    return -1;
                }


                //break;

            case 12:
                //EXAM SCHEDULER
                if(InitializeDatabase(sdb,pdb,adb,cdb,mdb)){
                    if(cmd.size()>1){

                        if(isYear(cmd[1])){
                            string nome_file = cmd[2];
                            string anno = cmd[1];

                            Sbase = sdb.OutputMap();      Pbase = pdb.OutputMap();          Abase = adb.OutputMap();       Cbase = cdb.OutputMap();
                            Mbase = mdb.OutputMap();

                            //initialize all objects from databases
                            pair<bool,Exam_db> ex = InitializeExam_db(exam, Pbase);
                            if(ex.first){

                                students =  InitializeStudents(Sbase);
                                profs =  InitializeProfs(Pbase,((ex.second).GetProfAV()));
                                majors =  InitializeMajors(Mbase);
                                aule =  InitializeAule(Abase);
                                courses =  InitializeCourses(Cbase,majors);


                                Session winter_session = (ex.second).FileToSession(anno,Winter);
                                Session summer_session = (ex.second).FileToSession(anno,Summer);
                                Session autumn_session = (ex.second).FileToSession(anno,Autumn);

                                vector<vector<std::shared_ptr<Exam>>> winter_groups = InitializeExams(courses,anno,Winter);
                                vector<vector<std::shared_ptr<Exam>>> summer_groups = InitializeExams(courses,anno,Summer);
                                vector<vector<std::shared_ptr<Exam>>> autumn_groups = InitializeExams(courses,anno,Autumn);

                                /////////////////////////////////////

                                //initialize empty schedules
                                Schedule winter(Winter,winter_groups);
                                Schedule summer(Summer,summer_groups);
                                Schedule autumn(Autumn,autumn_groups);

                                cout<<"\nScheduling exam sessions for the "<<anno<<" academic year . . .\n";


                            //SCHEDULE WINTER
                                const int MAX_N = 10;
                                bool done = false;
                                for(int i = 0;(i<MAX_N)&&(!done);i++){
                                    //cout<<i+1<<endl;/////////
                                    if(ScheduleExams(winter_groups, winter, 0, profs, winter_session, anno, aule))
                                        {
                                         done = true;
                                        }else{
                                            winter.clearSchedule();
                                         }


                                }
                                if(done){
                                 //   winter.printCalendar();///////
                                 //   winter.printClassroomCalendar();
                                    if(genSessionFile(nome_file,winter_session,winter,anno,genSessionFileContent(winter_session,winter,anno,winter_groups,aule)))
                                        cout<<"\nExams for the session: "<<dates::EpochToDate(winter_session.start_date)<<" / "<<dates::EpochToDate(winter_session.end_date)<<" have been scheduled successfully.\n";
                                }else{
                                    //relax first constraint!
                                }

                                //SCHEDULE SUMMER
                                done = false;
                                for(int i = 0;(i<MAX_N)&&(!done);i++){
                                    //cout<<i+1<<endl;/////////
                                    if(ScheduleExams(summer_groups, summer, 0, profs, summer_session, anno, aule))
                                    {
                                        done = true;
                                    }else{
                                        summer.clearSchedule();
                                    }


                                }
                                if(done){
                                    if(genSessionFile(nome_file,summer_session,summer,anno,genSessionFileContent(summer_session,summer,anno,summer_groups,aule)))
                                        cout<<"\nExams for the session: "<<dates::EpochToDate(summer_session.start_date)<<" / "<<dates::EpochToDate(summer_session.end_date)<<" have been scheduled successfully.\n";

                                }else{
                                    //relax first constraint!
                                }

                                //SCHEDULE AUTUMN
                                done = false;
                                for(int i = 0;(i<MAX_N)&&(!done);i++){
                                    //cout<<i+1<<endl;/////////
                                    if(ScheduleExams(autumn_groups, autumn, 0, profs, autumn_session, anno, aule))
                                    {
                                        done = true;
                                    }else{
                                        autumn.clearSchedule();
                                    }


                                }
                                if(done){
                                    if(genSessionFile(nome_file,autumn_session,autumn,anno,genSessionFileContent(autumn_session,autumn,anno,autumn_groups,aule)))
                                    cout<<"\nExams for the session: "<<dates::EpochToDate(autumn_session.start_date)<<" / "<<dates::EpochToDate(autumn_session.end_date)<<" have been scheduled successfully.\n";

                                }else{
                                    //relax first constraint!
                                }





                            }else{
                                cout<<"\nERROR: Failed to initialize the database for the exam generation procedure\n";
                                return -1;
                            }
                    }else{
                            cout<<"\nERROR: Failed to initialize the database for the exam generation procedure: "<<cmd[1]<<" is not a valid year format.\n";
                            return -1;
                        }


                    }else{
                        cout<<"\nERROR: Failed to initialize the database for the exam generation procedure\n";
                        return -1;
                    }
                }else{
                    cout<<"\nERROR: Failed to initialize the database for the exam generation procedure\n";
                    return -1;
                }


                break;

            default :
                cout<<"\nERROR: UNKNOWN COMMAND\n";
                return -1;
        }


    }else{
        cout << "\nERROR : NO COMMANDS RECEIVED" << "\n";
        return -1;
    }

    return 0;

}

bool InitializeDatabase(Students_db &sdb,Profs_db &pdb,Aule_db &adb,Courses_db &cdb,Majors_db &mdb){
    //initialize students
    if(!sdb.did_it_Fail()) {
        bool fs1 = sdb.SyntaxChecker(0, "");
        bool fs3 = pdb.SyntaxChecker(0, "");
        if (fs1 && fs3 && sdb.checkStudentEmails()){
            sdb.Tokenizer();
            sdb.Mapper();
        }else{
            return false;
        }
    }else{
        return false;
    }
    if(!pdb.did_it_Fail()) {
        bool fs1 = sdb.SyntaxChecker(0, "");
        bool fs3 = pdb.SyntaxChecker(0, "");
        if (fs1 && fs3 && pdb.checkProfEmails()){
            pdb.Tokenizer();
            pdb.Mapper();
        }else{
            return false;
        }
    }else{
        return false;
    }

    if(!cdb.did_it_Fail()) {
            cdb.SyntaxChecker(0,"");
        if (cdb.SyntaxChecker(0,"") && cdb.checkErrors()){
            cdb.Tokenizer();
            cdb.Mapper();
        }else{
            return false;
        }
    }else{
        return false;
    }

    if(!adb.did_it_Fail()) {
        adb.SyntaxChecker(0,"");
        if (adb.SyntaxChecker(0,"") && adb.checkTitles()){
            adb.Tokenizer();
            adb.Mapper();
        }else{
            return false;
        }
    }else{
        return false;
    }
//////////
    if(!mdb.did_it_Fail()) {
        mdb.SyntaxChecker(0,"");
        if (mdb.SyntaxChecker(0,"") && mdb.ErrorCheck()){
            mdb.Tokenizer();
            mdb.Mapper();
        }else{
            return false;
        }
    }else{
        return false;
    }

    //done
    return true;
}

map<int,Student> InitializeStudents(const map<int,vector<string>>& Sbase){
    map<int,Student> students;
    for(auto const& s : Sbase){
        Student student = Student(s.first,s.second[2],s.second[0],s.second[1]); //call constructor
        students.insert(pair<int,Student>(s.first,student));
    }
    return students;
}

map<int,Aula> InitializeAule(const map<int,vector<string>>& Abase){
    map<int,Aula> aule;
    for(auto const& a : Abase){
        Aula aula(a.first,a.second[2],a.second[3],a.second[1],a.second[0]); //call constructor
        aule.insert(pair<int,Aula>(a.first,aula));
    }
    return aule;
}

map<int,Prof> InitializeProfs(const map<int,vector<string>>& Pbase,const map<int,vector<pair<int,int>>>& av){
    map<int,Prof> profs;
    for(auto const& p : Pbase){
        Prof prof = Prof(p.first,p.second[2],p.second[0],p.second[1],av); //call constructor
        profs.insert(pair<int,Prof>(p.first,prof));
    }
    return profs;
}

map<int,Major> InitializeMajors(const map<int,vector<string>>& Mbase){
    map<int,Major> majors;
    for(auto const& m : Mbase){
        Major major = Major(m.first,m.second[0],m.second[1],m.second[2]); //call constructor
        majors.insert(pair<int,Major>(m.first,major));
    }
    return majors;
}

map<int,Course> InitializeCourses(const map<int,vector<string>>& Cbase,const map<int,Major> &m){
    map<int,Course> courses;
    for(auto const& c : Cbase){
        Course course = Course(c.first,c.second,m); //call constructor
        courses.insert(pair<int,Course>(c.first,course));
    }
    return courses;
}

pair<bool,Exam_db> InitializeExam_db(Exam_db exam, const map<int,vector<string>>& Pbase){

        if(exam.LoadAvailabilityFile()){
            if(exam.LoadSessionFile()){
                //if not empty, check for correct syntax of the add availability file.
                exam.TokenizeAvailability();
                exam.MapAvailability();
                if(exam.CheckIntegrity(Pbase,true)){
                    exam.MapProfAV();
                   // exam.printProfAV();/////////////////////
                    return {true, exam};

                }else{
                    cout<<"\nERROR: Conflicts in availability schedule: insertion process aborted.\n";
                    return {false, exam};
                }



            }else{
                return {false, exam};
            }



        }else{
            return {false, exam};
        }

}

vector<vector<std::shared_ptr<Exam>>> InitializeExams(const map<int,Course> &courses, const string &anno, semester sem){
    if(isYear(anno)){
        vector<vector<std::shared_ptr<Exam>>> output;
        vector<std::shared_ptr<Exam>> tgroup;
        vector<int> indx;//contains keys to courses already grouped

        for(auto it = courses.begin();it!=courses.end();it++){
            if(((((*it).second).Instance(anno)).first) && (find(indx.begin(),indx.end(),(*it).first) == indx.end())){

                indx.push_back((*it).first);
                instance inst = (((*it).second).Instance(anno)).second;//found instance of year Y in course X
                vector<string> ragr = ((*it).second).group(inst);

                if(!((*it).second).active(inst)){
                    //generate one exam for this session per version
                    for(int j = 0;j<inst.nr_versioni;j++){
                        int aps = j+1;
                       // Exam etemp = Exam(aps,(*it).second,j,anno,1,inst);
                        tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,1,inst)));
                    }
                    for(const auto &r : ragr){
                        auto ptr = courses.find(CourseIDtoInt(r));
                        indx.push_back((*ptr).first);
                        instance p_inst =  (((*ptr).second).Instance(anno)).second;
                        for(int k = 0;k<inst.nr_versioni;k++){
                            int aps = k+1;
                          //  Exam etemp = Exam(aps,(*it).second,k,anno,1,p_inst);
                            tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,1,inst)));
                        }
                    }

                    output.push_back(tgroup);
                    tgroup.clear();
                }else{
                    if((((*it).second).course_semester() == 1) && (sem == Winter)){

                        //generate two exams for course for this session per version
                        for(int i =0;i<2;i++){
                            for(int j = 0;j<inst.nr_versioni;j++){
                                int aps = j+1;

                               // Exam etemp = Exam(aps,(*it).second,j,anno,i+1,inst);
                                tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,i+1,inst)));
                            }
                            for(const auto &r : ragr){
                                auto ptr = courses.find(CourseIDtoInt(r));
                                indx.push_back((*ptr).first);
                                instance p_inst =  (((*ptr).second).Instance(anno)).second;
                                for(int k = 0;k<inst.nr_versioni;k++){
                                    int aps = k+1;
                                    //Exam etemp = Exam(aps,(*it).second,k,anno,i+1,p_inst);
                                    tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,i+1,inst)));
                                }
                            }

                            output.push_back(tgroup);
                            tgroup.clear();
                        }

                    }
////////////////////////
                    if((((*it).second).course_semester() == 1) && (sem == Autumn)){
                        //generate one exam for this session per version
                        for(int j = 0;j<inst.nr_versioni;j++){
                            int aps = j+1;
                            //Exam etemp = Exam(aps,(*it).second,j,anno,1,inst);
                            tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,1,inst)));
                        }
                        for(const auto &r : ragr){
                            auto ptr = courses.find(CourseIDtoInt(r));
                            indx.push_back((*ptr).first);
                            instance p_inst =  (((*ptr).second).Instance(anno)).second;
                            for(int k = 0;k<inst.nr_versioni;k++){
                                int aps = k+1;
                                //Exam etemp = Exam(aps,(*it).second,k,anno,1,p_inst);
                                tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,1,inst)));
                            }
                        }

                        output.push_back(tgroup);
                        tgroup.clear();
                    }

                    if((((*it).second).course_semester() == 1) && (sem == Summer)){
                        //generate one exam for this session per version
                        for(int j = 0;j<inst.nr_versioni;j++){
                            int aps = j+1;
                            //Exam etemp = Exam(aps,(*it).second,j,anno,1,inst);
                            tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,1,inst)));
                        }
                        for(const auto &r : ragr){
                            auto ptr = courses.find(CourseIDtoInt(r));
                            indx.push_back((*ptr).first);
                            instance p_inst =  (((*ptr).second).Instance(anno)).second;
                            for(int k = 0;k<inst.nr_versioni;k++){
                                int aps = k+1;
                               // Exam etemp = Exam(aps,(*it).second,k,anno,1,p_inst);
                                tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,1,inst)));
                            }
                        }

                        output.push_back(tgroup);
                        tgroup.clear();
                    }
                    //////////////////////

                    if((((*it).second).course_semester() == 2) && (sem == Winter)){
                        //generate one exam for this session per version
                        for(int j = 0;j<inst.nr_versioni;j++){
                            int aps = j+1;
                           // Exam etemp = Exam(aps,(*it).second,j,anno,1,inst);
                            tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,1,inst)));
                        }
                        for(const auto &r : ragr){
                            auto ptr = courses.find(CourseIDtoInt(r));
                            indx.push_back((*ptr).first);
                            instance p_inst =  (((*ptr).second).Instance(anno)).second;
                            for(int k = 0;k<inst.nr_versioni;k++){
                                int aps = k+1;
                              //  Exam etemp = Exam(aps,(*it).second,k,anno,1,p_inst);
                                tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,1,inst)));
                            }
                        }

                        output.push_back(tgroup);
                        tgroup.clear();
                    }

                    if((((*it).second).course_semester() == 2) && (sem == Autumn)){
                        //generate one exam for this session per version
                        for(int j = 0;j<inst.nr_versioni;j++){
                            int aps = j+1;
                           // Exam etemp = Exam(aps,(*it).second,j,anno,1,inst);
                            tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,1,inst)));
                        }
                        for(const auto &r : ragr){
                            auto ptr = courses.find(CourseIDtoInt(r));
                            indx.push_back((*ptr).first);
                            instance p_inst =  (((*ptr).second).Instance(anno)).second;
                            for(int k = 0;k<inst.nr_versioni;k++){
                                int aps = k+1;
                               // Exam etemp = Exam(aps,(*it).second,k,anno,1,p_inst);
                                tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,1,inst)));
                            }
                        }

                        output.push_back(tgroup);
                        tgroup.clear();
                    }

                    if((((*it).second).course_semester() == 2) && (sem == Summer)){
                        //generate two exams for course for this session per version
                        for(int i =0;i<2;i++){
                            for(int j = 0;j<inst.nr_versioni;j++){
                                int aps = j+1;
                                //Exam etemp = Exam(aps,(*it).second,j,anno,i+1,inst);
                                tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*it).second,j,anno,i+1,inst)));
                            }
                            for(const auto &r : ragr){
                                auto ptr = courses.find(CourseIDtoInt(r));
                                indx.push_back((*ptr).first);
                                instance p_inst =  (((*ptr).second).Instance(anno)).second;
                                for(int k = 0;k<inst.nr_versioni;k++){
                                    int aps = k+1;
                                   // Exam etemp = Exam(aps,(*it).second,k,anno,i+1,p_inst);
                                    tgroup.push_back(std::make_shared<Exam>(Exam(aps,(*ptr).second,k,anno,i+1,inst)));
                                }
                            }

                            output.push_back(tgroup);
                            tgroup.clear();
                        }
                    }


                }




            }

        }

        return output;
    }else{
        return {};
    }
}