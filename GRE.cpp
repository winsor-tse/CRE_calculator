/*
Winsor Tse
GRE calculator
data.json is the input file
output.json is the output file
*/

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>      
#include <math.h>       /* pow */
#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

//precondition: takes in an array with Total_cashflow, integer of Hold_period, and Discount_Rate
//postcondition: returns the Property_Present_Value
int Property_Present_Value (int a[], int size, double discount);

//precondition: takes in input data from JSON
//postcondition: returns true or false
bool Inside_Bounds(double Net, double growth, double exit, int hold, double discount, double cost);

int main() {
    // read in the json file
    ifstream i("data.json");
    //create unitiialized json object
    json j;
    // initialize json object with what was read from file
    i >> j;

    // uses at to access fields from json object
    double json_NOI_Growth = j.at("NOI_Growth");
    double json_NOI = j.at("Net_Operating_Income");
    double json_Exit = j.at("Exit_Cap_Rate");
    int json_Hold = j.at("Hold_Period");
    double json_discount = j.at("Discount_Rate");
    double json_cost = j.at("Cost_of_Sales");

    //reformulates to decimals
    double NOI = json_NOI; 
    double NOI_Growth = json_NOI_Growth/100;
    double Exit_Cap_Rate = json_Exit/100;
    int Hold_period = json_Hold;
    double Discount_Rate = json_discount/100;
    double Cost_Of_Sales = json_cost/100;

    //checks for bounds
    if(!Inside_Bounds(NOI, NOI_Growth, Exit_Cap_Rate, Hold_period, Discount_Rate, Cost_Of_Sales)){
        cout<<"Wrong input"<<endl;
        exit(0);
    }

    int Total_cashflow[Hold_period];

    json val;
    ofstream file("output.json");

    //loops to the maximum amount of years
    for(int i=1; i<=30; i++){
        cout<<"Year "<<i<<": "<<endl;
        cout<<"NOI Growth "<<NOI_Growth*100<<"%"<<endl;
        cout<<"NOI $"<<NOI<<endl;

        //Loop hits the hold period
        if(Hold_period==i){
            //NOI of year ahead divide by Exit Cap rate
            double NOI_yr_ahead = NOI * (1+NOI_Growth);
            double Residual = NOI_yr_ahead / Exit_Cap_Rate;

            cout<<"Residual Value is $"<<Residual<<endl;
            val["Residual_Value"] = Residual;

            double Cost = Residual*Cost_Of_Sales;

            cout<<"Cost of Sales is $"<<Cost<<endl;
            val["Cost_of_Sales"] = Cost;

            cout<<"Cash Flow is $"<<NOI+Residual-Cost<<endl;
            Total_cashflow[i-1]=NOI+Residual-Cost;
        }
        //store all cahsflow into an array
        else if(Hold_period>i){
            Total_cashflow[i-1]=NOI;
        }
        else{
            cout<<"Cash Flow is $"<<NOI<<endl;
        }
        NOI = NOI * (1+NOI_Growth);
        cout<<endl;
    }


    int out = Property_Present_Value(Total_cashflow, Hold_period, Discount_Rate);
    cout<<"Property_Present_Value is:"<<out;

    val["Property_Present_Value"] = out;

    //converts array to vector to output into JSON file
    vector<int> dest;
    for (int i: Total_cashflow) {
        dest.push_back(i);
    }
        val["CashFlow"] = dest;
        file << val<< endl;

    return 0;
}

int Property_Present_Value (int a[], int size, double discount){
    int Value = 0;
    //calculates based on formula
    for(int i=0; i<size; i++){
        Value += a[i] / pow(1+discount,i+1);
    }
    return Value;
}


bool Inside_Bounds (double Net, double growth, double exit, int hold, double discount, double cost){
    //returns false if not inside bounds
    if(Net>=10000000 || Net<0 || growth<-1 || growth>1 || exit<0 || exit>1 || hold>=31 || hold<=0 || 
        discount>1 || discount<0 || cost>1 || cost<0){
        return false;
    }
    
    return true;
}