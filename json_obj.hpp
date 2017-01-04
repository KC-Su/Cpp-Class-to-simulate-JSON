#ifndef JSON_OBJ_HPP
#define JSON_OBJ_HPP
#include <iostream>
#include <cstdarg>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeinfo>
#include <type_traits>

enum json_type {
    JSON_OBJ = 1,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL
};

class json;
//it's null as well
class json_basic{
public:
    int json_type;
    //construct
    json_basic() {
        json_type = JSON_NULL;
    }
};

class json_string : public json_basic {
public:
    std::string string;
    // construct
    json_string(std::string string) {
        json_type       = JSON_STRING;
        this->string    = string;
    }
};

class json_number : public json_basic {
public:
    float number;
    // construct
    json_number(float number) {
        json_type       = JSON_NUMBER;
        this->number    = number; 
    }
};

class json_boolean : public json_basic {
public:
    bool boolean;
    // construct
    json_boolean(bool boolean) {
        json_type       = JSON_BOOLEAN;
        this->boolean   = boolean;
    }
};

// similar to typedef in C
class json;
class json_object;
class json_array : public json_basic {
private:
    std::vector<std::shared_ptr<json_basic>> json_ary_vector;
    std::vector<std::shared_ptr<json_basic>>::iterator vector_itr;
    std::vector<std::shared_ptr<json_basic>>::reverse_iterator vector_ritr;

    // helper for recursive funtion calling (operator can't achieve recusive calling)
    void json_array_helper() {return;}
    template <typename... variadic_type>
    void json_array_helper(float f_number, variadic_type... args) { 
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_number>(f_number)));        
        json_array_helper(args...);
        return;
    }
    template <typename... variadic_type>
    void json_array_helper(std::string s, variadic_type... args) {
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_string>(s)));
        json_array_helper(args...);
        return;
    }
    template <typename boolean_type,
              typename = typename std::enable_if<std::is_same<typename std::decay<boolean_type>::type, bool>::value>::type,
              typename... variadic_type>
    void json_array_helper(boolean_type boolean, variadic_type... args) {
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_boolean>(boolean)));
        json_array_helper(args...);
        return;
    }
    template <typename... variadic_type>
    void json_array_helper(void *ptr, variadic_type... args) {
        json_ary_vector.push_back(std::make_shared<json_basic>());
        json_array_helper(args...);
        return;
    }
    template <typename... variadic_type>
    void json_array_helper(json &j, variadic_type... args);

    void resize_vector(unsigned int size) {
        // vector will be resized if size is bigger than vector capacity
        if (json_ary_vector.capacity() <= size) {
            json_ary_vector.resize(size, std::make_shared<json_basic>());
        }
        return;
    }
public:
    // to make print function in json work
    friend class json;
    // construct
    json_array() {
        json_type = JSON_ARRAY;
    }
    //deconstruct
    ~json_array() {
        json_ary_vector.clear();
    }

    // accept multiple elements
    // number
    template <typename... variadic_type>
    json_array& operator()(float f_number, variadic_type... args) {
        json_array_helper(f_number, args...);
        return *this;
    }
    // string
    template <typename... variadic_type>
    json_array& operator()(std::string s, variadic_type... args) {
        json_array_helper(s, args...);
        return *this;
    }
    // boolean
    template <typename boolean_type,
              typename = typename std::enable_if<std::is_same<typename std::decay<boolean_type>::type, bool>::value>::type,
              typename... variadic_type>
    json_array& operator()(boolean_type boolean, variadic_type... args) {
        json_array_helper(boolean, args...);
        return *this;
    }
    // null
    template <typename... variadic_type>
    json_array& operator()(void *ptr, variadic_type... args) {
        if (ptr == nullptr) {
            return *this;
        }
        json_array_helper(ptr, args...);
        return *this;
    }
    // json
    template <typename... variadic_type>
    json_array& operator()(json &j, variadic_type... args) {
        json_array_helper(j, args...);
        return *this;
    }

    // insert/push_back object and return new object
    // write latter
    json_object& insert_object(unsigned int i);
    json_object& push_object_back();
    // write latter
    json_object& insert_object(unsigned int i, json &j);
    json_object& push_object_back(json &j);
    // insert/push_back array and return new array
    json_array& insert_array(unsigned int i) {
        resize_vector(i);
        vector_itr = json_ary_vector.begin();
        json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_array>()));
        return *static_cast<json_array*>(json_ary_vector[i].get());
    }

    json_array& push_array_back() {
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_array>()));
        vector_ritr = json_ary_vector.rbegin();
        return *static_cast<json_array*>((*vector_ritr).get());
    }

    // insert number
    void insert_number(unsigned int i, float f_number) {
        resize_vector(i);
        vector_itr = json_ary_vector.begin();
        json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_number>(f_number)));
        return;
    }

    void push_number_back(float f_number) {
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_number>(f_number)));
        return;
    }
    // insert string
    void insert_string(unsigned int i, std::string s) {
        resize_vector(i);
        vector_itr = json_ary_vector.begin();
        json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_string>(s)));
        return;
    }

    void push_string_back(std::string s) {
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_string>(s)));
        return;
    }
    // insert boolean
    template <typename boolean_type,
              typename = typename std::enable_if<std::is_same<typename std::decay<boolean_type>::type, bool>::value>::type>
    void insert_boolean(unsigned int i, boolean_type boolean) {
        resize_vector(i);
        vector_itr = json_ary_vector.begin();
        json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_boolean>(boolean)));
        return;
    }

    template <typename boolean_type,
              typename = typename std::enable_if<std::is_same<typename std::decay<boolean_type>::type, bool>::value>::type>
    void push_boolean_back(boolean_type boolean) {
        json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_boolean>(boolean)));
        return;
    }
    // insert null
    void insert_string(unsigned int i, void *ptr) {
        if (ptr != nullptr) {
            return;
        }
        resize_vector(i);
        vector_itr = json_ary_vector.begin();
        json_ary_vector.insert(vector_itr + i, std::make_shared<json_basic>());
        return;
    }

    void push_string_back(void *ptr) {
        if (ptr != nullptr) {
            return;
        }
        json_ary_vector.push_back(std::make_shared<json_basic>());
        return;
    }
    // pop_back element
    void pop_any_back() {
        json_ary_vector.pop_back();
        return;
    }
    // erase single element
    void erase_any_single(unsigned int i) {
        vector_itr = json_ary_vector.begin();
        json_ary_vector.erase(vector_itr + i);
        return;
    }
    // erase range of elements
    void erase_any_range(int begin, int last) {
        vector_itr = json_ary_vector.begin();
        json_ary_vector.erase(vector_itr + begin, vector_itr + last);
        return;
    }

    // get object in array
    json_object& get_object(unsigned int i);

    // get array in array
    json_array& get_array(unsigned int i);
};

class json_object : public json_basic {
private:
    std::string key;
    std::unordered_map<std::string, std::shared_ptr<json_basic>> json_obj_map;
    std::unordered_map<std::string, std::shared_ptr<json_basic>>::iterator map_itr;
public:
    // to make print function in json work
    friend class json;
    // construct
    json_object() {
        // assign type
        json_type = JSON_OBJ;
    }
    // deconstruct
    ~json_object() {
        // release all json elements stored in map
        json_obj_map.clear();
    }
    // for json type object
    json_object& operator[](std::string s) {
        // check object of object process
        map_itr = json_obj_map.find(key);
        // store keyword s
        key = s;
        if (map_itr != json_obj_map.end() && map_itr->second->json_type == JSON_OBJ) {
            return (*static_cast<json_object*>(map_itr->second.get()))[s];
        } 
        map_itr = json_obj_map.find(s);
        // lookup this object exist in map or not 
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            json_obj_map[s] = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_object>());
            map_itr         = json_obj_map.find(s);
        }
        // check json_type store in map is object
        if (map_itr->second->json_type != JSON_OBJ) {
            // replace by json object 
            json_obj_map.erase(s);
            json_obj_map[s] = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_object>());
            map_itr         = json_obj_map.find(s);
        }
        return *this;
    }

    // for json type array
    json_array& operator()(std::string s) {
        map_itr = json_obj_map.find(s);
        // lookup this array exist in map or not
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            json_obj_map[s] = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_array>());
            map_itr             = json_obj_map.find(s);
        }
        // check json_type store in map is array
        if (map_itr->second->json_type != JSON_ARRAY) {
            // replace by json array
            json_obj_map.erase(s);
            json_obj_map[s] = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_array>());
            map_itr             = json_obj_map.find(s);
        }
        return *(static_cast<json_array*>(map_itr->second.get()));
    }

    template <typename X, typename... variadic_type>
    json_array& operator()(std::string s, X temp_arg, variadic_type... args) {
        //  object(s) --> array
        //  array(temp_arg, args...)
        return (*this)(s)(temp_arg, args...);
    }

    // for json type number
    void operator=(float f_number) {
        map_itr = json_obj_map.find(key);
        // lookup this number exist in map or not
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            json_obj_map[key]   = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_number>(f_number));
            map_itr             = json_obj_map.find(key);
            return;
        }
        // existing in map
        // check json_type store in map is number
        if (map_itr->second->json_type != JSON_NUMBER) {
            // replace by json number
            json_obj_map.erase(key);
            json_obj_map[key]   = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_number>(f_number));
            map_itr             = json_obj_map.find(key);
        } else {
            // assign new number
            static_cast<json_number*>(map_itr->second.get())->number = f_number;
        }
        return;
    }

    // for json type string
    void operator=(std::string s) {
        map_itr = json_obj_map.find(key);
        // lookup this string exist in map or not
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            json_obj_map[key]   = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_string>(s));
            map_itr             = json_obj_map.find(key);
            return;
        }
        // existing in map
        // check json_type store in map is string
        if (map_itr->second->json_type != JSON_STRING) {
            // replace by json string
            json_obj_map.erase(key);
            json_obj_map[key]   = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_string>(s));
            map_itr             = json_obj_map.find(key);
        } else {
            // assign new string
            static_cast<json_string*>(map_itr->second.get())->string = s;
        }
        return;
    }

    // just filter true, false and bool variable
    template <typename boolean_type,
              typename = typename std::enable_if<std::is_same<typename std::decay<boolean_type>::type, bool>::value>::type>
    // for json type boolean
    void operator=(boolean_type boolean) {
        map_itr = json_obj_map.find(key);
        // lookup this boolean exist in map or not
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            json_obj_map[key]   = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_boolean>(boolean));
            map_itr             = json_obj_map.find(key);
            return;
        }
        // existing in map
        // check json_type store in map is boolean
        if (map_itr->second->json_type != JSON_BOOLEAN) {
            // replace by json boolean
            json_obj_map.erase(key);
            json_obj_map[key]   = static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_boolean>(boolean));
            map_itr             = json_obj_map.find(key);
        } else {
            static_cast<json_boolean*>(map_itr->second.get())->boolean = boolean;
        }
        return;
    }

    // for json type null
    void operator=(void *ptr) {
        // check input argument is nullptr or not
        if (ptr != nullptr) {
            return;
        }
        map_itr = json_obj_map.find(key);
        // lookup this null exist in map or not
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            json_obj_map[key]   = std::make_shared<json_basic>();
            map_itr             = json_obj_map.find(key);
            return;
        }
        // existing in map
        // check json_type store in map is NULL
        if (map_itr->second->json_type != JSON_NULL) {
            // replace by json NULL
            json_obj_map.erase(key);
            json_obj_map[key]   = std::make_shared<json_basic>();
            map_itr             = json_obj_map.find(key);
        }
        return;
    }

    // for json 
    void operator=(json &j);

    // for delete any element with key
    void operator-=(std::string s) {
        map_itr = json_obj_map.find(s);
        // lookup this element exist in map or not
        if (map_itr == json_obj_map.end()) {
            // doesn't exist in map
            return;
        }
        // existing in map and erase it
        json_obj_map.erase(s);
        return;
    }
};

class json: public json_basic {
private:
    int tab_count;
    json_basic      *json_basic_ptr; // it's json_type null as well
    json_object     *json_object_ptr;
    json_array      *json_array_ptr;
    json_number     *json_number_ptr;
    json_boolean    *json_boolean_ptr;
    std::shared_ptr<json_object> json_obj_ptr;
    std::unordered_map<std::string, std::shared_ptr<json_basic>>::iterator temp_map_itr;
    std::vector<std::shared_ptr<json_basic>>::iterator temp_vector_itr;

    void print_tab() {
        int i;
        for(i = 0; i < tab_count; i++) {
            std::cout << "  ";
        }
    }

    void print_json_object(json_object *object_ptr) {
        tab_count++;
        for (object_ptr->map_itr = object_ptr->json_obj_map.begin(); object_ptr->map_itr != object_ptr->json_obj_map.end(); object_ptr->map_itr++) {
            if (object_ptr->map_itr->second->json_type == JSON_OBJ) {
                print_tab();
                std::cout << "\"" << object_ptr->map_itr->first << "\": {" << std::endl; 
                print_json_object(static_cast<json_object*>(object_ptr->map_itr->second.get()));
                print_tab();
                std::cout << "}"; 
            } else if (object_ptr->map_itr->second->json_type == JSON_ARRAY) {
                print_tab();
                std::cout << "\"" << object_ptr->map_itr->first << "\": [" << std::endl;
                print_json_array(static_cast<json_array*>(object_ptr->map_itr->second.get()));
                print_tab();
                std::cout << "]";
            } else if (object_ptr->map_itr->second->json_type == JSON_NUMBER) {
                print_tab();
                std::cout << "\"" << object_ptr->map_itr->first << "\": ";
                std::cout << static_cast<json_number*>(object_ptr->map_itr->second.get())->number;
            } else if (object_ptr->map_itr->second->json_type == JSON_STRING) {
                print_tab();
                std::cout << "\"" << object_ptr->map_itr->first << "\": \"";
                std::cout << static_cast<json_string*>(object_ptr->map_itr->second.get())->string << "\"";
            } else if (object_ptr->map_itr->second->json_type == JSON_BOOLEAN) {
                print_tab();
                std::cout << "\"" << object_ptr->map_itr->first << "\": ";
                if (static_cast<json_boolean*>(object_ptr->map_itr->second.get())->boolean) {
                    std::cout << "true";
                } else {
                    std::cout << "false";
                }
            } else { // null json_type is JSON_BASIC
                print_tab();
                std::cout << "\"" << object_ptr->map_itr->first << "\": null";
            }
            temp_map_itr = object_ptr->map_itr;
            temp_map_itr++;
            if (temp_map_itr != object_ptr->json_obj_map.end()) {
                std::cout << "," << std::endl;
            } else {
                std::cout << std::endl;
            }
        }
        tab_count--;
        return;
    }
    void print_json_array(json_array *array_ptr) {
        tab_count++;
        for (array_ptr->vector_itr = array_ptr->json_ary_vector.begin(); array_ptr->vector_itr != array_ptr->json_ary_vector.end(); array_ptr->vector_itr++) {
            if ((*array_ptr->vector_itr)->json_type == JSON_OBJ) {
                print_tab();
                std::cout << "{" << std::endl;
                print_json_object(static_cast<json_object*>((*array_ptr->vector_itr).get()));
                print_tab();
                std::cout << "}";
            } else if ((*array_ptr->vector_itr)->json_type == JSON_ARRAY) {
                print_tab();
                std::cout << "[" << std::endl;
                print_json_array(static_cast<json_array*>((*array_ptr->vector_itr).get()));
                print_tab();
                std::cout << "]";
            } else if ((*array_ptr->vector_itr)->json_type == JSON_NUMBER) {
                print_tab();
                std::cout << static_cast<json_number*>((*array_ptr->vector_itr).get())->number;
            } else if ((*array_ptr->vector_itr)->json_type == JSON_STRING) {
                print_tab();
                std::cout << "\"" << static_cast<json_string*>((*array_ptr->vector_itr).get())->string << "\"";
            } else if ((*array_ptr->vector_itr)->json_type == JSON_BOOLEAN) {
                print_tab();
                if (static_cast<json_boolean*>((*array_ptr->vector_itr).get())->boolean) {
                    std::cout << "true";
                } else {
                    std::cout << "false";
                }
            } else { // null json_type is JSON_BASIC
                print_tab();
                std::cout << "null";
            }
            temp_vector_itr = array_ptr->vector_itr;
            temp_vector_itr++;
            if (temp_vector_itr != array_ptr->json_ary_vector.end()) {
                std::cout << "," << std::endl;
            } else {
                std::cout << std::endl;
            }
        }
        tab_count--;
        return;
    }
public:
    // make json oepration capable
    friend class json_array;
    friend class json_object;
    // construct
    json() {
        json_type       = JSON_OBJ;
        json_obj_ptr    = std::make_shared<json_object>();
    }

    ~json() {
        json_obj_ptr    = nullptr;
    }
    
    json_object& operator[](std::string s) {
        return (*json_obj_ptr)[s];
    }

    template <typename... variadic_type>
    json_array& operator()(variadic_type... args) {
        return (*json_obj_ptr)(args...);
    }

    template <typename X>
    void operator=(X arg) {
        (*json_obj_ptr) = arg;
        return;
    }

    void operator-=(std::string s) {
        (*json_obj_ptr) -= s;
        return;
    }

    // dump in json format
    void print_json() {
        tab_count = 0;
        //std::cout << key_self << std::endl;
        std::cout << "{" << std::endl;
        print_json_object(static_cast<json_object*>(json_obj_ptr.get()));
        std::cout << "}" << std::endl;
        return;
    }
};

inline json_object& json_array::insert_object(unsigned int i) {
    resize_vector(i);
    vector_itr = json_ary_vector.begin();
    json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_object>()));
    return *static_cast<json_object*>(json_ary_vector[i].get());
}

inline json_object& json_array::push_object_back() {
    json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_object>()));
    vector_ritr = json_ary_vector.rbegin();
    return *static_cast<json_object*>((*vector_ritr).get());
}

inline json_object& json_array::insert_object(unsigned int i, json &j) {
    resize_vector(i);
    vector_itr = json_ary_vector.begin();
    json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(j.json_obj_ptr));
    return *static_cast<json_object*>(json_ary_vector[i].get());
}

inline json_object& json_array::push_object_back(json &j) {
    json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(j.json_obj_ptr));
    vector_ritr = json_ary_vector.rbegin();
    return *static_cast<json_object*>((*vector_ritr).get());
}

template <typename... variadic_type>
inline void json_array::json_array_helper(json &j, variadic_type... args) {
    json_ary_vector.push_back(static_cast<std::shared_ptr<json_basic>>(j.json_obj_ptr));
    json_array_helper(args...);
    return;
}

inline json_object& json_array::get_object(unsigned int i) {
    if (i >= json_ary_vector.capacity()) {
        resize_vector(i);
    }
    if (json_ary_vector[i]->json_type == JSON_OBJ) {
        // return existing object
        return *static_cast<json_object*>(json_ary_vector[i].get());
    }
    
    // insert a new json_object
    vector_itr = json_ary_vector.begin();
    json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_object>()));
    return *static_cast<json_object*>(json_ary_vector[i].get());
}

inline json_array& json_array::get_array(unsigned int i) {
    if (i >= json_ary_vector.capacity()) {
        resize_vector(i);
    }
    if (json_ary_vector[i]->json_type == JSON_ARRAY) {
        // return existing array
        return *static_cast<json_array*>(json_ary_vector[i].get());
    }

    // insert a new json_array
    vector_itr = json_ary_vector.begin();
    json_ary_vector.insert(vector_itr + i, static_cast<std::shared_ptr<json_basic>>(std::make_shared<json_array>()));
    return *static_cast<json_array*>(json_ary_vector[i].get());
}

inline void json_object::operator=(json &j) {
    map_itr = json_obj_map.find(key);
    // same key in map situation will let j replacing previous element
    if (map_itr != json_obj_map.end()) {
        // delete existing element
        json_obj_map.erase(key);
    }
    json_obj_map[key] = static_cast<std::shared_ptr<json_basic>>(j.json_obj_ptr);
    // clear key to break recursive object oepration
    key = "";
    return;
}

#endif
