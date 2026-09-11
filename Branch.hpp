#include <cstdio>
#include <stdexcept>

#include "Trunk.hpp"





inline void Multiple_Space_Probability_Appraise(INDEX calu_node_idx, INDEX curr_scene_idx)
{
    General_Node be_appraise_node = read_a_General_Node(calu_node_idx);
    
    for(int l = 0; l < be_appraise_node.Node_variable_attribute_list.size(); l++)
    {
        vector<float> probability_list;
        vector<int> stat_num_list;
        vector<float> restrain_list;
        int total_stat = 0;

        if( be_appraise_node.Node_variable_attribute_list[l].link_node_attribute.link_Kind == 3 )
        {
            INDEX source_idx = be_appraise_node.Node_variable_attribute_list[l].link_node_attribute.link_idx_or_id;
            General_Node& source_node = General_Node_Storage[source_idx];

            for(int k = 0; k < source_node.Node_variable_attribute_list.size(); k++)
            {
                if(source_node.Node_variable_attribute_list[k].link_node_attribute.link_idx_or_id != calu_node_idx)
                    continue;
                

                probability_list.push_back(source_node.Node_variable_attribute_list[k].link_node_attribute.link_value);
            }
        }

        float total_Probability = 0;

        for(int j = 0; j < probability_list.size(); j++)
        {
            total_Probability += probability_list[j] * stat_num_list[j]/total_stat * restrain_list[j];
        }

        be_appraise_node.complete_or_probability = total_Probability;
    }

}

inline void Multiple_Time_Probability_Appraise(INDEX calu_node_idx, INDEX curr_scene_idx)
{
    ;
}


inline void Multiple_Number_Probability_Appraise(INDEX calu_node_idx, INDEX curr_scene_idx)
{
    General_Node be_appraise_node = read_a_General_Node(calu_node_idx);
    
    long long sum_stat = 0;
    long long sum_realize = 0;

    vector<Variable_Attribute>& Variable_list = be_appraise_node.Node_variable_attribute_list;

    char have_number_source = 1;
    int size_variable = Variable_list.size();

    for(int a = 0; a < size_variable; a++)
    {
        Variable_Attribute& va = Variable_list[a];
        Neuro_Number_Item& simulate_number_attribute = va.neuro_number_item;

        if(simulate_number_attribute.is_neuro_item != 1
            && simulate_number_attribute.item_is_number != 2)
            continue;

        int stat_num;
        int realize_num;
        
        if(simulate_number_attribute.logic == 2)
        {
            stat_num = simulate_number_attribute.number_I;
            realize_num = simulate_number_attribute.number_II;
        }

        a++;
    }

    be_appraise_node.complete_or_probability = sum_realize / sum_stat;
    
    write_a_General_Node(calu_node_idx, be_appraise_node);
}

inline void Multiple_Text_Probability_Appraise(INDEX calu_node_idx, INDEX curr_scene_idx)
{
    ;
}


inline void Multiple_Action_Probability_Appraise(INDEX calu_node_idx, INDEX curr_scene_idx)
{
    ;
}


inline void Multiple_Probability_Cohesion_Appraise(INDEX node_idx, INDEX scene_idx)
{

    char task_work;

    switch(task_work)
    {
    case 1:
    {
        Multiple_Space_Probability_Appraise(node_idx, scene_idx);
    }
    break;
    case 2:
    {
        Multiple_Time_Probability_Appraise(node_idx, scene_idx);
    }
    break;
    case 3:
    {
        Multiple_Number_Probability_Appraise(node_idx, scene_idx);
    }
    break;
    case 4:
    {
        Multiple_Text_Probability_Appraise(node_idx, scene_idx);;
    }
    break;
    case 5:
    {
        ;
    }
    }

}



void image_attention_appraise(INDEX last_scene_idx, INDEX curr_scene_idx, char limit_time = 10 )
{
    auto start_total = chrono::steady_clock::now();
    int total_elapsed_time = 0;

    General_Scene& curr_scene = General_Scene_Storage[curr_scene_idx];
    vector<Variable_Attribute>& cur_Require_Object_List = curr_scene.Require_object_list;

    General_Scene& last_scene = General_Scene_Storage[last_scene_idx];
    Advanced_Value_Sort& last_attention_node_list = last_scene.Attention_object_sort;
    vector<Variable_Attribute>& last_Require_Object_List = last_scene.Require_object_list;

    INDEX att_idx = last_attention_node_list.list[0].lower_one;

    while( att_idx != 0 && limit_time < total_elapsed_time)
    {
        General_Node& last_node = General_Node_Storage[att_idx];

        Require_Object require_object;
        require_object.require_kind = 1;
        require_object.require_id_or_idx = last_node.self_id;
        require_object.require_value = last_node.node_attention / 10;
        Variable_Attribute va = {.require_object = require_object};
        cur_Require_Object_List.push_back(va);

        att_idx = last_attention_node_list.list[att_idx].lower_one;

        auto now = chrono::steady_clock::now();
        total_elapsed_time = chrono::duration_cast< chrono::milliseconds >(now - start_total).count();
    }

    for(int b = 0; b < last_Require_Object_List.size(); b++)
    {
        Require_Object& require_object = last_Require_Object_List[b].require_object;

        if(require_object.require_object_kind != 1)
            continue;

        require_object.require_value;
    }

}


INDEX require_object_select(vector< vector<int> >& require_list, Advanced_Value_Sort& Require_sort)
{
    INDEX finally_idx = 0;
    
    int space_require_value;
    int object_require_value;
    
    if(space_require_value > object_require_value)
    {
        char require_layer = require_list.size() - 1;
        int choose_value;
        int choose_idx;

        for(int q = require_layer; q >= 0; q--)
        {
            vector<int>& curr_inside_require_list = require_list[require_layer];
            int curr_inside_require_size = curr_inside_require_list.size();

            choose_idx = 0;
            choose_value = curr_inside_require_list[0];

            for(int w = 1; w < curr_inside_require_size; w++)
            {
                if(choose_value < curr_inside_require_list[w])
                {
                    choose_value = curr_inside_require_list[w];
                    choose_idx = w;
                }
            }

            finally_idx = finally_idx*8 + choose_idx;
            require_layer--;
        }
    } else {
        
    }

return finally_idx;
}


void A_Block_Model_Evolve(ID block_id)
{
    ;
}


void Image_A_Block_Model( INDEX image_scene_idx, INDEX curr_block_idx , char rough_view_idx)
{
    Image_Scene& curr_image_scene = Image_Scene_Storage[image_scene_idx];
    int height = curr_image_scene.height;
    int width = curr_image_scene.width;

    Image_Colour_Block& curr_block = curr_image_scene.Image_rough_view[rough_view_idx].Colour_block_list[curr_block_idx];
    COLOUR_BLOCK_ENTER_SORT[curr_block.ave_r*8*8 + curr_block.ave_g*8 + curr_block.ave_b*8];

    ;
}

void Image_A_Line_Model()
{
    ;
}


void Image_A_Neuro_Model( INDEX image_scene_idx, INDEX curr_node_idx )
{
    A_Neuro_Basic_Evolve(curr_node_idx, 1);

    Image_Scene& curr_image_scene = Image_Scene_Storage[image_scene_idx];
    int height = curr_image_scene.height;
    int width = curr_image_scene.width;
    vector< vector<INDEX> >& Node_space_record_list =
        curr_image_scene.Node_space_form_record_list;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    Node_Image_Attribute node_image_attribute = read_a_Node_Image_Attribute(curr_node_idx);
    short space_scan_range = sqrt(node_image_attribute.block_num) + sqrt(curr_node.node_attention);
    space_scan_range *= node_image_attribute.observe_size;

    int x = node_image_attribute.x * node_image_attribute.observe_size;
    int y = node_image_attribute.y * node_image_attribute.observe_size;

    short left = x - space_scan_range;
    short right = x + space_scan_range;
    short bottom = y - space_scan_range;
    short top = y + space_scan_range;

    if(left < 0) left = 0;
    if(right >= width) right = width - 1;
    if(bottom < 0) bottom = 0;
    if(top >= height) top = height - 1;
    
    Node_Time_Attribute node_time_attribute = read_a_Node_Time_Attribute(curr_node_idx);
    long long curr_node_time = node_time_attribute.front_time;
    curr_node_time = curr_node_time << 32;
    curr_node_time |= node_time_attribute.back_time;
    short time_scan_range = 10;

    vector<INDEX> can_chose_node;
    can_chose_node.reserve(128);
    vector<int> choose_node_weight;
    choose_node_weight.reserve(128);

    int record_distance = curr_image_scene.record_distance;
    int record_width = width / record_distance;

    bottom /= record_distance;
    top /= record_distance;
    left /= record_distance;
    right /= record_distance;

    int x0 = x / record_distance;
    int y0 = y / record_distance;

    int width_reduce_middle = record_width - (right-left);

    unordered_set<INDEX> have_link_idx;

    for(int p = 0; p < curr_node.Node_variable_attribute_list.size(); p++)
    {
        if( (curr_node.Node_variable_attribute_list[p].link_node_attribute.link_Kind >= 1 && curr_node.Node_variable_attribute_list[p].link_node_attribute.link_Kind <= 4) )
        {
            INDEX object_idx = curr_node.Node_variable_attribute_list[p].link_node_attribute.link_idx_or_id;
            have_link_idx.insert(object_idx);
        }
    }

    INDEX retrieval_idx = bottom * record_width + left;

    for(int y1 = bottom; y1 <= top; y1++)
    {
        for(int x1 = left; x1 <= right; x1++)
        {
            int true_x_dis = x1 - x0;
            int true_y_dis = y1 - y0;

            float distance_encourage_value = space_scan_range - sqrtf( true_x_dis*true_x_dis + true_y_dis*true_y_dis );

            if(distance_encourage_value < 0)
                continue;

            for(INDEX object_node_idx : Node_space_record_list[retrieval_idx])
            {
                General_Node& object_node = General_Node_Storage[object_node_idx];

                Node_Image_Attribute node_image_attritube = read_a_Node_Image_Attribute(object_node_idx);

                char standard_size = node_image_attritube.observe_size;
                int true_x_dis = (node_image_attritube.x * standard_size) - x;
                int true_y_dis = (node_image_attritube.y * standard_size) - y;

                float distance_encourage_value = space_scan_range -
                    sqrtf(true_x_dis*true_x_dis + true_y_dis*true_y_dis);

                int write_encourage_value;

                int node_weight = object_node.node_attention + distance_encourage_value
                    + object_node.neuro_spare_size + object_node.model_require;
                
                can_chose_node.push_back(object_node_idx);
                choose_node_weight.push_back(node_weight);
            }
            retrieval_idx += 1;
        }
        retrieval_idx += width_reduce_middle;
    }


    random_device RD;
    mt19937 Gen(RD());

    discrete_distribution<int> chose_tend_index_dist( choose_node_weight.begin(), choose_node_weight.end() );

    while ( curr_node.model_require > 10)
    {
        INDEX object_node_idx = can_chose_node[ chose_tend_index_dist(Gen) ];
        General_Node object_node = read_a_General_Node(object_node_idx);
        Node_Image_Attribute object_node_imageattribute = read_a_Node_Image_Attribute(object_node_idx);
        image_object_locate_info locate_info = image_object_locate(curr_node_idx, object_node_idx);

        if( object_node.model_require  > 10)
        {
            Attribute_Head_Item item_head;
            vector<General_Condition> general_condition_list;
            vector<General_Result> general_result_list;

            item_head.attribute_kind = 1;
            item_head.condition_num += 1;
            
            Neuro_Image_Item new_image_condition;
            new_image_condition.direction_scale = 3;
            new_image_condition.direction = locate_info.direction;
            new_image_condition.distance_scale = 3;
            new_image_condition.distance = locate_info.distance;
            new_image_condition.related_id = object_node.self_id;
            
            General_Condition condition = {.image_condition = new_image_condition};

            general_condition_list.push_back(condition);

            Simple_Neuro_Attribute_write(object_node.self_id, 0, 1, item_head,
                general_condition_list, general_result_list);
        }
    }
    


    for(int a = 0; a < curr_image_scene.Overall_image_node_list.size(); a++)
    {
        INDEX object_idx = curr_image_scene.Overall_image_node_list[a];
        General_Node object_node = read_a_General_Node(object_idx);
        Node_Time_Attribute object_time_attribute = read_a_Node_Time_Attribute(object_idx);

        long long object_node_time = object_time_attribute.front_time;
        object_node_time = object_node_time << 32;
        object_node_time |= object_time_attribute.back_time;

        char gap_time = object_node_time - curr_node_time;

        int stat_add_attract = object_node.model_require - gap_time/100;

        choose_node_weight.push_back(stat_add_attract);
    }

    char self_need_add_stat = curr_node.model_require*10 + curr_node.neuro_spare_size;

    while(self_need_add_stat >= 100)
    {
        
        INDEX choose_idx = chose_tend_index_dist(Gen);
        General_Node choose_model_node = read_a_General_Node(choose_idx);

        Attribute_Head_Item attribute_head;
        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        attribute_head.attribute_kind = 1;
        attribute_head.result_num += 1;

        Neuro_Time_Item time_condition;
        time_condition.time_left_range = 1;
        time_condition.time_scale = 3;
        time_condition.related_id = choose_model_node.self_id;

        Simple_Neuro_Attribute_write(curr_node.self_id, 0, 1,
            attribute_head, general_condition_list, general_result_list);
        
        self_need_add_stat -= 10;
    }


}


void Action_A_Neuro_Model( INDEX map_idx, INDEX node_idx )
{
    Image_Scene the_image;

    Require_Object require_object;
    General_Node action_node = General_Node_Storage[node_idx];
    Node_Action_Attribute action_attribute = read_a_Node_Action_Attribute(node_idx);

    for(int a = 0; a <= 8; a++)
    {
        Neuro_Time_Item condition_time;
        condition_time.time_left_range = -a;
        condition_time.time_scale = 1;

        for(int b = 0; b < the_image.model_output_action_num ; b++)
        {
            INDEX object_idx;
            ID object_id;
            Node_Image_Attribute object_attribute = read_a_Node_Image_Attribute(object_idx);

            char direction;
            char distance;
            get_two_point_direction_distance(object_attribute.x, object_attribute.y, action_attribute.x, action_attribute.y,
                direction, distance);
            
            Neuro_Action_Item condition_action;
            condition_action.direction = direction;
            condition_action.distance = distance;
            condition_action.action_kind = action_attribute.action_kind;
            condition_action.dwFlags = action_attribute.dwFlags;
            condition_action.locate_id = object_id;
            condition_action.mouseData = action_attribute.mouseData;
            condition_action.vk = action_attribute.vk;
        }
        
    }
    
    

    Neuro_Image_Item result_image;
    result_image.direction;
    result_image.distance;
    result_image.related_id;

}



void reason_logic_infer_calculate()
{
    //result clear 

    //unresult

    //
    ID reason_element;

    //
    ID new_reason_element[40];

    //
    
}



void General_Cognition_Calculate(INDEX cur_scene_idx, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    General_Scene& curr_general_scene = General_Scene_Storage[cur_scene_idx];
    vector<Variable_Attribute>& Require_object_list = curr_general_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_general_scene.Require_object_sort;

    INDEX curr_node_idx;

    random_device Task_RD;
    mt19937 Task_Gen(Task_RD());
    int work_weights[5];

    discrete_distribution<int> task_dist(work_weights, work_weights + 4);
    int task_work = task_dist(Task_Gen);

    while(elapsed_ms < limit_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < curr_general_scene.Require_object_list.size();q++)
            {
                Require_Object& require_object = curr_general_scene.Require_object_list[q].require_object;

                if(require_object.require_object_kind == 1)
                {
                    
                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_object_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            A_Neuro_Cognition_Generate(curr_node_idx, cur_scene_idx, 0);
        }
        break;
        case 2:
        {
            ;
        }

            auto end = chrono::steady_clock::now();
            elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
    }

}


void General_Scene_Simulate(INDEX curr_scene_idx, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;
    
    General_Scene& curr_general_scene = General_Scene_Storage[curr_scene_idx];
    unordered_map< ID, vector<Focus_Object> >& Id_find_focus_object = curr_general_scene.Id_find_focus_object;
    vector<INDEX>& General_overall_node_list = curr_general_scene.Overall_general_node_list;
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = curr_general_scene.Id_find_overall_node;
    vector<Variable_Attribute>& Require_object_list = curr_general_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_general_scene.Require_object_sort;
    vector<Variable_Attribute>& Scene_variable_attribute_List = curr_general_scene.Scene_variable_attribute_List;


    vector<INDEX> Process_Element;

    float root_require_rate;
    float allocation_require_rate;

    vector<INDEX> seed_scene_idx_list;

    Generate_Permit simulate_permit;


    while(elapsed_ms < limit_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < curr_general_scene.Require_object_list.size();q++)
            {
                Require_Object& require_object = curr_general_scene.Require_object_list[q].require_object;

                if(require_object.require_object_kind == 1)
                {
                    
                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_object_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            for(INDEX node_idx : Process_Element)
            {
                A_Neuro_Simulate_Generate(node_idx, curr_scene_idx, 4, simulate_permit);
            }
        }
        break;
        case 2:
        {
            INDEX require_front;
            INDEX require_back;
            bool require_joint = 0;
            bool have_more_feature = 1;
            
            int predict_tend[4] = {0, 0, 0, 0};
            
            for(int g = 0; g < Require_object_list.size(); g++)
            {
                Require_Object& require = Require_object_list[g].require_object;
                
                if(require.require_object_kind < 10
                    && require_joint == 0)
                {
                    require_front = g;

                } else if(require.require_object_kind > 10 
                    && require_joint == 0) {
                    
                    require_front = g;
                    require_joint = 1;

                } else if(require.require_object_kind < 10 
                    && require_joint == 1) {

                    require_back = g;
                    require_joint = 0;

                    have_more_feature = 1;
                }

                if(require_joint == 0)
                {
                    if(have_more_feature)
                    {
                        for(int a = require_front; a <= require_back; a++)
                        {
                            Require_Object& ro = Require_object_list[require_front].require_object;

                        }

                        have_more_feature = 0;

                    } else {

                        Require_Object& ro = Require_object_list[require_front].require_object;

                        if( ro.require_object_kind == 1)
                        {
                            ro.require_value;
                            ro.require_kind;

                        } if ( ro.require_object_kind == 3) {
                            
                            ro.require_value;
                            ro.require_kind;

                        }

                    }
                }

                
            }
            
        }
        break;
        case 3:
        {
            curr_general_scene.Attention_object_sort;
            ID object_id;

            while(object_id )
            {
                if(Id_find_focus_object.count(0) )
                {
                    vector<Focus_Object> focus_object_list = Id_find_focus_object[0];

                    for(Focus_Object focus_object : focus_object_list)
                    {
                        if(focus_object.kind == 2)
                        {
                            Require_Object& require_object = Require_object_list[focus_object.target_idx].require_object;


                        }
                    }
                }

                if(Id_find_focus_object.count(object_id) )
                {
                    vector<Focus_Object> focus_object_list = Id_find_focus_object[object_id];

                    for(Focus_Object foc_obj : focus_object_list)
                    {
                        foc_obj.kind == 2;
                    }

                }

            }
        }
        break;
        case 4:
        {
            INDEX key_id;
            vector<INDEX> same_id_node_list = curr_general_scene.Id_find_overall_node[key_id];
            for(INDEX b : same_id_node_list)
            {
                vector< INDEX > Attention_Sort;
                vector< INDEX > Free_Idx;

                int gap = 0;
                vector< INDEX > Gap_Index;

                Multiple_Probability_Cohesion_Appraise(b, curr_scene_idx);
            }

        }
        break;
        case 5:
        {
            vector<int> require_need_value;

            for(int detect_value : require_need_value)
            {

                General_Scene seed_simu_scene;
                seed_simu_scene.Overall_general_node_list;

                INDEX add_simu_idx = create_a_General_Scene(seed_simu_scene);
                seed_scene_idx_list.push_back(add_simu_idx);

                Link_Scene_Attribute link_scene;
                link_scene.related_kind == 1;
                link_scene.related_kind == 3;
                link_scene.related_value;
                link_scene.scene_idx = add_simu_idx;
                Variable_Attribute scene_attribute = {.link_scene_attribute = link_scene};

                Scene_variable_attribute_List.push_back(scene_attribute);
            }
        }
        break;
        case 6:
        {
            {
                INDEX a_scene_idx;
                char limit_time;
                General_Scene_Simulate(a_scene_idx, limit_time);
            }
        }
        }

        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
    

}



void Image_Cognition_Calculate( INDEX curr_map_idx, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Image_Scene& curr_image_scene = Image_Scene_Storage[curr_map_idx];
    short width = curr_image_scene.width;
    short height = curr_image_scene.height;
    vector<Variable_Attribute>& Require_object_list= curr_image_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_image_scene.Require_object_sort;
    vector< vector<int> >& Retrieve_require_list = curr_image_scene.Retrieve_total_require_list;
    vector< vector<int> >& Model_require_list = curr_image_scene.Model_total_require_list;
    vector< vector<int> >& Construct_require_list = curr_image_scene.Construct_total_require_list;

    vector<Wait_Active_Neuro> wait_active_list;
    vector<INDEX> free_wait_active_idx;


    if(curr_image_scene.have_init != 1)
    {
        if(curr_image_scene.is_lock_occupy == 1)
            return;

        curr_image_scene.is_lock_occupy += 1;

        if(curr_image_scene.last_map != 0)
            image_nature_attention_generate(curr_image_scene.last_map, curr_map_idx);
        else image_nature_attention_generate(curr_map_idx, curr_map_idx);

        image_rough_map_generate(curr_map_idx, 2, 4);
        image_rough_map_generate(curr_map_idx, 4, 16);

        init_image_require(curr_map_idx, Retrieve_require_list, 1);
        init_image_require(curr_map_idx, Model_require_list, 2);
        init_image_require(curr_map_idx, Construct_require_list, 2, 0);


        image_attention_appraise(curr_image_scene.last_map, curr_map_idx);

        curr_image_scene.have_init = 1;
        curr_image_scene.is_lock_occupy -= 1;

    }

    vector<INDEX> motion_branch_predict;

    while(elapsed_ms < limit_time && curr_image_scene.is_lock_occupy == 0)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < curr_image_scene.Require_object_list.size();q++)
            {
                Require_Object& require_object = curr_image_scene.Require_object_list[q].require_object;

                if(require_object.require_object_kind == 1)
                {

                    ;
                    
                } else if(require_object.require_id_or_idx == 3) {

                    if(require_object.require_kind = 10 && require_object.require_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            INDEX rough_idx;
            INDEX target_idx = require_object_select(Retrieve_require_list, curr_image_scene.Construct_require_sort);

            if(rough_idx == 0)
            {
                A_Space_Base_Compontent_Detect(target_idx, curr_map_idx);

            } else {
                A_Neuro_Cognition_Generate(target_idx, curr_map_idx, 1);
            }
            
        }
        break;
        case 2:
        {
            INDEX target_node_idx = require_object_select( Model_require_list, curr_image_scene.Model_require_sort);
            Image_A_Neuro_Model(curr_map_idx, target_node_idx);
        }
        }
        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

}


void Image_Scene_Simulate(INDEX image_scene_idx, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;
    
    Image_Scene& curr_image_scene = Image_Scene_Storage[image_scene_idx];
    unordered_map< ID, vector<Focus_Object> >& Id_find_focus_object = curr_image_scene.Id_find_focus_object;
    vector<INDEX>& Image_overall_node_list = curr_image_scene.Overall_image_node_list;
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = curr_image_scene.Id_find_overall_node;
    vector<Variable_Attribute>& Require_object_list = curr_image_scene.Require_object_list;
    
    Advanced_Value_Sort& Require_object_sort = curr_image_scene.Require_object_sort;
    vector<Variable_Attribute>& Scene_variable_attribute_List = curr_image_scene.Scene_variable_attribute_List;

    int root_require;

    int allocation_require;

    vector<INDEX> seed_scene_idx_list;

    Generate_Permit simulate_permit;


    while(elapsed_ms < limit_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < curr_image_scene.Require_object_list.size();q++)
            {
                Require_Object& require_object = curr_image_scene.Require_object_list[q].require_object;

                if(require_object.require_object_kind == 1)
                {

                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_detail_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            INDEX curr_node_idx;
            A_Neuro_Simulate_Generate(curr_node_idx, image_scene_idx, 4, simulate_permit);
        }
        break;
        case 2:
        {
            
        }
        break;
        case 3:
        {

        }
        break;
        case 4:
        {

        }
        break;
        case 5:
        {

        }
        break;
        case 6:
        {

        }
        }

        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
}



void Time_A_Neuro_Model(INDEX scene_idx, INDEX time_node_idx)
{
    General_Node& curr_node = General_Node_Storage[time_node_idx];

    A_Neuro_Basic_Evolve(time_node_idx, 3);

    Time_Scene& curr_time_scene = Time_Scene_Storage[scene_idx];
    vector<Advanced_Value_Sort>& Single_Time_attention_sort = curr_time_scene.Single_Time_attention_sort;
    vector<long long>& Single_record_time_list = curr_time_scene.Single_record_time_list;
    vector<Variable_Attribute>& Require_object_list = curr_time_scene.Require_object_list;
    vector<INDEX>& Overall_time_node_list = curr_time_scene.Overall_time_node_list;

    Node_Time_Attribute node_time_attribute = read_a_Node_Time_Attribute(time_node_idx);
    long long curr_node_time = int_time_to_longlong_time(node_time_attribute.front_time, node_time_attribute.back_time);
    vector<Variable_Attribute>& Node_variable_attribute_list = curr_node.Node_variable_attribute_list;

    int i = 0;
    while( i < Node_variable_attribute_list.size() )
    {
        if(Node_variable_attribute_list[i].require_object.is_require != 4)
            continue;
        
        Require_Object& require_object = Node_variable_attribute_list[i].require_object;

        if(require_object.require_kind != 2)
            continue;

        i++;

        Neuro_Time_Item time_require = Node_variable_attribute_list[i].neuro_time_item;

        if(time_require.is_neuro_item != 1 && time_require.item_is_time != 3)
            continue;
        
        int left_time = time_require.time_left_range * time_require.time_scale;
        int right_time = time_require.time_right_range * time_require.time_scale;

        int begin_time = curr_node_time - left_time;
        int end_time = curr_node_time - right_time;

        if(begin_time < curr_time_scene.begin_time)
            begin_time = curr_time_scene.begin_time;

        if(end_time < curr_time_scene.end_time)
            end_time = curr_time_scene.end_time;

        begin_time = (begin_time - curr_time_scene.begin_time) / 100;
        end_time = (end_time - curr_time_scene.end_time) / 100;

        if(begin_time < -16)
            begin_time = -16;

        if(end_time > 16)
            end_time = 16;

        for(int q = begin_time ; q <= end_time; q++)
        {
            while(require_object.require_value > 0)
            {
                Advanced_Value_Sort curr_value_sort = Single_Time_attention_sort[q];
                INDEX node_idx = get_a_highist_value_of_sort(curr_value_sort);

                General_Node& chose_model_node = General_Node_Storage[node_idx];
                chose_model_node.time_predict_stability;

                Neuro_Time_Item time_condition;
                time_condition.time_scale = 8;
                time_condition.time_left_range = q / time_condition.time_scale;
                time_condition.time_right_range = time_condition.time_left_range + 1;
                time_condition.related_id = chose_model_node.self_id;
            }

        }
    }
        
}


void Time_Cognition_Calculate(INDEX time_scene_idx, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Time_Scene curr_time_scene = Time_Scene_Storage[time_scene_idx];
    Advanced_Value_Sort& Require_object_sort = curr_time_scene.Require_object_sort;
    vector<Variable_Attribute>& Require_object_list = curr_time_scene.Require_object_list;

    INDEX time_node_idx;

    while(elapsed_ms < limit_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
            case 0:
            {
            for(int q = 0; q < curr_time_scene.Require_object_list.size();q++)
            {
                Require_Object& Require_object = curr_time_scene.Require_object_list[q].require_object;

                if(Require_object.require_object_kind == 1)
                {
                    
                    
                } else if(Require_object.require_object_kind == 2) {

                    if(Require_object.require_kind = 10 && Require_object.require_detail_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            vector<INDEX> generate_overall_node_list;
            generate_overall_node_list.reserve(32);

            time_node_idx;
            generate_overall_node_list = A_Neuro_Cognition_Generate(time_node_idx, time_scene_idx, 3);
        }
        break;
        case 2:
        {
            Time_A_Neuro_Model(time_node_idx, time_scene_idx);
        }
        }

        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    }
}


void Time_Scene_Simulate(INDEX time_simu_scene, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;
    
    Time_Scene& curr_time_scene = Time_Scene_Storage[time_simu_scene];
    vector<Variable_Attribute>& Require_object_list = curr_time_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_time_scene.Require_object_sort;
    vector< vector<int> >& Require_object_weights_map= curr_time_scene.Require_object_weights_map;
    vector<Variable_Attribute>& Related_scene_list = curr_time_scene.Scene_variable_attribute_List;
    unordered_map<ID, vector<Focus_Object> >& Id_find_focus_object = curr_time_scene.Id_find_focus_object;
    vector<General_Node>& Time_local_node_list = curr_time_scene.Local_time_node_list;


    vector<INDEX> process_element;
    
    Generate_Permit simulate_permit;
    simulate_permit.condition_time_permit = 1;
    simulate_permit.result_time_permit = 1;
    simulate_permit.condition_number_permit = 1;
    simulate_permit.result_number_permit = 1;
    vector<INDEX> wait_classify_node;
    
    while(elapsed_ms < limit_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch(task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < curr_time_scene.Require_object_list.size();q++)
            {
                Require_Object& require_object = curr_time_scene.Require_object_list[q].require_object;

                if(require_object.require_object_kind == 1)
                {

                    
                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_detail_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            vector<INDEX> generate_id;

            for(INDEX node_idx : process_element)
            {
                generate_id = A_Neuro_Simulate_Generate(node_idx, time_simu_scene, 3, simulate_permit);
            }

            for(int a = 0; a < generate_id.size(); a++)
            {
                INDEX generate_idx = generate_id[a];

                Require_Object require_object;
                require_object.require_kind = 300;
            }
        }
        break;
        case 2:
        {
            Require_object_weights_map;
            ID object_id;

            INDEX require_object_idx;
            Require_Object& require_object = Require_object_list[require_object_idx].require_object;

            if(require_object.require_object_kind == 1)
            {
                if(require_object.require_kind == 30)
                {
                    require_object.require_id_or_idx;
                    General_Node general_node;

                    general_node.self_id = require_object.require_id_or_idx;

                    Time_local_node_list.push_back(general_node);
                }

            } else if(require_object.require_object_kind == 3)
            {
                ;
            }

        }
        break;
        case 3:
        {
            INDEX appraise_idx;
            Multiple_Probability_Cohesion_Appraise(appraise_idx, time_simu_scene);
        }
        break;
        case 4:
        {
            Time_Scene seed_simu_scene;
            seed_simu_scene.Local_time_node_list;
            INDEX add_simu_idx = create_a_Time_Scene(seed_simu_scene);

            Link_Scene_Attribute link_scene;
            link_scene.related_kind == 1;
            link_scene.related_kind == 3;
            link_scene.related_value;
            link_scene.scene_idx = add_simu_idx;
            Variable_Attribute sva = {.link_scene_attribute = link_scene};
            Related_scene_list.push_back(sva);
        }
        break;
        case 5:
        {
            INDEX process_scene_idx;
            char limit_time;
            General_Scene_Simulate(process_scene_idx, limit_time);
        }
        }

        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

}



vector<ID> find_Common( vector<ID>& group_a, vector<ID>& group_b)
{
    unordered_set<ID> s(group_a.begin(), group_a.end());
    vector<ID> result;
    result.reserve(min(group_a.size(), group_b.size()));

    for (int x : group_b)
    {
        if (s.count(x))
        {
            result.push_back(x);
        }
    }

return result;
}


    struct Result_Diff
    {
        vector<ID> less;
        vector<ID> more;
    };


Result_Diff diff_element_search(
    vector<ID>& A, vector<ID>& B)
{
    unordered_set<ID> set_A(A.begin(), A.end());
    unordered_set<ID> set_B(B.begin(), B.end());

    Result_Diff result;

    for (ID & x : B)
    {
        if (!set_A.count(x))
            result.more.push_back(x);
    }

    for (ID & y : A)
    {
        if (!set_B.count(y))
            result.less.push_back(y);
    }

return result;
}


inline vector<float> Action_Bridge_World(INDEX action_scene_idx)
{
    General_Scene& The_motion_scene = General_Scene_Storage[action_scene_idx];

    Node_Action_Attribute node_motion_attribute;
    node_motion_attribute;

    Scene_Action_Attribute scene_motion_attribute;
    long long last_time = int_time_to_longlong_time(scene_motion_attribute.time_front, scene_motion_attribute.time_end);
    int gap_time = CURRENT_MODEL_TIME - last_time;
    INDEX last_motion_idx = scene_motion_attribute.action_idx;
    
    gap_time;
    The_motion_scene;
    Image_Scene The_image_scene = Image_Scene_Storage[Newist_Image_Idx];

    vector< vector<INDEX> > motion_branch_predict;
    vector<INDEX> reality_event;

    vector<float> quality_rate;


    return quality_rate;
}


inline void Link_Path_Detect_Moudle(INDEX check_inside_node, INDEX scene_idx,
    vector<General_Node>& inside_node,
    unordered_map<ID, INDEX>& id_to_inside_detect_node)
{
    int task_weights[6];
    if(check_inside_node == 0)
        return;

    General_Node& curr_node = inside_node[check_inside_node];

    ID self_id = curr_node.self_id;

    if(self_id == 0 || curr_node.node_attention < 10)
        return;
    
    int* ptr = node_find(self_id);

    if(ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(self_id);
        return;
    }

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    ID generate_id;

    char base_connection = inside_node[check_inside_node].component;

    for (short fea = 0; fea < neuro_head.used_item_num; )
    {
        int stat_num = *(ptr + 1);
        int realize_num = *(ptr + 2);
        float probability = realize_num / stat_num;

        Link_Node_Attribute link;
        link.connect_direction = base_connection;
        link.link_value = probability;

        Attribute_Head_Item attribute_head = attribute_head_read(ptr);

        char condition_kind;
        char condition_num = attribute_head.condition_num;
        char condition_size = 0;

        char result_kind;
        char result_num = attribute_head.result_num;
        char result_size = 0;

        int a = 0;

        while(a < result_num)
        {
            return_item_kind_and_size(ptr, result_kind, result_size);
            
            generate_id = *(ptr + result_size - 1);

            if( id_to_inside_detect_node.count(generate_id) )
            {
                General_Node& check_node = inside_node[generate_id];

                if( check_node.component != base_connection);
                {
                    check_node.component = 3;

                    INDEX node_idx = id_to_inside_detect_node[generate_id];
                    task_weights[1] += 1;
                    task_weights[0] -= 10;
                }

                if(check_node.complete_or_probability < probability)
                    check_node.complete_or_probability = probability;
                
            } else {
                INDEX pos_idx = inside_node.size();
                General_Node new_node;
                new_node.self_id = generate_id;
                new_node.component = base_connection;
                inside_node.push_back(new_node);
                id_to_inside_detect_node[generate_id] = pos_idx;
            }
            
            a++;
        }
        
        fea += attribute_head.item_num;
        ptr += attribute_head.item_num;
    }

}



void Action_Calculate_Execulate(INDEX scene_idx, char limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    General_Scene& action_organise_scene = General_Scene_Storage[scene_idx];
    vector<Variable_Attribute>& Require_object_list = action_organise_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = action_organise_scene.Require_object_sort;
    Variable_Attribute link_scene = read_a_scene_related_motion_detect(scene_idx);
    INDEX motion_connect_scene_idx = link_scene.link_scene_attribute.scene_idx;
    General_Scene& motion_connect_detect_scene = General_Scene_Storage[motion_connect_scene_idx];
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = motion_connect_detect_scene.Id_find_overall_node;
    vector<INDEX>& General_overall_node_list = motion_connect_detect_scene.Overall_general_node_list;
    vector<General_Node>& General_local_node_list = motion_connect_detect_scene.Local_general_node_list;
    unordered_map<ID, vector<INDEX> >& Id_find_local_node = motion_connect_detect_scene.Id_find_local_node;


    unordered_map<ID, INDEX> Temporary_id_to_inside_node;

    int need_do = 1;
    bool think_suit = 1;
    int check_idx = 0;

    vector<Link_Node_Attribute> probability_record_vec;
    vector<INDEX> finally_probability_vec;
    

    while(elapsed_ms < limit_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < action_organise_scene.Require_object_list.size();q++)
            {
                Require_Object& require_object = action_organise_scene.Require_object_list[q].require_object;

                if(require_object.require_object_kind >= 10 && require_object.require_object_kind < 20 )
                {
                    if(require_object.require_object_kind)
                    {
                        General_Node node_a;
                        General_Node node_b;
                        node_a.component = 1;
                        node_b.component = 2;
                        General_local_node_list.push_back(node_a);
                        General_local_node_list.push_back(node_b);
                        Temporary_id_to_inside_node[node_a.self_id] = 1;
                        Temporary_id_to_inside_node[node_b.self_id] = 2;
                    }
                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_detail_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            check_idx ;

            Link_Path_Detect_Moudle(scene_idx, check_idx, General_local_node_list,
                Temporary_id_to_inside_node);
        }
        break;
        case 2:
        {
            char source_direction;
            INDEX target_node_idx;
            INDEX source_node_idx;
            vector<INDEX> wait_check_vec;
            float allow_probability = 0.1;

            Link_Node_Attribute source_probability_record;
            source_probability_record.link_idx_or_id = source_node_idx;
            source_probability_record.link_value = 1;
            probability_record_vec.push_back(source_probability_record);
            wait_check_vec.push_back(0);

            for(INDEX choose_probability_idx : wait_check_vec)
            {
                source_probability_record = probability_record_vec[choose_probability_idx];
                INDEX choose_node_idx = source_probability_record.link_idx_or_id;

                General_Node& choose_node = General_local_node_list[choose_node_idx];

                for(int a = 0; a < choose_node.Node_variable_attribute_list.size(); a++)
                {
                    Link_Node_Attribute link = choose_node.Node_variable_attribute_list[a].link_node_attribute;
                    
                    if(link.is_node_link != 3 || link.connect_direction != source_direction)
                        continue;

                    Link_Node_Attribute probability_record;
                    probability_record.link_idx_or_id = link.link_idx_or_id;
                    probability_record.link_value = link.link_value * source_probability_record.link_value;
                    probability_record.link_value_II = choose_probability_idx;

                    if(probability_record.link_value < allow_probability)
                        continue;

                    probability_record_vec.push_back(probability_record);

                    if(link.link_value == target_node_idx)
                    {
                        finally_probability_vec.push_back(probability_record_vec.size() - 1);
                    }
                }
            }

        }
        break;
        case 3:
        {
            probability_record_vec;
            finally_probability_vec;

            int highist_value = 0;
            INDEX highist_idx;

            for(INDEX compare_idx : finally_probability_vec)
            {
                Link_Node_Attribute compare_one = probability_record_vec[compare_idx];
            }

            Link_Node_Attribute highist_one = probability_record_vec[highist_idx];

            Link_Node_Attribute curr_one = highist_one;

            int average_time;

            while(curr_one.link_value != 0)
            {
                curr_one = probability_record_vec[curr_one.link_value_II];
            }

            int score = average_time*highist_one.link_value;
        }
        break;
        case 4:
        {
            vector<INDEX> select_path_element;

            Link_Node_Attribute curr_one;
            while(curr_one.link_value != 0)
            {
                select_path_element.push_back(curr_one.link_idx_or_id);
                curr_one = probability_record_vec[curr_one.link_value_II];
            }

            INDEX gen_scene_idx = create_a_General_Scene();

            for(INDEX node_idx : select_path_element)
            {
                General_Node local_general_node = General_local_node_list[node_idx];
                General_Node generate_general_node;
                generate_general_node.self_id = local_general_node.self_id;

                INDEX gen_node_idx = create_a_General_Node(generate_general_node);

                A_Neuro_Simulate_Generate( gen_node_idx , gen_scene_idx, 1);
            }
        }
        break;
        case 5:
        {
            INDEX plan_idx;

            while(need_do || think_suit)
            {

            }
        }
        break;
        case 6:
        {
            ;
        }
        break;
        case 7:
        {
            vector<float> quality_list = Action_Bridge_World(scene_idx);

        }
        break;
        case 8:
        {
            Scene_Action_Attribute sma;
            sma.action_idx;

            Newist_Image_Idx;

            CURRENT_32_gap_MODEL_TIME;

            CURRENT_8_gap_MODEL_TIME;
            

            Action_Choose_List;
            {

            }

        }
        break;
        case 11:
        {

        }
        }

        auto end = chrono::steady_clock::now();
        elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }


return;
}


void Text_Neuro_Frequency_Init_Model(INDEX text_network_idx,
    INDEX node_idx)
{
    Text_Scene& text_network = Text_Scene_Storage[text_network_idx];
    vector<INDEX> Text_node_storage_list = text_network.Overall_text_node_list;
    vector< vector<INDEX> >Text_node_space_storage_list = text_network.Text_node_space_from_record_list;

    INDEX text_node_idx = Text_node_storage_list[node_idx];
    General_Node& curr_node = General_Node_Storage[text_node_idx];
    ID id = curr_node.self_id;

    INDEX text_pos;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Text_combo_unordered 
        = text_network.Node_combo_find_repeat;

    vector<int> front_modeling_require_scan(8, 0);
    vector<int> back_modeling_require_scan(8, 0);

    int* first_ptr = node_find(id);
    int* ptr = first_ptr;

    if(ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(id);
        return;
    }

    float forward_predict_stability = *(first_ptr + 4);
    float backward_predict_stability = *(first_ptr + 5);
    
    if(curr_node.model_require >= 7)
    {
        Node_Text_Attribute self_text_attribute = read_a_Node_Text_Attribute(text_node_idx);

        vector<Variable_Attribute>& Variable_list = curr_node.Node_variable_attribute_list;

        char scan_range = curr_node.model_require / 2;
        
        for(int l = 0; l < Variable_list.size(); l++)
        {
            INDEX idx = Variable_list[l].link_node_attribute.link_idx_or_id;
            General_Node_Storage[idx].node_calu_state = 2;
        }

        INDEX last_scan_outside_idx = 0;

        for(int i = -scan_range; i < scan_range; i++)
        {
            INDEX cur_scan_outside_idx = (text_pos + i)/8;
            if(last_scan_outside_idx == cur_scan_outside_idx)
                continue;

            last_scan_outside_idx = cur_scan_outside_idx;

            vector<INDEX> cur_inside_list = Text_node_space_storage_list[cur_scan_outside_idx];
            int inside_size = cur_inside_list.size();
            for(int a = 0; a < inside_size; a++)
            {
                INDEX idx = cur_inside_list[a];
                General_Node& object_node = General_Node_Storage[idx];

                if(object_node.node_calu_state == 2 || i == 0)
                    continue;
                
                int need_add_stat = curr_node.model_require - object_node.space_predict_stability
                    - i + curr_node.neuro_spare_size;

                if(i < 0)
                    need_add_stat *= forward_predict_stability;
                else if(i > 0)
                    need_add_stat *= backward_predict_stability;
                
                if(need_add_stat > 0)
                {
                    Attribute_Head_Item item_head;
                    vector<General_Condition> general_condition_list;
                    vector<General_Result> general_result_list;

                    Neuro_Text_Item text_item;
                    text_item.left_distance = 1;
                    text_item.distance_scale = 4;
                    text_item.target_id = object_node.self_id;;

                    General_Condition condition = {.text_condition = text_item};
                    general_condition_list.push_back(condition);

                    item_head.attribute_kind = 1;
                    item_head.condition_num += 1;
                    
                    Simple_Neuro_Attribute_write(curr_node.self_id, 0, 1,
                        item_head, general_condition_list, general_result_list);

                    Link_Node_Attribute la;
                    la.link_Attribute = 1;
                    la.link_Kind = 4;
                    la.link_idx_or_id = idx;

                    Variable_Attribute flex = {.link_node_attribute = la};
                    curr_node.Node_variable_attribute_list.push_back(flex);
                }
            }
        }
    }
}


void Text_Neuro_Identity_Init_Model(
    INDEX text_scene_idx, INDEX node_idx)
{
    Text_Scene& text_scene = Text_Scene_Storage[text_scene_idx];
    vector<INDEX>& Text_node_storage_list = text_scene.Overall_text_node_list;
    vector< vector<INDEX> >& Text_node_space_from_record_list = text_scene.Text_node_space_from_record_list;

    INDEX text_node_idx = Text_node_storage_list[node_idx];
    General_Node& curr_node = General_Node_Storage[text_node_idx];
    ID id = curr_node.self_id;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Text_combo_unordered 
        = text_scene.Node_combo_find_repeat;

    vector<INDEX> identity_list;
    unordered_set<INDEX> identity_unordered;

    int* ptr = node_find(id);

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    int* first_ptr = ptr;

    for(short fea = 0; fea <  neuro_head.used_item_num; )
    {
        Attribute_Head_Item attribute_head = attribute_head_read(ptr);

        char condition_kind;
        char condition_num = attribute_head.condition_num;
        char condition_size = 0;
        
        char result_kind;
        char result_num = attribute_head.result_num;
        char result_size = 0;

        ptr += 3;

        for(int q = 0; q < result_num; q++)
        {
            return_item_kind_and_size(ptr, result_kind, result_size);

            if(result_kind != 6)
            {
                first_ptr += attribute_head.item_num;
                ptr = first_ptr;
                break;
            }

            Neuro_Belief_Item belief_item = *(Neuro_Belief_Item*)(ptr);

            if(belief_item.detail_kind != 1)
            {
                first_ptr += attribute_head.item_num;
                ptr = first_ptr;
                break;
            }

            belief_item.belief_object_id;
            belief_item.u_value;
            belief_item.l_value;
            
        }
    }



    if(curr_node.identify_require >= 5)
    {
        text_scene.sum_identify_require;
        curr_node.identify_require;

        int Current_Attention_List_size = Attention_List.size();

        for(int a = 0; a < Current_Attention_List_size; a++)
        {

            vector<Require_Object> identity_tend;
            vector<int> tend_rate;


            {
                Require_Object& attention_object = Require_Object_List[a].require_object;
                attention_object.require_value;
            }

            vector<ID> possible_identity_object;
            vector<Neuro_Link_Attribute> possible_identity_relation;

            
            Attribute_Head_Item item_head;
            vector<General_Condition> general_condition_list;
            vector<General_Result> general_result_list;
            
            item_head.attribute_kind == 7;

            for(ID b : possible_identity_object)
            {
                item_head.result_num += 1;

                Neuro_Belief_Item identity_belief;
                identity_belief.belief_object_id = possible_identity_object[b];
                identity_belief.detail_kind == 1;
                identity_belief.logic == 1;
                General_Result GR = {.belief_result = identity_belief};

                general_result_list.push_back(GR);
            }


            Simple_Neuro_Attribute_write(curr_node.self_id, 0, 1,
                item_head, general_condition_list, general_result_list);
        }
        
    }
}

void Text_Neuro_Identity_Advanced_Model(ID id)
{

    int* first_ptr = node_find(id);
    int* ptr = first_ptr;

    if(ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(id);
        return;
    }

    vector<ID> neuro_list;

    Find_Common_Element_More(neuro_list);

    Neuro_Belief_Item belief_attribute;
    belief_attribute.belief_object_id;
    belief_attribute.detail_kind = 1;

    vector<Neuro_Link_Attribute> link_list;

    Find_Common_Relation_Rate(link_list);


}



inline void init_text_require(vector< vector<int> >& retrieve_require, unsigned short text_size, char unit_require_value)
{
    int need_size = text_size;
    int need_layer = 1;

    while(need_size >= 8)
    {
        need_size /= 8;
        need_layer += 1;
    }

    retrieve_require.resize(need_layer);

    need_size = text_size;
    need_layer = 0;


    while(need_size >= 8)
    {
        vector<int>& curr_retrieve_require = retrieve_require[need_layer];
        curr_retrieve_require.resize(need_size);

        for(int q = 0; q < text_size; q++)
        {
            curr_retrieve_require[q] = unit_require_value;
        }

        need_size /= 8;
        need_layer += 1;
        unit_require_value *= 8;
    }
}


inline void text_network_information_generate(
    INDEX object_node_idx, INDEX text_scene_idx,
    vector<INDEX>& generate_formation_node_list)
{
    Text_Scene text_scene = Text_Scene_Storage[text_scene_idx];
    unordered_map< ID, vector<INDEX> >& Id_find_overall_node = text_scene.Id_find_overall_node;/*璁板綍 瀛樺湪瀛楃鐨勬煡璇㈣〃*/
    vector<Variable_Attribute>& Require_object_list= text_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = text_scene.Require_object_sort;
    ID object_id = read_a_General_Node(object_node_idx).self_id;
    int* ptr = node_find(object_id);
    int* item_first_ptr;

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    int neuro_item_num = neuro_head.used_item_num;
    int fea = 0;

    vector<Neuro_Link_Attribute> Joint_Attribute_List;
    Joint_Attribute_List.reserve(4);
    
    while(fea > neuro_item_num)
    {
        Attribute_Head_Item attribute_head = attribute_head_read(ptr);

        char condition_num = attribute_head.condition_num;
        char condition_kind;
        char condition_size = 0;

        char result_num = attribute_head.result_num;
        char result_kind;
        char result_size = 0;

        item_first_ptr = ptr;
        ptr += 3;

        switch (attribute_head.attribute_kind)
        {
        case 1:
        {
            for(int q = 0; q < result_num; q++)
            {
                return_item_kind_and_size(ptr, result_kind, result_size);

                switch (result_kind)
                {
                    case 2:
                    {
                        Neuro_Number_Item number_item = *(Neuro_Number_Item*)(ptr);

                        Neuro_Number_Item number_require;
                        Variable_Attribute variable_object = {.neuro_number_item = number_require};
                        push_back_a_variable_object_to_General_Node(object_node_idx, variable_object);
                    }
                    break;
                    case 4:
                    {
                        ID stat_id = *(ptr + attribute_head.item_num);

                        if( Id_find_overall_node.count(stat_id) )
                        {
                            vector<INDEX>& node_idx_list = Id_find_overall_node[stat_id];

                            for(INDEX link_idx: node_idx_list)
                            {
                                Neuro_Text_Item text_item = *(Neuro_Text_Item*)(ptr);

                                Neuro_Text_Item text_require;
                                text_require.left_distance = text_item.left_distance;
                                text_require.right_distance = text_item.right_distance;
                                text_require.distance_scale = text_item.distance_scale;
                                text_require.target_id = link_idx;
                                Variable_Attribute variable_object = {.neuro_text_item = text_require};
                                push_back_a_variable_object_to_General_Node(object_node_idx, variable_object);

                            }
                        }
                    }
                    break;
                    case 5:
                    {
                        Neuro_Belief_Item belief_item = *(Neuro_Belief_Item*)(ptr);
                        

                    }
                }
            }
            
        }
        break;
        case 2:
        {
            return_item_kind_and_size(ptr, result_kind, result_size);
        }
        break;
        case 3:
        {

            for(int a = 0; a < attribute_head.result_num; a++)
            {
                return_item_kind_and_size(ptr, result_kind, result_size);
                

                Neuro_Text_Item text_item = *(Neuro_Text_Item*)(ptr);
                
                ID id = text_item.target_id;
                General_Node node;
                node.self_id = id;
                INDEX gen_idx = create_a_General_Node(node);

                generate_formation_node_list.push_back(gen_idx);

                Id_find_overall_node[id].push_back(gen_idx);

                //娣诲姞閾炬帴
                Neuro_Text_Item text_require;
                text_require.left_distance = text_item.left_distance;
                text_require.right_distance = text_item.right_distance;
                text_require.distance_scale = text_item.distance_scale;
                text_require.target_id = text_item.target_id;
                Variable_Attribute variable_object = {.neuro_text_item = text_require};
                push_back_a_variable_object_to_General_Node(object_node_idx, variable_object);
            }
        
        }
        }

        fea += result_size;
        ptr += result_size;
    }

}



inline void text_network_sequence_organise(
    vector<INDEX>& generate_formation_node_list)
{
    for(int a = 0; a < generate_formation_node_list.size(); a++)
    {
        INDEX idx = generate_formation_node_list[a];
        General_Node node = read_a_General_Node(idx);


        Node_Sequence_Info node_sequence_info = read_a_node_sequence_info(idx);

        int reward_I;
        int reward_II;

        auto variable_list = node.Node_variable_attribute_list;

        unordered_map< INDEX,int > hash_reward;
        
        //
        Advanced_Value_Sort node_important_sort_list;

        vector<INDEX> importance_record_list;
        vector<INDEX> blank_idx_list;

        for(Variable_Attribute attribute : variable_list)
        {
            if(attribute.neuro_text_item.is_neuro_item != 1)
                continue;
            
            Neuro_Text_Item simple_text;
            Neuro_Number_Item simple_number;

            Value_Sort_Unit input_unit;
            
            if( hash_reward.count(simple_text.target_id) )
            {
                int& reward = hash_reward[simple_text.target_id];
                Value_Sort_Unit find_unit;
                find_unit.target_idx = simple_text.target_id;
                find_unit.value = reward;

                input_unit.value = reward;
                node_important_sort_list.change_a_unit( find_unit, input_unit);
                reward += simple_number.number_I;

            } else {
                hash_reward[simple_text.target_id] = simple_number.number_I;

                input_unit.target_idx = simple_text.target_id;
                input_unit.value = simple_number.number_I;
                node_important_sort_list.insert_a_unit(input_unit);
            }

            input_unit;
            input_unit.target_idx;
            //
        }

        INDEX Max_one = node_important_sort_list.list[0].lower_one;
        node_important_sort_list.list[Max_one].target_idx;
        int history_top = 0;

        // text_operate(curr_text_idx, temporary_text_node_list, 123, 456 ,0);
        


        deque<INDEX> output_element;
    }
}


void Text_Cognition_Simulate_Calculate(INDEX curr_text_idx, short limit_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Text_Scene& curr_text_network = Text_Scene_Storage[curr_text_idx];
    vector< vector<INDEX> >& Text_node_space_storage_list = curr_text_network.Text_node_space_from_record_list;
    unsigned short original_text_size = curr_text_network.original_text_size;
    vector<Variable_Attribute>& Require_object_list = curr_text_network.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_text_network.Require_object_sort;
    vector< vector<int> >& Retrieve_total_require_list = curr_text_network.Retrieve_total_require_list;
    vector< vector<int> >& Model_total_require_list = curr_text_network.Model_total_require_list;
    unordered_map< ID, vector<INDEX> >& Id_find_overall_node = curr_text_network.Id_find_overall_node;

    if(curr_text_network.have_init == 0)
    {
        init_text_require(Retrieve_total_require_list, original_text_size, 10);
        init_text_require(Model_total_require_list, original_text_size, 10);
        curr_text_network.have_init = 1;

        Require_Object retrieve_require;
        retrieve_require.require_kind = 1;
        retrieve_require.require_value = original_text_size*10;
        retrieve_require.require_detail_kind = 0;
        Variable_Attribute va = {.require_object = retrieve_require};
        Require_object_list.push_back(va);

        Require_Object model_require;
        model_require.require_kind = 2;
        model_require.require_value = original_text_size*10;
        model_require.require_detail_kind = 0;
        Variable_Attribute va = {.require_object = model_require};
        Require_object_list.push_back(va);
    }

    
    INDEX retrieve_work_scene;
    INDEX construct_work_scene;

    int satisfied_value = 0;
    int curr_run = 1;

    while(elapsed_ms < limit_time || satisfied_value < 100 && curr_run)//璇�1锟�71锟�1锟�771锟�1锟�71锟�1锟�777
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            for(int q = 0; q < curr_text_network.Require_object_list.size();q++)
            {
                Require_Object& require_object = curr_text_network.Require_object_list[q].require_object;
                curr_text_network.Require_object_value[q];

                if(require_object.require_object_kind == 1)
                {

                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_detail_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            
            INDEX chose_object_idx;

            chose_object_idx = require_object_select(Retrieve_total_require_list, curr_text_network.Retrieve_require_sort);

            
            A_Neuro_Cognition_Generate(chose_object_idx, retrieve_work_scene, 2);

            
        }
        break;
        case 2:
        {
            
            INDEX chose_object_idx = require_object_select(Model_total_require_list, curr_text_network.Model_require_sort);

            INDEX node_idx = Text_node_space_storage_list[chose_object_idx][0];


            A_Neuro_Basic_Evolve(node_idx, 2);

            Text_Neuro_Frequency_Init_Model(retrieve_work_scene, node_idx );
        }
        break;
        case 3:
        {

            char set_time;
            retrieve_work_scene = curr_text_idx;
        }
        break;
        case 4:
        {
            
            vector<INDEX> generate_formation_node_list;
            Id_find_overall_node.reserve( generate_formation_node_list.size() );

            ID aim_object;
            
            text_network_information_generate(aim_object, curr_text_idx, generate_formation_node_list);
            
        }
        break;
        case 5:
        {
            vector<INDEX> generate_formation_node_list;
            
            text_network_sequence_organise(generate_formation_node_list);
        }
        break;
        case 6:
        {
            char set_time;
            retrieve_work_scene = construct_work_scene;
            // task_weights[1] += 0;
        }
        }

        
        
        auto end = chrono::steady_clock::now();
        elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
    
}



vector<Neuro_Manner_Item> statistics_all_manner_attribute(
    ID permit_stat_id, vector<ID> input_object,
    vector<float> number_or_rate)
{
    vector<Neuro_Manner_Item> return_inforamation;
    return_inforamation.resize(10);

    for(ID input_id : input_object)
    {
        int* ptr = node_find(input_id);

        if(ptr == 0)
        {
            add_to_Will_Read_Neuro_Queue(input_id);
            return return_inforamation;
        }

        Neuro_Head_Item neuro_head = neuro_head_read(*ptr);

        ptr += 16;
        short fea = 0;

        while( fea < neuro_head.used_item_num )
        {
            Attribute_Head_Item attribute_head = attribute_head_read(ptr);

            if(attribute_head.attribute_kind == 7)
            {
                Neuro_Manner_Item manner_item = *(Neuro_Manner_Item*)(ptr + 3);

                if(manner_item.effect_target == permit_stat_id)
                {
                    switch (manner_item.manner_kind)
                    {
                    case 1:
                    {
                        return_inforamation[0].manner_value += manner_item.manner_value;
                    }
                    break;
                    case 2:
                    {
                        return_inforamation[1].manner_rate += manner_item.manner_value;
                    }
                    break;
                    case 3:
                    {
                        return_inforamation[2].manner_value += manner_item.manner_value;
                    }
                    break;
                    case 4:
                    {
                        return_inforamation[3].manner_value += manner_item.manner_value;
                    }
                    case 5:
                    {
                        return_inforamation[4].manner_value += manner_item.manner_value;
                    }
                }
            }


            switch (manner_item.manner_kind)
            {
                case 1:
                {
                    return_inforamation[5].manner_value += manner_item.manner_value;
                }
                break;
                case 2:
                {
                    return_inforamation[6].manner_rate += manner_item.manner_value;
                }
                break;
                case 3:
                {
                    return_inforamation[7].manner_value += manner_item.manner_value;
                }
                break;
                case 4:
                {
                    return_inforamation[8].manner_value += manner_item.manner_value;
                }
                case 5:
                {
                    return_inforamation[9].manner_value += manner_item.manner_value;
                }
                }

            }

            fea += attribute_head.item_num;
            ptr += attribute_head.item_num;
        }
    }

    return return_inforamation;
}



void Interface_Text_Construct_Calculate( INDEX display_idx, char limit_time)
{
    Gtk3_Text_Display& gtk_interface = Gtk3_Text_Display_Storage[display_idx];

    INDEX input_text_idx = gtk_interface.input_Text_Scene_Idx;
    Text_Scene& input_text_network = Text_Scene_Storage[input_text_idx];

    INDEX output_text_idx = gtk_interface.output_Text_Scene_Idx;
    Text_Scene& output_text_network = Text_Scene_Storage[output_text_idx];
    vector<General_Node>& Text_local_node_list = output_text_network.Local_text_node_list;
    vector<INDEX>& Free_text_local_node_idx = output_text_network.Free_local_text_node_idx;
    unordered_map<ID, vector<INDEX> >& Id_find_local_node = output_text_network.Id_find_local_node;
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = output_text_network.Id_find_overall_node;

    INDEX text_simu_virtual_scene_idx;
    Image_Scene text_simu_virtual_scene;

    vector<Wait_Active_Neuro> wait_active_neuro_list;
    vector<INDEX> free_wait_active_neuro_idx;
    
    General_Scene primitive_scene;
    create_a_General_Scene(primitive_scene);
    
    if(gtk_interface.have_init == 0)
    {
        CharVector_To_Network(gtk_interface.input_Text_Scene_Idx, 
            gtk_interface.Input_string, CURRENT_MODEL_TIME);

        gtk_interface.have_init = 1;
    }
    
    INDEX work_scene_idx;

    int task_work_require[10];

    random_device Task_RD;
    mt19937 Task_Gen(Task_RD());
    discrete_distribution<int> task_dist(task_work_require, task_work_require + 4);

    int task_work = 0;



    switch(task_work)
    {
        case 0:
        {
            for(int q = 0; q < input_text_network.Require_object_list.size();q++)
            {
                Require_Object& require_object = input_text_network.Require_object_list[q].require_object;
                input_text_network.Require_object_value[q];

                if(require_object.require_object_kind == 1)
                {
                    
                } else if(require_object.require_object_kind == 3) {

                    if(require_object.require_kind = 10 && require_object.require_detail_kind == 0)
                    {
                        ;
                    }
                }
                
            }
        }
        break;
        case 1:
        {
            
            INDEX process_scene = gtk_interface.input_Text_Scene_Idx;
            char limit_time;
            
            Text_Cognition_Simulate_Calculate(process_scene, limit_time);
        }
        break;
        case 2:
        {
            vector<INDEX> generate_formation_node_list;
            unordered_map< ID, vector<INDEX> > hash_find_id_to_idx;
            hash_find_id_to_idx.reserve( generate_formation_node_list.size() );

            ID aim_object;
            
            text_network_information_generate(aim_object, output_text_idx, generate_formation_node_list);
            
        }
        break;
        case 3:
        {
            vector<INDEX> generate_formation_node_list;
            
            text_network_sequence_organise(generate_formation_node_list);
        }
        break;
        case 4:
        {
            task_work = 0;

            ID permit_stat_id;
            vector<ID> input_object;
            vector<float> number_or_rate;

            statistics_all_manner_attribute(
                permit_stat_id, input_object, number_or_rate);

        }
        break;
        case 6:
        {
            {
                task_work = 0;
            }

            {
                General_Scene_Simulate(work_scene_idx, limit_time);
            }
            
            {
                task_work = 1;
            }
            
        }
        break;
        case 7:
        {
            
            task_work = 0;

            gtk_interface.Input_string;
            
        }
        break;
        case 8:
        {
            task_work = 2;

            Require_Object ro;
            ro.require_value = gtk_interface.input_value_I[0];
            ro.require_object_kind;
            ro.require_id_or_idx;
            
            vector<Require_Object> Work_Aim;
            Work_Aim.push_back(ro);
            
            INDEX scene_idx;
            char limit_time;

            Action_Calculate_Execulate(scene_idx, limit_time);
        }
        break;
        case 9:
        {
            gtk_interface.response_aim_kind = 5;

            task_work = 2;

            char emotion_kind = gtk_interface.input_value_I[0];
            int emotion_value;

            Focus_Object focus_object;
            focus_object.kind = 2;

            Require_Object require_object;

            Require_Object_List;
            Id_Find_Focus_Object;
            
        }
        break;
        case 10:
        {
            task_work = 3;

        }
        break;
        case 11:
        {
            gtk_interface.Output_string = Network_To_CharVector(gtk_interface.output_Text_Scene_Idx);
        }
    }
}



void Auto_Core_Execulute(Single_Thread& thread_data)
{
    while(thread_data.now_work == 1)
    {
        if(!IF_SELF_WORKING)
        {
            while(!IF_SELF_WORKING)
            {
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }


        if(thread_data.need_free_time >= 30)
        {
            auto begin = chrono::steady_clock::now();
            this_thread::sleep_for(chrono::milliseconds(thread_data.need_free_time));
            auto end = chrono::steady_clock::now();
            int duration = chrono::duration_cast< chrono::milliseconds >(end - begin).count();
            thread_data.need_free_time - duration;
        }


        auto start_total = chrono::steady_clock::now();
        int total_elapsed_time = 0;

        unsigned char invest_time = 100 - thread_data.need_free_time;
        int elapsed_ms;

        auto start = chrono::steady_clock::now();

        while(elapsed_ms < invest_time)
        {
            INDEX idx = Formal_Require_Object_Sort.list[0].lower_one;
            idx = Formal_Require_Object_Sort.list[idx].target_idx;
            Require_Object& require_object = All_Formal_Require_Object_List[idx].require_object;

            if(require_object.require_object_kind < 3)
            {
                if(require_object.require_kind == 10)
                {
                    if(require_object.require_object_kind == 2)
                    {
                        General_Node general_node = read_a_General_Node(require_object.require_id_or_idx);
                        vector<Variable_Attribute>& Node_variable_attribute_list
                            = general_node.Node_variable_attribute_list;
                        
                        for(int m = 0; m < Node_variable_attribute_list.size(); m++)
                        {
                            Link_Scene_Attribute link_scene_attribute 
                                = Node_variable_attribute_list[m].link_scene_attribute;

                            if(link_scene_attribute.is_scene_link != 5)
                                continue;

                            switch(link_scene_attribute.scene_kind)
                            {
                            case 1:
                            {
                                Scene_Image_Attribute require_scene_image 
                                    = Node_variable_attribute_list[m].scene_image_attribute;
                            }
                            break;
                            case 2:
                            {
                                Scene_Number_Attribute require_scene_number 
                                    = Node_variable_attribute_list[m].scene_number_attribute;
                            }
                            break;
                            case 3:
                            {
                                Scene_Time_Attribute require_scene_time 
                                    = Node_variable_attribute_list[m].scene_time_attribute;
                            }
                            break;
                            case 4:
                            {
                                Scene_Text_Attribute require_scene_text 
                                    = Node_variable_attribute_list[m].scene_text_attribute;
                            }
                            }
                        }
                    } else
                    {
                        if(require_object.require_id_or_idx == 0)
                        {
                            General_Node general_node;
                            general_node.self_id = require_object.require_id_or_idx;
                            general_node.belong_scene_kind;

                        } else
                        {
                            ;
                        }
                        
                    }
                } else if(require_object.require_kind == 20)
                {
                    ;
                } else if(require_object.require_kind == 30)
                {
                    ;
                }
            } else {

                if(require_object.require_kind == 10 && require_object.require_detail_kind == 1)
                {
                    General_Cognition_Calculate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 10 && require_object.require_detail_kind == 2)
                {
                    Image_Cognition_Calculate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 10 && require_object.require_detail_kind == 3)
                {
                    Time_Cognition_Calculate(require_object.require_id_or_idx, invest_time);

                } else if (require_object.require_kind == 10 && require_object.require_detail_kind == 4)
                {
                    Text_Cognition_Simulate_Calculate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 30 && require_object.require_detail_kind == 1)
                {
                    General_Scene_Simulate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 30 && require_object.require_detail_kind == 2)
                {
                    Image_Scene_Simulate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 30 && require_object.require_detail_kind == 3)
                {
                    Time_Scene_Simulate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 30 && require_object.require_detail_kind == 4)
                {
                    Text_Cognition_Simulate_Calculate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 30 && require_object.require_detail_kind == 5)
                {
                    Action_Calculate_Execulate(require_object.require_id_or_idx, invest_time);

                } else if(require_object.require_kind == 30 && require_object.require_detail_kind == 6)
                {
                    Interface_Text_Construct_Calculate(require_object.require_id_or_idx, invest_time);
                }
            }
            
            auto now = chrono::steady_clock::now();
            elapsed_ms = chrono::duration_cast< chrono::milliseconds >(now - start).count();
            invest_time -= elapsed_ms;
        }

    }

}