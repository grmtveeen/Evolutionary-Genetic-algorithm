//MATVEEV IGOR

#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

//PARAMETERS FOR CONTROL CALCULATE
#define SIZE 100        //SIZE OF POPULATION
#define CYCLES 500000   //LIMIT OF CYCLES (MAX EVOLUTION TIME)
#define MAX 1000        //UPPER BOUND OF THE RANGE FOR THE SOURCE COEFFICIENTS
#define MIN 0           //LOWER BOUND OF THE RANGE FOR THE RESEARCH COEFFICIENTS

//FUNCTION TO CALCULATE POLYNOMIAL VALUE
double f(double a0, double a1, double a2, double a3, double a4, double x){
    double X0,X1,X2,X3,X4;
    X0=a0;
    X1=a1*x;
    X2=a2*x*x;
    X3=a3*x*x*x;
    X4=a4*x*x*x*x;
    return X0+X1+X2+X3+X4;
}

//CLASS FOR STORING COORDINATES
class data{
    public:
        double x,y;
        data();
        data(double xx, double yy);
        ~data();
        data &operator= (const data &src);
        friend ostream &operator<< (ostream &os, const data &src);
};

data::data(){
}

data::data(double xx, double yy){
    x=xx;
    y=yy;
}

data::~data(){
}

data &data::operator=(const data &src){
    if(&src==this) return *this;
    x=src.x;
    y=src.y;
    return *this;
}

ostream &operator<< (ostream &os, const data &src){
    os<<'('<<src.x<<','<<src.y<<')';
    return os;
}

// "PEOPLE" FROM POPULATION
class subject {
    private:
        double A[5];
    public:
        double D;
        subject();
        ~subject();

        void give_deviation(data coordinates[]);        //THIS METHOD FOR POPULATION SUBJECT DETERMINES IT'S RMS DEVIATION FROM THE DATA IN FILE
        subject (const subject &src);
        subject &operator= (const subject &src);
        subject operator+ (const subject &src) const;   //PAIRING BETWEEN TWO PARENTS TO GET NEW SUBJECT (CHILDREN)
        bool operator< (const subject &src) const;
        bool operator> (const subject &src) const;
        bool operator== (const subject &src) const;
        bool operator!= (const subject &src) const;

        friend ostream &operator<< (ostream &os, const subject &src);
};

//RANDOM INITIALIZATION
subject::subject(){
    for (int i=0; i<5; i++){
        A[i]=(rand()%MAX+MIN);
        if(rand()%2+0) A[i]*=(-1);
    }
}

subject::~subject(){
}

//DEVIATION CHECKING
void subject:: give_deviation(data coordinates[]){
    D=0;
    for(int i=0; i<SIZE; i++){
       D+=(coordinates[i].y-f(A[0],A[1],A[2],A[3],A[4],coordinates[i].x))*(coordinates[i].y-f(A[0],A[1],A[2],A[3],A[4],coordinates[i].x));
       cout << fixed;
       cout.precision(1);
    }
}

subject::subject (const subject &src){
    for (int i=0; i<5; i++){
        A[i]=src.A[i];
    }
}

subject &subject::operator=(const subject &src){
    for (int i=0; i<5; i++){
        A[i]=src.A[i];
    }
    D = src.D;
    return *this;
}

//CREATION OF CHILDREN
subject subject::operator+ (const subject &src) const {
    subject RES;
    int r;
    for (int i=0; i<5; i++){
        r=rand()%100+0;
        if (r<45){
            RES.A[i]=((*this).A[i]);
        }
        else {
            if(r<90){
                RES.A[i]=(src.A[i]);
            }
            else {
                RES.A[i]=(rand()%MAX+MIN);
                if(rand()%2+0) RES.A[i]*=(-1);
            }
        }
    }
    return RES;
}

bool subject :: operator<(const subject &src) const{
    return D < src.D;
}

bool subject :: operator> (const subject &src) const {
    return D > src.D;
}

bool subject :: operator== (const subject &src) const {
    if((A[0]==src.A[0])&&(A[1]==src.A[1])&&(A[2]==src.A[2])&&(A[3]==src.A[3])&&(A[4]==src.A[4])) return true;
    return false;
}

bool subject :: operator!= (const subject &src) const{
    return !((*this)==src);
}

ostream &operator<< (ostream &os, const subject &src){
    os<<'('<<src.A[0]<<','<<src.A[1]<<','<< src.A[2]<<','<< src.A[3]<<','<< src.A[4]<<") deviation = "<<src.D;
    return os;
}

//SAVE COORDINATES FROM FILE
void write (data coordinates[]){
    data XY;
    ifstream file;
    file.open("data.txt", ios_base:: in | ios_base:: binary);
    if(!file){
        cout<<"Error. File didn't open."<<endl; exit(2);
    }
    for(int i=0;!file.eof();i++){
        file>>XY.x;
        file>>XY.y;
        coordinates[i]=XY;
    }
    file.close();
}

int main(){
    int cnt, PROCENT10 = (10*SIZE)/100, PROCENT90 = (90*SIZE)/100;
    subject temp, RESULT;

    //SAVE COORDINATES FROM FILE
    data coordinates[SIZE];
    write(coordinates);

    //CREATE RANDOM FIRST POPULATION AND GET DIVIATION
    subject population[SIZE];
    for(int i=0; i<SIZE; i++){
        population[i].give_deviation(coordinates);
    }

    //START EVOLUTION
    for(int n=0; n<CYCLES; n++){


        //SORT START
        for (int i = 0; i < SIZE - 1; i++) {
            for (int j = 0; j < SIZE - i - 1; j++) {
                if (population[j]>population[j+1]) {
                    temp = population[j];
                    population[j] = population[j+1];
                    population[j+1] = temp;
                }
            }
        }
        //SORT STOP


        //INFORMATION THAT WE HAVE ALREDY DONE ANOTHER 1000 CYCLES
        if((((double)n)/(double)1000)==(n/1000))cout<<population[0]<<' '<<n<<endl;


        //IT'S CHEK ON RIGHT ANSVER
        if((int)(population[0].D)==0){
            cout<<"\t\t\tFinish!"<<endl;
            break;
        }


        //FIRST 10% GO TO THE NEW POPULATION
        subject new_population[SIZE];
        for(int i=0; i<PROCENT10; i++){
            new_population[i] = population[i];
        }


        //90% SUBJECTS ARE CREATED FROM THE FIRST 50% SUBJECTS
        for(int i=0; i<PROCENT90; i++){
            RESULT = population[rand()%(SIZE/2)+0]+population[rand()%(SIZE/2)+0];
            RESULT.give_deviation(coordinates);
            new_population[i + PROCENT10] = RESULT;
        }

        //INITIALIZATION AGAIN
        for(int i=0; i<SIZE; i++){
            population[i]=new_population[i];
        }

        //COUNTER FOR INFORMATION
        cnt = n;
    }

    //INFORMATION ABOUT RESUL POPULATION
    cout<<"==================== CYCLES: "<<cnt<<" ====================\n\t\t   RESUL POPULATION:"<<endl;
    for(int i=0; i<SIZE; i++) cout<<population[i]<<endl;


    return 0;
}

