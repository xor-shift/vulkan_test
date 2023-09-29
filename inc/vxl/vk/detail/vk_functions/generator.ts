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

function generate_typed_argument_list(args: string[][]): string {
    return args.map(arg => {
        let ret = `${arg[0]} ${arg[1]}`;

        if (arg[2] !== null) {
            ret += ` = ${arg[2]}`;
        }

        return ret;
    }).join(", ");
}

function generate_typeless_argument_list(args: string[][]): string {
    return args.map(arg => arg[1]).join(", ");
}

/*
FUNCTION CATEGORIES:
0: wrapperless   : no wrapper is generated
1: simple void   : the function takes some in-arguments (with potential out-semantics), returns no VkResult
2: simple wrapped: the function takes some in-arguments (with potential out-semantics), returns a VkResult
3: outputting    : one of the function's arguments is an explicit out argument
 */

function generate_wrapper_cat_1(vk_name: string, args: string[][]): string {
    const pretty_name = prettify_vk_name(vk_name);

    const args_typed = generate_typed_argument_list(args);
    const args_typeless = generate_typeless_argument_list(args);

    return `auto ${pretty_name}() const noexcept { return [this](${args_typed}) noexcept { return ${pretty_name}(${args_typeless}); }; }
void ${pretty_name}(${args_typed}) const noexcept { return ${vk_name}(${args_typeless}); }
`;
}

function generate_wrapper_cat_2(vk_name: string, success_results: string[], args: string[][]): string {
    const pretty_name = prettify_vk_name(vk_name);

    const args_typed = generate_typed_argument_list(args);
    const args_typeless = args.map(arg => arg[1]).join(", ");

    const return_type = success_results.length == 1 ? "std::expected<void, VkResult>" : "std::expected<VkResult, VkResult>";


    return `auto ${pretty_name}() const noexcept { return [this](${args_typed}) noexcept { return this->${pretty_name}(${args_typeless}); }; }
auto ${pretty_name}(${args_typed}) const noexcept -> ${return_type} {
    const auto res = ${vk_name}(${args_typeless});
    if (!success<${success_results.join(", ")}>(res)) {
        return std::unexpected{res};
    }
    return res;
}
`;
}

function generate_wrapper_cat_3(vk_name: string, out_index: number, args: string[][]): string {
    const arg_names = args.map(arg => arg[1])
    const args_without_out = args.slice(0, out_index).concat(args.slice(out_index + 1));

    const pretty_name = prettify_vk_name(vk_name);

    const arg_list_typeless = args_without_out.map(arg => arg[1]).join(", ");
    const arg_list_typed = generate_typed_argument_list(args_without_out);
    const vk_arg_list_typeless = [...arg_names.slice(0, out_index), "&ret", ...arg_names.slice(out_index + 1)].join(", ");

    const return_type = `std::remove_pointer_t<${args[out_index][0]}>`;

    return `auto ${pretty_name}() const noexcept { return [this](${arg_list_typed}) noexcept { this->${pretty_name}(${arg_list_typeless}); }; }
auto ${pretty_name}(${arg_list_typed}) const noexcept -> ${return_type} {
    TYPE_3_PRELUDE(${vk_name}, ${out_index});
    ${vk_name}(${vk_arg_list_typeless});
    return ret;
}
`
}

function generate_wrapper_cat_4(vk_name: string, out_index: number, success_results: string[], args: string[][]): string {
    const arg_names = args.map(arg => arg[1])
    const args_without_out = args.slice(0, out_index).concat(args.slice(out_index + 1));

    const pretty_name = prettify_vk_name(vk_name);

    const base_success_type = `std::remove_pointer_t<${args[out_index][0]}>`;
    const success_type = success_results.length == 1 ? base_success_type : `std::pair<${base_success_type}, VkResult>`;

    const arg_list_typeless = args_without_out.map(arg => arg[1]).join(", ");
    const arg_list_typed = generate_typed_argument_list(args_without_out);
    const vk_arg_list_typeless = [...arg_names.slice(0, out_index), "&ret", ...arg_names.slice(out_index + 1)].join(", ");

    const return_type = `std::expected<${success_type}, VkResult>`;

    return `auto ${pretty_name}() const noexcept { return [this](${arg_list_typed}) noexcept { this->${pretty_name}(${arg_list_typeless}); }; }
auto ${pretty_name}(${arg_list_typed}) const noexcept -> ${return_type} {
    TYPE_3_PRELUDE(${vk_name}, ${out_index});
    const auto res = ${vk_name}(${vk_arg_list_typeless});
    if (!success<${success_results.join(", ")}>(res)) {
        return std::unexpected{res};
    }
    return ret;
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

        for (const fn_info of group.functions) {
            out += `PFN_${fn_info.name} ${fn_info.name} = nullptr;\n`;
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

        for (const fn_info of group.functions) {
            switch (fn_info.category) {
                case 0:
                    break;
                case 1:
                    out += generate_wrapper_cat_1(fn_info.name, fn_info.arguments) + '\n';
                    break;
                case 2:
                    out += generate_wrapper_cat_2(fn_info.name, fn_info.success_results, fn_info.arguments) + '\n';
                    break;
                case 3:
                    out += generate_wrapper_cat_3(fn_info.name, fn_info.out_index, fn_info.arguments) + '\n';
                    break;
                case 4:
                    out += generate_wrapper_cat_4(fn_info.name, fn_info.out_index, fn_info.success_results, fn_info.arguments) + '\n';
                    break;
                default:
                    break;
            }
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
