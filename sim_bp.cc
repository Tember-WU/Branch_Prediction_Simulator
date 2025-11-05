#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iomanip>
#include "sim_bp.h"
using namespace std;


BIMODAL::BIMODAL(unsigned long int num_bits)
        : num_index_bits(num_bits)
{
    if(num_bits != 0){
        unsigned long int num_sets = 1 << num_bits;
        counters.resize(num_sets);
        for(unsigned long int i=0; i<num_sets; i++){
            counters[i] = 2;
        }
    }
    prediction_outcome = false;
    num_prediction = 0;
    num_misprediction = 0;
}

void BIMODAL::read_branch(unsigned long int addr, char actual_outcome){
    unsigned long int index = (addr >> 2) & ((1 << num_index_bits) - 1);
    prediction_outcome = make_prediction(index);
    if(actual_outcome == 't'){
        increment_counter(index);
        if(prediction_outcome == false){
            num_misprediction ++;
        }
    }else if(actual_outcome == 'n'){
        decrement_counter(index);
        if(prediction_outcome == true){
            num_misprediction ++;
        }
    }
}

bool BIMODAL::make_prediction(unsigned long int index){
    num_prediction ++;
    if(counters[index] >= 2){
        return true;
    }else{
        return false;
    }
}

void BIMODAL::increment_counter(unsigned long int index){
    counters[index] ++;
    if(counters[index] > 3){
        counters[index] = 3;
    }
}

void BIMODAL::decrement_counter(unsigned long int index){
    counters[index] --;
    if(counters[index] < 0){
        counters[index] = 0;
    }
}

void BIMODAL::print_content(){
    for (int i = 0; i < (int)counters.size(); i++) {
        printf(" %-3d%d\n", i, counters[i]);
    }
}

bool BIMODAL::get_one_prediction(unsigned long int addr){
    unsigned long int index = (addr >> 2) & ((1 << num_index_bits) - 1);
    if(counters[index] >= 2){
        return true;
    }else{
        return false;
    }
}

void BIMODAL::update_one_prediction(unsigned long int addr, bool predicted_outcome, char actual_outcome){
    unsigned long int index = (addr >> 2) & ((1 << num_index_bits) - 1);
    if(actual_outcome == 't'){
        increment_counter(index);
    }else if(actual_outcome == 'n'){
        decrement_counter(index);
    }
}



GSHARE::GSHARE(unsigned long int m, unsigned long int n)
        : num_index_bits(m), num_GBH_bits(n)
{
    if(m != 0){     // n could equal to 0, as this gshare works the same with bimodal
        unsigned long int num_sets = 1 << m;
        counters.resize(num_sets);
        for(unsigned long int i=0; i<num_sets; i++){
            counters[i] = 2;
        }
    }

    GBH = 0;
    prediction_outcome = false;
    num_prediction = 0;
    num_misprediction = 0;
}

void GSHARE::read_branch(unsigned long int addr, char actual_outcome){
    unsigned long int index;
    if(num_GBH_bits == 0){      // this gshare predictor works the same with bimodal
        index = (addr >> 2) & ((1 << num_index_bits) - 1);
    }else if(num_GBH_bits > 0){
        unsigned long int uppermost_PC = (addr >> (num_index_bits - num_GBH_bits + 2)) & ((1 << num_GBH_bits) - 1);
        unsigned long int n_bit_XOR = GBH ^ uppermost_PC;
        unsigned long int lowermost_PC = (addr >> 2) & ((1 << (num_index_bits - num_GBH_bits)) - 1);
        index = (n_bit_XOR << (num_index_bits - num_GBH_bits)) + lowermost_PC;
    }

    prediction_outcome = make_prediction(index);
    if(actual_outcome == 't'){
        increment_counter(index);
        if(prediction_outcome == false){
            num_misprediction ++;
        }
    }else if(actual_outcome == 'n'){
        decrement_counter(index);
        if(prediction_outcome == true){
            num_misprediction ++;
        }
    }

    Update_GBH(actual_outcome);
}

bool GSHARE::make_prediction(unsigned long int index){
    num_prediction ++;
    if(counters[index] >= 2){
        return true;
    }else{
        return false;
    }
}

void GSHARE::increment_counter(unsigned long int index){
    counters[index] ++;
    if(counters[index] > 3){
        counters[index] = 3;
    }
}

void GSHARE::decrement_counter(unsigned long int index){
    counters[index] --;
    if(counters[index] < 0){
        counters[index] = 0;
    }
}

void GSHARE::Update_GBH(char actual_outcome){
    if(num_GBH_bits <= 0){
        return;
    }
    if(actual_outcome == 't'){
        GBH = (GBH >> 1) + (1 << (num_GBH_bits-1));
    }else if(actual_outcome == 'n'){
        GBH = GBH >> 1;
    }
}

void GSHARE::print_content(){
    for (int i = 0; i < (int)counters.size(); i++) {
        printf(" %d\t%d\n", i, counters[i]);
    }
}

bool GSHARE::get_one_prediction(unsigned long int addr){
    unsigned long int index;
    if(num_GBH_bits == 0){      // this gshare predictor works the same with bimodal
        index = (addr >> 2) & ((1 << num_index_bits) - 1);
    }else if(num_GBH_bits > 0){
        unsigned long int uppermost_PC = (addr >> (num_index_bits - num_GBH_bits + 2)) & ((1 << num_GBH_bits) - 1);
        unsigned long int n_bit_XOR = GBH ^ uppermost_PC;
        unsigned long int lowermost_PC = (addr >> 2) & ((1 << (num_index_bits - num_GBH_bits)) - 1);
        index = (n_bit_XOR << (num_index_bits - num_GBH_bits)) + lowermost_PC;
    }
    if(counters[index] >= 2){
        return true;
    }else{
        return false;
    }
}

void GSHARE::update_one_prediction(unsigned long int addr, bool predicted_outcome, char actual_outcome){
    unsigned long int index;
    if(num_GBH_bits == 0){      // this gshare predictor works the same with bimodal
        index = (addr >> 2) & ((1 << num_index_bits) - 1);
    }else if(num_GBH_bits > 0){
        unsigned long int uppermost_PC = (addr >> (num_index_bits - num_GBH_bits + 2)) & ((1 << num_GBH_bits) - 1);
        unsigned long int n_bit_XOR = GBH ^ uppermost_PC;
        unsigned long int lowermost_PC = (addr >> 2) & ((1 << (num_index_bits - num_GBH_bits)) - 1);
        index = (n_bit_XOR << (num_index_bits - num_GBH_bits)) + lowermost_PC;
    }

    if(actual_outcome == 't'){
        increment_counter(index);
    }else if(actual_outcome == 'n'){
        decrement_counter(index);
    }
    // no matter select bimodal or gshare, gshare's GHB is always needed to be updated
    // so I do not update there, instead updating it individually after each hybrid prediction
    // Update_GBH(actual_outcome);
}


void increment_chooser_counter(vector<int>& chooser_table, unsigned long int chooser_table_index){
    chooser_table[chooser_table_index] ++;
    if(chooser_table[chooser_table_index] > 3){
        chooser_table[chooser_table_index] = 3;
    }
}

void decrement_chooser_counter(vector<int>& chooser_table, unsigned long int chooser_table_index){
    chooser_table[chooser_table_index] --;
    if(chooser_table[chooser_table_index] < 0){
        chooser_table[chooser_table_index] = 0;
    }
}



/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    params.K = 0;
    params.M1 = 0;
    params.M2 = 0;
    params.N = 0;
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n %s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n %s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    

    BIMODAL bimodal_predictor(params.M2);
    GSHARE gshare_predictor(params.M1, params.N);

    bool bimodal_outcome = false;
    bool gshare_outcome = false;
    bool chooer_outcome = false;
    double miss_rate = 0;
    int num_prediction = 0;
    int num_misprediction = 0;

    vector<int> chooser_table;
    if(params.K != 0){
        unsigned long int table_sets = 1 << params.K;
        chooser_table.resize(table_sets);
        for(unsigned long int i=0; i<table_sets; i++){
            chooser_table[i] = 1;
        }
    }

    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        outcome = str[0];

        if(strcmp(params.bp_name, "bimodal") == 0){
            bimodal_predictor.read_branch(addr, outcome);
        }else if(strcmp(params.bp_name, "gshare") == 0){
            gshare_predictor.read_branch(addr, outcome);
        }else if(strcmp(params.bp_name, "hybrid") == 0){
            bimodal_outcome = bimodal_predictor.get_one_prediction(addr);
            gshare_outcome = gshare_predictor.get_one_prediction(addr);

            unsigned long int chooser_table_index = (addr >> 2) & ((1 << params.K) - 1);
            if(chooser_table[chooser_table_index] >= 2){
                chooer_outcome = gshare_outcome;
                gshare_predictor.update_one_prediction(addr, chooer_outcome, outcome);
            }else{
                chooer_outcome = bimodal_outcome;
                bimodal_predictor.update_one_prediction(addr, chooer_outcome, outcome);
            }
            num_prediction ++;
            gshare_predictor.Update_GBH(outcome);

            if(outcome == 't'){
                if(chooer_outcome == false){
                    num_misprediction ++;
                }
                if(gshare_outcome == true && bimodal_outcome == false){ // gshare correct but bimodal incorrect
                    increment_chooser_counter(chooser_table, chooser_table_index);
                }else if(gshare_outcome == false && bimodal_outcome == true){   // bimodal correct but gshare incorrect
                    decrement_chooser_counter(chooser_table, chooser_table_index);
                }
            }else if(outcome == 'n'){
                if(chooer_outcome == true){
                    num_misprediction ++;
                }
                if(gshare_outcome == false && bimodal_outcome == true){ // gshare correct but bimodal incorrect
                    increment_chooser_counter(chooser_table, chooser_table_index);
                }else if(gshare_outcome == true && bimodal_outcome == false){   // bimodal correct but gshare incorrect
                    decrement_chooser_counter(chooser_table, chooser_table_index);
                }
            }
        }
    }

    
    if(strcmp(params.bp_name, "bimodal") == 0){
        num_prediction = bimodal_predictor.num_prediction;
        num_misprediction = bimodal_predictor.num_misprediction;
    }else if(strcmp(params.bp_name, "gshare") == 0){
        num_prediction = gshare_predictor.num_prediction;
        num_misprediction = gshare_predictor.num_misprediction;
    }
    miss_rate = (double)num_misprediction / (double)num_prediction * 100;

    cout << "OUTPUT" << endl;
    cout << " number of predictions:    " << num_prediction << endl;
    cout << " number of mispredictions: " << num_misprediction << endl;
    cout << " misprediction rate:       " << fixed << setprecision(2) << miss_rate << "%" << endl;

    if(strcmp(params.bp_name, "bimodal") == 0){
        cout << "FINAL BIMODAL CONTENTS" << endl;
        bimodal_predictor.print_content();
    }else if(strcmp(params.bp_name, "gshare") == 0){
        cout << "FINAL GSHARE CONTENTS" << endl;
        gshare_predictor.print_content();
    }else if(strcmp(params.bp_name, "hybrid") == 0){
        cout << "FINAL CHOOSER CONTENTS" << endl;
        for(int i = 0; i < (int)chooser_table.size(); i++){
            printf(" %d\t%d\n", i, chooser_table[i]);
        }
        cout << "FINAL GSHARE CONTENTS" << endl;
        gshare_predictor.print_content();
        cout << "FINAL BIMODAL CONTENTS" << endl;
        bimodal_predictor.print_content();
    }

    return 0;
}
