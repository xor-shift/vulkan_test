import * as fs from "fs";

const in_filename_json = "functions.json";
const out_filename_members = "generated/members.ipp";
const out_filename_wrappers = "generated/wrappers.ipp";
const out_filename_pre_instance_init = "generated/pre_instance_init.ipp";
const out_filename_instance_init = "generated/instance_init.ipp";
const out_filename_device_init = "generated/device_init.ipp";

const bit_pre_instance = 0b001;
const bit_instance = 0b010;
const bit_device = 0b100;

import json_file_data from "./functions.json" assert {type: "json"};

const generate_members = new Promise<void>(resolve => {
    let out = "";

    let first = true;
    for (const group of json_file_data.groups) {
        if (!first) {
            out += '\n';
        }
        first = false;

        out += `// ${group.name}\n\n`;

        for (const fn_info of group.functions) {
            out += `PFN_${fn_info.name} ${fn_info.name} = nullptr;\n`;
        }
    }

    fs.writeFile(out_filename_members, out, () => {
        resolve();
    });
});

const generate_wrappers = new Promise<void>(resolve => {
    let out = "";


    fs.writeFile(out_filename_wrappers, out, () => {
        resolve();
    });
});

function generate_initialisers(bit: number, filename: string): Promise<void> {
    return new Promise<void>(resolve => {

        let out = "";

        out += "static const std::pair<void (*vulkan_functions::*)(), const char*> functions_and_names[] {\n";

        let first = true;
        for (const group of json_file_data.groups) {
            if (!first) {
                out += '\n';
            }
            first = false;

            out += `  // ${group.name}\n`;

            for (const fn_info of group.functions) {
                const ok = (fn_info.stage_bits & bit) != 0;
                if (!ok) {
                    continue;
                }

                out += `  PTR_NAME_PAIR(${fn_info.name}),\n`;
            }
        }

        out += "};";

        fs.writeFile(filename, out, () => {
            resolve();
        });
    });
}

Promise.all([
    generate_members,
    generate_wrappers,
    generate_initialisers(bit_pre_instance, out_filename_pre_instance_init),
    generate_initialisers(bit_instance, out_filename_instance_init),
    generate_initialisers(bit_device, out_filename_device_init),
]).then()
