#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Branch.hpp"




const int SEND_PORT = 5095;
string IP = "10.43.79.101";
const long long VERIFICATION = 0b01010101010101010101010101010101;
int Last_Gap_Model_Time = -1;
int Gap_Model_Time = -1;


struct Recv_Overview
{
    char execulute_action_num;

    unsigned short image_width;
    unsigned short image_height;
    unsigned short receive_text_num;
    
    long long code_time;
}; // 15


struct Send_Instruct
{
    char if_need_working;
    char if_need_communication;
    char if_need_action;
    char if_need_capture;
    char action_num;
    unsigned short send_text_num;

    long long model_time;
}; // 15

vector<RGB_Unit> Receive_PixelMap;
char Receive_Pixel_Map_State = 0;
unsigned short Pixel_Map_Height;
unsigned short Pixel_Map_Weight;

const char text_11[32] = "鏃狅拷锟戒俊";
const char text_12[64] = "閫氫俊涓�1锟�71锟�1锟�77";
const char text_21[64] = "妯″瀷鏃犺繍琛�1锟�71锟�1锟�77";
const char text_22[64] = "妯″瀷杩愯涓�1锟�71锟�1锟�77";
char text_57[32] = "鏂囨湰椤圭洰杈撳叆";
char text_58[32] = "鏂囨湰椤圭洰鏄剧ず";
char text_513[32] = "鏂囨湰椤圭洰鍥炵瓟";

// 鍒涘缓瀹瑰櫒
struct gtkwidgt_group
{
    vector<GtkWidget *> widget;
    vector<char> kind; // string 1, int 2, long long 3
    vector<void *> number_or_string;
};

gtkwidgt_group update_group;
gtkwidgt_group input_output_group;
gtkwidgt_group flow_group;


char show_message(GtkWidget *parent,
    const char *title,
    const char *message)
{
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "%s", message
    );

    gtk_window_set_title(GTK_WINDOW(dialog), title);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog)); // 绛夊緟鐢ㄦ埛鐐瑰嚮
    // GtkResponseType;

    char return_value = 0;
    switch (response)
    {
    case -8: // GTK_RESPONSE_YES          = -8,
        return_value = 1;
        break;

    case -9: // GTK_RESPONSE_NO           = -9,
        return_value = 0;
        break;
    }

    gtk_widget_destroy(dialog);

    return return_value;
}


void Socket_Thread()
{
    Waiting_Send_Action_List.reserve(5);
    
    vector<char> Send_Vector;
    Send_Vector.reserve(512 * 1024);

    vector<char> Recv_Vector;
    Recv_Vector.reserve(512 * 1024);

    Receive_PixelMap.reserve(4 * 1024 * 3 * 1024);
    Send_Text_List.reserve(64 * 1024);
    Receive_Text_List.reserve(64 * 1024);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("[鍙戯拷锟界嚎绋媇 socket 鍒涘缓澶辫触");
        return;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(SEND_PORT);
    inet_pton(AF_INET, IP.c_str(), &server.sin_addr);

    int sendbuf = 512 * 1024; // 512KB
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(sendbuf));

    int rcvbuf = 30 * 1024 * 1024; // 30 MB
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&rcvbuf, sizeof(rcvbuf));

    while (IF_SELF_TURN_OFF)
    {
        if (IF_SELF_COMMUNIATE && IF_SELF_CONNECT == 0)
        {
            while (connect(sockfd, (sockaddr *)&server, sizeof(server)) < 0)
            {
                perror("[鍙戯拷锟界嚎绋媇 杩炴帴澶辫触");
                sleep(100);
                IF_SELF_CONNECT = 1;
            }
        }

        while (IF_SELF_COMMUNIATE && IF_SELF_CONNECT)
        {
            
            GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(input_output_group.widget[2])));
            gtk_text_buffer_set_text(buffer, text_12, -1);

            Send_Vector.insert(Send_Vector.end(), (char *)&VERIFICATION, (char *)&VERIFICATION + 8);

            Send_Instruct si;
            si.if_need_working = IF_NEED_WORKING;
            si.if_need_communication = IF_NEED_COMMUNIATE;
            si.if_need_capture = IF_NEED_CAPTURE;
            si.if_need_action = IF_NEED_ACTION;
            si.action_num = Waiting_Send_Action_List.size();
            si.send_text_num = Send_Text_List.size();
            si.model_time = CURRENT_MODEL_TIME;

            Send_Vector.insert(Send_Vector.end(), (char *)&si.if_need_working, (char *)&si.if_need_working + 1);
            Send_Vector.insert(Send_Vector.end(), (char *)&si.if_need_communication, (char *)&si.if_need_communication + 1);
            Send_Vector.insert(Send_Vector.end(), (char *)&si.if_need_action, (char *)&si.if_need_action + 1);
            Send_Vector.insert(Send_Vector.end(), (char *)&si.if_need_capture, (char *)&si.if_need_capture + 1);
            Send_Vector.insert(Send_Vector.end(), (char *)&si.action_num, (char *)&si.action_num + 1);
            Send_Vector.insert(Send_Vector.end(), (char *)&si.send_text_num, (char *)&si.send_text_num + 2);

            Send_Vector.insert(Send_Vector.end(), (char *)&si.model_time, (char *)&si.model_time + 8);

            Send_Vector.insert(Send_Vector.end(), (char *)Waiting_Send_Action_List.data(), (char *)Waiting_Send_Action_List.data() + 12 * si.action_num);
            Alreadly_Send_Action_List = Waiting_Send_Action_List;
            Waiting_Send_Action_List.clear();

            Send_Vector.insert(Send_Vector.end(), (char *)Send_Text_List.data(), (char *)Send_Text_List.data() + sizeof(char) * si.send_text_num);

            send(sockfd, (char *)Send_Vector.data(), Send_Vector.size(), 0);
            Send_Text_List.clear();
            Send_Vector.clear();
            
            int v = 0;
            int verification_count = 0;

            do
            {
                recv(sockfd, &v, 8, 0);
                verification_count += 1;
                if (verification_count > 1)
                {
                    show_message(input_output_group.widget[0], "涓ラ噸閿欒", "缃戠粶浼犺緭涓㈠寘");

                    IF_SELF_COMMUNIATE = 0;
                    IF_NEED_COMMUNIATE = 0;
                    IF_NEED_CAPTURE = 0;
                    IF_NEED_ACTION = 0;

                    GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(input_output_group.widget[2])));
                    gtk_text_buffer_set_text(buffer, text_11, -1);
                }

            } while (v != VERIFICATION);

            verification_count = 0;

            Last_Gap_Model_Time = stat_gap_time();

            Recv_Overview ro;
            recv(sockfd, Recv_Vector.data(), 16, 0);
            memcpy((char *)&ro.execulute_action_num, Recv_Vector.data(), 1);
            memcpy((char *)&ro.image_width, Recv_Vector.data()+1, 2);
            memcpy((char *)&ro.image_height, Recv_Vector.data() + 3, 2);
            memcpy((char *)&ro.receive_text_num, Recv_Vector.data() + 5, 2);
            memcpy((char *)&ro.code_time, Recv_Vector.data() + 7, 8);
            Recv_Vector.clear();

            if (ro.image_width > 4000 || ro.image_height > 3000)
            {
                cout << "鍥剧墖鎺ユ敹閿欒" << endl;
            }

            int give_task_attention = 1 + Free_Total_Task_Attention / 3;

            
            if (ro.image_width * ro.image_height != 0)
            {
                Receive_PixelMap.resize(ro.image_width * ro.image_height);
                int already_read = 0;
                while (already_read < ro.image_width * ro.image_height * 3)
                {
                    already_read += recv(sockfd, (char *)Receive_PixelMap.data() + already_read, ro.image_width * ro.image_height * 3 - already_read, 0); // 鎺ユ敹鍥剧墖鏁版嵁
                }

                Image_Scene new_image;
                new_image.RGB_Map = Receive_PixelMap;
                new_image.width = ro.image_width;
                new_image.height = ro.image_height;

                for(int a = 0; a < ro.execulute_action_num; a++)
                {
                    Model_Output_Action old_action = Alreadly_Send_Action_List[a];
                    Node_Action_Attribute naa;
                    naa.action_kind = old_action.action_kind;
                    naa.action_order = old_action.action_order;
                    naa.dwFlags = old_action.dwFlags;
                    naa.mouseData = old_action.mouseData;
                    naa.vk = old_action.vk;
                    naa.x = old_action.x;
                    naa.y = old_action.y;

                    General_Node action_node;
                    Variable_Attribute va = {.node_action_attribute = naa};
                    action_node.Node_variable_attribute_list.push_back(va);
                    new_image.Action_Node_Idx[a] = create_a_General_Node(action_node);
                }

                INDEX idx = create_a_Image_Scene(new_image);

                Require_Object require_object;
                require_object.require_kind = 1;
                require_object.require_value = give_task_attention;
                require_object.require_detail_kind = 1;
                require_object.require_id_or_idx = idx;
                INDEX req_idx = create_a_Formal_Require_Object(require_object);
                Value_Sort_Unit vsu;
                vsu.target_idx = req_idx;
                vsu.value = give_task_attention;
                Formal_Require_Object_Sort.insert_a_unit(vsu);

                Free_Total_Task_Attention -= give_task_attention;
            }

            give_task_attention = 20 + Free_Total_Task_Attention / 3;


            if (ro.receive_text_num != 0)
            {
                int already_read = 0;
                while (already_read < ro.image_width * ro.image_height * 3)
                {
                    already_read += recv(sockfd, Receive_Text_List.data() + already_read, ro.receive_text_num - already_read, 0);
                }

                Text_Scene new_text;
                INDEX idx = create_a_Text_Scene(new_text);
                CharVector_To_Network(idx, Receive_Text_List, CURRENT_MODEL_TIME);
                Receive_Text_List.clear();

                
                Require_Object require_object;
                require_object.require_kind = 1;
                require_object.require_value = give_task_attention;
                require_object.require_detail_kind = 3;
                require_object.require_id_or_idx = idx;
                INDEX req_idx = create_a_Formal_Require_Object(require_object);
                Value_Sort_Unit vsu;
                vsu.target_idx = req_idx;
                vsu.value = give_task_attention;
                Formal_Require_Object_Sort.insert_a_unit(vsu);

                Free_Total_Task_Attention -= give_task_attention;
            }

        }

        if (IF_SELF_COMMUNIATE == 0)
        {
            sleep(10);
        }
    }

    close(sockfd);
}


void Disk_Thread()
{
    char Require_Read_Level = 0;
    char Require_Write_Level = 0;
    short Read_Require = 0;
    short Write_Require = 0;

    while (IF_SELF_TURN_OFF)
    {
        while (IF_SELF_WORKING == 0)
        {
            sleep(10);
        }

        Read_Require = Will_Read_Neuro_Queue.size();
        Write_Require = Will_Write_Neuro_Queue.size();

        int order = 0;

        if (Read_Require > 5)
            Require_Read_Level = 5;
        else
            Require_Read_Level = Read_Require;

        if (Read_Require + Write_Require)
        {
            while (order < Require_Read_Level)
            {
                string target_path = "Neuro/";
                target_path += ID_To_Directory_Path(Will_Read_Neuro_Queue[0]);

                FILE *R_file = fopen(target_path.c_str(), "r");

                int head;
                int *head_ptr = &head;
                fread(head_ptr, 4, 1, R_file);

                Neuro_Head_Item head_info = neuro_head_read(head);

                int index = pack_insert_position_provide(Will_Read_Neuro_Queue[0], head_info.node_size);
                int *ptr = Neuro_Pack_Storage[index].attribute_item;

                fread(ptr, 256*head_info.node_size, 1, R_file);
                
                fclose(R_file);

                current_0_1s_memory_value.fetch_add(1);

                Neuro_Use_Record_Update(Will_Read_Neuro_Queue[0]);

                Will_Read_Neuro_Queue.pop_front();
                order++;
            }

            order = 0;

            if (Write_Require > 5)
                Require_Write_Level = 5;
            else
                Require_Write_Level = Write_Require;

            while (order < Require_Write_Level)
            {
                string target_path = "Neuro/";
                target_path += ID_To_Directory_Path(Will_Write_Neuro_Queue[0]);

                FILE *W_file = fopen(target_path.c_str(), "w");

                int index = pack_index_find(Will_Write_Neuro_Queue[0]);
                int *ptr = Neuro_Pack_Storage[index].attribute_item;

                char pack_size = Neuro_Pack_Record_Storage[index].pack_size;

                fwrite(ptr, 256*pack_size, 1, W_file);

                current_0_1s_memory_value.fetch_add(1);
                
                pack_delete(Will_Write_Neuro_Queue[0]);

                fclose(W_file);

                Will_Write_Neuro_Queue.pop_front();
                
                order++;
            }
        }
        else
        {
            sleep(1);
        }
    }

}

void setup_thread(unsigned char num)
{
    unsigned char change_num = num - Calculate_Thread_Num;

    while (change_num > 0)
    {
        if (Thread_Handle_List.size() < num)
        {
            Single_Thread reduce_thread;

            Thread_Handle_List.push_back(reduce_thread);
            thread cal(Auto_Core_Execulute, ref(Thread_Handle_List.back()));
            cal.detach();
        }
        else
        {
            Thread_Handle_List[Calculate_Thread_Num].now_work = 1;
            Thread_Handle_List[Calculate_Thread_Num].need_free_time = 0;

            thread cal_thr(Auto_Core_Execulute, ref(Thread_Handle_List[Calculate_Thread_Num]));
            cal_thr.detach();
        }

        change_num--;
        Calculate_Thread_Num++;
        Use_Thread_Num++;
        Total_Task_Attention += 100;
        Free_Total_Task_Attention += 100;
    }

    while (change_num < 0)
    {
        Thread_Handle_List[Calculate_Thread_Num - 1].now_work = 0;
        Thread_Handle_List[Calculate_Thread_Num - 1].need_free_time = 0;
        
        change_num++;
        Calculate_Thread_Num--;
        Use_Thread_Num--;
        Total_Task_Attention -= 100;
        Free_Total_Task_Attention -= 100;
    }

    return;
}


static void on_button_clicked(GtkWidget *widget, gpointer data)
{
    int msg = GPOINTER_TO_INT(data);

    switch (msg)
    {
    case 1:
        IF_SELF_COMMUNIATE = 1;
        IF_NEED_COMMUNIATE = 1;

        break;
    case 2:
    {
        IF_SELF_WORKING = 1;

        GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(input_output_group.widget[3])));
        gtk_text_buffer_set_text(buffer, text_22, -1);
        gtk_widget_hide(input_output_group.widget[5]);
        gtk_widget_show(input_output_group.widget[4]);
    }
    break;

    case 3:
        IF_NEED_CAPTURE = 1;

        break;
    case 4:
        IF_NEED_ACTION = 1;

        break;
    case 5:
    {
        GtkWidget *text_view = input_output_group.widget[6];
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        char **endptr;
        int n = strtol(text, endptr, 10);
        g_free(text);

        if (n <= 10)
        {
            if (n <= 0)
            {
                n = 0;
                bool right = show_message(input_output_group.widget[0], "鎻愮ず", "杈撳叆鍊艰涓�1锟�71锟�1锟�770锛�1锟�71锟�1锟�77");
                if (right == 1)
                    setup_thread(0);
            }
            else
                setup_thread(n);
        }
        else
        {
            bool right = show_message(input_output_group.widget[0], "鎻愮ず", "鏈拷澶э拷锟戒负10");
            setup_thread(10);
        }
    }

    break;
    case 6:
    {
        GtkWidget *text_view_1 = input_output_group.widget[7];
        GtkTextBuffer *buffer_1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_1));
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer_1, &start);
        gtk_text_buffer_get_end_iter(buffer_1, &end);
        gchar *text = gtk_text_buffer_get_text(buffer_1, &start, &end, FALSE);
        char **endptr;
        string str_curr(text);
        vector<char> text_cur(str_curr.begin(), str_curr.end());
        g_free(text);

        GtkWidget *text_view_2 = input_output_group.widget[9];
        GtkTextBuffer *buffer_2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_2));
        gtk_text_buffer_get_end_iter(buffer_2, &end);
        text = gtk_text_buffer_get_text(buffer_2, &start, &end, FALSE);
        int kind = strtol(text, endptr, 10);
        g_free(text);
        if (kind <= 0 || kind >= 7)
        {
            show_message(input_output_group.widget[0], "鎻愮ず", "杈撳叆鍊间负0");
            break;
        }

        
        Gtk3_Text_Display text_display;
        text_display.Input_string = text_cur;
        INDEX idx = create_a_gtk3_text_display(text_display);
    }

    break;

    case 7:
    {
        GtkWidget *text_view_1 = input_output_group.widget[12];
        GtkTextBuffer *buffer_1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_1));
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer_1, &start);
        gtk_text_buffer_get_end_iter(buffer_1, &end);
        gchar *text = gtk_text_buffer_get_text(buffer_1, &start, &end, FALSE);
        char **endptr;
        long No = strtol(text, endptr, 10);
        g_free(text);

        if (No < 0 || No > Gtk3_Text_Display_Storage.size())
        {
            show_message(input_output_group.widget[0], "鎻愮ず", "鏃犲搴旇緭鍏ワ拷锟�1锟�71锟�1锟�77");
            break;
        }

        delete_a_gtk3_text_display(No);
    }

    break;

    case 8:
    {
        GtkWidget *text_view = input_output_group.widget[7];
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        char **endptr;
        int n = strtol(text, endptr, 10);
        g_free(text);

    }

    break;

    case 9:
    {
        GtkWidget *text_view = input_output_group.widget[8];
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        char **endptr;
        int n = strtol(text, endptr, 10);
        g_free(text);

        Pack_Limit = n * 10000;
        Neuro_Pack_Storage.reserve(Pack_Limit);
        ID_Find_Pack_Index.reserve(Pack_Limit);
    }

    break;

    case 0:

        break;

    case -1:
    {
        IF_SELF_COMMUNIATE = 0;
        IF_NEED_COMMUNIATE = 0;
        IF_NEED_CAPTURE = 0;
        IF_NEED_ACTION = 0;

        GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(input_output_group.widget[2])));
        gtk_text_buffer_set_text(buffer, text_11, -1);
    }

    case -2:
    {
        IF_SELF_WORKING = 0;
        IF_NEED_CAPTURE = 0;
        IF_NEED_ACTION = 0;

        GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(input_output_group.widget[1])));
        gtk_text_buffer_set_text(buffer, text_21, -1);
        gtk_widget_hide(input_output_group.widget[4]);
        gtk_widget_show(input_output_group.widget[5]);
    }

    break;

    case -3:
        IF_NEED_CAPTURE = 0;

        break;

    case -4:
        IF_NEED_ACTION = 0;

        break;
    }
}


GtkWidget *create_button(GtkWidget *parent,
    int left, int right,
    int top, int bottom,
    int msg,
    const char *text)
{
    GtkWidget *button = gtk_button_new_with_label(text);

    int width = right - left;
    int height = bottom - top;

    gtk_widget_set_size_request(button, width, height);

    
    gtk_fixed_put(GTK_FIXED(parent), button, left, top);

    
    g_signal_connect(button, "clicked",
        G_CALLBACK(on_button_clicked),
        GINT_TO_POINTER(msg));

    gtk_widget_show(button);
    return button;
}


GtkWidget *create_number_display(GtkWidget *parent,
    int left, int right,
    int top, int bottom)
{
    long long init_value = 0;

    GtkWidget *label = gtk_label_new(NULL);

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", init_value);
    gtk_label_set_text(GTK_LABEL(label), buf);

    int width = right - left;
    int height = bottom - top;

    gtk_widget_set_size_request(label, width, height);

    
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_label_set_yalign(GTK_LABEL(label), 0.5);

    gtk_fixed_put(GTK_FIXED(parent), label, left, top);

    gtk_widget_show(label);
    return label;
}


static void only_allow_digits(GtkEditable *editable,
    const gchar *text,
    gint length,
    gint *position,
    gpointer data)
{
    for (int i = 0; i < length; i++)
    {
        if (text[i] < '0' || text[i] > '9')
        {
            g_signal_stop_emission_by_name(editable, "insert-text");
            return;
        }
    }
}



GtkWidget *create_number_input(
    GtkWidget *parent,
    int left, int right,
    int top, int bottom,
    long init_value)
{
    GtkWidget *entry = gtk_entry_new();

    int width = right - left;
    int height = bottom - top;

    gtk_widget_set_size_request(entry, width, height);

    // 璁剧疆鍒濆鍊�1锟�71锟�1锟�77
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", init_value);
    gtk_entry_set_text(GTK_ENTRY(entry), buf);

    // 闄愬埗鍙兘杈撳叆鏁板瓧
    g_signal_connect(entry, "insert-text", G_CALLBACK(only_allow_digits), NULL);

    gtk_fixed_put(GTK_FIXED(parent), entry, left, top);
    gtk_widget_show(entry);

    return entry;

    
}



GtkWidget *create_text_fixed_display(GtkWidget *parent,
    int left, int right,
    int top, int bottom,
    const char *text)
{
    GtkWidget *textview = gtk_text_view_new();

    int width = right - left;
    int height = bottom - top;

    gtk_widget_set_size_request(textview, width, height);

    
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview), FALSE);

    
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);

    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, text, -1);

    gtk_fixed_put(GTK_FIXED(parent), textview, left, top);
    gtk_widget_show(textview);

    return textview;
}


GtkWidget *create_text_flowed_display(
    GtkWidget *parent,
    int left, int right,
    int top, int bottom,
    const char *text)
{
    int width = right - left;
    int height = bottom - top;

    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled, width, height);

    
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    
    GtkWidget *textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);

    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, text, -1);

    
    gtk_container_add(GTK_CONTAINER(scrolled), textview);

    
    gtk_fixed_put(GTK_FIXED(parent), scrolled, left, top);

    gtk_widget_show_all(scrolled);

    return textview;
}



GtkWidget *create_text_input(GtkWidget *parent,
    int left, int right,
    int top, int bottom,
    const char *init_text)
{
    GtkWidget *textview = gtk_text_view_new();

    int width = right - left;
    int height = bottom - top;

    gtk_widget_set_size_request(textview, width, height);

    
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview), TRUE);

    
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);

    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, init_text, -1);

    gtk_fixed_put(GTK_FIXED(parent), textview, left, top);
    gtk_widget_show(textview);

    return textview;
}


gboolean update_timeout(gpointer user_data)
{
    GtkWidget *label;
    char kind;

    Code_Time += 100;

    if (IF_SELF_WORKING)
    {
        CURRENT_MODEL_TIME += 100;
        current_0_1s_memory_value.exchange(0);
        scene_clean();
    }

    if (*(long long *)(use_time + time_curr * 2) != CURRENT_MODEL_TIME & time_update_gap)
    {
        if (time_curr >= 3)
            time_curr = 0;
        else
            time_curr += 1;

        *(long long *)(use_time + time_curr * 2) = CURRENT_MODEL_TIME & time_update_gap;
    }

    for (int q = 0; q < update_group.widget.size(); q++)
    {
        kind = update_group.kind[q];

        if (kind == 1)
        {
            const char *ptr = (char *)update_group.number_or_string[q];
            label = update_group.widget[q];

            GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(label)));
            gtk_text_buffer_set_text(buffer, ptr, -1);
        }
        else if (kind == 2)
        {
            const int *ptr = (int *)update_group.number_or_string[q];
            label = update_group.widget[q];

            char buf[32];
            snprintf(buf, sizeof(buf), "%d", *ptr);
            gtk_label_set_text(GTK_LABEL(label), buf);
        }
        else if (kind == 3)
        {
            const long long *ptr = (long long *)update_group.number_or_string[q];
            label = update_group.widget[q];

            char buf[32];
            snprintf(buf, sizeof(buf), "%d", *ptr);
            gtk_label_set_text(GTK_LABEL(label), buf);
        }
    }

    string show_text;
    string response_text;
    GtkWidget *show_widget = input_output_group.widget[10];
    GtkWidget *response_widget = input_output_group.widget[13];

    vector<char> output_text_vector;

    
    for (int w = 1; w < Gtk3_Text_Display_Storage.size(); w++)
    {
        string text_string(Gtk3_Text_Display_Storage[w].Input_string.begin(), Gtk3_Text_Display_Storage[w].Input_string.end());
        string order = to_string(w);
        show_text += order;
        show_text += text_string;
        show_text += "\n\n";

        response_text += order;
        output_text_vector = Network_To_CharVector(Gtk3_Text_Display_Storage[w].input_Text_Scene_Idx);
        string output_text(output_text_vector.begin(), output_text_vector.end());
        response_text += output_text;
        response_text += "\n\n";
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(show_widget)));
    gtk_text_buffer_set_text(buffer, show_text.c_str(), -1);

    buffer = gtk_text_view_get_buffer((GTK_TEXT_VIEW(response_widget)));
    gtk_text_buffer_set_text(buffer, response_text.c_str(), -1);

    return TRUE;
}


void create_update(GtkWidget *widget, char kind, void *source)
{
    update_group.widget.push_back(widget);
    update_group.kind.push_back(kind);
    update_group.number_or_string.push_back((void *)source);
}


void init_model()
{
    
    load_model_record();
    load_pre_load_file();
    load_image_enter_line();
    load_text_enter_ch();

    
    thread base_t1(Socket_Thread);
    base_t1.detach();

    
    thread base_t2(Disk_Thread);
    base_t2.detach();

    
    thread base_t3(Pack_Clear_Thread);
    base_t3.detach();

    
    Image_Scene_Storage.resize(1);

    Text_Scene_Storage.resize(1);

    Gtk3_Text_Display_Storage.resize(1);

    General_Node_Storage.resize(1);

    General_Scene_Storage.resize(1);
}

void control_interface(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    
    GtkWidget *init_interface = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), init_interface);

    input_output_group.widget.reserve(20);
    input_output_group.widget[0] = window;
    input_output_group.widget[1] = init_interface;

    
    const char text_1[32] = "寤虹珛閫氫俊";
    GtkWidget *button_1 = create_button(init_interface, 100, 160, 100, 130, 1, text_1);

    GtkWidget *display_11 = create_text_fixed_display(init_interface, 100, 160, 150, 180, text_11);
    input_output_group.widget[2] = display_11;

    
    const char text_2[32] = "妯″瀷鍚姩";
    const char text_20[32] = "妯″瀷鍏抽棴";
    GtkWidget *button_2 = create_button(init_interface, 200, 260, 100, 130, 2, text_2);
    input_output_group.widget[5] = button_2;

    GtkWidget *button_20 = create_button(init_interface, 200, 260, 100, 130, -2, text_20);
    input_output_group.widget[4] = button_20;

    GtkWidget *display_21 = create_text_fixed_display(init_interface, 200, 260, 150, 180, text_21);
    input_output_group.widget[3] = display_21;

    
    const char text_30[32] = "杩愯璧勬簮鍒嗛厤";

    const char text_31[64] = "褰撳墠浣跨敤绾跨▼鏁�1锟�71锟�1锟�77";
    GtkWidget *display_31 = create_number_display(init_interface, 500, 560, 50, 80);
    create_update(display_31, 3, &Calculate_Thread_Num);

    const char text_32[64] = "鏇存敼绾跨▼鏁�1锟�71锟�1锟�77";
    GtkWidget *button_32 = create_button(init_interface, 500, 560, 100, 130, 5, text_32);
    GtkWidget *input_32 = create_text_input(init_interface, 500, 560, 150, 180, "10");
    input_output_group.widget[6] = input_32;

    const char text_33[32] = "褰撳墠鍐呭瓨涓婇檺";
    GtkWidget *button_33 = create_button(init_interface, 300, 360, 150, 180, 8, text_33);
    GtkWidget *input_33 = create_text_input(init_interface, 300, 360, 200, 230, "24");
    input_output_group.widget[7] = input_33;

    const char text_34[64] = "褰撳墠鍐呭瓨閲�1锟�71锟�1锟�77:G";
    GtkWidget *text_display_34 = create_text_fixed_display(init_interface, 300, 360, 250, 280, text_34);
    GtkWidget *num_display_34 = create_number_display(init_interface, 300, 360, 300, 330);
    create_update(num_display_34, 3, &CURRENT_SYSTEM_MEMORY);

    const char text_35[64] = "褰撳墠鍖呬笂闄�1锟�71锟�1锟�77";
    GtkWidget *button_35 = create_button(init_interface, 400, 460, 150, 180, 9, text_35);
    GtkWidget *num_input_35 = create_text_input(init_interface, 400, 460, 200, 230, "1400");
    input_output_group.widget[8] = num_input_35;

    const char text_36[64] = "瀹為檯鍖呴噺:涓�1锟�71锟�1锟�77";
    GtkWidget *text_display_36 = create_text_fixed_display(init_interface, 400, 460, 250, 280, text_36);
    GtkWidget *num_display_36 = create_number_display(init_interface, 400, 460, 300, 330);
    create_update(num_display_36, 3, &Current_Pack_Num);

    const char text_37[64] = "鍗佺鍐呭浘鍍忚妭鐐圭敓鎴愭暟";

    const char text_38[64] = "鍗佺鍐呮蹇佃妭鐐圭敓鎴愭暟";

    const char text_39[64] = "鍗佺鍐呮枃鏈妭鐐圭敓鎴愭暟";

    
    const char text_60[64] = "杩愯鐘讹拷锟界洃鎺�1锟�71锟�1锟�77";

    const char text_61[64] = "宸茶繍琛屾ā鍨嬫椂闂�1锟�71锟�1锟�77";
    GtkWidget *display_61 = create_number_display(init_interface, 100, 160, 50, 80);
    create_update(display_61, 3, &Code_Time);

    const char text_64[64] = "褰撳墠妯″瀷鏃堕棿";
    GtkWidget *display_64 = create_number_display(init_interface, 200, 260, 50, 80);
    create_update(display_64, 3, &CURRENT_MODEL_TIME);

    const char text_62[64] = "宸插瓨鍌ㄨ妭鐐规暟閲�1锟�71锟�1锟�77";
    create_text_fixed_display(init_interface, 300, 360, 100, 130, text_62);
    GtkWidget *display_62 = create_number_display(init_interface, 300, 360, 50, 80);
    create_update(display_62, 2, &NEWIST_USEFUL_ID);

    const char text_63[64] = "璁板綍瀛楃鏁�1锟�71锟�1锟�77";
    create_text_fixed_display(init_interface, 400, 460, 100, 130, text_63);
    GtkWidget *display_63 = create_number_display(init_interface, 400, 460, 50, 80);
    create_update(display_63, 2, &ALL_NUM_OF_CH);

    const char text_65[64] = "涓婃浼犺緭鐨勯棿闅�1锟�71锟�1锟�77";
    GtkWidget *text_display_65 = create_text_fixed_display(init_interface, 100, 160, 250, 280, text_65);
    GtkWidget *display_65 = create_number_display(init_interface, 100, 160, 200, 230);
    create_update(display_65, 2, &Last_Gap_Model_Time);

    
    const char text_40[64] = "模型参数控制";
    const char text_41[64] = "建模最小数量每秒";
    const char text_42[64] = "建模最大数量每秒";
    const char text_43[64] = "灞烇拷锟界敓鎴愮巼";
    const char text_44[64] = "鍥惧儚鐢熸垚鐜�1锟�71锟�1锟�77";
    const char text_45[64] = "鏂囨湰鐢熸垚鐜�1锟�71锟�1锟�77";
    const char text_46[64] = "璁ょ煡鐢熸垚鐜�1锟�71锟�1锟�77";

    const char text_47[64] = "随机输出行为倾向";
    const char text_48[24] = "瀵硅薄";
    const char text_49[24] = "值";

    
    const char text_50[64] = "鏂囨湰瀵硅瘽鎺у埗";

    const char text_51[24] = "杩芥眰鐩爣";
    const char text_52[24] = "缁存寔琛屼负";
    const char text_53[24] = "瑙ｉ噴鏂囨湰";
    const char text_54[24] = "姹傜煡鏂囨湰";
    const char text_55[64] = "璧嬩簣瀵硅薄灞烇拷锟�1锟�71锟�1锟�77";
    const char text_56[24] = "鍥炵瓟瀵硅瘽";

    GtkWidget *input_57 = create_text_input(init_interface, 100, 300, 400, 600, text_57);
    input_output_group.widget[7] = input_57;
    const char text_59[64] = "纭";
    GtkWidget *button_59 = create_button(init_interface, 100, 160, 350, 380, 6, text_59);
    input_output_group.widget[8] = button_59;
    const char text_510[64] = "鎿嶄綔绫诲瀷";
    GtkWidget *input_510 = create_text_input(init_interface, 200, 260, 350, 380, text_510);
    input_output_group.widget[9] = input_510;

    GtkWidget *output_58 = create_text_flowed_display(init_interface, 350, 550, 400, 600, text_58);
    input_output_group.widget[10] = output_58;
    const char text_511[64] = "鍒犻櫎";
    GtkWidget *button_511 = create_button(init_interface, 350, 410, 350, 380, 7, text_511);
    input_output_group.widget[11] = button_511;
    const char text_512[64] = "鍒犻櫎椤圭洰";
    GtkWidget *input_512 = create_text_input(init_interface, 450, 510, 350, 380, text_512);
    input_output_group.widget[12] = input_512;

    GtkWidget *output_513 = create_text_flowed_display(init_interface, 600, 1000, 400, 600, text_513);
    input_output_group.widget[13] = output_513;

    
    setup_thread(10);

    g_timeout_add(100, update_timeout, NULL);
    gtk_widget_show_all(window);
    gtk_widget_hide(button_20);

    gtk_main();
}

void end_model()
{
    
    IF_NEED_WORKING = 0;
    IF_SELF_TURN_OFF = 0;
    IF_SELF_COMMUNIATE = 0;
    IF_SELF_CONNECT = 0;

    setup_thread(0);

    
    save_pre_load_file();
    save_image_enter_line();
    save_text_enter_ch();
    save_record_model();
}



int main(int argc, char *argv[])
{
    
    init_model();
    
    control_interface(argc, argv);
    
    end_model();
    
    return 0;
}