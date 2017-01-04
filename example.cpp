#include <iostream>
#include <typeinfo>
#include "json_obj.hpp"

using namespace std;

int main()
{   
    int i;
    // have to give key to represent json itself
    json *a = new json();
    json s;

    /*principle
      1. x["key"] will always get object
      2. x("key") will always get array
      3. any key duplicating with different json type will lead to 
         overwrite previous element.
      4. try to get non-existing object or array will automatically create new one
    */

    //add number
    s["number"] = 1.2;
    //add string
    s["string"] = "Yeah~!";
    //add boolean
    s["boolean"] = true;
    //add null
    s["null"] = nullptr;
    //add object
    s["object"]["something"] = "string_in_something";
    //add array (with some operator)
    s("array")(2.2, "string_in_array");

    (*a)["number_in_a"] = 9487;
    //add another json
    s["key_a"] = *a;
    //add another json in array
    s("array_a")(*a);
    // a_json modification will be seen in s_json, and vice versa
    (*a)["string_in_a"] = "this string is in a";

    //push object in array
    //push_object_back(json) is also provided
    s("array").push_object_back();
    //insert json in array
    //insert_object(index) is also provided
    s("array").insert_object(3, *a);
    //psuh array in array
    s("array").push_array_back()(3.3, "another_string");
    //insert array in array
    s("array").insert_array(0)(4.4, nullptr);
    delete a;
    // deleting a_json will not influence s_jon

    //push number in array
    //push_string_back, push_boolean_back also provided
    s("array").push_number_back(5566);
    //insert number in array
    //insert_string, insert_boolean also provided
    s("array").insert_number(1, 426);

    // get array or object from array (it will insert a new element if not exist)
    s("array").get_array(0).push_string_back("string in array of array");
    s("array").get_object(4)["object_in_array"] = true;

    //delete any element from object by key
    s -= "array_a";
    s -= "a_json";

    //delete element from array by index (single or range) or just pop back
    s("array").pop_any_back();
    s("array").erase_any_single(4);
    s("array").erase_any_range(1,4); // not include 4
    //dump json to valid json format
    s.print_json();
    return 1;
}
