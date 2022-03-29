#include <iostream>
#include<vector>
#include<map>
#include<string>
#include<sstream>
#include<fstream>
#include <filesystem>
#include <unistd.h>
using namespace std;

//global variables
int rows=10;    
int seatsEachRow=20;                                                // number of seats in each row
int totalSeats=rows*seatsEachRow;                                   //total available seata in theater
vector<int> availableSeatsRow(10,20);                               //available seats in each row 
vector<vector<string> > theatreSeats(10, vector<string>(20," "));   // theater layout
vector<string> reserveID;                                           // to store reserve IDs
map<string, vector<string> > reserved_seatsMap;                     // hashmap to store reservation ID and seats allocated

//tokenizer - tokenize the reservation ID and number of seat requested
vector<string> tokenize(string reservation, char delimiter){
    stringstream ss(reservation);
    string s;
    vector<string> tokens;
    while(getline(ss,s,delimiter)){
        tokens.push_back(s);
    }
    return tokens;
}

//allocate seats 
int seatAllocation(string reserve_id, int seats_request){

    int row_counter=1;
    bool upflag=true;
    int rows_itr=rows-1;
    while(rows_itr>=0 && seats_request>0){
        if(availableSeatsRow[rows_itr]>=seats_request){         // accomadate group of customers together in a row
            int min_col=19;
            for(int col=19;col>=0 && seats_request>0 ; col--){
                if(theatreSeats[rows_itr][col]==" "){
                    theatreSeats[rows_itr][col]=reserve_id;
                    if(reserved_seatsMap.find(reserve_id)!=reserved_seatsMap.end()){
                        char ch = (rows_itr+65);
                        string seat_number= ch+ to_string(col+1);
                        min_col = min(min_col,col);
                        reserved_seatsMap[reserve_id].push_back(seat_number);
                    }else{
                        vector<string> seats_vt;
                        char ch = (rows_itr+65);
                        string seat_number= ch+ to_string(col+1);
                        seats_vt.push_back(seat_number);
                        min_col = min(min_col,col);
                        reserved_seatsMap[reserve_id]=seats_vt;
                    }
                    availableSeatsRow[rows_itr]--;
                    totalSeats--;
                    seats_request--;
                }
            } 
            // buffer three seats for customer safety
            int count=3;        
            while(min_col>=0 && count>0){
                min_col--;
                theatreSeats[rows_itr][min_col]="DNS";
                count--;
                availableSeatsRow[rows_itr]--;
                totalSeats--;
            }

        }
        rows_itr--;
    }
    if(seats_request==0) return 0;
    else{
        row_counter=1;
        upflag=true;
        rows_itr = rows - 1;
        while(rows_itr>=0 ){                                // in case no consecutive seats allocate cusstomers in the available seats in anywhere in hall 
                                                            // for maximum utilization of theater.
            if(availableSeatsRow[rows_itr]>0){
                int min_col = 19;
                for(int cols=19;cols>=0 && theatreSeats[rows_itr][cols]==" ";cols--){
                    theatreSeats[rows_itr][cols]=reserve_id;
                    if(reserved_seatsMap.find(reserve_id)!=reserved_seatsMap.end()){
                        char ch = (rows_itr+65);
                        string seat_number= ch+ to_string(cols+1);
                        min_col = min(min_col,cols);
                        reserved_seatsMap[reserve_id].push_back(seat_number);
                    }else{
                        vector<string> seats_vt;
                        char ch = (rows_itr+65);
                        min_col = min(min_col,cols);
                        string seat_number= ch+ to_string(cols+1);
                        seats_vt.push_back(seat_number);
                        reserved_seatsMap[reserve_id]=seats_vt;
                    }
                    availableSeatsRow[rows_itr]--;
                    totalSeats--;
                    seats_request--;

                }
                // buffer three seats for customer safety
                int count=3;
                while(min_col>=0 && count>0){
                    theatreSeats[rows_itr][min_col]="DNS";
                    min_col--;
                    count--;
                    availableSeatsRow[rows_itr]--;
                    totalSeats--;
                }
            }
            rows_itr--;
        }
        return 0;
    }
}

int seatReservation(string reservation){
    vector<string> reserve_tokens = tokenize(reservation, ' ');
    string reserve_id = reserve_tokens[0];
    int seats_request = stoi(reserve_tokens[1]);
    int group_seats = seats_request;

    int output;

    if(seats_request>0){
        if(totalSeats>=seats_request){
            reserveID.push_back(reserve_id);
            if(group_seats>seatsEachRow){
                while(group_seats>20){
                    output = seatAllocation(reserve_id,20);
                    group_seats-=20;
                }
                output = seatAllocation(reserve_id,group_seats);
            }else {
                output = seatAllocation(reserve_id,group_seats);
            }
            return 0;                                   //"Grab a popcorn move tickets reserved!! ";
        }else return 1;                                 //"Sorry no tickets for this show book for next show";
    }else return -1;                                    //   "Invaild request";
}

int main(int argc, char** argv)
{
   
    vector<string> inputString;
    string path_inputFile ;

    if(argc>1){
        path_inputFile = argv[argc-1];
    }else{
        cout<<"Enter path of input file :";
        cin>>path_inputFile;
    }
    
    // read the contents of input file and write to vector of string to pass 
    string inputline;
    ifstream inputfile (path_inputFile);
    if(inputfile.is_open()){
        while(getline(inputfile,inputline)){
            inputString.push_back(inputline);
        }
    } else cout<<"unable to open file"<<endl;
    
    for(int i=0;i<inputString.size();i++){
        int output = seatReservation(inputString[i]);
        if(output ==1)
            cout<< "Sorry no tickets for this show book for next show"<<endl;
        if(output ==-1)
            cout<< "Invaild request"<<endl;
       
    }

    //output file path
    char curr_directory_path[256];
    getcwd(curr_directory_path,256);

    const char * arr = curr_directory_path;
    string curr_directory(curr_directory_path);
    curr_directory +="/output.txt";

    // cout<<(curr_directory)<<endl;
    // open output file and write the seat number for reservation from map
    ofstream outputfile (curr_directory);
    map<string, vector<string> > ::iterator it = reserved_seatsMap.begin();
    
    //write seats numbers to out put file
    if(outputfile.is_open()){
        for(int i=0;i<reserveID.size();i++){
            string reserve_id = reserveID[i];
            vector<string> seats= reserved_seatsMap[reserveID[i]];
            string seats_string=reserve_id+" "+seats[0];
            for(int i=1; i<seats.size();i++){
                seats_string = seats_string+","+seats[i];
            }
            outputfile<<seats_string<<endl;
        }
        outputfile.close();
    }else cout<<"error in opening file"<<endl;        
    return 0;
}



