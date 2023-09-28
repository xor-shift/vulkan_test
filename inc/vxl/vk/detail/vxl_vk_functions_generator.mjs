import * as fs from "fs";
// import * as json from "json";

const in_filename_json = "vxl_vk_functions.json";
const out_filename_members = "vxl_vk_functions_members.ipp";
const out_filename_wrappers = "vxl_vk_functions_wrappers.ipp";
const out_filename_pre_instance_init = "vxl_vk_functions_pre_instance_init.ipp";
const out_filename_instance_init = "vxl_vk_functions_instance_init.ipp";
const out_filename_device_init = "vxl_vk_functions_device_init.ipp";

import json_file_data from "./vxl_vk_functions.json" assert { type: "json" };

/*const file_name = "./vxl_vk_functions.json";
const file_contents = fs.readFileSync(file_name);
const file_parsed = JSON.parse(file_contents);*/

console.log(json_file_data);

let members_list = "";
for (const group of json_file_data.groups) {
    console.log(`processing group with name "${group.name}" for member generation`);

    members_list += `\n// ${group.name}\n\n`;

    for (const fn_info of group.functions) {
        members_list += `PFN_${fn_info.name} ${fn_info.name} = nullptr;\n`
    }
}

console.log(members_list);