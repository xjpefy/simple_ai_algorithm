#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <random>
#include <mutex>
#include <atomic>

using namespace std;

typedef unsigned int ID;
typedef unsigned int INDEX;



atomic<int> Code_Time = 0;


char IF_SELF_TURN_OFF = 1;
char IF_SELF_COMMUNIATE = 0;
char IF_SELF_CONNECT = 0;
char IF_SELF_WORKING = 0;


char IF_NEED_COMMUNIATE = 1;
char IF_NEED_WORKING = 1;
char IF_NEED_CAPTURE = 0;
char IF_NEED_ACTION = 0;

atomic<ID> NEWIST_USEFUL_ID = 1;
atomic<int> CURRENT_MODEL_TIME = 1;
atomic<unsigned int> ALL_NUM_OF_CH = 0;
atomic<unsigned int> LAST_PACK_NUMBER = 0;

atomic<int> FREE_ID_NUM = 0;
vector<ID> FREE_ID_LIST;
atomic<int> Free_Id_Ocuppy(0);

atomic<int> CURRENT_SYSTEM_MEMORY;


char time_curr = 0;
atomic<int> use_time[4] = {0, 0, 0, 0};
atomic<int> time_node_occupy[4] = {0, 0, 0, 0};
atomic<int> node_total_occupy = 0;


struct state_record_file
{
    ID newist_useful_id = 1;
    int all_num_of_ch = 0;
    int last_pack_num = 0;

    char time_curr = 0;
    int time_node_occupy[4] = {0, 0, 0, 0};
    int use_time[4] = {0, 0, 0, 0};

    int current_model_time = 100;

    long long node_total_occupy = 0;
    long long size_of_pre_load_file = 0;
};



void load_model_record()
{
    FILE *fp = fopen("Record_Model_State.rec", "r");

    state_record_file rf;
    state_record_file *ptr = &rf;
    if (fp)
    {
        fread(ptr, sizeof(state_record_file), 1, fp);

        fseek(fp, sizeof(state_record_file), 1);
        fread(&FREE_ID_NUM, sizeof(int), 1, fp);

        FREE_ID_LIST.resize(FREE_ID_NUM);
        fseek(fp, sizeof(int), 1);
        fread(FREE_ID_LIST.data(), sizeof(int) * FREE_ID_NUM, 1, fp);

        fclose(fp);
    }
    
    NEWIST_USEFUL_ID = rf.newist_useful_id;
    CURRENT_MODEL_TIME = rf.current_model_time;
    ALL_NUM_OF_CH = rf.all_num_of_ch;
    LAST_PACK_NUMBER = rf.last_pack_num;

    time_curr = rf.time_curr;
    time_node_occupy[0] = rf.time_node_occupy[0];
    time_node_occupy[1] = rf.time_node_occupy[1];
    time_node_occupy[2] = rf.time_node_occupy[2];
    time_node_occupy[3] = rf.time_node_occupy[3];
    use_time[0] = rf.use_time[0];
    use_time[1] = rf.use_time[1];
    use_time[2] = rf.use_time[2];
    use_time[3] = rf.use_time[3];
    node_total_occupy = rf.node_total_occupy;
}


void save_record_model()
{
    FILE *fp = fopen("Record_Model_State.rec", "w");
    state_record_file rf;
    state_record_file *ptr = &rf;

    
    rf.newist_useful_id = NEWIST_USEFUL_ID;
    rf.current_model_time = CURRENT_MODEL_TIME;
    rf.all_num_of_ch = ALL_NUM_OF_CH;
    rf.last_pack_num = LAST_PACK_NUMBER;

    rf.time_curr = time_curr;
    rf.time_node_occupy[0] = time_node_occupy[0];
    rf.time_node_occupy[1] = time_node_occupy[1];
    rf.time_node_occupy[2] = time_node_occupy[2];
    rf.time_node_occupy[3] = time_node_occupy[3];
    rf.use_time[0] = use_time[0];
    rf.use_time[1] = use_time[1];
    rf.use_time[2] = use_time[2];
    rf.use_time[3] = use_time[3];
    rf.node_total_occupy = node_total_occupy;

    fwrite(ptr, sizeof(state_record_file), 1, fp);

    fseek(fp, sizeof(state_record_file), 1);
    fwrite(&FREE_ID_NUM, sizeof(int), 1, fp);

    fseek(fp, sizeof(int), 1);
    fwrite(FREE_ID_LIST.data(), sizeof(int) * FREE_ID_NUM, 1, fp);

    fclose(fp);
}

struct Pack_Inside
{
    int attribute_item[256];
};
// 256*4

vector<Pack_Inside> Neuro_Pack_Storage;
vector< atomic<char> > Neuro_Pack_Read_Occupy;
vector< atomic<char> > Neuro_Pack_Write_Occupy;
atomic<int> Occupy_Neuro_Pack_Storage(0);

unordered_map<ID, INDEX> ID_Find_Pack_Index;
vector<INDEX> Free_1_Pack_Index_List;
vector<INDEX> Free_2_Pack_Index_List;
vector<INDEX> Free_4_Pack_Index_List;
vector<INDEX> Free_8_Pack_Index_List;
atomic<int> Occupy_Neuro_Pack_Record_Storage(0);

long long Pack_Limit = 10 * 10000;
atomic<long long> Free_Pack_Num(Pack_Limit - 1);
atomic<long long> Current_Pack_Num(1);



struct Pack_Record
{
    char is_use = 1;
    char pack_state = 1;
    char pack_size = 0;
    char time_curr = 0;

    ID pack_id = 0;

    int use_time[4] = {0, 0, 0, 0};
    int history_use_num[4] = {0, 0, 0, 0};
};
// 10*4

vector<Pack_Record> Neuro_Pack_Record_Storage;
vector< atomic<char> > Neuro_Pack_Record_Occupy;


void load_pre_load_file()
{
    
    Neuro_Pack_Storage.reserve(Pack_Limit);
    Neuro_Pack_Storage.resize(1 + LAST_PACK_NUMBER);
    
    Neuro_Pack_Record_Storage.reserve(Pack_Limit);
    Neuro_Pack_Record_Storage.resize(1 + LAST_PACK_NUMBER);
    
    Neuro_Pack_Read_Occupy.resize(Pack_Limit);
    Neuro_Pack_Write_Occupy.resize(Pack_Limit);
    ID_Find_Pack_Index.reserve(Pack_Limit);

    if (!LAST_PACK_NUMBER)
        return;

    FILE* fp_I = fopen("Record_Pre_Load_File_I.rec", "r");
    FILE* fp_II = fopen("Record_Pre_Load_File_II.rec", "r");

    if (fp_I && fp_II)
    {
        int size_neuro_inside = sizeof(Pack_Inside);
        int size_neuro_record = sizeof(Pack_Record);

        Pack_Inside single_data;
        Pack_Inside* single_data_ptr = &single_data;

        Pack_Record single_record;
        Pack_Record* single_record_ptr = &single_record;

        int i = 1;

        while (i <= LAST_PACK_NUMBER)
        {
            fread(single_data_ptr, size_neuro_inside, 1, fp_I);
            fread(single_record_ptr, size_neuro_record, 1, fp_II);

            Neuro_Pack_Storage[i] = single_data;
            Neuro_Pack_Record_Storage[i] = single_record;

            if (single_record.pack_state)
                ID_Find_Pack_Index[single_record.pack_id] = i;
            else
                Free_1_Pack_Index_List.push_back(i);

            fseek(fp_I, size_neuro_inside, 1);
            fseek(fp_II, size_neuro_record, 1);

            i++;
        }

        fclose(fp_I);
        fclose(fp_II);
    }
}


void save_pre_load_file()
{
    FILE *fp = fopen("Record_Pre_Load_File.rec", "w");
    LAST_PACK_NUMBER = Neuro_Pack_Storage.size() - 1;
    fwrite(Neuro_Pack_Storage.data() + 1, LAST_PACK_NUMBER * sizeof(Pack_Inside), 1, fp);
    fclose(fp);
}


INDEX pack_index_find(ID target)
{
    auto position = ID_Find_Pack_Index.find(target);

    if (position != ID_Find_Pack_Index.end())
        return (position->second);
    else
        return 0;
}


INDEX pack_insert_position_provide(ID target, char pack_size)
{
    INDEX first_idx;
    INDEX idx;

    if(Free_Pack_Num.load() < 16)
        return 0;

    Current_Pack_Num.fetch_add(pack_size);

    
    if(pack_size == 1)
    {
        if (!Free_1_Pack_Index_List.empty())
        {
            int expected = 0;
            while(Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;

            idx = Free_1_Pack_Index_List.back();
            Free_1_Pack_Index_List.pop_back();

            expected = 1;
            while(Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;
            
            first_idx = idx;

            Neuro_Pack_Record_Storage[idx].pack_size = pack_size;
            Neuro_Pack_Record_Storage[idx].pack_state = 1;

        } else {
            Pack_Record new_record;
            new_record.pack_size = pack_size;
            new_record.pack_state = 1;

            Pack_Inside new_pack; 

            int expected = 0;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;

            idx = Neuro_Pack_Storage.size();
            Neuro_Pack_Record_Storage.push_back(new_record);
            Neuro_Pack_Storage.push_back(new_pack);

            expected = 1;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;
        }
    } else if(pack_size == 2) {

        char curr_check = 0;
        INDEX cover_check[8];
        char suitable_success = 1;

        while (!Free_2_Pack_Index_List.empty())
        {
            int expected = 0;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;

            idx = Free_2_Pack_Index_List.back();
            Free_2_Pack_Index_List.pop_back();

            expected = 1;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;

            char expected_char = 0;

            while(curr_check < pack_size && suitable_success != 0)
            {
                if(Neuro_Pack_Record_Storage[idx].pack_state != 0)
                {
                    suitable_success = 0;
                    break;
                }

                while(!Neuro_Pack_Write_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;
                
                while(!Neuro_Pack_Read_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;

                if(Neuro_Pack_Record_Storage[idx].pack_state == 0)
                {
                    cover_check[curr_check] = idx;
                    curr_check++;

                    if(curr_check == pack_size - 1)
                    {
                        suitable_success = 2;
                        curr_check = 0;
                        first_idx = cover_check[0];
                        
                        while(curr_check < pack_size)
                        {
                            idx = cover_check[curr_check];
                            Neuro_Pack_Record_Storage[idx].pack_size = pack_size;
                            Neuro_Pack_Record_Storage[idx].pack_state = 1;

                            Neuro_Pack_Write_Occupy[idx].fetch_sub(1);
                            Neuro_Pack_Read_Occupy[idx].fetch_sub(1);
                        }
                    }

                } else {
                    suitable_success = 0;
                    break;
                }
            }
            
        }
        
        if (Free_2_Pack_Index_List.empty() && suitable_success != 2)
        {
            int expected = 0;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;
            
            Pack_Record new_record;
            new_record.pack_size = pack_size;
            new_record.pack_state = 1;
            idx = Neuro_Pack_Record_Storage.size();
            Neuro_Pack_Record_Storage.push_back(new_record);

            first_idx = idx;

            Pack_Inside new_pack; 
            Neuro_Pack_Storage.push_back(new_pack);
            pack_size--;

            while(pack_size)
            {
                Pack_Record new_record;
                new_record.pack_state = 5;
                idx = Neuro_Pack_Record_Storage.size();
                Neuro_Pack_Record_Storage.push_back(new_record);

                Pack_Inside new_object; 
                Neuro_Pack_Storage.push_back(new_object);
                pack_size--;
            }

            expected = 1;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;
        }

    } else if(pack_size == 4) {

        char curr_check = 0;
        int cover_check[8];
        char suitable_success = 1;

        while (!Free_4_Pack_Index_List.empty())
        {
            int expected = 0;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;
            
            idx = Free_4_Pack_Index_List.back();
            Free_4_Pack_Index_List.pop_back();

            expected = 1;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                 expected = 1;
            
            char expected_char = 0;

            
            while(curr_check < pack_size && suitable_success != 0)
            {
                
                if(Neuro_Pack_Record_Storage[idx].pack_state != 0)
                {
                    suitable_success = 0;
                    break;
                }

                
                while(!Neuro_Pack_Write_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;
                
                
                while(!Neuro_Pack_Read_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;

                
                if(Neuro_Pack_Record_Storage[idx].pack_state == 0)
                {
                    cover_check[curr_check] = idx;
                    curr_check++;

                    if(curr_check == pack_size - 1)
                    {
                        suitable_success = 2;
                        curr_check = 0;
                        first_idx = cover_check[0];
                        
                        
                        while(curr_check < pack_size)
                        {
                            idx = cover_check[curr_check];
                            Neuro_Pack_Record_Storage[idx].pack_size = pack_size;
                            Neuro_Pack_Record_Storage[idx].pack_state = 1;

                            
                            Neuro_Pack_Write_Occupy[idx].fetch_sub(1);
                            Neuro_Pack_Read_Occupy[idx].fetch_sub(1);
                        }
                    }

                } else {
                    suitable_success = 0;
                    break;
                }
            }
            
        }
        
        if (Free_4_Pack_Index_List.empty() && suitable_success != 2)
        {
            
            int expected = 0;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;
            
            Pack_Record new_record;
            new_record.pack_size = pack_size;
            new_record.pack_state = 1;
            idx = Neuro_Pack_Record_Storage.size();
            Neuro_Pack_Record_Storage.push_back(new_record);

            first_idx = idx;

            Pack_Inside new_pack; 
            Neuro_Pack_Storage.push_back(new_pack);
            pack_size--;

            while(pack_size)
            {
                Pack_Record new_record;
                new_record.pack_state = 5;
                idx = Neuro_Pack_Record_Storage.size();
                Neuro_Pack_Record_Storage.push_back(new_record);

                Pack_Inside new_pack; 
                Neuro_Pack_Storage.push_back(new_pack);
                pack_size--;
            }

            expected = 1;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;
        }

    } else if(pack_size == 8) {
        
        char curr_check = 0;
        int cover_check[8];
        char suitable_success = 1;

        while (!Free_8_Pack_Index_List.empty())
        {
            int expected = 0;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 0;
            idx = Free_8_Pack_Index_List.back();
            Free_8_Pack_Index_List.pop_back();

            expected = 1;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;

            char expected_char = 0;

            
            while(curr_check < pack_size && suitable_success != 0)
            {
                if(Neuro_Pack_Record_Storage[idx].pack_state != 0)
                {
                    suitable_success = 0;
                    break;
                }

                while(!Neuro_Pack_Write_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;
                
                while(!Neuro_Pack_Read_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;

                if(Neuro_Pack_Record_Storage[idx].pack_state == 0)
                {
                    cover_check[curr_check] = idx;
                    curr_check++;

                    if(curr_check == pack_size - 1)
                    {
                        suitable_success = 2;
                        curr_check = 0;
                        first_idx = cover_check[0];
                        
                        
                        while(curr_check < pack_size)
                        {
                            idx = cover_check[curr_check];
                            Neuro_Pack_Record_Storage[idx].pack_size = pack_size;
                            Neuro_Pack_Record_Storage[idx].pack_state = 1;

                            Neuro_Pack_Write_Occupy[idx].fetch_sub(1);
                            Neuro_Pack_Read_Occupy[idx].fetch_sub(1);
                        }
                    }

                } else {
                    suitable_success = 0;
                    break;
                }
            }
            
        }
        
        if (Free_8_Pack_Index_List.empty() && suitable_success != 2)
        {
            
            int expected = 0;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;
            
            Pack_Record new_record;
            new_record.pack_size = pack_size;
            new_record.pack_state = 1;
            idx = Neuro_Pack_Record_Storage.size();
            Neuro_Pack_Record_Storage.push_back(new_record);

            first_idx = idx;

            Pack_Inside new_pack; 
            Neuro_Pack_Storage.push_back(new_pack);
            pack_size--;

            while(pack_size)
            {
                Pack_Record new_record;
                new_record.pack_state = 5;
                idx = Neuro_Pack_Record_Storage.size();
                Neuro_Pack_Record_Storage.push_back(new_record);

                Pack_Inside new_pack; 
                Neuro_Pack_Storage.push_back(new_pack);
                pack_size--;
            }

            expected = 1;
            while( Occupy_Neuro_Pack_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;
        }
    }
    
    ID_Find_Pack_Index[target] = first_idx;

    return first_idx;
}


void pack_delete(ID target)
{
    INDEX pack_idx = pack_index_find(target);

    if (pack_idx == 0)
        return;

    
    ID_Find_Pack_Index.erase(target);
    Neuro_Pack_Record_Storage[pack_idx].pack_state = 0;

    char pack_size = Neuro_Pack_Record_Storage[pack_idx].pack_size;

    if(pack_size == 1)
    {
        Neuro_Pack_Record_Storage[pack_idx].pack_state = 0;
        Free_1_Pack_Index_List.push_back(pack_idx);

    } else if(pack_size == 2) {

        Neuro_Pack_Record_Storage[pack_idx].pack_state = 0;
        Free_2_Pack_Index_List.push_back(pack_idx);

    } else if(pack_size == 4) {

        Neuro_Pack_Record_Storage[pack_idx].pack_state = 0;
        Free_4_Pack_Index_List.push_back(pack_idx);

    } else if(pack_size == 8) {

        Neuro_Pack_Record_Storage[pack_idx].pack_state = 0;
        Free_8_Pack_Index_List.push_back(pack_idx);
    }
    
}


inline uint8_t rand_0_to_255()
{
    static thread_local uint32_t x = 764182953u;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return uint8_t(x);
}


random_device Overall_RD;
mt19937 Overall_Gen(Overall_RD());


atomic<int> current_0_1s_memory_value = 0;
char memory_time_num = 0;
float total_memory_value_10s_average = 0;

atomic<int> newly_use_memoey = 0;
int newly_memory_max_value = 5;
float newly_memory_max_rate = 0.05;
float newly_memory_attract = 1;

atomic<int> restore_memory_need_value = 0;
float restore_memory_rate;

atomic<int> recall_memory_need_value = 0;
float recall_memory_rate;


deque<ID> Will_Read_Neuro_Queue;
mutex mutex_will_read;


deque<ID> Will_Write_Neuro_Queue;
mutex mutex_will_write;

void add_to_Will_Read_Neuro_Queue(ID target)
{
    mutex_will_read.lock();
    Will_Read_Neuro_Queue.push_back(target);
    mutex_will_read.unlock();

    recall_memory_need_value.fetch_add(1);
}

void add_to_Will_Write_Neuro_Queue(ID target)
{
    mutex_will_write.lock();
    Will_Write_Neuro_Queue.push_back(target);
    mutex_will_write.unlock();
    
    restore_memory_need_value.fetch_add(1);
}


string byteToHex(uint8_t byte)
{
    string out;
    static const char hex[] = "0123456789ABCDEF";
    out[0] = hex[(byte >> 4) & 0x0F];
    out[1] = hex[byte & 0x0F];
    return out;
}


string ID_To_Directory_Path(ID target)
{
    string dir1 = byteToHex(static_cast<unsigned char>((target >> 24) & 0xFF));
    string dir2 = byteToHex(static_cast<unsigned char>((target >> 16) & 0xFF));
    string dir3 = byteToHex(static_cast<unsigned char>((target >> 8) & 0xFF));
    string name4 = byteToHex(static_cast<unsigned char>((target) & 0xFF));

    std::string directory;
    directory = dir1 + "/" + dir2 + "/" + dir3 + "/" + name4 + ".n";

    return directory;
}


long long ALL_NEURO_RAM = 0;


void ram_coast()
{
    ALL_NEURO_RAM = sizeof(Pack_Inside) * Neuro_Pack_Storage.size();
}

long long one_G = 1024 * 1024 * 1024;

long long LIMIT_RAM = 24 * one_G;

float Pack_Attract_Rate = 0;

int gap_time = 16;

int time_update_gap = 0xFFFFFFF0;

void Neuro_Use_Record_Update(ID target_id)
{
    INDEX idx = ID_Find_Pack_Index[target_id];
    Pack_Record& pack_record = Neuro_Pack_Record_Storage[idx];

    int curr_time = pack_record.use_time[pack_record.time_curr];

    if (curr_time != (CURRENT_MODEL_TIME & time_update_gap) )
    {
        pack_record.time_curr += 1;

        if (pack_record.time_curr >= 4)
            pack_record.time_curr = 0;

        curr_time = CURRENT_MODEL_TIME & time_update_gap;

        pack_record.use_time[pack_record.time_curr] = curr_time;
    }

    pack_record.history_use_num[pack_record.time_curr] += 1;

    time_node_occupy[time_curr] += 1;

}


void Pack_Clear_Thread()
{
    int Half_Pack_Limit = Pack_Limit * 0.5;

    while (IF_SELF_TURN_OFF)
    {
        Pack_Attract_Rate = (Half_Pack_Limit - Current_Pack_Num.load()) / Half_Pack_Limit;

        for (int a = 0; a < 4; a++)
            node_total_occupy += time_node_occupy[a];

        float ave_occupy = node_total_occupy / (Current_Pack_Num.load() + 1);

        int i = 1;
        while (i < Neuro_Pack_Storage.size())
        {
            Pack_Record& pack_record = Neuro_Pack_Record_Storage[i];

            if (pack_record.pack_state == 0 || pack_record.pack_state != 2)
                continue;

            int curr_pack_total_history_use_num = 0;
            int low_permit = use_time[time_curr] - 4 * gap_time;

            for (int b = 0; b < 4; b++)
            {
                if (pack_record.use_time[b] < low_permit)
                    continue;
                
                curr_pack_total_history_use_num += pack_record.history_use_num[b];
            }

            float Curr_Pack_Attract = (curr_pack_total_history_use_num - ave_occupy) / ave_occupy;

            if (CURRENT_MODEL_TIME - pack_record.use_time[pack_record.time_curr] > 360
                || Curr_Pack_Attract + Pack_Attract_Rate < 0)
            {
                pack_record.pack_state = 6;

                add_to_Will_Write_Neuro_Queue(pack_record.pack_id);
            }
            i++;

        }

        i = 1;
    }
}



int* node_find(ID target)
{
    int vec_pos = pack_index_find(target);

    if (vec_pos == -1)
        return 0;

    Neuro_Use_Record_Update(target);

    int * return_ptr = Neuro_Pack_Storage[vec_pos].attribute_item;
    
    return return_ptr;
}




struct Neuro_Head_Item
{
    unsigned char neuro_kind;
    unsigned char node_size = 1;
    unsigned short used_item_num = 0;
    
};


Neuro_Head_Item neuro_head_read(int x)
{
    Neuro_Head_Item s;
    s.neuro_kind = (x >> 24) & 0xFF;
    s.node_size = (x >> 16) & 0xFF;
    s.used_item_num = x & 0xFFFF;
    return s;
}




struct Neuro_Concept_Desc
{
    char neuro_is_concept = 1;
    char be_restrain_num = 0;
    char limit_predict_num = 8;
    char predict_stability_num = 0; // 8/16/32

    int neuro_active_num = 0;
    int self_attention = 0;

    float predict_stability = 0;
};

struct Neuro_Image_Desc
{
    char neuro_is_image = 2;
    char detail_kind;
    char limit_predict_num = 8;
    char all_predict_stability_num = 0; // num = 8/16/32

    int neuro_active_num = 0;
    int self_attention = 0;
    
    float predict_stability_8 = 0;
    // float predict_stability_32 = 0;

    // 
    float upward_predict_ability = 0;
    float downward_predict_ability = 0;
    float left_predict_ability = 0;
    float right_predict_ability = 0;

    float appear_worth = 0;

    float puruse = 0;

    //10

    // unsigned char red;
    // unsigned char green;
    // unsigned char blue;

    // unsigned char length;
    // unsigned char direction;


}; // 15*4

struct Neuro_Time_Desc
{
    char neuro_is_time = 3;
    char detail_kind;
    char limit_predict_num = 8;
    char predict_stability_num = 0;

    int neuro_active_num = 0;
    int self_attention = 0;

    float predict_stability = 0;

    float forward_predict_stability = 0;
    float backward_predict_stability = 0;
};


struct Neuro_Text_Desc
{
    char neuro_is_text = 4;
    char detail_kind;
    char limit_predict_num = 8;
    char predict_stability_num = 0; // 8/16/32

    int neuro_active_num = 0;
    int self_attention = 0;

    float predict_stability = 0;

    float forward_predict_stability = 0;
    float backward_predict_stability = 0;
    // 6



}; // 15*4



//
union Neuro_Desc
{
    Neuro_Concept_Desc concept_desc;
    Neuro_Image_Desc image_desc;
    Neuro_Time_Desc time_desc;
    Neuro_Text_Desc text_desc;
};


struct Attribute_Head_Item
{
    char attribute_kind = -1;
    char updateable = 0;
    char condition_result_num = 0;

    int stat_num = 0;
    int releize_num = 0;
};

struct Neuro_Filler_Item
{
    char is_neuro_item = 1;
    char item_is_filler = 0;
};

Neuro_Filler_Item filler_item;


struct Neuro_Image_Item
{
    char is_neuro_item = 1; // 神经元属性10～19
    char item_is_image = 1;
    char logic = 1; // 0不存在，1存在，2为联合属性的除末尾外的前缀属性
    char related_scale; // 相对尺度，正数为当前尺度偏对方大 负数为当前尺度偏对方小 1为使用尺度相同

    char direction; // 方向区间
    char direction_scale = 3; // 方向覆盖规模 0(无方向性)、1=1、2=3、3=5 向两旁蔓延的幅度
    char distance; // 距离区间
    char distance_scale = 3; // 距离覆盖规模 0(无距离性)、1=1、2=3、3=5，原区间+-得到
    
    ID target_id;
};
// 3*4

struct Neuro_Colour_Item
{
    char is_neuro_item = 1;
    char item_is_colour;
    char red;
    char green;
    char blue;
};


struct Self_Inside_Locate_Item
{
    char is_neuro_item = 1;
    char item_is_self_inside_locate;

    unsigned char coding_mode = 0;
    unsigned char self_inside_I = 0;
    unsigned char self_inside_II = 0;
    unsigned char self_inside_III = 0;
};
// 1*4




struct Neuro_Number_Item
{
    char is_neuro_item = 1;
    char item_is_number = 2;
    char logic = 1;
    char detail = 0; // 作概率统计1、作数量统计2、作数量达标下限3、作数量达标上限4、作数量区间5
    
    int number_I = 0;
    int number_II = 0;
};
// 3*4


struct Neuro_Time_Item
{
    char is_neuro_item = 1;
    char item_is_time = 3;
    char logic = 1;

    char time_left_range;
    char time_right_range; // 时间区域(-4~4)，0是同时，1是从此刻至一规模时间，2是一规模时间至二规模时间，类推
    char time_scale; // 时间规模，通常是0是无规模(仅模糊方向)，1是0.1秒，2是0.2秒，3是0.3秒，类推

    ID target_id;
};
// 3*4


struct Neuro_Text_Item
{
    char is_neuro_item = 1;
    char item_is_text = 4;
    char logic = 1;
    char effect_kind; // 生成文本1，生成概念2  或 统计字符1，统计字符串2

    char left_distance = 1; // 左端作用位置，-8～8
    char right_distance = 1; // 右端作用位置，-8～8
    unsigned char distance_scale = 4; // 距离规模
    
    ID target_id;
};
// 3*4


struct Neuro_Concept_Item
{
    char is_neuro_item = 1;
    char item_is_concept = 5;
    char logic = 1;

    ID target_id;
};
// 2*4


struct Neuro_Action_Item
{
    char is_neuro_item = 1;
    char item_is_action = 6;
    char logic = 1;

    char action_kind; // 鼠标1，键盘2，文本传回3，文本显示4，窗口隐藏5，窗口显示6
    unsigned char vk; // 键码
    char mouseData; // 滚轮
    
    char direction;
    char distance;

    unsigned int dwFlags;

    ID target_id;
};
// 4*4


struct Neuro_Belief_Item
{
    char is_neuro_item = 1;
    char item_is_belief = 7;
    char detail_kind; //  等同对应1，产生2，信息3，属于4
    char logic = 1;

    int u_value = 0;
    int l_value = 0; 
    
    ID target_id;
};
//4*4


struct Neuro_Manner_Item
{
    char is_neuro_item = 1;
    char item_is_manner = 8;
    char manner_kind; // 注意力需求1 注意力转移2 识别需求3 理解需求4 选择需求5（实现需求）
    char target_kind; // id1 场景2
    
    int manner_value = 0;
    float manner_rate = 0;
    ID target_id;
};
//4*4


struct Neuro_Casual_Restrain_Item
{
    char is_neuro_item = 1;
    char item_is_restrain = 9;

    ID Restrain_Target;

    float Restrain_Rate;
};



union Neuro_Union_Attribute
{
    Attribute_Head_Item head_item;
    Neuro_Filler_Item filler_item;

    Neuro_Image_Item image_item;
    Neuro_Number_Item number_item;
    Neuro_Time_Item time_item;
    Neuro_Text_Item text_item;
    Neuro_Concept_Item concept_item;
    Neuro_Action_Item action_item;
    Neuro_Belief_Item belief_item;
    Neuro_Manner_Item manner_item;
};

Neuro_Union_Attribute filler_neuro_union = {.filler_item = filler_item};




char every_item_size[9] = {1, 3, 3, 3, 3, 2, 4, 4, 4};


inline void return_item_kind_and_size(int*& ptr, char& item_kind, char& item_size)
{
    ptr += item_size;
    item_kind = *(char*)(ptr);
    item_size = every_item_size[item_kind];
}


struct Attribute_Head
{
    char attribute_kind;
    char able_update = 0;
    
    char condition_num = 0;

    char result_num = 0;

    char item_num = 4;

    int stat_num = 0;
    int releize_num = 0;
};
//

Attribute_Head attribute_head_read(int *ptr)
{
    char one = *(char *)ptr;
    char two = *((char *)ptr + 1);
    char three = *((char *)ptr + 2);
    char four = *((char *)ptr + 3);

    Attribute_Head return_item;

    return_item.attribute_kind = one;
    return_item.able_update = two;
    
    return_item.condition_num = three >> 4;
    return_item.result_num = three & 0xf;

    return return_item;
}


void attribute_head_write(Attribute_Head attribute_head, vector<int>& target_list)
{
    int *ptr = target_list.data();

    char condition_num = attribute_head.result_num;
    char result_num = attribute_head.result_num;

    char one = attribute_head.attribute_kind;

    char two = attribute_head.able_update;

    char three = condition_num << 4;
    three |= result_num;

    char four;

    *((char *)ptr) = one;
    *((char *)ptr + 1) = two;
    *((char *)ptr + 2) = three;
    *((char *)ptr + 3) = four;

    *(ptr + 1) = attribute_head.stat_num;
    *(ptr + 2) = attribute_head.releize_num;

}


void general_attribute_put(
    Attribute_Head attribute_head, vector<int>& target_context_list,
    vector<Neuro_Union_Attribute>& general_result_condition_list)
{
    target_context_list.resize(4 + general_result_condition_list.size()*4);
    attribute_head_write(attribute_head, target_context_list);
    
    char attribute_num = general_result_condition_list.size();


    int* ptr = target_context_list.data();
    *(ptr + 1) = 0;
    *(ptr + 2) = 0;
    ptr += 3;

    for(int a = 0; a < attribute_num; a++)
    {
        Neuro_Union_Attribute neu_att = general_result_condition_list[a];
        
        switch(neu_att.action_item.item_is_action)
        {
            case 1:
            {
                *(Neuro_Image_Item*)(ptr) = general_result_condition_list[a].image_item;
                
                ptr += every_item_size[1];
            }
            break;
            case 2:
            {
                *(Neuro_Number_Item*)(ptr) = general_result_condition_list[a].number_item;

                ptr += every_item_size[2];
            }
            break;
            case 3:
            {
                *(Neuro_Time_Item*)(ptr) = general_result_condition_list[a].time_item;

                ptr += every_item_size[3];
            }
            break;
            case 4:
            {
                *(Neuro_Text_Item*)(ptr) = general_result_condition_list[a].text_item;

                ptr += every_item_size[4];
            }
            break;
            case 5:
            {
                *(Neuro_Concept_Item*)(ptr) = general_result_condition_list[a].concept_item;

                ptr += every_item_size[5];
            }
            break;
            case 6:
            {
                *(Neuro_Action_Item*)(ptr) = general_result_condition_list[a].action_item;

                ptr += every_item_size[6];
            }
            break;
            case 7:
            {
                *(Neuro_Belief_Item*)(ptr) = general_result_condition_list[a].belief_item;

                ptr += every_item_size[7];
            }
            break;
            case 8:
            {
                *(Neuro_Manner_Item*)(ptr) = general_result_condition_list[a].manner_item;
    
                ptr += every_item_size[8];
            }
        }
    }



}


struct Passive_Link_Attribute
{
    char kind_is_passive = 10;
    char link_attribute_kind;

    ID target_id;
};
// 2*4

//
void passive_link_put(
    Passive_Link_Attribute passive_link_attritube, 
    vector<int>& target_list)
{
    target_list.resize(2);
    int* ptr = target_list.data();
    *((char*)ptr) = passive_link_attritube.kind_is_passive;
    *((char*)ptr + 1) = passive_link_attritube.link_attribute_kind;
    *(ptr + 1) = passive_link_attritube.target_id;
}


struct modify_context
{
    ID target;
    vector<int> item_context;
    unsigned short position;
    bool add_or_insert;
};


unordered_set<ID> Waiting_Modify_Neuro_Set;
deque<modify_context> Waiting_Modify_Neuro_List;


void modify_neuro_queue_load(modify_context context)
{
    if (Waiting_Modify_Neuro_Set.count(context.target) == 1)
    {

    } else {
        Waiting_Modify_Neuro_List.push_back(context);
        Waiting_Modify_Neuro_Set.insert(context.target);
    }
}


ID new_neuro_create(char neuro_kind/* 1 2 3 4 */, Neuro_Desc neuro_desc)
{
    ID node_id;

    if (FREE_ID_NUM > 0)
    {
        FREE_ID_NUM--;
        node_id = FREE_ID_LIST.back();
        FREE_ID_LIST.pop_back();

    } else {

        node_id = NEWIST_USEFUL_ID;
        NEWIST_USEFUL_ID += 1;
    }

    INDEX idx = pack_insert_position_provide(node_id, 1);

    
    Neuro_Head_Item neuro_head;
    neuro_head.neuro_kind = neuro_kind;

    int *ptr = Neuro_Pack_Storage[idx].attribute_item;
    *(Neuro_Head_Item*)ptr = neuro_head;

    ptr += 1;
    if(neuro_kind == 1)
    {
        Neuro_Image_Desc desc = neuro_desc.image_desc;
        *(Neuro_Image_Desc*)ptr = desc;
        
    } else if(neuro_kind == 2) {
        
        Neuro_Time_Desc desc = neuro_desc.time_desc;
        *(Neuro_Time_Desc*)ptr = desc;

    } else if(neuro_kind == 3) {
        
        Neuro_Text_Desc desc = neuro_desc.text_desc;
        *(Neuro_Text_Desc*)ptr = desc;

    } else if(neuro_kind == 4) {
        
        Neuro_Concept_Desc desc = neuro_desc.concept_desc;
        *(Neuro_Concept_Desc*)ptr = desc;
    }

    return node_id;
}



void neuro_delete(ID node_id)
{
    FREE_ID_NUM++;
    FREE_ID_LIST.push_back(node_id);
}


char neuro_size_up(ID target_id, unsigned char level)
{

    
    INDEX ori_idx;

    if (ID_Find_Pack_Index.count(target_id) != 0)
        ori_idx = ID_Find_Pack_Index[target_id];
    else
        return -1;

    char curr_pack_size = Neuro_Pack_Record_Storage[ori_idx].pack_size;
    char more_pack_size = level - curr_pack_size;

    if (more_pack_size < 0)
        return -2;

    
    INDEX new_idx = pack_insert_position_provide(target_id, level);

    
    for(int a = 0; a < curr_pack_size; a++)
    {
        Neuro_Pack_Storage[new_idx + a] = Neuro_Pack_Storage[ori_idx + a];
        Neuro_Pack_Record_Storage[new_idx + a] = Neuro_Pack_Record_Storage[ori_idx + a];
    }

    
    char *head_ptr = (char *)Neuro_Pack_Storage[new_idx].attribute_item;
    head_ptr++;
    *head_ptr = level;
    head_ptr++;
    *(short *)head_ptr += more_pack_size * 256;

    return 1;
}


char neuro_item_delete(ID target,
    unsigned short item_order, unsigned char delete_num)
{
    INDEX vec_pos = pack_index_find(target);

    if (vec_pos == -1)
        return 0;

    int* ptr = Neuro_Pack_Storage[vec_pos].attribute_item;

    if (ptr == 0)
        return -1;

    char* pack_state = &Neuro_Pack_Record_Storage[vec_pos].pack_state;

    unsigned char pack_size = Neuro_Pack_Record_Storage[vec_pos].pack_size;
    unsigned short used_item_num = *((unsigned short *)(ptr) + 1);

    if (item_order > used_item_num)
        return -2;

    if (*pack_state != 2)
        return -3;

    *pack_state = 5;

    
    *((unsigned short *)(ptr) + 1) -= delete_num;

    int *delete_ptr = ptr;
    ptr += delete_num;

    for (unsigned short curr_item = item_order + delete_num; curr_item <= used_item_num; curr_item++)
    {
        *delete_ptr = *ptr;

        delete_ptr += 1;
        ptr += 1;
    }

    *pack_state = 2;

    return 1;
}


char neuro_attribute_write(
    ID target_ID, vector<int> item_context,
    unsigned short position, bool add_or_insert)
{
    int* ptr = node_find(target_ID);

    if (ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(target_ID);

        modify_context context;
        context.target = target_ID;
        context.item_context = item_context;

        modify_neuro_queue_load( context );
        return 0;
    }

    int *first_ptr = ptr;
    char *pack_state = ((char *)(first_ptr) + 1);

    if (*pack_state != 2)
        return 0;

    *pack_state = 4;
    char pack_size = *((char *)(first_ptr) + 1);
    unsigned short used_item_num = *((short *)(first_ptr) + 1);

    int item_context_size = item_context.size();

    *((short *)first_ptr + 1) += item_context_size;

    char curr_pack_order = 0;

    unsigned short curr_order;

    ptr += 15;

    if (add_or_insert == 0)
    {
        ptr += used_item_num;

        for (int single_item : item_context)
        {
            *ptr = single_item;
        }

    } else if (add_or_insert == 1) {

        char pack_size = *((char *)first_ptr + 1);
        unsigned short used_item_num = *((short *)first_ptr + 1);

        int insert_pos = position;

        int *write_ptr = first_ptr + used_item_num;
        int *end_ptr = first_ptr + used_item_num + item_context_size;
        int middle_item_num = used_item_num - insert_pos;

        for (int q = 0; q < middle_item_num; q++)
        {
            *end_ptr = *write_ptr;
            end_ptr--;
            write_ptr--;
        }

        ptr = first_ptr + insert_pos;

        for (int single_item : item_context)
        {
            *ptr = single_item;
            ptr++;
        }

    }

    *pack_state = 2;

    return 1;
}


inline void Simple_Neuro_Attribute_write(ID target_id, unsigned short position, bool add_or_insert,
    Attribute_Head attribute_head, vector<Neuro_Union_Attribute>& general_result_condition_list)
{
    vector<int> add_item;
    vector<int> passive_item;

    Passive_Link_Attribute passive_link_attribute;
    passive_link_attribute.link_attribute_kind = attribute_head.attribute_kind;
    passive_link_attribute.target_id = target_id;

    general_attribute_put(attribute_head, add_item, general_result_condition_list);
    passive_link_put(passive_link_attribute, passive_item);

    neuro_attribute_write(target_id, add_item, 0, 1);

    for(int a = 0; a < general_result_condition_list.size(); a++)
    {
        neuro_attribute_write(general_result_condition_list[a].image_item.target_id, passive_item, 0, 1);
    }
    
}


struct RGB_Unit
{
    unsigned char r, g, b;
};


struct Point_2d
{
    short x, y;
};


struct Point_2d_Equal
{
    bool operator()(const Point_2d &a, const Point_2d &b) const
    {
        return a.x == b.x && a.y == b.y;
    }
};

struct Point_2d_Hash
{
    size_t operator()(const Point_2d &p) const
    {
        return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
    }
};

inline bool point_less(const Point_2d &a, const Point_2d &b)
{
    if (a.x != b.x)
        return a.x < b.x;
    return a.y < b.y;
}


struct VectorUint32_tHash
{
    size_t operator()(const vector<uint32_t> &v) const noexcept
    {
        uint64_t h = 1469598103934665603ULL; // FNV-1a offset

        for (uint32_t x : v)
        {
            uint64_t z = static_cast<uint64_t>(x);
            h ^= z;
            h *= 1099511628211ULL; // FNV prime
        }

        return static_cast<size_t>(h);
    }
};

struct VectorUint32_tEq
{
    bool operator()(const vector<uint32_t> &a,
        const vector<uint32_t> &b) const noexcept
    {
        return a == b;
    }
};


struct Value_Sort_Unit 
{
    INDEX higher_one = 0; // >
    INDEX lower_one = 0; // <=
    int value = 0;
    INDEX target_idx = 0;
};

Value_Sort_Unit value_sort_unit_a;

class Advanced_Value_Sort
{
    private:
    int check_gap = 0;
    int change_number = 0;

    vector<Value_Sort_Unit> fast_check_list = {value_sort_unit_a};
    vector<INDEX> free_idx;
    
    public:
    vector<Value_Sort_Unit> list = {value_sort_unit_a};
    private:
    //1
    INDEX find_the_value_position(int value)//the_first<=
    {
        INDEX check_this = 1;

        for( ; check_this < fast_check_list.size(); check_this++)
        {
            if(fast_check_list[check_this].value > value) {
                continue;
            } else {
                check_this--;
                break;
            }
        }

        if(fast_check_list.size() != 1)
            check_this = fast_check_list[check_this].target_idx;
        else return 0;

        while(list[check_this].value > value)
        {
            INDEX lo = list[check_this].lower_one;
            
            if(lo != 0)
                check_this = lo;
            else
                break;
        }

    return check_this;
    }

    public:
    //2
    void insert_a_unit(Value_Sort_Unit insert_unit)
    {
        //
        INDEX lower_one = find_the_value_position(insert_unit.value);

        INDEX insert_position;
        if( !free_idx.empty() )
        {
            insert_position = free_idx.back();
            free_idx.pop_back();
        } else {
            insert_position = list.size();
            list.push_back(insert_unit);
        }


        INDEX upper_one = list[lower_one].higher_one;

        list[upper_one].lower_one = insert_position;
        list[lower_one].higher_one = insert_position;

        list[insert_position].higher_one = upper_one;
        list[insert_position].lower_one = lower_one;

        change_number++;

        if(change_number > 2*(check_gap + 1) )
            reset_enter();
    }

    //3
    void delete_a_unit(Value_Sort_Unit delete_unit)
    {
        INDEX pos = find_the_value_position(delete_unit.value);

        if(list[pos].value == delete_unit.value)
        {
            INDEX last_one = list[pos].higher_one;
            INDEX next_one = list[pos].lower_one;

            list[last_one].lower_one = next_one;
            list[next_one].higher_one = last_one;

            free_idx.push_back(pos);

            fast_check_list;
        }

        change_number--;

        if(change_number < -2*(check_gap - 1) )
            reset_enter();
    }

    //4
    bool change_a_unit(Value_Sort_Unit origin_unit, Value_Sort_Unit change_unit)
    {
        INDEX possible_idx = find_the_value_position(origin_unit.value);

        while(list[possible_idx].target_idx
            != origin_unit.target_idx)
        {
            possible_idx = list[possible_idx].lower_one;

            if(list[possible_idx].value != origin_unit.value)
            {
                return 0;
            }
        }

        list[possible_idx].value = change_unit.value;

        return 1;
    }

    private:
    //5
    void reset_enter()
    {
        if( change_number >= (check_gap+1)*2 )
        {
            int ture_number = list.size() - free_idx.size();

            int old_enter_gap = check_gap;

            check_gap += 1;

            int current_No = 1;

            INDEX target_idx = fast_check_list[1].target_idx;

            int write_idx = 2;

            for( ; write_idx < fast_check_list.size(); write_idx++)
            {
                for(int w = 0 ; w < check_gap; w++)
                {
                    target_idx = list[target_idx].target_idx;
                    current_No++;
                }

                fast_check_list[write_idx].target_idx = target_idx;
            }

            int number_of_this_unit = 0;

            while( current_No < ture_number )
            {   
                for(int q = 0; q < check_gap; q++)
                {
                    target_idx = list[target_idx].target_idx;
                    current_No++;
                    number_of_this_unit++;
                }

                if(number_of_this_unit == check_gap-1 )
                {
                    Value_Sort_Unit new_unit;
                    new_unit.target_idx = target_idx;
                    
                    fast_check_list.push_back(new_unit);
                    number_of_this_unit = 0;
                }
            }

        } else if (change_number <= 2*(check_gap-1) ) {

            int ture_number = list.size() - free_idx.size();

            int old_enter_gap = check_gap;

            check_gap -= 1;

            int current_No = 1;

            INDEX target_idx = fast_check_list[1].target_idx;

            int write_idx = 2;

            int number_of_this_unit = 0;

            for( ; write_idx < fast_check_list.size() && current_No < ture_number; write_idx++)
            {
                for(int w = 0 ; w < check_gap; w++)
                {
                    target_idx = list[target_idx].target_idx;
                    current_No++;
                    number_of_this_unit++;
                }

                if(number_of_this_unit == check_gap-1)
                {
                    fast_check_list[write_idx].target_idx = target_idx;
                    number_of_this_unit = 0;
                }
            }

            while(write_idx <= fast_check_list.size())
            {
                fast_check_list.pop_back();
            }


        }
    }
};



struct Node_Image_Single_Attribute
{
    char is_node_attribute = 2; //点自身属性2、点所需特征3
    char self_is_Node_Image_Attribute = 1;
    char observe_size;

    short x, y;

    int block_num = 0;
    int contain_distance = 0;
};
//4*4

struct Node_Image_Continuous_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Image_Continuous_Attribute = 1;

    char begin_direction;
    char end_direction;

    char begin_distance;
    char end_distance;
};


struct Node_Number_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Number_Attribute = 2;
    char logic; // node_number1 Probability2
    
    int number_I = 0;
    int number_II = 0;
};
//3*4


struct Node_Time_Single_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Time_Attribute = 3;
    char absolute_or_relative;

    int time;
    int block_num = 0;
};
//3*4


struct Node_Time_Continuous_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Time_Continuous_Attribute = 3;\

    int begin_time;
    int end_time;

    int block_num = 0;
};


struct Node_Text_Single_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Text_Attribute = 4;

    INDEX position = 0;
    int block_num = 0;
};


struct Node_Text_Continuous_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Text_Continuous_Attribute = 4;

    INDEX begin_position = 0;
    INDEX end_position = 0;
    int block_num = 0;
};


struct Node_Base_Action_Attribute
{
    char is_node_attribute = 2;
    char self_is_Node_Action_Attribute = 5;

    char action_order;
    char action_kind;
    unsigned char vk;
    char mouseData;

    short x, y;
    
    unsigned int dwFlags;
};
//4*4


struct Link_Node_Attribute
{
    char is_node_link = 4; // 点链接节点4、点链接场域5
    char link_Kind; // （点-点）下层1 上层2 统计3（点-场）所属
    char node_link; // 1 2 3 4
    char connect_direction = 0;
    
    uint32_t link_idx_or_id = 0;
    float link_value_I = 1;
    int link_value_II = 0;
};
//4*4




struct Scene_Image_Attribute
{
    char is_scene_attribute = 6;// 场自身属性6 场所需特征7
    char self_is_Scene_Image_Attribute = 1;
    
    short left, right, bottom, top;
};
// 3*4


struct Scene_Number_Attribute
{
    char is_scene_attribute = 6;
    char self_is_Scene_Number_Attribute = 2;
    
    int u_number;
    int l_number;
};
//3*4


struct Scene_Time_Attribute
{
    char is_scene_attribute = 6;
    char self_is_Scene_Time_Attribute = 3;
    
    int time;

    int durable_over_time;  
};
// 3*4


struct Scene_Text_Attribute
{
    char is_scene_attribute = 6;
    char self_is_Scene_Text_Attribute = 4;
    
    
    short left, right;
};
// 2*4


struct Scene_Action_Attribute
{
    char is_scene_attribute = 6;
    char self_is_Scene_Action_Attribute = 5;
    char last_or_curr_or_next;

    INDEX action_idx;

};
//2*4



struct Link_Scene_Attribute
{
    char is_scene_link = 8;//场链接场景8 场链接节点9
    unsigned char scene_kind;
    
    INDEX scene_idx;
    float related_value;
};
// 3*4


struct Require_Object
{
    char is_require = 10;// 单一需求为10 11为连带需求
    char require_object_kind; // id 1 scene 2
    char require_detail_kind; // 自身0 空间属性/区域1 数量属性/区域2 时间属性/区域3 文本属性/区域4
    char require_kind;
    /* retrieve识别1* 
    检索生成构造目的10 解释目标态度属性11 行动连接性探测12
    */
    /* model建模2* 
    文本对象性建模21 
    */
    /* construct构造3* ～ 4*
    构造下级与对应的节点31    排列文本构造需求节点32    构造为图像节点33     外部追求构造34
    */

    uint32_t require_id_or_idx = 0;
    int require_value = 0;// 1为神经元id、2为网络节点
};
//3*4

union Link_Variable_Attribute
{
    Neuro_Filler_Item filler_item;

    Neuro_Image_Item neuro_image_item;
    Neuro_Number_Item neuro_number_item;
    Neuro_Time_Item neuro_time_item;
    Neuro_Text_Item neuro_text_item;
    Neuro_Concept_Item neuro_concept_item;
    Neuro_Action_Item neuro_action_item;

    Neuro_Belief_Item neuro_belief_item;
    Neuro_Manner_Item neuro_manner_item;


    Node_Image_Single_Attribute node_image_single_attribute;
    Node_Image_Continuous_Attribute node_image_continuous_attribute;
    Node_Time_Single_Attribute node_time_single_attribute;
    Node_Time_Continuous_Attribute node_time_continuous_attribute;
    Node_Text_Continuous_Attribute node_text_continuous_attribute;
    Node_Text_Single_Attribute node_text_single_attribute;
    Node_Number_Attribute node_number_attribute;
    Node_Base_Action_Attribute node_action_attribute;

    Link_Node_Attribute link_node_attribute;


    Scene_Image_Attribute scene_image_attribute;
    Scene_Number_Attribute scene_number_attribute;
    Scene_Time_Attribute scene_time_attribute;
    Scene_Text_Attribute scene_text_attribute;
    Scene_Action_Attribute scene_action_attribute;

    Link_Scene_Attribute link_scene_attribute;


    Require_Object require_object;
};


Link_Variable_Attribute filler_variable_attribute = {.filler_item = filler_item};

struct General_Node
{
    bool is_use = 1;
    char exist_type; // 实际有id 实际无id 模拟已归类 模拟未归类
    char node_kind; // 存在 不存在 
    char belong_scene_kind = 0; //一般，图像，时间，文本

    char node_layer;
    char rough_neuro_spare_size = -1;
    char node_calu_state = 1;
    
    char space_stable_num;
    char time_stable_num;

    char was_retrieve_generate = 0;
    char was_simulate_generate = 0;
    char probability_num;

    int node_attention = 0;
    ID self_id = 0;
    INDEX belong_scene = 0;

    int component = 0;

    float complete_or_probability = 1;
    int u_value = 0;
    int l_value = 0;

    float space_predict_stability = 0;
    float time_predict_stability = 0;

    int retrirve_require = 0;
    int model_require = 0;
    int construct_require = 0;
    int identify_require = 0;

    int total_require = 0;
    int total_derive_require = 0;

    Link_Variable_Attribute self_attribute[3] = {filler_variable_attribute, filler_variable_attribute, filler_variable_attribute};
    vector<Link_Variable_Attribute> Node_variable_attribute_list;
};


struct Focus_Object
{
    char kind;
    INDEX target_idx;
};


struct Wait_Added_Record
{
    
    char curr_is_use = 1;
    char operation;
    char number = 0;
    unsigned char item_order;
    
    ID neuro_id;

    Neuro_Union_Attribute record_item;
};


struct Wait_Active_Neuro
{
    char curr_is_use = 1;
    ID id;
};


struct Match_Generate
{
    char curr_is_use = 1;
    char attritube_kind;
    char need_condition_num = 0;
    char result_num = 0;
    char node_num = 0;

    char feed_back_type; // 匹配 需求

    INDEX match_idx = 0;
    INDEX node_idx_from[5];
    Neuro_Union_Attribute match_condition[4] = {filler_neuro_union, filler_neuro_union, filler_neuro_union, filler_neuro_union};
    
    Neuro_Union_Attribute match_result[4] = {filler_neuro_union, filler_neuro_union, filler_neuro_union, filler_neuro_union};
};


struct General_Scene
{
    bool curr_use = 1;
    char scene_kind;

    int importance = 0;
    int total_invest_resource = 0;

    int scene_attention = 0;

    int sum_require_value = 0;
    int retrieve_require = 0;
    int model_require = 0;
    int construct_require = 0;

    int total_retrieve_require_valve = 0;
    vector< vector<int> > Retrieve_total_require_list;
    Advanced_Value_Sort Retrieve_require_sort;

    int total_model_require_value = 0;
    vector< vector<int> > Model_total_require_list;
    Advanced_Value_Sort Model_require_sort;

    int total_construct_require_value = 0;
    vector< vector<int> > Construct_total_require_list;
    Advanced_Value_Sort Construct_require_sort;

    vector<INDEX> Overall_general_node_list;
    vector<INDEX> Free_overall_general_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_overall_node;

    vector<General_Node> Local_general_node_list;
    vector<INDEX> Free_local_general_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_local_node;

    unordered_map<ID, vector<Focus_Object> > Id_find_focus_object;

    vector<Match_Generate> Match_generate_list;
    vector<INDEX> Free_match_generate_index;

    vector<Link_Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<INDEX> Free_require_object_idx;
    Advanced_Value_Sort Require_object_sort;

    Advanced_Value_Sort Attention_object_sort;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Node_combo_find_repeat;
    
    Link_Variable_Attribute self_scene_attribute[3] = {filler_variable_attribute, filler_variable_attribute, filler_variable_attribute};
    vector<Link_Variable_Attribute> Scene_variable_attribute_List;
};


struct Model_Output_Action
{
    char action_order;
    char action_kind;
    unsigned char vk;
    char mouseData;

    short x, y;
    
    unsigned int dwFlags;
};//3*4



struct Line_Object
{
    char kind;
    unsigned char size;
    
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    char direction;
    short length;

    int qualify_block_num = 0;
    int total_block_num = 0;

    int attention_value = 0;

    int x_sum = 0;
    int y_sum = 0;

    float reward_rate;
    int reward_value;

    float abs_diff_value;
    float sum_diff_value;
    float ave_diff_rate;

    Point_2d base_point;
    
    INDEX first_point_idx;
    vector<INDEX> Inside_Point;
};

struct Line_Object_II
{
    char line_kind;
    unsigned char size;
    
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    char direction;
    short length;

    INDEX first_point;
};


struct Image_Colour_Block
{
    char block_kind = 1;
    char diff_colour_block = 0b00000000;

    unsigned char ave_r, ave_g, ave_b;
};


struct Image_Rough_Map
{
    char have_init = 0;
    char rough_size;
    
    short rough_width, rough_height;
    
    vector<int> colour_number_distribution; // 8*8*8 = 512num
    vector<int> colour_x_distribution; // 512num
    vector<int> colour_y_distribution; // 512num

    vector<Image_Colour_Block> Colour_block_list;

    int rough_retrieve_require_valve = 0;
    vector< vector<int> > Retrieve_rough_require_list;

    int rough_model_require_value = 0;
    vector< vector<int> > Model_rough_require_list;

    int rough_construct_require_value = 0;
    vector< vector<int> > Construct_rough_require_list;
};


struct Space_Form_Node_Record_Map
{
    char record_distance = 4;
    short max_block_permit;
    short min_block_permit;
    vector< vector<INDEX> > record_list;
};

struct Time_Form_Node_Record_Map
{
    char record_distance = 4;
    int first_front_locate_time;
    vector< vector<INDEX> > Front_record_list;
    vector< vector<INDEX> > Back_record_list;
};

struct Image_Scene
{
    char curr_use = 1;
    char have_init = 0;

    short width, height;
    
    INDEX last_map = 0;
    INDEX next_map = 0;

    int attention = 0;
    int sum_nature_attention = 0;

    int sum_require_value = 0;
    int sum_retrieve_require = 0;
    int sum_model_space_require = 0;
    int sum_model_time_require = 0;
    int sum_construct_require = 0;

    float retrieve_add_rate = 1;
    float model_space_add_rate = 0.5;
    float model_time_add_rate = 0.5;
    float construct_add_rate = 0;
    
    char action_node_num = 0;
    INDEX Action_Node_Idx[5] = {0, 0, 0, 0, 0};

    int image_node_num = 0;

    int input_time;

    vector<RGB_Unit> RGB_Map;

    char Image_rough_view_number = 0;
    Image_Rough_Map Image_rough_view[4];

    char require_distance = 8;

    int total_retrieve_require_valve = 0;
    vector< vector<int> > Retrieve_total_require_list;
    Advanced_Value_Sort Retrieve_require_sort;

    int total_model_require_value = 0;
    vector< vector<int> > Model_total_require_list;
    Advanced_Value_Sort Model_require_sort;

    int total_construct_require_value = 0;
    vector< vector<int> > Construct_total_require_list;
    Advanced_Value_Sort Construct_require_sort;
    
    vector<INDEX> Overall_image_node_list;
    vector<INDEX> Free_overall_image_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_overall_node;

    vector<General_Node> Local_image_node_list;
    vector<INDEX> Free_local_image_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_local_node;

    Space_Form_Node_Record_Map Space_form_record[5];

    unordered_map<ID, vector<Focus_Object> > Id_find_focus_object;

    vector<Match_Generate> Match_generate_list;
    vector<INDEX> Free_match_generate_index;
    unordered_map<ID, vector<INDEX>> Id_find_match_generate;

    vector<Link_Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<INDEX> Free_require_idx;
    Advanced_Value_Sort Require_object_sort;

    Advanced_Value_Sort Attention_object_list;

    unordered_set<vector<ID>, VectorUint32_tHash, VectorUint32_tEq> Node_combo_find_repeat;

    Link_Variable_Attribute self_scene_attribute[3] = {filler_variable_attribute, filler_variable_attribute, filler_variable_attribute};
    vector<Link_Variable_Attribute> Scene_variable_attribute_List;
};


struct Time_Scene
{
    char curr_use = 1;
    char is_lock_occupy = 0;
    
    int attention = 0;

    int sum_require_value = 0;
    int sum_retrieve_require = 0;
    int sum_model_time_require = 0;
    int sum_construct_require = 0;

    int time_node_num = 0;

    int begin_time;
    int end_time;

    int total_retrieve_require_valve = 0;
    vector< vector<int> > Retrieve_total_require_list;
    Advanced_Value_Sort Retrieve_require_sort;

    int total_model_require_valve = 0;;
    vector< vector<int> > Model_total_require_list;
    Advanced_Value_Sort Model_require_sort;

    int total_construct_require_value = 0;
    vector< vector<int> > Construct_total_require_list;
    Advanced_Value_Sort Construct_require_sort;

    vector<INDEX> Overall_time_node_list;
    vector<INDEX> Free_overall_time_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_overall_node;

    vector<General_Node> Local_time_node_list;
    vector<INDEX> Free_local_time_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_local_index;

    Time_Form_Node_Record_Map Node_time_from_record[5];

    unordered_set<vector<ID>, VectorUint32_tHash, VectorUint32_tEq> Node_combo_find_repeat;

    unordered_map<ID, vector<Focus_Object> > Id_find_focus_object;

    vector<Match_Generate> Match_generate_list;
    vector<INDEX> Free_match_generate_index;
    unordered_map<ID, vector<INDEX>> Id_find_match_generate;

    vector<Link_Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<INDEX> Free_require_idx;
    vector< vector<int> > Require_object_weights_map;
    Advanced_Value_Sort Require_object_sort;
    
    Advanced_Value_Sort Attention_object_sort;

    Link_Variable_Attribute self_scene_attribute[3] = {filler_variable_attribute, filler_variable_attribute, filler_variable_attribute};
    vector<Link_Variable_Attribute> Scene_variable_attribute_list;

    //?
    unordered_map<int, INDEX> Time_to_single_time_idx;
    vector<Advanced_Value_Sort> Single_time_attention_sort;
};


struct Text_Scene
{
    char is_use = 1;
    char source_kind;
    char have_init = 0;
    char Retrieval_Density = 8;

    char scene_layer = 0;
    char scene_order = 0;
    
    int attention;
    int text_derive_attention;

    int sum_retrieve_require = 0;
    int sum_model_require = 0;
    int sum_identify_require = 0;
    int sum_construct_require = 0;

    int sum_total_require = 0;

    int original_text_size = 0;

    float retrieve_add_rate = 1;
    float model_add_rate = 0.5;
    float construct_add_rate = 0;

    long long input_time;

    vector<ID> Original_text;

    int total_retrieve_require_valve = 0;
    vector< vector<int> > Retrieve_total_require_list;
    Advanced_Value_Sort Retrieve_require_sort;

    int total_model_require_valve = 0;;
    vector< vector<int> > Model_total_require_list;
    Advanced_Value_Sort Model_require_sort;

    int total_construct_require_value = 0;
    vector< vector<int> > Construct_total_require_list;
    Advanced_Value_Sort Construct_require_sort;

    Space_Form_Node_Record_Map Space_form_record[5];

    vector< INDEX > Overall_text_node_list;
    vector<INDEX> Free_overall_text_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_overall_node;

    vector<General_Node> Local_text_node_list;
    vector<INDEX> Free_local_text_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_local_node;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Node_combo_find_repeat;

    //
    unordered_map<ID, vector<Focus_Object> > Id_find_focus_list;

    vector<Link_Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<int> Require_object_value;
    Advanced_Value_Sort Require_object_sort;

    Advanced_Value_Sort Attention_object_list;

    Link_Variable_Attribute self_scene_attribute[3] = {filler_variable_attribute, filler_variable_attribute, filler_variable_attribute};
    vector<Link_Variable_Attribute> Scene_variable_attribute_List;
};



struct Gtk3_Text_Display
{
    char use = 1;
    char have_init = 0;
    char response_aim_kind; // 解释1 回答2 求知3 追求4 对待5 固定6
    
    char input_value_num = 0;
    char input_value_kind;

    int input_value_I[4];
    /*
    0、工作算力
    1、外部追求倾向、外部满足程度、外部追求需求、内部理解需求、内部理解程度
    2、
    3、
    4、
    5、外部追求倾向、内部理解需求、注意力
    6、外部追求倾向、内部理解程度
    */


    char output_value_num = 0;
    char output_value_kind;
    
    int output_value[4];
    /*
    1、外部追求倾向、内部理解程度
    2、
    3、
    4、
    5、
    6、
    */

    INDEX input_Text_Scene_Idx = 0;
    INDEX output_Text_Scene_Idx = 0;

    int total_require_value = 0;

    vector<char> Input_string;
    vector<char> Output_string;
};


/*模型的终极目的

    <1> 尽可能地认知当前世界，低消耗地、准确地、全面地
    <2> 在1的基础上，符合期望地执行界面输入的内容

*/


struct Feedback_Feeling
{
    bool is_use = 1;
    char feeling_kind;
    char source_kind;

    int feeling_value;
    float feeling_attention;
    INDEX source_node = 0;
    int duration_time;
};


struct Single_Thread
{
    char now_work = 1;
    short need_free_time = 0;
};




atomic<int> Base_Thread_Num = 4;
atomic<int> Calculate_Thread_Num = 0;
atomic<int> Use_Thread_Num = Base_Thread_Num + Calculate_Thread_Num;
vector<Single_Thread> Thread_Handle_List;
float Thread_Limit_Free_Rate = 0.3;


vector<Model_Output_Action> Waiting_Send_Action_List;
atomic<char> Send_Action_Number(0);

vector<Model_Output_Action> Alreadly_Finish_Action_List;

vector<char> Send_Text_List;
atomic<bool> Text_Is_Send(0);

vector<char> Receive_Text_List;


atomic<int> Total_Task_Attention = 100 * Calculate_Thread_Num; 
atomic<int> Free_Total_Task_Attention = 100 * Calculate_Thread_Num;

unordered_map< ID, vector<Focus_Object> > Id_Find_Focus_Object;


vector<Require_Object> Attention_List;
vector<INDEX> Free_Attention_Inedx;

vector<Link_Variable_Attribute> Require_Object_List;
vector<INDEX> Free_Require_Object_Idx;
atomic<long long> Total_Require_Value;

Advanced_Value_Sort Total_Require_Sort;


vector<Match_Generate> Match_Generate_List;
vector<INDEX> Free_Match_Generate_Idx;

vector<Wait_Added_Record> Wait_Added_Record_List;
vector<INDEX> Free_Wait_Added_Record_Idx;


vector<Link_Variable_Attribute> All_Formal_Require_Object_List;
vector< atomic<char> > Occupy_Single_Formal_Require_Object;
atomic<int> Occupy_Formal_Require_Object_List;

vector<INDEX> Free_Formal_Require_Object_Index;
atomic<int> Occupy_Free_Formal_Require_Object_Index;

Advanced_Value_Sort Formal_Require_Object_Sort;


atomic<int> current_attention = 100;
atomic<int> free_attention = 0;

vector<Model_Output_Action> Action_Choose_List;
vector<int> Action_Choose_Weights;

short Current_X = 0;
short Current_Y = 0;
bool If_Current_Interface_Show = 1;
bool If_Current_Interface_Have_Text = 0;
bool If_Current_Text_Show = 0;


vector<Feedback_Feeling> Feedback_Feeling_List;
vector<INDEX> Free_Feedback_Feeling_Idx;

int Current_Joy = 0;
int Approach_Joy = 0;
int Satisfy_Standard = 0000;


vector<General_Node> General_Node_Storage;
vector< atomic<char> > Occupy_Single_General_Node;
atomic<int> Occupy_General_Node_Storage(0);

vector<INDEX> Free_General_Node_Index;
atomic<int> Occupy_Free_General_Node_Index(0);

atomic<long long> General_Node_Num;

unordered_map< ID, vector<INDEX> > ID_Find_General_Node;


General_Scene OVERALL_THINKING_SCENE;

vector<General_Scene> General_Scene_Storage;
vector< atomic<char> > Occupy_Single_General_Scene;
atomic<int> Occupy_General_Scene_Storage(0);

vector<INDEX> Free_General_Scene_Index;
atomic<int> Occupy_Free_General_Scene_Index(0);

atomic<int> General_Scene_Num = 2;


vector<Image_Scene> Image_Scene_Storage;
vector< atomic<char> > Occupy_Single_Image_Scene;
atomic<int> Occupy_Image_Scene_Storage(0);

vector<INDEX> Free_Image_Scene_Index;
atomic<int> Occupy_Free_Image_Scene_Index(0);

atomic<int> Image_Scene_Num = 1;

unordered_map<long long, INDEX> Time_Find_Image_Scene;
INDEX action_execulate_scene = 0;


Time_Scene OVERALL_TIME_SCENE;

vector<Time_Scene> Time_Scene_Storage;
vector< atomic<char> > Occupy_Single_Time_Scene;
atomic<int> Occupy_Time_Scene_Storage(0);

vector<INDEX> Free_Time_Scene_Index;
atomic<int> Occupy_Free_Time_Scene_Index(0);

atomic<int> Time_Scene_Num = 1;

unordered_map<long long, INDEX> Time_Find_Time_Scene;
vector<INDEX> Time_Scene_Storage;
atomic<INDEX> Earliset_Time_Scene = 0;

unordered_map< long long, INDEX > every_8_time_event;
long long CURRENT_8_gap_MODEL_TIME;
long long gap_8_operate_value = 0x8 * 100;
atomic<int> is_new_CURRENT_8_gap_MODEL_TIME(0);

unordered_map< long long, INDEX > every_32_time_event;
long long CURRENT_32_gap_MODEL_TIME;
long long gap_32_operate_value = 0x20 * 100;
atomic<int> is_new_CURRENT_32_gap_MODEL_TIME(0);


vector<Text_Scene> Text_Scene_Storage;
vector< atomic<char> > Occupy_Single_Text_Scene;
atomic<int> Occupy_Text_Scene_Storage(0);

vector<INDEX> Free_Text_Scene_Index;
atomic<int> Occupy_Free_Text_Scene_Index(0);

atomic<int> Text_Scene_Num = 1;
unordered_map<long long, INDEX> Time_Find_Text_Scene;

vector<Gtk3_Text_Display> Gtk3_Text_Display_Storage;



deque< vector<INDEX> > Fixed_Event_Predict_List;

vector<INDEX> Total_Event_Predict_List;
vector<INDEX> Free_Event_Predict_Idx;

/*
 *
 * fin
 *
 */



INDEX create_a_General_Node()
{
    INDEX idx;

    int expected = 0;
    while( Occupy_Free_General_Node_Index.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
        expected = 0;

    expected = 0;
    while( Occupy_General_Node_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
        expected = 0;

    if (!Free_General_Node_Index.empty())
    {
        idx = Free_General_Node_Index.back();
        Free_General_Node_Index.pop_back();

    } else {
        General_Node new_thing;
        idx = General_Node_Storage.size();
        General_Node_Storage.push_back(new_thing);
    }

    General_Node_Num++;

    expected = 1;
    while( Occupy_Free_General_Node_Index.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
        expected = 1;

    expected = 1;
    while( Occupy_General_Node_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
        expected = 1;

    return idx;
}

INDEX create_a_General_Node(General_Node new_node)
{
    INDEX idx;

    int expected = 0;
    while( Occupy_Free_General_Node_Index.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
        expected = 0;

    expected = 0;
    while( Occupy_General_Node_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
        expected = 0;

    if (!Free_General_Node_Index.empty())
    {
        idx = Free_General_Node_Index.back();
        Free_General_Node_Index.pop_back();
        General_Node_Storage[idx] = new_node;

    } else {
        idx = General_Node_Storage.size();
        General_Node_Storage.push_back(new_node);
    }

    General_Node_Num++;

    expected = 1;
    while( Occupy_Free_General_Node_Index.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
        expected = 1;

    expected = 1;
    while( Occupy_General_Node_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
        expected = 1;

    return idx;
}

void release_a_General_Node(INDEX idx)
{
    int expected = 0;
    while( Occupy_Free_General_Node_Index.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
        expected = 0;

    expected = 0;
    while( Occupy_General_Node_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
        expected = 0;
    
    General_Node& release_node = General_Node_Storage[idx];
    release_node.is_use = 0;
    Free_General_Node_Index.push_back(idx);

    General_Node_Num--;

    expected = 1;
    while( Occupy_Free_General_Node_Index.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
        expected = 1;

    expected = 1;
    while( Occupy_General_Node_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
        expected = 1;
}

General_Node read_a_General_Node(INDEX node_idx)
{
    General_Node return_node;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    return_node = General_Node_Storage[node_idx];

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 1;
    
    return return_node;
}

void write_a_General_Node(INDEX node_idx, General_Node write_context)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node& be_write_node = General_Node_Storage[node_idx];
    write_context.Node_variable_attribute_list = be_write_node.Node_variable_attribute_list; // 鍙彉鍖虹殑鏇存敼瀹规槗閫犳垚鍐欏叆鍐茬獊锛岃鐢ㄤ笅涓嚱鏁板啓
    be_write_node = write_context;

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
}


void create_a_Node_Image_Attribute(INDEX node_idx, Node_Image_Single_Attribute attribute)
{
    Link_Variable_Attribute fo = {.node_image_single_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Node_Image_Attribute(INDEX node_idx)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_list = the_node.Node_variable_attribute_list;
    int object_size = link_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_list[q].node_image_single_attribute.is_node_attribute == 2)
        {
            if (link_list[q].node_image_single_attribute.self_is_Node_Image_Attribute == 1)
            {
                link_list[q].node_image_single_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Node_Image_Single_Attribute read_a_Node_Image_Attribute(INDEX node_idx)
{
    Node_Image_Single_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_image_single_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_image_single_attribute.self_is_Node_Image_Attribute == 1)
            {
                attribute = the_node.self_attribute[q].node_image_single_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Node_Image_Attribute(INDEX node_idx, Node_Image_Single_Attribute write_attribute)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_image_single_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_image_single_attribute.self_is_Node_Image_Attribute == 1)
            {
                link_object_list[q].node_image_single_attribute = write_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Number_Attribute(INDEX node_idx, Node_Number_Attribute attribute)
{
    Link_Variable_Attribute fo = {.node_number_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Node_Number_Attribute(INDEX node_idx)
{
    Node_Number_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_number_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_number_attribute.self_is_Node_Number_Attribute == 3)
            {
                link_object_list[q].node_number_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Node_Number_Attribute read_a_Node_Number_Attribute(INDEX node_idx)
{
    Node_Number_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_number_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_number_attribute.self_is_Node_Number_Attribute == 3)
            {
                attribute = the_node.self_attribute[q].node_number_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;

}

void write_a_Node_Number_Attribute(INDEX node_idx, Node_Number_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_number_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_number_attribute.self_is_Node_Number_Attribute == 5)
            {
                link_object_list[q].node_number_attribute = write_attritube;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Time_Attribute(INDEX node_idx, Node_Time_Single_Attribute attribute)
{
    Link_Variable_Attribute fo = {.node_time_single_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Node_Time_Attribute(INDEX node_idx)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_time_single_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_time_single_attribute.self_is_Node_Time_Attribute == 3)
            {
                link_object_list[q].node_time_single_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Node_Time_Single_Attribute read_a_Node_Time_Attribute(INDEX node_idx)
{
    Node_Time_Single_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node &the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_time_single_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_time_single_attribute.self_is_Node_Time_Attribute == 3)
            {
                attribute = the_node.self_attribute[q].node_time_single_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Node_Time_Attribute(INDEX node_idx, Node_Text_Single_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_text_single_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_text_single_attribute.self_is_Node_Text_Attribute == 2)
            {
                the_node.self_attribute[q].node_text_single_attribute = write_attritube;
                break;
            }
        }
    }
    
    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Text_Attribute(INDEX node_idx, Node_Text_Single_Attribute attribute)
{
    Link_Variable_Attribute fo = {.node_text_single_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Node_Text_Attribute(INDEX node_idx)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_text_single_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_text_single_attribute.self_is_Node_Text_Attribute == 2)
            {
                link_object_list[q].node_text_single_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Node_Text_Single_Attribute read_a_Node_Text_Attribute(INDEX node_idx)
{
    Node_Text_Single_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    
    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_text_single_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_text_single_attribute.self_is_Node_Text_Attribute == 2)
            {
                attribute = the_node.self_attribute[q].node_text_single_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Node_Text_Attribute(INDEX node_idx, Node_Text_Single_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_text_single_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_text_single_attribute.self_is_Node_Text_Attribute == 2)
            {
                link_object_list[q].node_text_single_attribute = write_attritube;
                break;
            }
        }
    }
    
    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Action_Attribute(INDEX node_idx, Node_Base_Action_Attribute attribute)
{
    Link_Variable_Attribute fo = {.node_action_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Node_Action_Attribute(INDEX node_idx)
{
    Node_Number_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_number_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_number_attribute.self_is_Node_Number_Attribute == 3)
            {
                link_object_list[q].node_number_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Node_Base_Action_Attribute read_a_Node_Action_Attribute(INDEX node_idx)
{
    Node_Base_Action_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_action_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_action_attribute.self_is_Node_Action_Attribute == 5)
            {
                attribute = the_node.self_attribute[q].node_action_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;

}

void write_a_Node_Action_Attribute(INDEX node_idx, Node_Base_Action_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_action_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_action_attribute.self_is_Node_Action_Attribute == 5)
            {
                link_object_list[q].node_action_attribute = write_attritube;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}




void push_back_a_link_to_Node(INDEX node_idx, Link_Variable_Attribute variable_attribute)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node_Storage[node_idx].Node_variable_attribute_list.push_back(variable_attribute);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


Link_Node_Attribute read_a_node_action_connect_target(INDEX)
{
    Link_Node_Attribute lna;

    return lna;
}


struct Node_Sequence_Info
{
    ;
};

Node_Sequence_Info read_a_node_sequence_info(INDEX node_idx)
{
    Node_Sequence_Info node_sequence_info;

    return node_sequence_info;
}


void get_a_category_node_newist_constituent(INDEX node_idx, ID& constituent_id, INDEX& newist_scene)
{
    ID constituent_id;

    auto& Node_variable_attribute_list = General_Node_Storage[node_idx].Node_variable_attribute_list;

}


Link_Variable_Attribute read_a_category_node_condition(INDEX node_idx)
{
    Link_Variable_Attribute return_link = filler_variable_attribute;
    

    return return_link;
};





INDEX create_a_General_Scene()
{
    INDEX idx;
    General_Scene new_thing;

    int expect = 0;
    while( Occupy_General_Scene_Storage.compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    expect = 0;
    while( Occupy_Free_General_Scene_Index.compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    if (Free_General_Scene_Index.size() > 0)
    {
        idx = Free_General_Scene_Index.back();
        Free_General_Scene_Index.pop_back();
    } else {
        idx = General_Scene_Storage.size();
        General_Scene_Storage.push_back(new_thing);
    }

    expect = 1;
    while( Occupy_General_Scene_Storage.compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    expect = 1;
    while( Occupy_Free_General_Scene_Index.compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    General_Scene_Num++;

    return idx;
}

INDEX create_a_General_Scene(General_Scene new_thing)
{
    INDEX idx;

    int expect = 0;
    while( Occupy_General_Scene_Storage.compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    expect = 0;
    while( Occupy_Free_General_Scene_Index.compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    if (Free_General_Scene_Index.size() > 0)
    {
        idx = Free_General_Scene_Index.back();
        Free_General_Scene_Index.pop_back();
        General_Scene_Storage[idx] = new_thing;
    } else {
        idx = General_Scene_Storage.size();
        General_Scene_Storage.push_back(new_thing);
    }

    expect = 1;
    while( Occupy_General_Scene_Storage.compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    expect = 1;
    while( Occupy_Free_General_Scene_Index.compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    General_Scene_Num++;

    return idx;
}

void delete_a_General_Scene(INDEX idx)
{
    int expect = 0;
    while( Occupy_General_Scene_Storage.compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    char expect_ = 0;
    while( Occupy_Single_General_Scene[idx].compare_exchange_strong(expect_, 1, memory_order_seq_cst) )
        expect = 0;
    
    
    General_Scene& the_scene = General_Scene_Storage[idx];
    the_scene.Overall_general_node_list.reserve(0);
    Free_General_Scene_Index.push_back(idx);


    expect = 1;
    while( Occupy_Free_General_Scene_Index.compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    expect_ = 1;
    while( Occupy_Single_General_Scene[idx].compare_exchange_strong(expect_, 0, memory_order_seq_cst) )
        expect_ = 1;

    General_Scene_Num--;
}


INDEX create_a_Image_Scene(Image_Scene new_map)
{
    INDEX idx;

    if (Free_Image_Scene_Index.size() > 0)
    {
        idx = Free_Image_Scene_Index.back();
        Free_Image_Scene_Index.pop_back();
        Image_Scene_Storage[idx] = new_map;
        
    } else {
        idx = Image_Scene_Storage.size();
        Image_Scene_Storage.push_back(new_map);
    }

    Image_Scene_Num++;

    return idx;
}

void delete_a_Image_Scene(INDEX idx)
{
    Image_Scene_Storage[idx].curr_use = 0;
    Free_Image_Scene_Index.push_back(idx);
    Image_Scene_Num--;
    // clear;
    // reserve();
}


INDEX create_a_Time_Scene(Time_Scene new_map)
{
    INDEX idx;

    if (Free_Time_Scene_Index.size() > 0)
    {
        idx = Free_Time_Scene_Index.back();
        Free_Time_Scene_Index.pop_back();
        Time_Scene_Storage[idx] = new_map;
        
    } else {
        idx = Time_Scene_Storage.size();
        Time_Scene_Storage.push_back(new_map);
    }

    Time_Scene_Num++;

    return idx;
}

void delete_a_Time_Scene(INDEX idx)
{
    Time_Scene_Storage[idx].curr_use = 0;
    Free_Time_Scene_Index.push_back(idx);
    Time_Scene_Num--;
    // clear;
    // reserve();
}


INDEX create_a_Text_Scene(Text_Scene new_text)
{
    INDEX idx;

    if (!Free_Text_Scene_Index.empty())
    {
        idx = Free_Text_Scene_Index.back();
        Free_Text_Scene_Index.pop_back();
        Text_Scene_Storage[idx] = new_text;

    } else {
        new_text.input_time = CURRENT_MODEL_TIME;
        idx = Text_Scene_Storage.size();
        Text_Scene_Storage.push_back(new_text);
    }

    Text_Scene_Num++;

    return idx;
}

void delete_a_Text_Scene(INDEX idx)
{
    Text_Scene& text = Text_Scene_Storage[idx];
    text.is_use = 0;
    Free_Text_Scene_Index.push_back(idx);
    Text_Scene_Num--;
}


INDEX create_a_gtk3_text_display(Gtk3_Text_Display text_display)
{
    int i = 0;
    int size = Gtk3_Text_Display_Storage.size();

    while (i < size)
    {
        if (Gtk3_Text_Display_Storage[i].use == 0)
        {
            Gtk3_Text_Display_Storage[i] = text_display;
            break;
        }

        i++;
    }

    if (i == size)
    {
        Gtk3_Text_Display_Storage.push_back(text_display);
    }

    return i;
}

void delete_a_gtk3_text_display(INDEX idx)
{
    Gtk3_Text_Display_Storage[idx].use = 0;
}



void create_a_Scene_Image_Attribute(INDEX node_idx, Scene_Image_Attribute attribute)
{
    Link_Variable_Attribute va = {.scene_image_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(va);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Scene_Image_Attribute(INDEX node_idx)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_list = the_node.Node_variable_attribute_list;
    int object_size = link_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_list[q].node_image_single_attribute.is_node_attribute == 2)
        {
            if (link_list[q].node_image_single_attribute.self_is_Node_Image_Attribute == 1)
            {
                link_list[q].node_image_single_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Scene_Image_Attribute read_a_Scene_Image_Attribute(INDEX node_idx)
{
    Scene_Image_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].scene_image_attribute.is_scene_attribute == 5)
        {
            if (the_node.self_attribute[q].scene_image_attribute.self_is_Scene_Image_Attribute == 1)
            {
                attribute = the_node.self_attribute[q].scene_image_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Scene_Image_Attribute(INDEX node_idx, Scene_Image_Attribute write_attribute)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_image_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_image_attribute.self_is_Scene_Image_Attribute == 1)
            {
                link_object_list[q].scene_image_attribute = write_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Number_Attribute(INDEX node_idx, Scene_Number_Attribute attribute)
{
    Link_Variable_Attribute va = {.scene_number_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(va);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Scene_Number_Attribute(INDEX node_idx)
{
    Scene_Number_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_number_attribute.self_is_Scene_Number_Attribute == 5)
        {
            if (link_object_list[q].scene_number_attribute.self_is_Scene_Number_Attribute == 2)
            {
                link_object_list[q].scene_image_attribute.is_scene_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Scene_Number_Attribute read_a_Scene_Number_Attribute(INDEX node_idx)
{
    Scene_Number_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].scene_number_attribute.is_scene_attribute == 5)
        {
            if (the_node.self_attribute[q].scene_number_attribute.self_is_Scene_Number_Attribute == 2)
            {
                attribute = the_node.self_attribute[q].scene_number_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;

}

void write_a_Scene_Number_Attribute(INDEX node_idx, Scene_Number_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_number_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_number_attribute.self_is_Scene_Number_Attribute == 2)
            {
                link_object_list[q].scene_number_attribute = write_attritube;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Scene_Time_Attribute(INDEX node_idx, Scene_Time_Attribute attribute)
{
    Link_Variable_Attribute fo = {.scene_time_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Scene_Time_Attribute(INDEX node_idx)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_time_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_time_attribute.self_is_Scene_Time_Attribute == 3)
            {
                link_object_list[q].scene_time_attribute.is_scene_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Scene_Time_Attribute read_a_Scene_Time_Attribute(INDEX node_idx)
{
    Scene_Time_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node &the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].scene_time_attribute.is_scene_attribute == 5)
        {
            if (the_node.self_attribute[q].scene_time_attribute.self_is_Scene_Time_Attribute == 3)
            {
                attribute = the_node.self_attribute[q].scene_time_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Scene_Time_Attribute(INDEX node_idx, Scene_Time_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].scene_time_attribute.is_scene_attribute == 5)
        {
            if (the_node.self_attribute[q].scene_time_attribute.self_is_Scene_Time_Attribute == 3)
            {
                the_node.self_attribute[q].scene_time_attribute = write_attritube;
                break;
            }
        }
    }
    
    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Scene_Text_Attribute(INDEX node_idx, Scene_Text_Attribute attribute)
{
    Link_Variable_Attribute va = {.scene_text_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(va);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Scene_Text_Attribute(INDEX node_idx)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_text_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_text_attribute.self_is_Scene_Text_Attribute == 4)
            {
                link_object_list[q].scene_text_attribute.is_scene_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Scene_Text_Attribute read_a_Scene_Text_Attribute(INDEX node_idx)
{
    Scene_Text_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    
    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].scene_text_attribute.is_scene_attribute == 5)
        {
            if (the_node.self_attribute[q].scene_text_attribute.self_is_Scene_Text_Attribute == 4)
            {
                attribute = the_node.self_attribute[q].scene_text_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Scene_Text_Attribute(INDEX node_idx, Scene_Text_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_text_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_text_attribute.self_is_Scene_Text_Attribute == 4)
            {
                link_object_list[q].scene_text_attribute = write_attritube;
                break;
            }
        }
    }
    
    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Scene_Action_Attribute(INDEX node_idx, Scene_Action_Attribute attribute)
{
    Link_Variable_Attribute fo = {.scene_action_attribute = attribute};

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& curr_node = General_Node_Storage[node_idx];
    curr_node.Node_variable_attribute_list.push_back(fo);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

void release_a_Scene_Action_Attribute(INDEX node_idx)
{
    Scene_Number_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_action_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_action_attribute.self_is_Scene_Action_Attribute == 5)
            {
                link_object_list[q].node_number_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Scene_Action_Attribute read_a_Scene_Action_Attribute(INDEX node_idx)
{
    Scene_Action_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].scene_action_attribute.is_scene_attribute == 5)
        {
            if (the_node.self_attribute[q].scene_action_attribute.self_is_Scene_Action_Attribute == 5)
            {
                attribute = the_node.self_attribute[q].scene_action_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Scene_Action_Attribute(INDEX node_idx, Scene_Action_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node the_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].scene_action_attribute.is_scene_attribute == 5)
        {
            if (link_object_list[q].scene_action_attribute.self_is_Scene_Action_Attribute == 5)
            {
                link_object_list[q].scene_action_attribute = write_attritube;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}




void push_back_a_node_to_scene(INDEX scene_idx, INDEX node_idx, char scene_kind)
{
    Link_Node_Attribute link;
    Link_Variable_Attribute va = {.link_node_attribute = link};
    
    if(scene_kind == 1)
    {
        char expect = 0;
        while( Occupy_Single_General_Scene[scene_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
            expect = 0;

        General_Scene& scene = General_Scene_Storage[scene_idx];
        scene.Overall_general_node_list.push_back(node_idx);
        General_Node& node = General_Node_Storage[node_idx];
        ID node_id = node.self_id;
        scene.Id_find_overall_node[node_id].push_back(node_idx);
        
        expect = 1;
        while( Occupy_Single_General_Scene[scene_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
            expect = 1;
        
    } else if(scene_kind == 2) {

        char expect = 0;
        while( Occupy_Single_Image_Scene[scene_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
            expect = 0;

        Image_Scene& scene = Image_Scene_Storage[scene_idx];
        scene.Overall_image_node_list.push_back(node_idx);
        General_Node& node = General_Node_Storage[node_idx];
        ID node_id = node.self_id;
        scene.Id_find_overall_node[node_id].push_back(node_idx);
        
        expect = 1;
        while( Occupy_Single_Image_Scene[scene_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
            expect = 1;
        
    } else if(scene_kind == 3) {
        
        char expect = 0;
        while( Occupy_Single_Time_Scene[scene_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
            expect = 0;

        Time_Scene& scene = Time_Scene_Storage[scene_idx];
        scene.Overall_time_node_list.push_back(node_idx);
        General_Node& node = General_Node_Storage[node_idx];
        ID node_id = node.self_id;
        scene.Id_find_overall_node[node_id].push_back(node_idx);
        
        expect = 1;
        while( Occupy_Single_Time_Scene[scene_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
            expect = 1;
        
    } else if(scene_kind == 4) {
        
        char expect = 0;
        while( Occupy_Single_Text_Scene[scene_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
            expect = 0;

        Text_Scene& scene = Text_Scene_Storage[scene_idx];
        scene.Overall_text_node_list.push_back(node_idx);
        General_Node& node = General_Node_Storage[node_idx];
        ID node_id = node.self_id;
        scene.Id_find_overall_node[node_id].push_back(node_idx);
        
        expect = 1;
        while( Occupy_Single_Text_Scene[scene_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
            expect = 1;
    }



}

Link_Variable_Attribute;

void push_back_a_link_to_scene(INDEX scene_idx, Link_Scene_Attribute scene_related, char scene_kind)
{
    Link_Variable_Attribute so = {.link_scene_attribute = scene_related};
    char expect = 0;
    while( Occupy_Single_General_Scene[scene_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Scene& scene = General_Scene_Storage[scene_idx];
    scene.Scene_variable_attribute_List.push_back(so);

    expect = 1;
    while( Occupy_Single_General_Scene[scene_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Link_Variable_Attribute read_a_scene_related_motion_detect(INDEX scene_idx)
{
    ;
}


INDEX get_a_highist_value_of_sort(Advanced_Value_Sort& Require_object_sort)
{
    INDEX idx = Require_object_sort.list[0].lower_one;
    
    // while()
    // {
    //     idx = Require_object_sort.list[idx].target_idx;
    // }

    return idx;
}

//
INDEX create_a_local_Match_Generate(
    vector<Match_Generate>& match_generate_List, vector<INDEX>& Free_match_generate_index,
    Match_Generate& added_image_match)
{
    INDEX idx;

    if(!Free_match_generate_index.empty())
    {
        idx = Free_match_generate_index.back();
        Free_match_generate_index.pop_back();
        match_generate_List[idx] = added_image_match;
    } else {
        idx = match_generate_List.size();
        match_generate_List.push_back(added_image_match);
    }

    return idx;
}

void release_a_local_Match_Generate(
    vector<Match_Generate>& match_generate_List,
    vector<INDEX>& Free_image_match_index,
    INDEX target_idx)
{
    match_generate_List[target_idx].attritube_kind = 0;
    Free_image_match_index.push_back(target_idx);
}


void get_a_local_focus_object(int example_number, vector<INDEX> &storage_list, vector<INDEX> &free_list)
{
    if (!free_list.empty())
    {
        storage_list[free_list.back()] = example_number;
        free_list.pop_back();
    
    } else {
        storage_list.push_back(example_number);
    }
}


INDEX set_a_overall_Require_Object(Require_Object require_object)
{
    INDEX idx;
    if(Free_Require_Object_Idx.size() != 0)
    {
        idx = Free_Require_Object_Idx.back();
        Free_Require_Object_Idx.pop_back();
        Require_Object_List[idx].require_object = require_object;

    } else {
        idx = Require_Object_List.size();
        Link_Variable_Attribute ro = {.require_object = require_object};
        Require_Object_List.push_back(ro);
    }
    
    return idx;
}

void clear_a_overall_Require_Object(INDEX idx)
{
    Free_Require_Object_Idx.push_back(idx);
}

INDEX set_a_overall_Generate_Match(Match_Generate rm)
{
    INDEX idx;
    if(Free_Match_Generate_Idx.size() != 0)
    {
        idx = Free_Match_Generate_Idx.back();
        Free_Match_Generate_Idx.pop_back();
        Match_Generate_List[idx] = rm;

    } else {
        idx = Match_Generate_List.size();
        Match_Generate_List.push_back(rm);
    }
    
    return idx;
}

void clear_a_overall_Generate_Match(INDEX idx)
{
    Free_Match_Generate_Idx.push_back(idx);
    Match_Generate_List[idx].curr_is_use = 0;
}

INDEX create_a_Formal_Require_Object(Require_Object require_object)
{
    INDEX idx;
    if(Free_Formal_Require_Object_Index.size() != 0)
    {
        idx = Free_Formal_Require_Object_Index.back();
        Free_Formal_Require_Object_Index.pop_back();
        All_Formal_Require_Object_List[idx].require_object = require_object;

    } else {
        idx = All_Formal_Require_Object_List.size();
        Link_Variable_Attribute va = {.require_object = require_object};
        All_Formal_Require_Object_List.push_back(va);
    }
    
    return idx;
}

void clear_a_Formal_Require_Object(INDEX idx)
{
    Free_Formal_Require_Object_Index.push_back(idx);
    All_Formal_Require_Object_List[idx].require_object.is_require = 0;
}


INDEX read_require_summary(ID target)
{
    vector<Focus_Object>& chose_list = Id_Find_Focus_Object[target];

    for(int a = 0; a < chose_list.size(); a++)
    {
        Focus_Object& fo = chose_list[a];

        if(fo.kind = 3)
            return fo.target_idx;
    }
    
    return 0;
}

void add_scene_a_require_object(INDEX scene_idx, Require_Object add_object)
{
    
}

void update_scene_a_require_value(vector< vector<int> >& require_list, INDEX require_idx)
{
    
}

void text_require_object_update(vector< vector<int> >& require_list, INDEX require_idx,
    int change_num)
{
    int outside_size = require_list.size();

    for(int a = 0; a < outside_size; a++)
    {
        require_list[a][require_idx] += change_num;
        require_idx /= 8;
    }
}





static auto last_staedy_time = std::chrono::steady_clock::now();
static int32_t last_system_seconds = 0;

int get_current_second()
{
    auto system_now = std::chrono::system_clock::now();

    int current_system_seconds = std::chrono::duration_cast<std::chrono::seconds>(system_now.time_since_epoch()).count();

    if (current_system_seconds < last_system_seconds)
    {
        std::cout << "警告：系统时间异常回拨";
    }
    else
    {
        last_system_seconds = current_system_seconds;
    }

    return current_system_seconds;
}


bool check_time_updated()
{
    static auto last_time = std::chrono::system_clock::now();

    auto current_time = std::chrono::system_clock::now();
    auto current_sec = std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch());
    auto last_sec = std::chrono::duration_cast<std::chrono::seconds>(last_time.time_since_epoch());

    if (current_sec > last_sec)
    {
        last_time = current_time;
        return true;
    }
    return false;
}


int stat_gap_time()
{
    auto steady_now = std::chrono::steady_clock::now();
    int steady_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(steady_now - last_staedy_time).count();
    last_staedy_time = steady_now;

    return steady_elapsed;
}


chrono::milliseconds hundred_ms(100);


inline long long int_time_to_longlong_time(int front_time, int back_time)
{
    long long true_time = front_time;
    true_time = true_time << 32;
    true_time |= back_time;
    return true_time;
}

inline void longlong_time_to_int_time(long long true_time, int& front_time, int& back_time)
{
    front_time = true_time >> 32;
    back_time = true_time;
}