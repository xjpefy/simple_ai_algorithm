#include <cstdio>
#include <stdexcept>

#include "Trunk.hpp"





inline void Multiple_Probability_Cohesion_Appraise(INDEX appraise_node_idx)
{
    General_Node appraise_node = read_a_General_Node(appraise_node_idx);
    vector<Link_Variable_Attribute>& Node_variable_attribute_list = appraise_node.Node_variable_attribute_list;

    long long sum_stat = 0;
    long long sum_realize = 0;

    vector<int> stat_list;
    vector<int> realize_list;

    for(int i = 0; i < Node_variable_attribute_list.size(); i++)
    {
        int total_stat = 0;

        Link_Variable_Attribute& va = Node_variable_attribute_list[i];

        if( va.link_node_attribute.link_Kind == 3 )
        {
            INDEX source_idx = appraise_node.Node_variable_attribute_list[i].link_node_attribute.link_idx_or_id;
            General_Node& source_node = General_Node_Storage[source_idx];

            i += 1;
            va = Node_variable_attribute_list[i];
            Neuro_Number_Item& neuro_number_attribute = va.neuro_number_item;

            if(neuro_number_attribute.is_neuro_item != 1
                && neuro_number_attribute.item_is_number != 2)
                continue;

            if(neuro_number_attribute.logic == 1)
            {
                sum_stat += neuro_number_attribute.number_I;
                sum_realize += neuro_number_attribute.number_II;

                stat_list.push_back( neuro_number_attribute.number_I);
                realize_list.push_back( neuro_number_attribute.number_II);
            }
        }

    }

    float total_Probability = sum_stat / sum_realize;
    appraise_node.complete_or_probability = total_Probability;
    appraise_node.l_value = sum_realize;
    appraise_node.u_value = sum_stat;

    int total_divisive_value = 0;

    for(int q = 0; q < stat_list.size(); q++)
    {
        int divisive_value = (sum_realize) - realize_list[q] * (sum_stat / stat_list[q] );
        total_divisive_value += divisive_value;
    }

    write_a_General_Node(appraise_node_idx, appraise_node);
}

void image_attention_appraise(INDEX last_scene_idx, INDEX curr_scene_idx, char invest_time = 10 )
{
    auto start_total = chrono::steady_clock::now();
    int total_elapsed_time = 0;

    General_Scene& curr_scene = General_Scene_Storage[curr_scene_idx];
    vector<Link_Variable_Attribute>& Require_Object_List = curr_scene.Require_object_list;

    General_Scene& last_scene = General_Scene_Storage[last_scene_idx];
    Advanced_Value_Sort& last_attention_node_list = last_scene.Attention_object_sort;
    vector<Link_Variable_Attribute>& last_Require_Object_List = last_scene.Require_object_list;

    INDEX att_idx = last_attention_node_list.list[0].lower_one;

    while( att_idx != 0 && invest_time < total_elapsed_time)
    {
        General_Node& last_node = General_Node_Storage[att_idx];

        Require_Object require_object;
        require_object.require_kind = 1;
        require_object.require_id_or_idx = last_node.self_id;
        require_object.require_value = last_node.node_attention / 10;
        Link_Variable_Attribute va = {.require_object = require_object};
        Require_Object_List.push_back(va);

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

INDEX region_require_object_select(vector< vector<int> >& require_list)
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

void casual_logic_infer_calculate()
{
    //result clear 

    //unresult

    //
    ID reason_element;

    //
    ID new_reason_element[40];

    //
    
}




void locate_identity_element_between_scene(INDEX a_scene_idx, INDEX b_scene_idx)
{
    ;
}



void Concept_A_Neuro_Model(INDEX scene_idx, char cene_kind)
{
    ;
}


void General_A_Neuro_Model(INDEX general_scene_idx, INDEX curr_node_idx )
{
    A_Neuro_Basic_Evolve(curr_node_idx, 1);

    General_Scene curr_general_scene = General_Scene_Storage[general_scene_idx];
    unordered_map< ID, vector<INDEX> > Id_find_overall_node = curr_general_scene.Id_find_overall_node;

    ;
}

void General_Cognition_Calculate(INDEX cur_scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    General_Scene& curr_general_scene = General_Scene_Storage[cur_scene_idx];
    vector<Link_Variable_Attribute>& Require_object_list = curr_general_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_general_scene.Require_object_sort;

    INDEX curr_node_idx;

    random_device Task_RD;
    mt19937 Task_Gen(Task_RD());
    int work_weights[5];

    discrete_distribution<int> task_dist(work_weights, work_weights + 4);
    int task_work = task_dist(Task_Gen);

    while(elapsed_ms < invest_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
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

void General_Scene_Simulate(INDEX curr_scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;
    
    General_Scene& curr_general_scene = General_Scene_Storage[curr_scene_idx];
    unordered_map< ID, vector<Focus_Object> >& Id_find_focus_object = curr_general_scene.Id_find_focus_object;
    vector<INDEX>& Overall_general_node_list = curr_general_scene.Overall_general_node_list;
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = curr_general_scene.Id_find_overall_node;
    vector<Link_Variable_Attribute>& Require_object_list = curr_general_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_general_scene.Require_object_sort;
    vector<Link_Variable_Attribute>& Scene_variable_attribute_List = curr_general_scene.Scene_variable_attribute_List;


    vector<INDEX> Process_Element;

    float root_require_rate;
    float allocation_require_rate;

    vector<INDEX> seed_scene_idx_list;

    Generate_Permit simulate_permit;


    while(elapsed_ms < invest_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 1:
        {
            A_Neuro_Simulate_Generate(task_chose.require_id_or_idx, curr_scene_idx, 3, simulate_permit);
        }
        break;
        case 2:
        {
            INDEX appraise_idx = task_chose.require_id_or_idx;
            Multiple_Probability_Cohesion_Appraise(appraise_idx);
        }
        break;
        case 3:
        {
            INDEX scene_idx;
            push_back_a_node_to_scene(scene_idx, task_chose.require_id_or_idx, 1);
            
        }
        break;
        case 4:
        {
            INDEX node_idx;
            push_back_a_node_to_scene(task_chose.require_id_or_idx, node_idx, 1);
        }
        break;
        case 5:
        {
            General_Scene seed_general_scene;
            seed_general_scene.Local_general_node_list;
            INDEX add_simu_idx = create_a_General_Scene(seed_general_scene);

            Link_Scene_Attribute link_scene;
            link_scene.scene_kind == 3;
            link_scene.related_value;
            link_scene.scene_idx = add_simu_idx;
            Link_Variable_Attribute va = {.link_scene_attribute = link_scene};
            Scene_variable_attribute_List.push_back(va);
        }
        break;
        case 6:
        {
            char invest_time = task_chose.require_value / curr_general_scene.sum_require_value + 1;

            General_Scene_Simulate(task_chose.require_id_or_idx, invest_time);
        }
        }

        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
    

}



void Image_A_Neuro_Space_Model(INDEX curr_scene_idx, INDEX curr_node_idx)
{
    A_Neuro_Basic_Evolve(curr_node_idx, 1);

    Image_Scene& curr_image_scene = Image_Scene_Storage[curr_scene_idx];
    int height = curr_image_scene.height;
    int width = curr_image_scene.width;
    vector< vector<INDEX> >& Node_space_record_list =
        curr_image_scene.Space_form_record[0].record_list;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    vector<Link_Variable_Attribute>& Node_variable_attribute_list = curr_node.Node_variable_attribute_list;
    Node_Image_Single_Attribute node_image_att = read_a_Node_Image_Attribute(curr_node_idx);

    Require_Object require_object ;

    short suit_space_scan_range = 1.5*sqrt(node_image_att.block_num);
    short space_scan_limit_range = 2.5*sqrt(node_image_att.block_num);
    space_scan_limit_range *= node_image_att.observe_size;

    short space_min_block = node_image_att.block_num / 4;
    short space_max_block = node_image_att.block_num * 4;

    int x = node_image_att.x * node_image_att.observe_size;
    int y = node_image_att.y * node_image_att.observe_size;

    short left = x - space_scan_limit_range;
    short right = x + space_scan_limit_range;
    short bottom = y - space_scan_limit_range;
    short top = y + space_scan_limit_range;

    if(left < 0) left = 0;
    if(right >= width) right = width - 1;
    if(bottom < 0) bottom = 0;
    if(top >= height) top = height - 1;

    Advanced_Value_Sort chose_object_sort;
    unordered_set<INDEX> have_link_idx;

    int block_num = node_image_att.block_num;

    char record_idx = 0;
    while(block_num > 16)
    {
        block_num /= 16;
        record_idx += 16;
    }

    int record_distance = curr_image_scene.Space_form_record[record_idx].record_distance;
    int record_width = width / record_distance;

    bottom /= record_distance;
    top /= record_distance;
    left /= record_distance;
    right /= record_distance;

    int x0 = x / record_distance;
    int y0 = y / record_distance;

    int width_reduce_middle = record_width - (right-left);


    for(int i = 0; i < Node_variable_attribute_list.size(); i++ )
    {
        Link_Node_Attribute link_ = Node_variable_attribute_list[i].link_node_attribute;
        if(link_.is_node_link != 4 && link_.link_Kind != 3)
            continue;

        i++;

        Neuro_Image_Item image_item = Node_variable_attribute_list[i].neuro_image_item;

        if(image_item.is_neuro_item != 1 && image_item.item_is_image != 1)
            continue;
        
        have_link_idx.insert(image_item.target_id);
    }


    for(int a = 0; a < Node_variable_attribute_list.size(); a++)
    {
        if(Node_variable_attribute_list[a].require_object.is_require != 10
            && Node_variable_attribute_list[a].require_object.require_kind != 2)
            continue;
        
        a++;

        Neuro_Image_Item image_require = Node_variable_attribute_list[a].neuro_image_item;

        if(image_require.is_neuro_item != 1 && image_require.item_is_image != 1)
            continue;

        
        INDEX retrieval_idx = bottom * record_width + left;

        for(int y1 = bottom; y1 <= top; y1++)
        {
            for(int x1 = left; x1 <= right; x1++)
            {
                int true_x_dis = x1 - x0;
                int true_y_dis = y1 - y0;

                float distance_encourage_value = abs( suit_space_scan_range - sqrtf( true_x_dis*true_x_dis + true_y_dis*true_y_dis ) );

                

                if(distance_encourage_value < 0)
                    continue;

                for(INDEX object_node_idx : Node_space_record_list[retrieval_idx])
                {
                    General_Node& object_node = General_Node_Storage[object_node_idx];

                    Node_Image_Single_Attribute node_image_attribute = read_a_Node_Image_Attribute(object_node_idx);

                    char observe_size = node_image_attribute.observe_size;
                    int true_x_dis = (node_image_attribute.x * observe_size) - x;
                    int true_y_dis = (node_image_attribute.y * observe_size) - y;

                    int model_encourage_value = object_node.node_attention + distance_encourage_value
                        + object_node.rough_neuro_spare_size;

                    Value_Sort_Unit model_unit;
                    model_unit.value = model_encourage_value;
                    model_unit.target_idx = object_node_idx;
                    chose_object_sort.insert_a_unit(model_unit);
                }
                retrieval_idx += 1;
            }
            retrieval_idx += width_reduce_middle;
        }


        while ( curr_node.model_require > 0)
        {
            INDEX highist_one_idx = get_a_highist_value_of_sort(chose_object_sort);
            Value_Sort_Unit highist_one = chose_object_sort.list[highist_one_idx];
            INDEX object_node_idx = highist_one.target_idx;
            General_Node object_node = read_a_General_Node(object_node_idx);
            INDEX object_node_id = object_node.self_id;
            Node_Image_Single_Attribute object_node_image_att = read_a_Node_Image_Attribute(object_node_idx);
            image_object_locate_info locate_info = image_object_locate(curr_node_idx, object_node_idx);

            Attribute_Head attribute_head;
            vector<Neuro_Union_Attribute> general_condition_result_list;

            attribute_head.attribute_kind = 1;
            attribute_head.condition_num += 1;
            
            Neuro_Image_Item image_condition;
            image_condition.direction_scale = 3;
            image_condition.direction = locate_info.direction;
            image_condition.distance_scale = 3;
            image_condition.distance = locate_info.distance;
            image_condition.target_id = object_node_id;
            
            Neuro_Union_Attribute condition = {.image_item = image_condition};
            general_condition_result_list.push_back(condition);
            Simple_Neuro_Attribute_write(curr_node_idx, 0, 1, attribute_head,
                general_condition_result_list);

            require_object.require_value -= highist_one.value;
        }
    }


    // INDEX curr_block_idx ; char rough_view_idx;
    // {
    //     Image_Scene& curr_image_scene = Image_Scene_Storage[image_scene_idx];
    //     int height = curr_image_scene.height;
    //     int width = curr_image_scene.width;

    //     Image_Colour_Block& curr_block = curr_image_scene.Image_rough_view[rough_view_idx].Colour_block_list[curr_block_idx];
    //     COLOUR_BLOCK_ENTER_SORT[curr_block.ave_r*8*8 + curr_block.ave_g*8 + curr_block.ave_b*8];

    //     ;
    // }


}

void Image_Cognition_Calculate(  INDEX curr_scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Image_Scene& curr_image_scene = Image_Scene_Storage[curr_scene_idx];
    short width = curr_image_scene.width;
    short height = curr_image_scene.height;
    vector<Link_Variable_Attribute>& Require_object_list= curr_image_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_image_scene.Require_object_sort;
    vector< vector<int> >& Retrieve_require_list = curr_image_scene.Retrieve_total_require_list;
    vector< vector<int> >& Model_require_list = curr_image_scene.Model_total_require_list;
    vector< vector<int> >& Construct_require_list = curr_image_scene.Construct_total_require_list;

    vector<Wait_Active_Neuro> wait_active_list;
    vector<INDEX> free_wait_active_idx;


    if(curr_image_scene.have_init != 1)
    {
        if(Occupy_Single_Image_Scene[curr_scene_idx] == 1)
            return;

        Occupy_Single_Image_Scene[curr_scene_idx] += 1;

        if(curr_image_scene.last_map != 0)
            image_nature_attention_generate(curr_image_scene.last_map, curr_scene_idx);
        else image_nature_attention_generate(curr_scene_idx, curr_scene_idx);

        image_rough_map_generate(curr_scene_idx, 2, 4);
        image_rough_map_generate(curr_scene_idx, 4, 16);

        init_require_vector(curr_scene_idx, Retrieve_require_list);
        init_require_vector(curr_scene_idx, Model_require_list);
        init_require_vector(curr_scene_idx, Construct_require_list, 0);

        image_attention_appraise(curr_image_scene.last_map, curr_scene_idx);

        curr_image_scene.have_init = 1;
        Occupy_Single_Image_Scene[curr_scene_idx] -= 1;
    }

    vector<INDEX> motion_branch_predict;

    while(elapsed_ms < invest_time && Occupy_Single_Image_Scene[curr_scene_idx] == 0)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 1:
        {
            INDEX rough_idx;
            INDEX target_idx = region_require_object_select(Retrieve_require_list);

            if(rough_idx == 0)
            {
                A_Space_Base_Compontent_Detect(target_idx, curr_scene_idx);

            } else {
                A_Neuro_Cognition_Generate(target_idx, curr_scene_idx, 1);
            }
            
        }
        break;
        case 2:
        {
            INDEX target_node_idx = region_require_object_select( Model_require_list);
            Image_A_Neuro_Space_Model(curr_scene_idx, target_node_idx);
        }
        }
        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

}

void Image_Scene_Simulate(INDEX curr_scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;
    
    Image_Scene& curr_image_scene = Image_Scene_Storage[curr_scene_idx];
    unordered_map< ID, vector<Focus_Object> >& Id_find_focus_object = curr_image_scene.Id_find_focus_object;
    vector<INDEX>& Image_overall_node_list = curr_image_scene.Overall_image_node_list;
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = curr_image_scene.Id_find_overall_node;
    vector<Link_Variable_Attribute>& Require_object_list = curr_image_scene.Require_object_list;
    
    Advanced_Value_Sort& Require_object_sort = curr_image_scene.Require_object_sort;
    vector<Link_Variable_Attribute>& Scene_variable_attribute_List = curr_image_scene.Scene_variable_attribute_List;

    int root_require;

    int allocation_require;

    vector<INDEX> seed_scene_idx_list;

    Generate_Permit simulate_permit;


    while(elapsed_ms < invest_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 1:
        {
            A_Neuro_Simulate_Generate(task_chose.require_id_or_idx, curr_scene_idx, 2, simulate_permit);
        }
        break;
        case 2:
        {
            INDEX appraise_idx = task_chose.require_id_or_idx;
            Multiple_Probability_Cohesion_Appraise(appraise_idx);
        }
        break;
        case 3:
        {
            INDEX node_idx = task_chose.require_id_or_idx;
            Node_Image_Single_Attribute image_att = read_a_Node_Image_Attribute(node_idx);

            INDEX scene_idx;

            push_back_a_node_to_scene(scene_idx, node_idx, 3);
        }
        break;
        case 4:
        {
            Image_Scene seed_image_scene;
            seed_image_scene.Local_image_node_list;
            INDEX add_simu_idx = create_a_Image_Scene(seed_image_scene);

            Link_Scene_Attribute link_scene;
            link_scene.scene_kind == 3;
            link_scene.related_value;
            link_scene.scene_idx = add_simu_idx;
            Link_Variable_Attribute va = {.link_scene_attribute = link_scene};
            Scene_variable_attribute_List.push_back(va);
        }
        break;
        case 5:
        {
            char invest_time = task_chose.require_value / curr_image_scene.sum_require_value + 1;

            General_Scene_Simulate(task_chose.require_id_or_idx, invest_time);
        }
        }

        
        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
}



void Action_A_Neuro_Model(INDEX curr_scene_idx, INDEX node_idx)
{
    General_Node action_node = General_Node_Storage[node_idx];

    Node_Base_Action_Attribute action_attribute = read_a_Node_Action_Attribute(node_idx);
    Node_Image_Single_Attribute effect_space_attribute = read_a_Node_Image_Attribute(node_idx);

    Attribute_Head attribute_head;
    vector<Neuro_Union_Attribute> general_condition_result_list;

    attribute_head.attribute_kind = 1;
    
    Neuro_Action_Item condition_front_action;
    condition_front_action.action_kind = action_attribute.action_kind;
    condition_front_action.dwFlags = action_attribute.dwFlags;
    condition_front_action.mouseData = action_attribute.mouseData;
    condition_front_action.vk = action_attribute.vk;

    vector<INDEX> possible_list;

    for(int a = 0; a < possible_list.size() ; a++)
    {
        INDEX be_effect_object_idx = possible_list[a];
        Node_Image_Single_Attribute be_effect_node_image_attribute = read_a_Node_Image_Attribute(be_effect_object_idx);

        General_Node be_effect_node = General_Node_Storage[be_effect_object_idx];

        char direction;
        char distance;
        get_two_point_direction_distance(be_effect_node_image_attribute.x, be_effect_node_image_attribute.y,
            action_attribute.x, action_attribute.y, direction, distance);
        
        condition_front_action.direction = direction;
        condition_front_action.distance = distance;
        condition_front_action.target_id = be_effect_node.self_id;

        Neuro_Union_Attribute condition_input = {.action_item = condition_front_action};
        general_condition_result_list.push_back(condition_input);
        attribute_head.condition_num += 1;

        INDEX result_object_idx;
        Node_Image_Single_Attribute result_node_image_attribute = read_a_Node_Image_Attribute(result_object_idx);

        get_two_point_direction_distance(be_effect_node_image_attribute.x, be_effect_node_image_attribute.y,
            result_node_image_attribute.x, result_node_image_attribute.y, direction, distance);
        
        Neuro_Time_Item condition_back_time;
        condition_back_time.time_left_range;
        condition_back_time.time_right_range;
        condition_back_time.time_scale = 8;

        Neuro_Image_Item condition_back_image;
        condition_back_image.direction = direction;
        condition_back_image.distance = distance;
        condition_back_image.target_id;

        condition_input = {.time_item = condition_back_time};
        general_condition_result_list.push_back(condition_input);
        condition_input = {.image_item = condition_back_image};
        general_condition_result_list.push_back(condition_input);
        attribute_head.condition_num += 2;

        Simple_Neuro_Attribute_write(be_effect_node.self_id, 0, 1, attribute_head,
            general_condition_result_list);
    }


}

void Image_A_Neuro_Time_Model(INDEX curr_scene_idx, INDEX curr_node_idx)
{
    ;
}

void Time_A_Neuro_Self_Model(INDEX scene_idx, INDEX curr_node_idx)
{
    A_Neuro_Basic_Evolve(curr_node_idx, 3);

    General_Node& curr_node = General_Node_Storage[curr_node_idx];
    vector<Link_Variable_Attribute>& Node_variable_attribute_list = curr_node.Node_variable_attribute_list;
    vector<Node_Image_Single_Attribute> object_node_image_att_list;
    vector<Node_Time_Single_Attribute> object_node_time_att_list;

    int i = 0;
    while( i < Node_variable_attribute_list.size() )
    {
        Link_Node_Attribute link_node_att = Node_variable_attribute_list[i].link_node_attribute;
        if(link_node_att.is_node_link != 3 && link_node_att.link_Kind != 1 && link_node_att.node_link != 1)
            continue;

        Node_Image_Single_Attribute object_node_image_att = read_a_Node_Image_Attribute(link_node_att.link_idx_or_id);
        Node_Time_Single_Attribute object_node_time_att = read_a_Node_Time_Attribute(link_node_att.link_idx_or_id);
        object_node_image_att_list.push_back(object_node_image_att);
        object_node_time_att_list.push_back(object_node_time_att);
    }

    long long compare_time = object_node_time_att_list[0].time;

    for(int a = 1; a < object_node_time_att_list.size(); a++)
    {
        if(object_node_time_att_list[a].time < compare_time)
        {
            while(object_node_time_att_list[a].time < compare_time)
            {
                compare_time = object_node_time_att_list[a].time;
            }

            object_node_image_att_list;
        }
    }

    Attribute_Head attribute_head;
    vector<Neuro_Union_Attribute> general_condition_result_list;

    int sum_x = 0;
    int sum_y = 0;

    int sum_plus_x = 0;
    int sum_minus_x = 0;
    int sum_plus_y = 0;
    int sum_minus_y = 0;

    Node_Image_Single_Attribute last_node_image_att;

    for(int b = 1; b < object_node_image_att_list.size(); b++ )
    {
        int move_x = (object_node_image_att_list[b].x + last_node_image_att.x);
        int move_y = (object_node_image_att_list[b].y + last_node_image_att.y);

        if(move_x > 0)
        {
            sum_plus_x += move_x;
        } else {
            sum_minus_x += move_x;
        }

        if(move_y > 0)
        {
            sum_plus_y += move_y;
        } else {
            sum_minus_y += move_y;
        }

        sum_x += move_x;
        sum_y += move_y;
        
        last_node_image_att = object_node_image_att_list[b];

        Simple_Neuro_Attribute_write(curr_node.self_id, 0, 1, attribute_head,
            general_condition_result_list);
    }

    ;

}

void Time_A_Neuro_Model(INDEX scene_idx, INDEX curr_node_idx)
{
    General_Node& curr_node = General_Node_Storage[curr_node_idx];
    A_Neuro_Basic_Evolve(curr_node_idx, 3);

    Time_Scene& curr_time_scene = Time_Scene_Storage[scene_idx];
    vector<Advanced_Value_Sort>& Single_Time_attention_sort = curr_time_scene.Single_time_attention_sort;
    vector<Link_Variable_Attribute>& Require_object_list = curr_time_scene.Require_object_list;
    vector<INDEX>& Overall_time_node_list = curr_time_scene.Overall_time_node_list;
    unordered_map<int, INDEX>& Time_to_single_time_idx = curr_time_scene.Time_to_single_time_idx;

    Node_Time_Single_Attribute origin_node_time_att = read_a_Node_Time_Attribute(curr_node_idx);
    vector<Link_Variable_Attribute>& Node_variable_attribute_list = curr_node.Node_variable_attribute_list;
    
    Require_Object require_object ;

    unordered_set<INDEX> have_link_idx;
    Advanced_Value_Sort chose_object_sort;
    
    int i = 0;
    while( i < Node_variable_attribute_list.size() )
    {
        Link_Node_Attribute link_ = Node_variable_attribute_list[i].link_node_attribute;
        if(link_.is_node_link != 3)
            continue;

        i++;

        Neuro_Time_Item time_item = Node_variable_attribute_list[i].neuro_time_item;

        if(time_item.is_neuro_item != 1 && time_item.item_is_time != 3)
            continue;
        
        have_link_idx.insert(time_item.target_id);
    }


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

        int begin_time = origin_node_time_att.time + left_time;
        int end_time = origin_node_time_att.time + right_time;

        if(begin_time < curr_time_scene.begin_time)
            begin_time = curr_time_scene.begin_time;

        if(end_time < curr_time_scene.end_time)
            end_time = curr_time_scene.end_time;

        for(int q = begin_time; q <= end_time; q++)
        {
            INDEX sort_idx = Time_to_single_time_idx[q];
            Advanced_Value_Sort& curr_value_sort = Single_Time_attention_sort[sort_idx];
            
            INDEX unit_idx = get_a_highist_value_of_sort(curr_value_sort);
            Value_Sort_Unit curr_value_unit = curr_value_sort.list[unit_idx];

            while(curr_value_unit.lower_one != 0)
            {
                General_Node& object_node = General_Node_Storage[curr_value_unit.target_idx];

                // if(object_node)
                //     ;

                if(object_node.rough_neuro_spare_size > 10)
                {
                    int model_encourage_value = object_node.time_predict_stability;

                    Value_Sort_Unit model_object;
                    model_object.value = model_encourage_value;
                    model_object.target_idx = curr_value_unit.target_idx;

                    chose_object_sort.insert_a_unit(model_object);

                    curr_value_unit = curr_value_sort.list[curr_value_unit.lower_one];
                }

            }
        }


        while(require_object.require_value > 0)
        {
            INDEX highist_one_idx = get_a_highist_value_of_sort(chose_object_sort);
            Value_Sort_Unit highist_one = chose_object_sort.list[highist_one_idx];
            INDEX node_idx = highist_one.target_idx;
            
            General_Node& chose_model_node = General_Node_Storage[node_idx];
            Node_Time_Single_Attribute object_node_time_att = read_a_Node_Time_Attribute(node_idx);
            
            Attribute_Head attribute_head;
            vector<Neuro_Union_Attribute> general_condition_result_list;

            Neuro_Time_Item time_condition;
            time_condition.time_scale = 8;
            time_condition.time_left_range = (object_node_time_att.time - origin_node_time_att.time) / time_condition.time_scale;
            time_condition.time_right_range = time_condition.time_left_range + 1;
            time_condition.target_id = chose_model_node.self_id;

            Neuro_Union_Attribute condition = {.time_item = time_condition};
            general_condition_result_list.push_back(condition);
            Simple_Neuro_Attribute_write(curr_node_idx, 0, 1, attribute_head,
                general_condition_result_list);

            require_object.require_value -= highist_one.value;
        }
    }

}

void Time_Cognition_Calculate(INDEX curr_scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Time_Scene& curr_time_scene = Time_Scene_Storage[curr_scene_idx];
    Advanced_Value_Sort& Require_object_sort = curr_time_scene.Require_object_sort;
    vector<Link_Variable_Attribute>& Require_object_list = curr_time_scene.Require_object_list;

    vector<INDEX> generate_overall_node_list;
    generate_overall_node_list.reserve(32);

    INDEX time_node_idx;

    while(elapsed_ms < invest_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            generate_overall_node_list = A_Neuro_Cognition_Generate(task_chose.require_id_or_idx, curr_scene_idx, 3);
        }
        break;
        case 1:
        {
            INDEX curr_time_node_idx = task_chose.require_id_or_idx;
            General_Node& curr_time_node = General_Node_Storage[curr_time_node_idx];
            
            ID constituent_id;
            INDEX newist_image_idx;
            INDEX newist_node_idx;
            get_a_category_node_newist_constituent(curr_time_node_idx, constituent_id, newist_image_idx);
            
            Image_Scene& last_appear_scnce = Image_Scene_Storage[newist_image_idx];
            INDEX next_check_image_idx = last_appear_scnce.next_map;
            Image_Scene& check_image_scnce = Image_Scene_Storage[newist_image_idx];
            Node_Image_Single_Attribute compare_node_image_att = read_a_Node_Image_Attribute(newist_node_idx);

            Link_Variable_Attribute va = read_a_category_node_condition(curr_time_node_idx);
            Node_Image_Single_Attribute category_condition = va.node_image_single_attribute;

            if(check_image_scnce.Id_find_overall_node.count(constituent_id))
            {
                vector<INDEX>& candidate_list = check_image_scnce.Id_find_overall_node[constituent_id];
                
                int a = 0;
                for(; a < candidate_list.size(); a++)
                {
                    INDEX candidate_idx = candidate_list[a];

                    Node_Image_Single_Attribute candidate_attribute =
                        read_a_Node_Image_Attribute(candidate_idx);

                    if( abs(candidate_attribute.x - compare_node_image_att.x ) > category_condition.x
                        && abs(candidate_attribute.y - compare_node_image_att.y ) > category_condition.y )
                    {
                        General_Node choice_node = read_a_General_Node(candidate_idx);

                        Link_Node_Attribute link_node_attribute;
                        link_node_attribute.link_idx_or_id = candidate_idx;
                        link_node_attribute.link_Kind = 1;
                        Link_Variable_Attribute lva = {.link_node_attribute = link_node_attribute};
                        push_back_a_link_to_Node( curr_time_node_idx, lva);

                        link_node_attribute.link_idx_or_id = curr_time_node_idx;
                        link_node_attribute.link_Kind = 2;
                        lva = {.link_node_attribute = link_node_attribute};
                        push_back_a_link_to_Node( candidate_idx, lva);
                    }

                }

                if(a == candidate_list.size())
                {
                    ;
                }

            } else {
                ;
            }
            
        }
        break;
        case 2:
        {
            Time_A_Neuro_Model(curr_scene_idx, task_chose.require_id_or_idx);
        }
        break;
        case 3:
        {
            Action_A_Neuro_Model(curr_scene_idx, task_chose.require_id_or_idx);
        }
        }

        auto end = chrono::steady_clock::now();
        elapsed_ms= chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    }
}

void Time_Scene_Simulate(INDEX curr_scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;
    
    Time_Scene& curr_time_scene = Time_Scene_Storage[curr_scene_idx];
    vector<Link_Variable_Attribute>& Require_object_list = curr_time_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_time_scene.Require_object_sort;
    vector< vector<int> >& Require_object_weights_map = curr_time_scene.Require_object_weights_map;
    vector<Link_Variable_Attribute>& Scene_variable_attribute_list = curr_time_scene.Scene_variable_attribute_list;
    unordered_map<ID, vector<Focus_Object> >& Id_find_focus_object = curr_time_scene.Id_find_focus_object;
    vector<General_Node>& Local_time_node_list = curr_time_scene.Local_time_node_list;


    vector<INDEX> process_element;
    
    Generate_Permit simulate_permit;
    simulate_permit.condition_time_permit = 1;
    simulate_permit.result_time_permit = 1;
    simulate_permit.condition_number_permit = 1;
    simulate_permit.result_number_permit = 1;

    vector<INDEX> wait_classify_node;
    
    while(elapsed_ms < invest_time)
    {
        INDEX task_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[task_idx].require_object;

        switch(task_chose.require_kind)
        {
        case 1:
        {
            A_Neuro_Simulate_Generate(task_chose.require_id_or_idx, curr_scene_idx, 3, simulate_permit);
        }
        break;
        case 10:
        {
            vector<INDEX> same_position_node_list = curr_time_scene.Id_find_overall_node[task_chose.require_id_or_idx];
            Node_Time_Single_Attribute time_att = read_a_Node_Time_Attribute(task_chose.require_id_or_idx);
        }
        break;
        case 2:
        {
            Multiple_Probability_Cohesion_Appraise(task_chose.require_id_or_idx);
        }
        break;
        case 3:
        {
            Node_Time_Single_Attribute time_att = read_a_Node_Time_Attribute(task_chose.require_id_or_idx);

            for(int a = 0; a < Scene_variable_attribute_list.size(); a++)
            {
                Link_Scene_Attribute link_scene = Scene_variable_attribute_list[a].link_scene_attribute;
                if(link_scene.is_scene_link != 6)
                    continue;

                INDEX scene_idx = link_scene.scene_idx;

                a++;

                Scene_Time_Attribute scene_time_attribute = Scene_variable_attribute_list[a].scene_time_attribute;
                if(scene_time_attribute.is_scene_attribute != 5 && scene_time_attribute.self_is_Scene_Time_Attribute != 3)
                    continue;

                Time_Scene& time_scene = Time_Scene_Storage[scene_idx];
                

                if(0)
                {
                    push_back_a_node_to_scene(scene_idx, task_chose.require_id_or_idx, 3);
                }
            }
            
        }
        break;
        case 4:
        {
            Time_Scene& time_scene = Time_Scene_Storage[task_chose.require_id_or_idx];
            Scene_Time_Attribute scene_time_att = read_a_Scene_Time_Attribute(task_chose.require_id_or_idx);

            vector<INDEX> node_list;
            for(int a = 0; a < node_list.size(); a++)
            {
                INDEX node_idx;

                Node_Time_Single_Attribute node_time_att = read_a_Node_Time_Attribute(node_idx);

                scene_time_att; node_time_att;

                if(0)
                {
                    push_back_a_node_to_scene(task_chose.require_id_or_idx, node_idx, 3);
                }
            }

        }
        break;
        case 5:
        {
            Time_Scene seed_time_scene;
            seed_time_scene.self_scene_attribute;
            seed_time_scene.Overall_time_node_list.push_back(task_chose.require_id_or_idx);
            INDEX add_simu_idx = create_a_Time_Scene(seed_time_scene);

            Require_Object require_object;
            require_object.require_object_kind = 2;
            require_object.require_id_or_idx = task_chose.require_id_or_idx;
            Link_Variable_Attribute lva = {.require_object = require_object};
            Scene_variable_attribute_list.push_back(lva);

            Link_Scene_Attribute link_scene;
            link_scene.scene_kind = 3;
            link_scene.related_value;
            link_scene.scene_idx = add_simu_idx;
            lva = {.link_scene_attribute = link_scene};
            Scene_variable_attribute_list.push_back(lva);

            int require_value = task_chose.require_value;
            task_chose.require_value = 0;

        }
        break;
        case 6:
        {
            char invest_time = task_chose.require_value / curr_time_scene.sum_require_value + 1;

            Time_Scene_Simulate(task_chose.require_id_or_idx, invest_time);
        }
        break;
        case 7:
        {
            char invest_time = task_chose.require_value / curr_time_scene.sum_require_value + 1;

            Image_Scene_Simulate(task_chose.require_id_or_idx, invest_time);
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



inline void Link_Path_Detect_Moudle(INDEX check_inside_node, INDEX scene_idx,
    vector<General_Node>& inside_node, unordered_map<ID, INDEX>& id_to_inside_detect_node)
{
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
        link.link_value_I = probability;

        Attribute_Head attribute_head = attribute_head_read(ptr);

        char result_kind;
        char result_num = attribute_head.result_num + attribute_head.condition_num;
        char result_size = 0;

        ptr += 3;

        int a = 0;
        while(a < result_num)
        {
            return_item_kind_and_size(ptr, result_kind, result_size);
            
            switch(result_kind)
            {
            case 1:
            {
                Neuro_Image_Item image_item = *(Neuro_Image_Item *)(ptr);
                generate_id = image_item.target_id;
            }
            break;
            case 2:
            {
                ;
            }
            break;
            case 3:
            {
                Neuro_Time_Item time_item = *(Neuro_Time_Item *)(ptr);
                generate_id = time_item.target_id;
            }
            break;
            case 4:
            {
                Neuro_Text_Item text_item = *(Neuro_Text_Item *)(ptr);
                generate_id = text_item.target_id;
            }
            break;
            case 5:
            {
                Neuro_Concept_Item concept_item = *(Neuro_Concept_Item *)(ptr);
                generate_id = concept_item.target_id;
            }
            break;
            case 6:
            {
                ;
            }
            break;
            case 7:
            {
                Neuro_Belief_Item belief_condition = *(Neuro_Belief_Item *)(ptr);
                generate_id = belief_condition.target_id;
            }
            break;
            case 8:
            {
                ;
            }
            }
            

            if( id_to_inside_detect_node.count(generate_id) )
            {
                General_Node& check_node = inside_node[generate_id];

                if( check_node.component != base_connection);
                {
                    check_node.component = 3;

                    INDEX node_idx = id_to_inside_detect_node[generate_id];
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
        
        
    }

}



void Text_Neuro_Space_Model(
    INDEX text_network_idx, INDEX node_idx)
{
    Text_Scene& text_network = Text_Scene_Storage[text_network_idx];
    vector<INDEX> Text_node_storage_list = text_network.Overall_text_node_list;
    vector< vector<INDEX> >Text_node_space_storage_list = text_network.Space_form_record[0].record_list;

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
        Node_Text_Single_Attribute self_text_attribute = read_a_Node_Text_Attribute(text_node_idx);

        vector<Link_Variable_Attribute>& Variable_list = curr_node.Node_variable_attribute_list;

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
                    - i + curr_node.rough_neuro_spare_size;

                if(i < 0)
                    need_add_stat *= forward_predict_stability;
                else if(i > 0)
                    need_add_stat *= backward_predict_stability;
                
                if(need_add_stat > 0)
                {
                    Attribute_Head item_head;
                    vector<Neuro_Union_Attribute> general_condition_result_list;

                    Neuro_Text_Item text_item;
                    text_item.left_distance = 1;
                    text_item.distance_scale = 4;
                    text_item.target_id = object_node.self_id;;

                    Neuro_Union_Attribute condition = {.text_item = text_item};
                    general_condition_result_list.push_back(condition);

                    item_head.attribute_kind = 1;
                    item_head.condition_num += 1;
                    
                    Simple_Neuro_Attribute_write(curr_node.self_id, 0, 1,
                        item_head, general_condition_result_list);

                    Link_Node_Attribute la;
                    la.link_Kind = 4;
                    la.link_idx_or_id = idx;

                    Link_Variable_Attribute flex = {.link_node_attribute = la};
                    curr_node.Node_variable_attribute_list.push_back(flex);
                }
            }
        }
    }
}


void Text_Neuro_Identity_Model(
    INDEX text_scene_idx, INDEX node_idx)
{
    Text_Scene& text_scene = Text_Scene_Storage[text_scene_idx];
    vector<INDEX>& Overall_text_node_list = text_scene.Overall_text_node_list;
    vector< vector<INDEX> >& Text_node_space_from_record_list = text_scene.Space_form_record[0].record_list;

    General_Node& curr_node = General_Node_Storage[node_idx];
    vector<Link_Variable_Attribute>& Node_variable_attribute_list= curr_node.Node_variable_attribute_list;
    ID id = curr_node.self_id;

    unordered_set< vector<ID>, VectorUint32_tHash, VectorUint32_tEq > Node_combo_find_repeat 
        = text_scene.Node_combo_find_repeat;

    vector<INDEX> identity_list;
    unordered_set<INDEX> identity_find_repeat;
    
    vector<INDEX> possible_identity_object;

    int* ptr = node_find(id);

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    int* first_ptr = ptr;

    for(short fea = 0; fea <  neuro_head.used_item_num; )
    {
        Attribute_Head attribute_head = attribute_head_read(ptr);

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
                first_ptr += result_kind;

                for( ; q < result_num; q++)
                {
                    return_item_kind_and_size(ptr, result_kind, result_size);
                    first_ptr += result_kind;
                }
                
                ptr = first_ptr;
                break;
            }

            Neuro_Belief_Item belief_item = *(Neuro_Belief_Item*)(ptr);

            if(belief_item.detail_kind != 1)
            {
                for( ; q < result_num; q++)
                {
                    return_item_kind_and_size(ptr, result_kind, result_size);
                    first_ptr += result_kind;
                }
                
                ptr = first_ptr;
                break;
            }

            identity_find_repeat.insert(belief_item.target_id);
        }
    }


    int i = 0;
    while( i < Node_variable_attribute_list.size() )
    {
        if(Node_variable_attribute_list[i].require_object.is_require != 4)
            continue;
        
        Require_Object& require_object = Node_variable_attribute_list[i].require_object;

        if(require_object.require_kind != 20)
            continue;

        i++;

        Neuro_Time_Item time_require = Node_variable_attribute_list[i].neuro_time_item;

        if(time_require.is_neuro_item == 1 && time_require.item_is_time == 3)
        {
            // int left_time = text_require.time_left_range * text_require.time_scale;
            // int right_time = text_require.time_right_range * text_require.time_scale;

            // int begin_time = curr_node_time - left_time*100;
            // int end_time = curr_node_time - right_time*100;

            // if(begin_time < curr_time_scene.begin_time)
            //     begin_time = curr_time_scene.begin_time;

            // if(end_time < curr_time_scene.end_time)
            //     end_time = curr_time_scene.end_time;

            // for(long long q = begin_time ; q <= end_time; q+=100 )
            // {
            //     INDEX sort_idx = Time_to_single_time_idx[q];
            //     Advanced_Value_Sort& curr_value_sort = Single_Time_attention_sort[sort_idx];
                
            //     INDEX unit_idx = get_a_highist_value_of_sort(curr_value_sort);
            //     Value_Sort_Unit curr_value_unit[unit_idx];

            //     while(curr_value_unit.lower_one != 0)
            //     {
            //         General_Node& object_node = General_Node_Storage[node_idx];

            //         if(chose_model_node.neuro_spare_size > 10)
            //         {
            //             int model_encourage_value = chose_model_node.time_predict_stability;

            //             Value_Sort_Unit model_object;
            //             model_object.value = model_encourage_value;
            //             model_object.target_idx = node_idx;

            //             chose_object_sort.insert_a_unit(model_object);

            //             curr_value_unit = curr_value_sort.list[curr_value_unit.lower_one];
            //         }

            //     }
            // }

            // for(int a = 0; a < Attention_List.size(); a++)
            // {
            //     Require_Object& attention_object = Require_Object_List[a].require_object;
            //     attention_object.require_value;
            // }

            // vector<ID> possible_identity_object;
            // vector<Neuro_Link_Attribute> possible_identity_relation;

        
            Attribute_Head item_head;
            vector<Neuro_Union_Attribute> general_condition_result_list;
            
            item_head.attribute_kind == 2;

            for(ID b : possible_identity_object)
            {
                item_head.result_num += 1;

                Neuro_Belief_Item identity_belief;
                identity_belief.target_id = possible_identity_object[b];
                identity_belief.detail_kind == 1;
                identity_belief.logic == 1;
                Neuro_Union_Attribute GR = {.belief_item = identity_belief};

                general_condition_result_list.push_back(GR);
            }

            Simple_Neuro_Attribute_write(curr_node.self_id, 0, 1,
                item_head, general_condition_result_list);
            
        } else if(time_require.is_neuro_item == 1 && time_require.item_is_time == 1)
        {
            Neuro_Image_Item image_require = Node_variable_attribute_list[i].neuro_image_item;
        }

        Advanced_Value_Sort chose_object_sort;

        while( require_object.require_value > 0 )
        {
            INDEX node_idx = get_a_highist_value_of_sort(chose_object_sort);

            General_Node& chose_model_node = General_Node_Storage[node_idx];

            Neuro_Belief_Item belief_condition;
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
    belief_attribute.target_id;
    belief_attribute.detail_kind = 1;

    vector<Link_Variable_Attribute> link_list;

    Find_Common_Relation_Rate(link_list);

    

}


inline void init_text_require(vector< vector<int> >& retrieve_require,
    unsigned short text_size, char unit_require_value)
{
    int need_size = text_size;
    int need_layer = 0;

    while(need_size >= 8)
    {
        need_size /= 8;
        need_layer += 1;
    }

    retrieve_require.resize(need_layer);

    need_size = text_size / 8;
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


inline void text_network_sequence_organise(
    INDEX text_scene_idx)
{
    vector<INDEX> formation_node_list;

    for(int a = 0; a < formation_node_list.size(); a++)
    {
        INDEX idx = formation_node_list[a];
        General_Node node = read_a_General_Node(idx);

        Node_Sequence_Info node_sequence_info = read_a_node_sequence_info(idx);

        int reward_I;
        int reward_II;

        auto Node_variable_attribute_list = node.Node_variable_attribute_list;

        unordered_map< INDEX,int > id_find_reward;
        
        //
        Advanced_Value_Sort node_important_sort_list;

        for(Link_Variable_Attribute attribute : Node_variable_attribute_list)
        {
            if(attribute.neuro_text_item.is_neuro_item != 1)
                continue;
            
            Neuro_Text_Item simple_text;
            Neuro_Number_Item simple_number;

            Value_Sort_Unit input_unit;
            
            if( id_find_reward.count(simple_text.target_id) )
            {
                int& reward = id_find_reward[simple_text.target_id];
                Value_Sort_Unit find_unit;
                find_unit.target_idx = simple_text.target_id;
                find_unit.value = reward;

                input_unit.value = reward;
                node_important_sort_list.change_a_unit( find_unit, input_unit);
                reward += simple_number.number_I;

            } else {
                id_find_reward[simple_text.target_id] = simple_number.number_I;

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
        
    }
}


inline void text_a_node_sequence_organise(
    INDEX text_node_idx, INDEX text_scene_idx)
{
    ;
}

void Text_Cognition_Calculate(INDEX curr_text_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Text_Scene& curr_text_network = Text_Scene_Storage[curr_text_idx];
    vector< vector<INDEX> >& Text_node_space_storage_list = curr_text_network.Space_form_record[0].record_list;
    unsigned short original_text_size = curr_text_network.original_text_size;
    vector<Link_Variable_Attribute>& Require_object_list = curr_text_network.Require_object_list;
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
        Link_Variable_Attribute va = {.require_object = retrieve_require};
        Require_object_list.push_back(va);

        Require_Object model_require;
        model_require.require_kind = 2;
        model_require.require_value = original_text_size*10;
        model_require.require_detail_kind = 0;
        va = {.require_object = model_require};
        Require_object_list.push_back(va);
    }

    vector<INDEX> generate_formation_node_list;

    while(elapsed_ms < invest_time || curr_text_network.sum_total_require == 0)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            //chose_object_idx = region_require_object_select(Retrieve_total_require_list);
            
            A_Neuro_Cognition_Generate(task_chose.require_id_or_idx, curr_text_idx, 2);
        }
        break;
        case 1:
        {
            INDEX chose_object_idx = region_require_object_select(Model_total_require_list);

            INDEX node_idx = Text_node_space_storage_list[chose_object_idx][0];

            A_Neuro_Basic_Evolve(node_idx, 2);

            Text_Neuro_Space_Model(curr_text_idx, task_chose.require_id_or_idx );
        }
        }
        
        auto end = chrono::steady_clock::now();
        elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
    
}

void Text_Scene_Simulate(INDEX curr_text_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Text_Scene& curr_text_network = Text_Scene_Storage[curr_text_idx];
    vector< vector<INDEX> >& Text_node_space_storage_list = curr_text_network.Space_form_record[0].record_list;
    vector<Link_Variable_Attribute>& Require_object_list = curr_text_network.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = curr_text_network.Require_object_sort;
    unordered_map< ID, vector<INDEX> >& Id_find_overall_node = curr_text_network.Id_find_overall_node;

    
    while(elapsed_ms < invest_time || curr_text_network.sum_total_require == 0)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch( task_chose.require_kind)
        {
        case 2:
        {
            A_Neuro_Simulate_Generate(task_chose.require_id_or_idx , curr_text_idx, 4);
        }
        break;
        case 30:
        {
            ;
        }
        case 3:
        {
            text_network_sequence_organise(curr_text_idx);
        }
        break;
        case 4:
        {
            Image_Scene& curr_image_scene = Image_Scene_Storage[task_chose.require_id_or_idx];

            char invest_time = task_chose.require_value / curr_image_scene.sum_require_value + 1;
            Text_Cognition_Calculate( task_chose.require_id_or_idx,invest_time );
        }
        }

        auto end = chrono::steady_clock::now();
        elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
}


struct Action_Chose_Unit
{
    int reward_value = 0;
    INDEX action_target = 0;
};

struct Action_Chose_Node
{
    INDEX last_action_idx = 0;
    int action_chooe_unit_num = 0;

    Action_Chose_Unit action_chose_sort[7];
};

struct Action_Chose_Network
{
    INDEX begin_action_idx = 0;
    INDEX end_action_idx = 0;
    INDEX curr_action_idx;
    vector<Action_Chose_Node> Action_Chose_Node_List;
    vector<INDEX> Free_Action_Chose_Node_Index;
};


void Action_Calculate_Execulate(INDEX scene_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    General_Scene& action_organise_scene = General_Scene_Storage[scene_idx];
    vector<Link_Variable_Attribute>& Require_object_list = action_organise_scene.Require_object_list;
    Advanced_Value_Sort& Require_object_sort = action_organise_scene.Require_object_sort;
    Link_Variable_Attribute link_scene = read_a_scene_related_motion_detect(scene_idx);
    INDEX action_connect_scene_idx = link_scene.link_scene_attribute.scene_idx;
    General_Scene& motion_connect_detect_scene = General_Scene_Storage[action_connect_scene_idx];
    unordered_map<ID, vector<INDEX> >& Id_find_overall_node = motion_connect_detect_scene.Id_find_overall_node;
    vector<INDEX>& General_overall_node_list = motion_connect_detect_scene.Overall_general_node_list;
    vector<General_Node>& General_local_node_list = motion_connect_detect_scene.Local_general_node_list;
    unordered_map<ID, vector<INDEX> >& Id_find_local_node = motion_connect_detect_scene.Id_find_local_node;


    unordered_map<ID, INDEX> Temporary_id_to_inside_node;

    int check_idx = 0;

    vector<Link_Node_Attribute> probability_record_vec;
    vector<INDEX> finally_probability_vec;
    
    Action_Chose_Network action_chose_network;

    while(elapsed_ms < invest_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 0:
        {
            Link_Path_Detect_Moudle(scene_idx, task_chose.require_id_or_idx, General_local_node_list,
                Temporary_id_to_inside_node);
        }
        break;
        case 1:
        {
            probability_record_vec.clear();
            finally_probability_vec.clear();

            char source_direction;
            INDEX source_node_idx = task_chose.require_id_or_idx;
            INDEX target_node_idx = read_a_node_action_connect_target(source_node_idx).link_idx_or_id;
            
            vector<INDEX> wait_check_vec;
            float allow_probability = 0.1;

            Link_Node_Attribute source_probability_record;
            source_probability_record.link_idx_or_id = source_node_idx;
            source_probability_record.link_value_I = 1;
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
                    probability_record.link_value_I = link.link_value_I * source_probability_record.link_value_I;
                    probability_record.link_value_II = choose_probability_idx;

                    if(probability_record.link_value_I < allow_probability)
                        continue;

                    probability_record_vec.push_back(probability_record);

                    if(link.link_value_I == target_node_idx)
                    {
                        finally_probability_vec.push_back(probability_record_vec.size() - 1);
                    }
                }
            }

        }
        break;
        case 2:
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

            while(curr_one.link_value_I != 0)
            {
                curr_one = probability_record_vec[curr_one.link_value_II];
            }

            int score = average_time*highist_one.link_value_I;
        }
        break;
        case 3:
        {
            vector<INDEX> select_path_element;

            Link_Node_Attribute curr_one;
            while(curr_one.link_value_I != 0)
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

                push_back_a_node_to_scene(gen_scene_idx, node_idx, 1);
            }


        }
        break;
        case 4:
        {
            INDEX plan_scene_idx = task_chose.require_id_or_idx;

            General_Scene& plan_scene = General_Scene_Storage[plan_scene_idx];

            for(INDEX path_node : plan_scene.Overall_general_node_list)
            {
                A_Neuro_Simulate_Generate(path_node, plan_scene_idx, 1);
            }
        }
        break;
        case 5:
        {
            ;
        }
        break;
        case 6:
        {
            INDEX action_scene_idx;
            General_Scene& The_action_scene = General_Scene_Storage[action_scene_idx];

            Node_Base_Action_Attribute node_action_attribute;

            The_action_scene.Scene_variable_attribute_List;

            Scene_Action_Attribute scene_action_attribute;
            long long time;
            int gap_time = CURRENT_MODEL_TIME - time;
            INDEX last_action_idx = scene_action_attribute.action_idx;
            

            vector< vector<INDEX> > action_branch_predict;
            vector<INDEX> reality_event;

            vector< INDEX > generate_identity_node;
            Neuro_Belief_Item belief_item;
            belief_item.detail_kind = 1;

            vector<float> quality_rate;
            
            vector<Require_Object> require_;
            for(int a = 0; a < require_.size(); a++)
            {
                Require_Object require_object;
                INDEX need_identity_node = require_object.require_id_or_idx;
                General_Node general_node = read_a_General_Node(need_identity_node);

                Time_Scene The_time_scene = Time_Scene_Storage[action_execulate_scene];

                if( The_time_scene.Id_find_overall_node.count(general_node.self_id) )
                {
                    vector<INDEX> time_node_list = The_time_scene.Id_find_overall_node[general_node.self_id];
                    INDEX time_node_idx = time_node_list[0];
                    Node_Time_Single_Attribute node_time_attribute = read_a_Node_Time_Attribute(time_node_idx);
                    The_time_scene;
                }

                Image_Scene The_image_scene = Image_Scene_Storage[action_execulate_scene];
                
                if(The_image_scene.Id_find_overall_node.count(general_node.self_id))
                {
                    vector<INDEX> image_node_list = The_image_scene.Id_find_overall_node[general_node.self_id];
                    INDEX image_node_idx = image_node_list[0];
                    Node_Image_Single_Attribute node_image_attribute = read_a_Node_Image_Attribute(image_node_idx);

                    Neuro_Belief_Item belief_attribute;
                    belief_attribute.detail_kind = 1;
                    belief_attribute.target_id = image_node_idx;

                    Link_Variable_Attribute variable_object = {.neuro_belief_item = belief_attribute};
                    push_back_a_link_to_Node( need_identity_node, variable_object);

                    belief_attribute.target_id = need_identity_node;
                    variable_object = {.neuro_belief_item = belief_attribute};
                    push_back_a_link_to_Node( image_node_idx, variable_object);

                } else {
                    Match_Generate match;
                }


            }

        }
        break;
        case 7:
        {
            Scene_Action_Attribute saa = read_a_Scene_Action_Attribute(scene_idx);
            saa.action_idx;

            action_execulate_scene;
            
            
            action_chose_network.Action_Chose_Node_List;


            Model_Output_Action chose_action;
            chose_action.action_kind;
            chose_action.action_order;
            chose_action.dwFlags;
            chose_action.mouseData;
            chose_action.vk;
            chose_action.x;
            chose_action.y;

            int action_weight;

            Action_Choose_List.push_back(chose_action);
            Action_Choose_Weights.push_back(action_weight);
        }
        }

        auto end = chrono::steady_clock::now();
        elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }


return;
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

        char item_kind;
        char item_size = 0;

        ptr += 16;
        short fea = 0;

        while( fea < neuro_head.used_item_num )
        {
            Attribute_Head attribute_head = attribute_head_read(ptr);
            ptr += 3;

            return_item_kind_and_size(ptr, item_kind, item_size);
            if(item_kind != 8)
            {
                for(int q = 0; q < attribute_head.condition_num + attribute_head.result_num; q++)
                {
                    return_item_kind_and_size(ptr, item_kind, item_size);
                    fea += item_size;
                }

                item_size = 0;

                break;
            }

            fea += item_size;

            Neuro_Manner_Item manner_item = *(Neuro_Manner_Item*)(ptr);

            if(manner_item.target_id == permit_stat_id)
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

            item_size = 0;
        }
    }

    return return_inforamation;
}


void Interface_Text_Construct_Calculate( INDEX display_idx, short invest_time)
{
    auto start = chrono::steady_clock::now();
    int elapsed_ms = 0;

    Gtk3_Text_Display& gtk_interface = Gtk3_Text_Display_Storage[display_idx];

    INDEX input_text_idx = gtk_interface.input_Text_Scene_Idx;
    Text_Scene& input_text_network = Text_Scene_Storage[input_text_idx];
    vector<Link_Variable_Attribute> Require_object_list= input_text_network.Require_object_list;
    Advanced_Value_Sort Require_object_sort = input_text_network.Require_object_sort;

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

    vector<INDEX> generate_formation_node_list;

    while(elapsed_ms < invest_time)
    {
        INDEX req_idx = get_a_highist_value_of_sort(Require_object_sort);
        Require_Object task_chose = Require_object_list[req_idx].require_object;

        switch (task_chose.require_kind)
        {
        case 1:
        {
            INDEX process_scene = gtk_interface.input_Text_Scene_Idx;

            A_Neuro_Cognition_Generate(task_chose.require_id_or_idx, input_text_idx, 2);
        }
        break;
        case 2:
        {
            unordered_map< ID, vector<INDEX> > id_find_idx;
            id_find_idx.reserve( generate_formation_node_list.size() );

            ID aim_object;
            
            A_Neuro_Simulate_Generate(aim_object, output_text_idx, 3);
        }
        break;
        case 3:
        {
            text_network_sequence_organise(output_text_idx);
        }
        break;
        case 4://界面功能1：解释文本态度属性
        {
            ID permit_stat_id;
            vector<ID> input_object;
            vector<float> number_or_rate;

            statistics_all_manner_attribute(
                permit_stat_id, input_object, number_or_rate);
        }
        break;
        case 6://界面功能2：文本对象演化回答
        {
            char invest_time = task_chose.require_value / gtk_interface.total_require_value + 1;

            General_Scene_Simulate(task_chose.require_id_or_idx, invest_time);
        }
        break;
        case 7://界面功能3：文本知识建模
        {
            ;
        }
        break;
        case 8://界面功能4：文本转化外部追求
        {
            char invest_time = task_chose.require_value / gtk_interface.total_require_value + 1;

            Action_Calculate_Execulate(task_chose.require_id_or_idx, invest_time);
        }
        break;
        case 9://界面功能5：采取对待态度
        {
            gtk_interface.response_aim_kind = 5;

            char emotion_kind = gtk_interface.input_value_I[0];
            int emotion_value;

            Focus_Object focus_object;
            focus_object.kind = 2;

            Require_Object require_object;

            Require_Object_List;
            Id_Find_Focus_Object;
            
        }
        break;
        case 10://界面功能6：态度固定至神经元
        {
            ;
        }
        break;
        case 11://文本网络存储转化为输出文本
        {
            gtk_interface.Output_string = Network_To_CharVector(gtk_interface.output_Text_Scene_Idx);
        }
        }

        auto end = chrono::steady_clock::now();
        elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


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
                        vector<Link_Variable_Attribute>& Node_variable_attribute_list
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
                    Text_Cognition_Calculate(require_object.require_id_or_idx, invest_time);

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
                    Text_Scene_Simulate(require_object.require_id_or_idx, invest_time);

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