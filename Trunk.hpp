#include <algorithm>
#include <cmath>
#include <cstring>
#include <math.h>
#include <stdexcept>
#include <cassert>
#include <thread>
#include <mutex>

#include "Root.hpp"

// 图形数据存储

// 方向种类
static const char Dir_kind[32] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
    9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 0};

// 对应方向数量＄1�71ￄ1�7732方向 ↄ1�71ￄ1�77 16方向 ↄ1�71ￄ1�77 每象附1�71ￄ1�774丄1�71ￄ1�77(因为象限会正贄1�71ￄ1�77)
static const float kBorder[8] = {0.1989f, 0.422f, 0.6682f, 1.0f, 1.4966f, 2.422f, 5.0273f, 9999.0f};

// 长度范围计算，用于映射的区间下限
vector<int> thresholds = {0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192};

// 用于返回长度的区间�ￄ1�71ￄ1�77
vector<int> categories = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

// 方向计算区间
char fast_dir(float diff_x, float diff_y) // 与原点的差异
{
    if (diff_x == 0 && diff_y == 0)
        return 0; // 0属于丢�个像素点

    // 确定象限偏移
    int base = 0;
    if (diff_x < 0)
        base += 8; // 左半琄1�71ￄ1�77
    if (diff_y < 0)
        base = 24 - base; // 下半琄1�71ￄ1�77

    float ax = fabsf(diff_x);
    float ay = fabsf(diff_y);

    // 决定 ratio
    float r;

    if (ax != 0)
        r = ay / ax;
    else if (diff_y > 0)
        return 4;
    else if (diff_y <= 0)
        return 12;

    int idx = 0;
    for (; idx < 8; idx++)
        if (r <= kBorder[idx])
            break;

    // 如果圄1�71ￄ1�772〄1�71ￄ1�774象限，就霢�要旋转象限方向编叄1�71ￄ1�77
    if ((diff_x < 0 && diff_y > 0) || (diff_x > 0 && diff_y < 0))
        idx = 7 - idx;

    char dir = base + idx;

    dir = Dir_kind[dir];

    return dir; // 方向 0~15
} // 方向计算函数结束

// 长度查找
int Length_trans_Range(int value) // 返回值所属的类别
{
    // 二分查找找到第一个大于等于value的阈值位罄1�71ￄ1�77
    auto it = lower_bound(thresholds.begin(), thresholds.end(), value);

    // 返回对应的类刄1�71ￄ1�77
    int index = distance(thresholds.begin(), it);

    return categories[index];
}

// 范围转长庄1�71ￄ1�77
int Range_trans_Length(int value)
{
    // 二分查找找到第一个大于等于value的阈值位罄1�71ￄ1�77
    auto it = lower_bound(categories.begin(), categories.end(), value);

    // 返回对应的类刄1�71ￄ1�77
    int index = distance(categories.begin(), it);

    return thresholds[index];
}

// 得到两点之间的方向距禄1�71ￄ1�77
inline void get_two_point_direction_distance(int x0, int y0, int x1, int y1,
                                             char &return_direction, char &return_distance)
{
    int dif_x = x1 - x0;
    int dif_y = y1 - y0;

    return_direction = fast_dir(dif_x, dif_y);
    return_distance = Length_trans_Range(sqrt(dif_x * dif_x + dif_y * dif_y));
}

// 得到在原点方向距离下的点
inline void get_another_point(int x0, int y0, char direction_range, char distance_range,
    int &return_x1, int &return_y1)
{
    // 方向角度转换弧度
    float angle = direction_range * 22.5 / (2 * 3.1415f); // 角度/2t=弧度

    int distance = Range_trans_Length(distance_range);
    int x_add = distance * 0.7;

    // 计算斜率前置釄1�71ￄ1�77
    float dx = cosf(angle);
    float dy = sinf(angle);

    // 水平角度情况
    if (fabsf(dx) < 1e-6f)
    {
        return_x1 += distance;
        return_y1 = y0; // 差不多是原y

        return;
    }

    float slope = dy / dx; // 斜率

    return_y1 = y0 + slope * (return_x1 - x0);

    char test_distance_range;
    char test_direction_range;
    get_two_point_direction_distance(x0, y0, return_x1, return_y1, test_direction_range, test_distance_range);

    while (test_distance_range < distance_range)
    {
        return_x1 += 1;
        return_y1 = y0 + slope * (return_x1 - x0);
        get_two_point_direction_distance(x0, y0, return_x1, return_y1, test_direction_range, test_distance_range);
    }
}

void coordinate_exchange(int origin_x, int origin_y, int origin_size,
    int& object_x, int& object_y, int object_size)
{
    float compare_size = object_size / origin_size;
    object_x = origin_x / compare_size;
    object_y = origin_y / compare_size;
}

// 四舍五入
int approximate(float value)
{
    int un_float = (int)value;

    if (value > 0)
    {
        if (value - un_float > 0.5)
            return un_float + 1;
        else
            return un_float;
    }
    else
    {
        if (value - un_float > -0.5)
            return un_float;
        else
            return un_float - 1;
    }
}

// 连线计算
void two_point_to_line(
    short x0, short y0,
    short x1, short y1,
    vector<Point_2d> &return_line)
{
    int dx = x0 - x1;
    int dy = y0 - y1;

    return_line.reserve(dx + dy);

    float slope = dy / dx; // 斜率

    for (int q = 0; q < dx; q++)
    {
        float x = x0 + q;
        float y = y0 + slope * q; // y点�ￄ1�71ￄ1�77
        Point_2d p2;

        if (x - (int)x < 0.5) // 求近伄1�71ￄ1�77
            p2.x = x;
        else
            p2.x = x + 1;

        if (y - (int)y < 0.5)
            p2.y = y;
        else
            p2.y = y + 1;

        return_line.push_back(p2);
    }
}

// 矩形空间界限
struct rectangle_border
{
    vector<Point_2d> a_line;
    vector<Point_2d> b_line;
    vector<Point_2d> c_line;
    vector<Point_2d> d_line;
};


rectangle_border dir_extend_region(
    short x0, short y0,
    char min_dir, char max_dir,
    char min_len, char max_len)
{
    short min_length = Range_trans_Length(min_len);
    short max_length = Range_trans_Length(max_len);

    char min_direction = min_dir * 2 - 1;
    char max_direction = max_dir * 2 + 1;

    if (min_direction < 0)
        min_direction += 31;

    float min_angle = min_direction * 11.25 / (2 * 3.1415f);
    float max_angle = max_direction * 11.25 / (2 * 3.1415f);

    float min_dx = cosf(min_angle);
    float min_dy = sinf(min_angle);
    float max_dx = cosf(max_angle);
    float max_dy = sinf(max_angle);

    int x1 = min_length * min_dx;
    int y1 = min_length * min_dy;

    int x2 = max_length * min_dx;
    int y2 = max_length * min_dy;

    int x3 = max_length * max_dx;
    int y3 = max_length * max_dy;

    int x4 = min_length * max_dx;
    int y4 = min_length * max_dy;

    rectangle_border result;

    two_point_to_line(x1, y1, x2, y2, result.a_line);
    two_point_to_line(x2, y2, x3, y3, result.b_line);
    two_point_to_line(x3, y3, x4, y4, result.c_line);
    two_point_to_line(x4, y4, x1, y1, result.d_line);

    return result;
}

struct image_object_locate_info
{
    char direction;
    char distance;
    char relative_scale;
    char a;
};


image_object_locate_info image_object_locate(INDEX self_idx,   // 本体
    INDEX object_idx) // 目标
{
    
    image_object_locate_info info;

    Node_Image_Attribute object_attribute = read_a_Node_Image_Attribute(object_idx);
    Node_Image_Attribute self_attribute = read_a_Node_Image_Attribute(self_idx);

    char object_size = object_attribute.observe_size;
    char self_size = self_attribute.observe_size;

    get_two_point_direction_distance(
        self_attribute.x * self_size,
        self_attribute.y * self_size,
        object_attribute.x * object_size,
        object_attribute.y * object_size,
        info.direction, info.distance);

    if (object_size > self_size)
        info.relative_scale = -(object_size / self_size);
    else if (object_size < self_size)
        info.relative_scale = self_size / object_size;
    else
        info.relative_scale = 1;

    return info;
}


vector<vector<INDEX>> stat_connected_components(
    vector<INDEX> &curr_being)
{
    int n = curr_being.size();
    vector<vector<INDEX>> result;
    vector<char> connect(n, 0);

    int connect_signal = 1;

    for (int a = 0; a < n; a++)
    {
        General_Node &curr_node = General_Node_Storage[a];
        if (connect[a] == 0)
        {
            for (int b = 0; b < curr_node.Node_variable_attribute_list.size(); b++)
            {
                if (curr_node.Node_variable_attribute_list[b].link_node_attribute.link_Kind >= 1 && curr_node.Node_variable_attribute_list[b].link_node_attribute.link_Kind <= 4)
                {
                    INDEX object_idx = curr_node.Node_variable_attribute_list[b].link_node_attribute.link_idx_or_id;
                    General_Node &object_node = General_Node_Storage[object_idx];

                    int component_idx = object_node.component;

                    if (component_idx != 0)
                    {
                        connect[a] = component_idx;
                        result[component_idx].push_back(curr_being[a]);
                        break;
                    }
                }
            }

            curr_node.component = connect_signal;
            connect_signal = connect_signal + 1;
        }
    }

    return result;
}


void cental_feature(INDEX node_idx)
{

    General_Node curr_node = read_a_General_Node(node_idx);

    ID self_id = curr_node.self_id;

    int *ptr = node_find(self_id);

    if (ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(self_id);

    }

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    vector<ID> check_id;
    vector<float> excellent_rate;

    int stat_num;
    int realize_num;

    for (short fea = 0; fea < neuro_head.used_item_num;)
    {
        Attribute_Head_Item attribute_head = attribute_head_read(ptr);

        if (attribute_head.attribute_kind == 1)
        {
            stat_num = *(ptr + 1);
            realize_num = *(ptr + 2);

            float excellent_rate = realize_num / stat_num;

            if (stat_num < 10)
            {
                excellent_rate = excellent_rate * stat_num / 10;
            }

            Neuro_Time_Item time_condition;
            time_condition.time_left_range;
            time_condition.time_scale;
            auto id = time_condition.related_id;

            check_id.push_back(id);
        }

        fea += attribute_head.item_num;
        ptr += attribute_head.item_num;
    }

}

//
void produce_excellent_calculate(INDEX node_idx)
{
    General_Node curr_node = read_a_General_Node(node_idx);

    ID self_id = curr_node.self_id;

    int *ptr = node_find(self_id);

    if (ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(self_id);
    }

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    unordered_set<vector<INDEX>, VectorUint32_tHash, VectorUint32_tEq> exist_combo_list;

    for (short fea = 0; fea < neuro_head.used_item_num;)
    {
        ;
    }
}


void condition_statistic(INDEX scene_idx, INDEX node_id)
{
    ;
}

void scene_casual_conclude(INDEX scene_idx, INDEX node_id)
{
    ;
}


void inside_concurrence(INDEX scene_idx, INDEX node_id)
{
    ;
}


void contradict_sift(INDEX source_scene_idx,
    vector<INDEX> &generate_predict_event_idx)
{
    General_Scene source_scene = General_Scene_Storage[source_scene_idx];
    generate_predict_event_idx;
}


struct node_explore_measure
{
    int explore_value;
    int useful_value;
};


vector<ID> Find_Common_Element_Twin(INDEX object_a, INDEX object_b)
{
    vector<ID> group_a;
    vector<ID> group_b;

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


vector<ID> Find_Common_Element_More(vector<ID> neuro_list)
{
    vector<ID> return_element;

    return return_element;
}


vector<Neuro_Link_Attribute> Find_Common_Relation_Rate(vector<Neuro_Link_Attribute> link_list)
{
    vector<Neuro_Link_Attribute> return_relation;

    if (link_list[0].image_attribute.item_is_image == 1)
    {
        ;
    }

    for (Neuro_Link_Attribute a_link : link_list)
    {
        ;
    }

    return return_relation;

}


INDEX new_upper_network_node_generate(
    vector<INDEX> material_node_list,
    char generate_kind, INDEX scene_idx, char scene_kind)
{
    char material_size = material_node_list.size();

    INDEX new_idx = create_a_General_Node();
    
    if (generate_kind == 1)
    {
        General_Node new_node;
        new_node.Node_variable_attribute_list.resize(material_size);

        for (int q = 0; q < material_size; q++)
        {
            INDEX i = material_node_list[q];

            Link_Node_Attribute link_object;

            link_object.link_Kind = 001;
            link_object.link_idx_or_id = i;
            link_object.link_value = 1;

            new_node.Node_variable_attribute_list[q].link_node_attribute = link_object;
        }

        create_a_General_Node(new_node);
    }
    else if (generate_kind == 2)
    {
        Image_Scene& curr_map = Image_Scene_Storage[scene_idx];

        General_Node& new_node = General_Node_Storage[new_idx];

        int all_occupy = 0;
        int x_sum = 0;
        int y_sum = 0;

        for (int a = 0; a < material_size; a++)
        {
            INDEX i = material_node_list[a];
            General_Node &material_node = General_Node_Storage[i];

            Link_Node_Attribute link_object;

            link_object.link_Kind = 2;
            link_object.link_idx_or_id = new_idx;
            Variable_Attribute fo{.link_node_attribute = link_object};
            material_node.Node_variable_attribute_list.push_back(fo);

            link_object.link_Kind = 1;
            link_object.link_idx_or_id = i;
            Variable_Attribute fo_{.link_node_attribute = link_object};
            new_node.Node_variable_attribute_list.push_back(fo_);

            Node_Image_Attribute obj_att = read_a_Node_Image_Attribute(i);

            x_sum += obj_att.block_num * obj_att.x;
            y_sum += obj_att.block_num * obj_att.y;

            all_occupy += obj_att.block_num;
        }

        x_sum = x_sum / all_occupy;
        y_sum = y_sum / all_occupy;

        Node_Image_Attribute add_att; // = Node_Image_Attribute_List[create_a_Node_Image_Attribute(new_idx)];
        add_att.block_num = all_occupy;
        add_att.x = x_sum;
        add_att.y = y_sum;
        new_node.complete_or_probability = 1;

        for (int t = 0; t < material_size; t++)
        {
            INDEX i = material_node_list[t];
            General_Node &material_node = General_Node_Storage[i];
            Node_Image_Attribute att = read_a_Node_Image_Attribute(i);

            int dif_x = x_sum - att.x;
            int dif_y = y_sum - att.y;

            add_att.contain_distance += sqrt(dif_x * dif_x + dif_y * dif_y);
        }

        curr_map.Node_combo_find_repeat.insert(material_node_list);

        char require_number = 1;
        char require_idx = 0;

        while (require_number * require_number >= all_occupy && require_idx < 4)
        {
            require_number *= 2;
            require_idx += 1;
        }

        //

        char require_distance =
            curr_map.require_distance;

        x_sum /= require_distance;
        y_sum /= require_distance;

        int require_width = curr_map.width / require_distance;

        curr_map.Node_space_form_record_list[y_sum * require_width + x_sum].push_back(new_idx);
    }
    else if (generate_kind == 3)
    {
        Text_Scene &curr_text = Text_Scene_Storage[scene_idx];

        vector<vector<INDEX>> text_infer_node_list = curr_text.Text_node_space_from_record_list;

        General_Node &new_node = General_Node_Storage[new_idx];

        int all_occupy = 0;
        int distance_sum = 0;

        for (int a = 0; a < material_size; a++)
        {
            INDEX i = material_node_list[a];
            General_Node &material_node = General_Node_Storage[i];

            Link_Node_Attribute link_object;

            link_object.link_Kind = 001;
            link_object.link_idx_or_id = new_idx;
            link_object.link_value = 1;
            Variable_Attribute fo{.link_node_attribute = link_object};

            material_node.Node_variable_attribute_list.push_back(fo);

            link_object.link_Kind = 002;
            link_object.link_idx_or_id = i;
            link_object.link_value = 1;
            Variable_Attribute fo_{.link_node_attribute = link_object};

            new_node.Node_variable_attribute_list.push_back(fo_);

            Node_Text_Attribute att = read_a_Node_Text_Attribute(i);
            distance_sum += att.block_num * att.idx;
            all_occupy += att.block_num;
        }

        
        distance_sum = distance_sum / all_occupy;

        Node_Text_Attribute add_att;
        add_att.block_num = all_occupy;
        add_att.idx = distance_sum;
        create_a_Node_Text_Attribute(new_idx, add_att);


        INDEX add_pos = add_att.idx;
        add_pos = add_pos / 8;

        text_infer_node_list[add_pos].push_back(new_idx);
    }
    else if (generate_kind == 4)
    {

        General_Node& new_node = General_Node_Storage[new_idx];

        int all_occupy = 0;
        int distance_sum = 0;

        for (int a = 0; a < material_size; a++)
        {
            INDEX i = material_node_list[a];
            General_Node &material_node = General_Node_Storage[i];

            Link_Node_Attribute link_object;

            link_object.link_Kind = 001;
            link_object.link_idx_or_id = new_idx;
            link_object.link_value = 1;
            Variable_Attribute flex = {.link_node_attribute = link_object};

            material_node.Node_variable_attribute_list.push_back(flex);

            link_object.link_Kind = 003;
            link_object.link_idx_or_id = i;
            link_object.link_value = 1;
            flex = {.link_node_attribute = link_object};

            new_node.Node_variable_attribute_list.push_back(flex);

            Node_Time_Attribute att = read_a_Node_Time_Attribute(i);

            long long material_time = 0;

            material_time = att.front_time;
            material_time = material_time << 32;
            material_time |= att.back_time;

            // distance_sum += att.node_num * material_time;
            // all_occupy += att.node_num;
        }

        Node_Time_Attribute add_att;
        // add_att.node_num = all_occupy;
        add_att.back_time = distance_sum;
        create_a_Node_Time_Attribute(new_idx, add_att);

        //
    }

    return new_idx;
}


void new_neuro_generate_record(
    INDEX upper_node_idx, char generate_node_kind,
    float rich_rate)
{
    unsigned char allow_value = 255 * rich_rate;

    General_Node &upper_node = General_Node_Storage[upper_node_idx];

    vector<Variable_Attribute> &upper_link_object_list = upper_node.Node_variable_attribute_list;

    if (generate_node_kind == 1)
    {
        Neuro_Image_Desc image_desc;
        Neuro_Desc neuro_desc = {.image_desc = image_desc};

        ID get_id = new_neuro_create(1, neuro_desc);
        upper_node.self_id = get_id;

        Attribute_Head_Item upper_attribute_head;
        upper_attribute_head.attribute_kind = 2;

        vector<General_Condition> upper_general_condition_list;
        vector<General_Result> upper_general_result_list;

        vector<int> add_item;

        char lower_node_num = 0;

        for (int q = 0; q < upper_link_object_list.size(); q++)
        {
            Link_Node_Attribute la = upper_link_object_list[q].link_node_attribute;

            if (la.link_Attribute != 2 && la.link_Kind != 1)
                continue;

            INDEX write_pos = la.link_idx_or_id;
            ID material_id = General_Node_Storage[write_pos].self_id;

            if (rand_0_to_255() < allow_value)
            {
                lower_node_num += 1;

                Attribute_Head_Item lower_attribute_head;
                lower_attribute_head.attribute_kind = 2;

                vector<General_Condition> lower_general_condition_list;
                vector<General_Result> lower_general_result_list;

                for (int w = 0; w < upper_link_object_list.size(); w++)
                {
                    if (la.link_Attribute != 2 && la.link_Kind != 1 && q == w)
                        continue;

                    INDEX con_pos = upper_link_object_list[w].link_node_attribute.link_idx_or_id;

                    image_object_locate_info space_feature = image_object_locate(write_pos, con_pos);

                    Neuro_Image_Item image_comdition;

                    image_comdition.direction = space_feature.direction;
                    image_comdition.distance = space_feature.distance;
                    image_comdition.related_scale = space_feature.relative_scale;
                    image_comdition.related_id = General_Node_Storage[con_pos].self_id;

                    General_Condition condition = {.image_condition = image_comdition};
                    lower_general_condition_list.push_back(condition);

                    lower_attribute_head.condition_num += 1;
                }

                Neuro_Image_Item result_image;
                image_object_locate_info space_feature;
                space_feature = image_object_locate(write_pos, upper_node_idx);

                result_image.direction = space_feature.direction;
                result_image.distance = space_feature.distance;
                result_image.related_scale = space_feature.relative_scale;
                result_image.related_id = get_id;

                General_Result result = {.image_result = result_image};
                lower_general_result_list.push_back(result);

                lower_attribute_head.result_num += 1;

                general_attribute_put(lower_attribute_head, add_item, lower_general_condition_list, lower_general_result_list);
                neuro_attribute_write(material_id, add_item, 0, 1);
            }
        }

        add_item.clear();

        for (int a = 0; a < lower_node_num; a++)
        {
            general_attribute_put(upper_attribute_head, add_item, upper_general_condition_list, upper_general_result_list);
            neuro_attribute_write(get_id, add_item, 0, 1);
        }

        ID_Find_General_Node[get_id].push_back(upper_node_idx);
    }
    else if (generate_node_kind == 2)
    {
        Neuro_Text_Desc text_desc;
        Neuro_Desc neuro_desc = {.text_desc = text_desc};

        ID get_id = new_neuro_create(2, neuro_desc);
        upper_node.self_id = get_id;

        Attribute_Head_Item upper_attribute_head;
        upper_attribute_head.attribute_kind = 2;

        vector<General_Condition> upper_general_condition_list;
        vector<General_Result> upper_general_result_list;

        vector<int> add_item;

        char lower_node_num = 0;

        for (int q = 0; q < upper_link_object_list.size(); q++)
        {
            Link_Node_Attribute lo = upper_link_object_list[q].link_node_attribute;

            if (lo.link_Kind != 1 && lo.link_Attribute != 2)
                continue;

            INDEX wri_pos = lo.link_idx_or_id;
            ID material_id = General_Node_Storage[wri_pos].self_id;

            if (rand_0_to_255() < allow_value)
            {
                lower_node_num += 1;

                Attribute_Head_Item lower_attribute_head;
                lower_attribute_head.attribute_kind = 2;

                vector<General_Condition> lower_general_condition_list;
                vector<General_Result> lower_general_result_list;

                for (int w = 0; w < upper_link_object_list.size(); w++)
                {
                    if (upper_link_object_list[w].link_node_attribute.link_Attribute == 2 && upper_link_object_list[w].link_node_attribute.link_Kind == 1 && q != w)
                        continue;

                    INDEX con_pos = upper_link_object_list[w].link_node_attribute.link_idx_or_id;

                    Neuro_Text_Item text_comdition;

                    text_comdition.left_distance;
                    text_comdition.right_distance;

                    General_Condition condition = {.text_condition = text_comdition};
                    lower_general_condition_list.push_back(condition);

                    lower_attribute_head.condition_num += 1;
                }

                Neuro_Text_Item text_result;

                General_Result result = {.text_result = text_result};
                lower_general_result_list.push_back(result);

                lower_attribute_head.result_num += 1;

                Simple_Neuro_Attribute_write(material_id, 0, 1, lower_attribute_head,
                    lower_general_condition_list, lower_general_result_list);

                general_attribute_put(lower_attribute_head, add_item, lower_general_condition_list, lower_general_result_list);
                neuro_attribute_write(material_id, add_item, 0, 1);
            }
        }

        for (int a = 0; a < lower_node_num; a++)
        {
            general_attribute_put(upper_attribute_head, add_item, upper_general_condition_list, upper_general_result_list);
            neuro_attribute_write(get_id, add_item, 0, 1);
        }

        ID_Find_General_Node[get_id].push_back(upper_node_idx);
    }
    else if (generate_node_kind == 3)
    {
        Neuro_Concept_Desc concept_desc;
        Neuro_Desc neuro_desc = {.concept_desc = concept_desc};

        ID get_id = new_neuro_create(3, neuro_desc);
        upper_node.self_id = get_id;

        Attribute_Head_Item upper_attribute_head;
        upper_attribute_head.attribute_kind = 2;

        vector<General_Condition> upper_general_condition_list;
        vector<General_Result> upper_general_result_list;

        vector<int> add_item;

        char lower_node_num = 0;

        for (int q = 0; q < upper_link_object_list.size(); q++)
        {
            Link_Node_Attribute lo = upper_link_object_list[q].link_node_attribute;

            if (lo.link_Kind != 1 && lo.link_Attribute != 2)
                continue;

            INDEX wri_pos = lo.link_idx_or_id;
            ID material_id = General_Node_Storage[wri_pos].self_id;

            if (rand_0_to_255() < allow_value)
            {
                lower_node_num += 1;

                Attribute_Head_Item lower_attribute_head;
                lower_attribute_head.attribute_kind = 2;

                vector<General_Condition> lower_general_condition_list;
                vector<General_Result> lower_general_result_list;

                for (int w = 0; w < upper_link_object_list.size(); w++)
                {
                    Link_Node_Attribute ola = upper_link_object_list[w].link_node_attribute;

                    if (ola.link_Attribute == 2 && ola.link_Kind == 1 && q != w)
                        continue;

                    INDEX con_pos = ola.link_idx_or_id;

                    Neuro_Time_Item time_condition;

                    time_condition.logic;
                    time_condition;

                    General_Condition condition = {.time_condition = time_condition};
                    lower_general_condition_list.push_back(condition);

                    lower_attribute_head.condition_num += 1;
                }

                // 生成上级
                Neuro_Time_Item time_result;

                General_Result result = {.time_result = time_result};
                lower_general_result_list.push_back(result);

                lower_attribute_head.result_num += 1;

                Simple_Neuro_Attribute_write(material_id, 0, 1, lower_attribute_head,
                                             lower_general_condition_list, lower_general_result_list);

                general_attribute_put(lower_attribute_head, add_item, lower_general_condition_list, lower_general_result_list);
                neuro_attribute_write(material_id, add_item, 0, 1);
            }
        }

        for (int a = 0; a < lower_node_num; a++)
        {
            general_attribute_put(upper_attribute_head, add_item, upper_general_condition_list, upper_general_result_list);
            neuro_attribute_write(get_id, add_item, 0, 1);
        }

        ID_Find_General_Node[get_id].push_back(upper_node_idx);
    }
}
// 函数结束



struct Generate_Permit
{
    bool condition_image_permit = 0;
    char condition_time_permit = 0;
    char condition_number_permit = 0;
    char condition_text_permit = 0;
    char conditon_action_permit = 0;

    char result_image_permit = 0;
    char result_time_permit = 0;
    char result_number_permit = 0;
    char result_text_permit = 0;
    char result_belief_permit = 0;
    char result_manner_permit = 0;
};

Generate_Permit a_generate_permit;


inline void General_Cognition_Generate_Module(
    INDEX curr_node_idx, INDEX scene_idx,
    short &fea, int *&ptr,
    Generate_Permit generate_permit = a_generate_permit)
{
    Attribute_Head_Item attribute_head = attribute_head_read(ptr);
    char attribute_kind = attribute_head.attribute_kind;

    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size = 0;

    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size = 0;

    switch (attribute_kind)
    {
        ;
    }
}


inline void Image_Cognition_Generate_Module(
    INDEX curr_node_idx, INDEX scene_idx,
    short &fea, int *&ptr,
    Generate_Permit generate_permit = a_generate_permit)
{
    Image_Scene &curr_scene = Image_Scene_Storage[scene_idx];
    unordered_map<ID, vector<INDEX>> &Id_find_overall_node = curr_scene.Id_find_overall_node;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID curr_node_id = curr_node.self_id;

    Node_Image_Attribute image_attribute = read_a_Node_Image_Attribute(curr_node_idx);
    unsigned short x = image_attribute.x;
    unsigned short y = image_attribute.y;
    INDEX require_idx = x + curr_scene.width * y;

    Attribute_Head_Item attribute_head = attribute_head_read(ptr);

    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size = 0;

    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size = 0;

    int matchCount = result_num;
    
    vector<INDEX> the_combo_record;

    *(ptr + 1) += 1;

    ptr + 3;

    if (attribute_head.attribute_kind == 1)
    {
        Match_Generate match_generate;
        match_generate.node_idx_from[0] = curr_node_idx;
        match_generate.node_num += 1;

        for (char a = 0; a < result_num; a += 1)
        {
            return_item_kind_and_size(ptr, condition_kind, condition_size);

            switch (condition_kind)
            {
            case 1:
            {
                Neuro_Image_Item image_condition = *(Neuro_Image_Item *)(ptr);
                ID need_id = image_condition.related_id;
                char logic = image_condition.logic;
                
                if (!Id_find_overall_node.count(need_id))
                {
                    matchCount--;

                    match_generate.match_condition[match_generate.result_num].image_condition = image_condition;
                    match_generate.result_num += 1;
                    match_generate.node_idx_from[match_generate.node_num] = curr_node_idx;
                    continue;
                } else
                {
                    vector<INDEX> possible_object = Id_find_overall_node[need_id];
                    int possible_object_size = possible_object.size();

                    char object_is_exist = 0;

                    for (int b = 0; b < possible_object_size; b++)
                    {
                        INDEX possible_object_idx = possible_object[b];
                        Node_Image_Attribute object_image_attribute = read_a_Node_Image_Attribute(possible_object_idx);

                        char compare_direction;
                        char compare_distance;

                        get_two_point_direction_distance(x, y, object_image_attribute.x,
                            object_image_attribute.y, compare_direction, compare_distance);

                        char diff_direction = abs(compare_direction - image_condition.direction);
                        char diff_distance = abs(compare_distance - image_condition.distance);

                        if (diff_direction < image_condition.direction_scale && diff_distance < image_condition.distance_scale)
                        {
                            *(ptr + 2) += 1;
                            object_is_exist = 1;
                            break;
                        }
                    }

                    if (object_is_exist == 0)
                    {
                        match_generate.match_result[match_generate.result_num].image_result = image_condition;
                        match_generate.result_num += 1;
                        match_generate.node_idx_from[match_generate.node_num] = curr_node_idx;
                        continue;

                        INDEX match_idx = create_a_local_Match_Generate(curr_scene.Match_generate_list,
                            curr_scene.Free_match_generate_index, match_generate);

                        curr_scene.Id_find_image_match[need_id].push_back(match_idx);
                    }
                }
            }
            break;
            case 2:
            {
                Neuro_Number_Item number_result = *(Neuro_Number_Item *)(ptr);
                ID need_id;
                char logic = number_result.logic;
            }
            break;
            case 3:
            {
                Neuro_Time_Item time_result = *(Neuro_Time_Item *)(ptr);
                ID need_id = time_result.related_id;
                char logic = time_result.logic;
            }
            break;
            case 5:
            {
                Neuro_Concept_Item concept_result = *(Neuro_Concept_Item *)(ptr);
                ID need_id = concept_result.target_id;
                char logic = concept_result.logic;
            }
            }
        }
    }
    else if (attribute_head.attribute_kind == 2)
    {

        Match_Generate match_generate;
        match_generate.node_idx_from[0] = curr_node_idx;
        match_generate.node_num += 1;

        for (char a = 0; a < condition_num; a += 1)
        {
            return_item_kind_and_size(ptr, condition_kind, condition_size);

            switch (condition_kind)
            {
            case 1:
            {
                Neuro_Image_Item image_condition = *(Neuro_Image_Item *)(ptr);
                ID need_id = image_condition.related_id;
                char logic = image_condition.logic;

                if (!Id_find_overall_node.count(need_id))
                {
                    if (logic != 0)
                    {
                        matchCount--;

                        match_generate.match_condition[match_generate.need_condition_num].image_condition = image_condition;
                        match_generate.node_num += 1;
                        continue;
                    }
                }

                char candidate_amount = Id_find_overall_node[need_id].size();

                char need_direction = image_condition.direction;
                char need_distance = image_condition.distance;
                char need_related_scale = image_condition.related_scale;
                char permit_direction_scale = image_condition.direction_scale;
                char permit_distance_scale = image_condition.distance_scale;

                vector<INDEX> candidate_list = Id_find_overall_node[need_id];

                for (int b = 0; b < candidate_amount; b++)
                {
                    INDEX target_idx = candidate_list[b];

                    Node_Image_Attribute target_image_attritube = read_a_Node_Image_Attribute(target_idx);
                    char com_direction;
                    char com_distance;
                    get_two_point_direction_distance(x, y, target_image_attritube.x, target_image_attritube.y,
                        com_direction, com_distance);

                    if (abs(com_distance - need_distance) <= (permit_distance_scale - 1) &&
                        abs(com_direction - need_direction) <= (permit_direction_scale - 1))
                    {
                        the_combo_record.push_back(target_idx);
                        break;
                    }
                    else if (b == (candidate_amount - 1))
                    {
                        match_generate.match_condition[match_generate.need_condition_num].image_condition = image_condition;
                        match_generate.need_condition_num += 1;

                        matchCount--;
                    }
                }
            }
            break;
            case 2:
            {
                Neuro_Number_Item number_condition = *(Neuro_Number_Item *)(ptr);
                ID need_id;
                char logic = number_condition.logic;

                if (!Id_find_overall_node.count(need_id))
                {
                    if (logic != 0)
                    {
                        matchCount--;

                        match_generate.match_condition[match_generate.need_condition_num].number_condition = number_condition;
                        match_generate.node_num += 1;
                        continue;
                    }
                }

                char candidate_amount = Id_find_overall_node[need_id].size();

                // if()
                // {
                //     ;
                // }
            }
            break;
            case 3:
            {
                Neuro_Time_Item time_condition = *(Neuro_Time_Item *)(ptr);
                ID need_id = time_condition.related_id;
                char logic = time_condition.logic;

                if (!Id_find_overall_node.count(need_id))
                {
                    if (logic != 0)
                    {
                        matchCount--;

                        match_generate.match_condition[match_generate.need_condition_num].time_condition = time_condition;
                        match_generate.node_num += 1;
                        continue;
                    }
                }

                char candidate_amount = Id_find_overall_node[need_id].size();
            }
            break;
            case 5:
            {
                Neuro_Concept_Item concept_condition = *(Neuro_Concept_Item *)(ptr);
                ID need_id = concept_condition.target_id;
                char logic = concept_condition.logic;

                if (!Id_find_overall_node.count(need_id))
                {
                    if (logic != 0)
                    {
                        matchCount--;

                        match_generate.match_condition[match_generate.need_condition_num].concept_condition = concept_condition;
                        match_generate.node_num += 1;
                        continue;
                    }
                }

                char candidate_amount = Id_find_overall_node[need_id].size();
            }
            }
        }

        return_item_kind_and_size(ptr, result_kind, result_size);
        Neuro_Image_Item image_result = *(Neuro_Image_Item *)(ptr); // 为什么确定是图结构1�71ￄ1�77

        if (matchCount < result_num)
        {
            fea += attribute_head.item_num;
            ptr += attribute_head.item_num;

            match_generate.match_result[match_generate.result_num].image_result = image_result;
            create_a_local_Match_Generate(curr_scene.Match_generate_list, curr_scene.Free_match_generate_index,
                match_generate);
            return;
        }

        the_combo_record.push_back(curr_node_id);
        the_combo_record.push_back(*(ptr + 9));
        sort(the_combo_record.begin(), the_combo_record.end());

        
        if (curr_scene.Node_combo_find_repeat.count(the_combo_record))
        {
            vector<Variable_Attribute> &Variable_list = curr_node.Node_variable_attribute_list;

            //
            for (Variable_Attribute variable_attribute : Variable_list)
            {
                if (variable_attribute.link_node_attribute.link_Attribute != 2)
                    continue;

                if (variable_attribute.link_node_attribute.link_idx_or_id != image_result.related_id)
                    continue;

                INDEX idx = variable_attribute.link_node_attribute.link_idx_or_id;
                Node_Image_Attribute image_attribute = read_a_Node_Image_Attribute(idx);

                char compare_direction;
                char compare_distance;

                get_two_point_direction_distance(x, y, image_attribute.x * image_attribute.observe_size,
                    image_attribute.y * image_attribute.observe_size, compare_direction, compare_distance);

                if (abs(image_result.direction - compare_direction) < image_result.direction_scale && abs(image_result.distance - compare_distance) < image_result.distance_scale)
                {
                    fea += attribute_head.item_num;
                    ptr += attribute_head.item_num;
                    return;
                }
            }
        }

        
        if (result_kind == 1)
        {
            INDEX add_idx = new_upper_network_node_generate(the_combo_record, result_kind, scene_idx, 1);
            the_combo_record.clear();
            General_Node &add_node = General_Node_Storage[add_idx];
            add_node.complete_or_probability = matchCount / result_num;

            add_node.self_id = image_result.related_id;
            curr_scene.Id_find_overall_node[image_result.related_id].push_back(add_idx);

            int image_match_num = 0;
            vector<Match_Generate> &Match_generate_list = curr_scene.Match_generate_list;

            
            if (curr_scene.Id_find_image_match.count(add_node.self_id))
            {
                vector<INDEX> &match_idx = curr_scene.Id_find_image_match[add_node.self_id];
                image_match_num = match_idx.size();

                
                for (int q = 0; q < image_match_num; q++)
                {
                    if (match_idx[q] == -1)
                        continue;

                    Match_Generate image_match = Match_generate_list[match_idx[q]];
                    int i = 0;

                    for (; i < 5 && image_match.match_condition[i].image_condition.related_id != add_node.self_id; i++)
                        ;
                    Neuro_Image_Item oritation_condition = image_match.match_condition[i].image_condition;

                    if (oritation_condition.logic == 1)
                    {
                        oritation_condition.related_scale;
                        oritation_condition.direction;
                        oritation_condition.distance;

                        {
                            image_match.match_condition[i].image_condition.related_id = 0;
                            match_idx[q] = -1;
                            image_match.need_condition_num -= 1;

                            if (image_match.need_condition_num == 0)
                            {
                                vector<INDEX> source_node;
                                source_node.reserve(image_match.node_num);

                                for (int a = 0; a < image_match.node_num; a++)
                                    source_node.push_back(image_match.node_idx_from[a]);

                                char generate_kind;

                                add_idx = new_upper_network_node_generate(source_node, generate_kind, scene_idx, 1);
                                General_Node &new_node = General_Node_Storage[add_idx];
                                ID id = image_match.match_result[0].image_result.related_id;
                                new_node.self_id = id;
                                curr_scene.Id_find_overall_node[id].push_back(add_idx);
                            }
                        }
                    }
                    else
                    {
                        ;
                    }
                }
            }
        }
        else if (result_kind == 5)
        {
            for (int b = 0; b < result_num; b++)
            {
                return_item_kind_and_size(ptr, result_kind, result_size);
                Neuro_Manner_Item manner_result = *(Neuro_Manner_Item *)(ptr);

                if (manner_result.manner_kind >= 3)
                {
                    manner_result.manner_value;
                    manner_result.effect_target;

                    Focus_Object focus_object;
                }
            }
        }
    }
    else if (attribute_head.attribute_kind == 4)
    {
        // **** //
        // 类别生成
        // **** //

        Match_Generate match_generate;

        char a = 0;
        for (; a < condition_num; a += 1)
        {
            return_item_kind_and_size(ptr, condition_kind, condition_size);
            Neuro_Image_Item image_condition = *(Neuro_Image_Item *)(ptr);
            ID need_id = image_condition.related_id;
            char logic = image_condition.logic;

            if (!Id_find_overall_node.count(need_id))
            {
                matchCount--;

                match_generate.match_condition[match_generate.need_condition_num].image_condition = image_condition;
                match_generate.node_num += 1;
                continue;
            }

            char candidate_amount = Id_find_overall_node[need_id].size();

            switch (condition_kind)
            {
            case 1:
            {
                char need_direction = image_condition.direction;
                char need_distance = image_condition.distance;
                char need_related_scale = image_condition.related_scale;
                char permit_direction_scale = image_condition.direction_scale;
                char permit_distance_scale = image_condition.distance_scale;

                vector<INDEX> candidate_list = Id_find_overall_node[need_id];

                for (int b = 0; b < candidate_amount; b++)
                {
                    INDEX target_idx = candidate_list[b];

                    Node_Image_Attribute target_image_attritube = read_a_Node_Image_Attribute(target_idx);
                    char com_direction;
                    char com_distance;

                    get_two_point_direction_distance(x, y, target_image_attritube.x, target_image_attritube.y,
                        com_direction, com_distance);

                    if (abs(com_distance - need_distance) <= (permit_distance_scale - 1) &&
                        abs(com_direction - need_direction) <= (permit_direction_scale - 1))
                    {
                        the_combo_record.push_back(target_idx);
                        goto condition_end;
                    }
                    else if (b == (candidate_amount - 1))
                    {
                        match_generate.match_condition[match_generate.need_condition_num].image_condition = image_condition;
                        match_generate.need_condition_num += 1;

                        matchCount--;
                    }
                }
            }
            break;
            case 2:
            {
                ;
            }
            break;
            case 3:
            {
                ;
            }
            }
        }

    condition_end:

        return_item_kind_and_size(ptr, result_kind, result_size);
        Neuro_Image_Item image_result = *(Neuro_Image_Item *)(ptr); // 为什么确定是图结构1�71ￄ1�77

        // 条件不满足，则跳迄1�71ￄ1�77
        if (matchCount < result_num)
        {
            fea += attribute_head.item_num;
            ptr += attribute_head.item_num;

            match_generate.match_result[match_generate.result_num].image_result = image_result;
            create_a_local_Match_Generate(curr_scene.Match_generate_list, curr_scene.Free_match_generate_index,
                match_generate);
            return;
        }

        the_combo_record.push_back(curr_node_id);
        the_combo_record.push_back(*(ptr + 9));
        sort(the_combo_record.begin(), the_combo_record.end());

        // 1-3、条件满足，进行结果去重棢�浄1�71ￄ1�77
        if (curr_scene.Node_combo_find_repeat.count(the_combo_record))
        {
            // 防止下级节点重复生成，结果的空间方位若一臄1�71ￄ1�77

            // 扫描节点连接，找出连接上节点，匹配是否重处1�71ￄ1�77
            vector<Variable_Attribute> &Variable_list = curr_node.Node_variable_attribute_list;

            //
            for (Variable_Attribute variable_attribute : Variable_list)
            {
                if (variable_attribute.link_node_attribute.link_Attribute != 2)
                    continue;

                if (variable_attribute.link_node_attribute.link_idx_or_id != image_result.related_id)
                    continue;

                INDEX idx = variable_attribute.link_node_attribute.link_idx_or_id;
                Node_Image_Attribute image_attribute = read_a_Node_Image_Attribute(idx);

                char compare_direction;
                char compare_distance;

                get_two_point_direction_distance(x, y, image_attribute.x * image_attribute.observe_size,
                                                 image_attribute.y * image_attribute.observe_size, compare_direction, compare_distance);

                if (abs(image_result.direction - compare_direction) < image_result.direction_scale && abs(image_result.distance - compare_distance) < image_result.distance_scale)
                {
                    // 判断=重复
                    fea += attribute_head.item_num;
                    ptr += attribute_head.item_num;
                    return;
                }
            }
        }

        // 2、节点记载添劄1�71ￄ1�77
        if (result_kind == 1)
        {
            INDEX add_idx = new_upper_network_node_generate(the_combo_record, result_kind, scene_idx, 1);
            the_combo_record.clear();
            General_Node &add_node = General_Node_Storage[add_idx];
            add_node.complete_or_probability = matchCount / result_num;

            add_node.self_id = image_result.related_id;
            curr_scene.Id_find_overall_node[image_result.related_id].push_back(add_idx);

            int image_match_num = 0;
            vector<Match_Generate> &Match_generate_list = curr_scene.Match_generate_list;

            // 3、匹配激洄1�71ￄ1�77
            if (curr_scene.Id_find_image_match.count(add_node.self_id))
            {
                vector<INDEX> &match_idx = curr_scene.Id_find_image_match[add_node.self_id];
                image_match_num = match_idx.size();

                // 挨个匹配列表中的匹配
                for (int q = 0; q < image_match_num; q++)
                {
                    if (match_idx[q] == -1)
                        continue;

                    Match_Generate image_match = Match_generate_list[match_idx[q]];
                    int i = 0;

                    for (; i < 5 && image_match.match_condition[i].image_condition.related_id != add_node.self_id; i++)
                        ;
                    Neuro_Image_Item oritation_condition = image_match.match_condition[i].image_condition;

                    if (oritation_condition.logic == 1) // 图像
                    {
                        oritation_condition.related_scale;
                        oritation_condition.direction;
                        oritation_condition.distance;

                        // if() 匹配通过
                        {
                            image_match.match_condition[i].image_condition.related_id = 0;
                            match_idx[q] = -1;
                            image_match.need_condition_num -= 1;

                            if (image_match.need_condition_num == 0) // 若全部条件满足，生成该节炄1�71ￄ1�77
                            {
                                vector<INDEX> source_node;
                                source_node.reserve(image_match.node_num);

                                for (int a = 0; a < image_match.node_num; a++)
                                    source_node.push_back(image_match.node_idx_from[a]);

                                char generate_kind;

                                add_idx = new_upper_network_node_generate(source_node, generate_kind, scene_idx, 1);
                                General_Node &new_node = General_Node_Storage[add_idx];
                                ID id = image_match.match_result[0].image_result.related_id;
                                new_node.self_id = id;
                                curr_scene.Id_find_overall_node[id].push_back(add_idx);
                            }
                        }
                    }
                    else
                    { // 概念
                        // 同上
                    }
                }
            }
        }
    }

    fea += attribute_head.item_num;
    ptr += attribute_head.item_num;
    
}


inline void Text_Cognition_Generate_Module(
    INDEX pos_idx, INDEX scene_idx,
    short &fea, int *&ptr,
    Generate_Permit generate_permit = a_generate_permit)
{
    Text_Scene &curr_text_network = Text_Scene_Storage[scene_idx];

    vector<vector<INDEX>> &text_node_storage_list = curr_text_network.Text_node_space_from_record_list;

    vector<Variable_Attribute> All_Require_Object = curr_text_network.Require_object_list;
    INDEX text_node_idx = pos_idx;

    General_Node curr_node = read_a_General_Node(text_node_idx);
    ID curr_node_id = curr_node.self_id;

    Attribute_Head_Item attribute_head = attribute_head_read(ptr);

    char attribute_kind = attribute_head.attribute_kind;
    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size;

    char result_kind;
    int original_text_size = curr_text_network.original_text_size;

    *(ptr + 1) += 1;

    if (attribute_kind == 1)
    {
        Neuro_Text_Item text_condition = *(Neuro_Text_Item *)(ptr + 3);

        ID neighbor_id = text_condition.target_id;

        int left_distance = text_condition.left_distance;
        int right_distance = text_condition.right_distance;
        int distance_scale = text_condition.distance_scale;

        int front_boundary = pos_idx + left_distance * distance_scale;
        int back_boundary = pos_idx + right_distance * distance_scale;

        if (front_boundary < 0)
            front_boundary = 0;

        if (back_boundary > original_text_size - 1)
            back_boundary = original_text_size - 1;

        char num = 0;

        INDEX front_vec = front_boundary / 8;
        INDEX back_vec = back_boundary / 8;

        
        if (text_condition.effect_kind == 1)
        {
            for (INDEX vec_idx = front_vec; vec_idx <= back_vec; vec_idx++)
            {
                vector<INDEX> &inside_list = text_node_storage_list[vec_idx];
                int inside_size = inside_list.size();

                for (int b = 8; b < inside_size; b++)
                {
                    INDEX object_idx = inside_list[b];
                    Node_Text_Attribute object_attribute_text = read_a_Node_Text_Attribute(object_idx);

                    if (object_attribute_text.idx > back_boundary && object_attribute_text.idx < front_boundary)
                        continue;

                    if (neighbor_id = General_Node_Storage[inside_list[b]].self_id)
                        num += 1;
                }
            }

            if (num)
            {
                *(ptr + 2) += 1;
            }

        }
        else if (text_condition.effect_kind == 2)
        {
            for (INDEX vec_idx = front_vec; vec_idx <= back_vec; vec_idx++)
            {
                vector<INDEX> &inside_list = text_node_storage_list[vec_idx];
                int inside_size = inside_list.size();

                for (int b = 8; b < inside_size; b++)
                {
                    INDEX object_idx = inside_list[b];

                    Node_Text_Attribute object_attribute_text = read_a_Node_Text_Attribute(object_idx);
                    INDEX space_pos = object_attribute_text.idx;

                    if (space_pos < front_boundary && space_pos > back_boundary)
                        continue;

                    if (neighbor_id = General_Node_Storage[object_idx].self_id)
                        num += 1;
                }
            }

            if (num)
            {
                *(ptr + 2) += 1;
            }
        }
    }
    else if (attribute_kind == 2)
    {
        vector<INDEX> condition_node_list;

        char match_count = 0;

        for (int b = 0; b < attribute_head.condition_num; b++)
        {
            if (condition_kind == 4)
            {
                Neuro_Text_Item text_condition_item = *(Neuro_Text_Item *)(ptr + 3 + b * 2);
                ID need_id = text_condition_item.target_id;

                INDEX front_boundary = pos_idx + text_condition_item.left_distance;
                INDEX back_boundary = pos_idx + text_condition_item.right_distance;

                char distance_scale = text_condition_item.distance_scale;

                if (front_boundary < 0)
                    front_boundary = 0;

                if (back_boundary > original_text_size - 1)
                    back_boundary = original_text_size - 1;

                INDEX front_vec = front_boundary / 8;
                INDEX back_vec = back_boundary / 8;

                char condition_statify = 0;

                if (text_condition_item.effect_kind == 1)
                {
                    for (INDEX vec_idx = front_vec; vec_idx <= back_vec && condition_statify == 0; vec_idx++)
                    {
                        vector<INDEX> &inside_list = text_node_storage_list[vec_idx];
                        int inside_size = inside_list.size();

                        for (int b = 8; b < inside_size; b++)
                        {
                            INDEX object_idx = inside_list[b];

                            Node_Text_Attribute object_attribute_text = read_a_Node_Text_Attribute(object_idx);
                            INDEX space_pos = object_attribute_text.idx;

                            if (space_pos < front_boundary && space_pos > back_boundary)
                                continue;

                            if (need_id = General_Node_Storage[object_idx].self_id)
                                match_count++;
                        }
                    }

                    if (condition_statify == 0)
                    {
                        ;
                    }
                }
                else if (text_condition_item.effect_kind == 2)
                {
                    for (INDEX vec_idx = front_vec; vec_idx <= back_vec && condition_statify == 0; vec_idx++)
                    {
                        vector<INDEX> &inside_list = text_node_storage_list[vec_idx];
                        int inside_size = inside_list.size();

                        for (int b = 8; b < inside_size; b++)
                        {
                            INDEX object_idx = inside_list[b];

                            Node_Text_Attribute scan_object_attribute = read_a_Node_Text_Attribute(object_idx);
                            int space_pos = scan_object_attribute.idx;

                            if (space_pos < front_boundary && front_boundary > back_boundary)
                                continue;

                            if (need_id = General_Node_Storage[inside_list[b]].self_id)
                            {
                                match_count += 1;
                                break;
                            }
                        }
                    }

                    if (condition_statify == 0)
                    {
                        ;
                    }
                }
            }
            else if (condition_kind == 5)
            {

                Neuro_Manner_Item manner_result = *(Neuro_Manner_Item *)(ptr + 2 + condition_num * condition_size);

                manner_result.manner_value;

                Focus_Object focus_object;
                focus_object.kind = 2;
                focus_object.target_idx;

                if (manner_result.target_kind == 1)
                {
                    Require_Object require_object;
                    require_object.require_id_or_idx = manner_result.effect_target;
                    require_object.require_kind = manner_result.manner_kind;
                    require_object.require_value = manner_result.manner_value;
                    add_scene_a_local_require_object(text_node_idx, require_object);
                }
                else if (manner_result.target_kind == 2)
                {
                    Require_Object require_object;
                    require_object.require_value = manner_result.manner_value;
                    add_scene_a_local_require_object(text_node_idx, require_object);
                }
            }
        }

        if (match_count == condition_num)
        {
            if (result_kind == 4)
            {
                Neuro_Text_Item result_item = *(Neuro_Text_Item *)(ptr + 3 + attribute_head.condition_num * 2 + 1);

                new_upper_network_node_generate(condition_node_list, 3, scene_idx, 2);
            }
            else if (result_kind == 1)
            {
                ;
            }
            else if (result_kind == 5)
            {
                ;
            }
        }

        // 匹配可能有需求的节点
    }
    else if (attribute_kind == 4)
    {
        ;
    }

    ptr += attribute_head.item_num;
    fea += attribute_head.item_num;
}


inline void Time_Cognition_Generate_Module(
    INDEX curr_node_idx, INDEX scene_idx,
    short &fea, int *&ptr,
    Generate_Permit generate_permit = a_generate_permit)
{
    Attribute_Head_Item attribute_head = attribute_head_read(ptr);
    char attribute_kind = attribute_head.attribute_kind;

    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size = 0;

    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size = 0;

    switch (attribute_kind)
    {
        ;
    }
}


vector<INDEX> A_Neuro_Cognition_Generate(
    INDEX curr_node_idx, INDEX scene_idx,
    char scene_kind, Generate_Permit generate_permit = a_generate_permit)
{
    vector<INDEX> generate_node_idx_list;
    generate_node_idx_list.reserve(32);

    if (curr_node_idx == 0)
        return generate_node_idx_list;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID self_id = curr_node.self_id;

    if (self_id == 0 || curr_node.node_attention < 10)
        return generate_node_idx_list;

    int *first_ptr = node_find(self_id);
    int *ptr = first_ptr;

    
    if (ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(self_id);

        return generate_node_idx_list;
    }

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;

    (first_ptr);


    if (scene_kind == 1)
    {
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            General_Cognition_Generate_Module(curr_node_idx, scene_idx, fea, ptr, generate_permit);
        }
    }

    if (scene_kind == 2)
    {
        float forward_predict_stability = *(first_ptr + 4);
        float backward_predict_stability = *(first_ptr + 5);

        float upward_predict_ability = *(first_ptr + 6);
        float downward_predict_ability = *(first_ptr + 7);
        float left_predict_ability = *(first_ptr + 8);
        float right_predict_ability = *(first_ptr + 9);

        Require_Object require_object;

        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            Image_Cognition_Generate_Module(curr_node_idx, scene_idx, fea, ptr, generate_permit);
        }
    }

    if (scene_kind == 3)
    {
        INDEX pos_idx = curr_node_idx;

        float forward_predict_stability = *(first_ptr + 4);
        float backward_predict_stability = *(first_ptr + 5);

        // 棢�索生戄1�71ￄ1�77 遍历神经元属怄1�71ￄ1�77
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            Text_Cognition_Generate_Module(pos_idx, scene_idx, fea, ptr, generate_permit);
        }
    }

    if (scene_kind == 4)
    {
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            Time_Cognition_Generate_Module(curr_node_idx, scene_idx, fea, ptr, generate_permit);
        }
    }


    return generate_node_idx_list;
}



inline void Image_Imagine_Generate_Module(
    INDEX curr_node_idx, INDEX scene_idx, short &fea, int *&ptr,
    Generate_Permit generate_permit = a_generate_permit)
{
    char condition_image_permit = generate_permit.condition_image_permit;
    char condition_time_permit = generate_permit.condition_time_permit;
    char condition_number_permit = generate_permit.condition_number_permit;
    char condition_text_permit = generate_permit.condition_text_permit;
    char conditon_action_permit = generate_permit.conditon_action_permit;

    char result_image_permit = generate_permit.result_image_permit;
    char result_time_permit = generate_permit.result_time_permit;
    char result_number_permit = generate_permit.result_number_permit;
    char result_text_permit = generate_permit.result_text_permit;
    char result_belief_permit = generate_permit.result_belief_permit;
    char result_manner_permit = generate_permit.result_manner_permit;
    //

    General_Scene &simu_scene = General_Scene_Storage[scene_idx];

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID curr_node_id = curr_node.self_id;

    Node_Image_Attribute node_image_attribute = read_a_Node_Image_Attribute(curr_node_idx);

    int stat_num = *(ptr + 1);
    int realize_num = *(ptr + 2);
    float probability = realize_num / stat_num;

    Attribute_Head_Item attribute_head = attribute_head_read(ptr);

    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size;

    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size;

    switch (attribute_head.attribute_kind)
    {
        case 1:
        {
            if (result_kind == 1 && result_image_permit)
            {
                Neuro_Image_Item image_result = *(Neuro_Image_Item *)(ptr + 2);

                ID generate_id = image_result.related_id;
                char related_scale = image_result.related_scale;
                char distance = image_result.distance;
                char direction = image_result.direction;
                char distance_scale = image_result.distance_scale;
                char direction_scale = image_result.direction_scale;
                char logic = image_result.logic;

                char min_dir = direction - (direction_scale - 1);
                char max_dir = direction + (direction_scale - 1);
                char min_len = distance - (distance_scale - 1);
                char max_len = distance + (distance_scale + 1);

                dir_extend_region(node_image_attribute.x, node_image_attribute.y,
                    min_dir, max_dir, min_len, max_len);

                Neuro_Image_Item simu_image_attri;
                simu_image_attri.direction = direction;
                simu_image_attri.direction_scale = direction_scale;
                simu_image_attri.distance = distance;
                simu_image_attri.distance_scale = distance_scale;
                simu_image_attri.logic = logic;
                simu_image_attri.related_id = generate_id;

                General_Node simu_node;
                simu_node.self_id = generate_id;
                simu_node.complete_or_probability = probability;
                simu_node.node_attention = curr_node.node_attention * probability;

                INDEX simu_idx = create_a_General_Node(simu_node);

                char reserve_direction = simu_image_attri.direction - 16;
                reserve_direction = reserve_direction > 0 ? reserve_direction : reserve_direction + 32;
                simu_image_attri.direction = reserve_direction;

                Variable_Attribute va = {.neuro_image_item = simu_image_attri};
                push_back_a_variable_object_to_General_Node(simu_idx, va);

                push_back_a_node_to_General_Scene(simu_idx, scene_idx);
            }
            else if (result_kind == 2 && result_time_permit)
            {

                Neuro_Time_Item time_result = *(Neuro_Time_Item *)(ptr + 2);

                ID generate_id = time_result.related_id;
                char time_range = time_result.time_left_range;
                char time_scale = time_result.time_scale;

                Neuro_Time_Item simu_time_attri;
                simu_time_attri.time_left_range = time_range;
                simu_time_attri.time_scale = time_scale;
                simu_time_attri.related_id = generate_id;

                General_Node simu_node;
                simu_node.self_id = generate_id;
                simu_node.complete_or_probability = probability;
                simu_node.node_attention = curr_node.node_attention * probability;
            }
            else if (result_kind == 4 && result_number_permit)
            {

                Neuro_Number_Item number_result = *(Neuro_Number_Item *)(ptr + 2);
                int number_I = number_result.number_I;
                int number_II = number_result.number_II;
                char logic = number_result.logic;
                char detail = number_result.detail;

                Neuro_Number_Item simu_num_attri;
                simu_num_attri.number_I = number_I;
                simu_num_attri.number_II = number_II;
                simu_num_attri.logic = logic;
                simu_num_attri.detail = detail;

                General_Node simu_node;
            }
        }
        break;
        case 2:
        {
            if (condition_kind == 1 && result_kind == 1 && condition_image_permit && result_image_permit)
            {
                ID result_generate_id = *(ptr + 3 + condition_size * condition_num + result_size);
            }
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
        case 6:
        {
            if (generate_permit.conditon_action_permit != 1)
                break;
        }
        break;
        case 7:
        {
            Neuro_Manner_Item manner_item = *(Neuro_Manner_Item *)(ptr + 2);

            manner_item.manner_kind;

            Require_Object require_object;
        }
    }

    fea += attribute_head.item_num;
    ptr += attribute_head.item_num;
}


inline void Text_Imagine_Generate_Moudle(INDEX curr_node_idx, INDEX scene_idx,
    short &fea, int *&ptr,
    Generate_Permit simulate_permit = a_generate_permit)
{
    Text_Scene &curr_text_network = Text_Scene_Storage[scene_idx];

    Image_Scene curr_simu_scene;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID curr_node_id = curr_node.self_id;

    Node_Text_Attribute text_attribute = read_a_Node_Text_Attribute(curr_node_idx);

    Attribute_Head_Item attribute_head = attribute_head_read(ptr);

    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size = 0;

    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size = 0;

    ptr + 3;

    switch (attribute_head.attribute_kind)
    {
    case 1:
    {
        return_item_kind_and_size(ptr, result_kind, result_size);
        Neuro_Text_Item text_condition = *(Neuro_Text_Item *)(ptr);

        ID generate_id = text_condition.target_id;

        char logic = text_condition.logic;
        char left_distance = text_condition.left_distance;
        char right_distance = text_condition.right_distance;
        char distance_scale = text_condition.distance_scale;

        General_Node generate_node;
        generate_node.self_id = generate_id;
        generate_node.node_kind = 1;
        generate_node.belong_scene_kind = 3;
        INDEX cre_idx = create_a_General_Node(generate_node);

        Link_Node_Attribute link;

        link.link_idx_or_id = cre_idx;
        link.link_Attribute = 2;
        link.link_Kind = 22;
        Variable_Attribute self_to_generate_link = {.link_node_attribute = link};
        push_back_a_variable_object_to_General_Node(curr_node_idx, self_to_generate_link);

        link.link_idx_or_id = curr_node_idx;
        link.link_Attribute = 2;
        link.link_Kind = 21;
        Variable_Attribute generate_to_self_link = {.link_node_attribute = link};
        push_back_a_variable_object_to_General_Node(cre_idx, generate_to_self_link);
    }
    break;
    case 2:
    {
        return_item_kind_and_size(ptr, condition_kind, condition_size);
        Neuro_Text_Item text_result = *(Neuro_Text_Item *)(ptr);

        ID generate_id = text_result.target_id;

        char left_distance = text_result.left_distance;
        char right_distance = text_result.right_distance;
        char distance_scale = text_result.distance_scale;
    }
    break;
    case 3:
    {
        for (int a = 0; a < result_num; a++)
        {
            return_item_kind_and_size(ptr, result_kind, result_size);
            Neuro_Text_Item text_result = *(Neuro_Text_Item *)(ptr + 2 + a);

            ID generate_id = text_result.target_id;

            char logic = text_result.logic;
            char left_distance = text_result.left_distance;
            char right_distance = text_result.right_distance;
            char distance_scale = text_result.distance_scale;
        }
    }
    break;
    case 4:
    {
        ;
    }
    }
}

inline void Time_Imagine_Generate_Moudle(INDEX curr_node_idx, INDEX scene_idx,
    short &fea, int *&ptr, Generate_Permit simulate_permit = a_generate_permit)
{
    Time_Scene &curr_time_network = Time_Scene_Storage[scene_idx];

    Image_Scene curr_simu_scene;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID curr_node_id = curr_node.self_id;
    Node_Text_Attribute text_attribute = read_a_Node_Text_Attribute(curr_node_idx);

    Attribute_Head_Item attribute_head = attribute_head_read(ptr);
    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size = 0;
    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size = 0;

    ptr += 2;

    switch (attribute_head.attribute_kind)
    {
        case 1:
        {
            Neuro_Time_Item time_result = *(Neuro_Time_Item *)(ptr);

            ID generate_id = time_result.related_id;

            char logic = time_result.logic;
            char time_scale = time_result.time_scale;
            char time_range = time_result.time_left_range;

            General_Node generate_node;
            INDEX cre_idx = create_a_General_Node(generate_node);

            generate_node.self_id = generate_id;
            generate_node.node_kind = 1;
            generate_node.belong_scene_kind = 3;

            Link_Node_Attribute link;

            link.link_idx_or_id = cre_idx;
            link.link_Attribute = 2;
            link.link_Kind = 22;
            Variable_Attribute self_to_generate_link = {.link_node_attribute = link};
            push_back_a_variable_object_to_General_Node(curr_node_idx, self_to_generate_link);

            link.link_idx_or_id = curr_node_idx;
            link.link_Attribute = 2;
            link.link_Kind = 21;
            Variable_Attribute generate_to_self_link = {.link_node_attribute = link};
            push_back_a_variable_object_to_General_Node(cre_idx, generate_to_self_link);
        }
        break;
        case 2:
        {
            for (int a = 0; a < condition_num + result_num; a++)
            {
                return_item_kind_and_size(ptr, condition_kind, condition_size);
                Neuro_Time_Item time_generate = *(Neuro_Time_Item *)(ptr);

                char logic = time_generate.logic;
                char time_range = time_generate.time_left_range;
                char time_scale = time_generate.time_scale;

                General_Node result_node;
                ID generate_id = time_generate.related_id;
                result_node.self_id = generate_id;
            }
        }
        break;
        case 3:
        {
            for (int a = 0; a < result_num; a++)
            {
                return_item_kind_and_size(ptr, result_kind, result_size);
                Neuro_Time_Item time_result = *(Neuro_Time_Item *)(ptr);

                ID generate_id = time_result.related_id;

                char logic = time_result.logic;
                char time_range = time_result.time_left_range;
                char time_scale = time_result.time_scale;
            }
        }
        break;
        case 4:
        {
            ;
        }
    }
}

inline void General_Imagine_Generate_Moudle(INDEX curr_node_idx, INDEX scene_idx,
    short &fea, int *&ptr, Generate_Permit simulate_permit = a_generate_permit)
{
    General_Scene &curr_general_network = General_Scene_Storage[scene_idx];

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID curr_node_id = curr_node.self_id;

    Node_Text_Attribute text_attribute = read_a_Node_Text_Attribute(curr_node_idx);

    Attribute_Head_Item attribute_head = attribute_head_read(ptr);

    char condition_kind;
    char condition_num = attribute_head.condition_num;
    char condition_size = 0;

    char result_kind;
    char result_num = attribute_head.result_num;
    char result_size = 0;

    switch (attribute_head.attribute_kind)
    {
    case 1:
    {
        Neuro_Text_Item text_condition = *(Neuro_Text_Item *)(ptr + 2);

        ID generate_id = text_condition.target_id;

        char logic = text_condition.logic;
        char left_distance = text_condition.left_distance;
        char right_distance = text_condition.right_distance;
        char distance_scale = text_condition.distance_scale;

        
        General_Node generate_node;
        generate_node.self_id = generate_id;
        generate_node.node_kind = 1;
        generate_node.belong_scene_kind = 3;
        INDEX cre_idx = create_a_General_Node();

        Link_Node_Attribute link;

        link.link_idx_or_id = cre_idx;
        link.link_Attribute = 2;
        link.link_Kind = 22;
        Variable_Attribute self_to_generate_link = {.link_node_attribute = link};
        push_back_a_variable_object_to_General_Node(curr_node_idx, self_to_generate_link);

        link.link_idx_or_id = curr_node_idx;
        link.link_Attribute = 2;
        link.link_Kind = 21;
        Variable_Attribute generate_to_self_link = {.link_node_attribute = link};
        push_back_a_variable_object_to_General_Node(cre_idx, generate_to_self_link);
    }
    break;
    case 2:
    {
        Neuro_Text_Item text_result = *(Neuro_Text_Item *)(ptr + 2);

        unsigned int generate_id = text_result.target_id;

        char logic = text_result.logic;
        char left_distance = text_result.left_distance;
        char right_distance = text_result.right_distance;
        char distance_scale = text_result.distance_scale;
    }
    break;
    case 3:
    {
        for (int a = 0; a < result_num; a++)
        {
            Neuro_Text_Item text_result = *(Neuro_Text_Item *)(ptr + 2 + a);

            ID generate_id = text_result.target_id;

            char logic = text_result.logic;
            char left_distance = text_result.left_distance;
            char right_distance = text_result.right_distance;
            char distance_scale = text_result.distance_scale;
        }
    }
    break;
    case 4:
    {
        ;
    }
    }
}


vector<INDEX> A_Neuro_Simulate_Generate(
    INDEX curr_node_idx, INDEX scene_idx,
    char scene_kind,  Generate_Permit simulate_permit = a_generate_permit)
{
    vector<INDEX> generate_node_idx_list; // 生成特征
    generate_node_idx_list.reserve(32);

    // 濢�活拦戄1�71ￄ1�77
    if (curr_node_idx == 0)
        return generate_node_idx_list;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID self_id = curr_node.self_id;

    if (self_id == 0 || curr_node.node_attention < 10)
        return generate_node_idx_list;

    int *first_ptr = node_find(self_id); // 头部位置指针
    int *ptr = first_ptr;

    // 若包被删除，填入待做任务，未完成
    if (ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(self_id);

        return generate_node_idx_list;
    }

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16; // 指针转入特征位置

    // 神经元使用记录加丢�
    *(first_ptr + 1) += 1;

    // 无条件模拟，允许生成含统计概率的模拟节点
    // 含行动模拟，允许额外生成行动节点

    // 1、图像工作节点生戄1�71ￄ1�77
    if (scene_kind == 1)
    {
        // 模拟生成
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            Image_Imagine_Generate_Module(curr_node_idx, scene_idx, fea, ptr, simulate_permit);
        }

        curr_node.was_active = 1;
    }

    // 2、文本工作节点生戄1�71ￄ1�77
    if (scene_kind == 2)
    {
        INDEX pos_idx = curr_node_idx; // 文本时传入位罄1�71ￄ1�77

        // 模拟生成
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            Text_Imagine_Generate_Moudle(pos_idx, scene_idx, fea, ptr);
        }

        curr_node.was_active = 1;
    }

    if (scene_kind == 3)
    {
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            Time_Imagine_Generate_Moudle(curr_node_idx, scene_idx, fea, ptr);
        }

        curr_node.was_active = 1;
    }

    if (scene_kind == 4)
    {
        for (short fea = 0; fea < neuro_head.used_item_num;)
        {
            General_Imagine_Generate_Moudle(curr_node_idx, scene_idx, fea, ptr);
        }

        curr_node.was_active = 1;
    }

    return generate_node_idx_list;
}


inline void Image_Evolve_Generate_Module(Attribute_Head_Item origin_attribute_head, ID curr_node_id,
    int *&ptr, short &fea)
{
    int active_stat_num = *(ptr + 1);
    int succss_realize_num = *(ptr + 2);

    ID result_id = *(ptr + origin_attribute_head.item_num - 1);

    float rate = succss_realize_num / active_stat_num;


    if (rate > 0.5 && origin_attribute_head.attribute_kind == 1 && active_stat_num > 12)
    {
        Attribute_Head_Item new_attribute_head;
        new_attribute_head.attribute_kind = 1;

        Neuro_Image_Item origin_image_condition = *(Neuro_Image_Item *)(ptr + 3);

        char direction_scale = origin_image_condition.direction_scale;
        char distance_scale = origin_image_condition.distance_scale;

        Neuro_Image_Item new_image_result;

        if (direction_scale > 1)
            new_image_result.direction_scale = direction_scale - 1;

        if (distance_scale > 1)
            new_image_result.distance_scale = distance_scale - 1;

        // 标记已升级，后续不可重复升级
        origin_attribute_head.able_update = 2;

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        General_Result new_result = {.image_result = new_image_result};
        general_result_list.push_back(new_result);

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
                                     new_attribute_head, general_condition_list, general_result_list);
    }

    // 图像的精细化丢�般会产生多种条件下的结果，先生成，再根据棢�验剔附1�71ￄ1�77

    // 升级(节点匄1�71ￄ1�77)
    if (rate > 0.8 && origin_attribute_head.attribute_kind != 2 && active_stat_num > 12)
    {
        Attribute_Head_Item new_attribute_head;
        new_attribute_head.attribute_kind = 2;

        Neuro_Image_Item origin_image_condition = *(Neuro_Image_Item *)(ptr + 3);

        char direction_scale = origin_image_condition.direction_scale;
        char distance_scale = origin_image_condition.distance_scale;

        Neuro_Image_Item new_image_condition;

        // 内部方向精确
        if (direction_scale > 1)
            new_image_condition.direction_scale = direction_scale - 1;

        // 内部距离精确
        if (distance_scale > 1)
            new_image_condition.distance_scale = distance_scale - 1;

        // 遍历演化方向
        for (int a = 0; a < 2; a++)
        {
            direction_scale;
            origin_image_condition.direction;

            //
            for (int b = 0; b < 2; b++)
            {
                distance_scale;
                origin_image_condition.distance;
            }
        }

        // 标记已升级，后续不可重复升级
        new_attribute_head.able_update = 2;

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        neuro_item_delete(curr_node_id, fea, origin_attribute_head.item_num);

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
                                     new_attribute_head, general_condition_list, general_result_list);
    }

    // 降级(删除)
    if (rate < 0.1 && origin_attribute_head.attribute_kind == 1 && active_stat_num > 12)
    {
        neuro_item_delete(curr_node_id, fea, origin_attribute_head.item_num);
    }

    // 降级(去节点化)
    if (rate < 0.3 && origin_attribute_head.attribute_kind == 2 && active_stat_num > 12)
    {
        Attribute_Head_Item new_item_head;
        new_item_head.attribute_kind = 2;

        Neuro_Time_Item image_item_II;

        for (int a = 0; a < origin_attribute_head.condition_num; a++)
        {
            image_item_II = *(Neuro_Time_Item *)(ptr + 3 + 3 * a);
        }

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        neuro_item_delete(curr_node_id, fea, new_item_head.item_num);

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
            new_item_head, general_condition_list, general_result_list);
    }
}


inline void Time_Evolve_Generate_Module(Attribute_Head_Item origin_attribute_head, ID curr_node_id,
    int *&ptr, short &fea)
{
    int active_stat_num = *(ptr + 1);
    int succss_realize_num = *(ptr + 2);

    unsigned int result_id = *(ptr + origin_attribute_head.item_num - 1);

    float rate = succss_realize_num / active_stat_num;

    if (rate > 0.5 && origin_attribute_head.attribute_kind == 1)
    {
        Attribute_Head_Item new_attribute_head;
        new_attribute_head.attribute_kind = 1;

        Neuro_Time_Item origin_time_condition;
        origin_time_condition.time_left_range;
        origin_time_condition.time_scale;

        char time_range = origin_time_condition.time_left_range;
        char time_scale = origin_time_condition.time_scale;

        Neuro_Time_Item new_time_condition;

        if (time_range > 1)
            new_time_condition.time_left_range = time_range;

        if (time_scale > 1)
            new_time_condition.time_scale = time_scale - 1;

        new_attribute_head.able_update = 2;

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
            new_attribute_head, general_condition_list, general_result_list);
    }

    if (rate > 0.8 && origin_attribute_head.attribute_kind == 1 && active_stat_num > 20)
    {
        Attribute_Head_Item new_attribute_head;
        new_attribute_head.attribute_kind = 2;

        Neuro_Time_Item origin_time_condition = *(Neuro_Time_Item *)(ptr + 3);

        char time_range = origin_time_condition.time_left_range;
        char time_scale = origin_time_condition.time_scale;

        Neuro_Time_Item new_image_condition;

        // if(direction_scale > 1)
        //     new_image_condition.direction_scale = direction_scale - 1;

        // if(distance_scale > 1)
        //     new_image_condition.distance_scale = distance_scale - 1;

        for (int a = 0; a < 2; a++)
        {
            // direction_scale;
            // origin_time_condition.direction;

            // //
            // for(int b = 0; b < 2; b++)
            // {
            //     distance_scale;
            //     origin_time_condition.distance;
            // }
        }

        new_attribute_head.able_update = 2;

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        neuro_item_delete(curr_node_id, fea, origin_attribute_head.item_num);

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
                                     new_attribute_head, general_condition_list, general_result_list);
    }

    if (rate < 0.4 && origin_attribute_head.attribute_kind == 1 && active_stat_num > 12)
    {
        neuro_item_delete(curr_node_id, fea, origin_attribute_head.item_num);
        ;
    }

    if (rate < 0.4 && origin_attribute_head.attribute_kind == 2 && active_stat_num > 30)
    {
        Attribute_Head_Item new_item_head;
        new_item_head.attribute_kind = 2;

        Neuro_Time_Item time_condition;

        for (int a = 0; a < origin_attribute_head.condition_num; a++)
        {
            time_condition = *(Neuro_Time_Item *)(ptr + 3 + 3 * a);
        }

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        neuro_item_delete(curr_node_id, fea, new_item_head.item_num);

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
            new_item_head, general_condition_list, general_result_list);
    }
}


inline void Text_Evolve_Generate_Module(Attribute_Head_Item origin_attribute_head, ID curr_node_id,
    int *&ptr, short &fea)
{
    int active_stat_num = *(ptr + 1);    // 濢�活��统计的次数
    int succss_realize_num = *(ptr + 2); // 生成、实现的次数

    ID result_id = *(ptr + origin_attribute_head.item_num - 1);

    float rate = succss_realize_num / active_stat_num;

    // 升级(精细匄1�71ￄ1�77)
    if (rate > 0.5 && origin_attribute_head.attribute_kind == 1)
    {
        Attribute_Head_Item new_attribute_head;
        new_attribute_head.attribute_kind = 1;

        Neuro_Text_Item text_item = *(Neuro_Text_Item *)(ptr + 3);

        char distance_scale = text_item.distance_scale;

        Neuro_Text_Item new_text_item;

        if (distance_scale > 1)
            new_text_item.distance_scale = distance_scale - 1;

        // 生成多种条目结果

        // 标记已升级，后续不可升级
        new_attribute_head.able_update = 2;

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
                                     new_attribute_head, general_condition_list, general_result_list);
    }

    // 降级(删除)
    if (rate < 0.1 && origin_attribute_head.attribute_kind == 1)
    {
        neuro_item_delete(curr_node_id, fea, origin_attribute_head.item_num);
    }

    // 降级(去节点化)
    if (rate < 0.3 && origin_attribute_head.attribute_kind == 2)
    {
        Attribute_Head_Item new_item_head;
        new_item_head.attribute_kind = 1;

        vector<General_Condition> general_condition_list;
        vector<General_Result> general_result_list;

        // 原条件其实不用改，但没写别的写入函数
        Neuro_Text_Item origin_text_condition;

        for (int a = 0; a < origin_attribute_head.condition_num; a++)
        {
            origin_text_condition = *(Neuro_Text_Item *)(ptr + 3 + 3 * a);
            General_Condition condition = {.text_condition = origin_text_condition};
            general_condition_list.push_back(condition);
        }

        ID delete_id = *(ptr + origin_attribute_head.item_num - 1);

        neuro_item_delete(curr_node_id, fea, new_item_head.item_num);

        Simple_Neuro_Attribute_write(curr_node_id, 0, 1,
                                     new_item_head, general_condition_list, general_result_list);

        neuro_delete(delete_id);
    }
}


void Identity_Belief_Evolve_Module(Attribute_Head_Item origin_attribute_head, ID curr_node_id,
    int *&ptr, short &fea)
{
    Neuro_Belief_Item identity_belief_attribute;
}


void Number_Evolve_Module(Attribute_Head_Item origin_attribute_head, ID curr_node_id,
    int *&ptr, short &fea)
{
    Neuro_Number_Item number_attribute;
}


void A_Neuro_Basic_Evolve(INDEX curr_node_idx, char scene_kind /*1 2 3 4*/)
{
    if (curr_node_idx == 0)
        return;

    General_Node curr_node = read_a_General_Node(curr_node_idx);
    ID self_id = curr_node.self_id;

    if (self_id == 0 || curr_node.node_attention < 10)
        return;

    int *first_ptr = node_find(self_id);
    int *ptr = first_ptr;

    
    if (ptr == 0)
    {
        add_to_Will_Read_Neuro_Queue(self_id);

        return;
    }

    Neuro_Head_Item neuro_head = neuro_head_read(*ptr);
    ptr += 16;


    (first_ptr);

    float forward_predict_stability = 0;
    float backward_predict_stability = 0;

    float upward_predict_ability = 0;
    float downward_predict_ability = 0;
    float left_predict_ability = 0;
    float right_predict_ability = 0;

    if (scene_kind == 1)
    {
        float forward_predict_stability = *(first_ptr + 4);
        float backward_predict_stability = *(first_ptr + 5);

        float upward_predict_ability = *(first_ptr + 6);
        float downward_predict_ability = *(first_ptr + 7);
        float left_predict_ability = *(first_ptr + 8);
        float right_predict_ability = *(first_ptr + 9);
    }

    if (scene_kind == 2)
    {
        float forward_predict_stability = *(first_ptr + 4);
        float backward_predict_stability = *(first_ptr + 5);
    }

    
    for (short fea = 0; fea < neuro_head.used_item_num; fea++)
    {
        Attribute_Head_Item origin_attribute_head = attribute_head_read(first_ptr);

        
        if (origin_attribute_head.able_update = 0)
        {
            first_ptr += origin_attribute_head.item_num;
            fea += origin_attribute_head.item_num;
            continue;
        }

        char attribute_kind = origin_attribute_head.attribute_kind;


        if (scene_kind == 1)
        {
            Image_Evolve_Generate_Module(origin_attribute_head, self_id,
                ptr, fea);
        }
        else if (scene_kind == 2)
        {
            Text_Evolve_Generate_Module(origin_attribute_head, self_id,
                ptr, fea);
        }
        else if (scene_kind == 3)
        {
            Time_Evolve_Generate_Module(origin_attribute_head, self_id,
                ptr, fea);
        }

        first_ptr += origin_attribute_head.item_num;
        fea += origin_attribute_head.item_num;

    }
}


struct Line_Object_Feature
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    unsigned char direction;
    unsigned char length;
};


vector<ID> COLOUR_LINE_ENTER_SORT; // num = 8*8*8*16*16

vector<ID> LINE_FORM_ENTER_SORT; // num = 16*16

vector<ID> COLOUR_BLOCK_ENTER_SORT; // num = 8*8*8


void load_image_enter_line()
{
    COLOUR_LINE_ENTER_SORT.resize(8 * 8 * 8 * 16 * 16);
    LINE_FORM_ENTER_SORT.resize(16 * 16);
    COLOUR_BLOCK_ENTER_SORT.resize(8 * 8 * 8);

    FILE *fp = fopen("Record_enter_line.rec", "r");
    if (fp)
    {
        fread(COLOUR_LINE_ENTER_SORT.data(), (8 * 8 * 8 * 16 * 16) * 4, 1, fp);

        fseek(fp, (8 * 8 * 8 * 16 * 16) * 4, 1);
        fread(LINE_FORM_ENTER_SORT.data(), (16 * 16) * 4, 1, fp);

        fseek(fp, (16 * 16) * 4, 1);
        fread(COLOUR_BLOCK_ENTER_SORT.data(), (8 * 8 * 8) * 4, 1, fp);

        fclose(fp);
    }
}


void save_image_enter_line()
{
    FILE *fp = fopen("Record_enter_line.rec", "w");
    fwrite(COLOUR_LINE_ENTER_SORT.data(), (8 * 8 * 8 * 16 * 16) * 4, 1, fp);

    fseek(fp, (8 * 8 * 8 * 16 * 16) * 4, 1);
    fwrite(LINE_FORM_ENTER_SORT.data(), (16 * 16) * 4, 1, fp);

    fseek(fp, (16 * 16) * 4, 1);
    fwrite(COLOUR_BLOCK_ENTER_SORT.data(), (8 * 8 * 8) * 4, 1, fp);

    fclose(fp);
}


ID new_colour_line_record(Line_Object curr_line)
{
    Neuro_Image_Desc image_desc;
    Neuro_Desc neuro_desc = {.image_desc = image_desc};

    ID id = new_neuro_create(1, neuro_desc);

    COLOUR_LINE_ENTER_SORT[curr_line.red * 8 * 8 * 16 * 16 + curr_line.green * 8 * 16 * 16 +
        curr_line.blue * 16 * 16 + curr_line.length * 16 + curr_line.direction] = id;

    int *ptr = node_find(id);

    return id;

}

ID new_form_line_record(char length, char direction)
{
    Neuro_Image_Desc image_desc;
    Neuro_Desc neuro_desc = {.image_desc = image_desc};

    ID id = new_neuro_create(1, neuro_desc);
    LINE_FORM_ENTER_SORT[length * 16 + direction] = id;

    int *ptr = node_find(id);

    return id;

}

ID new_colour_block_record(char red, char green, char blue)
{
    Neuro_Image_Desc image_desc;
    Neuro_Desc neuro_desc = {.image_desc = image_desc};

    ID id = new_neuro_create(1, neuro_desc);
    COLOUR_BLOCK_ENTER_SORT[red*8*8 + green*8 + blue] = id;

    int *ptr = node_find(id);

    Attribute_Head_Item attribute_head;
    vector<General_Condition> general_condition_list;
    vector<General_Result> general_result_list;

    Neuro_Manner_Item item_require;
    Neuro_Image_Item item_space;

    for(int p = 0; p < 8; p += 2)
    {
        attribute_head.attribute_kind = 2;
        item_space.direction = p;
        item_require.manner_kind = 10;

        Simple_Neuro_Attribute_write(id, 0, 0, 
            attribute_head, general_condition_list, general_result_list);
    }


    return id;
}


void image_a_line_retrieve_generate(
    INDEX image_scene_idx, INDEX rough_idx,
    Line_Object &line_object)
{
    Image_Scene &image_scene = Image_Scene_Storage[image_scene_idx];
    Image_Rough_Map &Space_rough_view = image_scene.Image_rough_view[rough_idx];
    unsigned short width = Space_rough_view.rough_width;

    General_Node add_node_c;
    General_Node add_node_f;

    int colour_idx = line_object.red * 8 * 8 * 16 * 16 + line_object.green * 8 * 16 * 16 + line_object.blue * 16 * 16 + line_object.length * 16 + line_object.direction;

    int form_idx = line_object.length * 16 + line_object.direction;

    add_node_c.self_id = COLOUR_LINE_ENTER_SORT[colour_idx];
    add_node_f.self_id = LINE_FORM_ENTER_SORT[form_idx];

    // 若该条边未记彄1�71ￄ1�77
    if (add_node_c.self_id == 0)
    {
        new_colour_line_record(line_object); // 进行记录

        // 若该形��未记录
        if (add_node_f.self_id == 0)
        {
            new_form_line_record(line_object.length, line_object.direction); // 进行记录
        }
    }

    // 未存储内存，装入读取任务队列
    if (pack_index_find(add_node_c.self_id) == 0)
    {
        add_to_Will_Read_Neuro_Queue(add_node_c.self_id);
        // 加入未读取，准备后续查找
    }

    // 未存储内存，装入读取任务队列
    if (pack_index_find(add_node_f.self_id) == 0)
    {
        add_to_Will_Read_Neuro_Queue(add_node_f.self_id);
    }

    // 线段数据写入
    Node_Image_Attribute node_image_attribute;
    node_image_attribute.block_num = line_object.qualify_block_num;
    // node_image_attribute.x = line_object.middle_coordinate.x;
    // node_image_attribute.y = line_object.middle_coordinate.y;

    // 线段点添劄1�71ￄ1�77
    INDEX add_node_c_idx = create_a_General_Node(add_node_c);
    INDEX add_node_f_idx = create_a_General_Node(add_node_f);

    // 创建图像数据环节
    create_a_Node_Image_Attribute(add_node_c_idx, node_image_attribute);
    create_a_Node_Image_Attribute(add_node_f_idx, node_image_attribute);

    image_scene.Id_find_overall_node[add_node_c.self_id].push_back(add_node_c_idx);
    image_scene.Id_find_overall_node[add_node_f.self_id].push_back(add_node_f_idx);

    int require_idx = 0;
    int occupy_number = 1;

    while (occupy_number <= line_object.qualify_block_num && require_idx < 5)
    {
        occupy_number *= 2;
        require_idx += 1;
    }

    // Space_Require_Map& Storage_View = top_map.Space_retrieve_require_view[require_idx];

    // int Retrieval_distance = Storage_View.record_distance;
    // int general_idx = Line.middle_coordinate.y / Retrieval_distance*width +
    // Line.middle_coordinate.x / Retrieval_distance;

    // Storage_View.Node_space_record[general_idx].push_back(add_node_c_idx);
    // Storage_View.Node_space_record[general_idx].push_back(add_node_f_idx);
}


void image_nature_attention_generate(
    INDEX last_image_idx, INDEX current_image_idx)
{
    Image_Scene &last_image = Image_Scene_Storage[last_image_idx];
    Image_Scene &current_image = Image_Scene_Storage[current_image_idx];

    current_image.Block_attention_list.reserve(5);

    unsigned short curr_width = current_image.width;
    unsigned short curr_height = current_image.height;

    Rigion_Value_Stat basic;
    basic.width_unit_size = 1;
    basic.width = curr_width;
    basic.height_unit_size = 1;
    basic.height = curr_height;
    basic.value_unit_list.resize(curr_width * curr_height);

    auto *last_rgb_ptr = last_image.RGB_Map.data();
    auto *curr_rgb_ptr = current_image.RGB_Map.data();

    auto *last_att_ptr = last_image.Block_attention_list[0].value_unit_list.data();
    auto *curr_att_ptr = basic.value_unit_list.data();
    int total_att = 0;

    if (current_image.is_lock_occupy)
        return;

    for (int n = 0; n < last_image.RGB_Map.size(); n++)
    {
        int att = last_att_ptr[n];

        int diff_r = last_rgb_ptr[n].r - curr_rgb_ptr[n].r;
        int diff_g = last_rgb_ptr[n].g - curr_rgb_ptr[n].g;
        int diff_b = last_rgb_ptr[n].b - curr_rgb_ptr[n].b;

        diff_r = (diff_r >= 0 ? diff_r : -diff_r);
        diff_g = (diff_g >= 0 ? diff_g : -diff_g);
        diff_b = (diff_b >= 0 ? diff_b : -diff_b);

        int diff_strength = (diff_r + diff_g + diff_b) / 16; // 0~45
        att = att / 2 + diff_strength;

        total_att += att;
        curr_att_ptr[n] = att;
    }

    current_image.sum_nature_attention = total_att;
    current_image.Block_attention_list.push_back(basic);

    vector<unsigned short> width_s(5);
    vector<unsigned short> height_s(5);

    width_s[0] = curr_width;
    height_s[0] = curr_height;

    char count_a = 1;

    while (curr_width > 1920 || curr_height > 1080)
    {
        ;
    }

    // 1920*1080 960*540 480*270 240*135
    while (curr_width > 480 && curr_height > 270 && count_a < 5)
    {
        curr_height = curr_height / 2;
        curr_width = curr_width / 2;
        width_s[count_a] = curr_width;
        height_s[count_a] = curr_height;
        count_a += 1;
        current_image;
    }

    
    for (int a = 1; a < width_s.size(); a++)
    {
        Rigion_Value_Stat rvs;
        rvs.width = width_s[a];
        rvs.width_unit_size = current_image.width / rvs.width;
        rvs.height = height_s[a];
        rvs.height_unit_size = current_image.height / rvs.height;
        rvs.value_unit_list.resize(rvs.width * rvs.height);

        current_image.Block_attention_list.push_back(rvs);

        Rigion_Value_Stat &target_area = current_image.Block_attention_list[a];
        target_area.value_unit_list.resize(target_area.width * target_area.height);
        Rigion_Value_Stat &origin_area = current_image.Block_attention_list[a - 1];

        auto &origin_unit_attention_list = target_area.value_unit_list;
        auto &target_unit_attention_list = origin_area.value_unit_list;
        int origin_width = origin_area.width;
        int target_width = target_area.width;

        int origin_idx = 0;
        int target_idx = 0;
        int origin_y = 0;
        int target_x = 0;

        int target_unit_num = target_area.value_unit_list.size();
        int size_occupy_height = target_area.height_unit_size / origin_area.height_unit_size;
        int size_occupy_width = target_area.width_unit_size / origin_area.width_unit_size;
        int gap_idx = size_occupy_height * origin_width;

        int origin_left_idx = 0;
        int origin_const_left_idx = 0;
        int origin_right_idx;
        int origin_top = size_occupy_height;

        while (target_idx < target_unit_num)
        {
            origin_left_idx = origin_const_left_idx;
            origin_right_idx = origin_left_idx + size_occupy_width;

            origin_idx = origin_left_idx;

            while (origin_y <= origin_top)
            {
                origin_unit_attention_list[target_idx] += target_unit_attention_list[origin_idx];

                origin_idx += 1;

                if (origin_idx == origin_right_idx)
                {
                    origin_y += 1;
                    origin_left_idx += origin_width;
                    origin_idx = origin_left_idx;
                    origin_right_idx += origin_width;
                }
            }

            target_idx += 1;
            target_x += 1;
            origin_const_left_idx += size_occupy_width;

            if (target_x == target_width)
            {
                origin_const_left_idx += gap_idx;
                origin_top += size_occupy_height;
                target_x = 0;
            }

        }

    }


    vector<RGB_Unit> &current_rgb_map = current_image.RGB_Map;

    int current_image_size = current_image.width * current_image.height;

    vector<Image_Colour_Block>& current_Colour_block_list = 
        current_image.Image_rough_view[0].Colour_block_list;
    current_Colour_block_list.resize(current_image_size);

    int origin_pos = 0;

    for (int y = 0; y < curr_height; y++)
    {
        for (int x = 0; x < curr_width; x++)
        {
            current_Colour_block_list[origin_pos].ave_r = current_rgb_map[origin_pos].r;
            current_Colour_block_list[origin_pos].ave_g = current_rgb_map[origin_pos].r;
            current_Colour_block_list[origin_pos].ave_b = current_rgb_map[origin_pos].r;
        }
    }

    origin_pos = 0;

    for (int y = 0; y < curr_height; y++)
    {
        for (int x = 0; x < curr_width; x++)
        {
            unsigned char r, g, b;
            auto &current_block = current_Colour_block_list[origin_pos];

            r = current_block.ave_r;
            g = current_block.ave_g;
            b = current_block.ave_b;

            auto &diff_colour = current_block.diff_colour_block;
            auto &proc_stage = current_block.block_kind;

            if (x > 0)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos - 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 4);
                    proc_stage = 2;
                }
            }

            if (x < curr_width - 1)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos + 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 0);
                    proc_stage = 2;
                }
            }

            if (y > 0)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos - curr_width];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 2);
                    proc_stage = 2;
                }
            }

            if (y < curr_height - 1)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos + curr_width];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 6);
                    proc_stage = 2;
                }
            }

            if (x > 0 && y > 0)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos - curr_width - 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 3);
                    proc_stage = 2;
                }
            }

            if (x < curr_width - 1 && y > 0)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos - curr_width + 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 1);
                    proc_stage = 2;
                }
            }

            if (x > 0 && y < curr_height - 1)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos + curr_width - 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 5);
                    proc_stage = 2;
                }
            }

            if (x < curr_width - 1 && y < curr_height - 1)
            {
                auto neighbor_block = current_Colour_block_list[origin_pos + curr_width + 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 7);
                    proc_stage = 2;
                }
            }

            origin_pos += 1;

        }
    }

}


void image_rough_map_generate(
    INDEX current_Map_idx, char standard_size, int require_base_value = 1)
{
    Image_Scene &curr_image = Image_Scene_Storage[current_Map_idx];

    // 1、生成粗粒化视图

    // 选取原图
    unsigned char suit_one = standard_size / 2;
    char choose_idx = -1;
    char quit = 0;
    int history_choose;

    for (int i = 0; i < 5; i++)
    {
        int history_diff;
        int diff = standard_size - curr_image.Image_rough_view[i].rough_size;

        if (diff >= suit_one)
        {
            if (diff < history_diff)
            {
                history_diff = diff;
                choose_idx = i;
            }

            if (diff == suit_one)
                choose_idx = i;
        }
        else if (diff == 0)
            quit = 1;
    }

    if (quit = 1)
        return;

    unsigned short from_width = curr_image.Image_rough_view[choose_idx].rough_width;
    unsigned short from_height = curr_image.Image_rough_view[choose_idx].rough_height;
    vector<Image_Colour_Block> from_Colour_block_list = curr_image.Image_rough_view[choose_idx].Colour_block_list;


    unsigned short origin_width = curr_image.width;
    unsigned short origin_height = curr_image.height;

    Image_Rough_Map generate_view;
    generate_view.rough_size = standard_size;
    generate_view.colour_number_distribution.assign(512, 0); // 8*8*8
    vector<int> &colour_number_distribution = generate_view.colour_number_distribution;

    int target_width = origin_width / standard_size;
    int target_height = origin_height / standard_size;
    generate_view.rough_width = target_width;
    generate_view.rough_height = target_height;

    generate_view.Colour_block_list.resize(origin_width * origin_height);

    auto &Cover_Block_List = generate_view.Colour_block_list;

    long size = generate_view.rough_width * generate_view.rough_height;


    int origin_idx = 0;
    int target_idx = 0;
    int origin_y = 0;
    int target_x = 0;

    int target_unit_num = Cover_Block_List.size();
    int size_occupy_height = from_height / target_height;
    int size_occupy_width = from_width / target_width;
    int gap_idx = size_occupy_height * origin_width;

    int origin_left_idx = 0;
    int origin_const_left_idx = 0;
    int origin_right_idx;
    int origin_top = size_occupy_height;

    int sum_r = 0;
    int sum_g = 0;
    int sum_b = 0;
    unsigned char ave_r;
    unsigned char ave_g;
    unsigned char ave_b;

    int size_block_num = size_occupy_height * size_occupy_width;

    while (target_idx < target_unit_num)
    {
        origin_left_idx = origin_const_left_idx;
        origin_right_idx = origin_left_idx + size_occupy_width;

        origin_idx = origin_left_idx;

        while (origin_y <= origin_top)
        {
            Image_Colour_Block &origin_block = from_Colour_block_list[origin_idx];

            
            sum_b += origin_block.ave_b;
            sum_g += origin_block.ave_g;
            sum_r += origin_block.ave_r;

            origin_idx += 1;

            if (origin_idx == origin_right_idx)
            {
                origin_y += 1;
                origin_left_idx += origin_width;
                origin_idx = origin_left_idx;
                origin_right_idx += origin_width;
            }
        }

        
        auto &target_block = Cover_Block_List[target_idx];
        ave_r = sum_r / size_block_num;
        ave_g = sum_g / size_block_num;
        ave_b = sum_b / size_block_num;
        target_block.ave_r = ave_r;
        target_block.ave_g = ave_g;
        target_block.ave_b = ave_b;

        colour_number_distribution[ave_r * 64 + ave_g * 8 + ave_b] += 1;

        
        target_idx += 1;
        target_x += 1;
        origin_const_left_idx += size_occupy_width;

        sum_b = 0;
        sum_g = 0;
        sum_r = 0;

        if (target_x == from_width)
        {
            origin_const_left_idx += gap_idx;
            origin_top += size_occupy_height;
            target_x = 0;
        }

    }


    INDEX att_idx = 0;

    while (curr_image.Block_attention_list[att_idx].height_unit_size != standard_size &&
           curr_image.Block_attention_list[att_idx].width_unit_size != standard_size)
    {
        att_idx++;

        if (curr_image.Block_attention_list.size() <= att_idx)
            ;
    }

    vector<int> &attention_unit_list = curr_image.Block_attention_list[att_idx].value_unit_list;

    int origin_pos = 0;

    for (int y = 0; y < target_height; y++)
    {
        for (int x = 0; x < target_width; x++)
        {
            unsigned char r, g, b;
            auto &origin_block = Cover_Block_List[origin_pos];

            r = origin_block.ave_r;
            g = origin_block.ave_g;
            b = origin_block.ave_b;

            auto &diff_colour = origin_block.diff_colour_block;
            auto &block_kind = origin_block.block_kind;
            auto &block_attention = attention_unit_list[origin_pos];

            if (x > 0)
            {
                auto neighbor_block = Cover_Block_List[origin_pos - 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 4);
                    block_kind = 2;
                    block_attention += 15;
                }
            }

            if (x < target_width - 1)
            {
                auto neighbor_block = Cover_Block_List[origin_pos + 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 0);
                    block_kind = 2;
                    block_attention += 15;
                }
            }

            if (y > 0)
            {
                auto neighbor_block = Cover_Block_List[origin_pos - target_width];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 2);
                    block_kind = 2;
                    block_attention += 15;
                }
            }

            if (y < target_height - 1)
            {
                auto neighbor_block = Cover_Block_List[origin_pos + target_width];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 6);
                    block_kind = 2;
                    block_attention += 15;
                }
            }

            
            if (x > 0 && y > 0)
            {
                auto neighbor_block = Cover_Block_List[origin_pos - target_width - 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 3);
                    block_kind = 2;
                    block_attention += 10;
                }
            }

            
            if (x < target_width - 1 && y > 0)
            {
                auto neighbor_block = Cover_Block_List[origin_pos - target_width + 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 1);
                    block_kind = 2;
                    block_attention += 10;
                }
            }

            
            if (x > 0 && y < target_height - 1)
            {
                auto neighbor_block = Cover_Block_List[origin_pos + target_width - 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 5);
                    block_kind = 2;
                    block_attention += 10;
                }
            }

            
            if (x < target_width - 1 && y < target_height - 1)
            {
                auto neighbor_block = Cover_Block_List[origin_pos + target_width + 1];

                if (abs(r - neighbor_block.ave_r) > 32 || abs(g - neighbor_block.ave_g) > 32 || abs(b - neighbor_block.ave_b) > 32)
                {
                    diff_colour |= (1 << 7);
                    block_kind = 2;
                    block_attention += 10;
                }
            }

            origin_pos += 1;

        }
    }

    generate_view.nave_init = 1;


    curr_image.Image_rough_view[curr_image.Image_rough_view_number] = generate_view;
    curr_image.Image_rough_view_number++;

}

void init_image_require(
    INDEX current_image_idx, vector<vector<int>> &require_list,
    char value_give_kind = 1, char base_value = 10)
{
    Image_Scene &curr_map = Image_Scene_Storage[current_image_idx];
    int height = curr_map.height;
    int width = curr_map.width;

    int need_height_size = height;
    int need_width_size = width;

    int need_height_layer = 0;
    int need_width_layer = 0;

    while (need_height_size >= 8) // 计算扢�霢�层数
    {
        need_height_size /= 8;
        need_height_layer += 1;
    }

    while (need_width_size >= 8) // 计算扢�霢�层数
    {
        need_width_size /= 8;
        need_width_layer += 1;
    }

    bool high_or_width_bigger = 0;
    int chose_calcul_value;
    int need_layer;

    if (need_height_layer > need_width_layer)
    {
        require_list.resize(need_height_layer);
        high_or_width_bigger = 1;
        need_layer = need_height_layer;
        chose_calcul_value = need_height_size;
    }
    else
    {
        require_list.resize(need_width_layer);
        high_or_width_bigger = 2;
        need_layer = need_width_layer;
        chose_calcul_value = need_width_size;
    }

    // 填入基础倄1�71ￄ1�77 复制注意劄1�71ￄ1�77
    if (value_give_kind == 1)
    {
        require_list[0].resize(width * height);
        require_list[0] = curr_map.Block_attention_list[0].value_unit_list;
    }
    else
    {
        require_list[0].assign(width * height, base_value);
    }

    int origin_width = width;
    int origin_height = height;
    int target_width = width;
    int target_height = height;

    int target_stat_idx = 0;

    int width_more_value = 0;
    int height_more_value = 0;

    int origin_width_limit;
    int origin_heigth_limit;

    bool have_right_more = 0;
    bool have_top_more = 0;

    // 处理汇�ￄ1�71ￄ1�77
    while (target_stat_idx < need_layer)
    {
        chose_calcul_value /= 8;

        target_stat_idx += 1;
        origin_width = target_width;

        if (target_width >= 8)
        {
            width_more_value = target_width % 8;
            if (width_more_value > 0)
            {
                target_width /= 8;
                origin_width_limit = target_width * 8;
                target_width += 1; // 余数处理
            }
            else
                target_width /= 8;
        }
        else
            target_width = 1;

        origin_height = target_height;
        if (target_height >= 8)
        {
            height_more_value = target_height % 8;
            if (height_more_value > 0)
            {
                target_height /= 8;
                origin_heigth_limit = target_height * 8;
                target_height += 1;
            }
            else
                target_height /= 8;
        }
        else
            target_height = 1;

        vector<int> &target_stat = require_list[target_stat_idx];

        target_stat.resize(target_width * target_height);
        vector<int> &origin_stat = require_list[target_stat_idx - 1];

        // 将原坐标值映射到新坐栄1�71ￄ1�77
        int origin_idx = 0;
        int target_idx = 0;
        int origin_y = 0;
        int origin_x = 0;
        int target_x = 0;

        int target_unit_num = target_stat.size();
        int size_occupy_height = 8;
        int size_occupy_width = 8;
        int gap_idx = size_occupy_height * origin_width;

        int origin_left_idx = 0;
        int origin_const_left_idx = 0;
        int origin_right_idx;
        int origin_top = size_occupy_height;

        // 将原坐标值映射到新坐栄1�71ￄ1�77
        while (target_idx < target_unit_num)
        {
            origin_left_idx = origin_const_left_idx;

            if (width_more_value && origin_x == origin_width_limit) // 右界
            {
                origin_right_idx = origin_left_idx + width_more_value;
                have_right_more = 1;
            }
            else
                origin_right_idx = origin_left_idx + size_occupy_width;

            origin_idx = origin_left_idx;

            // 遍历本块
            if (have_right_more == 0 && have_top_more == 0) // 完整坄1�71ￄ1�77
            {
                while (origin_y <= origin_top)
                {
                    // 棢�索力求和
                    target_stat[target_idx] += origin_stat[origin_idx];

                    origin_idx += 1;

                    if (origin_idx == origin_right_idx)
                    {
                        origin_y += 1;
                        origin_left_idx += origin_width;
                        origin_idx = origin_left_idx;
                        origin_right_idx += origin_width;
                    }
                }
            }
            else
            { // 余数坄1�71ￄ1�77

                while (origin_y <= origin_top)
                {
                    // 棢�索需求求咄1�71ￄ1�77
                    target_stat[target_idx] += origin_stat[origin_idx];

                    origin_idx += 1;

                    if (origin_idx == origin_right_idx)
                    {
                        if (have_right_more == 0)
                        {
                            origin_y += 1;
                            origin_left_idx += origin_width;
                            origin_idx = origin_left_idx;
                            origin_right_idx += width_more_value; // 改动
                        }
                    }
                }
            }

            // 选择下一个块
            target_idx += 1;
            target_x += 1;
            origin_const_left_idx += size_occupy_width;
            have_right_more = 0;

            if (target_x == target_width)
            {
                origin_const_left_idx += gap_idx;

                if (width_more_value && origin_top == origin_heigth_limit)
                {
                    origin_top += height_more_value;
                    have_top_more = 1;
                }
                else
                    origin_top += size_occupy_height;

                target_x = 0;
            }

        } // 丢�层需求遍历完戄1�71ￄ1�77

    } // 霢�求汇总生成完毄1�71ￄ1�77

    // 计算总需求度
    char max_idx = require_list.size() - 1;
    auto &max_retrieval_require_list = require_list[max_idx];
    int max_size = require_list[max_idx].size();
    int total_retrieval_require = 0;

    for (int a = 0; a < max_size; a++)
        total_retrieval_require += max_retrieval_require_list[a];

    curr_map.sum_retrieve_require = total_retrieval_require;
}


void line_appraise(
    Line_Object &curr_line)
{
    int point_number = curr_line.Inside_Point.size();

    Point_2d begin_point = curr_line.base_point;

    float ave_x = (curr_line.x_sum - begin_point.x * point_number) / point_number; // x均�ￄ1�71ￄ1�77
    float ave_y = (curr_line.y_sum - begin_point.y * point_number) / point_number; // y均�ￄ1�71ￄ1�77 相对于初始坐标的误差

    int dx = (begin_point.x - ave_x) * 2;
    int dy = (begin_point.y - ave_y) * 2;

    char gradient = fast_dir(ave_x - begin_point.x, ave_y - begin_point.y);

    curr_line.direction = gradient;
    curr_line.length = sqrt(dx * dx + dy * dy);
    curr_line.qualify_block_num = point_number;

    float abs_diff_value = 0; // 误差釄1�71ￄ1�77
    float sum_diff_value = 0;

    // 计算拟合奖励玄1�71ￄ1�77
    if (ave_y != 0)
    {
        float slope = ave_y / ave_x;

        for (int i = 0; i < point_number; i++)
        {
            //point_2d this_point = curr_line.Inside_Point[i];
            float dif_value ;//= this_point.y - slope * this_point.x;
            if (dif_value > 0.6)
            {
                abs_diff_value += dif_value;
                sum_diff_value += dif_value;
            }
            if (dif_value < -0.6)
            {
                abs_diff_value += -dif_value;
                sum_diff_value += dif_value;
            }
        }
    }
    else
    { // 防止slope丄1�71ￄ1�770

        for (int i = 0; i < point_number; i++)
        {
            //point_2d this_point = curr_line.Inside_Point[i];
            float dif_value ;//= this_point.y - ave_y;
            if (dif_value > 0.6)
            {
                abs_diff_value += dif_value;
                sum_diff_value += dif_value;
            }
            if (dif_value < -0.6)
            {
                abs_diff_value += -dif_value;
                sum_diff_value += dif_value;
            }
        }
    }

    char reward_level;

    if (gradient % 4 == 0)
        reward_level = 10;
    else if (gradient % 4 == 2)
        reward_level = 9;
    else if (gradient % 4 == 1 || gradient % 4 == 3)
        reward_level = 8;

    curr_line.abs_diff_value = abs_diff_value;
    curr_line.ave_diff_rate = abs_diff_value / point_number;
    curr_line.reward_rate = (1 - (curr_line.ave_diff_rate)) * reward_level;
    curr_line.reward_value = curr_line.reward_rate * curr_line.qualify_block_num;
}



float line_similarity_dectect(Line_Object_Feature a, Line_Object_Feature b)
{
    float similarity = (abs(a.red - b.red) + abs(a.green - b.green) + abs(a.blue - b.blue) // 0~21
        + abs(a.direction - b.direction) + abs(a.length - b.length));      // 0~30

    similarity = 1 - (similarity / 51);

    return similarity;
}


inline void line_single_extend(
    Line_Object& line_object, INDEX image_scene_idx, INDEX rough_idx, int x0, int y0,
    vector<Point_2d> &return_result, vector<INDEX> &return_index, vector<char> &return_direction)
{
    Image_Scene &curr_image = Image_Scene_Storage[rough_idx];
    Image_Rough_Map &rough_map = curr_image.Image_rough_view[image_scene_idx];

    int width = rough_map.rough_width;
    int height = rough_map.rough_height;
    int origin_idx = x0 + y0 * width;
    vector<Image_Colour_Block> &Cover_block_list = rough_map.Colour_block_list;
    return_result.clear();
    return_index.clear();
    return_direction.clear();

    INDEX target_index;

    if (x0 > 0)
    {
        target_index = origin_idx - 1;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Target_Block.block_kind == 2 && ((Target_Block.diff_colour_block >> 4) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = (x0 - 1);
            add_point.y = y0;
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }


    if (y0 > 0)
    {
        target_index = origin_idx - width;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Cover_block_list[target_index].block_kind == 2 && ((Cover_block_list[origin_idx].diff_colour_block >> 2) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = x0;
            add_point.y = (y0 - 1);
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }
    

    if (x0 > 0 && y0 > 0)
    {
        target_index = origin_idx - width - 1;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Cover_block_list[target_index].block_kind == 2 && ((Cover_block_list[origin_idx].diff_colour_block >> 3) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = (x0 - 1);
            add_point.y = (y0 - 1);
            return_index.push_back(target_index);
        }
    }
    

    if (x0 > 0 && y0 < height - 1)
    {
        target_index = origin_idx - width + 1;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Cover_block_list[target_index].block_kind == 2 && ((Cover_block_list[origin_idx].diff_colour_block >> 1) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = (x0 + 1);
            add_point.y = (y0 - 1);
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }
    
    if (x0 < width - 1)
    {
        target_index = origin_idx + 1;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Target_Block.block_kind == 2 && ((Target_Block.diff_colour_block >> 0) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = (x0 + 1);
            add_point.y = y0;
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }
    
    if (y0 < height - 1)
    {
        target_index = origin_idx + width;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Target_Block.block_kind == 2 && ((Target_Block.diff_colour_block >> 6) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = x0;
            add_point.y = (y0 + 1);
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }

    if (x0 < width && y0 < height - 1)
    {
        target_index = origin_idx + width - 1;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Target_Block.block_kind == 2 && ((Target_Block.diff_colour_block >> 5) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = (x0 - 1);
            add_point.y = (y0 + 1);
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }


    if (x0 > 0 && y0 < height - 1)
    {
        target_index = origin_idx + width + 1;
        Image_Colour_Block &Target_Block = Cover_block_list[target_index];

        if (Target_Block.block_kind == 2 && ((Target_Block.diff_colour_block >> 7) & 1) != 1)
        {
            Point_2d add_point;
            add_point.x = (x0 + 1);
            add_point.y = (y0 + 1);
            return_result.push_back(add_point);
            return_index.push_back(target_index);
        }
    }
}


void line_fixed_extend(
    Line_Object& line_object,
    INDEX image_scene_idx, INDEX rough_idx,
    int x0, int y0, char extend_direction)
{
    vector<INDEX> return_result;

    Image_Scene &curr_map = Image_Scene_Storage[image_scene_idx];
    Image_Rough_Map &explain_View = curr_map.Image_rough_view[rough_idx];

    int width = explain_View.rough_width;
    int height = explain_View.rough_height;
    vector<Image_Colour_Block> &Cover_Block_List = explain_View.Colour_block_list;
    int origin_index = width * y0 + x0;
    return_result.clear();

    char turn_on = 1;

    while (turn_on)
    {
        switch (extend_direction)
        {
        case 4:
            if (x0 > 0)
            {
                int target_index = origin_index - 1;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 4) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 2:
            if (y0 > 0)
            {
                int target_index = origin_index - width;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 2) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 3:
            if (x0 > 0 && y0 > 0)
            {
                int target_index = origin_index - width - 1;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 3) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 1:
            if (x0 > 0 && y0 < height - 1)
            {
                int target_index = origin_index - width + 1;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 1) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 0:
            if (x0 < width - 1)
            {
                int target_index = origin_index + 1;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 0) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 6:
            if (y0 < height - 1)
            {
                int target_index = origin_index + width;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 6) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 5:
            if (x0 < width && y0 < height - 1)
            {
                int target_index = origin_index + width - 1;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 5) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;

        case 7:
            if (x0 > 0 && y0 < height - 1)
            {
                int target_index = origin_index + width + 1;

                if (Cover_Block_List[target_index].block_kind == 2 && ((Cover_Block_List[origin_index].diff_colour_block >> 7) & 1) != 1)
                {
                    return_result.push_back(target_index);
                }
                else
                    turn_on = 0;
            }
            else
                turn_on = 0;
            break;
        }
    }
}


void line_initiative_fit(
    Line_Object& line_object,
    INDEX image_scene_idx, INDEX rough_idx,
    int x0, int y0, char direction )
{
    ;
}


void line_initiative_fit_II(
    INDEX image_scene_idx, INDEX rough_idx,
    unsigned char length, char direction)
{
    ;
}


void Base_Detect_Model(INDEX )
{
    ;
}



void A_Space_Base_Compontent_Detect(INDEX target_block_idx, INDEX image_scene_idx)
{
    Image_Scene& curr_image_scene = Image_Scene_Storage[image_scene_idx];

    INDEX rough_map_idx;

    short width;

    int x = target_block_idx % width;
    int y = target_block_idx / width;

    struct Observe_Require
    {
        char is_record = 0;
        char direction_8;
        char distance_8;

        short self_x;
        short self_y;

        INDEX self_i;
        int require_value = 0;
    };

    Advanced_Value_Sort require_sort;
    vector<Observe_Require> observe_list;
    vector<INDEX> free_observe_idx;
    int sum_require = 0;

    bool new_node = 1;

    
    while(new_node)
    {
        Image_Colour_Block &curr_block = curr_image_scene.Image_rough_view[rough_map_idx].Colour_block_list[target_block_idx];
        INDEX colour_idx = curr_block.ave_r*8*8 + curr_block.ave_g*8 + curr_block.ave_b;
        ID colour_id = COLOUR_BLOCK_ENTER_SORT[colour_idx];

        if(colour_id == 0)
        {
            colour_id = new_colour_block_record(curr_block.ave_r, curr_block.ave_g, curr_block.ave_b);
        }

        int *ptr = node_find(colour_id);

        Neuro_Head_Item neuro_head = neuro_head_read(*ptr);

        *(ptr + 1) += 1;

        ptr += 16;
        
        int fea = 0;

        // 1
        while(fea < neuro_head.used_item_num)
        {
            Attribute_Head_Item attribute_head = attribute_head_read(ptr);

            char condition_kind;
            char condition_num = attribute_head.condition_num;
            char condition_size = 0;

            char result_kind;
            char result_num = attribute_head.result_num;
            char result_size = 0;

            ptr + 3;
            fea + 3;

            if (attribute_head.attribute_kind == 2 && condition_num == 0)
            {
                for (char a = 0; a < result_num; a ++)
                {
                    return_item_kind_and_size(ptr, result_kind, result_size);
                    fea += result_size;
                    if(result_kind != 7)
                        continue;
                    Neuro_Manner_Item manner_item = *(Neuro_Manner_Item*)(ptr);
                    if(manner_item.manner_kind != 10)
                        continue;

                    return_item_kind_and_size(ptr, result_kind, result_size);
                    fea += result_size;
                    Neuro_Image_Item image_item;

                    Observe_Require observe_require;
                    observe_require.direction_8 = image_item.direction;
                    observe_require.require_value = manner_item.manner_value;
                    observe_require.self_x = x;
                    observe_require.self_y = y;
                    
                    Line_Object line_object;
                    line_fixed_extend(line_object, image_scene_idx, rough_map_idx,
                        observe_require.self_x, observe_require.self_y, observe_require.direction_8);
                    image_a_line_retrieve_generate(image_scene_idx, rough_map_idx, line_object);
                }
            }
        }

    }

}


void scene_clean()
{
    for (int a = 0; a < General_Scene_Storage.size(); a++)
    {
        if (General_Scene_Storage[a].scene_attention < 10)
            delete_a_General_Scene(a);
    }

    for (int b = 0; b < Text_Scene_Storage.size(); b++)
    {
        if (Text_Scene_Storage[b].text_derive_attention < 10 || Text_Scene_Storage[b].input_time > 60000)
            delete_a_Text_Scene(b);
    }

    for (int c = 0; c < Image_Scene_Storage.size(); c++)
    {
        if (Image_Scene_Storage[c].attention < 10 || CURRENT_MODEL_TIME - Image_Scene_Storage[c].input_time > 3200)
            delete_a_Image_Scene(c);
    }
}


struct text_file_unit
{
    int ch;
    unsigned int link_id;
};


unordered_map<int, ID> Charter_Find_Id;
unordered_map<ID, int> Id_Find_Charter;


vector<text_file_unit> Enter_CH_Storage;


void load_text_enter_ch()
{
    Enter_CH_Storage.resize(ALL_NUM_OF_CH);

    Charter_Find_Id.reserve(ALL_NUM_OF_CH * 1.5);
    Charter_Find_Id.max_load_factor(0.7);

    Id_Find_Charter.reserve(ALL_NUM_OF_CH * 1.5);
    Id_Find_Charter.max_load_factor(0.7);

    FILE *fp = fopen("Record_enter_ch.rec", "r");
    if (fp)
    {
        fread(Enter_CH_Storage.data(), ALL_NUM_OF_CH * 8, 1, fp);
        fclose(fp);
    }

    for (int a = 0; a < ALL_NUM_OF_CH; a++)
    {
        Charter_Find_Id[Enter_CH_Storage[a].ch] = Enter_CH_Storage[a].link_id;
        Id_Find_Charter[Enter_CH_Storage[a].link_id] = Enter_CH_Storage[a].ch;
    }
}


void save_text_enter_ch()
{
    FILE *fp = fopen("Record_enter_ch.rec", "w");
    fwrite(Enter_CH_Storage.data(), ALL_NUM_OF_CH * 8, 1, fp);
    fclose(fp);
}


ID text_enter_ch_add(int new_ch)
{
    Neuro_Text_Desc text_desc;
    Neuro_Desc neuro_desc = {.text_desc = text_desc};

    ID id = new_neuro_create(2, neuro_desc);

    text_file_unit tf;
    tf.ch = new_ch;
    tf.link_id = id;
    Enter_CH_Storage.push_back(tf);
    Charter_Find_Id[new_ch] = id;
    Id_Find_Charter[id] = new_ch;
    ALL_NUM_OF_CH += 1;

    return id;
}



inline void Character_to_node(
    const vector<char> &input_vec,
    vector<INDEX> &result)
{
    int code = 0;
    int bytesNeeded = 0;

    vector<int> utf8_list;

    for (unsigned char c : input_vec)
    {
        int utf8_ch;

        if (bytesNeeded == 0)
        {
            if ((c & 0x80) == 0)
            {
                utf8_ch = c;
                utf8_list.push_back(utf8_ch);
            }
            else if ((c & 0xE0) == 0xC0)
            {
                code = c & 0x1F;
                bytesNeeded = 1;
            }
            else if ((c & 0xF0) == 0xE0)
            {
                code = c & 0x0F;
                bytesNeeded = 2;
            }
            else if ((c & 0xF8) == 0xF0)
            {
                code = c & 0x07;
                bytesNeeded = 3;
            }
        }
        else
        {
            code = (code << 6) | (c & 0x3F);
            bytesNeeded--;

            if (bytesNeeded == 0)
            {
                utf8_ch = c;
                utf8_list.push_back(utf8_ch);
            }
        }
    }

    int a = 0;

    
    for (int utf8_ch : utf8_list)
    {
        General_Node node;

        if (Charter_Find_Id.count(utf8_ch))
        {
            node.self_id = (Charter_Find_Id[utf8_ch]);
        }
        else
        {
            ID id = text_enter_ch_add(utf8_ch);
            node.self_id = id;
        }

        General_Node_Storage.push_back(node);
        INDEX idx = General_Node_Storage.size() - 1;

        result.push_back(idx);

        a++;
    }
}


void CharVector_To_Network(INDEX scene_idx,
    const vector<char> &input_vec, long long input_time)
{
    
    Text_Scene &curr_scene = Text_Scene_Storage[scene_idx];

    curr_scene.input_time = input_time;
    curr_scene.source_kind = 1;
    curr_scene.Text_node_space_from_record_list.clear();

    vector<vector<INDEX>> &text_node_storage_list = curr_scene.Text_node_space_from_record_list;

    vector<INDEX> result;
    result.reserve(input_vec.size());

    Character_to_node(input_vec, result);

    int result_1_8_size = result.size() / 8;
    int result_order = 0;

    if (result.size() % 8 > 0)
    {
        result_1_8_size += 1;
    }

    for (int a = 0; a < result_1_8_size; a++)
    {
        for (int b = 0; b < 8; b++)
        {
            text_node_storage_list[a][b] = result[result_order];
            result_order++;
        }
    }

    return;
}


vector<char> Network_To_CharVector(INDEX text_idx)
{
    Text_Scene text_scene = Text_Scene_Storage[text_idx];

    vector<vector<INDEX>> &text_node_storage_list =
        text_scene.Text_node_space_from_record_list;

    int original_text_size = text_scene.original_text_size;
    vector<INDEX> &list = text_scene.Original_text;
    list.resize(original_text_size);

    int result_1_8_size = original_text_size / 8;
    int result_order = 0;

    if (original_text_size % 8 > 0)
        result_1_8_size += 1;

    for (int a = 0; a < result_1_8_size; a++)
    {
        for (int b = 0; b < 8 && result_order < original_text_size; b++)
        {
            list[result_order] = text_node_storage_list[a][b];
            result_order++;
        }
    }

    vector<char> output;
    output.reserve(list.size() * 4);

    for (INDEX b : list)
    {
        General_Node &node = General_Node_Storage[b];
        int code = Id_Find_Charter[node.self_id]; // id转字笄1�71ￄ1�77

        if (code <= 0x7F)
        {
            output.push_back((char)(code));
        }
        else if (code <= 0x7FF)
        {
            output.push_back((char)(0xC0 | (code >> 6)));
            output.push_back((char)(0x80 | (code & 0x3F)));
        }
        else if (code <= 0xFFFF)
        {
            output.push_back((char)(0xE0 | (code >> 12)));
            output.push_back((char)(0x80 | ((code >> 6) & 0x3F)));
            output.push_back((char)(0x80 | (code & 0x3F)));
        }
        else
        {
            output.push_back((char)(0xF0 | (code >> 18)));
            output.push_back((char)(0x80 | ((code >> 12) & 0x3F)));
            output.push_back((char)(0x80 | ((code >> 6) & 0x3F)));
            output.push_back((char)(0x80 | (code & 0x3F)));
        }
    }

    output.push_back('\0');

    return output;
}


void text_send_to(INDEX curr_text_idx)
{
    Text_Scene &curr_text = Text_Scene_Storage[curr_text_idx];

    if (curr_text.source_kind == 3)
        return;

    bool expected = 0;

    if (Text_Is_Send.compare_exchange_strong(expected, 1, memory_order_seq_cst))
    {
        Send_Text_List = Network_To_CharVector(curr_text_idx);
        Text_Is_Send.store(0);
    }
    else
    {
        
    }
}


void text_operate(
    INDEX curr_text_idx, vector<unsigned int> input_text_list,
    int beg_pos, int end_pos, char operate_kind)
{
    Text_Scene &curr_text = Text_Scene_Storage[curr_text_idx];
    vector<vector<INDEX>> &text_node_storage_list = curr_text.Text_node_space_from_record_list;

    if (operate_kind == 0)
    {
        text_node_storage_list.erase(text_node_storage_list.begin() + beg_pos, text_node_storage_list.begin() + end_pos);
    }
    else if (operate_kind == 1)
    {
        text_node_storage_list.insert(text_node_storage_list.begin() + beg_pos,
            input_text_list.begin(), input_text_list.end());
    }

}



unsigned int Mouse_DwFlags[10] = {
    0x0001, // 相对移动	MOUSEEVENTF_MOVE
    0x0002, // 左键按下	MOUSEEVENTF_LEFTDOWN
    0x0004, // 左键抬起	MOUSEEVENTF_LEFTUP
    0x0008, // 右键按下	MOUSEEVENTF_RIGHTDOWN
    0x0010, // 右键抬起	MOUSEEVENTF_RIGHTUP
    0x0020, // 中键按下	MOUSEEVENTF_MIDDLEDOWN
    0x0040, // 中键抬起  MOUSEEVENTF_MIDDLEUP
    0x0800, // 垂直滚轮	MOUSEEVENTF_WHEEL
    0x1000, // 横向滚轮	MOUSEEVENTF_HWHEEL
    0x8000  // 绝对移动	MOUSEEVENTF_ABSOLUTE
};


unsigned int Keyboard_DwFlags[2] = {
    0x0000, // 按下 KEYEVENTF_KEYDOWN
    0x0002  // 松开 KEYEVENTF_KEYUP
};


unsigned char Keyboard_Common_Keys[79] = {

    0x08, // VK_BACK
    0x09, // VK_TAB
    0x0D, // VK_RETURN (Enter)
    0x10, // VK_SHIFT
    0x11, // VK_CONTROL
    0x12, // VK_MENU (Alt)
    0x1B, // VK_ESCAPE
    0x20, // VK_SPACE
    0x21, // VK_PRIOR (Page Up)
    0x22, // VK_NEXT  (Page Down)
    0x23, // VK_END
    0x24, // VK_HOME
    0x2D, // VK_INSERT
    0x2E, // VK_DELETE
    0x5B, // VK_LWIN
    0x5C, // VK_RWIN

    0x25, // VK_LEFT
    0x26, // VK_UP
    0x27, // VK_RIGHT
    0x28, // VK_DOWN

    0x30, // 0 )
    0x31, // 1 !
    0x32, // 2 @
    0x33, // 3 #
    0x34, // 4 $
    0x35, // 5 %
    0x36, // 6 ^
    0x37, // 7 &
    0x38, // 8 *
    0x39, // 9 (

    0x41, // A
    0x42, // B
    0x43, // C
    0x44, // D
    0x45, // E
    0x46, // F
    0x47, // G
    0x48, // H
    0x49, // I
    0x4A, // J
    0x4B, // K
    0x4C, // L
    0x4D, // M
    0x4E, // N
    0x4F, // O
    0x50, // P
    0x51, // Q
    0x52, // R
    0x53, // S
    0x54, // T
    0x55, // U
    0x56, // V
    0x57, // W
    0x58, // X
    0x59, // Y
    0x5A, // Z

    0x70, // VK_F1
    0x71, // VK_F2
    0x72, // VK_F3
    0x73, // VK_F4
    0x74, // VK_F5
    0x75, // VK_F6
    0x76, // VK_F7
    0x77, // VK_F8
    0x78, // VK_F9
    0x79, // VK_F10
    0x7A, // VK_F11
    0x7B, // VK_F12

    0xBA, // ; :
    0xBB, // = +
    0xBC, // , <
    0xBD, // - _
    0xBE, // . >
    0xBF, // / ?
    0xC0, // ` ~
    0xDB, // [ {
    0xDC, // \ |
    0xDD, // ] }
    0xDE  // ' "
};


vector<ID> ACTION_SORT;
// 鼠标 6 + 16*16(位置) +2*16(滚轮) + 键盘 79*2 + 4 = 456


void Load_External_Action_File()
{
    ACTION_SORT.resize(456);

    FILE *fp = fopen("Record_Action_Sort.rec", "r");
    fread(ACTION_SORT.data(), 456 * 4, 1, fp);

    fclose(fp);
}


void Save_external_action_file()
{
    FILE *fp = fopen("Record_enter_line.rec", "w");
    fwrite(ACTION_SORT.data(), 456 * 4, 1, fp);

    fclose(fp);
}


void New_external_action_record()
{
    Neuro_Text_Desc text_desc;
    Neuro_Desc neuro_desc = {.text_desc = text_desc};

    ID id = new_neuro_create(4, neuro_desc);
    INDEX pos;
    ACTION_SORT[pos] = id;

    int *ptr = node_find(id);
}


vector<int> Keybord_Action_Tended(79, 20);
vector<int> Key_Press_Tended(79, 1);
vector<int> Key_Release_Tended(79, 1);


Model_Output_Action random_keybord()
{
    static thread_local random_device Keybord_RD;
    static thread_local mt19937 Keybord_Gen(Keybord_RD());
    discrete_distribution<int> Keybord_choose_dist(
        Keybord_Action_Tended.begin(), Keybord_Action_Tended.end());

    Model_Output_Action moa;
    moa.action_kind = 2;

    
    int vk = Keybord_choose_dist(Keybord_Gen);

    moa.vk = Keyboard_Common_Keys[vk];

    int Press_Release[2] = {Key_Press_Tended[vk], Key_Release_Tended[vk]};

    discrete_distribution<int> Press_Release_choose_dist(
        Press_Release, Press_Release + 1);

    int d = Press_Release_choose_dist(Keybord_Gen);

    if (d == 0)
        moa.dwFlags = 0x0000; // 按下 KEYEVENTF_KEYDOWN
    else
        moa.dwFlags = 0x0002; // 松开 KEYEVENTF_KEYUP

    return moa;
}

vector<int> Mouse_Action_Tended(10, 20);


Model_Output_Action random_mouse()
{
    static thread_local random_device Mouse_RD;
    static thread_local mt19937 Mouse_Gen(Mouse_RD());
    discrete_distribution<int> Mouse_choose_dist(
        Mouse_Action_Tended.begin(), Mouse_Action_Tended.end());

    Model_Output_Action moa;
    moa.action_kind = 1;


    int d = rand_0_to_255();

    while (d > 10)
        d -= 10;

    moa.dwFlags = Mouse_DwFlags[d];

    if (moa.dwFlags == 0x0001     // 相对移动	MOUSEEVENTF_MOVE
        || moa.dwFlags == 0x8000) // 绝对移动 MOUSEEVENTF_ABSOLUTE
    {
        moa.x = rand_0_to_255();
        moa.y = rand_0_to_255();
    }


    if (moa.dwFlags == 0x0800     // 垂直滚轮	MOUSEEVENTF_WHEEL
        || moa.dwFlags == 0x1000) // 横向滚轮 MOUSEEVENTF_HWHEEL
    {
        moa.mouseData = rand_0_to_255() - 128;
    }

    return moa;
}


void External_Action_Choose(void)
{
    discrete_distribution<int> action_choose_dist(Action_Choose_Weights.begin(), Action_Choose_Weights.end()); // 填入注意力的倄1�71ￄ1�77

    uniform_int_distribution<> equal_dist(0, 4);

    static thread_local random_device Motion_RD;
    static thread_local mt19937 Motion_Gen(Motion_RD());

    //
    Model_Output_Action model_action;

    int random_tended;
    int aim_tended;

    int mouse_tended = 100;
    int key_tended = 100;
    int interface_tended = 10;

    short action_limit = 5;
    short n = 0;

    while (n < action_limit)
    {
        int choose_I[2] = {random_tended, aim_tended};

        discrete_distribution<int> action_kind_dist(choose_I, choose_I + 1);
        char action_kind = action_kind_dist(Motion_Gen);

        if (action_kind == 1)
        {
            vector<int> choose_II = {mouse_tended, key_tended, interface_tended};
            discrete_distribution<int> choose_II_dist(choose_II.begin(), choose_II.end());
            action_kind = choose_II_dist(Overall_Gen);

            if (action_kind == 1)
            {
                model_action = random_mouse();
            }
            else if (action_kind == 2)
            {
                model_action = random_keybord();
            }
            else if (action_kind == 3)
            {
                vector<int> choose_III = {1, 1, 1, 1};
                discrete_distribution<int> choose_III_dist(choose_III.begin(), choose_III.end());
                model_action.action_kind = choose_III_dist(Overall_Gen);
            }

        }
        else
        {
            model_action = Action_Choose_List[action_choose_dist(Overall_Gen)];
        }

        model_action.action_order = n;
        Waiting_Send_Action_List.push_back(model_action);
        n++;
    }

}