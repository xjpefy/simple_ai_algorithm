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





atomic<long long> Code_Time = 0;


char IF_SELF_TURN_OFF = 1;
char IF_SELF_COMMUNIATE = 0;
char IF_SELF_CONNECT = 0;
char IF_SELF_WORKING = 0;


char IF_NEED_COMMUNIATE = 1;
char IF_NEED_WORKING = 1;
char IF_NEED_CAPTURE = 0;
char IF_NEED_ACTION = 0;

atomic<ID> NEWIST_USEFUL_ID = 1;
atomic<long long> CURRENT_MODEL_TIME = 100;
atomic<unsigned int> ALL_NUM_OF_CH = 0;
atomic<unsigned int> LAST_PACK_NUMBER = 0;

atomic<int> FREE_ID_NUM = 0;
vector<ID> FREE_ID_LIST;
atomic<int> Free_Id_Ocuppy(0);

atomic<int> CURRENT_SYSTEM_MEMORY;


char time_curr = 0;
atomic<long long> use_time[4] = {0, 0, 0, 0};
atomic<int> time_node_occupy[4] = {0, 0, 0, 0};
atomic<int> node_total_occupy = 0;


struct state_record_file
{
    ID newist_useful_id = 1;
    unsigned int all_num_of_ch = 0;
    unsigned int last_pack_num = 0;

    char time_curr = 0;
    int time_node_occupy[4] = {0, 0, 0, 0};
    long long use_time[4] = {0, 0, 0, 0};
    long long node_total_occupy = 0;

    long long current_model_time = 100;
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

    int use_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int history_use_num[4] = {0, 0, 0, 0};
};
// 14*4

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

        } else { // 鏂板缓
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
        { // 瑕嗙洊
            int expected = 0;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 1, memory_order_seq_cst) )
                expected = 0;

            idx = Free_2_Pack_Index_List.back();
            Free_2_Pack_Index_List.pop_back();

            expected = 1;
            while( !Occupy_Neuro_Pack_Record_Storage.compare_exchange_strong(expected, 0, memory_order_seq_cst) )
                expected = 1;

            //涓夐噸妫拷楠�1锟�7
            char expected_char = 0;

            //瀛樺偍杩炵画鎬ф楠�1锟�7
            while(curr_check < pack_size && suitable_success != 0)
            {
                //鎻愬墠绛涘嚭
                if(Neuro_Pack_Record_Storage[idx].pack_state != 0)
                {
                    suitable_success = 0;
                    break;
                }

                //闈炲啓
                while(!Neuro_Pack_Write_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;
                
                //闈炶
                while(!Neuro_Pack_Read_Occupy[idx].compare_exchange_strong(expected_char, 1, memory_order_seq_cst))
                    expected_char = 0;

                //闈炲崰鐢�1锟�7
                if(Neuro_Pack_Record_Storage[idx].pack_state == 0)
                {
                    cover_check[curr_check] = idx;
                    curr_check++;

                    if(curr_check == pack_size - 1)
                    {
                        suitable_success = 2;
                        curr_check = 0;
                        first_idx = cover_check[0];
                        
                        //鍏ㄤ綋鍖呭～鍐�1锟�7
                        while(curr_check < pack_size)
                        {
                            idx = cover_check[curr_check];
                            Neuro_Pack_Record_Storage[idx].pack_size = pack_size;
                            Neuro_Pack_Record_Storage[idx].pack_state = 1;

                            //瑙ｉ櫎鍗犵敤
                            Neuro_Pack_Write_Occupy[idx].fetch_sub(1);
                            Neuro_Pack_Read_Occupy[idx].fetch_sub(1);
                        }
                    }//鎴愬姛鎵惧埌杩炵画浣嶇疆

                } else {
                    suitable_success = 0;
                    break;
                }
            }
            
        }// 瑕嗙洊缁撴潫 鎴�1锟�7 澶辫触
        
        if (Free_2_Pack_Index_List.empty() && suitable_success != 2)
        {
            // 鏂板缓
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
        { // 瑕嗙洊
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

int gap_time = 16 * 256;

long long time_update_gap = 0xFFFFFFFFFFFFF000;

void Neuro_Use_Record_Update(ID target_id)
{
    INDEX idx = ID_Find_Pack_Index[target_id];
    Pack_Record& pack_record = Neuro_Pack_Record_Storage[idx];

    long long curr_time = 0;
    long long front = pack_record.use_time[pack_record.time_curr * 2];
    front = front << 32;
    long long back = pack_record.use_time[pack_record.time_curr * 2 + 1];
    curr_time |= front;
    curr_time |= back;

    if (curr_time != (CURRENT_MODEL_TIME & time_update_gap) )
    {
        pack_record.time_curr += 1;

        if (pack_record.time_curr >= 4)
            pack_record.time_curr = 0;

        curr_time = CURRENT_MODEL_TIME & time_update_gap;

        pack_record.use_time[pack_record.time_curr * 2] = curr_time >> 32;
        pack_record.use_time[pack_record.time_curr * 2 + 1] = curr_time & 0xffffffff;
    }

    pack_record.history_use_num[pack_record.time_curr] += 1;
    // 鏇存柊tory_use_num[pack_record.time_curr] += 1;

    time_node_occupy[time_curr] += 1;

} // 浣跨敤娆℃暟鏇存柊鍑芥暟缁撴潫


// 鍐峰寘鍓旈櫎
void Pack_Clear_Thread()
{
    //闈為噸瑕佸唴瀛樻暟鎹墧闄�1锟�7
    int Half_Pack_Limit = Pack_Limit * 0.5;

    while (IF_SELF_TURN_OFF)
    {
        Pack_Attract_Rate = (Half_Pack_Limit - Current_Pack_Num.load()) / Half_Pack_Limit;

        for (int a = 0; a < 4; a++)
            node_total_occupy += time_node_occupy[a];

        float ave_occupy = node_total_occupy / (Current_Pack_Num.load() + 1);

        // 浣跨敤棰戝害 鏁版嵁鍗犵敤绋嬪害锛屽彇鏃堕棿浣跨敤鎬�1锟�7
        int i = 1;
        while (i < Neuro_Pack_Storage.size())
        {
            Pack_Record& pack_record = Neuro_Pack_Record_Storage[i];

            if (pack_record.pack_state == 0 || pack_record.pack_state != 2)
                continue;

            int curr_pack_total_history_use_num = 0;
            long long low_permit = use_time[time_curr] - 4 * gap_time;

            for (int b = 0; b < 4; b++)
            {
                if (*(long long *)(pack_record.use_time + 2 * b) < low_permit)
                    continue;
                
                curr_pack_total_history_use_num += pack_record.history_use_num[b];
            }

            float Curr_Pack_Attract = (curr_pack_total_history_use_num - ave_occupy) / ave_occupy;

            // 瑙﹀彂鍒犻櫎鍒ゅ畾
            if (CURRENT_MODEL_TIME - *(long long *)(pack_record.use_time + pack_record.time_curr * 2) > 360000 // 360绉掓湭浣跨敤
                || Curr_Pack_Attract + Pack_Attract_Rate < 0)
            {
                // 鍖呬娇鐢ㄨ褰�1锟�7 鍒犻櫎
                pack_record.pack_state = 6;

                // 鍒犻櫎鍖�1锟�7
                add_to_Will_Write_Neuro_Queue(pack_record.pack_id);
            } // 瑙﹀彂鍒犻櫎鍒ゅ畾 瑙ｅ喅
            i++;

        } // 閬嶅巻瀹屼竴涓寘

        i = 1;
    } // 閲嶅宸ヤ綔
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
    char neuro_is_concept = 4;
    char be_restrain_num = 0;
    char limit_predict_num = 8;
    char predict_stability_num = 0; // 8/16/32

    int neuro_active_num = 0;
    int self_attention = 0;

    float predict_stability = 0;
};


struct Neuro_Image_Desc
{
    char neuro_is_image = 1;
    char detail_kind;
    char limit_predict_num = 8;
    char all_predict_stability_num = 0; // 8/16/32

    int neuro_active_num = 0;
    int self_attention = 0;
    
    float predict_stability_8 = 0;
    // float predict_stability_32 = 0;
    float forward_predict_stability = 0;
    float backward_predict_stability = 0;

    // 
    float upward_predict_ability = 0;
    float downward_predict_ability = 0;
    float left_predict_ability = 0;
    float right_predict_ability = 0;

    float appear_worth = 0;

    float puruse = 0;

    //9

    // unsigned char red;
    // unsigned char green;
    // unsigned char blue;

    // unsigned char length;
    // unsigned char direction;


}; // 15*4

struct Neuro_Time_Desc
{
    char neuro_is_time = 2;
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
    char neuro_is_text = 3;
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
    Neuro_Image_Desc image_desc;
    Neuro_Text_Desc text_desc;
    Neuro_Concept_Desc concept_desc;
};

//鍥惧儚鏉＄洰
struct Neuro_Image_Item
{
    char is_neuro_item = 1;
    char item_is_image = 1;
    char logic = 1;
    char related_scale;

    char direction;
    char direction_scale = 3;
    char distance;
    char distance_scale = 3;
    
    ID related_id;
};
// 3*4

struct Neuro_Colour_Item
{
    char item_is_colour;
    char red;
    char green;
    char blue;
};


struct Self_Cognition_Item
{
    unsigned char coding_mode = 0;
    unsigned char self_inside_I = 0;
    unsigned char self_inside_II = 0;
    unsigned char self_inside_III = 0;
};
// 1*4


struct Initiative_Observation_Item
{

    ID target;
};


struct Neuro_Number_Item
{
    char is_neuro_item = 1;
    char item_is_number = 2;
    char detail = 0;
    char logic = 0;

    int number_I = 0;
    int number_II = 0;
};
// 3*4


struct Neuro_Time_Item
{
    char is_neuro_item = 1;
    char item_is_time = 3;
    char logic = 0;

    char time_left_range;
    char time_right_range;
    char time_scale;

    char effect_kind;

    ID related_id;
};
// 3*4


struct Neuro_Text_Item
{
    char is_neuro_item = 1;
    char item_is_text = 4;
    char logic = 0;
    char effect_kind;

    char left_distance = 1;
    char right_distance = 1;
    unsigned char distance_scale = 4;
    
    ID target_id;
};
// 3*4


struct Neuro_Concept_Item
{
    char is_neuro_item = 1;
    char item_is_concept = 5;
    char logic = 0;

    ID target_id;
};
// 2*4


struct Neuro_Action_Item
{
    char is_neuro_item = 1;
    char item_is_action = 6;
    char logic;

    char action_kind;
    unsigned char vk;
    char mouseData;
    
    char direction;
    char distance;

    unsigned int dwFlags;

    ID locate_id;
};
// 4*4


struct Neuro_Belief_Item
{
    char is_neuro_item = 1;
    char item_is_belief = 7; // 
    char detail_kind;
    char logic;

    int u_value = 0;
    int l_value = 0; 
    
    ID belief_object_id;
};
//4*4


struct Neuro_Manner_Item
{
    char is_neuro_item = 1;
    char item_is_manner = 8;
    char manner_kind;
    char target_kind;
    
    int manner_value = 0;
    float manner_rate = 0;
    ID effect_target;
};
//4*4



struct Neuro_Casual_Restrain_Item
{
    ID Restrain_Target;
    
};

Neuro_Image_Item filler_condition;
Neuro_Image_Item filler_result;


union Neuro_Link_Attribute
{
    Neuro_Image_Item image_attribute;
    Neuro_Number_Item number_attribute;
    Neuro_Time_Item time_attribute;
    Neuro_Text_Item text_attribute;
    Neuro_Concept_Item concept_attribute;
    Neuro_Action_Item action_attribute;
    Neuro_Belief_Item belief_attribute;
    Neuro_Manner_Item manner_attribute;
};


union General_Condition
{
    Neuro_Image_Item image_condition;
    Neuro_Number_Item number_condition;
    Neuro_Time_Item time_condition;
    Neuro_Text_Item text_condition;
    Neuro_Concept_Item concept_condition;
    Neuro_Action_Item action_conditon;
};


union General_Result
{
    Neuro_Image_Item image_result;
    Neuro_Number_Item number_result;
    Neuro_Time_Item time_result;
    Neuro_Text_Item text_result;
    Neuro_Concept_Item concept_result;
    Neuro_Belief_Item belief_result;
    Neuro_Manner_Item manner_result;
};


struct General_Flow_Attribute
{
    //
    unsigned char attritube_kind;
    unsigned char update_kind = 0;
    unsigned char number_of_condition_and_result;
    unsigned char non;
    
    int active_stat_num;
    int realize_detect_num;

    //
    General_Condition condition[4];
    General_Result result[4];
    //

}; //(4~36)*4

struct Attribute_Head_Item
{
    char attribute_kind;
    char able_update = 0;
    
    char condition_num = 0;

    char result_num = 0;

    char item_num;
};

char every_item_size[8] = {3, 3, 3, 3, 2, 4, 4, 4};



void return_item_kind_and_size(int*& ptr, char& item_kind, char& item_size)
{
    ptr += item_size;
    item_kind = *(char*)(ptr);
    item_size = every_item_size[item_kind];
}



Attribute_Head_Item attribute_head_read(int *ptr)
{
    char one = *(char *)ptr;
    char two = *((char *)ptr + 1);
    char three = *((char *)ptr + 2);
    char four = *((char *)ptr + 3);

    Attribute_Head_Item return_item;

    return_item.attribute_kind = one;
    return_item.able_update = two;
    
    return_item.condition_num = three >> 4;
    return_item.result_num = three & 0xf;

    return return_item;
}


void attribute_head_write(Attribute_Head_Item attribute_head, vector<int>& target_list)
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

}


void general_attribute_put(
    Attribute_Head_Item attribute_head, vector<int>& target_context_list,
    vector<General_Condition>& general_condition_list, vector<General_Result>& general_result_list)
{
    attribute_head_write(attribute_head, target_context_list);
    
    char condition_kind;
    char result_kind;

    char condition_num = general_condition_list.size();
    char result_num = general_result_list.size();

    int* ptr = target_context_list.data();
    *(ptr + 1) = 0; 
    *(ptr + 2) = 0;
    ptr += 3;

    switch(condition_kind)
    {
        case 1:
        {
            for(int a = 0; a < condition_num; a++)
            {
                general_condition_list[a].image_condition;

                ptr += 3;
            }
        }
        break;
        case 2:
        {
            for(int a = 0; a < condition_num; a++)
            {
                general_condition_list[a].number_condition;

                ptr += 3;
            }
        }
        break;
        case 3:
        {
            for(int a = 0; a < condition_num; a++)
            {
                general_condition_list[a].time_condition;

                ptr += 3;
            }
        }
        break;
        case 4:
        {
            for(int a = 0; a < condition_num; a++)
            {
                general_condition_list[a].text_condition;

                ptr += 3;
            }
        }
        break;
        case 5:
        {
            for(int a = 0; a < condition_num; a++)
            {
                general_condition_list[a].concept_condition;

                ptr += 2;
            }
        }
        break;
        case 6:
        {
            for(int a = 0; a < condition_num; a++)
            {
                general_condition_list[a].action_conditon;

                ptr += 4;
            }
        }
        break;
    }

    switch(result_kind)
    {
        case 1:
        {
            for(int b = 0; b < result_num; b++)
            {
                general_result_list[b].image_result;

                ptr += 3;
            }
        }
        break;
        case 2:
        {
            for(int b = 0; b < result_num; b++)
            {
                general_result_list[b].number_result;

                ptr += 3;
            }
        }
        break;
        case 3:
        {
            for(int b = 0; b < result_num; b++)
            {
                general_result_list[b].time_result;

                ptr += 3;
            }
        }
        break;
        case 4:
        {
            for(int b = 0; b < result_num; b++)
            {
                general_result_list[b].text_result;

                ptr += 3;
            }
        }
        break;
        case 5:
        {
            for(int b = 0; b < condition_num; b++)
            {
                general_result_list[b].concept_result;

                ptr += 2;
            }
        }
        break;
        case 6:
        {
            for(int b = 0; b < condition_num; b++)
            {
                general_result_list[b].belief_result;

                ptr += 4;
            }
        }
        break;
        case 7:
        {
            for(int b = 0; b < condition_num; b++)
            {
                general_result_list[b].manner_result;
    
                ptr += 4;
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
void passive_link_put(Passive_Link_Attribute passive_link_attritube, 
    vector<int>& target_list)
{
    target_list.resize(2);
    int* ptr = target_list.data();
    *((char*)ptr) = passive_link_attritube.kind_is_passive;
    *((char*)ptr + 1) = passive_link_attritube.link_attribute_kind;
    *(ptr + 1) = passive_link_attritube.target_id;
}


unordered_set<ID> waiting_modify_neuro_search;
deque<ID> waiting_modify_neuro;
deque<vector<int>> waiting_modify_context;


void modify_neuro_queue_load(ID target, vector<int> context)
{
    if (waiting_modify_neuro_search.count(target) == 1)
    {

    } else {
        waiting_modify_neuro.push_back(target);
        waiting_modify_context.push_back(context);
        waiting_modify_neuro_search.insert(target);
    }
}


ID new_neuro_create(char neuro_kind/* 1 2 3 */, Neuro_Desc neuro_desc)
{
    ID node_id;

    if (FREE_ID_NUM > 0)
    {
        FREE_ID_NUM--;
        node_id = FREE_ID_LIST.back();
        FREE_ID_LIST.pop_back();
    }
    else
    {
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
        
        Neuro_Text_Desc desc = neuro_desc.text_desc;
        *(Neuro_Text_Desc*)ptr = desc;

    } else if(neuro_kind == 3) {
        
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


char neuro_size_up(unsigned int target_id, unsigned char level)
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

        modify_neuro_queue_load(target_ID, item_context);
        return 0;
    }

    int *first_ptr = ptr;
    char *pack_state = ((char *)(first_ptr - 15) + 1);

    if (*pack_state != 2)
        return 0;

    *pack_state = 4;
    char pack_size = *((char *)(first_ptr) + 1);
    unsigned short used_item_num = *((short *)(first_ptr) + 1);

    int item_context_size = item_context.size();

    if ((pack_size * 256 - 16 - used_item_num) > item_context_size)
    {
        vector<int> context;
        modify_neuro_queue_load(target_ID, context);
        return 0;
    }

    *((short *)first_ptr + 1) += item_context_size;

    char curr_pack_order = 0;

    unsigned short curr_order;

    bool have_more_pack = 0;

    if (pack_size > 1)
        have_more_pack = 1;
    

    if (add_or_insert == 1)
    {
        ptr += used_item_num;

        for (int single_item : item_context)
        {
            *ptr = single_item;
        }

    } else if (add_or_insert == 2) {

        char pack_size = *((char *)first_ptr + 1);
        unsigned short used_item_num = *((short *)first_ptr + 1);

        int insert_pos = position;

        int *write_ptr;
        int *end_ptr;
        int middle_item_num = used_item_num - insert_pos;

        if (have_more_pack == 0)
        {
            write_ptr = first_ptr + used_item_num;
            end_ptr = first_ptr + used_item_num + item_context_size;

            for (int q = 0; q < middle_item_num; q++)
            {
                *end_ptr = *write_ptr;
                end_ptr--;
                write_ptr--;
            }
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


inline void Simple_Neuro_Attribute_write(ID target_id, 
    unsigned short position, bool add_or_insert,
    Attribute_Head_Item attribute_head, 
    vector<General_Condition>& general_condition_list, vector<General_Result>& general_result_list)
{
    vector<int> add_item;
    vector<int> passive_item;

    Passive_Link_Attribute passive_link_attribute;
    passive_link_attribute.link_attribute_kind = attribute_head.attribute_kind;
    passive_link_attribute.target_id = target_id;

    general_attribute_put(attribute_head, add_item, general_condition_list, general_result_list);
    passive_link_put(passive_link_attribute, passive_item);

    neuro_attribute_write(target_id, add_item, 0, 1);
    neuro_attribute_write(general_result_list[0].image_result.related_id, passive_item, 0, 1);
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


struct Value_Sort_Unit {
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

        change_number += 1;

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

        change_number -= 1;

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
        int ture_numder = list.size() - free_idx.size();

        if( ture_numder >= (check_gap+1)*(check_gap+1) )
        {
            int old_enter_gap = check_gap;

            check_gap += 1;

            int current_No = 1;

            INDEX target_idx = fast_check_list[1].target_idx;

            int write_idx = 2;

            //1銆�1锟�7
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

            while( current_No < ture_numder )
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

        } else if (ture_numder <= (check_gap-1)*(check_gap-1) ) {

            int old_enter_gap = check_gap;

            check_gap -= 1;

            int current_No = 1;

            INDEX target_idx = fast_check_list[1].target_idx;

            int write_idx = 2;

            int number_of_this_unit = 0;

            for( ; write_idx < fast_check_list.size() && current_No < ture_numder; write_idx++)
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


struct Node_Filler_Attribute
{
    char curr_is_use = 0;
};

Node_Filler_Attribute filler_attribute;

struct Node_Image_Attribute
{
    char is_node_attribute = 2;
    char kind_is_Node_Image_Attribute = 1;
    char observe_size;

    short x, y;

    int block_num = 0;
    int contain_distance = 0;
};
//4*4

struct Node_Number_Attribute
{
    char is_node_attribute = 2;
    char kind_is_Node_Number_Attribute = 2;
    char logic; // node_number1 Probability2
    
    int number_I = 0;
    int number_II = 0;
};
//3*4


struct Node_Time_Attribute
{
    char is_node_attribute = 2;
    char kind_is_Node_Time_Attribute = 3;

    char time_range;
    char time_scale;
    char time_logic = 0;

    int front_time = 0;
    int back_time = 0;
};
//4*4

struct Node_Text_Attribute
{
    char is_node_attribute = 2;
    char kind_is_Node_Text_Attribute = 4;

    INDEX idx = 0;
    int block_num = 0;
};
//3*4


struct Node_Action_Attribute
{
    char is_node_attribute = 2;
    char kind_is_Node_Motion_Attribute = 5;

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
    char is_node_link = 3;
    char link_Attribute = 0;
    char link_Kind = 0;
    char connect_direction = 0;
    
    uint32_t link_idx_or_id = 0;
    float link_value = 1;
    int link_value_II = 0;
};
//4*4



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

    Neuro_Link_Attribute record_item;
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
    char need_condition_num;
    char result_num;
    char node_num = 0;

    INDEX node_idx_from[5];
    General_Condition match_condition[4] = {filler_condition, filler_condition, filler_condition, filler_condition};
    
    General_Result match_result[4] = {filler_result, filler_result, filler_result ,filler_result};
};


struct Casual_Restrain
{
    INDEX Restrain_Source;
    ID Restrain_Target;

    float Restrain_Rate;
};


struct Require_Object
{
    char is_require = 4;
    char require_object_kind;
    char require_detail_kind;
    char require_kind;

    uint32_t require_id_or_idx;
    int require_value = 0;
};
//3*4


struct Scene_Image_Attribute
{
    char is_scene_attribute = 5;
    char Self_Scene_Attribute_Is_Image = 1;
    char related_kind = 2;
    
    short left, right, bottom, top;
};
// 3*4


struct Scene_Number_Attribute
{
    char is_scene_attribute = 5;
    char Self_Scene_Attribute_Is_Number = 2;
    char related_kind = 2;
    

    int u_number;
    int l_number;
};
//3*4


struct Scene_Time_Attribute
{
    char is_scene_attribute = 5;
    char Self_Scene_Attribute_Is_Time = 3;
    char related_kind = 2;
    
    int begin_time_front;
    int begin_time_end;

    int durable_over_time;
};
// 4*4


struct Scene_Text_Attribute
{
    char is_scene_attribute = 5;
    char Self_Scene_Attribute_Is_Text = 4;
    char related_kind = 2;
    
    short left, right;
};
// 2*4


struct Scene_Action_Attribute
{
    char is_scene_attribute = 5;
    char Self_Scene_Attribute_Is_Motion = 5;
    char related_kind = 2;
    char last_or_curr;

    INDEX action_idx;

    int time_front;
    int time_end;
};
//4*4



struct Link_Scene_Attribute
{
    char is_scene_link = 6;
    unsigned char scene_kind;
    char related_kind;

    INDEX scene_idx;
    float related_value;
};
// 3*4


union Variable_Attribute
{
    Node_Filler_Attribute filler_attribute;

    Node_Image_Attribute node_image_attribute;
    Node_Time_Attribute node_time_attribute;
    Node_Text_Attribute node_text_attribute;
    Node_Number_Attribute node_number_attribute;
    Node_Action_Attribute node_action_attribute;

    Link_Node_Attribute link_node_attribute;

    Scene_Image_Attribute scene_image_attribute;
    Scene_Number_Attribute scene_number_attribute;
    Scene_Time_Attribute scene_time_attribute;
    Scene_Text_Attribute scene_text_attribute;
    Scene_Action_Attribute scene_action_attribute;

    Link_Scene_Attribute link_scene_attribute;

    Require_Object require_object;

    Neuro_Image_Item neuro_image_item;
    Neuro_Number_Item neuro_number_item;
    Neuro_Time_Item neuro_time_item;
    Neuro_Text_Item neuro_text_item;
    
    // Neuro_Concept_Item neuro_concept_item;
    // Neuro_Action_Item neuro_action_item;
    // Neuro_Belief_Item neuro_belief_item;
    // Neuro_Manner_Item neuro_manner_item;
};


struct General_Node
{
    bool is_use = 1;
    char now_occupy = 0;
    char node_kind;
    char belong_scene_kind = 0;

    unsigned char node_layer;
    unsigned char neuro_spare_size;
    char node_calu_state = 1;
    char space_stability_num;
    char time_stability_num;

    char was_active = 0;

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

    Variable_Attribute self_attribute[3] = {filler_attribute, filler_attribute, filler_attribute};
    vector<Variable_Attribute> Node_variable_attribute_list;
};


struct General_Scene
{
    bool curr_use = 1;
    char scene_kind;

    int importance = 0;
    int total_invest_resource = 0;

    int scene_attention = 0;

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
    unordered_map<ID, vector<INDEX>> Id_find_overall_node;

    vector<General_Node> Local_general_node_list;
    vector<INDEX> Free_local_general_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_local_node;

    unordered_map<ID, vector<Focus_Object> > Id_find_focus_object;

    vector<Match_Generate> Generate_match_list;
    vector<INDEX> Free_generate_match_idx;

    vector<Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<INDEX> Free_require_object_idx;
    Advanced_Value_Sort Require_object_sort;

    Advanced_Value_Sort Attention_object_sort;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Node_combo_find_repeat;
    
    Variable_Attribute self_scene_attribute[3] = {filler_attribute, filler_attribute, filler_attribute};
    vector<Variable_Attribute> Scene_variable_attribute_List;

    unordered_map<long long , INDEX> Info_to_scene;

    unordered_map< long long, INDEX > every_4_time_event;
    unordered_map< long long, INDEX > every_16_time_event;
    unordered_map< long long, INDEX > every_64_time_event;
};


struct Model_Output_Action
{
    char action_order;
    char action_kind;
    unsigned char vk;
    char mouseData;

    short x, y;
    
    unsigned int dwFlags;
};



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
    char nave_init = 0;
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


struct Rigion_Value_Stat
{
    unsigned short height;
    unsigned short width;
    unsigned short height_unit_size;
    unsigned short width_unit_size;

    vector<int> value_unit_list;
};


struct Image_Scene
{
    char curr_use = 1;
    char have_init = 0;
    char is_lock_occupy = 0;
    char model_output_action_num = 0;

    short width, height;
    
    INDEX last_map = 0;
    INDEX next_map = 0;

    int attention = 0;
    int sum_nature_attention = 0;

    int sum_retrieve_require = 0;
    int sum_model_space_require = 0;
    int sum_model_time_require = 0;
    int sum_construct_require = 0;

    float retrieve_add_rate = 1;
    float model_space_add_rate = 0.5;
    float model_time_add_rate = 0.5;
    float construct_add_rate = 0;

    int image_node_num = 0;

    Model_Output_Action Current_output_action[5];

    INDEX Action_Node_Idx[5] = {0, 0, 0, 0, 0};

    long long input_time;

    vector<RGB_Unit> RGB_Map;


    char Image_rough_view_number = 0;
    Image_Rough_Map Image_rough_view[4];

    vector<Rigion_Value_Stat> Block_attention_list;

    char require_distance = 1;

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

    char record_distance = 1;
    vector< vector<INDEX> > Node_space_form_record_list;

    unordered_map<ID, vector<Focus_Object> > Id_find_focus_object;

    vector<Match_Generate> Match_generate_list;
    vector<INDEX> Free_match_generate_index;
    unordered_map<ID, vector<INDEX>> Id_find_image_match;

    vector<Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<INDEX> Free_require_idx;
    Advanced_Value_Sort Require_object_sort;

    Advanced_Value_Sort Attention_object_list;

    unordered_set<vector<ID>, VectorUint32_tHash, VectorUint32_tEq> Node_combo_find_repeat;

    Variable_Attribute self_scene_attribute[3] = {filler_attribute, filler_attribute, filler_attribute};
    vector<Variable_Attribute> Scene_variable_attribute_List;
};


struct Time_Scene
{
    char curr_use = 1;
    char is_lock_occupy = 0;
    
    int attention = 0;

    int sum_retrieve_require = 0;
    int sum_model_time_require = 0;
    int sum_construct_require = 0;

    int time_node_num = 0;

    long long begin_time;
    long long end_time;

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

    vector<long long> Single_record_time_list;
    vector< vector<INDEX> > Single_node_time_from_record_list;
    vector<Advanced_Value_Sort> Single_Time_attention_sort;

    unordered_set<vector<ID>, VectorUint32_tHash, VectorUint32_tEq> Node_combo_find_repeat;

    unordered_map<ID, vector<Focus_Object> > Id_find_focus_object;

    vector<Match_Generate> Match_generate_list;
    vector<INDEX> Free_match_generate_index;
    unordered_map<ID, vector<INDEX>> Find_match_generate_need;

    vector<Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<INDEX> Free_require_idx;
    vector< vector<int> > Require_object_weights_map;
    Advanced_Value_Sort Require_object_sort;
    
    Advanced_Value_Sort Attention_object_sort;

    Variable_Attribute self_scene_attribute[3] = {filler_attribute, filler_attribute, filler_attribute};
    vector<Variable_Attribute> Scene_variable_attribute_List;
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

    vector< vector<INDEX> > Text_node_space_from_record_list;

    vector< INDEX > Overall_text_node_list;
    vector<INDEX> Free_overall_text_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_overall_node;

    vector<General_Node> Local_text_node_list;
    vector<INDEX> Free_local_text_node_idx;
    unordered_map<ID, vector<INDEX> > Id_find_local_node;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Node_combo_find_repeat;

    //
    unordered_map<ID, vector<Focus_Object> > Id_find_focus_list;

    vector<Variable_Attribute> Require_object_list;
    vector< atomic<char> > Occupy_single_require_object_list;
    vector<int> Require_object_value;
    Advanced_Value_Sort Require_object_sort;

    Advanced_Value_Sort Attention_object_list;

    Variable_Attribute self_scene_attribute[3] = {filler_attribute, filler_attribute, filler_attribute};
    vector<Variable_Attribute> Scene_variable_attribute_List;
};



struct Gtk3_Text_Display
{
    char use = 1;
    char have_init = 0;
    char response_aim_kind;
    
    char input_value_num = 0;
    char input_value_kind;

    int input_value_I[4];

    char output_value_num = 0;
    char output_value_kind;
    
    int output_value[4];

    INDEX input_Text_Scene_Idx = 0;
    INDEX output_Text_Scene_Idx = 0;

    vector<char> Input_string;
    vector<char> Output_string;
};




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

vector<Model_Output_Action> Alreadly_Send_Action_List;

vector<char> Send_Text_List;
atomic<bool> Text_Is_Send(0);

vector<char> Receive_Text_List;


atomic<int> Total_Task_Attention = 100 * Calculate_Thread_Num; 
atomic<int> Free_Total_Task_Attention = 100 * Calculate_Thread_Num;

unordered_map< ID, vector<Focus_Object> > Id_Find_Focus_Object;


vector<Require_Object> Attention_List;
vector<INDEX> Free_Attention_Inedx;

vector<Variable_Attribute> Require_Object_List;
vector<INDEX> Free_Require_Object_Idx;
atomic<long long> Total_Require_Value;

Advanced_Value_Sort Total_Require_Sort;


vector<Match_Generate> Match_Generate_List;
vector<INDEX> Free_Match_Generate_Idx;

vector<Wait_Added_Record> Wait_Added_Record_List;
vector<INDEX> Free_Wait_Added_Record_Idx;


vector<Variable_Attribute> All_Formal_Require_Object_List;
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


vector<General_Scene> General_Scene_Storage;
vector< atomic<char> > Occupy_Single_General_Scene;
atomic<int> Occupy_General_Scene_Storage(0);

vector<INDEX> Free_General_Scene_Index;
atomic<int> Occupy_Free_General_Scene_Index(0);

atomic<int> General_Scene_Num = 1;


vector<Image_Scene> Image_Scene_Storage;
vector< atomic<char> > Occupy_Single_Image_Scene;
atomic<int> Occupy_Image_Scene_Storage(0);

vector<INDEX> Free_Image_Scene_Index;
atomic<int> Occupy_Free_Image_Scene_Index(0);

atomic<int> Image_Scene_Num = 1;

unordered_map<long long, INDEX> Time_Find_Image_Scene;
INDEX Newist_Image_Idx = 0;


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


void push_back_a_variable_object_to_General_Node(INDEX node_idx, Variable_Attribute variable_object)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Node_Storage[node_idx].Node_variable_attribute_list.push_back(variable_object);

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Image_Attribute(INDEX node_idx, Node_Image_Attribute attribute)
{
    Variable_Attribute fo = {.node_image_attribute = attribute};

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
    vector<Variable_Attribute>& link_list = the_node.Node_variable_attribute_list;
    int object_size = link_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_list[q].node_image_attribute.is_node_attribute == 2)
        {
            if (link_list[q].node_image_attribute.kind_is_Node_Image_Attribute == 1)
            {
                link_list[q].node_image_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Node_Image_Attribute read_a_Node_Image_Attribute(INDEX node_idx)
{

    Node_Image_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_image_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_image_attribute.kind_is_Node_Image_Attribute == 1)
            {
                attribute = the_node.self_attribute[q].node_image_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Node_Image_Attribute(INDEX node_idx, Node_Image_Attribute write_attribute)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_image_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_image_attribute.kind_is_Node_Image_Attribute == 1)
            {
                link_object_list[q].node_image_attribute = write_attribute;
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
    Variable_Attribute fo = {.node_number_attribute = attribute};

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
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_number_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_number_attribute.kind_is_Node_Number_Attribute == 3)
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
            if (the_node.self_attribute[q].node_number_attribute.kind_is_Node_Number_Attribute == 3)
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
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_number_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_number_attribute.kind_is_Node_Number_Attribute == 5)
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


void create_a_Node_Time_Attribute(INDEX node_idx, Node_Time_Attribute attribute)
{
    Variable_Attribute fo = {.node_time_attribute = attribute};

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
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_time_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_time_attribute.kind_is_Node_Time_Attribute == 3)
            {
                link_object_list[q].node_time_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

}

Node_Time_Attribute read_a_Node_Time_Attribute(INDEX node_idx)
{
    Node_Time_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node &the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_time_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_time_attribute.kind_is_Node_Time_Attribute == 3)
            {
                attribute = the_node.self_attribute[q].node_time_attribute;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Node_Time_Attribute(INDEX node_idx, Node_Text_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_text_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_text_attribute.kind_is_Node_Text_Attribute == 2)
            {
                the_node.self_attribute[q].node_text_attribute = write_attritube;
                break;
            }
        }
    }
    
    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Text_Attribute(INDEX node_idx, Node_Text_Attribute attribute)
{
    Variable_Attribute fo = {.node_text_attribute = attribute};

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
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_text_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_text_attribute.kind_is_Node_Text_Attribute == 2)
            {
                link_object_list[q].node_text_attribute.is_node_attribute = 0;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Node_Text_Attribute read_a_Node_Text_Attribute(INDEX node_idx)
{
    Node_Text_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    
    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_text_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_text_attribute.kind_is_Node_Text_Attribute == 2)
            {
                attribute = the_node.self_attribute[q].node_text_attribute;
                break;
            }
        }
    }

    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;

    return attribute;
}

void write_a_Node_Text_Attribute(INDEX node_idx, Node_Text_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_text_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_text_attribute.kind_is_Node_Text_Attribute == 2)
            {
                link_object_list[q].node_text_attribute = write_attritube;
                break;
            }
        }
    }
    
    expect = 1;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}


void create_a_Node_Action_Attribute(INDEX node_idx, Node_Action_Attribute attribute)
{
    Variable_Attribute fo = {.node_action_attribute = attribute};

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
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_number_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_number_attribute.kind_is_Node_Number_Attribute == 3)
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

Node_Action_Attribute read_a_Node_Action_Attribute(INDEX node_idx)
{
    Node_Action_Attribute attribute;

    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node& the_node = General_Node_Storage[node_idx];

    for (int q = 0; q < 3; q++)
    {
        if (the_node.self_attribute[q].node_action_attribute.is_node_attribute == 2)
        {
            if (the_node.self_attribute[q].node_action_attribute.kind_is_Node_Motion_Attribute == 5)
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

void write_a_Node_Action_Attribute(INDEX node_idx, Node_Action_Attribute write_attritube)
{
    char expect = 0;
    while( Occupy_Single_General_Node[node_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;
    
    General_Node the_node = General_Node_Storage[node_idx];
    vector<Variable_Attribute>& link_object_list = the_node.Node_variable_attribute_list;
    int object_size = link_object_list.size();

    for (int q = 0; q < object_size; q++)
    {
        if (link_object_list[q].node_action_attribute.is_node_attribute == 2)
        {
            if (link_object_list[q].node_action_attribute.kind_is_Node_Motion_Attribute == 5)
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

INDEX get_a_highist_value_of_sort(Advanced_Value_Sort& Require_object_sort)
{
    INDEX idx = Require_object_sort.list[0].lower_one;
    
    // while()
    // {
    //     idx = Require_object_sort.list[idx].target_idx;
    // }

    return idx;
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
        Variable_Attribute ro = {.require_object = require_object};
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
        Variable_Attribute va = {.require_object = require_object};
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

void add_scene_a_local_require_object(INDEX scene_idx, Require_Object add_object)
{
    
}

void update_scene_a_local_require_value(vector< vector<int> >& require_list, INDEX require_idx)
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


void push_back_a_node_to_General_Scene(INDEX scene_idx, INDEX node_idx)
{
    Link_Node_Attribute link;
    Variable_Attribute va = {.link_node_attribute = link}; // number_node鐨�1锟�7
    
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
}

void push_back_a_related_scene_to_General_Scene(INDEX scene_idx, Link_Scene_Attribute scene_related)
{
    Variable_Attribute so = {.link_scene_attribute = scene_related};
    char expect = 0;
    while( Occupy_Single_General_Scene[scene_idx].compare_exchange_strong(expect, 1, memory_order_seq_cst) )
        expect = 0;

    General_Scene& scene = General_Scene_Storage[scene_idx];
    scene.Scene_variable_attribute_List.push_back(so);

    expect = 1;
    while( Occupy_Single_General_Scene[scene_idx].compare_exchange_strong(expect, 0, memory_order_seq_cst) )
        expect = 1;
}

Variable_Attribute read_a_scene_related_motion_detect(INDEX scene_idx)
{
    ;
}


static auto last_staedy_time = std::chrono::steady_clock::now();
static int32_t last_system_seconds = 0;

int get_current_second()
{
    auto system_now = std::chrono::system_clock::now();

    int current_system_seconds = std::chrono::duration_cast<std::chrono::seconds>(system_now.time_since_epoch()).count();

    if (current_system_seconds < last_system_seconds)
    {
        std::cout << "璀﹀憡锛氱郴缁熸椂闂村洖鎷�1锟�7";
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