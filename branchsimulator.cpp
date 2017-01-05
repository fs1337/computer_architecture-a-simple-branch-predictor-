/*
Taken Predictor
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;

#define ST  3   // strongly taken(11)
#define SNT 0   // strongly not taken(00)
#define WT  2   // weakly taken(10)
#define WNT 1   // weakly not taken(01)

struct config{
    int mbits;
};

class SaturateCounter{
public:
    SaturateCounter(int m = 12)
    {
        m_bits = m;
        saturation_counter.resize(pow(2,m_bits));
    }
    
    void initialize()
    {
        bitset<2> predict_start = bitset<2> (ST);
        for(int i = 0; i < pow(2,m_bits); i ++)
        {
            saturation_counter[i] = predict_start;
        }

    }
    
    void test()
    {
        for(int i = 0; i < pow(2,m_bits); i ++)
        {
            printf("saturation_counter[%d] %d\n", i, saturation_counter[i].to_ulong());;
        }
        printf("m_bits %d\n", m_bits);
        printf("total counter size %d\n", saturation_counter.size());
    }
    
    int get_m_bits()
    {
        return m_bits;
    }
    
    bitset<2> get_s_counter_i(int i)
    {
        return saturation_counter[i];
    }
    void set_s_counter_i(int i, bitset<2> counter)
    {
        saturation_counter[i] = counter;
    }
    
private:
    int m_bits;
    vector< bitset<2> > saturation_counter;
};

bitset<2> state_machine(bitset<2> s_counter, int take_state);
int predict(bitset<2> s_counter);
int number;

int main(int argc, char* argv[]){
    
    config mbit_config;
    ifstream LSB_params;
    LSB_params.open(argv[1]);
    while(!LSB_params.eof())  // read config file, and get the value of m
    {
      LSB_params>>mbit_config.mbits;
    }
    
    // the value of m: m<=20, or else just exit
    if(mbit_config.mbits > 20)
    {
        exit(1);
    }
  
   // Implement by you: 
   // initialize the 2^m 2-bits saturation counters
    SaturateCounter SC1(mbit_config.mbits);//(mbit_config.mbits);
    SC1.initialize();
    //SC1.test();
    
   
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    
    string line;
    string taken_type;      // the Read/Write access type from the memory trace;
    string xaddr;           // the address from the memory trace store in hex;
    unsigned int addr;      // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr;  // the address from the memory trace store in the bitset;
    
    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache
            istringstream iss(line); 
            if (!(iss >> xaddr >> taken_type)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
            int branch_predicted_state = 1;
            int branch_actual_state = -1;
            int index = 0;
           
           //get index
            bitset<32> reference_index = bitset<32> (0);
            for(int i = 0; i < SC1.get_m_bits(); i ++)
            {
                reference_index.set(i);
            }
            bitset<32> address_index = reference_index & accessaddr;
            index = address_index.to_ulong();
            
            //get predict value
            bitset<2> current_s_counter = SC1.get_s_counter_i(index);
            branch_predicted_state = predict(current_s_counter);
            
            //get current branch state
            if (taken_type.compare("0")==0)
            {
                branch_actual_state = 0;
            }else if(taken_type.compare("1")==0)
            {
                branch_actual_state = 1;
            }
            
            number ++;
            
            if(index == 0x54a)
            {
                printf("%d %d\n", current_s_counter.to_ulong(), number);
            }
        
            //update saturation counter
            bitset<2> updated_s_counter = state_machine(current_s_counter, branch_actual_state);
            SC1.set_s_counter_i(index, updated_s_counter);
 
            
            tracesout<< branch_predicted_state <<endl;
        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";
   
    return 0;
}

bitset<2> state_machine(bitset<2> s_counter, int take_state)
{
    int state = s_counter.to_ulong();
    bitset<2> new_counter = bitset<2> (ST);
    switch(state)
    {
        case SNT:
        {
            if(take_state == 1)
            {
                new_counter = bitset<2> (WNT);
            }
            
            if(take_state == 0)
            {
                new_counter = bitset<2> (SNT);
            }
        }
            break;
        case WNT:
        case WT:
        {
            if(take_state == 1)
            {
                new_counter = bitset<2> (ST);
            }
            
            if(take_state == 0)
            {
                new_counter = bitset<2> (SNT);
            }
        }
            break;
        case ST:
        {
            if(take_state == 1)
            {
                new_counter = bitset<2> (ST);
            }
            
            if(take_state == 0)
            {
                new_counter = bitset<2> (WT);
            }
        }
            break;
        default:
            break;
    }
    return new_counter;
}

int predict(bitset<2> s_counter)
{
    int state = s_counter.to_ulong();
    int predicted_state = 1;
    
    if(state == ST || state == WT)
    {
        predicted_state = 1;
    }
    
    if(state == SNT || state == WNT)
    {
        predicted_state = 0;
    }
    
    return predicted_state;
}


