import * as fs from "fs";

const in_filename_json = "functions.json";
const out_filename_members = "generated/members.ipp";
const out_filename_wrappers = "generated/wrappers.ipp";
const out_filename_pre_instance_init = "generated/pre_instance_init.ipp";
const out_filename_instance_init = "generated/instance_init.ipp";
const out_filename_device_init = "generated/device_init.ipp";
const out_filename_pre_instance_replacements = "generated/pre_instance_init_replace.ipp";
const out_filename_instance_replacements = "generated/instance_init_replace.ipp";
const out_filename_device_replacements = "generated/device_init_replace.ipp";

const bit_pre_instance = 0b001;
const bit_instance = 0b010;
const bit_device = 0b100;

interface function_info_common {
    name: string,
    stage_bits: number,
    category: number,
}

interface function_info_cat_0 extends function_info_common {
}

interface function_info_cat_1 extends function_info_common {
    arguments: [string, string, string | undefined][],
}

interface function_info_cat_2 extends function_info_cat_1 {
    success_results: string[],
}

interface function_info_cat_3 extends function_info_cat_1 {
    out_index: number,
}

interface function_info_cat_4 extends function_info_cat_2 {
    out_index: number,
}

interface group {
    name: string,
    requires: string | undefined,
    functions: (function_info_cat_0 | function_info_cat_1 | function_info_cat_2 | function_info_cat_3 | function_info_cat_4)[],
    replacements: [[string, string][], [string, string][], [string, string][]]
}

import json_file_data_raw from "./functions.json" assert {type: "json"};
const json_file_data = json_file_data_raw as { groups: group[] };

// sanity checks
for (const group of json_file_data.groups) {
    // console.log(`checking "${group.name}" which contains ${group.functions.length} function(s) and requires ${group.requires !== undefined ? group.requires + " to be defined" : "no macro to be defined"}`);

    for (const function_info of group.functions) {
        if (function_info.category < 0 || function_info.category > 4) {
            console.log(`ERROR: function "${function_info.name}" of group "${group.name}" has an invalid category`);
        }

        const all_bits = bit_pre_instance | bit_instance | bit_device;
        const excess_bits_mask = ~all_bits;
        if ((function_info.stage_bits & excess_bits_mask) != 0) {
            console.log(`ERROR: function "${function_info.name}" of group "${group.name}" has an stage_bits value (excess bits)`);
        }
    }

    for (const replacement_list_index in group.replacements) {
        const replacements_list = group.replacements[replacement_list_index];
        for (const replacement of replacements_list) {
            const replaces_with = replacement[1];
            if (group.functions.find(function_info => function_info.name == replaces_with) !== undefined) {
                continue;
            }

            console.log(`ERROR: replacement for stage #${replacement_list_index} in group "${group.name}" which replaces "${replacement[0]}" with "${replacement[1]}" is faulty ("${replacement[1]}" is not initialised by this group)`);
        }
    }
}

// quick and dirty
function prettify_vk_name(name: string): string {
    name = name.substring(2);

    let ret = "";

    let first = true;
    let prev_was_lower = false;
    for (const c of name) {
        const lower = c.toLowerCase();
        if (lower == c) {
            ret += c;
            prev_was_lower = true;
            continue;
        }

        if (!first && prev_was_lower) {
            ret += '_';
        }

        ret += lower;

        prev_was_lower = false;
        first = false;
    }

    return ret;
}

function generate_typed_argument_list(args: [string, string, string | undefined][]): string {
    return args.map(arg => {
        let ret = `${arg[0]} ${arg[1]}`;

        if (arg[2] !== undefined) {
            ret += ` = ${arg[2]}`;
        }

        return ret;
    }).join(", ");
}

function generate_typeless_argument_list(args: [string, string, string | undefined][]): string {
    return args.map(arg => arg[1]).join(", ");
}

function generate_wrapper_cat_1(function_info: function_info_cat_1): string {
    const pretty_name = prettify_vk_name(function_info.name);

    const args_typed = generate_typed_argument_list(function_info.arguments);
    const args_typeless = generate_typeless_argument_list(function_info.arguments);

    return `auto ${pretty_name}() const noexcept { return [this](${args_typed}) noexcept { return ${pretty_name}(${args_typeless}); }; }
void ${pretty_name}(${args_typed}) const noexcept { return ${function_info.name}(${args_typeless}); }
`;
}

function generate_wrapper_cat_2(function_info: function_info_cat_2): string {
    const pretty_name = prettify_vk_name(function_info.name);

    const args_typed = generate_typed_argument_list(function_info.arguments);
    const args_typeless = function_info.arguments.map(arg => arg[1]).join(", ");

    const return_type = function_info.success_results.length == 1 ? "std::expected<void, VkResult>" : "std::expected<VkResult, VkResult>";
    const return_expression = function_info.success_results.length == 1 ? "{}" : "res";

    return `auto ${pretty_name}() const noexcept { return [this](${args_typed}) noexcept { return this->${pretty_name}(${args_typeless}); }; }
auto ${pretty_name}(${args_typed}) const noexcept -> ${return_type} {
    const auto res = ${function_info.name}(${args_typeless});
    if (!success<${function_info.success_results.join(", ")}>(res)) {
        return std::unexpected{res};
    }
    return ${return_expression};
}
`;
}

function generate_wrapper_cat_3(function_info: function_info_cat_3): string {
    const arg_names = function_info.arguments.map(arg => arg[1])
    const args_without_out = function_info.arguments.slice(0, function_info.out_index).concat(function_info.arguments.slice(function_info.out_index + 1));

    const pretty_name = prettify_vk_name(function_info.name);

    const arg_list_typeless = args_without_out.map(arg => arg[1]).join(", ");
    const arg_list_typed = generate_typed_argument_list(args_without_out);
    const vk_arg_list_typeless = [...arg_names.slice(0, function_info.out_index), "&ret", ...arg_names.slice(function_info.out_index + 1)].join(", ");

    const return_type = `std::remove_pointer_t<${function_info.arguments[function_info.out_index][0]}>`;

    return `auto ${pretty_name}() const noexcept { return [this](${arg_list_typed}) noexcept { this->${pretty_name}(${arg_list_typeless}); }; }
auto ${pretty_name}(${arg_list_typed}) const noexcept -> ${return_type} {
    TYPE_3_PRELUDE(${function_info.name}, ${function_info.out_index});
    ${function_info.name}(${vk_arg_list_typeless});
    return ret;
}
`
}

function generate_wrapper_cat_4(function_info: function_info_cat_4): string {
    const arg_names = function_info.arguments.map(arg => arg[1])
    const args_without_out = function_info.arguments.slice(0, function_info.out_index).concat(function_info.arguments.slice(function_info.out_index + 1));

    const pretty_name = prettify_vk_name(function_info.name);

    const base_success_type = `std::remove_pointer_t<${function_info.arguments[function_info.out_index][0]}>`;
    const success_type = function_info.success_results.length == 1 ? base_success_type : `std::pair<${base_success_type}, VkResult>`;
    const return_expression = function_info.success_results.length == 1 ? "ret" : `${success_type}{ret, res}`;

    const arg_list_typeless = args_without_out.map(arg => arg[1]).join(", ");
    const arg_list_typed = generate_typed_argument_list(args_without_out);
    const vk_arg_list_typeless = [...arg_names.slice(0, function_info.out_index), "&ret", ...arg_names.slice(function_info.out_index + 1)].join(", ");

    const return_type = `std::expected<${success_type}, VkResult>`;

    return `auto ${pretty_name}() const noexcept { return [this](${arg_list_typed}) noexcept { this->${pretty_name}(${arg_list_typeless}); }; }
auto ${pretty_name}(${arg_list_typed}) const noexcept -> ${return_type} {
    TYPE_3_PRELUDE(${function_info.name}, ${function_info.out_index});
    const auto res = ${function_info.name}(${vk_arg_list_typeless});
    if (!success<${function_info.success_results.join(", ")}>(res)) {
        return std::unexpected{res};
    }
    return ${return_expression};
}
`
}

const generate_members = new Promise<void>(resolve => {
    let out = "";

    let first = true;
    for (const group of json_file_data.groups) {
        if (!first) {
            out += '\n';
        }
        first = false;

        out += `// ${group.name}\n\n`;

        if (group.requires !== undefined) {
            out += `#ifdef ${group.requires}\n`;
        }

        for (const fn_info of group.functions) {
            out += `PFN_${fn_info.name} ${fn_info.name} = nullptr;\n`;
        }

        if (group.requires !== undefined) {
            out += `#endif // ${group.requires}\n`;
        }
    }

    fs.writeFile(out_filename_members, out, () => {
        resolve();
    });
});

const generate_wrappers = new Promise<void>(resolve => {
    let out = `#ifndef VXL_VK_GENERATED_DONT_INCLUDE_IDE_HELPERS
#    include <stuff/core.hpp>
#    include <stuff/intro/introspectors/function.hpp>
#    include <vulkan/vulkan_core.h>
#    include <expected>

#define TYPE_3_PRELUDE(_name, _n)                                                \\
    using introspector_t = stf::intro::function_introspector<decltype(_name)>;   \\
    using return_type = std::remove_pointer_t<introspector_t::nth_argument<_n>>; \\
    auto ret = return_type {}

template<auto... SuccessValues>
static constexpr auto success(auto value) -> bool {
    return ((value == SuccessValues) || ...);
}

#endif

`;

    let first = true;
    for (const group of json_file_data.groups) {
        if (!first) {
            out += '\n';
        }
        first = false;

        out += `// ${group.name}\n`;

        if (group.requires !== undefined) {
            out += `#ifdef ${group.requires}\n`;
        }

        for (const fn_info of group.functions) {
            switch (fn_info.category) {
                case 0:
                    break;
                case 1:
                    out += generate_wrapper_cat_1(fn_info as function_info_cat_1) + '\n';
                    break;
                case 2:
                    out += generate_wrapper_cat_2(fn_info as function_info_cat_2) + '\n';
                    break;
                case 3:
                    out += generate_wrapper_cat_3(fn_info as function_info_cat_3) + '\n';
                    break;
                case 4:
                    out += generate_wrapper_cat_4(fn_info as function_info_cat_4) + '\n';
                    break;
                default:
                    break;
            }
        }

        if (group.requires !== undefined) {
            out += `#endif // ${group.requires}\n`;
        }
    }

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

            if (group.requires !== undefined) {
                out += `#ifdef ${group.requires}\n`;
            }

            for (const fn_info of group.functions) {
                const ok = (fn_info.stage_bits & bit) != 0;
                if (!ok) {
                    continue;
                }

                out += `  PTR_NAME_PAIR(${fn_info.name}),\n`;
            }

            if (group.requires !== undefined) {
                out += `#endif // ${group.requires}\n`;
            }
        }

        out += "};";

        fs.writeFile(filename, out, () => {
            resolve();
        });
    });
}

function generate_replacements(stage: number, filename: string): Promise<void> {
    return new Promise<void>(resolve => {
        let out = "";

        out += "static const std::pair<void (*vulkan_functions::*)(), void (*vulkan_functions::*)()> replacement_pairs[] {\n";

        let first = true;
        for (const group of json_file_data.groups) {
            if (!first) {
                out += '\n';
            }
            first = false;

            out += `  // ${group.name}\n`;

            if (group.requires !== undefined) {
                out += `#ifdef ${group.requires}\n`;
            }

            for (const replacement_info of group.replacements[stage]) {
                out += `  PTR_PTR_PAIR(${replacement_info[0]}, ${replacement_info[1]}),\n`;
            }

            if (group.requires !== undefined) {
                out += `#endif // ${group.requires}\n`;
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
    generate_replacements(0, out_filename_pre_instance_replacements),
    generate_replacements(1, out_filename_instance_replacements),
    generate_replacements(2, out_filename_device_replacements),
]).then()
