#ifndef SIM_BP_H
#define SIM_BP_H

#include <vector>

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;



class BIMODAL{
public:
    explicit BIMODAL(unsigned long int num_bits);

    void read_branch(unsigned long int addr, char actual_outcome);
    bool get_one_prediction(unsigned long int addr);
    void update_one_prediction(unsigned long int addr, bool predicted_outcome, char actual_outcome);
    void print_content();

    bool prediction_outcome;
    int  num_prediction;
    int  num_misprediction;

private:
    bool make_prediction(unsigned long int index);
    void increment_counter(unsigned long int index);
    void decrement_counter(unsigned long int index);

    unsigned long int num_index_bits;
    std::vector<int>  counters;
};



class GSHARE{
public:
    explicit GSHARE(unsigned long int m, unsigned long int n);

    void read_branch(unsigned long int addr, char actual_outcome);
    bool get_one_prediction(unsigned long int addr);
    void update_one_prediction(unsigned long int addr, bool predicted_outcome, char actual_outcome);
    void Update_GBH(char actual_outcome);
    void print_content();

    bool prediction_outcome;
    int  num_prediction;
    int  num_misprediction;

private:
    bool make_prediction(unsigned long int index);
    void increment_counter(unsigned long int index);
    void decrement_counter(unsigned long int index);

    unsigned long int num_index_bits;  // m
    unsigned long int num_GBH_bits;    // n
    unsigned long int GBH;
    std::vector<int>  counters;
};



#endif
